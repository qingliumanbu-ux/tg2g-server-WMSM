/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2010
Author:			zhouyueqi
Version:		1.0
Date:			2023年5月24日
Description:	炼钢全厂指示初始化
Update:
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"

/* ***** 静态函数申明 ***** */
void get_fosmt95a_EPcode(CString tbl, int row_idx, EIClass* bcls_ret, CDbConnection* conn);//获取EP小代码块
void get_fosmt95a_var_col(CString tbl, EIClass* bcls_ret, CDbConnection* conn);//获取动态表头

BM2F_ENTERACE(fosmt00b_inq)
int f_fosmt00b_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_where = "";
	CString sqlstr_order = "";
	int	TotalRecordCount = 0;

	CPageInfo pageInfo;
	CDbCommand cmd_inq(conn);

	CDecimal page_id = 0;
	CString tbl = "";
	CString date_time = "";
	CDecimal date_interval = 0;

	CModel twmsmfosm95c("TWMSMFOSM95C");

	try
	{
		CDateTime datetime = CDateTime::Now();

		//获取传入参数
		//DATE_TIME必输
		if (bcls_rec->Tables[0].Columns.Contains("DATE_TIME"))
		{
			//查询日期
			date_time = bcls_rec->Tables[0].Rows[0]["DATE_TIME"].ToString();
			Log::Trace("", __FUNCTION__, "date_time[{0}]", date_time);
		}
		else
		{
			strcpy(s.msg, "未传入DATE_TIME！");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//DATE_INTERVAL时间范围可选
		if (bcls_rec->Tables[0].Columns.Contains("DATE_INTERVAL"))
		{
			date_interval = bcls_rec->Tables[0].Rows[0]["DATE_INTERVAL"].ToDecimal();
			Log::Trace("", __FUNCTION__, "date_interval[{0}]", date_interval);
		}
		bcls_ret->Tables[0].Copy(bcls_rec->Tables[0]);

		//PAGE_ID或TABLE_NAME二选一
		if (!bcls_ret->Tables.Contains("TWMSMFOSM95C"))
		{
			bcls_ret->Tables.Add("TWMSMFOSM95C");
		}
		bcls_ret->Tables["TWMSMFOSM95C"].Clone(twmsmfosm95c);
		if (bcls_rec->Tables[0].Columns.Contains("PAGE_ID"))
		{
			//按page查询
			page_id = bcls_rec->Tables[0].Rows[0]["PAGE_ID"].ToDecimal();
			Log::Trace("", __FUNCTION__, "page_id[{0}]", page_id);

			//获取图表配置信息
			if (page_id == 0)
			{
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
					sqlstr =
						" SELECT * FROM TWMSMFOSM95C WHERE 1 = 1"
						" ORDER BY TABLE_NAME"
						;
					break;
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables["TWMSMFOSM95C"]);
				cmd_inq.Close();
			}
			else
			{
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
					sqlstr =
						" SELECT * FROM TWMSMFOSM95C WHERE PAGE_NUM = @PAGE_NUM"
						" ORDER BY TABLE_NAME"
						;
					break;
				}
				cmd_inq.Parameters.Set("PAGE_NUM", page_id);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables["TWMSMFOSM95C"]);
				cmd_inq.Close();
			}
		}
		else if (bcls_rec->Tables[0].Columns.Contains("TABLE_NAME"))
		{
			//按图查询
			tbl = bcls_rec->Tables[0].Rows[0]["TABLE_NAME"].ToString();
			Log::Trace("", __FUNCTION__, "tbl[{0}]", tbl);

			//获取图表配置信息
			twmsmfosm95c["TABLE_NAME"] = tbl;
			twmsmfosm95c.Query("TABLE_NAME");
			twmsmfosm95c.MergeTo(bcls_ret->Tables["TWMSMFOSM95C"]);
		}
		else
		{
			strcpy(s.msg, "未传入PAGE_ID或TABLE_NAME！");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//循环执行图表配置
		for (int i = 0; i < bcls_ret->Tables["TWMSMFOSM95C"].Rows.get_Count(); i++)
		{
			Log::Trace("", __FUNCTION__, "tbl[{0}]", __LINE__, tbl);
			//列配置信息
			tbl = bcls_ret->Tables["TWMSMFOSM95C"].Rows[i]["TABLE_NAME"].ToString();

			bcls_ret->Tables.Add("C_" + tbl);
			bcls_ret->Tables["C_" + tbl].Clone(twmsmfosm95c);
			twmsmfosm95c.MergeFrom(bcls_ret->Tables["TWMSMFOSM95C"].Rows[i]);
			twmsmfosm95c.MergeTo(bcls_ret->Tables["C_" + tbl]);

			bcls_ret->Tables.Add("S_" + tbl);
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr =
					" SELECT * FROM TWMSMFOSM95A WHERE TABLE_NAME = @TABLE_NAME"
					" ORDER BY SEQ_NO"
					;
				break;
			}
			cmd_inq.Parameters.Set("TABLE_NAME", tbl);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteQuery(bcls_ret->Tables["S_" + tbl]);
			cmd_inq.Close();

			for (int j = 0; j < bcls_ret->Tables["S_" + tbl].Rows.get_Count(); j++)
			{
				if (bcls_ret->Tables["S_" + tbl].Rows[j]["ITEM_TYPE"].ToString() == "5"
					|| bcls_ret->Tables["S_" + tbl].Rows[j]["ITEM_TYPE"].ToString().SubstringNE(2, 1) == "5")
				{
					//EP小代码
					get_fosmt95a_EPcode(tbl, j, bcls_ret, conn);
				}
				else if (bcls_ret->Tables["S_" + tbl].Rows[j]["ITEM_TYPE"].ToString() == "6"
					|| bcls_ret->Tables["S_" + tbl].Rows[j]["ITEM_TYPE"].ToString().SubstringNE(2, 1) == "6")
				{
					//SQL小代码
				}
			}

			//列配置规则信息
			bcls_ret->Tables.Add("R_" + tbl);
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr =
					" SELECT * FROM TWMSMFOSM95B WHERE TABLE_NAME = @TABLE_NAME"
					" ORDER BY SEQ_NO"
					;
				break;
			}
			cmd_inq.Parameters.Set("TABLE_NAME", tbl);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteQuery(bcls_ret->Tables["R_" + tbl]);
			cmd_inq.Close();

			//执行内容查询
			if (false)
			{
				//模板
				bcls_ret->Tables.Add(tbl);
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
					sqlstr =
						""
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();
			}
			else if (tbl == "TFOSMT01")//早会安排工作完成情况
			{

				bcls_ret->Tables.Add(tbl);
				sqlstr = " select to_char(ROWNUM) row_no,PROD_DATE,BACK_S1,BACK_S2,BACK_S3,BACK_S4,BACK_S5,BACK_S6 from TWMSMFOSM01 where PAGE_ID='"+ tbl +"' and  PROD_DATE= to_char(to_date('" + date_time + "','yyyyMMdd')-1,'yyyyMMdd') ";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();
			}
			else if (tbl == "TFOSMT02")//安全环保
			{

				bcls_ret->Tables.Add(tbl);
				sqlstr = " select BACK_S1,BACK_S2,BACK_S3 from TWMSMFOSM01 where PAGE_ID='" + tbl + "' and PROD_DATE= '" + date_time + "' ";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();
			}
			else if (tbl == "TFOSMT04")//生产总述
			{

				bcls_ret->Tables.Add(tbl);
				sqlstr = " select BACK_S1,BACK_S2,BACK_S3,BACK_S4,BACK_S5,BACK_S6 from TWMSMFOSM01 where PAGE_ID='" + tbl + "' and PROD_DATE= '" + date_time + "' ";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();
			}
			else if (tbl == "TFOSMT041")//生产总述
			{

				bcls_ret->Tables.Add(tbl);
				sqlstr = " select substr2(PROD_DATE,7,2)||'日'  PROD_DATE,BACK_S1,BACK_S2,BACK_S3,BACK_S4,BACK_S5,BACK_S6 from TWMSMFOSM01 where PAGE_ID='" + tbl + "' and PROD_DATE= '" + date_time + "' ";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();
			}
			else if (tbl == "TFOSMT05")//关键指标监控
			{

				bcls_ret->Tables.Add(tbl);
				sqlstr = " select BACK_S1,BACK_S2,BACK_S3,BACK_S4,BACK_S5 from TWMSMFOSM01 where PAGE_ID='" + tbl + "' and PROD_DATE= '" + date_time + "' ";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();
			}
			

			get_fosmt95a_var_col(tbl, bcls_ret, conn);
		}


		//返回提示栏信息
		CString ts = ((CDecimal)(CDateTime::Now() - datetime).TotalMilliseconds()).Round(0).ToString();
		CFormattable arguments[] = { ts };
		CMessageFormat::Format(s.msg, "数据读取成功！SVC用时[{0}ms]", arguments, 1);
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
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

void get_fosmt95a_EPcode(CString tbl, int row_idx, EIClass* bcls_ret, CDbConnection* conn)
{
	CString sqlstr = "";
	CString sqlstr_where = "";
	CString sqlstr_order = "";
	CDbCommand cmd_inq(conn);

	CString ep_code = bcls_ret->Tables["S_" + tbl].Rows[row_idx]["TEXT_FORMAT"].ToString();
	//Log::Trace("", __FUNCTION__, "ep_code[{0}]", ep_code);

	CString tbl_ep = "S_" + tbl + "_" + bcls_ret->Tables["S_" + tbl].Rows[row_idx]["COL_SEQ"].ToString() + "_" + ep_code;
	Log::Trace("", __FUNCTION__, "tbl_ep[{0}]", tbl_ep);
	bcls_ret->Tables.Add(tbl_ep);
	switch (conn->DatabaseKind)
	{
	case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
	case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
	case DB_KIND_MSSQL:				// MS SQL Server数据库
	case DB_KIND_ORACLE:	        // Oracle 数据库
	default:
		sqlstr =
			" SELECT CODE, CODE_DESC_1_CONTENT FROM TEP0002 WHERE CODE_CLASS = @CODE_CLASS"
			;
		break;
	}
	cmd_inq.SetCommandText(sqlstr);
	cmd_inq.Parameters.Set("CODE_CLASS", ep_code);
	sqlstr_where = bcls_ret->Tables["S_" + tbl].Rows[row_idx]["SQL_CONTEXT_01"].ToString().Trim();
	sqlstr_order = bcls_ret->Tables["S_" + tbl].Rows[row_idx]["SQL_CONTEXT_02"].ToString().Trim();
	if (sqlstr_where.GetLength() > 0)
	{
		sqlstr += " AND " + sqlstr_where;
	}
	if (sqlstr_order.GetLength() > 0)
	{
		sqlstr += " ORDER BY " + sqlstr_order;
	}
	cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl_ep]);
	cmd_inq.Close();
}

void get_fosmt95a_var_col(CString tbl, EIClass* bcls_ret, CDbConnection* conn)
{
	CString sqlstr = "";
	CString sqlstr_where = "";
	CString sqlstr_order = "";
	CDbCommand cmd_inq(conn);

	if (false)
	{
		for (int i = 0; i < bcls_ret->Tables["S_" + tbl].Rows.get_Count(); i++)
		{
			if (bcls_ret->Tables["S_" + tbl].Rows[i]["ITEM_TYPE"].ToString() == "4")
			{
				bcls_ret->Tables["S_" + tbl].Rows[i]["ITEM_CNAME"] = "";
			}
		}
	}
	else if (tbl == "TFOSMT03A")
	{
		for (int i = 0; i < bcls_ret->Tables["S_" + tbl].Rows.get_Count(); i++)
		{
			if (bcls_ret->Tables["S_" + tbl].Rows[i]["ITEM_TYPE"].ToString() == "4")
			{
				bcls_ret->Tables["S_" + tbl].Rows[i]["ITEM_CNAME"] =
					bcls_ret->Tables[tbl].Rows[3][bcls_ret->Tables["S_" + tbl].Rows[i]["TEXT_FORMAT"].ToString()].ToString();
			}
		}
	}
	else if (tbl == "TFOSMT03G")
	{
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			sqlstr =
				" SELECT CODE, CODE_DESC_1_CONTENT FROM TEP0002"
				" WHERE CODE_CLASS = @CODE_CLASS AND CODE_DESC_3_CONTENT <> ' '"
				" ORDER BY CODE_DESC_3_CONTENT"
				;
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("CODE_CLASS", "FOSMT1");
		EIClass blk;
		cmd_inq.ExecuteQuery(blk.Tables[0]);

		for (int i = 0; i < bcls_ret->Tables["S_" + tbl].Rows.get_Count(); i++)
		{
			if (bcls_ret->Tables["S_" + tbl].Rows[i]["ITEM_TYPE"].ToString() == "4")
			{
				for (int j = 0; j < blk.Tables[0].Rows.get_Count(); j++)
				{
					if (blk.Tables[0].Rows[j]["CODE"].ToString() == bcls_ret->Tables["S_" + tbl].Rows[i]["TEXT_FORMAT"].ToString())
					{
						bcls_ret->Tables["S_" + tbl].Rows[i]["ITEM_CNAME"] = blk.Tables[0].Rows[j]["CODE_DESC_1_CONTENT"].ToString();
					}
				}
			}
		}
	}
	else if (tbl == "TFOSMT06")
	{
		//date_time
		CDateTime dt = CDateTime::Parse(bcls_ret->Tables[0].Rows[0]["DATE_TIME"].ToString());
		CString dt1 = dt.AddDays(-1).ToString("dd");
		CString dt2 = dt.AddDays(-2).ToString("dd");
		CString dt3 = dt.AddDays(-3).ToString("dd");
		CString dt4 = dt.AddDays(-4).ToString("dd");
		CString dt5 = dt.AddDays(-5).ToString("dd");

		for (int i = 0; i < bcls_ret->Tables["S_" + tbl].Rows.get_Count(); i++)
		{
			if (bcls_ret->Tables["S_" + tbl].Rows[i]["ITEM_TYPE"].ToString() == "4")
			{
				if (bcls_ret->Tables["S_" + tbl].Rows[i]["TEXT_FORMAT"].ToString() == "YYYY")
				{
					bcls_ret->Tables["S_" + tbl].Rows[i]["ITEM_CNAME"] = dt.AddDays(-1).ToString("yyyy") + "年";
				}
				if (bcls_ret->Tables["S_" + tbl].Rows[i]["TEXT_FORMAT"].ToString() == "-1")
				{
					bcls_ret->Tables["S_" + tbl].Rows[i]["ITEM_CNAME"] = dt1;
				}
				if (bcls_ret->Tables["S_" + tbl].Rows[i]["TEXT_FORMAT"].ToString() == "-2")
				{
					bcls_ret->Tables["S_" + tbl].Rows[i]["ITEM_CNAME"] = dt2;
				}
				if (bcls_ret->Tables["S_" + tbl].Rows[i]["TEXT_FORMAT"].ToString() == "-3")
				{
					bcls_ret->Tables["S_" + tbl].Rows[i]["ITEM_CNAME"] = dt3;
				}
				if (bcls_ret->Tables["S_" + tbl].Rows[i]["TEXT_FORMAT"].ToString() == "-4")
				{
					bcls_ret->Tables["S_" + tbl].Rows[i]["ITEM_CNAME"] = dt4;
				}
				if (bcls_ret->Tables["S_" + tbl].Rows[i]["TEXT_FORMAT"].ToString() == "-5")
				{
					bcls_ret->Tables["S_" + tbl].Rows[i]["ITEM_CNAME"] = dt5;
				}
			}
		}
	}
	else if (tbl == "TFOSMT08")
	{
		CDateTime dt = CDateTime::Parse(bcls_ret->Tables[0].Rows[0]["DATE_TIME"].ToString());
		CString dt1 = dt.AddDays(-1).ToString("yyyy年MM月dd日");

		for (int i = 0; i < bcls_ret->Tables["S_" + tbl].Rows.get_Count(); i++)
		{
			if (bcls_ret->Tables["S_" + tbl].Rows[i]["ITEM_TYPE"].ToString() == "4")
			{
				bcls_ret->Tables["S_" + tbl].Rows[i]["ITEM_CNAME"] = dt1;
			}
		}
	}
	else if (tbl == "TFOSMT09")
	{
		CDateTime dt = CDateTime::Parse(bcls_ret->Tables[0].Rows[0]["DATE_TIME"].ToString());
		CString dt1 = dt.AddDays(-1).AddYears(-1).ToString("yyyy年实绩").SubstringNE(2);
		CString dt2 = dt.AddDays(-1).ToString("yyyy年目标").SubstringNE(2);
		CString dt3 = dt.AddDays(-1).ToString("dd");
		CString dt4 = dt.AddDays(-2).ToString("dd");
		CString dt5 = dt.AddDays(-3).ToString("dd");

		for (int i = 0; i < bcls_ret->Tables["S_" + tbl].Rows.get_Count(); i++)
		{
			if (bcls_ret->Tables["S_" + tbl].Rows[i]["ITEM_TYPE"].ToString() == "4")
			{
				if (bcls_ret->Tables["S_" + tbl].Rows[i]["TEXT_FORMAT"].ToString() == "YYYY-1")
				{
					bcls_ret->Tables["S_" + tbl].Rows[i]["ITEM_CNAME"] = dt1;
				}
				if (bcls_ret->Tables["S_" + tbl].Rows[i]["TEXT_FORMAT"].ToString() == "YYYY")
				{
					bcls_ret->Tables["S_" + tbl].Rows[i]["ITEM_CNAME"] = dt2;
				}
				if (bcls_ret->Tables["S_" + tbl].Rows[i]["TEXT_FORMAT"].ToString() == "-1")
				{
					bcls_ret->Tables["S_" + tbl].Rows[i]["ITEM_CNAME"] = dt3;
				}
				if (bcls_ret->Tables["S_" + tbl].Rows[i]["TEXT_FORMAT"].ToString() == "-2")
				{
					bcls_ret->Tables["S_" + tbl].Rows[i]["ITEM_CNAME"] = dt4;
				}
				if (bcls_ret->Tables["S_" + tbl].Rows[i]["TEXT_FORMAT"].ToString() == "-3")
				{
					bcls_ret->Tables["S_" + tbl].Rows[i]["ITEM_CNAME"] = dt5;
				}
			}
		}
	}
	else if (tbl == "TFOSMT10H")
	{
		CDateTime dt = CDateTime::Parse(bcls_ret->Tables[0].Rows[0]["DATE_TIME"].ToString());
		CString dt1 = dt.AddDays(-1).ToString("yyyy年MM月") + "产量累计(t)";

		for (int i = 0; i < bcls_ret->Tables["S_" + tbl].Rows.get_Count(); i++)
		{
			if (bcls_ret->Tables["S_" + tbl].Rows[i]["ITEM_TYPE"].ToString() == "4")
			{
				bcls_ret->Tables["S_" + tbl].Rows[i]["ITEM_CNAME"] = dt1;
			}
		}
	}
	else if (tbl == "TFOSMT11")
	{
		CDateTime dt = CDateTime::Parse(bcls_ret->Tables[0].Rows[0]["DATE_TIME"].ToString());
		CString dt1 = dt.AddDays(-dt.AddDays(-1).DayOfWeek() - 1).ToString("dd");
		CString dt2 = dt.AddDays(-dt.AddDays(-1).DayOfWeek() + 0).ToString("dd");
		CString dt3 = dt.AddDays(-dt.AddDays(-1).DayOfWeek() + 1).ToString("dd");
		CString dt4 = dt.AddDays(-dt.AddDays(-1).DayOfWeek() + 2).ToString("dd");
		CString dt5 = dt.AddDays(-dt.AddDays(-1).DayOfWeek() + 3).ToString("dd");
		CString dt6 = dt.AddDays(-dt.AddDays(-1).DayOfWeek() + 4).ToString("dd");
		CString dt7 = dt.AddDays(-dt.AddDays(-1).DayOfWeek() + 5).ToString("dd");
		Log::Trace("", __FUNCTION__, "dt1[{0}]", dt1);

		for (int i = 0; i < bcls_ret->Tables["S_" + tbl].Rows.get_Count(); i++)
		{
			if (bcls_ret->Tables["S_" + tbl].Rows[i]["ITEM_TYPE"].ToString() == "4")
			{
				if (bcls_ret->Tables["S_" + tbl].Rows[i]["TEXT_FORMAT"].ToString() == "W1")
				{
					bcls_ret->Tables["S_" + tbl].Rows[i]["ITEM_CNAME"] = dt1;
				}
				if (bcls_ret->Tables["S_" + tbl].Rows[i]["TEXT_FORMAT"].ToString() == "W2")
				{
					bcls_ret->Tables["S_" + tbl].Rows[i]["ITEM_CNAME"] = dt2;
				}
				if (bcls_ret->Tables["S_" + tbl].Rows[i]["TEXT_FORMAT"].ToString() == "W3")
				{
					bcls_ret->Tables["S_" + tbl].Rows[i]["ITEM_CNAME"] = dt3;
				}
				if (bcls_ret->Tables["S_" + tbl].Rows[i]["TEXT_FORMAT"].ToString() == "W4")
				{
					bcls_ret->Tables["S_" + tbl].Rows[i]["ITEM_CNAME"] = dt4;
				}
				if (bcls_ret->Tables["S_" + tbl].Rows[i]["TEXT_FORMAT"].ToString() == "W5")
				{
					bcls_ret->Tables["S_" + tbl].Rows[i]["ITEM_CNAME"] = dt5;
				}
				if (bcls_ret->Tables["S_" + tbl].Rows[i]["TEXT_FORMAT"].ToString() == "W6")
				{
					bcls_ret->Tables["S_" + tbl].Rows[i]["ITEM_CNAME"] = dt6;
				}
				if (bcls_ret->Tables["S_" + tbl].Rows[i]["TEXT_FORMAT"].ToString() == "W7")
				{
					bcls_ret->Tables["S_" + tbl].Rows[i]["ITEM_CNAME"] = dt7;
				}
			}
		}
	}
	else if (tbl == "TFOSMT10J")
	{
		CDateTime dt = CDateTime::Parse(bcls_ret->Tables[0].Rows[0]["DATE_TIME"].ToString());
		CString dt1 = dt.AddDays(-7).ToString("MM月dd日");
		CString dt2 = dt.AddDays(-6).ToString("MM月dd日");
		CString dt3 = dt.AddDays(-5).ToString("MM月dd日");
		CString dt4 = dt.AddDays(-4).ToString("MM月dd日");
		CString dt5 = dt.AddDays(-3).ToString("MM月dd日");
		CString dt6 = dt.AddDays(-2).ToString("MM月dd日");
		CString dt7 = dt.AddDays(-1).ToString("MM月dd日");


		for (int i = 0; i < bcls_ret->Tables["S_" + tbl].Rows.get_Count(); i++)
		{
			if (bcls_ret->Tables["S_" + tbl].Rows[i]["ITEM_TYPE"].ToString() == "4")
			{
				if (bcls_ret->Tables["S_" + tbl].Rows[i]["TEXT_FORMAT"].ToString() == "-7")
				{
					bcls_ret->Tables["S_" + tbl].Rows[i]["ITEM_CNAME"] = dt1;
				}
				if (bcls_ret->Tables["S_" + tbl].Rows[i]["TEXT_FORMAT"].ToString() == "-6")
				{
					bcls_ret->Tables["S_" + tbl].Rows[i]["ITEM_CNAME"] = dt2;
				}
				if (bcls_ret->Tables["S_" + tbl].Rows[i]["TEXT_FORMAT"].ToString() == "-5")
				{
					bcls_ret->Tables["S_" + tbl].Rows[i]["ITEM_CNAME"] = dt3;
				}
				if (bcls_ret->Tables["S_" + tbl].Rows[i]["TEXT_FORMAT"].ToString() == "-4")
				{
					bcls_ret->Tables["S_" + tbl].Rows[i]["ITEM_CNAME"] = dt4;
				}
				if (bcls_ret->Tables["S_" + tbl].Rows[i]["TEXT_FORMAT"].ToString() == "-3")
				{
					bcls_ret->Tables["S_" + tbl].Rows[i]["ITEM_CNAME"] = dt5;
				}
				if (bcls_ret->Tables["S_" + tbl].Rows[i]["TEXT_FORMAT"].ToString() == "-2")
				{
					bcls_ret->Tables["S_" + tbl].Rows[i]["ITEM_CNAME"] = dt6;
				}
				if (bcls_ret->Tables["S_" + tbl].Rows[i]["TEXT_FORMAT"].ToString() == "-1")
				{
					bcls_ret->Tables["S_" + tbl].Rows[i]["ITEM_CNAME"] = dt7;
				}
			}
		}
	}
}