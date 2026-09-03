/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:      李振
Version:     1.0
Date:        2023-11-17 11:35:08
Description: 汽运、铁路卸车确认
**************************************************/

#include "stdafx.h"
#include "epex.h"


BM2_FUNCTION_EXPORT
int f_wmsm_21a010_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	EPEX epex;

	int doFlag = 0;
	int ret = 0;
	CString s_tc_no = " ";
	CString sqlstr = "";

	//定义表实体对象 
	CModel tmmsm96("TMMSM96");
	CModel twmsm62("TWMSM62");

	CDbCommand comm(conn);

	try
	{
		s_tc_no = "21A010";
		twmsm62.MergeFrom(bcls_rec->Tables["ZCHO"].Rows[0]);


		//初始化
		ret = epex.Initialize(s_tc_no);
		if (ret < 0)
		{
			CFormattable arguments[] = { s_tc_no }; // 定义参数列表的数组
			CMessageFormat::Format(s.msg, _RES("YM00S0000514")/*初始化电文[{0}]失败。*/, arguments, 1);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		//拼电文数据
		if (epex.SetValue("ZCHO_XCSJ",0, twmsm62) < 0)
		{
			strcpy(s.msg, _RES("GCRSS0000015"));//系统出现异常，电文拼接出错，请联系系统维护人员。
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		for (int i = 0; i<bcls_rec->Tables["ZCHO"].Rows.get_Count(); i++) {
			if (epex.SetValue("ZCHO_XCSJ1", "MAT_NO",i, bcls_rec->Tables["ZCHO"].Rows[i]["MAT_NO"].ToString()) < 0
				|| epex.SetValue("ZCHO_XCSJ1", "SG_SIGN", i, bcls_rec->Tables["ZCHO"].Rows[i]["SG_SIGN"].ToString()) < 0
				|| epex.SetValue("ZCHO_XCSJ1", "MATERIAL_CODE", i, bcls_rec->Tables["ZCHO"].Rows[i]["MATERIAL_CODE"].ToString()) < 0
				|| epex.SetValue("ZCHO_XCSJ1", "LENGTH", i, bcls_rec->Tables["ZCHO"].Rows[i]["LENGTH"].ToDecimal()) < 0
				|| epex.SetValue("ZCHO_XCSJ1", "WIDTH", i, bcls_rec->Tables["ZCHO"].Rows[i]["WIDTH"].ToDecimal()) < 0
				|| epex.SetValue("ZCHO_XCSJ1", "THICK", i, bcls_rec->Tables["ZCHO"].Rows[i]["THICK"].ToDecimal()) < 0
				|| epex.SetValue("ZCHO_XCSJ1", "WEIGHT", i, bcls_rec->Tables["ZCHO"].Rows[i]["WEIGHT"].ToDecimal()) < 0
				|| epex.SetValue("ZCHO_XCSJ1", "DEDUCT_WEIGHT", i, bcls_rec->Tables["ZCHO"].Rows[i]["DEDUCT_WEIGHT"].ToDecimal()) < 0)
			{
				strcpy(s.msg, _RES("GCRSS0000015"));//系统出现异常，电文拼接出错，请联系系统维护人员。
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}

		if (epex.SendTele() < 0)
		{
			strcpy(s.msg, _RES("GCRSS0000032")/*电文发送失败。*/);
			sprintf(s.sysmsg, "[%s]发送失败", (const char*)s_tc_no);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}


		epex.Uninitialize();

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

