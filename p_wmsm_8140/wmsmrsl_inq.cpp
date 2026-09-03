/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         lizhen
Version:		1.0
Date:			2024-4-1
Description:	碳钢热送率
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

BM2F_ENTERACE(wmsmrsl_inq);

int f_wmsmrsl_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */


	CString sqlstr = "";
	
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_con(conn);


	//系统的分页类信息。
	CPageInfo pageInfo;

	/* 业务变量 */

	
	try {

		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		sqlstr = " with mm1 as (\
			select decode(TRAN_TIME, ' ', TRAN_END_TIME, TRAN_TIME)                TRAN_TIME,\
			substr2(decode(TRAN_TIME, ' ', TRAN_END_TIME, TRAN_TIME), 0, 8) tran_date,\
			C_ISHOTSEND,\
			MAT_ACT_WT\
			from hMMSM01\
		where C_DIV = '2'\
			and C_DELIVERY_STOCK = '6361'),\
			mm2 as(select tran_date, sum(MAT_ACT_WT) RSL_DON from mm1 where 1 = 1 GROUP BY tran_date),\
			mm3 as(select tran_date, sum(MAT_ACT_WT) RSL_NUM from mm1 where 1 = 1 AND C_ISHOTSEND = '1' GROUP BY tran_date)\
			select A.tran_date, RSL_DON, RSL_NUM, CASE WHEN RSL_DON = 0 THEN 0 ELSE ROUND(RSL_NUM / RSL_DON, 3) END RSL_LV\
			from mm2 a\
			left join mm3 b on a.tran_date = B.tran_date\
			where 1 = 1 ";
		if (bcls_rec->Tables[0].Rows[0]["TRAN_TIME_FROM"].ToString().Trim() != "")
		{
			sqlstr += " AND A.tran_date>='" + bcls_rec->Tables[0].Rows[0]["TRAN_TIME_FROM"].ToString().SubstringNE(0, 8) + "' ";
		}
		if (bcls_rec->Tables[0].Rows[0]["TRAN_TIME_TO"].ToString().Trim() != "")
		{
			sqlstr += " AND A.tran_date<='" + bcls_rec->Tables[0].Rows[0]["TRAN_TIME_TO"].ToString().SubstringNE(0, 8) + "' ";
		}
		sqlstr += " order by a.tran_date desc ";
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