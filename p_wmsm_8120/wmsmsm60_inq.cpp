/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2013
Author:     lizhen
Version:    1.0
Date:      2023/12/05
Description: 倒运计划查询
**************************************************/
//框架头文件
#include "stdafx.h" 
//#include "twma1.h"

/*<remark>=========================================================
/// <summary>
/// 倒运计划查询
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件

//外部函数声明


BM2F_ENTERACE(wmsmsm60_inq)

int f_wmsmsm60_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;

	/* 业务变量 */
	CString	datetime("");
	CDecimal totalCount = 0;
	int	record_count_per_page = 0; /* 每页记录数 */
	int	current_page_no = 0; /* 需查询的页号,从0开始计数 */
	int	start_row = 0; /* 将要压入outBlock的起始行 */

	CDecimal d_thick_fr = 0;
	CDecimal d_thick_to = 0;
	CString v_time_fr = "";
	CString v_time_to = "";
	CString roll_plan_no = "";
	CString ingot_code = "";
	CString flag = "";
	CString raw_origin = "";
	CString mat_line_type = "";
	CString mat_kind = "";

	/* 实体类定义 */
	//CTWMA1 twma1(conn);
	CModel twma1 = CModel("TMMSM01");

	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CString sqlstr_count;
	CString s_userid("");
	CString s_load_code("");//装点
	CString s_unload_code("");//卸点

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	try
	{
		CString datetime = CDateTime::Now().ToString("yyyyMMdd");
		Log::Trace("", "", "", "datetime[{0}]", datetime);
		s_load_code = bcls_rec->Tables[0].Rows[0]["LOAD_CODE"].ToString();
		s_unload_code = bcls_rec->Tables[0].Rows[0]["UNLOAD_CODE"].ToString();

		sqlstr = " select * from twmsm60 where PLAN_NO like '" + datetime + "%' ";
		if (s_load_code.Trim()!="")
		{
			sqlstr += " and LOAD_CODE= '" + s_load_code + "' ";
		}
		if (s_unload_code.Trim() != "")
		{
			sqlstr += " and UNLOAD_CODE= '" + s_unload_code + "' ";
		}
		sqlstr += " AND DEAL_FLAG='I' and plan_status ='0' ";
		Log::Trace("", "", "", "sqlstr[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();
		
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
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
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;

}
