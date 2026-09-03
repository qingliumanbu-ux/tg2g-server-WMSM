/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:      李振
Version:     1.0
Date:        2023-12-04 11:35:08
Description: 调拨申请
**************************************************/

#include "stdafx.h"
#include "epex.h"


BM2_FUNCTION_EXPORT
int f_wmsm_t80ry0_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	EPEX epex;

	int doFlag = 0;
	int ret = 0;
	CString s_tc_no = " ";
	CString sqlstr = "";

	//定义表实体对象 
	CModel twm41dj("TWM41DJ");

	CDbCommand comm(conn);

	try
	{
		s_tc_no = "T80RY0";
		twm41dj["C_BATCHUNIT"] = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString();
		twm41dj["C_DELIVERYID"] = bcls_rec->Tables[0].Rows[0]["C_DELIVERYID"].ToString();
		Log::Trace("", __FUNCTION__, "材料	=[{0}]材料	=[{1}]", bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString(), bcls_rec->Tables[0].Rows[0]["C_DELIVERYID"].ToString());
		twm41dj.Query("C_BATCHUNIT,C_DELIVERYID");


		//初始化
		ret = epex.Initialize(s_tc_no);
		if (ret < 0)
		{
			CFormattable arguments[] = { s_tc_no }; // 定义参数列表的数组
			CMessageFormat::Format(s.msg, _RES("YM00S0000514")/*初始化电文[{0}]失败。*/, arguments, 1);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		//拼电文数据
		if (epex.SetValue( "MAT_NO",0, twm41dj["C_BATCHUNIT"].ToString()) < 0
			|| epex.SetValue("DEAL_FLAG", 0, "I") < 0
			|| epex.SetValue("PLANT", 0, twm41dj["C_SENDDEPT"].ToString()) < 0
			|| epex.SetValue("STGE_LOC", 0, twm41dj["C_SENDSTOCK"].ToString()) < 0
			|| epex.SetValue("MOVE_PLANT", 0, twm41dj["C_ACCEPTDEPT"].ToString()) < 0
			|| epex.SetValue("MOVE_STLOC", 0, twm41dj["C_ACCEPTSTOCK"].ToString()) < 0
			|| epex.SetValue("HEAD_TEXT", 0, twm41dj["C_DELIVERYID"].ToString()) < 0)
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

