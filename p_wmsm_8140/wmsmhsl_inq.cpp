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

BM2F_ENTERACE(wmsmhsl_inq);

int f_wmsmhsl_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
		sqlstr = " with mm1 as (select C_DIV,\
            ST_NO,\
            C_DELIVERY_FAC,\
            TRAN_END_TIME,\
            C_ISHOTSEND,\
            MAT_ACT_WT\
            from hMMSM01\
            WHERE USAGE_DECISION != '3005'\
            and MAT_NO not in(select IN_MAT_NO from tmmsm35)\
            ),\
            MM2 AS(SELECT SUBSTR(TRAN_END_TIME, 0, 6)                                                    TRAN_MONTH,\
                ROUND(SUM(MAT_ACT_WT), 3)                                                      NI_2250_J,\
                ROUND(SUM(DECODE(C_ISHOTSEND, '1', MAT_ACT_WT, 0)), 3)                         NI_2250_H,\
                ROUND(SUM(DECODE(C_ISHOTSEND, '1', MAT_ACT_WT, 0)) / SUM(MAT_ACT_WT), 4) * 100 NI_2250_L\
                FROM MM1\
                WHERE SUBSTR(ST_NO, 0, 2) IN('1A', '1D', '1P')\
                AND C_DELIVERY_FAC = '6360'\
                GROUP BY SUBSTR(TRAN_END_TIME, 0, 6)),\
            MM3 AS(SELECT SUBSTR(TRAN_END_TIME, 0, 6)                                                    TRAN_MONTH,\
                ROUND(SUM(MAT_ACT_WT), 3)                                                      CR_2250_J,\
                ROUND(SUM(DECODE(C_ISHOTSEND, '1', MAT_ACT_WT, 0)), 3)                         CR_2250_H,\
                ROUND(SUM(DECODE(C_ISHOTSEND, '1', MAT_ACT_WT, 0)) / SUM(MAT_ACT_WT), 4) * 100 CR_2250_L\
                FROM MM1\
                WHERE SUBSTR(ST_NO, 0, 2) IN('1M', '1F')\
                AND C_DELIVERY_FAC = '6360'\
                GROUP BY SUBSTR(TRAN_END_TIME, 0, 6)),\
            MM4 AS(SELECT SUBSTR(TRAN_END_TIME, 0, 6)                                                    TRAN_MONTH,\
                ROUND(SUM(MAT_ACT_WT), 3)                                                      C_2250_J,\
                ROUND(SUM(DECODE(C_ISHOTSEND, '1', MAT_ACT_WT, 0)), 3)                         C_2250_H,\
                ROUND(SUM(DECODE(C_ISHOTSEND, '1', MAT_ACT_WT, 0)) / SUM(MAT_ACT_WT), 4) * 100 C_2250_L\
                FROM MM1\
                WHERE C_DIV = '2'\
                AND C_DELIVERY_FAC = '6360'\
                GROUP BY SUBSTR(TRAN_END_TIME, 0, 6)),\
            MM5 AS(SELECT SUBSTR(TRAN_END_TIME, 0, 6)                                                    TRAN_MONTH,\
                ROUND(SUM(MAT_ACT_WT), 3)                                                      C_1549_J,\
                ROUND(SUM(DECODE(C_ISHOTSEND, '1', MAT_ACT_WT, 0)), 3)                         C_1549_H,\
                ROUND(SUM(DECODE(C_ISHOTSEND, '1', MAT_ACT_WT, 0)) / SUM(MAT_ACT_WT), 4) * 100 C_1549_L\
                FROM MM1\
                WHERE C_DIV = '2'\
                AND C_DELIVERY_FAC = '6350'\
                GROUP BY SUBSTR(TRAN_END_TIME, 0, 6)),\
            MM6 AS(\
                SELECT CASE\
                WHEN MM2.TRAN_MONTH IS NOT NULL THEN MM2.TRAN_MONTH\
                WHEN MM3.TRAN_MONTH IS NOT NULL THEN MM3.TRAN_MONTH\
                WHEN\
                MM4.TRAN_MONTH IS NOT NULL THEN MM4.TRAN_MONTH\
                ELSE MM5.TRAN_MONTH END TRAN_MONTH,\
                NI_2250_J,\
                NI_2250_H,\
                NI_2250_L,\
                CR_2250_J,\
                CR_2250_H,\
                CR_2250_L,\
                C_2250_J,\
                C_2250_H,\
                C_2250_L,\
                C_1549_J,\
                C_1549_H,\
                C_1549_L\
                FROM MM2\
                FULL JOIN MM3 ON MM2.TRAN_MONTH = MM3.TRAN_MONTH\
                FULL JOIN MM4 ON MM2.TRAN_MONTH = MM4.TRAN_MONTH\
                FULL JOIN MM5 ON MM2.TRAN_MONTH = MM5.TRAN_MONTH)\
            SELECT*\
            FROM MM6 WHERE 1=1 ";
		if (bcls_rec->Tables[0].Rows[0]["TRAN_MONTH"].ToString().Trim() != "")
		{
			sqlstr += " AND TRAN_MONTH ='" + bcls_rec->Tables[0].Rows[0]["TRAN_MONTH"].ToString() + "' ";
		}
		
		sqlstr += " order by TRAN_MONTH desc ";
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