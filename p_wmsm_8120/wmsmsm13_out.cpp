/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         JHZHAO
Version:		1.0
Date:			2016-02-01
Description:	直供出坯确认材料出库功能
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件

//业务头文件


//函数申明
BM2_FUNCTION_IMPORT
int f_wmsmsm_stock_in(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);
BM2_FUNCTION_IMPORT
int f_wmsmsm_stock_out(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);
BM2_FUNCTION_IMPORT
int f_wm00_queue(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

int f_wmsmsm13_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn); 
/*<remark>=========================================================
///<summary>
///直供出坯确认材料出库结束功能
///<para>
///</para>
///<para>数据库表：TMMSM01坯料主档表
///<returns>更新主档材料信息</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wmsmsm13_out);

int f_wmsmsm13_out(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString procDiv = "";
	CString unitCode = "";
	CDecimal matActThick = 0;
	CDecimal matActWidth = 0;
	CString stockNoTo = " ";


	/* 数据库SQL操作字符串 */
	CString sqlstr = "";

	try
	{

		//if (!bcls_rec->Tables[0].Columns.Contains("PROC_DIV"))
		//{
		//	bcls_rec->Tables[0].Columns.Add(DT_STRING, "PROC_DIV");
		//}
		//bcls_rec->Tables[0].Rows[0]["PROC_DIV"] = "OUT";

		//doFlag = f_wmsmsm13_proc(bcls_rec, bcls_ret, conn);
		//if (doFlag != 0)
		//{
		//	throw CApplicationException(-1, s.msg, log.Location);

		//}
		CModel twma1 = CModel("TMMSM01");
		CModel twma2 = CModel("TWMA2");
		CModel twma0 = CModel("TWMA0");

		//调用仓库入库主函数
		EIClass bcls_stock_in;
		bcls_stock_in.Tables[0].set_TableName("WM_STOCK");
		bcls_stock_in.Tables[0].Columns.Add(twma0);
		bcls_stock_in.Tables[0].Columns.Add(twma2);
		bcls_stock_in.Tables[0].Rows.Clear();

		//调用仓库出库主函数
		EIClass bcls_stock_out;
		bcls_stock_out.Tables[0].set_TableName("WM_STOCK");
		bcls_stock_out.Tables[0].Columns.Add(twma0);
		bcls_stock_out.Tables[0].Columns.Add(twma2);
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
		bcls_stock_que.Tables["WM00QUE"].Columns.Add(DT_STRING, "FROM_STOCK_NO");
		bcls_stock_que.Tables["WM00QUE"].Rows.Clear();


		//调用计划函数
		EIClass bcls_rec_wmps99;
		bcls_rec_wmps99.Tables[0].set_TableName("WMPS99");
		bcls_rec_wmps99.Tables[0].Columns.Add(twma1);
		bcls_rec_wmps99.Tables[0].Columns.Add(twma2);
		bcls_rec_wmps99.Tables[0].Rows.Clear();




		CDbCommand cmd_inq(conn);

		stockNoTo = bcls_rec->Tables[0].Rows[0]["DEST_STOCK"].ToString().Trim();
		CString carNo = bcls_rec->Tables[0].Rows[0]["CAR_NO"].ToString().Trim();

		Log::Trace("", __FUNCTION__, "stockNoTo【{0}】", stockNoTo);

		if (stockNoTo.Trim() == "")
		{
			strcpy(s.msg, "请先选择出库去向");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (carNo.Trim() == "")
		{
			strcpy(s.msg, "卡车号不能为空。");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			twma1.Reset();
			twma2.Reset();

			twma1["MAT_NO"] = bcls_rec->Tables[0].Rows[i]["MAT_NO"].ToString().Trim();
			if (!twma1.Query())
			{
				strcpy(s.msg, "查询材料" + twma1["MAT_NO"].ToString() + "信息出错。");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			twma2["MAT_NO"] = twma1["MAT_NO"];
			twma2.Query("MAT_NO");



			//if (twma1["HOT_SEND_FLAG"].ToString().Trim() == "0" ||
			//	twma1["HOT_SEND_FLAG"].ToString().Trim() == "")
			//{
			//	strcpy(s.msg, "该材料" + twma1["MAT_NO"].ToString() + "不能直送,请选择下线入库操作。");
			//	throw CApplicationException(-1, s.msg, log.Location);
			//}

			if (twma1["SLABTOP_FLAG"].ToString().Trim() != "0" &&
				twma1["SLABTOP_FLAG"].ToString().Trim() != "")
			{
				strcpy(s.msg, "该材料" + twma1["MAT_NO"].ToString() + "已经出坯开始，不能进行出库操作。");
				throw CApplicationException(-1, s.msg, log.Location);
			}


			//补入库
			sqlstr =
				" SELECT STOCK_NO FROM TWM000E"
				" WHERE EVENT_ID = @stock_oper_order"
				" AND UNIT_CODE = @unit_code";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("stock_oper_order", "1B");
			cmd_inq.Parameters.Set("unit_code", twma1["UNIT_CODE"].ToString());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				twma2["STOCK_NO"] = cmd_inq.GetString(1);
			}
			else
			{
				sprintf(s.msg, "机组【%s】未配置目标库区（TWM000E）。", (const char*)twma1["UNIT_CODE"]);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			cmd_inq.Close();

			Log::Trace("", __FUNCTION__, "twma1.STOCK_NO[{0}]", twma2["STOCK_NO"].ToString());

			//杨晓明 2020.3.25 读取到出口库位后 也并没有使用 格格姐懿旨 将此段代码注释
			/*sqlstr =
				" SELECT STOCK_PLACE_NO FROM TWM04"
				" WHERE STOCK_NO = @stock_no"
				" AND UNIT_CODE = @unit_code"
				" AND ENTRANCE_EXIT_DIV = '2'";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("stock_no", twma2["STOCK_NO"].ToString());
			cmd_inq.Parameters.Set("unit_code", twma1["UNIT_CODE"].ToString());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				twma2["STOCK_PLACE_NO"] = cmd_inq.GetString(1);
			}
			else
			{
				sprintf(s.msg, "机组【%s】未配置出口库位（TWM04）。", (const char*)twma1["UNIT_CODE"]);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			cmd_inq.Close();

			Log::Trace("", __FUNCTION__, "twma2.STOCK_PLACE_NO[{0}]", twma2["STOCK_PLACE_NO"].ToString());*/



			//调用仓库入库主函数
			bcls_stock_in.Tables["WM_STOCK"].Rows.Clear();
			bcls_stock_in.Tables["WM_STOCK"].Rows.Add();
			bcls_stock_in.Tables["WM_STOCK"].Rows[0]["MAT_NO"] = twma1["MAT_NO"];
			bcls_stock_in.Tables["WM_STOCK"].Rows[0]["STOCK_OPER_ORDER"] = "1B";
			bcls_stock_in.Tables["WM_STOCK"].Rows[0]["STOCK_NO"] = twma2["STOCK_NO"];
			bcls_stock_in.Tables["WM_STOCK"].Rows[0]["STOCK_PLACE_NO"] = twma2["STOCK_PLACE_NO"];
			bcls_stock_in.Tables["WM_STOCK"].Rows[0]["ROWNO"] = " ";
			bcls_stock_in.Tables["WM_STOCK"].Rows[0]["COLUMN_NO"] = " ";
			bcls_stock_in.Tables["WM_STOCK"].Rows[0]["LAYERNO"] = 0;
			bcls_stock_in.Tables["WM_STOCK"].Rows[0]["STOCK_PLACE_POSITION"] = " ";



			doFlag = f_wmsmsm_stock_in(&bcls_stock_in, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}


			//调用仓库出库主函数
			bcls_stock_out.Tables["WM_STOCK"].Rows.Clear();
			bcls_stock_out.Tables["WM_STOCK"].Rows.Add();
			bcls_stock_out.Tables["WM_STOCK"].Rows[0]["MAT_NO"] = twma1["MAT_NO"];
			if (twma1["HOT_SEND_FLAG"].ToString().Trim() == "0" ||
				twma1["HOT_SEND_FLAG"].ToString().Trim() == "")
			{
				bcls_stock_out.Tables["WM_STOCK"].Rows[0]["STOCK_OPER_ORDER"] = "2G";
			}
			else
			{
				bcls_stock_out.Tables["WM_STOCK"].Rows[0]["STOCK_OPER_ORDER"] = "2X";
			}
			bcls_stock_out.Tables["WM_STOCK"].Rows[0]["STOCK_NO"] = stockNoTo;
			bcls_stock_out.Tables["WM_STOCK"].Rows[0]["STOCK_PLACE_NO"] = " ";
			bcls_stock_out.Tables["WM_STOCK"].Rows[0]["ROWNO"] = " ";
			bcls_stock_out.Tables["WM_STOCK"].Rows[0]["COLUMN_NO"] = " ";
			bcls_stock_out.Tables["WM_STOCK"].Rows[0]["LAYERNO"] = 0;
			bcls_stock_out.Tables["WM_STOCK"].Rows[0]["STOCK_PLACE_POSITION"] = " ";



			doFlag = f_wmsmsm_stock_out(&bcls_stock_out, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}


			//下分厂入库队列
			bcls_stock_que.Tables["WM00QUE"].Rows.Clear();
			bcls_stock_que.Tables["WM00QUE"].Rows.Add();
			bcls_stock_que.Tables["WM00QUE"].Rows[0]["MAT_NO"] = twma1["MAT_NO"];
			bcls_stock_que.Tables["WM00QUE"].Rows[0]["MAT_NUM"] = twma1["MAT_NUM"];
			bcls_stock_que.Tables["WM00QUE"].Rows[0]["PLAN_NO"] = " ";
			bcls_stock_que.Tables["WM00QUE"].Rows[0]["PLAN_EXEC_SEQ_NO"] = 0;
			bcls_stock_que.Tables["WM00QUE"].Rows[0]["STOCK_NO"] = stockNoTo;
			bcls_stock_que.Tables["WM00QUE"].Rows[0]["TRANS_TOOL"] = " ";
			bcls_stock_que.Tables["WM00QUE"].Rows[0]["PRE_UNIT_CODE"] = twma1["UNIT_CODE"];
			bcls_stock_que.Tables["WM00QUE"].Rows[0]["NEXT_UNIT_CODE"] = twma1["NEXT_UNIT_CODE"];
			bcls_stock_que.Tables["WM00QUE"].Rows[0]["OPER_FLAG"] = "I";
			bcls_stock_que.Tables["WM00QUE"].Rows[0]["STOCK_OPER_ORDER"] = "1X";
			bcls_stock_que.Tables["WM00QUE"].Rows[0]["STOCK_OPER_ORDER_DIV"] = " ";
			bcls_stock_que.Tables["WM00QUE"].Rows[0]["FROM_STOCK_NO"] = twma2["STOCK_NO"];

			doFlag = f_wm00_queue(&bcls_stock_que, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
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