/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   KE2111
Version:
Date:     2024/11/15
Description: 查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"


/***** C++ 的业务头文件部分 *****/

// service入口
BM2F_ENTERACE(wmsmshow_init)

int f_wmsmshow_init(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";

	CDbCommand cmd_inq(conn);



	try
	{

		sqlstr = " select * from twmsmfoRm where 1=1 and VIEW_FLAG!='F' order by IDX_REQ ";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();

		bcls_ret->Tables.Add("CON");
		sqlstr = " select * from twmsmformcon ";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables["CON"]);
		cmd_inq.Close();
	}
	catch (CDbException& ex) // 捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006") /*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1; // 数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex) // 捕获应用错误
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
