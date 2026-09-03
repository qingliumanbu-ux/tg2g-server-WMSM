/* **************************************************************************
*	Copyright (c) Baosight Corporation 2008 . All Rights Reserved.
*  	BM2PES 宝信生产执行系统
*****************************************************************************
*  程序名称			: f_wm00_stock_in
*  程序描述			: 仓库入库主函数
*  备注说明			:
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
int f_wmsmsm_stock_update(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);

BM2_FUNCTION_IMPORT
int f_wmsmsm_mm0099(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);

BM2_FUNCTION_IMPORT
int f_wmsmsm_stock_log(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);

BM2_FUNCTION_IMPORT





BM2_FUNCTION_EXPORT
int f_wmsmsm_stock_in(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	/* ***** 程序变量 ***** */
	int doFlag = 0;
	int blkNum = 0;
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

	/* ***** 数据库SQL操作字符串 ***** */
	CString	sqlstr = " ";

	/* ***** 数据库操作类定义 ***** */
	CDbCommand comm(conn);
	CDbCommand comm1(conn);

	/* ***** 定义表实体对象 ***** */
	CModel twma0 = CModel("TWMA0");
	CModel twma1 = CModel("TMMSM01");
	CModel twma2 = CModel("TWMA2");
	CModel twma2_old = CModel("TWMA2");
	CModel twma4 = CModel("TWMA4");
	CModel twmsm62("TWMSM62");


	//调用物料函数
	EIClass bcls_rec_wmmm99;
	bcls_rec_wmmm99.Tables[0].set_TableName("WMMM99");
	bcls_rec_wmmm99.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
	bcls_rec_wmmm99.Tables[0].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");
	bcls_rec_wmmm99.Tables[0].Columns.Add(DT_STRING, "STOCK_OPER_ORDER_DIV");
	bcls_rec_wmmm99.Tables[0].Columns.Add(DT_STRING, "OLD_STOCK_NO");
	bcls_rec_wmmm99.Tables[0].Columns.Add(DT_STRING, "OLD_STOCK_PLACE_NO");
	bcls_rec_wmmm99.Tables[0].Columns.Add(DT_DECIMAL, "OLD_LAYER_NO");
	bcls_rec_wmmm99.Tables[0].Rows.Clear();



	


	//更新库位
	EIClass bcls_rec_stock_upd;
	bcls_rec_stock_upd.Tables[0].set_TableName("WM_STOCK_UPDATE");
	bcls_rec_stock_upd.Tables[0].Columns.Add(twma2);
	bcls_rec_stock_upd.Tables[0].Columns.Add(DT_STRING, "CRANE_NO");
	bcls_rec_stock_upd.Tables[0].Rows.Clear();


	//记录履历
	EIClass bcls_rec_stock_log;
	bcls_rec_stock_log.Tables[0].set_TableName("WM_STOCK_LOG");
	bcls_rec_stock_log.Tables[0].Rows.Clear();

	//发卸车确认电文
	EIClass bcls_xcqr;
	bcls_xcqr.Tables[0].set_TableName("ZCHO");
	bcls_xcqr.Tables[0].Columns.Add(twmsm62);
	bcls_xcqr.Tables[0].Rows.Clear();

	//2250发板坯电文
	EIClass bcls_2250;
	bcls_2250.Tables[0].Columns.Add(DT_STRING,"MAT_NO");
	bcls_2250.Tables[0].Rows.Clear();

	blkNum = bcls_rec->Tables.IndexOf("T80RYA");
	if (blkNum < 0)
	{
		bcls_rec->Tables.Add("T80RYA");
		bcls_rec->Tables["T80RYA"].Columns.Add(twma0);
		if (!bcls_rec->Tables["T80RYA"].Columns.Contains("STOCK_OPER_TIME"))
			bcls_rec->Tables["T80RYA"].Columns.Add(DT_STRING, "STOCK_OPER_TIME");
		bcls_rec->Tables["T80RYA"].Rows.Clear();
	}


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

			Log::Trace("", __FUNCTION__, "传入参数v_mat_no\t[{0}]", v_mat_no);
			Log::Trace("", __FUNCTION__, "传入参数v_stock_oper_order\t[{0}]", v_stock_oper_order);
			Log::Trace("", __FUNCTION__, "传入参数v_stock_oper_order_div\t[{0}]", v_stock_oper_order_div);
			Log::Trace("", __FUNCTION__, "传入参数v_stock_no\t[{0}]", v_stock_no);
			Log::Trace("", __FUNCTION__, "传入参数v_stock_place_no\t[{0}]", v_stock_place_no);
			Log::Trace("", __FUNCTION__, "传入参数v_layerno\t[{0}]", v_layerno);
			Log::Trace("", __FUNCTION__, "传入参数v_stock_place_position\t[{0}]", v_stock_place_position);
			Log::Trace("", __FUNCTION__, "传入参数v_crane_no\t[{0}]", v_crane_no);
			Log::Trace("", __FUNCTION__, "传入参数v_vehicle_no\t[{0}]", v_vehicle_no);

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
			

			Log::Trace("", __FUNCTION__, "传入参数\t[{0}]", __LINE__);
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
			//twma0.Query("MAT_NO,STOCK_OPER_ORDER");
			Log::Trace("", __FUNCTION__, "传入参数\t[{0}]", __LINE__);

			//2. 检查A1
			twma1["MAT_NO"] = v_mat_no;
			if (!twma1.Query("MAT_NO"))
			{
				sprintf(s.msg, "TWMA1没有查询到[%s]。", (const char*)v_mat_no);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			

			//3. 检查A2（非必需）
			twma2_old["MAT_NO"] = v_mat_no;
			twma2_old.Query("MAT_NO");

			if (twma2_old["STOCK_PLACE_NO"].ToString().Trim() == "" &&
				twma2_old["OLD_STOCK_PLACE_NO"].ToString().Trim() != "")
			{
				//对于行车落下时  库位上原有的东西会被踢出去 记录原库位，按照原库位操作
				twma2_old["STOCK_PLACE_NO"] = twma2_old["OLD_STOCK_PLACE_NO"];
				twma2_old["STOCK_PLACE_POSITION"] = twma2_old["OLD_STOCK_PLACE_POSITION"];
			}
			Log::Trace("", __FUNCTION__, "传入参数\t[{0}]", __LINE__);

			//4. 更新库位（f_wmsmsm_stock_update）
			twma2["MAT_NO"] = v_mat_no;
			twma2["STOCK_NO"] = v_stock_no;
			twma2["STOCK_PLACE_NO"] = v_stock_place_no;
			twma2["LAYERNO"] = v_layerno;
			twma2["STOCK_PLACE_POSITION"] = v_stock_place_position;

			twma2.MergeTo(bcls_rec_stock_upd.Tables["WM_STOCK_UPDATE"], false);
			bcls_rec_stock_upd.Tables["WM_STOCK_UPDATE"].Rows[iRow]["CRANE_NO"] = v_crane_no;

			Log::Trace("", __FUNCTION__, "传入参数\t[{0}]", __LINE__);

			//5. 删A0（按材料号、类型）
			twma0["MAT_NO"] = v_mat_no;
			//twma0.STOCK_OPER_ORDER = v_stock_oper_order;
			twma0.Delete("MAT_NO, STOCK_OPER_ORDER");

			Log::Trace("", __FUNCTION__, "传入参数\t[{0}]", __LINE__);
			//6. MM0099
			//调用物料跟踪
			bcls_rec_wmmm99.Tables["WMMM99"].Rows.Add();
			bcls_rec_wmmm99.Tables["WMMM99"].Rows[iRow]["MAT_NO"] = twma1["MAT_NO"];
			bcls_rec_wmmm99.Tables["WMMM99"].Rows[iRow]["STOCK_OPER_ORDER"] = twma0["STOCK_OPER_ORDER"];
			bcls_rec_wmmm99.Tables["WMMM99"].Rows[iRow]["STOCK_OPER_ORDER_DIV"] = v_stock_oper_order_div;
			bcls_rec_wmmm99.Tables["WMMM99"].Rows[iRow]["OLD_STOCK_NO"] = twma2_old["STOCK_NO"];
			bcls_rec_wmmm99.Tables["WMMM99"].Rows[iRow]["OLD_STOCK_PLACE_NO"] = twma2_old["STOCK_PLACE_NO"];
			bcls_rec_wmmm99.Tables["WMMM99"].Rows[iRow]["OLD_LAYER_NO"] = twma2_old["LAYERNO"];


			Log::Trace("", __FUNCTION__, "传入参数\t[{0}]", __LINE__);



			//13. 写A4
			twma4.CopyFrom(twma1);
			twma4["STOCK_OPER_ORDER"] = v_stock_oper_order;
			if (twma2_old["STOCK_NO"].ToString().Trim() != "" &&
				twma2_old["STOCK_NO"].ToString().Trim() == twma2["STOCK_NO"].ToString().Trim())
			{
				//如果材料当前有库位，且入库库区与当前库区一致，履历记录为倒垛
				if (v_stock_oper_order[0] == '1')
				{
					CString v_temp = twma4["STOCK_OPER_ORDER"];
					v_temp[0] = '3';
					twma4["STOCK_OPER_ORDER"] = v_temp;
				}
			}
			twma4["STOCK_NO"] = v_stock_no;
			twma4["STOCK_L2"]= Db::QueryCString(" SELECT STOCK_NO_ANOTHER FROM TWM01 WHERE STOCK_NO='" + v_stock_no + "' ");
			twma4["STOCK_PLACE_NO"] = v_stock_place_no;
			twma4["LAYERNO"] = v_layerno;
			twma4["FROM_STOCK_NO"] = twma0["FROM_STOCK_NO"];
			twma4["FROM_STOCK_PLACE_NO"] = twma0["FROM_STOCK_PLACE_NO"];
			twma4["CRANE_NO"] = v_crane_no;
			twma4["VEHICLE_NO"] = v_vehicle_no;
			twma4["TRANS_TOOL"] = twma0["TRANS_TOOL"];

			twma4.MergeTo(bcls_rec_stock_log.Tables["WM_STOCK_LOG"], false);

		

			sqlstr = " SELECT CODE_DESC_3_CONTENT FROM TWMSMZD02  WHERE  CODE_CLASS='WM02' and CODE='" + twma1["GUIDE_DEST"].ToString() + "' ";
			CString kefa = Db::QueryCString(sqlstr);
			if (kefa =="1")
			{
				bcls_2250.Tables[0].Rows.Add();
				bcls_2250.Tables[0].Rows[iRow]["MAT_NO"] = twma1["MAT_NO"];
			}
			Log::Trace("", __FUNCTION__, "传入参数\t[{0}]", __LINE__);
			//15、向制造发送入库
			/*bcls_rec->Tables["T80RYA"].Rows.Add();
			bcls_rec->Tables["T80RYA"].Rows[0]["STOCK_OPER_ORDER"] = twma0["STOCK_OPER_ORDER"];
			bcls_rec->Tables["T80RYA"].Rows[0]["STOCK_OPER_ORDER_DIV"] = twma0["STOCK_OPER_ORDER_DIV"];
			bcls_rec->Tables["T80RYA"].Rows[0]["MAT_NO"] = twma0["MAT_NO"];
			bcls_rec->Tables["T80RYA"].Rows[0]["MAT_NUM"] = twma0["MAT_NUM"];
			bcls_rec->Tables["T80RYA"].Rows[0]["MAT_LINE_TYPE"] = twma0["MAT_LINE_TYPE"];
			bcls_rec->Tables["T80RYA"].Rows[0]["MAT_KIND"] = twma0["MAT_KIND"];
			bcls_rec->Tables["T80RYA"].Rows[0]["FACTORY_DIV"] = "LG1";
			bcls_rec->Tables["T80RYA"].Rows[0]["USER_ID"] = twma0["USER_ID"];
			bcls_rec->Tables["T80RYA"].Rows[0]["STOCK_OPER_TIME"] = v_datetime;
			bcls_rec->Tables["T80RYA"].Rows[0]["TO_STOCK_NO"] = twma0["TO_STOCK_NO"];
			bcls_rec->Tables["T80RYA"].Rows[0]["TO_STOCK_PLACE_NO"] = twma0["TO_STOCK_PLACE_NO"];
			bcls_rec->Tables["T80RYA"].Rows[0]["TO_LAYERNO"] = twma0["TO_LAYERNO"];*/
		}

		Log::Trace("", __FUNCTION__, "传入参数\t[{0}]", __LINE__);

		//调用更新库位函数
		doFlag = f_wmsmsm_stock_update(&bcls_rec_stock_upd, bcls_ret, conn);
		if (doFlag != 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
		Log::Trace("", __FUNCTION__, "传入参数v_vehicle_no\t[{0}]", __LINE__);
		

		//调用物料/电文函数
		doFlag = f_wmsmsm_mm0099(&bcls_rec_wmmm99, bcls_ret, conn);
		if (doFlag != 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
		Log::Trace("", __FUNCTION__, "传入参数v_vehicle_no\t[{0}]", __LINE__);
		//调用履历函数
		doFlag = f_wmsmsm_stock_log(&bcls_rec_stock_log, bcls_ret, conn);
		if (doFlag != 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
		Log::Trace("", __FUNCTION__, "传入参数v_vehicle_no\t[{0}]", __LINE__);
		
		Log::Trace("", __FUNCTION__, "传入参数v_vehicle_no\t[{0}]", __LINE__);
		
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
