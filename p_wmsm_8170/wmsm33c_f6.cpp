/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     lizhen
Version:    1.0
Date:       2025-7-16
Description: 成品大炉号按钢种新增成分
**************************************************/
//框架头文件
#include "stdafx.h"

int f_210010_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);



// service入口
BM2F_ENTERACE(wmsm33c_f6)

int f_wmsm33c_f6(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
		
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tqmts29.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tmmsm33c.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			tqmts29["REC_REVISOR"] = s.userid;
			tqmts29["REC_REVISE_TIME"] = datetime;
			tqmts29.Update("REC_REVISOR,REC_REVISE_TIME,ELM_ACT", "HEAT_NO,ST_SAMPLE_NO,ELM_CODE");
		}
		
		


		EIClass bcls_rec_xh;
		bcls_rec_xh.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
		bcls_rec_xh.Tables[0].Rows.Add();
		bcls_rec_xh.Tables[0].Rows[0]["HEAT_NO"] = tqmts29["HEAT_NO"].ToString();

		doFlag = f_210010_snd(&bcls_rec_xh, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}



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
