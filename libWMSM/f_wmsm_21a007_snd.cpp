/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      05501061
Version:     1.0
Date:        2023-11-20 14:28:44
Description: 拒收重车实绩
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2_FUNCTION_EXPORT


int f_wmsm_21a007_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	try
	{
		CTracer log(__FUNCTION__);

		int doFlag = 0;

		CString sqlstr("");

		//电文号
		CString cs_tc_no("");
		//电文变量
		EPEX epex(&s, conn);

		/* 实体类定义 */
		CModel twmsm62 = CModel("TWMSM62");//卸车实绩表

		//系统当前时间
		CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		//判断传入参数是否存在 21A007 表
		if (bcls_rec->Tables.IndexOf("21A007") < 0)
		{
			strncpy(s.msg, (const char*)"传入参数 21A007 表不存在", sizeof(s.msg) - 1);
			s.flag = -1;
			doFlag = -1;
			return doFlag;
		}


		cs_tc_no = "21A007";
		//电文初始化
		if (epex.Initialize(cs_tc_no) < 0)
		{
			strncpy(s.msg, (const char*)"电文初始化失败", sizeof(s.msg) - 1);
			s.flag = -1;
			doFlag = -1;
			return doFlag;
		}
		if (epex.SetValue("ZCHO_GPDYJS",0, twmsm62) < 0)
		{
			sprintf(s.msg, "发送电文失败，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		for (int i = 0; i < bcls_rec->Tables["21A007"].Rows.get_Count(); i++)
		{
			twmsm62.MergeFrom(bcls_rec->Tables["21A007"].Rows[i]);
			if (epex.SetValue("ZCHO_GPDYJS1",i, twmsm62) < 0)
			{
				sprintf(s.msg, "发送电文失败，原因[%s]", epex.GetMsg());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

		}

		//电文发送
		if (epex.SendTele() < 0)
		{
			strncpy(s.msg, (const char*)"电文发送失败", sizeof(s.msg) - 1);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		epex.Uninitialize();
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


