/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2024
Author:      nyy
Version:     1.0
Date:        2024-01-9 13:10:05
Description: 非销售出厂计划关闭，并发送物流系统
**************************************************/

#include "stdafx.h"
//函数申明
BM2_FUNCTION_IMPORT
int f_wmsm_21a019_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
BM2F_ENTERACE(wmsm30c_f8)

int f_wmsm30c_f8(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString plan_no = " ";
	//实体类定义
	CModel twmsm30 = CModel("TWMSM30");
	CModel twmsm30m = CModel("TWMSM30M");
	CModel tmmsm01 = CModel("TMMSM01");
	try
	{
		if (!bcls_rec->Tables.Contains("21A019"))
		{
			bcls_rec->Tables.Add("21A019");
			bcls_rec->Tables["21A019"].Columns.Add(DT_STRING, "PLAN_NO");
			bcls_rec->Tables["21A019"].Columns.Add(DT_STRING, "DEAL_FLAG");
		}

		// 传入块中第一个表的行数
		int rowCount1 = bcls_rec->Tables[0].Rows.get_Count();
		Log::Trace("", "", "rowCount1=[{0}]", rowCount1);
		//新增
		if (rowCount1 >0)
		{
			for (int i = 0; i < rowCount1; i++)
			{
				plan_no = bcls_rec->Tables[0].Rows[i]["PLAN_NO"].ToString();
				twmsm30["PLAN_NO"] = plan_no;
				twmsm30.Query("PLAN_NO");
				/*if (twmsm30["STATUS"].ToString() == "3")
				{
					sprintf(s.msg, "已审核的计划{0}不可关闭，操作失败！", (const char*)plan_no);
					throw CApplicationException(-1, s.msg, s.svc_name);
				}*/
				if (twmsm30["STATUS"].ToString() == "4")
				{
					sprintf(s.msg, "已完成的计划{0}不可关闭，操作失败！", (const char*)plan_no);
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				twmsm30m["PLAN_NO"] = twmsm30["PLAN_NO"].ToString();
				if (twmsm30m.QueryCount("PLAN_NO") <= 0)
				{
					sprintf(s.msg, "计划{0}的材料明细未维护，如不需要请删除计划!", (const char*)plan_no);
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				twmsm30["STATUS"] = "5";
				twmsm30["REC_REVISE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				twmsm30["REC_REVISOR"] = s.userid;
				twmsm30.Update("STATUS,REC_REVISE_TIME,REC_REVISOR", "PLAN_NO");
				bcls_rec->Tables["21A019"].Rows.Add();
				bcls_rec->Tables["21A019"].Rows[i]["PLAN_NO"] = bcls_rec->Tables[0].Rows[i]["PLAN_NO"].ToString();
			}
			bcls_rec->Tables["21A019"].Rows[0]["DEAL_FLAG"] = "C";
			doFlag = f_wmsm_21a019_snd(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
			throw CApplicationException(-1, s.msg, log.Location);
			}
		}
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


