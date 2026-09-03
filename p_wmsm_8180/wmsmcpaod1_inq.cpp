/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         LIZHEN
Version:		1.0
Date:			2023-11-07
Description:
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"
//程序用头文件


//函数申明

/*<remark>=========================================================

===========================================================</remark>*/

BM2F_ENTERACE(wmsmcpaod1_inq);

int f_wmsmcpaod1_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
	CString s_tc_no = " ";
	/* 全局变量 */

	CDbCommand cmd_inq(conn);

	//系统的分页类信息。


	CString datetime_o = CDateTime::Now().AddMonths(-1).ToString("yyyyMMddHHmmss");
	CString datetime_n = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString v_dev_code;
	CString FACTORY_2;
	CString c_div;
	try
	{
		/*bcls_ret->Tables.Add();
		bcls_ret->Tables[1].Columns.Add(DT_STRING, "CODE_DESC_4_CONTENT");
		bcls_ret->Tables[1].Columns.Add(DT_STRING, "CODE_DESC_2_CONTENT");
		sql = " SELECT CODE_DESC_4_CONTENT,CODE_DESC_2_CONTENT,CODE_DESC_3_CONTENT,CODE_DESC_5_CONTENT FROM TWMSMZD02 WHERE CODE_CLASS = 'WMCZT' AND CODE_DESC_1_CONTENT = @userid and CODE_DESC_2_CONTENT='" + bcls_rec->Tables[0].Rows[0]["STATION_NO"].ToString() + "'  ";
		Log::Trace("", "", "sql1", sql, s.userid);
		cmd_inq.SetCommandText(sql);
		cmd_inq.Parameters.Set("userid", s.userid);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			bcls_ret->Tables[1].Rows.Add();
			bcls_ret->Tables[1].Rows[0]["CODE_DESC_4_CONTENT"] = cmd_inq.GetString(1);
			bcls_ret->Tables[1].Rows[0]["CODE_DESC_2_CONTENT"] = cmd_inq.GetString(2);
			v_dev_code = cmd_inq.GetString(2);
			FACTORY_2 = cmd_inq.GetString(3);
			c_div = cmd_inq.GetString(4);
		}
		else {
			sprintf(s.msg, "该工号[%s]没有配置工位，请联系运维人员！", s.userid);

			throw CApplicationException(-1, s.msg, log.Location);
		}
		cmd_inq.Close();*/

		CString ST_NO_NOW = "";//当前正在生产的钢种
		CString HEAT_NO_NOW = "";//当前正在生产的熔炼号
		sql = " SELECT ST_NO,HEAT_NO FROM V_PSSM_DEV_ST_NO WHERE DEV_CODE = '" + bcls_rec->Tables[0].Rows[0]["STATION_NO"].ToString() + "' ";
		Log::Trace("", __FUNCTION__, "sql[{0}]  ", sql);
		cmd_inq.SetCommandText(sql);
		cmd_inq.Parameters.Set("userid", s.userid);
		Log::Trace("", __FUNCTION__, "sql0[{0}]  ", sql);
		cmd_inq.SetCommandText(sql);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			ST_NO_NOW = cmd_inq.GetString(1);
			HEAT_NO_NOW = cmd_inq.GetString(2);
		}
		cmd_inq.Close();

		if (!bcls_ret->Tables[0].Columns.Contains("HEAT_NO"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
		}
		if (!bcls_ret->Tables[0].Columns.Contains("ST_NO"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "ST_NO");
		}
		bcls_ret->Tables[0].Rows[0]["HEAT_NO"] = HEAT_NO_NOW;
		bcls_ret->Tables[0].Rows[0]["ST_NO"] = ST_NO_NOW;
		Log::Trace("", __FUNCTION__, "ST_NO_NOW[{0}]  ", ST_NO_NOW);
		Log::Trace("", __FUNCTION__, "HEAT_NO_NOW[{0}]  ", HEAT_NO_NOW);
		//过程内控
		bcls_ret->Tables.Add();
		CString sql_tmp = "  ";
		sql_tmp = " T3 AS (select ELM_NAME, SPE_MIN, SPE_MAX, MAIN_AIM, '" + ST_NO_NOW + "' ST_NO, FACTORY_2 from TWMSMCZTS_GC A WHERE ST_NO LIKE '%" + ST_NO_NOW + "%'   AND FACTORY_2 = 'A'   AND DIS_FLAG = '1') ";
		sql = " with t1 as (select *\
			  	from(select row_number() over(partition by HEAT_NO, WHOLE_BACKLOG_CODE order by REC_CREATE_TIME DESC) ROW_ID, t.*\
				from(SELECT HEAT_NO, ST_SAMPLE_NO, WHOLE_BACKLOG_CODE, REC_CREATE_TIME FROM TQMTS24 where 1 = 1 AND ST_SAMPLE_DIV='T' AND HEAT_NO = '" + HEAT_NO_NOW + "' AND WHOLE_BACKLOG_CODE='A') t) where ROW_ID = 1),\
				T2 AS(SELECT ELM_NAME, T2.ELM_ACT,T2.ST_SAMPLE_NO,T1.HEAT_NO FROM T1 LEFT JOIN TQMTS25 T2 ON T1.ST_SAMPLE_NO = T2.ST_SAMPLE_NO ), ";
		sql += sql_tmp;
		sql += " select T3.*, T2.ELM_ACT,T2.ST_SAMPLE_NO,T2.HEAT_NO from t3 left join  t2 on t3.ELM_NAME = t2.ELM_NAME where 1 = 1 ";
		Log::Trace("", __FUNCTION__, "sql1[{0}]  ", sql);
		cmd_inq.SetCommandText(sql);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[1]);
		cmd_inq.Close();

		//过程指标
		bcls_ret->Tables.Add();
		sql = " SELECT * FROM TWMSMCP_GCZBAOD where 1 = 1 ";
		Log::Trace("", __FUNCTION__, "sql2[{0}]  ", sql);
		cmd_inq.SetCommandText(sql);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[2]);
		cmd_inq.Close();

		//查询报警内容
		bcls_ret->Tables.Add();
		//sql = " SELECT * FROM (SELECT T.*,ROW_NUMBER() OVER (PARTITION BY T.REMARK ORDER BY T.REC_CREATE_TIME DESC) RN FROM TWMSMZHZLLL T WHERE T.HEAT_NO = '" + HEAT_NO_NOW + "') WHERE RN = '1' ORDER BY TIME_STAMPS DESC ";

		sql = " SELECT * FROM (SELECT T.*,ROW_NUMBER() OVER (PARTITION BY T.REMARK ORDER BY T.REC_CREATE_TIME DESC) RN FROM TWMSMZHZLLL T WHERE T.HEAT_NO = '" + HEAT_NO_NOW + "' AND  ) WHERE RN = '1' ORDER BY TIME_STAMPS DESC ";

		Log::Trace("", __FUNCTION__, "HEAT_NO_NOW[{0}]  ", HEAT_NO_NOW);
		Log::Trace("", __FUNCTION__, "sql3[{0}]  ", sql);
		cmd_inq.SetCommandText(sql);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[3]);
		cmd_inq.Close();


		//if (bcls_ret->Tables[4].Rows.get_Count() == 0)
		//{
		//bcls_ret->Tables[4].Rows.Add();
		//}

		/*N_ST_NO = bcls_ret->Tables[0].Rows[0]["ST_NO"].ToString();
		bcls_ret->Tables.Add();
		sql = " SELECT * FROM TQMTS02 WHERE IDX_NO=(select ELM_STD_IDX_A from TQMTS0X where ST_NO='"+ N_ST_NO +"')  ";
		Log::Trace("", "", "sql", sql, s.fore_ip);
		cmd_inq.SetCommandText(sql);

		cmd_inq.ExecuteQuery(bcls_ret->Tables[2]);
		cmd_inq.Close();*/
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