/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2024
Author:      lz
Version:     1.0
Date:        2024-01-9 13:10:05
Description: 熔炼号转变
**************************************************/

#include "stdafx.h"
//函数申明
BM2_FUNCTION_IMPORT


BM2F_ENTERACE(wmsmturn_inq)


int f_wmsmturn_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CDbCommand cmd_inq(conn);
	//实体类定义
	CModel tmmsm01("TMMSM01");
	CModel hmmsm01("HMMSM01");
	try
	{
		sqlstr = " SELECT * FROM TMMSM01 WHERE HEAT_NO='" + bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString() + "' ";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();

		bcls_ret->Tables.Add();
		sqlstr = " SELECT * FROM TWMSMTURN WHERE 1=1 ORDER BY REC_CREATE_TIME DESC ";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[1]);
		cmd_inq.Close();

	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}


