/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      KE2111
Version:     1.0
Date:        2023-11-20 14:28:44
Description: 板坯删除信息电文
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_mmsm99(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);

int f_wmsm_t8p302_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString sqlstr1 = " ";

	//电文号
	CString cs_tc_no("");
	//电文变量
	EPEX epex(&s, conn);

	/* 实体类定义 */
	CModel tmmsm01 = CModel("TMMSM01");
	CModel tqmts29 = CModel("TQMTS29");
	CModel tmmsm96("TMMSM96");
	//
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);

	//系统当前时间
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	EIClass mm0099;
	mm0099.Tables[0].set_TableName("MM0099");
	mm0099.Tables[0].Columns.Add(tmmsm96);
	mm0099.Tables[0].Rows.Clear();
	try
	{
		CTracer log(__FUNCTION__);

		cs_tc_no = "T8P302";
		//电文初始化
		if (epex.Initialize(cs_tc_no) < 0)
		{
			strncpy(s.msg, (const char*)"电文初始化失败", sizeof(s.msg) - 1);
			s.flag = -1;
			doFlag = -1;
			return doFlag;
		}
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			tmmsm96.Reset();
			tmmsm96.CopyFrom(tmmsm01);
			tmmsm96["HR_SEND_FLAG"] = "0";//1--发送0--取消

			tmmsm96["EVENT_ID"] = "MM79";
			tmmsm96["SYSTEM_ID"] = "MMSM";
			tmmsm96["EVENT_LINE_TYPE"] = "00";
			tmmsm96["FUNC_ID"] = s.svc_name;
			tmmsm96.MergeTo(mm0099.Tables["MM0099"], false);

			if (epex.SetValue("VirtSlabID", 0, tmmsm01["PONO_SLAB"].ToString()) < 0//虚拟板坯号
				|| epex.SetValue("SlabID", 0, tmmsm01["SLAB_NO"].ToString()) < 0//板坯号
				|| epex.SetValue("SeqID", 0, i) < 0//板坯号
				|| epex.SetValue("length", 0, 383) < 0
				|| epex.SetValue("ID", 0, "0") < 0
				|| epex.SetValue("time", 0, datetime.Substring(0, 10) + ":" + datetime.Substring(10, 2) + ":" +
					datetime.Substring(12, 2)) < 0
				)
			{
				sprintf(s.msg, "发送电文失败，原因[%s]", epex.GetMsg());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			//电文发送
			if (epex.SendTele() < 0)
			{
				strncpy(s.msg, (const char*)"电文发送失败", sizeof(s.msg) - 1);
				throw CApplicationException(-1, s.msg, s.svc_name);
			}


		}
		if (mm0099.Tables[0].Rows.get_Count() > 0)
		{
			doFlag = f_mmsm99(&mm0099, bcls_ret, conn);
			if (doFlag < 0) {
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}
		epex.Uninitialize();//释放
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


