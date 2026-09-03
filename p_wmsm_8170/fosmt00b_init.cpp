/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2010
Author:			zhouyueqi
Version:		1.0
Date:			2023年5月24日
Description:	炼钢全厂指示初始化
Update:
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"

/* ***** 静态函数申明 ***** */

BM2F_ENTERACE(fosmt00b_init)
int f_fosmt00b_init(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_where = "";
	CString sqlstr_order = "";
	int	TotalRecordCount = 0;

	CPageInfo pageInfo;
	CDbCommand cmd_inq(conn);

	CDecimal type = 0;
	CString tbl = "";
	CString date_time = "";


	try
	{
		CDateTime datetime = CDateTime::Now();

		//校验传参
		if (bcls_rec->Tables[0].Columns.Contains("TYPE"))
		{
			type = bcls_rec->Tables[0].Rows[0]["TYPE"].ToDecimal();
		}
		if (bcls_rec->Tables[0].Columns.Contains("DATE_TIME"))
		{
			date_time = bcls_rec->Tables[0].Rows[0]["DATE_TIME"].ToString();
		}

		//页面标题标签
		if (type == 0)
		{
			tbl = "TFOSMT00";
			bcls_ret->Tables.Add(tbl);

			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr =
					" SELECT ITEM_ENAME, REMARK, PAGE_TYPE"
					" FROM TWMSMFOSM00 WHERE 1 = 1 ORDER BY PAGE_TYPE, ITEM_ENAME"
					;
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
			cmd_inq.Close();
		}
		else
		{
			//tbl = "TFOSMT91";
			//bcls_ret->Tables.Add(tbl);

			//switch (conn->DatabaseKind)
			//{
			//case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			//case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			//case DB_KIND_MSSQL:				// MS SQL Server数据库
			//case DB_KIND_ORACLE:	        // Oracle 数据库
			//default:
			//	sqlstr =
			//		" SELECT *"
			//		" FROM TFOSMT91"
			//		" WHERE DATE_TIME = @DATE_TIME"
			//		" ORDER BY SEQ_NO"
			//		;
			//	break;
			//}
			//cmd_inq.SetCommandText(sqlstr);
			//cmd_inq.Parameters.Set("DATE_TIME", date_time);
			//cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
			//cmd_inq.Close();
		}

		//返回提示栏信息
		CString ts = ((CDecimal)(CDateTime::Now() - datetime).TotalMilliseconds()).Round(0).ToString();
		CFormattable arguments[] = { ts };
		CMessageFormat::Format(s.msg, "数据读取成功！SVC用时[{0}ms]", arguments, 1);
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
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
