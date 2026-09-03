/* **************************************************************************
*	Copyright (c) Baosight Corporation 2008 . All Rights Reserved.
*  	BM2PES 宝信生产执行系统
*****************************************************************************
*  程序名称			: f_wm00_stock_back
*  程序描述			: 来料退料入库主函数
*  功能说明			:更新卸车实绩，删除入库队列，
* 电文说明          ：有调拨单反馈调拨单，反馈卸车实绩
*  修改历史			:
*  		henno 2016-09-28			(ADD)程序建立
*			... ...
* **************************************************************************** */
/* C/C++ 的标准头文件部分 */
#include "stdafx.h"		// 框架头，不可删除 
//#include "twma0.h" 
//#include "twma1.h"
//#include "twma2.h"
//#include "twma4.h"




BM2_FUNCTION_IMPORT
int f_wmsm_21a010_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//卸车确认

int f_wmsmsm_allot_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//调拨反馈






BM2_FUNCTION_EXPORT
int f_wmsmsm_stock_back(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	/* ***** 程序变量 ***** */
	int doFlag = 0;
	CString v_datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString v_mat_no = " ";
	CString v_stock_oper_order = " ";
	CString v_stock_no = " ";
	CString v_stock_place_no = " ";
	CString v_rowno = " ";
	CString v_column_no = "";
	CDecimal v_layerno = 0;
	CString v_stock_place_position = " ";
	CString v_crane_no = " ";
	CString v_vehicle_no = " ";
	CString v_stock_oper_order_div = "";
	CString upd_name = "";

	CString v_shift_no = " ";
	CString v_shift_group = " ";

	/* ***** 数据库SQL操作字符串 ***** */
	CString	sqlstr = " ";

	/* ***** 数据库操作类定义 ***** */
	CDbCommand comm(conn);
	CDbCommand comm1(conn);
	CDbCommand cmd_inq(conn);

	/* ***** 定义表实体对象 ***** */
	CModel twma0 = CModel("TWMA0");
	CModel twma1 = CModel("TMMSM01");
	CModel twma2 = CModel("TWMA2");
	CModel twma2_old = CModel("TWMA2");
	CModel twm41dj = CModel("TWM41DJ");
	CModel twmsm61("TWMSM61");
	CModel twmsm62("TWMSM62");
	CModel twmsm12("TWMSM12");
	CModel hwmsm12("HWMSM12");



	//发卸车确认电文
	EIClass bcls_xcqr;
	bcls_xcqr.Tables[0].set_TableName("ZCHO");
	bcls_xcqr.Tables[0].Columns.Add(twmsm62);
	bcls_xcqr.Tables[0].Rows.Clear();

	//发调拨反馈
	EIClass bcls_all;
	bcls_xcqr.Tables[0].Columns.Add(twm41dj);
	bcls_xcqr.Tables[0].Rows.Clear();



	/* ***** 应用程序开始处理 ***** */
	try
	{
		if (!bcls_rec->Tables.Contains("WM_STOCK"))
		{
			sprintf(s.msg, "函数f_wm00_stock_in中找不到接收块名[WM_STOCK]");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		for (int iRow = 0; iRow < bcls_rec->Tables["WM_STOCK"].Rows.get_Count(); iRow++)
		{
			//获取传入参数
			twma0.Reset();
			twma0.MergeFrom(bcls_rec->Tables["WM_STOCK"].Rows[iRow]);
			twma0.TrimOrBlank();

			v_mat_no = twma0["MAT_NO"];
			v_stock_oper_order = twma0["STOCK_OPER_ORDER"];
			v_stock_oper_order_div = twma0["STOCK_OPER_ORDER_DIV"];
			v_stock_no = twma0["STOCK_NO"];
			v_stock_place_no = twma0["STOCK_PLACE_NO"];
			v_layerno = twma0["LAYERNO"];
			v_stock_place_position = twma0["STOCK_PLACE_POSITION"];
			v_crane_no = twma0["CRANE_NO"];
			v_vehicle_no = twma0["VEHICLE_NO"];

			

			if (v_stock_oper_order.Trim() == "")
			{
				sprintf(s.msg, "库操作指示不能为空.");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (v_stock_oper_order[0] != '1')
			{
				sprintf(s.msg, "库操作指示应为入库类事件.");
				continue;
			}
			if (v_mat_no.Trim() == "")
			{
				sprintf(s.msg, "材料号不能为空.");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (v_stock_no.Trim() == "")
			{
				sprintf(s.msg, "库区不能为空.");
				throw CApplicationException(-1, s.msg, log.Location);
			}


			if (v_stock_place_no.Trim() == "")
			{
				sprintf(s.msg, "库位不能为空.");
				throw CApplicationException(-1, s.msg, log.Location);
			}



			//1. 检查A0（非必需）
			sqlstr =
				" SELECT * FROM TWMA0"
				" WHERE MAT_NO = @mat_no"
				" AND STOCK_OPER_ORDER LIKE @stock_oper_order";
			comm.SetCommandText(sqlstr);
			comm.Parameters.Set("mat_no", twma0["MAT_NO"].ToString());
			comm.Parameters.Set("stock_oper_order", twma0["STOCK_OPER_ORDER"].ToString().Substring(0, 1) + "%");
			comm.ExecuteReader();
			if (comm.Read())
			{
				comm.Fetch(twma0);
			}
			comm.Close();
			
			//2. 删A0（按材料号、类型）
			twma0["MAT_NO"] = v_mat_no;
			//twma0.STOCK_OPER_ORDER = v_stock_oper_order;
			twma0.Delete("MAT_NO, STOCK_OPER_ORDER");

			if (bcls_rec->Tables["WM_STOCK"].Rows[0]["IF_LOGI"].ToString() == "1")
			{
				twmsm62.MergeFrom(bcls_rec->Tables["WM_STOCK"].Rows[iRow]);
				twmsm62["UNLOAD_FLAG"] = "1";//卸车标记
				twmsm62["AFFIRM_FLAG"] = "9";//确认标记
				twmsm62["UNLOAD_END_TIME"] = v_datetime;
				if (twmsm62["UNLOAD_EMP_CODE"].ToString().Trim() == "")
				{
					twmsm62["UNLOAD_EMP_CODE"] = s.userid;
				}
				if (twmsm62["UNLOAD_EMP_NAME"].ToString().Trim() == "")
				{
					twmsm62["UNLOAD_EMP_NAME"] = s.username;
				}
				f_epep_get_shift_group("SMCP", v_datetime, v_shift_no, v_shift_group, conn);
				twmsm62["SHIFT_NO"] = v_shift_no;
				twmsm62["SHIFT_GROUP"] = v_shift_group;
				twmsm62.Update("UNLOAD_FLAG,AFFIRM_FLAG,UNLOAD_EMP_CODE,UNLOAD_EMP_NAME,BACK1,UNLOAD_END_TIME,SHIFT_NO,SHIFT_GROUP", "PRACTICE_NO,MAT_NO");


				twmsm62.Query("PRACTICE_NO,MAT_NO");
				if (twmsm62["UNLOAD_CODE_FACTORY"].ToString() == "6240")
				{
					twmsm12["MAT_NO"] = twmsm62["MAT_NO"].ToString();
					twmsm61["MAT_NO"] = twmsm62["MAT_NO"].ToString();
					if (twmsm12.Query("MAT_NO"))
					{
						hwmsm12.CopyFrom(twmsm12);
						hwmsm12.Insert();
						twmsm12.Delete("MAT_NO");
					}
					if (twmsm61.QueryCount("MAT_NO") > 0)
					{
						twmsm61["UNLOAD_STATE"] = "3";
						twmsm61.Update("UNLOAD_STATE", "MAT_NO");
					}
					twma1["MAT_NO"]= twmsm62["MAT_NO"].ToString();
					twma1["LOGISTICS_STATUS"] ="0";
					twma1["PRE_LOAD_FLAG"] = "0";
					twma1.Update("LOGISTICS_STATUS,PRE_LOAD_FLAG", "MAT_NO");
				}
				else
				{
					
				}
				twmsm62.MergeTo(bcls_xcqr.Tables[0], false);

				if (bcls_rec->Tables["WM_STOCK"].Rows[iRow]["C_DELIVERYID"].ToString().Trim() == "")
				{
					sqlstr = " select * from TWM41DJ where C_BATCHUNIT='" + twma0["MAT_NO"].ToString() + "' and C_STATESIGN='1' ";
					Log::Trace("", __FUNCTION__, "查询到有未显示的调拨单号[{0}]", sqlstr);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						twm41dj.Reset();
						cmd_inq.Fetch(twm41dj);
						f_epep_get_shift_group("SMCP", v_datetime, v_shift_no, v_shift_group, conn);
						twm41dj["C_GROUP"] = v_shift_group;
						twm41dj["C_QULITYTYPE"] = bcls_rec->Tables["WM_STOCK"].Rows[iRow]["BACK1"].ToString();
						upd_name = "";

						twm41dj["C_STATESIGN"] = "2";
						upd_name += "C_STATESIGN,C_GROUP,C_QULITYTYPE";
						twm41dj["T_INSTOCKTIME"] = v_datetime;
						upd_name += ",T_INSTOCKTIME";
						if (twm41dj["C_SENDDEPT"].ToString() == "6360")//2250
						{
							twm41dj["T_ACCEPTTIME"] = v_datetime;
							upd_name += ",T_ACCEPTTIME";
							twm41dj["C_CLOSEGATETIME"] = v_datetime;
							upd_name += ",C_CLOSEGATETIME";
							twm41dj["T_UPLOADTIME"] = v_datetime;
							upd_name += ",T_UPLOADTIME";

							twm41dj["T_SALESCOMFIRMTIME"] = v_datetime;
							upd_name += ",T_SALESCOMFIRMTIME";
							twm41dj["T_OVERRULETIME"] = v_datetime;
							upd_name += ",T_OVERRULETIME";
							twm41dj["D_REQUIREDATE"] = v_datetime;
							upd_name += ",D_REQUIREDATE";
							twm41dj["D_BILLDATE"] = v_datetime;
							upd_name += ",D_BILLDATE";
							twm41dj["T_OUTSTOCKTIME"] = v_datetime;
							upd_name += ",T_OUTSTOCKTIME";
						}
						twm41dj.Update(upd_name, "C_DELIVERYID");
						twm41dj.MergeTo(bcls_all.Tables[0], false);
					}
					cmd_inq.Close();
				}
			}
		


			twm41dj.Reset();
			twm41dj.MergeFrom(bcls_rec->Tables["WM_STOCK"].Rows[iRow]);
			if (twm41dj["C_DELIVERYID"].ToString().Trim() != "")
			{
				twm41dj.Query("C_DELIVERYID");
				f_epep_get_shift_group("SMCP", v_datetime, v_shift_no, v_shift_group, conn);
				twm41dj["C_GROUP"] = v_shift_group;
				twm41dj["C_QULITYTYPE"] = bcls_rec->Tables["WM_STOCK"].Rows[iRow]["BACK1"].ToString();
				upd_name = "";
				
				twm41dj["C_STATESIGN"] = "2";
				upd_name += "C_STATESIGN,C_GROUP,C_QULITYTYPE";
				twm41dj["T_INSTOCKTIME"] = v_datetime;
				upd_name += ",T_INSTOCKTIME";
				if (twm41dj["C_SENDDEPT"].ToString() == "6360")//2250
				{
					twm41dj["T_ACCEPTTIME"] = v_datetime;
					upd_name += ",T_ACCEPTTIME";
					twm41dj["C_CLOSEGATETIME"] = v_datetime;
					upd_name += ",C_CLOSEGATETIME";
					twm41dj["T_UPLOADTIME"] = v_datetime;
					upd_name += ",T_UPLOADTIME";
					
					twm41dj["T_SALESCOMFIRMTIME"] = v_datetime;
					upd_name += ",T_SALESCOMFIRMTIME";
					twm41dj["T_OVERRULETIME"] = v_datetime;
					upd_name += ",T_OVERRULETIME";
					twm41dj["D_REQUIREDATE"] = v_datetime;
					upd_name += ",D_REQUIREDATE";
					twm41dj["D_BILLDATE"] = v_datetime;
					upd_name += ",D_BILLDATE";
					twm41dj["T_OUTSTOCKTIME"] = v_datetime;
					upd_name += ",T_OUTSTOCKTIME";
				}
				twm41dj.Update(upd_name, "C_DELIVERYID");
				Log::Trace("", __FUNCTION__, "rowCount[{0}]rowCount[{1}]", upd_name, twm41dj["C_QULITYTYPE"].ToString(), bcls_rec->Tables["WM_STOCK"].Rows[iRow]["BACK1"].ToString());
				twm41dj.MergeTo(bcls_all.Tables[0], false);
			}
			

			


		}



		



		

		

		//发送卸车确认电文
		if (bcls_xcqr.Tables[0].Rows.get_Count() > 0) {
			doFlag = f_wmsm_21a010_snd(&bcls_xcqr, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		//发送调拨反馈
		if (bcls_all.Tables[0].Rows.get_Count() > 0) {
			doFlag = f_wmsmsm_allot_snd(&bcls_all, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };

		/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006"), arguments, 1);
		CString str = ex.GetMsg() + "\r\n" + sqlstr;

		/*返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应*/
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);

		/*数据库异常时返回-1，事务将被回滚*/
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), 399);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), 399);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}
