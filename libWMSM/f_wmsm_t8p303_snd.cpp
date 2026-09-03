/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      KE2111
Version:     1.0
Date:        2023-11-20 14:28:44
Description: 板坯去向电文
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2_FUNCTION_EXPORT


int f_wmsm_t8p303_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
	//
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);

	//系统当前时间
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	try
	{
		CTracer log(__FUNCTION__);

		cs_tc_no = "T8P303";
		//电文初始化
		if (epex.Initialize(cs_tc_no) < 0)
		{
			strncpy(s.msg, (const char*)"电文初始化失败", sizeof(s.msg) - 1);
			s.flag = -1;
			doFlag = -1;
			return doFlag;
		}
		tmmsm01["MAT_NO"] = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString();
		tmmsm01.Query("MAT_NO");
		CString v_guide_dest = Db::QueryCString(" select CODE_DESC_1_CONTENT from TWMSMZD02 where CODE_CLASS = 'WM02' and CODE ='" + tmmsm01["GUIDE_DEST"].ToString() + "' ");

		if ( epex.SetValue("ZCHO_2250BPQX","MARKERNUMBER", 0, tmmsm01["SLAB_NO"].ToString()) < 0//板坯号
			|| epex.SetValue("ZCHO_2250BPQX", "PONUMBER", 0, tmmsm01["ORDER_NO"].ToString()) < 0//合同号
			|| epex.SetValue("ZCHO_2250BPQX", "CUTTIME", 0, tmmsm01["SLAB_CUT_TIME"].ToString()) < 0//切割时间
			|| epex.SetValue("ZCHO_2250BPQX", "MATNO", 0, tmmsm01["MAT_CODE"].ToString()) < 0//物料编码
			|| epex.SetValue("ZCHO_2250BPQX", "BATCHNUMBER", 0, tmmsm01["MAT_NO"].ToString()) < 0//材料号
			|| epex.SetValue("ZCHO_2250BPQX", "STEELGRADE", 0, tmmsm01["ST_NO"].ToString()) < 0//内部钢种
			|| epex.SetValue("ZCHO_2250BPQX", "WIDTH", 0, tmmsm01["MAT_WIDTH"].ToString()) < 0//宽
			|| epex.SetValue("ZCHO_2250BPQX", "WIDTHTAIL", 0, tmmsm01["SLAB_TAIL_WIDTH"].ToString()) < 0//尾宽
			|| epex.SetValue("ZCHO_2250BPQX", "WIDTHHEAD", 0, tmmsm01["SLAB_HEAD_WIDTH"].ToString()) < 0//头宽
			|| epex.SetValue("ZCHO_2250BPQX", "SLABLENGTH", 0, tmmsm01["MAT_LEN"].ToString()) < 0//长
			|| epex.SetValue("ZCHO_2250BPQX", "THICKNESS", 0, tmmsm01["MAT_THICK"].ToString()) < 0//厚
			|| epex.SetValue("ZCHO_2250BPQX", "WEIGHT", 0, tmmsm01["MAT_WT"].ToString()) < 0//重量
			|| epex.SetValue("ZCHO_2250BPQX", "PLANDIRECTION", 0, v_guide_dest) < 0//计划流向
			|| epex.SetValue("ZCHO_2250BPQX", "CX_MAT_NO", 0, tmmsm01["PONO_SLAB"].ToString()) < 0//材料号
			|| epex.SetValue("ZCHO_2250BPQX", "SG_GRADE_1", 0, tmmsm01["SG_GRADE_1"].ToString()) < 0//钢种
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


