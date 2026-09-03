/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         NYY
Version:		1.0
Date:			2023-12-1
Description:	 
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件

//函数申明

/*<remark>=========================================================
///<summary>
///卸车计划查询
///<para>
///2.排序方式：
///</para>
///<para>数据库表：TWMSM62 倒运计划表；
///<returns>返回符合查询条件的计划信息</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wmsmgdwg_inq);

int f_wmsmgdwg_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */

	CModel twmsm62 = CModel("TWMSM62");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlstr1 = "";
	CString sqlwhere = "";
	CString s_userid("");
	CString sqlstr_count;
	CString sqlstr_temp;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_con(conn);


	//系统的分页类信息。
	CPageInfo pageInfo;

	/* 业务变量 */

	CString truck_no("");

	CString	truck_board_no("");
	CString	practice_no("");
	CString	load_code_factory("");

	CString	unload_flag("");
	CString	plan_start_time("");
	CString	archive_flag("");

	//CString	datetime("");

	CDecimal cd_count = 0;
	int	record_count_per_page = 0; /* 每页记录数 */
	int	current_page_no = 0; /* 需查询的页号,从0开始计数 */
	int	start_row = 0; /* 将要压入outBlock的起始行 */
	try {

		sqlstr = " select MAT_NO, SLAB_CUT_TIME, STOCK_L2, STOCK_PLACE_NO,RECV_MAT_TIME\
			from tmmsm01 a \
			left join twmsmzd02 b on a.GUIDE_DEST = b.CODE and b.CODE_CLASS = 'WM02' \
			WHERE B.CODE_DESC_1_CONTENT = '2250轧机' \
			and STOCK_L2 in('CS-HSM', 'SS-HSM', 'HF') ";
			
		//Log::Trace("", __FUNCTION__, "archive_flag				= [{0}]", (const char*)archive_flag);
		Log::Trace("", __FUNCTION__, "sqlstr				= [{0}]", (const char*)sqlstr);

		if (bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim() != "")
		{
			sqlwhere += " and MAT_NO='" + bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString() + "' ";
		}
		sqlwhere += " order by SLAB_CUT_TIME  ";
		sqlstr += sqlwhere;
		Log::Trace("", __FUNCTION__, "sqlstr				= [{0}]", (const char*)sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "Database processing error，sqlcode = [{0}]." /* _RES("GCRSS0000006")*//*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
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