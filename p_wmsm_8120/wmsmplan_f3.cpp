/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         LIZHEN
Version:		1.0
Date:			2023-11-07
Description:	板坯自动匹配
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件

int f_mmsm99(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wmsm_21a006_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);

//函数申明

/*<remark>=========================================================

===========================================================</remark>*/

BM2F_ENTERACE(wmsmplan_f3);

int f_wmsmplan_f3(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */
	//CTWM06 twm06(conn);
	CModel twm06("TWM06");

	/* 数据库SQL操作字符串 */
	CString sql = "";
	CString sqlstr = "";
	CString sqlwhere = "";
	CString sqlstr_count;
	CString sqlstr_temp;

	/* 业务变量 */
	CString	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString	cs_ladle_no("");
	CString	s_heat_no("");
	CDecimal cd_count = 0;
	int	record_count_per_page = 0; /* 每页记录数 */
	int	current_page_no = 0; /* 需查询的页号,从0开始计数 */
	int	start_row = 0; /* 将要压入outBlock的起始行 */


	/* 全局变量 */
	CString crane_no = "";
	CString stock_place_no_from = "";
	CString stock_place_no_to = "";
	/* 数据库操作类定义 */
	CModel twmsm60("TWMSM60");

	CString v_plan_no = "";
	CString sqlstr_item = "";
	CDbCommand cmd_item_inq(conn);
	CDataTable dt;
	int cs = 0;
	CDbCommand cmd_load_inq(conn);
	CString sqlstr_load = "";
	CString wm_planid_seq = "";
	CString sqlstr_unload = "";
	CDbCommand cmd_unload_inq(conn);
	
	CDbCommand cmd_inq(conn);

	//系统的分页类信息。
	CPageInfo pageInfo;

	blkNum = bcls_rec->Tables.IndexOf("TWMSM60");
	if (blkNum < 0)
	{
		bcls_rec->Tables.Add("TWMSM60");
		bcls_rec->Tables["TWMSM60"].Columns.Add(twmsm60);
		if (!bcls_rec->Tables["TWMSM60"].Columns.Contains("DEAL_FLAG")) {
			bcls_rec->Tables["TWMSM60"].Columns.Add(DT_STRING, "DEAL_FLAG");
		}

	}

	try
	{
		sqlstr = " select t.plan_no from twmsm60 t where REC_CREATE_TIME LIKE '" + datetime.Substring(0, 8) + "%' and PLAN_STATUS='0' and DEAL_FLAG='I' ";
		Log::Trace("", __FUNCTION__, "sqlstr[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			sprintf(s.msg, "当前有可用的计划[%s]未关闭，请先关闭当天的所有可用计划，再生成！",(const char*)cmd_inq.GetString(1));
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		cmd_inq.Close();
		

		//开始循环装卸点信息表，生成4*14条新计划
		sqlstr_load = "select t.LOAD_CODE_FACTORY,t.LOAD_CODE_AREA,t.LOAD_CODE,t.LOAD_NAME from TWMSM61A t";
		cmd_load_inq.SetCommandText(sqlstr_load);
		cmd_load_inq.ExecuteReader();
		while (cmd_load_inq.Read())
		{
			Log::Trace("", __FUNCTION__, "LINE[{0}]", __LINE__);
			twmsm60.Reset();
			twmsm60["REC_CREATE_TIME"] = datetime;
			twmsm60["REC_CREATOR"] = s.svc_name;
			twmsm60["DEAL_FLAG"] = "I";
			twmsm60["GOODS_CODE"] = "DP";
			twmsm60["LOAD_CODE_FACTORY"] = cmd_load_inq.GetString(1);
			twmsm60["LOAD_CODE_AREA"] = cmd_load_inq.GetString(2);
			twmsm60["LOAD_CODE"] = cmd_load_inq.GetString(3);
			twmsm60["LOAD_NAME"] = cmd_load_inq.GetString(4);
			twmsm60["PLAN_WT"] = 10000;
			twmsm60["PLAN_STATUS"] = "0";
			twmsm60["PLAN_START_TIME"] = datetime.Substring(0, 8) + "000000";
			twmsm60["PLAN_END_TIME"] = CDateTime::Now().AddDays(+1).ToString("yyyyMMdd") + "000000";
			Log::Trace("", __FUNCTION__, "LINE[{0}]", __LINE__);
			sqlstr_unload = "select t.UNLOAD_CODE_FACTORY,t.UNLOAD_CODE_AREA,t.UNLOAD_CODE,t.UNLOAD_NAME from TWMSM62A t";
			cmd_unload_inq.SetCommandText(sqlstr_unload);
			Log::Trace("", __FUNCTION__, "LINE[{0}]", __LINE__);
			cmd_unload_inq.ExecuteReader();
			while (cmd_unload_inq.Read())
			{
				Log::Trace("", __FUNCTION__, "LINE[{0}]", __LINE__);
			
				twmsm60["UNLOAD_CODE_FACTORY"] = cmd_unload_inq.GetString(1);
				twmsm60["UNLOAD_CODE_AREA"] = cmd_unload_inq.GetString(2);
				twmsm60["UNLOAD_CODE"] = cmd_unload_inq.GetString(3);
				twmsm60["UNLOAD_NAME"] = cmd_unload_inq.GetString(4);
				if (twmsm60["LOAD_CODE"].ToString() == twmsm60["UNLOAD_CODE"].ToString())
				{
					continue;
				}
				sqlstr = " SELECT LPAD(TO_CHAR(WM_PLANID_SEQ.NEXTVAL), 4, '0') FROM DUAl ";
				wm_planid_seq = Db::QueryCString(sqlstr);
				Log::Trace("", __FUNCTION__, "wm_planid_seq[{0}]", wm_planid_seq);
				twmsm60["PLAN_NO"] = CString::Format("%s%s", (const char*)datetime.Substring(0, 12), (const char*)wm_planid_seq);
				twmsm60.Insert();
				bcls_rec->Tables["TWMSM60"].Rows.Clear();
				bcls_rec->Tables["TWMSM60"].Rows.Add();
				bcls_rec->Tables["TWMSM60"].Rows[0]["PLAN_NO"] = twmsm60["PLAN_NO"].ToString();
				Log::Trace("", __FUNCTION__, "LINE[{0}]", __LINE__);
				//调用发送物流电文
				doFlag = f_wmsm_21a006_snd(bcls_rec, bcls_ret, conn);
				if (doFlag != 0)
				{
					strcpy(s.msg, "新增倒运计划发送失败！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				Log::Trace("", __FUNCTION__, "LINE[{0}]", __LINE__);
			}
			cmd_unload_inq.Close();
			Log::Trace("", __FUNCTION__, "LINE[{0}]", __LINE__);
		}
		cmd_load_inq.Close();

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