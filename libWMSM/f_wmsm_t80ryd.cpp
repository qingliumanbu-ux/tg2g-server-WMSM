/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:      李振
Version:     1.0
Date:        2023-12-04 11:35:08
Description: 材料处置
**************************************************/

#include "stdafx.h"
#include "epex.h"

int f_mmsm99(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);		//物料跟踪函数

BM2_FUNCTION_EXPORT
int f_wmsm_t80ryd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	EPEX epex;

	int doFlag = 0;
	int ret = 0;
	CString s_tc_no = "T80RYD";
	CString sqlstr = "";

	//定义表实体对象 
	CModel tmmsm01("TMMSM01");
	CModel tmmsm96("TMMSM96");
	CModel tqmtst802("TQMTST802");
	CDbCommand comm(conn);

	if (bcls_rec->Tables.IndexOf("MM0099") < 0)
	{
		bcls_rec->Tables.Add("MM0099");
		bcls_rec->Tables["MM0099"].Columns.Add(tmmsm96);
		bcls_rec->Tables["MM0099"].Rows.Clear();
	}

	try
	{
		tmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tqmtst802.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tqmtst802.Query("SLAB_NO");

		//初始化
		ret = epex.Initialize(s_tc_no);
		if (ret < 0)
		{
			CFormattable arguments[] = { s_tc_no }; // 定义参数列表的数组
			CMessageFormat::Format(s.msg, _RES("YM00S0000514")/*初始化电文[{0}]失败。*/, arguments, 1);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		//拼电文数据
		if (epex.SetValue("MAT_NO", 0, tmmsm01["MAT_NO"].ToString()) < 0
			|| epex.SetValue("plant_no", 0, "6240") < 0
			|| epex.SetValue("deal_flag", 0, "6") < 0
			|| epex.SetValue("hold_cause_code", 0, tqmtst802["SAP_ERP_MARK_7"].ToString()) < 0
			|| epex.SetValue("hold_time", 0, CDateTime::Now().ToString("yyyyMMddHHmmss")) < 0
			|| epex.SetValue("hold_maker", 0, s.userid) < 0
			|| epex.SetValue("hold_remark", 0, tqmtst802["REMARK"].ToString()) < 0)
		{
			strcpy(s.msg, _RES("GCRSS0000015"));//系统出现异常，电文拼接出错，请联系系统维护人员。
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		if (epex.SendTele() < 0)
		{
			strcpy(s.msg, _RES("GCRSS0000032")/*电文发送失败。*/);
			sprintf(s.sysmsg, "[%s]发送失败", (const char*)s_tc_no);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		epex.Uninitialize();
		tmmsm01.Query("MAT_NO");
		tmmsm96.CopyFrom(tmmsm01);
		tmmsm96["EVENT_ID"] = "MM07";
		tmmsm96["EVENT_LINE_TYPE"] = "SM";
		tmmsm96["FUNC_ID"] = s.formname;
		tmmsm96["SYSTEM_ID"] = "MMSM";
		tmmsm96["MNG_HOLD_CAUSE_CODE"] = tqmtst802["SAP_ERP_MARK_7"].ToString();
		tmmsm96["MNG_HOLD_REMARK"] = tqmtst802["REMARK"].ToString();
		tmmsm96["EVENT_DESC"] = "炼钢侧材料管理封锁";

		tmmsm96.MergeTo(bcls_rec->Tables["MM0099"], false);
		doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000021")/*信息读取失败。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1; //数据库异常时返回-1，事务将被回滚
	}
	catch (const CApplicationException& ex)
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

	return(doFlag);
}

