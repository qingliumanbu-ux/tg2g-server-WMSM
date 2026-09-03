/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     lizhen
Version:    1.0
Date:       2025-7-16
Description: 成品大炉号删除
**************************************************/
//框架头文件
#include "stdafx.h"


int f_mmsm99(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);


// service入口
BM2F_ENTERACE(wmsm33c_f3)

int f_wmsm33c_f3(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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



	CDbCommand cmd_inq(conn);
	CModel tmmsm01("TMMSM01");
	CModel tmmsm96("TMMSM96");
	
	CModel tmmsm33c("TMMSM33C");
	
	try
	{
		CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		//调用物料事件
		EIClass mm0099;
		mm0099.Tables[0].set_TableName("MM0099");
		mm0099.Tables[0].Columns.Add(tmmsm96);
		mm0099.Tables[0].Rows.Clear();

		

		tmmsm33c.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsm33c.Query("HEAT_NO");
		CString v_mat = Db::QueryCString("SELECT COUNT(1) FROM VMMSM01 WHERE HEAT_NO='" + tmmsm33c["HEAT_NO"].ToString() + "' AND RCV_MAT_FLAG='S'");
		if (v_mat !="0")
		{
			strcpy(s.msg, "该炉号还有未取消收货的材料，不能删除!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (tmmsm33c["HEAT_USE_FLAG"].ToString().Trim() != "")
		{
			strcpy(s.msg, "炉次消耗未冲销，不能删除!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (tmmsm33c["STEEL_AMOUNT_FLAG"].ToString().Trim() != "")
		{
			strcpy(s.msg, "过钢量未冲销，不能删除!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		sqlstr = " SELECT * FROM tmmsm01 WHERE HEAT_NO='" + tmmsm33c["HEAT_NO"].ToString() + "'  ";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			cmd_inq.Fetch(tmmsm96);
			tmmsm96["EVENT_ID"] = "MM04";
			tmmsm96["EVENT_LINE_TYPE"] = "SM";
			tmmsm96["SYSTEM_ID"] = "MMSM";
			tmmsm96["FUNC_ID"] = "wmsm33c_f3";
			tmmsm96.MergeTo(mm0099.Tables["MM0099"], false);
		}
		//调用物料事件
		if (mm0099.Tables["MM0099"].Rows.get_Count() > 0)
		{
			doFlag = f_mmsm99(&mm0099, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}

		tmmsm33c.Delete("HEAT_NO");

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
