/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         nieyuanyuan
Version:		1.0
Date:			2023-11-14
Description:	
**************************************************/

//框架头文件
#include "stdafx.h"

//程序用头文件

/*<remark>=========================================================
///<summary>
///
///<para>
///
///</para>
///<para>数据库表：TWMSM60 倒运计划表；
///<returns>倒运计划生成，发送物流系统</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wmsmsjwz_auto)
int f_wmsmsjwz_auto(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	int count = 0;
	int cs = 0;
	int blkNum = 0;
	CString date_time = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CString v_factory_div = "";
	CString v_stock_no = "";
	CString v_plan_no = "";
	CString s_date_from = "";
	CString s_date_to = "";
	CString s_factory_div = "";
	CString s_stock_no = "";
	CString s_seq_no = "";

	CDateTime dt_date;


	CString plan_time_from = "";
	CString plan_time_to = "";

	/* 实体类定义 */
	

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CDbCommand cmd_inq(conn);

	CString v_shift_no = " ";
	CString v_shift_group = " ";
	f_epep_get_shift_group("SMCP", CDateTime::Now().ToString("yyyyMMddHHmmss"), v_shift_no, v_shift_group, conn);


	try
	{
		sqlstr = " insert into DA_MAT_SJWZ\
			select C_DIV,\
			location,\
			to_char(sysdate, 'yyyyMMdd') DATA_DAY,\
			to_char(sysdate, 'hh24')     DATA_HOUR,\
			count(1)                     sum_cou,\
			sum(MAT_WT)                  sum_wt,\
 '" + v_shift_group + "' PROD_SHIFT_GROUP\
			from(\
				select C_DIV,\
				case\
				when STOCK_L2 in('CS-HSM', 'SS-HSM', 'HF') THEN '2250'\
				when DST_STOCK_CODE = 'WXK104' THEN '钢坯库'\
				when DST_STOCK_CODE in('WXK101', 'WXK102', 'WXK103') THEN '储运站'\
				when DST_STOCK_CODE in('635003') THEN '1549'\
				when DST_STOCK_CODE in('639002') THEN '4300'\
				when DST_STOCK_CODE in('622002') THEN '南区'\
				when DST_STOCK_CODE in('632002') THEN '线材'\
				when DST_STOCK_CODE in('631003') THEN '型材'\
				when DST_STOCK_CODE in('TBZX01') THEN '太北'\
					else '二钢北区' end location,\
					MAT_WT\
					from tmmsm01)\
			group by C_DIV, location  ";
		Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteNonQuery();
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
		Log::Trace("", __FUNCTION__, "s.flag[{0}]", s.flag);
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

