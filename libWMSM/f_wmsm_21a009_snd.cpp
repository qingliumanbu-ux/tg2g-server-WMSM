/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      05501061
Version:     1.0
Date:        2023-11-23 16:42:10
Description: 汽运、铁路装车电文
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2_FUNCTION_EXPORT


int f_wmsm_21a009_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	int blkNum = 0;
	CString sqlstr = " ";
	CString deal_flag = " ";
	//电文号
	CString cs_tc_no("");
	//电文变量
	EPEX epex(&s, conn);
	/*实体类定义*/
	CModel twmsm61("TWMSM61");
	
	try
	{
		blkNum = bcls_rec->Tables.IndexOf("21A009");
		if (blkNum < 0)
		{
			strcpy(s.msg, "传入数据块21A009不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (bcls_rec->Tables["21A009"].Columns.Contains("DEAL_FLAG"))
		{
			deal_flag = bcls_rec->Tables["21A009"].Rows[0]["DEAL_FLAG"];
		}
		cs_tc_no = "21A009";
		if (epex.Initialize(cs_tc_no) < 0)
		{
			sprintf(s.msg, "初始化电文失败，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, s.svc_name);

		}
		twmsm61.MergeFrom(bcls_rec->Tables["21A009"].Rows[0]);
		if (epex.SetValue("ZCHO_ZCSJ", 0, twmsm61) < 0)
		{
			sprintf(s.msg, "发送电文失败，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (epex.SetValue("ZCHO_ZCSJ", "DEAL_FLAG", 0, deal_flag) < 0)
		{
			sprintf(s.msg, "系统出现异常，DEAL_FLAG电文拼接出错");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		for (int i = 0; i < bcls_rec->Tables["21A009"].Rows.get_Count(); i++)
		{

			twmsm61.MergeFrom(bcls_rec->Tables["21A009"].Rows[i]);
			twmsm61.Query("PRACTICE_NO,MAT_NO");
			
			if (epex.SetValue("ZCHO_ZCSJ1", i, twmsm61) < 0)
			{
				sprintf(s.msg, "发送电文失败，原因[%s]", epex.GetMsg());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			
		}
		if (epex.SendTele() < 0)
		{
			sprintf(s.msg, "电文发送失败");
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


