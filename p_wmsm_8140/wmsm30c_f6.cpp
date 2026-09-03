/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2024
Author:      nyy
Version:     1.0
Date:        2024-01-9 13:10:05
Description: 新增非销售出厂计划材料明细
**************************************************/

#include "stdafx.h"
//函数申明
BM2_FUNCTION_IMPORT
int f_wmsm_21a019_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
BM2F_ENTERACE(wmsm30c_f6)
CString f_ts00_get_new_guid();

int f_wmsm30c_f6(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	//实体类定义
	CModel twmsm30 = CModel("TWMSM30");
	CModel twmsm30m = CModel("TWMSM30M");
	CModel tmmsm01 = CModel("TMMSM01");
	try
	{
		
		twmsm30.Reset();
		twmsm30.MergeFrom(bcls_rec->Tables[1].Rows[0]);
		twmsm30.TrimOrBlank();
		twmsm30.Query("PLAN_NO");
		if (twmsm30["STATUS"].ToString().Trim() > '2')
		{
			strcpy(s.msg, "计划审核之后不能添加材料");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		// 传入块中第一个表的行数
		int rowCount1 = bcls_rec->Tables[0].Rows.get_Count();	
		Log::Trace("", "", "rowCount1=[{0}]", rowCount1);	
		//新增
		if (rowCount1 >0)
		{
			for (int i = 0; i < rowCount1; i++)
			{
				
				twmsm30m.Reset();				
				twmsm30m.MergeFrom(bcls_rec->Tables[0].Rows[i]);
				
				Log::Trace("", "", "00000000000000000000");
				twmsm30m.TrimOrBlank();
				tmmsm01["MAT_NO"] = twmsm30m["MAT_NO"].ToString().Trim();
				Log::Trace("", "", "111111111111111");
				tmmsm01.Query("MAT_NO");
				//设置记录者信息
				twmsm30m["REC_CREATOR"] = s.userid;
				twmsm30m["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				twmsm30m["PLAN_NO"] = twmsm30["PLAN_NO"].ToString();
				twmsm30m["STATUS"] = " ";
				twmsm30m["SG_SIGN"] = tmmsm01["PREC_ST_NO"].ToString();
				twmsm30m["MODEL_SIZE"] = " ";
				twmsm30m["MAT_WT"] = tmmsm01["MAT_ACT_WT"].ToString();
				twmsm30m["MAT_WIDTH"] = tmmsm01["MAT_WIDTH"].ToString();
				twmsm30m["MAT_LEN"] = tmmsm01["MAT_LEN"].ToString();
				twmsm30m["MAT_THICK"] = tmmsm01["MAT_THICK"].ToString();
				twmsm30m["MATERIAL_NAME"] = " ";
				twmsm30m["FACTORY_DIV1"] = twmsm30["FACTORY_DIV1"].ToString();
				twmsm30m["AREA_CODE"] = twmsm30["LOAD_CODE_AREA"].ToString();
				twmsm30m["LOAD_CODE"] = twmsm30["LOAD_CODE"].ToString();
				twmsm30m["PROD_ORDER_NO"] = tmmsm01["ORDER_NO"].ToString();
				twmsm30m["REMARK"] = " ";
				twmsm30m["REMARK1"] = " ";
				twmsm30m["REMARK2"] = " ";
				twmsm30m["MATERIAL_CODE1"] = " ";
				twmsm30m["STORE_PLACE"] = tmmsm01["STOCK_PLACE_NO"].ToString();
				twmsm30m.Insert();
				
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


