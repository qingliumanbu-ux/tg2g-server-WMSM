/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         QL
Version:		1.0
Date:			2016-10-08
Description:	吊车命令查询
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件

//函数申明
int f_epes_get_auth_other(const char *iuser, int irestype, EIClass *bcls_ret, CDbConnection * conn);

/*<remark>=========================================================
///<summary>
///吊车命令查询
///<para>
///2.排序方式：CMD_SEQ
///</para>
///<para>数据库表：TWMA7 行车命令表；
///<returns>返回符合查询条件的命令信息</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wmsmsma7_inq);

int f_wmsmsma7_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */
	//CTWMA7 twma7(conn);
	CModel twma7 = CModel("TWMA7");

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

	CString crane_no("");
	CString	mat_no("");
	CString	stock_no("");
	CString	rec_create_time("");
	CString	rec_time_to("");
	CString	stock_place_no_from("");
	CString	stock_place_no_to("");



	//CString	datetime("");
	CString	cs_ladle_no("");
	CString	cs_sm_unit_no("");
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
		record_count_per_page = bcls_rec->Tables[0].Rows[0]["RECORD_COUNT_PER_PAGE"];
		current_page_no = bcls_rec->Tables[0].Rows[0]["CURRENT_PAGE_NO"];

		if (bcls_rec->Tables[0].Columns.Contains("CRANE_NO"))
			crane_no = bcls_rec->Tables[0].Rows[0]["CRANE_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("MAT_NO"))
			mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("STOCK_NO"))
			stock_no = bcls_rec->Tables[0].Rows[0]["STOCK_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("REC_CREATE_TIME"))
			rec_create_time = bcls_rec->Tables[0].Rows[0]["REC_CREATE_TIME"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("REC_CREATE_TIME_TO"))
			rec_time_to = bcls_rec->Tables[0].Rows[0]["REC_CREATE_TIME_TO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("STOCK_PLACE_NO_FROM"))
			stock_place_no_from = bcls_rec->Tables[0].Rows[0]["STOCK_PLACE_NO_FROM"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("STOCK_PLACE_NO_TO"))
			stock_place_no_to = bcls_rec->Tables[0].Rows[0]["STOCK_PLACE_NO_TO"].ToString().Trim();
		
		
		Log::Info("", __FUNCTION__, "crane_no		= [{0}]", crane_no);
		Log::Info("", __FUNCTION__, "mat_no	= [{0}]", mat_no);
		Log::Trace("", __FUNCTION__, "stock_no = [{0}]", stock_no);
		Log::Info("", __FUNCTION__, "rec_create_time	= [{0}]", rec_create_time);
		Log::Trace("", __FUNCTION__, "rec_time_to = [{0}]", rec_time_to);
		Log::Info("", __FUNCTION__, "stock_place_no_from	= [{0}]", stock_place_no_from);
		Log::Trace("", __FUNCTION__, "stock_place_no_to = [{0}]", stock_place_no_to);
		Log::Trace("", __FUNCTION__, "s.fore_ip	= [{0}]", s.fore_ip);


		Log::Trace("", __FUNCTION__, "record_count_per_page	= [{0}]", record_count_per_page);
		Log::Trace("", __FUNCTION__, "current_page_no			= [{0}]", current_page_no);


		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库

			sqlstr_count = "SELECT COUNT(1) FROM TWMA7 WHERE 1=1 ";

			sqlstr = "SELECT * FROM TWMA7 WHERE 1=1 ";

			if (crane_no.Trim() != "")
			{
				sqlstr_temp += " AND CRANE_NO = @crane_no  ";
			}

			if (mat_no.Trim() != "")
			{
				sqlstr_temp += " AND MAT_NO = @mat_no ";
			}
			if (stock_no.Trim() != "")
			{
				sqlstr_temp += " AND STOCK_NO = @stock_no ";
			}
			if (rec_create_time.Trim() != "")
			{
				sqlstr_temp += " AND REC_CREATE_TIME >= @rec_create_time ";
			}
			if (rec_time_to.Trim() != "")
			{
				sqlstr_temp += " AND REC_CREATE_TIME <= @rec_time_to ";
			}
			if (stock_place_no_from.Trim() != "")
			{
				sqlstr_temp += " AND STOCK_PLACE_NO_FROM = @stock_place_no_from ";
			}
			if (stock_place_no_to.Trim() != "")
			{
				sqlstr_temp += " AND STOCK_PLACE_NO_TO = @stock_place_no_to ";
			}

			sqlstr_count = sqlstr_count + sqlstr_temp;
			sqlstr_temp += " ORDER BY CRANE_NO ASC, rec_create_time desc ";
			sqlstr = sqlstr + sqlstr_temp;


			// 设置SQL中的变量
			cmd_inq.Parameters.Set("crane_no", crane_no);
			cmd_inq.Parameters.Set("mat_no", mat_no);
			cmd_inq.Parameters.Set("stock_no", stock_no);
			cmd_inq.Parameters.Set("rec_create_time", rec_create_time);
			cmd_inq.Parameters.Set("rec_time_to", rec_time_to);
			cmd_inq.Parameters.Set("stock_place_no_from", stock_place_no_from);
			cmd_inq.Parameters.Set("stock_place_no_to", stock_place_no_to);
			break;
		}


		Log::Trace("", __FUNCTION__, "sqlstr_temp			= [{0}]", (const char*)sqlstr_temp);
		Log::Trace("", __FUNCTION__, "sqlstr_count		= [{0}]", (const char*)sqlstr_count);
		Log::Trace("", __FUNCTION__, "sqlstr				= [{0}]", (const char*)sqlstr);
		//cmd_inq.Parameters.Clear();
		//if (crane_no.Trim() != "")
		//{
		//	cmd_inq.Parameters.Set("crane_no", crane_no);
		//}
		cmd_inq.SetCommandText(sqlstr_count);

		cd_count = cmd_inq.ExecuteScalar();
		Log::Trace("", __FUNCTION__, "cd_count = [{0}]", cd_count);
		start_row = record_count_per_page * (current_page_no - 1);
		if (start_row > cd_count.ToDouble())
		{
			start_row = 0;
		}
		cmd_inq.Close();
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], start_row, record_count_per_page);
		cmd_inq.Close();



		//返回分页信息 
		bcls_ret->Tables.Add("PAGEINFO");	//增加块
		bcls_ret->Tables["PAGEINFO"].Columns.Add(DT_DECIMAL, "TOTAL_RECORD");						//总记录数
		bcls_ret->Tables["PAGEINFO"].Rows.Add();
		bcls_ret->Tables["PAGEINFO"].Rows[0][0] = cd_count.ToInt32();
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