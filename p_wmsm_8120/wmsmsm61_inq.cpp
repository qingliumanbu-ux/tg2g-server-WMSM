/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2013
Author:     lizhen
Version:    1.0
Date:      2023/12/05
Description: 装车信息
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


BM2F_ENTERACE(wmsmsm61_inq)

int f_wmsmsm61_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
	CString s_plan_no("");//计划号
	CString s_practice_no("");//实绩号

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	try
	{
		CString datetime = CDateTime::Now().ToString("yyyyMMdd");
		Log::Trace("", "", "", "datetime[{0}]", datetime);
		/*s_plan_no = bcls_rec->Tables[0].Rows[0]["PLAN_NO"].ToString();
		s_practice_no = bcls_rec->Tables[0].Rows[0]["PRACTICE_NO"].ToString();*/

		sqlstr = " SELECT distinct PRACTICE_NO,PLAN_NO,TRUCK_NO,TRUCK_BOARD_NO,WAGONNO,TRUCK_MODEL,'2' UNLOAD_STATE,TRUCK_MODEL_DESC,LOAD_CODE_FACTORY,LOAD_CODE_AREA,LOAD_CODE,LOAD_NAME,UNLOAD_CODE_FACTORY,UNLOAD_CODE_AREA,UNLOAD_CODE,LOAD_END_TIME FROM TWMSM61 WHERE 1=1 AND ARCHIVE_FLAG!='1' AND UNLOAD_STATE IN ('2') ";
		if (bcls_rec->Tables[0].Columns.Contains("PLAN_NO") && bcls_rec->Tables[0].Rows[0]["PLAN_NO"].ToString().Trim() != "")
		{
			sqlstr += " and PLAN_NO= '" + bcls_rec->Tables[0].Rows[0]["PLAN_NO"].ToString() + "' ";
		}
		if (bcls_rec->Tables[0].Columns.Contains("PRACTICE_NO") && bcls_rec->Tables[0].Rows[0]["PRACTICE_NO"].ToString().Trim() != "")
		{
			sqlstr += " and PRACTICE_NO= '" + bcls_rec->Tables[0].Rows[0]["PRACTICE_NO"].ToString() + "' ";
		}
		if (bcls_rec->Tables[0].Columns.Contains("MAT_NO") && bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim() != "")
		{
			sqlstr += " and MAT_NO= '" + bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString() + "' ";
		}
		if (bcls_rec->Tables[0].Columns.Contains("TRUCK_NO") && bcls_rec->Tables[0].Rows[0]["TRUCK_NO"].ToString().Trim() != "")
		{
			sqlstr += " and TRUCK_NO= '" + bcls_rec->Tables[0].Rows[0]["TRUCK_NO"].ToString() + "' ";
		}
		if (bcls_rec->Tables[0].Columns.Contains("UNLOAD_STATE")&& bcls_rec->Tables[0].Rows[0]["UNLOAD_STATE"].ToString().Trim()!="")
		{
			bcls_rec->Tables[0].Rows[0]["UNLOAD_STATE"] = bcls_rec->Tables[0].Rows[0]["UNLOAD_STATE"].ToString().Replace(",", "','");
			sqlstr += " and UNLOAD_STATE IN ('" + bcls_rec->Tables[0].Rows[0]["UNLOAD_STATE"].ToString() + "') ";
		}
		sqlstr += " ORDER BY LOAD_END_TIME DESC ";
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
