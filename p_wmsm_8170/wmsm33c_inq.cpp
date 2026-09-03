/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     lizhen
Version:    1.0
Date:       2025-7-16
Description: 成品大炉号查询
**************************************************/
//框架头文件
#include "stdafx.h"


// service入口
BM2F_ENTERACE(wmsm33c_inq)

int f_wmsm33c_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int		TotalRecordCount = 0;
	//排产钢种查当前时间前6h--后24h
	//CString v_start_time = CDateTime::Now().AddHours(-6).ToString("yyyyMMddHHmmss");
	//CString v_end_time = CDateTime::Now().AddDays(1).ToString("yyyyMMddHHmmss");
	CString v_station_id = "";

	//系统的分页类信息。
	CPageInfo pageInfo;


	CDbCommand cmd_inq(conn);

	try
	{
		

		sqlstr = " select A.*,(SELECT COUNT(1) FROM VMMSM01 WHERE VMMSM01.HEAT_NO=A.HEAT_NO) COUNT_NUM from TMMSM33C A where 1=1 ";
		if (bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim() != "")
		{
			sqlstr += " and A.heat_no='" + bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString() + "' ";
		}

				

		Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);

		//分页获取
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();

		

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
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
