/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         LIZHEN
Version:		1.0
Date:			2023-11-07
Description:	板坯取消匹配
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件

int f_mmsm99(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_t8z_23m_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//发送专家系统数据

//函数申明

/*<remark>=========================================================

===========================================================</remark>*/

BM2F_ENTERACE(wmsm01q0_tuo);

int f_wmsm01q0_tuo(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;


	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */
	//CTWM06 twm06(conn);
	CModel twm06("TWM06");

	/* 数据库SQL操作字符串 */
	CString sql = "";
	CString sqlstr = "";
	CString sqlwhere = "";
	CString sqlstr_count;
	CString sqlstr_temp;

	/* 业务变量 */
	CString	datetime("");
	CString	cs_ladle_no("");
	CString	s_heat_no("");
	CDecimal cd_count = 0;
	int	record_count_per_page = 0; /* 每页记录数 */
	int	current_page_no = 0; /* 需查询的页号,从0开始计数 */
	int	start_row = 0; /* 将要压入outBlock的起始行 */


	/* 全局变量 */
	CString crane_no = "";
	CString stock_place_no_from = "";
	CString stock_place_no_to = "";
	/* 数据库操作类定义 */
	CModel tmmsm01("TMMSM01");
	CModel tmmsm96("TMMSM96");
	CModel tpssm03("TPSSM03");
	CDbCommand cmd_inq(conn);

	//系统的分页类信息。
	CPageInfo pageInfo;

	EIClass in_23m;
	in_23m.Tables[0].Columns.Add(DT_STRING, "TC_NO");
	in_23m.Tables[0].Rows.Add();
	in_23m.Tables[0].Rows[0]["TC_NO"] = "T82322";
	in_23m.Tables.Add();
	in_23m.Tables[1].Columns.Add(tmmsm01);
	try
	{
		if (!bcls_rec->Tables.Contains("MM0099"))
		{
			bcls_rec->Tables.Add("MM0099");
			bcls_rec->Tables["MM0099"].Columns.Add(tmmsm96);
		}
		//RCV_MAT_FLAG
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++) {
			tmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tpssm03["LSLAB_NO"]=tmmsm01["LSLAB_NO"];
			if (tpssm03.QueryCount("LSLAB_NO")==0)
			{
				sprintf(s.msg, "该命令板坯已经不存在。");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			else {
				/*if (tpssm03["SLAB_PROD_FLAG"].ToString() != "1") {
					sprintf(s.msg, "该命令板坯并未使用，不能取消订单绑定。");
					throw CApplicationException(-1, s.msg, log.Location);
				}*/
				tpssm03["SLAB_PROD_FLAG"] = "0";
			}
			if (!tmmsm01.Query("MAT_NO"))
			{
				sprintf(s.msg, "当前材料[%s]已归档不可操作。", (const char*)tmmsm01["MAT_NO"]);
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			tmmsm01.MergeTo(in_23m.Tables[1]);
			in_23m.Tables[1].Rows[0]["MAT_STATUS"] = "T";
			if (tmmsm01["RCV_MAT_FLAG"].ToString() == "S") //已收货；使用MM74
			{
				/*tmmsm96.CopyFrom(tmmsm01);
				tmmsm96["EVENT_ID"] = "MM74";
				tmmsm96["EVENT_LINE_TYPE"] = "00";
				tmmsm96["SYSTEM_ID"] = "WMSM";
				tmmsm96["FUNC_ID"] = s.svc_name;
				tmmsm96["EVENT_DESC"] = "收货后取消匹配";
				tmmsm96["FORM_NAME"] = s.formname;
				tmmsm96.MergeTo(bcls_rec->Tables["MM0099"], false);*/
				sprintf(s.msg, "该材料[%S]已经收货，不能取消订单绑定。", (const char*)tmmsm01["MAT_NO"]);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			else//已收货；使用MM73
			{
				tmmsm96.CopyFrom(tmmsm01);
				tmmsm96["EVENT_ID"] = "MM73";
				tmmsm96["EVENT_LINE_TYPE"] = "00";
				tmmsm96["SYSTEM_ID"] = "WMSM";
				tmmsm96["FUNC_ID"] = s.svc_name;
				tmmsm96["EVENT_DESC"] = "收货后取消匹配";
				tmmsm96["FORM_NAME"] = s.formname;
				tmmsm96.MergeTo(bcls_rec->Tables["MM0099"], false);
			}
			tpssm03.Update("SLAB_PROD_FLAG", "LSLAB_NO");
		}
		if (bcls_rec->Tables["MM0099"].Rows.get_Count() > 0) {
			
			doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
#pragma region 调用函数，发送智慧质量电文
		if (in_23m.Tables[1].Rows.get_Count() > 0)
		{
			doFlag = f_t8z_23m_snd(&in_23m, bcls_ret, conn);
		}
#pragma endregion

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。" /* _RES("GCRSS0000006")*//*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
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
	return doFlag;

}