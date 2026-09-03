/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:
Version:		1.0
Date:			2016-03-05
Description:	出库功能
**************************************************/

//框架头文件
#include "stdafx.h"
//#include "smhs.h"

//程序用头文件
//#include "twma0.h"
//#include "twma1.h"
//#include "twma2.h"
//#include "twm01.h"



//函数申明
BM2_FUNCTION_IMPORT
int f_wmsmsm_stock_out(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);

//int f_cm_7z8t03_snd(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);
/*<remark>=========================================================
///<summary>
///板坯出库功能
///<para>
///2.排序方式：
///</para>
///<para>数据库表：TWMA0 倒躲队列；TWMA1 物料主档表
///<returns>执行预材料预入库功能</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wmsmsm12o_outstock1)
int f_wmsmsm12o_outstock1(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	int row_plan = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	int ii = 0;

	CDecimal rowCount = 0;
	CString aim_stock_no = "";
	CString out_stock_time = "";
	CString trnp_mode_code = "";
	CString truck_no = "";
	CString if_wt = "";
	CString dest_fin = "";
	CString aim_store = "";
	CString mat_line_type = "";
	CString remark0 = "";
	CString remark1 = "";
	CString store_keeper = "";
	CString crane_resp = "";
	CString crane_no = "";
	CString sg_sign = "";
	CString rel_remark = "";

	CString	update_data = "";
	CString	condition_data = "";
	CDecimal layerno1 = 0;

	/* 实体类定义 */
	//CTWMA0 twma0(conn);
	//CTWMA1 twma1(conn);
	//CTWMA2 twma2(conn);
	//CTWM01 twm01(conn);
	CModel twma0 = CModel("TWMA0");
	CModel twma1 = CModel("TMMSM01");
	CModel twma2 = CModel("TWMA2");
	CModel twm01 = CModel("TWM01");
	CModel twm0d = CModel("TWM0D");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);



	//调用仓库入库主函数
	EIClass bcls_stock_out;
	bcls_stock_out.Tables[0].set_TableName("WM_STOCK");
	bcls_stock_out.Tables[0].Columns.Add(twma0);
	bcls_stock_out.Tables[0].Columns.Add(twma2);
	bcls_stock_out.Tables[0].Columns.Add(DT_STRING, "REL_REMARK");
	bcls_stock_out.Tables[0].Rows.Clear();

	//调用物流发车主函数
	EIClass bcls_wl_car;
	bcls_wl_car.Tables[0].set_TableName("WL_CAR");
	bcls_wl_car.Tables["WL_CAR"].Columns.Add(DT_STRING, "DEAL_FLAG");
	bcls_wl_car.Tables["WL_CAR"].Columns.Add(DT_STRING, "BUSI_TYPE");
	bcls_wl_car.Tables["WL_CAR"].Columns.Add(DT_STRING, "TRUCK_NO");
	bcls_wl_car.Tables["WL_CAR"].Columns.Add(DT_STRING, "TRUCKBOARD_NO");
	bcls_wl_car.Tables["WL_CAR"].Columns.Add(DT_STRING, "LOAD_CODE_AREA");
	bcls_wl_car.Tables["WL_CAR"].Columns.Add(DT_STRING, "LOAD_NAME_AREA");
	bcls_wl_car.Tables["WL_CAR"].Columns.Add(DT_STRING, "LOAD_CODE");
	bcls_wl_car.Tables["WL_CAR"].Columns.Add(DT_STRING, "LOAD_NAME");
	bcls_wl_car.Tables["WL_CAR"].Columns.Add(DT_STRING, "EMP_CODE");
	bcls_wl_car.Tables["WL_CAR"].Columns.Add(DT_STRING, "EMP_NAME");
	bcls_wl_car.Tables["WL_CAR"].Columns.Add(DT_STRING, "PLAN_NO");
	bcls_wl_car.Tables["WL_CAR"].Columns.Add(DT_STRING, "ATTENTN_NO");
	bcls_wl_car.Tables["WL_CAR"].Columns.Add(DT_STRING, "MAT_NO");
	bcls_wl_car.Tables["WL_CAR"].Columns.Add(DT_STRING, "SG_SIGN");
	bcls_wl_car.Tables["WL_CAR"].Columns.Add(DT_DECIMAL, "LENGTH");
	bcls_wl_car.Tables["WL_CAR"].Columns.Add(DT_DECIMAL, "WIDTH");
	bcls_wl_car.Tables["WL_CAR"].Columns.Add(DT_DECIMAL, "THICK");
	bcls_wl_car.Tables["WL_CAR"].Columns.Add(DT_DECIMAL, "WEIGHT");

	bcls_wl_car.Tables[0].Rows.Clear();


	//入库队列
	EIClass bcls_stock_que;
	bcls_stock_que.Tables.Add("WM00QUE");
	bcls_stock_que.Tables["WM00QUE"].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");
	bcls_stock_que.Tables["WM00QUE"].Columns.Add(DT_STRING, "STOCK_OPER_ORDER_DIV");
	bcls_stock_que.Tables["WM00QUE"].Columns.Add(DT_STRING, "MAT_NO");
	bcls_stock_que.Tables["WM00QUE"].Columns.Add(DT_STRING, "MAT_NUM");
	bcls_stock_que.Tables["WM00QUE"].Columns.Add(DT_STRING, "PLAN_NO");
	bcls_stock_que.Tables["WM00QUE"].Columns.Add(DT_STRING, "PLAN_EXEC_SEQ_NO");
	bcls_stock_que.Tables["WM00QUE"].Columns.Add(DT_STRING, "STOCK_NO");
	bcls_stock_que.Tables["WM00QUE"].Columns.Add(DT_STRING, "TRANS_TOOL");
	bcls_stock_que.Tables["WM00QUE"].Columns.Add(DT_STRING, "PRE_UNIT_CODE");
	bcls_stock_que.Tables["WM00QUE"].Columns.Add(DT_STRING, "NEXT_UNIT_CODE");
	bcls_stock_que.Tables["WM00QUE"].Columns.Add(DT_STRING, "MAT_DESTION");
	bcls_stock_que.Tables["WM00QUE"].Columns.Add(DT_STRING, "OPER_FLAG");
	bcls_stock_que.Tables["WM00QUE"].Rows.Clear();

	try
	{


		if (bcls_rec->Tables[0].Columns.Contains("OUT_STOCK_TIME")){
			out_stock_time = bcls_rec->Tables[0].Rows[0]["OUT_STOCK_TIME"].ToString().Trim();
		}
		if (out_stock_time == ""){
			out_stock_time = datetime;
		}
		trnp_mode_code = bcls_rec->Tables[0].Rows[0]["TRNP_MODE_CODE"].ToString().Trim();
		truck_no = bcls_rec->Tables[0].Rows[0]["TRUCK_NO"].ToString().Trim();

		if (trnp_mode_code == "1")
		{
			if (truck_no == "")
			{
				sprintf(s.msg, "运输方式为卡车，卡车号不能为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		if (bcls_rec->Tables[0].Columns.Contains("REMARK0"))
		{
			remark0 = bcls_rec->Tables[0].Rows[0]["REMARK0"].ToString().Trim();//20160706 GONGLEI 驾驶员
		}
		if (bcls_rec->Tables[0].Columns.Contains("REMARK1"))
		{
			remark1 = bcls_rec->Tables[0].Rows[0]["REMARK1"].ToString().Trim();//20160706 GONGLEI 手机号
		}
		if (bcls_rec->Tables[0].Columns.Contains("STORE_KEEPER"))
		{
			store_keeper = bcls_rec->Tables[0].Rows[0]["STORE_KEEPER"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("CRANE_RESP"))
		{
			crane_resp = bcls_rec->Tables[0].Rows[0]["CRANE_RESP"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("CRANE_NO"))
		{
			crane_no = bcls_rec->Tables[0].Rows[0]["CRANE_NO"].ToString().Trim();
		}


		Log::Trace("", __FUNCTION__, "传入参数 out_stock_time：\t[{0}]", out_stock_time);
		Log::Trace("", __FUNCTION__, "传入参数 trnp_mode_code：\t[{0}]", trnp_mode_code);
		Log::Trace("", __FUNCTION__, "传入参数 truck_no：\t[{0}]", truck_no);
		Log::Trace("", __FUNCTION__, "传入参数 remark0：\t[{0}]", remark0);
		Log::Trace("", __FUNCTION__, "传入参数 remark1：\t[{0}]", remark1);


		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{

			if (bcls_rec->Tables[0].Columns.Contains("TO_STOCK_NO") && bcls_rec->Tables[0].Rows[i]["TO_STOCK_NO"].ToString().Trim() == "")
			{
				aim_stock_no = bcls_rec->Tables[0].Rows[0]["AIM_STOCK_NO"].ToString().Trim();
			}
			else
			{
				aim_stock_no = bcls_rec->Tables[0].Rows[i]["TO_STOCK_NO"].ToString().Trim();
			}

			twma0.Reset();
			twma1.Reset();
			twma0.MergeFrom(bcls_rec->Tables[0].Rows[i]);


			Log::Trace("", __FUNCTION__, " twma1.MAT_NO =[{0}]", twma0["MAT_NO"].ToString());
			Log::Trace("", __FUNCTION__, " twma1.MAT_KIND =[{0}]", twma0["MAT_KIND"].ToString());
			Log::Trace("", __FUNCTION__, "传入参数 aim_stock_no：\t[{0}]", aim_stock_no);

			if (twma0["MAT_NO"].ToString().Trim() == "")
			{
				sprintf(s.msg, "材料号不能为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}




			//查询材料主档、库位跟踪表
			twma1["MAT_NO"] = twma0["MAT_NO"];
			if (!twma1.Query("MAT_NO"))
			{
				sprintf(s.msg, "物料档该材料不存在");
				throw CApplicationException(-1, s.msg, log.Location);
			}


			twma0["STOCK_OPER_ORDER"] = "2O";

			if (twma1["STOCK_NO"].ToString().Trim() == aim_stock_no)
			{
				sprintf(s.msg, "材料转库起始库区[" + twma1["STOCK_NO"].ToString() + "]与目标库区[" + aim_stock_no + "]一致,不能出库.");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			//方坯
			if (twma1["STOCK_NO"].ToString().Trim().Substring(0, 1) == "B"){

				rel_remark = bcls_rec->Tables[0].Rows[0]["REL_REMARK"].ToString().Trim();

				if (rel_remark == "")
				{
					sprintf(s.msg, "小票编号不能为空");
					throw CApplicationException(-1, s.msg, log.Location);
				}

				//调用仓库出库主函数
				bcls_stock_out.Tables["WM_STOCK"].Rows.Add();
				bcls_stock_out.Tables["WM_STOCK"].Rows[i]["MAT_NO"] = twma0["MAT_NO"];
				bcls_stock_out.Tables["WM_STOCK"].Rows[i]["STOCK_OPER_ORDER"] = twma0["STOCK_OPER_ORDER"];
				bcls_stock_out.Tables["WM_STOCK"].Rows[i]["STOCK_NO"] = aim_stock_no;
				bcls_stock_out.Tables["WM_STOCK"].Rows[i]["STOCK_PLACE_NO"] = " ";
				bcls_stock_out.Tables["WM_STOCK"].Rows[i]["ROWNO"] = " ";
				bcls_stock_out.Tables["WM_STOCK"].Rows[i]["COLUMN_NO"] = " ";
				bcls_stock_out.Tables["WM_STOCK"].Rows[i]["LAYERNO"] = 0;
				bcls_stock_out.Tables["WM_STOCK"].Rows[i]["STOCK_PLACE_POSITION"] = " ";
				bcls_stock_out.Tables["WM_STOCK"].Rows[i]["VEHICLE_NO"] = truck_no;
				bcls_stock_out.Tables["WM_STOCK"].Rows[i]["REL_REMARK"] = rel_remark;

				if (truck_no != "")
				{
					if (i == 0)
					{
						switch (conn->DatabaseKind)
						{
						case DB_KIND_DB2:           // DB2 数据库（未开Oracle兼容）
						case DB_KIND_DB2_ORACLE:    // DB2 数据库（开Oracle兼容）
						case DB_KIND_MSSQL:	        // MS SQL Server数据库
						case DB_KIND_ORACLE:        // Oracle 数据库
						default:
							sqlstr = CString(
								" SELECT LOADING_PLAN_NO,STOCK_NO,AIM_STOCK_NO FROM twm0d WHERE stock_no =@twm0d.STOCK_NO and EXPIRY_DATE >=@twm0d.DATE_TIME AND PRO_FLAG ='I' and truck_no =@twm0d.TRUCK_NO"
								);
							break;
						}
						Log::Debug("", __FUNCTION__, "STOCK_NO	= [{0}]", twma1["STOCK_NO"].ToString());
						Log::Debug("", __FUNCTION__, "TRUCK_NO	= [{0}]", truck_no);
						cmd_inq.SetCommandText(sqlstr);
						cmd_inq.Parameters.Set("twm0d.STOCK_NO", twma1["STOCK_NO"].ToString());
						cmd_inq.Parameters.Set("twm0d.DATE_TIME", datetime.SubstringNE(0, 8));
						cmd_inq.Parameters.Set("twm0d.TRUCK_NO", truck_no);
						cmd_inq.ExecuteReader();
						if (cmd_inq.Read())
						{
							twm0d["LOADING_PLAN_NO"] = cmd_inq.GetString(1);
							twm0d["STOCK_NO"] = cmd_inq.GetString(2);
							twm0d["AIM_STOCK_NO"] = cmd_inq.GetString(3);
						}
						cmd_inq.Close();

						if (twma1["STOCK_NO"].ToString().Trim() != twm0d["STOCK_NO"].ToString().Trim())
						{
							sprintf(s.msg, "材料转库起始库区[" + twma1["STOCK_NO"].ToString() + "]与物流发车起始库区[" + twm0d["STOCK_NO"].ToString() + "]不一致.");
							throw CApplicationException(-1, s.msg, s.svc_name);
						}

						if (aim_stock_no != twm0d["AIM_STOCK_NO"].ToString())
						{
							sprintf(s.msg, "材料转库目标库区[" + aim_stock_no + "]与物流发车目标库区[" + twm0d["AIM_STOCK_NO"].ToString() + "]不一致.");
							throw CApplicationException(-1, s.msg, s.svc_name);
						}



					}

					twma1["JL_PLAN_NO"] = twm0d["LOADING_PLAN_NO"];
					twma1["VEHICLE_NO"] = truck_no;
					twma1["REL_REMARK"] = rel_remark;
					twma1.Update("JL_PLAN_NO,VEHICLE_NO,REL_REMARK", "MAT_NO");

					Log::Trace("", __FUNCTION__, " REL_REMARK =[{0}]", twma1["REL_REMARK"].ToString());

					twm01["STOCK_NO"] = twma1["STOCK_NO"];
					if (!twm01.Query("STOCK_NO"))
					{
						sprintf(s.msg, "库区不存在");
						throw CApplicationException(-1, s.msg, log.Location);
					}

					//调用物流发车主函数
					bcls_wl_car.Tables["WL_CAR"].Rows.Add();
					bcls_wl_car.Tables["WL_CAR"].Rows[i]["DEAL_FLAG"] = "I";
					bcls_wl_car.Tables["WL_CAR"].Rows[i]["BUSI_TYPE"] = "1";
					bcls_wl_car.Tables["WL_CAR"].Rows[i]["TRUCK_NO"] = truck_no;
					bcls_wl_car.Tables["WL_CAR"].Rows[i]["TRUCKBOARD_NO"] = " ";
					bcls_wl_car.Tables["WL_CAR"].Rows[i]["LOAD_CODE_AREA"] = twma1["STOCK_NO"];
					bcls_wl_car.Tables["WL_CAR"].Rows[i]["LOAD_NAME_AREA"] = twm01["STOCK_DESC"];
					bcls_wl_car.Tables["WL_CAR"].Rows[i]["LOAD_CODE"] = " ";
					bcls_wl_car.Tables["WL_CAR"].Rows[i]["LOAD_NAME"] = " ";
					bcls_wl_car.Tables["WL_CAR"].Rows[i]["EMP_CODE"] = s.userid;
					bcls_wl_car.Tables["WL_CAR"].Rows[i]["EMP_NAME"] = " ";
					bcls_wl_car.Tables["WL_CAR"].Rows[i]["PLAN_NO"] = twm0d["LOADING_PLAN_NO"];
					bcls_wl_car.Tables["WL_CAR"].Rows[i]["ATTENTN_NO"] = datetime;
					bcls_wl_car.Tables["WL_CAR"].Rows[i]["SG_SIGN"] = twma1["SG_SIGN"];
					bcls_wl_car.Tables["WL_CAR"].Rows[i]["MAT_NO"] = twma0["MAT_NO"];
					bcls_wl_car.Tables["WL_CAR"].Rows[i]["LENGTH"] = twma1["MAT_ACT_LEN"];
					bcls_wl_car.Tables["WL_CAR"].Rows[i]["WIDTH"] = twma1["MAT_ACT_WIDTH"];
					bcls_wl_car.Tables["WL_CAR"].Rows[i]["THICK"] = twma1["MAT_ACT_THICK"];
					bcls_wl_car.Tables["WL_CAR"].Rows[i]["WEIGHT"] = twma1["MAT_ACT_WT"];
				}
			}
			//板坯
			else{
				//调用仓库出库主函数
				bcls_stock_out.Tables["WM_STOCK"].Rows.Add();
				bcls_stock_out.Tables["WM_STOCK"].Rows[i]["MAT_NO"] = twma0["MAT_NO"];
				bcls_stock_out.Tables["WM_STOCK"].Rows[i]["STOCK_OPER_ORDER"] = twma0["STOCK_OPER_ORDER"];
				bcls_stock_out.Tables["WM_STOCK"].Rows[i]["STOCK_NO"] = aim_stock_no;
				bcls_stock_out.Tables["WM_STOCK"].Rows[i]["STOCK_PLACE_NO"] = " ";
				bcls_stock_out.Tables["WM_STOCK"].Rows[i]["ROWNO"] = " ";
				bcls_stock_out.Tables["WM_STOCK"].Rows[i]["COLUMN_NO"] = " ";
				bcls_stock_out.Tables["WM_STOCK"].Rows[i]["LAYERNO"] = 0;
				bcls_stock_out.Tables["WM_STOCK"].Rows[i]["STOCK_PLACE_POSITION"] = " ";
				bcls_stock_out.Tables["WM_STOCK"].Rows[i]["VEHICLE_NO"] = truck_no;
				if (truck_no != "")
				{
					if (i == 0)
					{
						switch (conn->DatabaseKind)
						{
						case DB_KIND_DB2:           // DB2 数据库（未开Oracle兼容）
						case DB_KIND_DB2_ORACLE:    // DB2 数据库（开Oracle兼容）
						case DB_KIND_MSSQL:	        // MS SQL Server数据库
						case DB_KIND_ORACLE:        // Oracle 数据库
						default:
							sqlstr = CString(
								" SELECT LOADING_PLAN_NO,STOCK_NO,AIM_STOCK_NO FROM twm0d WHERE stock_no =@twm0d.STOCK_NO and EXPIRY_DATE >=@twm0d.DATE_TIME AND PRO_FLAG ='I' and truck_no =@twm0d.TRUCK_NO"
								);
							break;
						}
						Log::Debug("", __FUNCTION__, "STOCK_NO	= [{0}]", twma1["STOCK_NO"].ToString());
						Log::Debug("", __FUNCTION__, "TRUCK_NO	= [{0}]", truck_no);
						cmd_inq.SetCommandText(sqlstr);
						cmd_inq.Parameters.Set("twm0d.STOCK_NO", twma1["STOCK_NO"].ToString());
						cmd_inq.Parameters.Set("twm0d.DATE_TIME", datetime.SubstringNE(0, 8));
						cmd_inq.Parameters.Set("twm0d.TRUCK_NO", truck_no);
						cmd_inq.ExecuteReader();
						if (cmd_inq.Read())
						{
							twm0d["LOADING_PLAN_NO"] = cmd_inq.GetString(1);
							twm0d["STOCK_NO"] = cmd_inq.GetString(2);
							twm0d["AIM_STOCK_NO"] = cmd_inq.GetString(3);
						}
						cmd_inq.Close();

						if (twma1["STOCK_NO"].ToString().Trim() != twm0d["STOCK_NO"].ToString().Trim())
						{
							sprintf(s.msg, "材料转库起始库区[" + twma1["STOCK_NO"].ToString() + "]与物流发车起始库区[" + twm0d["STOCK_NO"].ToString() + "]不一致.");
							throw CApplicationException(-1, s.msg, s.svc_name);
						}

						if (aim_stock_no != twm0d["AIM_STOCK_NO"].ToString())
						{
							sprintf(s.msg, "材料转库目标库区[" + aim_stock_no + "]与物流发车目标库区[" + twm0d["AIM_STOCK_NO"].ToString() + "]不一致.");
							throw CApplicationException(-1, s.msg, s.svc_name);
						}



					}

					twma1["JL_PLAN_NO"] = twm0d["LOADING_PLAN_NO"];
					twma1["VEHICLE_NO"] = truck_no;
					twma1.Update("JL_PLAN_NO,VEHICLE_NO", "MAT_NO");

					twm01["STOCK_NO"] = twma1["STOCK_NO"];
					if (!twm01.Query("STOCK_NO"))
					{
						sprintf(s.msg, "库区不存在");
						throw CApplicationException(-1, s.msg, log.Location);
					}

					////获取流水号
					//char id[6];//序列号
					//if (0 > EPGetNextSeq("WM_ATTENTN_NO", id)){
					//	sprintf(s.msg, "系统出现异常，请联系管理人员!");
					//	throw CApplicationException(-1, s.msg, log.Location);
					//}

					//CString attentn_no = datetime + id;

					//调用物流发车主函数
					bcls_wl_car.Tables["WL_CAR"].Rows.Add();
					bcls_wl_car.Tables["WL_CAR"].Rows[i]["DEAL_FLAG"] = "I";
					bcls_wl_car.Tables["WL_CAR"].Rows[i]["BUSI_TYPE"] = "1";
					bcls_wl_car.Tables["WL_CAR"].Rows[i]["TRUCK_NO"] = truck_no;
					bcls_wl_car.Tables["WL_CAR"].Rows[i]["TRUCKBOARD_NO"] = " ";
					bcls_wl_car.Tables["WL_CAR"].Rows[i]["LOAD_CODE_AREA"] = twma1["STOCK_NO"];
					bcls_wl_car.Tables["WL_CAR"].Rows[i]["LOAD_NAME_AREA"] = twm01["STOCK_DESC"];
					bcls_wl_car.Tables["WL_CAR"].Rows[i]["LOAD_CODE"] = " ";
					bcls_wl_car.Tables["WL_CAR"].Rows[i]["LOAD_NAME"] = " ";
					bcls_wl_car.Tables["WL_CAR"].Rows[i]["EMP_CODE"] = s.userid;
					bcls_wl_car.Tables["WL_CAR"].Rows[i]["EMP_NAME"] = " ";
					bcls_wl_car.Tables["WL_CAR"].Rows[i]["PLAN_NO"] = twm0d["LOADING_PLAN_NO"];
					bcls_wl_car.Tables["WL_CAR"].Rows[i]["ATTENTN_NO"] = datetime;
					//bcls_wl_car.Tables["WL_CAR"].Rows[i]["ATTENTN_NO"] = attentn_no;
					bcls_wl_car.Tables["WL_CAR"].Rows[i]["SG_SIGN"] = twma1["SG_SIGN"];
					bcls_wl_car.Tables["WL_CAR"].Rows[i]["MAT_NO"] = twma0["MAT_NO"];
					bcls_wl_car.Tables["WL_CAR"].Rows[i]["LENGTH"] = twma1["MAT_ACT_LEN"];
					bcls_wl_car.Tables["WL_CAR"].Rows[i]["WIDTH"] = twma1["MAT_ACT_WIDTH"];
					bcls_wl_car.Tables["WL_CAR"].Rows[i]["THICK"] = twma1["MAT_ACT_THICK"];
					bcls_wl_car.Tables["WL_CAR"].Rows[i]["WEIGHT"] = twma1["MAT_ACT_WT"];
				}
			}
		}


		doFlag = f_wmsmsm_stock_out(&bcls_stock_out, bcls_ret, conn);
		if (doFlag != 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
		Log::Trace("", __FUNCTION__, "传入参数111111 truck_no：\t[{0}]", truck_no);

		if (truck_no != "")
		{
			/*doFlag = f_cm_7z8t03_snd(&bcls_wl_car, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}*/
		}
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		Log::Trace("", __FUNCTION__, "s.flag[{0}]", s.flag);
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;

}

