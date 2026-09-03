/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     lizhen
Version:    1.0
Date:       2025-7-16
Description: 成品大炉号按钢种新增成分
**************************************************/
//框架头文件
#include "stdafx.h"





// service入口
BM2F_ENTERACE(wmsm33c_f7)

int f_wmsm33c_f7(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";


	CString v_station_id = "";
	CString v_resume_seq_no = "";//序号
	CString v_shll_seq = "";//收货履历序号

	CModel tqmts29("TQMTS29");
	CModel tqmts29_o("TQMTS29");
	CModel tmmsm33c("TMMSM33C");

	CDbCommand cmd_inq(conn);


	try
	{
		CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		
		tqmts29.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsm33c.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		

		
		tqmts29.Delete("HEAT_NO");
		
		tmmsm33c["REP_ELM_SEL_FLAG"] = " ";
		tmmsm33c["REC_REVISOR"] = s.userid;
		tmmsm33c["REC_REVISE_TIME"] = datetime;
		tmmsm33c.Update("REP_ELM_SEL_FLAG,REC_REVISOR,REC_REVISE_TIME", "HEAT_NO");



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
