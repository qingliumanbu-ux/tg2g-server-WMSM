/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2020
Author:      nyy
Version:     1.0
Date:        2024-01-09 13:49:38
Description: ²éÑ¯²ÄÁÏÃ÷Ï¸
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(wmsm32_mat_inq)


int f_wmsm32_mat_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	try
	{
		Log::Trace("", "", "ffffff=[{0}]");
		CString PLAN_NO = bcls_rec->Tables[1].Rows[0]["PLAN_NO"].ToString().Trim();


		Log::Trace("", "", "PLAN_NO=[{0}]", PLAN_NO);
		CString sql = " select * from twmsm30m t  where  t.STATUS<'3'";
		CDbCommand comm1(conn);
		if (!PLAN_NO.IsEmpty())
		{
			sql += " and PLAN_NO = @PLAN_NO ";
			comm1.Parameters.Set("PLAN_NO", PLAN_NO);
		}
		else
		{
			sql += " and PLAN_NO = ' ' ";

		}
		
		comm1.SetCommandText(sql);
		Log::Trace("", __FUNCTION__, "sql = [{0}]", sql);
		int affectRow = comm1.ExecuteQuery(bcls_ret->Tables[0]);
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


