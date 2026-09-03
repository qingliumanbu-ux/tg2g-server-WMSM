/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         LIZHEN
Version:		1.0
Date:			2023-11-07
Description:   板坯向2250发送电文
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"
//程序用头文件
int f_wmsm_t8p301_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//发送板坯信息
int f_wmsm_t8p302_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//发送板坯丢失
int f_mmsm99(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);



//函数申明

/*<remark>=========================================================

===========================================================</remark>*/

BM2F_ENTERACE(wmsm01q0_snd);

int f_wmsm01q0_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;


	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */
	//CTWM06 twm06(conn);

	

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
	CString send_flag = "";
	CString tc_no = "";
	CString stock_place_no_to = "";
	/* 数据库操作类定义 */
	CModel twm06("TWM06");
	CModel tmmsm01("TMMSM01");
	CModel tmmsm96("TMMSM96");
	CModel tpssm03("TPSSM03");
	CModel tqmom01("TQMOM01");
	CDbCommand cmd_inq(conn);
	EPEX epex(&s, conn);

	//系统的分页类信息。
	CPageInfo pageInfo;
	EIClass inblock;
	inblock.Tables[0].Columns.Add(tmmsm01);
	inblock.Tables[0].Rows.Clear();

	//调用物料事件
	

	try
	{
		send_flag = bcls_rec->Tables[1].Rows[0]["FLAG"];
		if (send_flag == "L")//发送板坯丢失{
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++) {
				tmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[i]);
				if (!tmmsm01.Query("MAT_NO"))
				{
					sprintf(s.msg, "当前材料[%s]已归档不可操作。", (const char*)tmmsm01["MAT_NO"]);
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				
				/*sqlstr = " SELECT CODE_DESC_3_CONTENT FROM TWMSMZD02  WHERE  CODE_CLASS='WM02' and CODE='" + tmmsm01["GUIDE_DEST"].ToString() + "' ";
				CString kefa = Db::QueryCString(sqlstr);
				if (kefa.Find("1") < 0)
				{
					sprintf(s.msg, "当前材料[%s]去向不为2250，不可发送电文", (const char*)tmmsm01["MAT_NO"]);
					throw CApplicationException(-1, s.msg, s.svc_name);
				}*/
			
				/*if (tmmsm01["HR_SEND_FLAG"].ToString() != "1")
				{
					sprintf(s.msg, "当前材料[%s]未发送板坯信息，不可发送电文", (const char*)tmmsm01["MAT_NO"]);
					throw CApplicationException(-1, s.msg, s.svc_name);
				}*/
				tmmsm01.MergeTo(inblock.Tables[0], false);

				
			}
			
			if (inblock.Tables[0].Rows.get_Count() > 0) {
				doFlag = f_wmsm_t8p302_snd(&inblock, bcls_ret, conn);
				if (doFlag < 0) {
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}
			
		}
		if (send_flag == "S")//发送板坯信息 
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++) {
				tmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[i]);
				if (!tmmsm01.Query("MAT_NO"))
				{
					sprintf(s.msg, "当前材料[%s]已归档不可操作。", (const char*)tmmsm01["MAT_NO"]);
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				sqlstr = " SELECT CODE_DESC_3_CONTENT FROM TWMSMZD02  WHERE  CODE_CLASS='WM02' and CODE='" + tmmsm01["GUIDE_DEST"].ToString() + "' ";
				CString kefa = Db::QueryCString(sqlstr);
				if (kefa.Find("1") < 0)
				{
					sprintf(s.msg, "当前材料[%s]去向不为2250，不可发送电文", (const char*)tmmsm01["MAT_NO"]);
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (tmmsm01["RCV_MAT_FLAG"].ToString() != "S")
				{
					sprintf(s.msg, "当前材料[%s]未收货，不可发送电文", (const char*)tmmsm01["MAT_NO"]);
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				
				tmmsm01.MergeTo(inblock.Tables[0], false);
			
			}
			
			if (inblock.Tables[0].Rows.get_Count() > 0) {
				doFlag = f_wmsm_t8p301_snd(&inblock, bcls_ret, conn);
				if (doFlag < 0) {
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}
		}
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