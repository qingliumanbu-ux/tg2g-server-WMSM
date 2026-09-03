/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         BHY
Version:		1.0
Date:			2025-02-21
Description:
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"
//程序用头文件


//函数申明

/*<remark>=========================================================

===========================================================</remark>*/

BM2F_ENTERACE(wmsmyryccm_inq);

int f_wmsmyryccm_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	int ret = 0;

	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */

	EPEX epex;

	/* 数据库SQL操作字符串 */
	CString sql = "";
	CString sqlstr = "";
	CString sqlwhere = "";
	CString sqlstr_count;
	CString sqlstr_temp;

	/* 业务变量 */
	CString v_st_no = " ";
	/* 全局变量 */

	CDbCommand cmd_inq(conn);

	//系统的分页类信息。


	/*CString datetime_o = CDateTime::Now().AddMonths(-1).ToString("yyyyMMddHHmmss");
	CString datetime_n = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString v_dev_code;
	CString FACTORY_2;
	CString c_div;*/
	try
	{

		/*if (bcls_rec->Tables[0].Columns.Contains("ST_NO"))
			v_st_no = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString();*/

		//连铸模块
		bcls_ret->Tables.Add();
		sql = " select t2.st_no,t1.start_time as CC_BEGIN_TIME,t1.end_time as CC_END_TIME \
			    from TPSSM12 t1 left join TPSSM11 t2 on t1.sm_plan_nol2 = t2.sm_plan_nol2 \
		        where t1.dev_code = 'C0' and t1.end_time_real = ' ' order by t1.start_time   ";
		Log::Trace("", "", "sql0", sql);
		cmd_inq.SetCommandText(sql);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();
		/*if (bcls_ret->Tables[0].Rows.get_Count() == 0)
		{
			bcls_ret->Tables[0].Rows.Add();
		}*/

		bcls_ret->Tables.Add();
		sql = " select t2.st_no,t1.start_time as CC_BEGIN_TIME,t1.end_time as CC_END_TIME \
			    from TPSSM12 t1 left join TPSSM11 t2 on t1.sm_plan_nol2 = t2.sm_plan_nol2 \
			    where t1.dev_code = 'C1' and t1.end_time_real = ' ' order by t1.start_time   ";		
		Log::Trace("", "", "sql1", sql);
		cmd_inq.SetCommandText(sql);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[1]);
		cmd_inq.Close();
		/*if (bcls_ret->Tables[1].Rows.get_Count() == 0)
		{
			bcls_ret->Tables[1].Rows.Add();
		}*/

		bcls_ret->Tables.Add();
		sql = " select t2.st_no,t1.start_time as CC_BEGIN_TIME,t1.end_time as CC_END_TIME \
			  	from TPSSM12 t1 left join TPSSM11 t2 on t1.sm_plan_nol2 = t2.sm_plan_nol2 \
				where t1.dev_code = 'C2' and t1.end_time_real = ' ' order by t1.start_time   ";
		Log::Trace("", "", "sql2", sql);
		cmd_inq.SetCommandText(sql);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[2]);
		cmd_inq.Close();
		
		//作业区通知查询
		bcls_ret->Tables.Add();
		sql = " SELECT MEMO_DETAIL FROM (SELECT FACTORY_2,LISTAGG(MEMO_DETAIL,';'||chr(10)) AS MEMO_DETAIL FROM TWMSMYRYTS T GROUP BY FACTORY_2)T WHERE T.FACTORY_2 = 'AOD'";
		Log::Trace("", "", "AOD作业区通知table3=[{0}]", sql);
		cmd_inq.SetCommandText(sql);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[3]);
		cmd_inq.Close();
		if (bcls_ret->Tables[3].Rows.get_Count() == 0)
		{
		bcls_ret->Tables[3].Rows.Add();
		}

		bcls_ret->Tables.Add();
		sql = " SELECT MEMO_DETAIL FROM (SELECT FACTORY_2,LISTAGG(MEMO_DETAIL,';'||chr(10)) AS MEMO_DETAIL FROM TWMSMYRYTS T GROUP BY FACTORY_2)T WHERE T.FACTORY_2 = 'AMF'";
		Log::Trace("", "", "AMF作业区通知table4=[{0}]", sql);
		cmd_inq.SetCommandText(sql);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[4]);
		cmd_inq.Close();
		if (bcls_ret->Tables[4].Rows.get_Count() == 0)
		{
			bcls_ret->Tables[4].Rows.Add();
		}

		bcls_ret->Tables.Add();
		sql = " SELECT MEMO_DETAIL FROM (SELECT FACTORY_2,LISTAGG(MEMO_DETAIL,';'||chr(10)) AS MEMO_DETAIL FROM TWMSMYRYTS T GROUP BY FACTORY_2)T WHERE T.FACTORY_2 = 'EAF'";
		Log::Trace("", "", "EAF作业区通知table5=[{0}]", sql);
		cmd_inq.SetCommandText(sql);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[5]);
		cmd_inq.Close();
		if (bcls_ret->Tables[5].Rows.get_Count() == 0)
		{
			bcls_ret->Tables[5].Rows.Add();
		}

		//三脱站信息
		bcls_ret->Tables.Add();
		sql = "SELECT T.PROC_NO AS HEAT_NO,T.IRON_LADLE_NO AS IRON_LADLE_ID,T.DE_S_REP_TEMP AS OUT_STEEL_TEMP,T.END_TIME AS OUT_STEEL_TIME FROM TMMSM14 T \
			WHERE T.REC_CREATE_TIME > to_char(sysdate - 1, 'yyyyMMddHH24miss') AND T.HEAT_NO = ' ' \
			ORDER BY T.REC_CREATE_TIME DESC ";
		Log::Trace("", "", "DES三脱站信息table6=[{0}]", sql);
		cmd_inq.SetCommandText(sql);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[6]);
		cmd_inq.Close();
		if (bcls_ret->Tables[6].Rows.get_Count() == 0)
		{
			bcls_ret->Tables[6].Rows.Add();
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