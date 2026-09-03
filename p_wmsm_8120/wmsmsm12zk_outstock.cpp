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

/*<remark>=========================================================
///<summary>
///板坯出库功能
///<para>
///2.排序方式：
///</para>
///<para>数据库表：TWMA0 倒躲队列；TWMA1 物料主档表
///<returns>执行预材料预入库功能</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wmsmsm12zk_outstock)
int f_wmsmsm12zk_outstock(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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

	CString  SHIFT_NO, SHIFT_GROUP = "";

	CString	update_data = "";
	CString	condition_data = "";
	CDecimal layerno1 = 0;

	CString SEQ_ID = "";                 //顺序号 件数
	CString LAYERNO ="";                 //层号
	CString mat_no,transfer_bill_no, transfer_plan_no, STOCK_PLACE_POSITION = "";  //车内顺序号
	/* 实体类定义 */
	//CTWMA0 twma0(conn);
	//CTWMA1 twma1(conn);
	//CTWMA2 twma2(conn);
	//CTWM01 twm01(conn);
	CModel twma0 = CModel("TWMA0");
	CModel twma1 = CModel("TMMSM01");
	CModel twma2 = CModel("TWMA2");
	CModel twm01 = CModel("TWM01");
	CModel twmZC = CModel("TWMZC");
	CModel twm41dj("TWM41DJ");
	CModel twm42("TWM42");
	CModel twm41("TWM41");
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
	bcls_stock_out.Tables[0].Columns.Add(twmZC);
	bcls_stock_out.Tables[0].Rows.Clear();



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

		truck_no = bcls_rec->Tables[0].Rows[0]["TRUCK_NO"].ToString().Trim();

		if (truck_no == ""){
			sprintf(s.msg, "车号不能为空");
			throw CApplicationException(-1, s.msg, log.Location);
		}
	
		Log::Trace("", __FUNCTION__, "传入参数 truck_no：\t[{0}]", truck_no);

		for (int i = 0; i < bcls_rec->Tables[1].Rows.get_Count(); i++)
		{
			transfer_plan_no = bcls_rec->Tables[1].Rows[i]["TRANSFER_PLAN_NO"].ToString().Trim();
			transfer_bill_no = bcls_rec->Tables[1].Rows[i]["TRANSFER_BILL_NO"].ToString().Trim();
			mat_no == bcls_rec->Tables[1].Rows[i]["MAT_NO"].ToString().Trim();
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
			if (twma0["STOCK_OPER_ORDER"].ToString().Trim() == "")
			{
				sprintf(s.msg, "库业务类型不能为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (twma0["STOCK_OPER_ORDER"].ToString().Trim() != "2G")
			{
				sprintf(s.msg, "不是转库材料，不能出库");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (aim_stock_no.Trim() == "" &&
				twma0["TO_STOCK_NO"].ToString().Trim() == "")
			{
				sprintf(s.msg, "目的位置不能为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			sqlstr =
				" SELECT COUNT(1) FROM TWMA0"
				" WHERE MAT_NO = @mat_no"
				" AND STOCK_OPER_ORDER = @stock_oper_order";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("mat_no", twma0["MAT_NO"].ToString());
			cmd_inq.Parameters.Set("stock_oper_order", twma0["STOCK_OPER_ORDER"].ToString());
			rowCount = cmd_inq.ExecuteScalar();

			if (rowCount != 1)
			{
				sprintf(s.msg, "队列已变化，请刷新画面。");
				throw CApplicationException(-1, s.msg, log.Location);
			}


			//查询材料主档、库位跟踪表
			twma1["MAT_NO"] = twma0["MAT_NO"];
			if (!twma1.Query("MAT_NO"))
			{
				sprintf(s.msg, "物料档该材料不存在");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			//冷送板坯 需要表判合格、有最终出钢记号
			if (twma1["HOT_SEND_FLAG"].ToString().Trim() == "0" && twma1["FIN_ST_NO"].ToString().Trim() == "")
			{
				sprintf(s.msg, "材料没有最终出钢记号，不能出库");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (twma1["HOT_SEND_FLAG"].ToString().Trim() == "0" && twma1["SURFACE_DECIDE_CODE"].ToString().Trim() == "0")
			{
				sprintf(s.msg, "材料没有表面判定或表面判定不合格，不能出库");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			twma2["MAT_NO"] = twma0["MAT_NO"];
			twma2.Query("MAT_NO");
			layerno1 = twma2["LAYERNO"];




			//更新队列表
			twma0["PROC_STATUS"] = "9";
			twma0["VEHICLE_NO"] = truck_no;
			twma0["TO_STOCK_NO"] = aim_stock_no;
			twma0["EVENT_TIME"] = out_stock_time;
			twma0["TRANS_TOOL"] = trnp_mode_code;
			twma0["REMARK0"] = remark0;
			twma0["REMARK1"] = remark1;
			twma0["STORE_KEEPER"] = store_keeper;
			twma0["CRANE_RESP"] = crane_resp;
			twma0["CRANE_NO"] = crane_no;

			twma0.TrimOrBlank();
			twma0.Update(
				"PROC_STATUS,"
				"VEHICLE_NO,"
				"TO_STOCK_NO,"
				"EVENT_TIME,"
				"TRANS_TOOL,"
				"REMARK0,"
				"REMARK1,"
				"STORE_KEEPER,"
				"CRANE_RESP,"
				"CRANE_NO"
				,
				"MAT_NO,"
				"STOCK_OPER_ORDER");



			//调用仓库出库主函数
			bcls_stock_out.Tables["WM_STOCK"].Rows.Add();
			bcls_stock_out.Tables["WM_STOCK"].Rows[i]["MAT_NO"] = twma0["MAT_NO"];
			bcls_stock_out.Tables["WM_STOCK"].Rows[i]["STOCK_OPER_ORDER"] = twma0["STOCK_OPER_ORDER"];
			bcls_stock_out.Tables["WM_STOCK"].Rows[i]["STOCK_NO"] = aim_stock_no;
			bcls_stock_out.Tables["WM_STOCK"].Rows[i]["STOCK_PLACE_NO"] = " ";
			bcls_stock_out.Tables["WM_STOCK"].Rows[i]["ROWNO"] = " ";
			bcls_stock_out.Tables["WM_STOCK"].Rows[i]["COLUMN_NO"] = " ";
			bcls_stock_out.Tables["WM_STOCK"].Rows[i]["LAYERNO"] = 0;
			//bcls_stock_out.Tables["WM_STOCK"].Rows[i]["STOCK_PLACE_POSITION"] = " ";

		//	bcls_stock_out.Tables["WM_STOCK"].Rows[i]["SEQ_ID"] = " ";
			//bcls_stock_out.Tables["WM_STOCK"].Rows[i]["LAYERNO"] = LAYERNO;
			bcls_stock_out.Tables["WM_STOCK"].Rows[i]["STOCK_PLACE_POSITION"] = " ";

			//更新转库计划和转库材料
			twm42["TRANSFER_PLAN_NO"] = transfer_plan_no;
			twm42["MAT_NO"] = twma0["MAT_NO"];
			Log::Trace("", __FUNCTION__, " transfer_plan_no =[{0}]", transfer_plan_no);
			Log::Trace("", __FUNCTION__, " twm42.MAT_NO =[{0}]", twm42["MAT_NO"].ToString());
			if (!twm42.Query("TRANSFER_PLAN_NO, MAT_NO"))
			{
				CFormattable arguments[] = { mat_no }; // 定义参数列表的数组
				CMessageFormat::Format(s.msg, _RES("YM00S0000313")/*[{0}]没有转库计划，请确认接收到计划以后再操作。*/, arguments, 1);
				sprintf(s.msg, "转库计划号[%s]材料号[%s]在转库计划明细表中不存在。",
					(const char*)transfer_plan_no, (const char*)mat_no);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (twm42["AFFIRM_MARK"].ToString() != "8")//确认标志
			{
				sprintf(s.msg, "转库计划号[%s]材料号[%s]在已经装车！ ",
					(const char*)transfer_plan_no, (const char*)mat_no);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			//委托序号
			twm42["TRUCK"] = truck_no;
			twm42["TRUST_ID"] = datetime.Substring(3,11);
			twm42["SEND_FLAG"] = "1";
			twm42["SEND_TIME"] = datetime;
			twm42["AFFIRM_MARK"] = "9";
			
			Log::Debug("", __FUNCTION__, "f_epep_get_shift_group------------------------------开始");
			f_epep_get_shift_group("SM", datetime, SHIFT_NO, SHIFT_GROUP, conn);
			twm42["SHIFT_GROUP"] = SHIFT_GROUP;
			twm42["SHIFT_NO"] = SHIFT_NO;
			Log::Debug("", __FUNCTION__, "f_epep_get_shift_group------------------------------结束");
			twm42.Update("AFFIRM_MARK,TRUCK,SEND_FLAG,SEND_TIME,TRUST_ID,SHIFT_GROUP,SHIFT_NO", "TRANSFER_PLAN_NO, MAT_NO");

			//单据

			twm41dj["TRANSFER_PLAN_NO"] = transfer_plan_no;
			twm41dj["TRANSFER_BILL_NO"] = transfer_bill_no;
			if (!twm41dj.Query("TRANSFER_PLAN_NO, TRANSFER_BILL_NO"))
			{
				sprintf(s.msg, "转库计划号[%s]单据号[%s]在转库单据表中不存在。",
					(const char*)transfer_plan_no, (const char*)twm41dj["TRANSFER_BILL_NO"]);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (twm41dj["MOVE_BILL_STATUS"].ToString() != "4")//确认标志
			{
				sprintf(s.msg, "转库计划号[%s]单据号[%s]在已经装车！ ",
					(const char*)transfer_plan_no, (const char*)transfer_bill_no);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (twm42.QueryCount("TRANSFER_PLAN_NO,TRANSFER_BILL_NO,AFFIRM_MARK") == twm41dj["BILL_MAT_NUM"].ToDecimal()){
				twm41dj["MOVE_BILL_STATUS"] = "9";//出库
				twm41dj.Update("MOVE_BILL_STATUS", "TRANSFER_PLAN_NO, TRANSFER_BILL_NO");
			}

			//计划

			twm41["TRANSFER_PLAN_NO"] = transfer_plan_no;
			if (!twm41.Query("TRANSFER_PLAN_NO"))
			{
				sprintf(s.msg, "转库计划号[%s]在转库单据表中不存在。",
					(const char*)transfer_plan_no);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (twm41["TRANSFER_STATUS"].ToString() != "4")//确认标志
			{
				sprintf(s.msg, "转库计划号[%s]在已经装车！ ",
					(const char*)transfer_plan_no);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			//计划
			if (twm42.QueryCount("TRANSFER_PLAN_NO,AFFIRM_MARK") == twm41["TOTAL_NUM"].ToDecimal()){
				twm41["TRANSFER_STATUS"] = "9";//出库
				twm41.Update("TRANSFER_STATUS", "TRANSFER_PLAN_NO");
			}
		}


		doFlag = f_wmsmsm_stock_out(&bcls_stock_out, bcls_ret, conn);
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

