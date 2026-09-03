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

BM2F_ENTERACE(wmsmcztscp_inq);

int f_wmsmcztscp_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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


	CString datetime_o = CDateTime::Now().AddMonths(-1).ToString("yyyyMMddHHmmss");
	CString datetime_n = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString v_dev_code;
	CString FACTORY_2;
	CString c_div;
	try
	{

		if (bcls_rec->Tables[0].Columns.Contains("ST_NO"))
			v_st_no = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString();

		bcls_ret->Tables.Add();
		bcls_ret->Tables[1].Columns.Add(DT_STRING, "CODE_DESC_4_CONTENT");
		bcls_ret->Tables[1].Columns.Add(DT_STRING, "CODE_DESC_2_CONTENT");
		sql = " SELECT CODE_DESC_4_CONTENT,CODE_DESC_2_CONTENT,CODE_DESC_3_CONTENT,CODE_DESC_5_CONTENT FROM TWMSMZD02 WHERE CODE_CLASS = 'WMCZT' AND CODE_DESC_1_CONTENT = @userid and CODE_DESC_2_CONTENT='" 
			+ bcls_rec->Tables[0].Rows[0]["STATION_NO"].ToString() + "'  ";
		Log::Trace("", __FUNCTION__, "STATION_NO[{0}]  ", bcls_rec->Tables[0].Rows[0]["STATION_NO"].ToString());
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
		cmd_inq.Close();

		//CString ST_NO_NOW = "";//当前正在生产的钢种
		//CString HEAT_NO_NOW = "";//当前正在生产的熔炼号
		//sql = " SELECT ST_NO,HEAT_NO FROM V_PSSM_DEV_ST_NO WHERE DEV_CODE = '" + bcls_rec->Tables[0].Rows[0]["STATION_NO"].ToString() + "' ";
		//Log::Trace("", "", "sql2", sql, s.userid);
		//cmd_inq.SetCommandText(sql);
		//cmd_inq.Parameters.Set("userid", s.userid);
		//cmd_inq.ExecuteReader();
		//if (cmd_inq.Read())
		//{
		//	ST_NO_NOW = cmd_inq.GetString(1);
		//	HEAT_NO_NOW = cmd_inq.GetString(2);
		//}
		//cmd_inq.Close();

		sql = " select  MEMO_DETAIL,st_no,GUICHENG,MEMO_DETAIL1,'1' ok_flag from TWMSMCZTS where ST_NO like '%" + bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString() + "%'\
			AND FACTORY_2 = '" + FACTORY_2 + "' ";
		Log::Trace("", "", "sql3", sql, s.userid);
		cmd_inq.SetCommandText(sql);
		cmd_inq.Parameters.Set("userid", s.userid);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();




		if (bcls_ret->Tables[0].Rows.get_Count() == 0)
		{
			bcls_ret->Tables[0].Rows.Add();
			bcls_ret->Tables[0].Rows[0]["OK_FLAG"] = "0";
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "MSG");
			bcls_ret->Tables[0].Rows[0]["MSG"] = "当前工位" + v_dev_code + "生产的钢种" + bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString() + "暂无操作要点，请按照规程操作！";

		}
		if (bcls_ret->Tables[0].Rows.get_Count() >= 2)
		{
			bcls_ret->Tables[0].Rows.Clear();
			bcls_ret->Tables[0].Rows.Add();
			bcls_ret->Tables[0].Rows[0]["OK_FLAG"] = "0";
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "MSG");
			bcls_ret->Tables[0].Rows[0]["MSG"] = "当前工位" + v_dev_code + "生产的钢种" + bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString() + "有重复的操作要点，请按照规则维护！";
		}
		if (!bcls_ret->Tables[0].Columns.Contains("HEAT_NO"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
		}
		if (!bcls_ret->Tables[0].Columns.Contains("FACTORY_2"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "FACTORY_2");
		}
		/*bcls_ret->Tables[0].Rows[0]["HEAT_NO"] = HEAT_NO_NOW;
		bcls_ret->Tables[0].Rows[0]["ST_NO"] = ST_NO_NOW;
		bcls_ret->Tables[0].Rows[0]["FACTORY_2"] = FACTORY_2;*/

		bcls_ret->Tables.Add();
		CString sql_tmp = "  ";

		//根据钢种查询内控成分
		sql = " SELECT t.st_no,re.spe_min,re.spe_max,re.elm_code,re.elm_name,re.* from tqmts0x t "
			" left join tqmts02 re  on t.elm_std_idx_a = re.idx_no where t.st_no = '" + bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString() +"'";
		Log::Trace("", "", "sql4", sql);
		cmd_inq.SetCommandText(sql);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[2]);
		cmd_inq.Close();

		//事故案例
		bcls_ret->Tables.Add();
		sql = " SELECT MEMO_DETAIL FROM TWMSMSGAL WHERE FACTORY_2 = '" + FACTORY_2 + "' " + "AND ST_NO='" + v_st_no + "'";
		Log::Trace("", "", "sql5", sql);
		cmd_inq.SetCommandText(sql);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[3]);
		cmd_inq.Close();
		if (bcls_ret->Tables[3].Rows.get_Count() == 0)
		{
			bcls_ret->Tables[3].Rows.Add();
		}

		//作业区要点
		bcls_ret->Tables.Add();
		sql = " SELECT MEMO_DETAIL1 FROM TWMSMCZTS_GYHX WHERE FACTORY_2 = '" + FACTORY_2 + "' ";
		if (c_div.Trim() != "")
		{
			sql += " and c_div='" + c_div + "' ";
		}
		if (FACTORY_2 == "C")
		{
			sql += " and dev_code='" + v_dev_code + "'";
		}
		Log::Trace("", "", "sql6", sql);
		cmd_inq.SetCommandText(sql);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[4]);
		cmd_inq.Close();
		if (bcls_ret->Tables[4].Rows.get_Count() == 0)
		{
			bcls_ret->Tables[4].Rows.Add();
		}

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