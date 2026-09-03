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

BM2F_ENTERACE(wmsmdm_auto)
int f_wmsmdm_auto(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
		sqlstr = " insert into DA_MAT_DAY_DM\
			SELECT 钢种, to_char(sysdate, 'yyyyMMdd') DATA_DAY, COUNT(1) COU,SUM(MAT_WT) SUM_WT\
			FROM(\
				select A.MAT_NO,MAT_WT,\
				case\
				when A.ST_NO like '1A1%' THEN '304'\
				WHEN A.ST_NO like '1A3%' THEN '316'\
				WHEN A.ST_NO like '1A2%' THEN '321'\
				WHEN A.ST_NO like '1D%' THEN '双相'\
				WHEN A.ST_NO like '1A6%' THEN '耐热'\
				WHEN A.ST_NO like '1F3%' THEN 'Cr13'\
				WHEN A.ST_NO like '1F401%' THEN '430'\
				WHEN A.ST_NO like '1F2%' THEN '409'\
				WHEN A.ST_NO like '1F5%' THEN '超纯'\
				WHEN A.ST_NO like '28101%' THEN '9Ni'\
				WHEN A.ST_NO like '1F4041%' THEN 'CTSZB'\
				WHEN A.ST_NO in('1A1125', '1A1127') THEN '304LG'\
					else '其他' end 钢种,\
					B.MEND_CAUSE\
					from tmmsm01 A\
					LEFT JOIN GET_MEND_FLAG B ON A.MAT_NO = B.MAT_NO\
					where(A.MEND_FLAG = '0' OR A.MEND_FLAG = ' ')\
					AND A.DIV_FLAG = ' '\
					AND B.MAT_NO IS NOT NULL)\
			GROUP BY 钢种  ";
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

