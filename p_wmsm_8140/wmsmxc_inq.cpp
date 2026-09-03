/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         NYY
Version:		1.0
Date:			2023-12-1
Description:	卸车计划查询
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件

//函数申明

/*<remark>=========================================================
///<summary>
///卸车计划查询
///<para>
///2.排序方式：
///</para>
///<para>数据库表：TWMSM62 倒运计划表；
///<returns>返回符合查询条件的计划信息</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wmsmxc_inq);

int f_wmsmxc_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */

	CModel twmsm62 = CModel("TWMSM62");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlstr1 = "";
	CString sqlwhere = "";
	CString s_userid("");
	CString sqlstr_count;
	CString sqlstr_temp;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_con(conn);


	//系统的分页类信息。
	CPageInfo pageInfo;

	/* 业务变量 */

	CString truck_no("");

	CString	truck_board_no("");
	CString	practice_no("");
	CString	load_code_factory("");

	CString	unload_flag("");
	CString	plan_start_time("");
	CString	plan_end_time("");

	//CString	datetime("");

	CDecimal cd_count = 0;
	int	record_count_per_page = 0; /* 每页记录数 */
	int	current_page_no = 0; /* 需查询的页号,从0开始计数 */
	int	start_row = 0; /* 将要压入outBlock的起始行 */
	try{

		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		//分页信息
		//CDataTable& table = bcls_ret->Tables.Add("PAGEINFO");//
		//table.Columns.Add(DT_DECIMAL, "recordsum");
		//rec_time_to = bcls_rec->Tables[0].Rows[0]["REC_CREATE_TIME_TO"].ToString().Trim();	//炼钢单元号
		/*	record_count_per_page = bcls_rec->Tables[0].Rows[0]["RECORD_COUNT_PER_PAGE"];
		current_page_no = bcls_rec->Tables[0].Rows[0]["CURRENT_PAGE_NO"];*/

		if (bcls_rec->Tables[0].Columns.Contains("TRUCK_NO"))
			truck_no = bcls_rec->Tables[0].Rows[0]["TRUCK_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("LOAD_CODE_FACTORY"))
			load_code_factory = bcls_rec->Tables[0].Rows[0]["LOAD_CODE_FACTORY"].ToString().Trim();
		/*if (bcls_rec->Tables[0].Columns.Contains("PLAN_START_TIME"))
			plan_start_time = bcls_rec->Tables[0].Rows[0]["PLAN_START_TIME"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("PLAN_END_TIME"))
			plan_end_time = bcls_rec->Tables[0].Rows[0]["PLAN_END_TIME"].ToString().Trim();*/
		if (bcls_rec->Tables[0].Columns.Contains("PRACTICE_NO"))
			practice_no = bcls_rec->Tables[0].Rows[0]["PRACTICE_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("TRUCK_BOARD_NO"))
			truck_board_no = bcls_rec->Tables[0].Rows[0]["TRUCK_BOARD_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("UNLOAD_FLAG"))
			unload_flag = bcls_rec->Tables[0].Rows[0]["UNLOAD_FLAG"].ToString().Trim();

		Log::Info("", __FUNCTION__, "truck_no		= [{0}]", truck_no);
		Log::Info("", __FUNCTION__, "load_code_factory	= [{0}]", load_code_factory);
		Log::Trace("", __FUNCTION__, "truck_board_no = [{0}]", truck_board_no);
		Log::Info("", __FUNCTION__, "unload_flag	= [{0}]", unload_flag);

		/*Log::Trace("", __FUNCTION__, "record_count_per_page	= [{0}]", record_count_per_page);
		Log::Trace("", __FUNCTION__, "current_page_no			= [{0}]", current_page_no);
		*/

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库

			sqlstr_count = "SELECT COUNT(1) FROM TWMSM62 WHERE 1=1 ";

			sqlstr = "SELECT  distinct PRACTICE_NO，PLAN_NO，TRUCK_NO，TRUCK_BOARD_NO，WAGONNO，TRUCK_MODEL，TRUCK_MODEL_DESC，LOAD_CODE_FACTORY，LOAD_CODE，Z_METAGE_NUM，Z_METAGE_WEIGHT，EMP_CODE，EMP_NAME，SHIFT_NO，ONDUTY_SN FROM TWMSM62 WHERE 1=1 ";

			if (truck_no.Trim() != "")
			{
				sqlstr_temp += " AND truck_no = @truck_no  ";
			}
			if (truck_board_no.Trim() != "")
			{
				sqlstr_temp += " AND truck_board_no = @truck_board_no  ";
			}
			if (load_code_factory.Trim() != "")
			{
				sqlstr_temp += " AND load_code_factory = @load_code_factory ";
			}
			/*if (plan_start_time.Trim() != "")
			{
				sqlstr_temp += " AND REC_CREATE_TIME >= @plan_start_time ";
			}
			if (plan_end_time.Trim() != "")
			{
				sqlstr_temp += " AND REC_CREATE_TIME <= @plan_end_time ";
			}*/
			if (practice_no.Trim() != "")
			{
				sqlstr_temp += " AND PRACTICE_NO like @practice_no ";
			}
			if (unload_flag.Trim() != "")
			{
				sqlstr_temp += " AND unload_flag = @unload_flag ";
			}


			sqlstr_count = sqlstr_count + sqlstr_temp;
			sqlstr_temp += " order BY PRACTICE_NO  ";
			sqlstr = sqlstr + sqlstr_temp;


			// 设置SQL中的变量
			cmd_inq.Parameters.Set("practice_no", practice_no + "%");
			cmd_inq.Parameters.Set("unload_flag", unload_flag );
			cmd_inq.Parameters.Set("load_code_factory", load_code_factory);
			cmd_inq.Parameters.Set("truck_no", truck_no);
			cmd_inq.Parameters.Set("truck_board_no", truck_board_no);
			/*cmd_inq.Parameters.Set("plan_start_time", plan_start_time + "000000");
			cmd_inq.Parameters.Set("plan_end_time", plan_end_time + "999999");*/

			break;
		}


		Log::Trace("", __FUNCTION__, "sqlstr_temp			= [{0}]", (const char*)sqlstr_temp);
		Log::Trace("", __FUNCTION__, "sqlstr_count		= [{0}]", (const char*)sqlstr_count);
		Log::Trace("", __FUNCTION__, "sqlstr				= [{0}]", (const char*)sqlstr);

		//cmd_inq.SetCommandText(sqlstr_count);

		//cd_count = cmd_inq.ExecuteScalar();
		//Log::Trace("", __FUNCTION__, "cd_count = [{0}]", cd_count);
		//start_row = record_count_per_page * (current_page_no - 1);
		//if (start_row > cd_count.ToDouble())
		//{
		//	start_row = 0;
		//}
		//cmd_inq.Close();
		cmd_inq.SetCommandText(sqlstr);
		/*cmd_inq.ExecuteQuery(bcls_ret->Tables[0], start_row, record_count_per_page);*/
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();



		//返回分页信息 
		//bcls_ret->Tables.Add("PAGEINFO");	//增加块
		//bcls_ret->Tables["PAGEINFO"].Columns.Add(DT_DECIMAL, "TOTAL_RECORD");						//总记录数
		//bcls_ret->Tables["PAGEINFO"].Rows.Add();
		//bcls_ret->Tables["PAGEINFO"].Rows[0][0] = cd_count.ToInt32();
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "Database processing error，sqlcode = [{0}]." /* _RES("GCRSS0000006")*//*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
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