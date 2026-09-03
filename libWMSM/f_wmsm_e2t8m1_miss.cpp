/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      李振
Version:     1.0
Date:        2024-01-17
Description: 制造出库
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2_FUNCTION_EXPORT


int f_wmsm_e2t8m1_miss(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	int blkNum = 0;
	CString sqlstr = " ";
	//电文号
	CString cs_tc_no("");
	//电文变量
	EPEX epex(&s, conn);
	/* 实体类定义 */
	CModel tmmsm01("TMMSM01");
	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString s_location = " ";

	try
	{
		CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


		/* 判断是否存在指定块 */
		blkNum = bcls_rec->Tables.IndexOf("E2T8M1");
		if (blkNum < 0)
		{
			strcpy(s.msg, "传入数据块 E2T8M1 不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		cs_tc_no = "T8E2M1";
		//电文初始化

		for (int i = 0; i < bcls_rec->Tables["E2T8M1"].Rows.get_Count(); i++)
		{
			if (epex.Initialize(cs_tc_no) < 0)
			{
				strncpy(s.msg, (const char*)"电文初始化失败", sizeof(s.msg) - 1);
				s.flag = -1;
				doFlag = -1;
				return doFlag;
			}
			tmmsm01["MAT_NO"] = bcls_rec->Tables["E2T8M1"].Rows[i]["MAT_NO"].ToString();
			if (!tmmsm01.Query("MAT_NO"))
			{
				strcpy(s.msg, "材料号 不存在。");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			//s_location = tmmsm01["STOCK_L2"].ToString();
			if (tmmsm01["SLAB_CUT_TIME"].ToString().GetLength() == 0)
			{
				tmmsm01["SLAB_CUT_TIME"] = dateNow;
			}
			if (tmmsm01["C_DELIVERY_FAC"].ToString().Trim() != "")
			{
				s_location = tmmsm01["C_DELIVERY_FAC"].ToString();
			}
			if (epex.SetValue("INT_SYM_SLAB_REPORT", "AGGREGATE_NAME", 0, tmmsm01["DEV_CODE"].ToString()) < 0
				|| epex.SetValue("INT_SYM_SLAB_REPORT", "SLAB_NUMBER", 0, tmmsm01["SLAB_NO"].ToString()) < 0
				|| epex.SetValue("INT_SYM_SLAB_REPORT", "VIRTUAL_SLAB_ID", 0, tmmsm01["LSLAB_NO"].ToString()) < 0
				|| epex.SetValue("INT_SYM_SLAB_REPORT", "MARKING_NUMBER", 0, tmmsm01["SLAB_NO"].ToString()) < 0
				|| epex.SetValue("INT_SYM_SLAB_REPORT", "GRADE", 0, tmmsm01["ST_NO"].ToString()) < 0
				|| epex.SetValue("INT_SYM_SLAB_REPORT", "LOCATION", 0, "STC2") < 0
				|| epex.SetValue("INT_SYM_SLAB_REPORT", "ACTUAL_LENGTH", 0, (tmmsm01["MAT_LEN"].ToDecimal() / 1000).Round(3)) < 0
				|| epex.SetValue("INT_SYM_SLAB_REPORT", "ACTUAL_THICKNESS", 0, (tmmsm01["MAT_THICK"].ToDecimal() / 1000).Round(3)) < 0
				|| epex.SetValue("INT_SYM_SLAB_REPORT", "WIDTH_HEAD", 0, (tmmsm01["SLAB_HEAD_WIDTH"].ToDecimal() / 1000).Round(3)) < 0
				|| epex.SetValue("INT_SYM_SLAB_REPORT", "WIDTH_TAIL", 0, (tmmsm01["SLAB_TAIL_WIDTH"].ToDecimal() / 1000).Round(3)) < 0
				|| epex.SetValue("INT_SYM_SLAB_REPORT", "WEIGHT", 0, tmmsm01["MAT_WT"].ToDecimal() * 1000) < 0
				|| epex.SetValue("INT_SYM_SLAB_REPORT", "BATCHNO", 0, tmmsm01["BATCH"].ToString()) < 0
				|| epex.SetValue("INT_SYM_SLAB_REPORT", "SLAB_CUT_TIME", 0, tmmsm01["SLAB_CUT_TIME"].ToString().Trim() == "" ? dateNow : tmmsm01["SLAB_CUT_TIME"].ToString()) < 0
				|| epex.SetValue("INT_SYM_SLAB_REPORT", "TIME_STAMP", 0, dateNow) < 0)
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
			else
			{
				Log::Trace("", __FUNCTION__, "发送电文成功");
			}
			epex.Uninitialize();
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


