/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         nieyuanyuan
Version:		1.0
Date:			2023-11-14
Description:	倒运计划定时生成
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
int f_wmsm_21a006_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
BM2F_ENTERACE(wmsm60_snd_bat)
int f_wmsm60_snd_bat(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
	CModel twmsm60 = CModel("TWMSM60");
	
	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlstr_plan = "";
	CString sqlstr_count = "";
	CString sqlstr_item = "";
	CString sqlstr_condition = "";
	CString sqlstr_load = "";
	CString sqlstr_unload = "";
	CString wm_planid_seq = "";

	/* 数据库操作类定义 */
	CDbCommand comm(conn);
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_item_inq(conn);
	CDbCommand cmd_load_inq(conn);
	CDbCommand cmd_unload_inq(conn);
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
		Log::Trace("", "", "", "", "[{0}]", __LINE__);
		/*关闭倒运计划
		1、计划下所有装车单都完成的置为2；
		2、计划下没有装车单的置为0；
		3、计划下有装车单没有完成的置为1*/
		CDataTable dt;
		sqlstr_plan = "select t.plan_no from twmsm60 t where t.plan_status IN ('0','1')";
		cmd_inq.SetCommandText(sqlstr_plan);
		Log::Trace("", "", "", "", "[{0}]", __LINE__);
		cmd_inq.Parameters.Clear();
		Log::Trace("", "", "", "", "[{0}]", __LINE__);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			Log::Trace("", "", "", "", "[{0}]", __LINE__);
			v_plan_no = cmd_inq.GetString(1);

			sqlstr_item = "select t.plan_no from twmsm61 t where t.plan_no=@plan_no and t.affirm_flag<>'9'";
			cmd_item_inq.SetCommandText(sqlstr_item);
			cmd_item_inq.Parameters.Set("plan_no", v_plan_no);
			cmd_item_inq.ExecuteQuery(dt);
			cmd_item_inq.Close();
			if (dt.Rows.get_Count() > 0)//如果还有未完成的倒运计划，则将倒运计划置为1，则不能继续装车
			{
				twmsm60["PLAN_STATUS"] = "1";
				twmsm60["PLAN_NO"] = v_plan_no;
				twmsm60.Update("PLAN_STATUS", "PLAN_NO");

			}
			else//如果没有未完成的装车单，则关闭该倒运计划
			{
				twmsm60["PLAN_STATUS"] = "2";
				twmsm60["DEAL_FLAG"] = "C";
				twmsm60["PLAN_NO"] = v_plan_no;
				twmsm60.Update("PLAN_STATUS,DEAL_FLAG", "PLAN_NO");
				if (twmsm60["UNLOAD_CODE"].ToString() != "6240ZTXD1")
				{
					bcls_rec->Tables["TWMSM60"].Rows.Add();
					bcls_rec->Tables["TWMSM60"].Rows[cs]["DEAL_FLAG"] = "C";
					bcls_rec->Tables["TWMSM60"].Rows[cs]["PLAN_NO"] = v_plan_no;
				}
				

			}
		}
		cmd_inq.Close();
		Log::Trace("", "", "", "", "[{0}]", __LINE__);
		if (bcls_rec->Tables["TWMSM60"].Rows.get_Count() > 0)
		{
			doFlag = f_wmsm_21a006_snd(bcls_rec, bcls_ret, conn);
			if (doFlag != 0)
			{
				strcpy(s.msg, "关闭倒运计划发送失败！");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			bcls_rec->Tables["TWMSM60"].Rows.Clear();
		}
		Log::Trace("", "", "", "", "[{0}]", __LINE__);
		//开始循环装卸点信息表，生成4*14条新计划
			sqlstr_load = "select t.LOAD_CODE_FACTORY,t.LOAD_CODE_AREA,t.LOAD_CODE,t.LOAD_NAME from TWMSM61A t";
			cmd_load_inq.SetCommandText(sqlstr_load);
			cmd_load_inq.ExecuteReader();
			while (cmd_load_inq.Read())
			{
				Log::Trace("", __FUNCTION__, "LINE[{0}]",__LINE__);
				twmsm60.Reset();
				twmsm60["REC_CREATE_TIME"] = date_time;
				twmsm60["REC_CREATOR"] = s.svc_name;
				twmsm60["DEAL_FLAG"] = "I";					
				twmsm60["GOODS_CODE"] = "DP";
				twmsm60["LOAD_CODE_FACTORY"] = cmd_load_inq.GetString(1);
				twmsm60["LOAD_CODE_AREA"] = cmd_load_inq.GetString(2);
				twmsm60["LOAD_CODE"] = cmd_load_inq.GetString(3);
				twmsm60["LOAD_NAME"] = cmd_load_inq.GetString(4);
				twmsm60["PLAN_WT"] = 10000;
				twmsm60["PLAN_STATUS"] = "0";
				twmsm60["PLAN_START_TIME"] = date_time.Substring(0, 8) + "000000";
				twmsm60["PLAN_END_TIME"] = CDateTime::Now().AddDays(+1).ToString("yyyyMMdd")+"000000";
				Log::Trace("", __FUNCTION__, "LINE[{0}]", __LINE__);
				sqlstr_unload = "select t.UNLOAD_CODE_FACTORY,t.UNLOAD_CODE_AREA,t.UNLOAD_CODE,t.UNLOAD_NAME from TWMSM62A t";
				cmd_unload_inq.SetCommandText(sqlstr_unload);
				Log::Trace("", __FUNCTION__, "LINE[{0}]", __LINE__);
				cmd_unload_inq.ExecuteReader();
				while (cmd_unload_inq.Read())
				{
					Log::Trace("", __FUNCTION__, "LINE[{0}]", __LINE__);
					sqlstr = " SELECT LPAD(TO_CHAR(WM_PLANID_SEQ.NEXTVAL), 4, '0') FROM DUAl ";
				
					twmsm60["UNLOAD_CODE_FACTORY"] = cmd_unload_inq.GetString(1);
					twmsm60["UNLOAD_CODE_AREA"] = cmd_unload_inq.GetString(2);
					twmsm60["UNLOAD_CODE"] = cmd_unload_inq.GetString(3);
					twmsm60["UNLOAD_NAME"] = cmd_unload_inq.GetString(4);
					if (twmsm60["LOAD_CODE"].ToString() == twmsm60["UNLOAD_CODE"].ToString())
					{
						continue;
					}
					
					wm_planid_seq = Db::QueryCString(sqlstr);
					Log::Trace("", __FUNCTION__, "wm_planid_seq[{0}]", wm_planid_seq);
					twmsm60["PLAN_NO"] = CString::Format("%s%s", (const char*)date_time.Substring(0, 12), (const char*)wm_planid_seq);
					twmsm60.Insert();
					if (twmsm60["UNLOAD_CODE"].ToString() != "6240ZTXD1")
					{
						bcls_rec->Tables["TWMSM60"].Rows.Clear();
						bcls_rec->Tables["TWMSM60"].Rows.Add();
						bcls_rec->Tables["TWMSM60"].Rows[0]["PLAN_NO"] = twmsm60["PLAN_NO"].ToString();
					}
				
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

