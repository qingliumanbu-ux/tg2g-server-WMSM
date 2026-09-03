/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2020
Author:      nyy
Version:     1.0
Date:        2024-01-09 13:49:38
Description: 查询材料明细
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(wmsm32_inq1)


int f_wmsm32_inq1(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	try
	{
		//获取输入参数
		CString MISSION_NO = bcls_rec->Tables[0].Rows[0]["MISSION_NO"].ToString().Trim();
		CString PLAN_NO = bcls_rec->Tables[0].Rows[0]["PLAN_NO"].ToString().Trim();
		/*int INDEX_FROM = bcls_rec->Tables[0].Rows[0]["INDEX_FROM"].ToDecimal().ToInt32();
		int RETURN_NUM = bcls_rec->Tables[0].Rows[0]["RETURN_NUM"].ToDecimal().ToInt32();*/
		CDbCommand comm1(conn);
		CString sql = "select  *  from twmsm32m t  where 1=1  and t.MISSION_NO = @MISSION_NO  and t.PLAN_NO = @PLAN_NO  ";
		comm1.Parameters.Set("MISSION_NO", MISSION_NO);
		comm1.Parameters.Set("PLAN_NO", PLAN_NO);
		//连接数据库


		Log::Trace("", "", "MISSION_NO=[{0}]", MISSION_NO);
		Log::Trace("", "", "sql=[{0}]", sql);
		comm1.SetCommandText(sql);

		/*int affectRow = comm1.ExecuteQuery(bcls_ret->Tables[0], INDEX_FROM, RETURN_NUM);*/
		int affectRow = comm1.ExecuteQuery(bcls_ret->Tables[0]);
		//获取总行数
		Log::Trace("", "", "affectRowsrrrr");
		bcls_ret->Tables.Add();
		CDbCommand comm2(conn);
		Log::Trace("", "", "ggggggg");
		comm2.Parameters.Set("MISSION_NO", MISSION_NO);
		comm2.Parameters.Set("PLAN_NO", PLAN_NO);
		//连接数据库
		comm2.SetCommandText(sql);
		Log::Trace("", "", "hhhhh");
		int affectRows = comm2.ExecuteQuery(bcls_ret->Tables[1]);
		Log::Trace("", "", "affectRows  = [{0}] ", affectRows);
		comm2.Close();

		bcls_ret->Tables[0].set_TableName("MATPM46");
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


