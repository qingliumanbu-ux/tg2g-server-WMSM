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
int f_wmsmsm_stock_out_tg(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);

/*<remark>=========================================================
///<summary>
///板坯出库功能
///<para>
///2.排序方式：
///</para>
///<para>数据库表：TWMA0 倒躲队列；TWMA1 物料主档表
///<returns>执行预材料预入库功能</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wmsmsm12_outstock_tg)
int f_wmsmsm12_outstock_tg(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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

	CString	update_data = "";
	CString	condition_data = "";
	CDecimal layerno1 = 0;

	CString SEQ_ID = "";                 //顺序号 件数
	CString LAYERNO = "";                 //层号
	CString STOCK_PLACE_POSITION = "";  //车内顺序号
	CString	load_scheme_no = "";//预装方案
	/* 实体类定义 */
	//CTWMA0 twma0(conn);
	//CTWMA1 twma1(conn);
	//CTWMA2 twma2(conn);
	//CTWM01 twm01(conn);
	CModel twma0 = CModel("TWMA0");
	CModel twma1 = CModel("TMMSM01");
	CModel tmmsm01 = CModel("TMMSM01");
	CModel twma2 = CModel("TWMA2");
	CModel twm01 = CModel("TWM01");
	CModel twm41dj = CModel("TWM41DJ");
	CModel twmsm61 = CModel("TWMSM61");
	CModel twmsm12 = CModel("TWMSM12");
	CModel hwmsm12 = CModel("HWMSM12");
	

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
		trnp_mode_code = bcls_rec->Tables[0].Rows[0]["TRNP_MODE_CODE"].ToString().Trim();
		truck_no = bcls_rec->Tables[0].Rows[0]["TRUCK_NO"].ToString().Trim();

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

		if (bcls_rec->Tables[0].Columns.Contains("SEQ_ID"))
		{
			SEQ_ID = bcls_rec->Tables[0].Rows[0]["SEQ_ID"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("LAYERNO"))
		{
			LAYERNO = bcls_rec->Tables[0].Rows[0]["LAYERNO"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("STOCK_PLACE_POSITION"))
		{
			STOCK_PLACE_POSITION = bcls_rec->Tables[0].Rows[0]["STOCK_PLACE_POSITION"].ToString().Trim();
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
			Log::Trace("", __FUNCTION__, "行：\t[{0}]", __LINE__);
			aim_stock_no = bcls_rec->Tables[0].Rows[0]["AIM_STOCK_NO"].ToString().Trim();
			
			
			Log::Trace("", __FUNCTION__, "传入参数 aim_stock_no：\t[{0}]", aim_stock_no);

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
				sprintf(s.msg, "材料[%s]未收货，不能出库！",(const char*)twma1["MAT_NO"]);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			Log::Trace("", __FUNCTION__, "行：\t[{0}]", __LINE__);
			


			//查询材料主档、库位跟踪表
			

			//冷送板坯 需要表判合格、有最终出钢记号
			/*if (twma1["HOT_SEND_FLAG"].ToString().Trim() == "0" && twma1["FIN_ST_NO"].ToString().Trim() == "")
			{
				sprintf(s.msg, "材料没有最终出钢记号，不能出库");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (twma1["HOT_SEND_FLAG"].ToString().Trim() == "0" && twma1["SURFACE_DECIDE_CODE"].ToString().Trim() == "0")
			{
				sprintf(s.msg, "材料没有表面判定或表面判定不合格，不能出库");
				throw CApplicationException(-1, s.msg, log.Location);
			}*/
			Log::Trace("", __FUNCTION__, "行：\t[{0}]", __LINE__);
			twma2["MAT_NO"] = twma1["MAT_NO"];
			twma2.Query("MAT_NO");
			layerno1 = twma2["LAYERNO"];



			Log::Trace("", __FUNCTION__, "行：\t[{0}]", __LINE__);
			



			//调用仓库出库主函数
			bcls_stock_out.Tables["WM_STOCK"].Rows.Add();
			bcls_stock_out.Tables["WM_STOCK"].Rows[i].Merge(bcls_rec->Tables[0].Rows[i]);
			bcls_stock_out.Tables["WM_STOCK"].Rows[i]["MAT_NO"] = twma1["MAT_NO"];
			bcls_stock_out.Tables["WM_STOCK"].Rows[i]["STOCK_NO"] = aim_stock_no;
			bcls_stock_out.Tables["WM_STOCK"].Rows[i]["STOCK_PLACE_NO"] = " ";
			bcls_stock_out.Tables["WM_STOCK"].Rows[i]["ROWNO"] = " ";
			bcls_stock_out.Tables["WM_STOCK"].Rows[i]["COLUMN_NO"] = " ";
			bcls_stock_out.Tables["WM_STOCK"].Rows[i]["LAYERNO"] = 0;
			bcls_stock_out.Tables["WM_STOCK"].Rows[i]["STOCK_PLACE_POSITION"] = " ";
			if (truck_no.SubstringNE(0, 1) == "C")
			{
				bcls_stock_out.Tables["WM_STOCK"].Rows[i]["TRUCK_BOARD_NO"] = truck_no;
			}
			else
			{
				bcls_stock_out.Tables["WM_STOCK"].Rows[i]["TRUCK_NO"] = truck_no;
			}
			bcls_stock_out.Tables["WM_STOCK"].Rows[i]["TRANS_TOOL"] = trnp_mode_code;
			//bcls_stock_out.Tables["WM_STOCK"].Rows[i]["LOAD_END_TIME"] = out_stock_time;
			//bcls_stock_out.Tables["WM_STOCK"].Rows[i]["SEQ_ID"] = SEQ_ID;
			//bcls_stock_out.Tables["WM_STOCK"].Rows[i]["LAYERNO"] = LAYERNO;
			//bcls_stock_out.Tables["WM_STOCK"].Rows[i]["STOCK_PLACE_POSITION"] = STOCK_PLACE_POSITION;

			twmsm12["LOAD_SCHEME_NO"] = bcls_rec->Tables[1].Rows[0]["LOAD_SCHEME_NO"].ToString();
			if (twmsm12["LOAD_SCHEME_NO"].ToString().Trim() != "")
			{
				load_scheme_no = twmsm12["LOAD_SCHEME_NO"].ToString();
			}
			twmsm12["MAT_NO"] = twma0["MAT_NO"].ToString();
			if (twmsm12["LOAD_SCHEME_NO"].ToString().Trim() != "") {
				twmsm12.Query("LOAD_SCHEME_NO,MAT_NO");
				hwmsm12.CopyFrom(twmsm12);
				hwmsm12.Insert();
				twmsm12.Delete("LOAD_SCHEME_NO,MAT_NO");
			}
			if (twmsm12.QueryCount("MAT_NO") > 0)
			{
				twmsm12.Delete("MAT_NO");
			}
		}
		twmsm12["LOAD_SCHEME_NO"] = load_scheme_no;
		if (twmsm12.QueryCount("LOAD_SCHEME_NO") > 0)
		{
			//sqlstr = " select * from twmsm12 where LOAD_SCHEME_NO='" + load_scheme_no + "' ";//代表有材料在预装车单中未使用，预装车标记清空
			//Log::Trace("", __FUNCTION__, "传入参数 sqlstr：\t[{0}]", sqlstr);
			//cmd_inq.SetCommandText(sqlstr);
			//cmd_inq.ExecuteReader();
			//while (cmd_inq.Read())
			//{
			//	cmd_inq.Fetch(twmsm12);
			//	tmmsm01["MAT_NO"] = twmsm12["MAT_NO"];
			//	tmmsm01["PRE_LOAD_FLAG"] = "0";
			//	tmmsm01["LOAD_SCHEME_NO"] = " ";
			//	tmmsm01.Update("PRE_LOAD_FLAG,LOAD_SCHEME_NO", "MAT_NO");
			//}
			//cmd_inq.Close();
			//twmsm12.Delete("LOAD_SCHEME_NO");
			sprintf(s.msg, "改装车单[%s]下有材料未被选择，如无需装车，请在预装车画面进行撤销装车！", (const char*)load_scheme_no);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		doFlag = f_wmsmsm_stock_out_tg(&bcls_stock_out, bcls_ret, conn);
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

