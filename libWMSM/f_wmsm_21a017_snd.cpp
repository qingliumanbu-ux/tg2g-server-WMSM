/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:      李振
Version:     1.0
Date:        2023-11-17 11:35:08
Description: 铁路装车线可走车
**************************************************/

#include "stdafx.h"
#include "epex.h"


BM2_FUNCTION_EXPORT
int f_wmsm_21a017_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
		s_tc_no = "21A017";
		


		//初始化
		ret = epex.Initialize(s_tc_no);
		if (ret < 0)
		{
			CFormattable arguments[] = { s_tc_no }; // 定义参数列表的数组
			CMessageFormat::Format(s.msg, _RES("YM00S0000514")/*初始化电文[{0}]失败。*/, arguments, 1);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		//拼电文数据
		if (epex.SetValue("ZCHO_KZC","DEAL_FLAG", 0, "I") < 0)
		{
			strcpy(s.msg, _RES("GCRSS0000015"));//系统出现异常，电文拼接出错，请联系系统维护人员。
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		for (int i = 0; bcls_rec->Tables["ZCHO_KZC"].Rows.get_Count(); i++) {
			if (epex.SetValue("ZCHO_KZC1", "WAGONNO", i, bcls_rec->Tables["ZCHO_KZC"].Rows[i]["WAGONNO"].ToString()) < 0
				)
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

