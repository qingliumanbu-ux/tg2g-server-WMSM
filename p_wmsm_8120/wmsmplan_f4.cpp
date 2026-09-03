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

BM2F_ENTERACE(wmsmplan_f4);

int f_wmsmplan_f4(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
		


		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			twmsm60.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			if (twmsm60["DEAL_FLAG"].ToString() != "I")
			{
				sprintf(s.msg, "当前计划[%s]关闭，不可重复关闭！", (const char*)twmsm60["PLAN_NO"]);
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			sqlstr_item = "select t.plan_no from twmsm61 t where t.plan_no=@plan_no and t.affirm_flag<>'9'";
			cmd_item_inq.SetCommandText(sqlstr_item);
			cmd_item_inq.Parameters.Set("plan_no", twmsm60["PLAN_NO"].ToString());
			cmd_item_inq.ExecuteQuery(dt);
			cmd_item_inq.Close();
			if (dt.Rows.get_Count() > 0)//如果还有未完成的倒运计划，则将倒运计划置为1，则不能继续装车
			{
				twmsm60["PLAN_STATUS"] = "1";
				twmsm60["PLAN_NO"] = twmsm60["PLAN_NO"].ToString();
				twmsm60.Update("PLAN_STATUS", "PLAN_NO");

			}
			else//如果没有未完成的装车单，则关闭该倒运计划
			{
				twmsm60["PLAN_STATUS"] = "2";
				twmsm60["DEAL_FLAG"] = "C";
				twmsm60["PLAN_NO"] = twmsm60["PLAN_NO"].ToString();
				twmsm60.Update("PLAN_STATUS,DEAL_FLAG", "PLAN_NO");
				bcls_rec->Tables["TWMSM60"].Rows.Add();
				bcls_rec->Tables["TWMSM60"].Rows[cs]["DEAL_FLAG"] = "C";
				bcls_rec->Tables["TWMSM60"].Rows[cs]["PLAN_NO"] = twmsm60["PLAN_NO"].ToString();

			}
			cmd_item_inq.Close();

			
		}
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