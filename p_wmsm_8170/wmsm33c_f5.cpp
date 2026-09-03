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
BM2F_ENTERACE(wmsm33c_f5)

int f_wmsm33c_f5(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
		tqmts29["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO_1"].ToString();
		if (tqmts29.QueryCount("HEAT_NO") == 0)
		{
			sprintf(s.msg, "该炉次[%s]没有代表成分，不能用于生成新代表成分!", (const char*)tqmts29["HEAT_NO"].ToString());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		tqmts29_o.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsm33c.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		if (tqmts29_o.QueryCount("HEAT_NO") > 0)
		{
			sprintf(s.msg, "炉号[%s]已有代表成分，不可重复新增!", (const char*)tqmts29_o["HEAT_NO"].ToString());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		
		sqlstr = " SELECT * FROM TQMTS29 WHERE HEAT_NO='"+ tqmts29["HEAT_NO"].ToString() +"' ";

		Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			tqmts29.Reset();
			cmd_inq.Fetch(tqmts29);
			tqmts29["REC_CREATOR"] = "MMSM33C";
			tqmts29["REC_CREATE_TIME"] = datetime;
			tqmts29["HEAT_NO"] = tqmts29_o["HEAT_NO"].ToString();
			tqmts29["PONO"] = tqmts29_o["HEAT_NO"].ToString();
			tqmts29["ST_SAMPLE_NO"] = tqmts29_o["HEAT_NO"].ToString() + "-" + tmmsm33c["UNIT_CODE"].ToString() + "T-1#-1";
			tqmts29["ST_NO"] = tmmsm33c["ST_NO"].ToString();
			tqmts29.Insert();
		}
		cmd_inq.Close();

		tmmsm33c["REP_ELM_SEL_FLAG"] = "1";
		tmmsm33c["REC_REVISOR"] = s.userid;
		tmmsm33c["REC_REVISE_TIME"] = datetime;
		tmmsm33c.Update("REP_ELM_SEL_FLAG,REC_REVISOR,REC_REVISE_TIME", "HEAT_NO");



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
