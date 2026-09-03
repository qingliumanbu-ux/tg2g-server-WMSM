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
int f_wmsmsm_stock_out_db(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);

/*<remark>=========================================================
///<summary>
///板坯出库功能
///<para>
///2.排序方式：
///</para>
///<para>数据库表：TWMA0 倒躲队列；TWMA1 物料主档表
///<returns>执行预材料预入库功能</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wmsmsm12_outstock_db)
int f_wmsmsm12_outstock_db(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
	CString c_deliverytype = "";
	CString c_acceptdept = "";
	CString c_acceptstock = "";
	CString	update_data = "";
	CString	condition_data = "";
	CDecimal layerno1 = 0;

	CString SEQ_ID = "";                 //顺序号 件数
	CString LAYERNO = "";                 //层号
	CString STOCK_PLACE_POSITION = "";  //车内顺序号
	/* 实体类定义 */
	//CTWMA0 twma0(conn);
	//CTWMA1 twma1(conn);
	//CTWMA2 twma2(conn);
	//CTWM01 twm01(conn);
	CModel twma0 = CModel("TWMA0");
	CModel twma1 = CModel("TMMSM01");
	CModel twma2 = CModel("TWMA2");
	CModel twm01 = CModel("TWM01");
	CModel twm41dj = CModel("TWM41DJ");
	CModel twmsm61 = CModel("TWMSM61");


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
	bcls_stock_out.Tables[0].Columns.Add(twm41dj);
	bcls_stock_out.Tables[0].Columns.Add(twmsm61);

	bcls_stock_out.Tables[0].Rows.Clear();





	try
	{


		out_stock_time = bcls_rec->Tables[0].Rows[0]["OUT_STOCK_TIME"].ToString().Trim();
		//trnp_mode_code = bcls_rec->Tables[0].Rows[0]["TRNP_MODE_CODE"].ToString().Trim();
		//truck_no = bcls_rec->Tables[0].Rows[0]["TRUCK_NO"].ToString().Trim();
		//c_deliverytype = bcls_rec->Tables[0].Rows[0]["C_DELIVERYTYPE"].ToString().Trim();
		c_acceptdept = bcls_rec->Tables[0].Rows[0]["C_ACCEPTDEPT"].ToString().Trim();
		c_acceptstock = bcls_rec->Tables[0].Rows[0]["C_ACCEPTSTOCK"].ToString().Trim();

		if (out_stock_time.Trim() == "")
		{
			out_stock_time = CDateTime::Now().ToString("yyyyMMddHHmmss");
		}


		Log::Trace("", __FUNCTION__, "传入参数 out_stock_time：\t[{0}]", out_stock_time);
		Log::Trace("", __FUNCTION__, "传入参数 trnp_mode_code：\t[{0}]", trnp_mode_code);
		Log::Trace("", __FUNCTION__, "传入参数 truck_no：\t[{0}]", truck_no);
		Log::Trace("", __FUNCTION__, "传入参数 remark0：\t[{0}]", remark0);
		Log::Trace("", __FUNCTION__, "传入参数 remark1：\t[{0}]", remark1);

		Log::Trace("", __FUNCTION__, "传入参数 SEQ_ID：\t[{0}]", SEQ_ID);
		Log::Trace("", __FUNCTION__, "传入参数 LAYERNO：\t[{0}]", LAYERNO);
		Log::Trace("", __FUNCTION__, "传入参数 STOCK_PLACE_POSITION：\t[{0}]", STOCK_PLACE_POSITION);

		for (int i = 0; i < bcls_rec->Tables[1].Rows.get_Count(); i++)
		{
			

			twma0.Reset();
			twma1.Reset();
			twma0.MergeFrom(bcls_rec->Tables[1].Rows[i]);
			Log::Trace("", __FUNCTION__, " twma1.MAT_NO =[{0}]", twma0["MAT_NO"].ToString());
			Log::Trace("", __FUNCTION__, " twma1.MAT_KIND =[{0}]", twma0["MAT_KIND"].ToString());
			Log::Trace("", __FUNCTION__, "传入参数 aim_stock_no：\t[{0}]", aim_stock_no);

			if (twma0["MAT_NO"].ToString().Trim() == "")
			{
				sprintf(s.msg, "材料号不能为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			twma0["STOCK_OPER_ORDER"] = "2A";
			


			//查询材料主档、库位跟踪表
			twma1["MAT_NO"] = twma0["MAT_NO"];
			if (!twma1.Query("MAT_NO"))
			{
				sprintf(s.msg, "物料档该材料不存在");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (twma1["RCV_MAT_FLAG"].ToString() != "S")
			{
				sprintf(s.msg, "材料[%s]未收货，不能出库！", (const char*)twma1["MAT_NO"]);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			twma2["MAT_NO"] = twma0["MAT_NO"];
			twma2.Query("MAT_NO");
			layerno1 = twma2["LAYERNO"];




			



			//调用仓库出库主函数
			bcls_stock_out.Tables["WM_STOCK"].Rows.Add();
			bcls_stock_out.Tables["WM_STOCK"].Rows[i].Merge(bcls_rec->Tables[1].Rows[i]);
			bcls_stock_out.Tables["WM_STOCK"].Rows[i]["MAT_NO"] = twma0["MAT_NO"];
			bcls_stock_out.Tables["WM_STOCK"].Rows[i]["STOCK_OPER_ORDER"] = twma0["STOCK_OPER_ORDER"];
			bcls_stock_out.Tables["WM_STOCK"].Rows[i]["STOCK_NO"] = aim_stock_no;
			bcls_stock_out.Tables["WM_STOCK"].Rows[i]["STOCK_PLACE_NO"] = " ";
			bcls_stock_out.Tables["WM_STOCK"].Rows[i]["ROWNO"] = " ";
			bcls_stock_out.Tables["WM_STOCK"].Rows[i]["COLUMN_NO"] = " ";
			bcls_stock_out.Tables["WM_STOCK"].Rows[i]["LAYERNO"] = 0;
			bcls_stock_out.Tables["WM_STOCK"].Rows[i]["STOCK_PLACE_POSITION"] = " ";
			bcls_stock_out.Tables["WM_STOCK"].Rows[i]["TRUCK_NO"] = truck_no;
			bcls_stock_out.Tables["WM_STOCK"].Rows[i]["TRUCK_BOARD_NO"] = truck_no;
			//bcls_stock_out.Tables["WM_STOCK"].Rows[i]["TRANS_TOOL"] = trnp_mode_code;
			bcls_stock_out.Tables["WM_STOCK"].Rows[i]["C_ACCEPTDEPT"] = c_acceptdept;
			bcls_stock_out.Tables["WM_STOCK"].Rows[i]["C_ACCEPTSTOCK"] = c_acceptstock;


		}


		doFlag = f_wmsmsm_stock_out_db(&bcls_stock_out, bcls_ret, conn);
		if (doFlag != 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
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

