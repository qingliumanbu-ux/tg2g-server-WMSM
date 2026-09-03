 /*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:
Version:		1.0
Date:			2016-03-05
Description:	出库行车命令功能
**************************************************/

//框架头文件
#include "stdafx.h"


//函数申明
//命令生成函数
BM2_FUNCTION_IMPORT
//int f_wmhpsm_cranecmd_make(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);

/*<remark>=========================================================
///<summary>
///板坯出库功能
///<para>
///2.排序方式：
///</para>
///<para>数据库表：TWMA0 倒躲队列；TWMA1 物料主档表
///<returns>执行预材料预入库功能</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wmsmsma2_out_cmd)
int f_wmsmsma2_out_cmd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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

	/* 实体类定义 */
	//CTWMA0 twma0(conn);
	//CTWMA1 twma1(conn);
	//CTWMA2 twma2(conn);
	//CTWM01 twm01(conn);
	CModel twma0 = CModel("TWMA0");
	CModel twma1 = CModel("TMMSM01");
	CModel twma2 = CModel("TWMA2");
	CModel twm01 = CModel("TWM01");


	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);




	EIClass bcls_cmd;
	bcls_cmd.Tables.Add("CMD_MAKE");
	bcls_cmd.Tables["CMD_MAKE"].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");
	bcls_cmd.Tables["CMD_MAKE"].Columns.Add(DT_STRING, "STOCK_PLACE_NO_TO");
	bcls_cmd.Tables["CMD_MAKE"].Columns.Add(DT_STRING, "MAT_NO");

	try
	{
		if (bcls_rec->Tables[0].Rows[0]["AIM_STOCK_NO"].ToString().Trim() == "")
		{
			aim_stock_no = bcls_rec->Tables[1].Rows[0]["TO_STOCK_NO"].ToString().Trim();
		}
		else
		{
			aim_stock_no = bcls_rec->Tables[0].Rows[0]["AIM_STOCK_NO"].ToString().Trim();
		}

		if (bcls_rec->Tables[0].Columns.Contains("OUT_STOCK_TIME"))
		{
			out_stock_time = bcls_rec->Tables[0].Rows[0]["OUT_STOCK_TIME"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("TRNP_MODE_CODE"))
		{
			trnp_mode_code = bcls_rec->Tables[0].Rows[0]["TRNP_MODE_CODE"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("TRUCK_NO"))
		{
			truck_no = bcls_rec->Tables[0].Rows[0]["TRUCK_NO"].ToString().Trim();
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


		Log::Trace("", __FUNCTION__, "传入参数 aim_stock_no：\t[{0}]", aim_stock_no);
		Log::Trace("", __FUNCTION__, "传入参数 out_stock_time：\t[{0}]", out_stock_time);
		Log::Trace("", __FUNCTION__, "传入参数 trnp_mode_code：\t[{0}]", trnp_mode_code);
		Log::Trace("", __FUNCTION__, "传入参数 truck_no：\t[{0}]", truck_no);
		Log::Trace("", __FUNCTION__, "传入参数 remark0：\t[{0}]", remark0);
		Log::Trace("", __FUNCTION__, "传入参数 remark1：\t[{0}]", remark1);


		for (int i = 0; i < bcls_rec->Tables[1].Rows.get_Count(); i++)
		{
			//获取传入参数
			twma0.Reset();
			twma1.Reset();
			twma0.MergeFrom(bcls_rec->Tables[1].Rows[i]);
			Log::Trace("", __FUNCTION__, " twma1.MAT_NO =[{0}]", twma0["MAT_NO"].ToString());
			Log::Trace("", __FUNCTION__, " twma1.MAT_KIND =[{0}]", twma0["MAT_KIND"].ToString());
			Log::Trace("", __FUNCTION__, " twma1.STOCK_OPER_ORDER =[{0}]", twma0["STOCK_OPER_ORDER"].ToString());

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

			if (twma0["STOCK_OPER_ORDER"].ToString().Trim() == "2G")
			{
				if (aim_stock_no.Trim() == "" &&
					twma0["TO_STOCK_NO"].ToString().Trim() == "")
				{
					sprintf(s.msg, "目的位置不能为空");
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}


			//查询材料主档、库位跟踪表
			twma1["MAT_NO"] = twma0["MAT_NO"];
			if (!twma1.Query("MAT_NO"))
			{
				sprintf(s.msg, "物料档该材料不存在");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			twma2["MAT_NO"] = twma0["MAT_NO"];
			twma2.Query("MAT_NO");
			layerno1 = twma2["LAYERNO"];




			//调用仓库出库主函数
			bcls_cmd.Tables["CMD_MAKE"].Rows.Add();
			bcls_cmd.Tables["CMD_MAKE"].Rows[i]["MAT_NO"] = twma0["MAT_NO"];
			bcls_cmd.Tables["CMD_MAKE"].Rows[i]["STOCK_OPER_ORDER"] = twma0["STOCK_OPER_ORDER"];
			bcls_cmd.Tables["CMD_MAKE"].Rows[i]["STOCK_PLACE_NO_TO"] = aim_stock_no;





		}


		//doFlag = f_wmhpsm_cranecmd_make(&bcls_cmd, bcls_ret, conn);
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

