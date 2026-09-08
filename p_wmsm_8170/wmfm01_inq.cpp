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

	CModel tfosmt95c("TFOSMT95C");

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
		if (!bcls_ret->Tables.Contains("TFOSMT95C"))
		{
			bcls_ret->Tables.Add("TFOSMT95C");
		}
		bcls_ret->Tables["TFOSMT95C"].Clone(tfosmt95c);
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
						" SELECT * FROM TFOSMT95C WHERE 1 = 1"
						" ORDER BY TABLE_NAME"
						;
					break;
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables["TFOSMT95C"]);
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
						" SELECT * FROM TFOSMT95C WHERE PAGE_NUM = @PAGE_NUM"
						" ORDER BY TABLE_NAME"
						;
					break;
				}
				cmd_inq.Parameters.Set("PAGE_NUM", page_id);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables["TFOSMT95C"]);
				cmd_inq.Close();
			}
		}
		else if (bcls_rec->Tables[0].Columns.Contains("TABLE_NAME"))
		{
			//按图查询
			tbl = bcls_rec->Tables[0].Rows[0]["TABLE_NAME"].ToString();
			Log::Trace("", __FUNCTION__, "tbl[{0}]", tbl);

			//获取图表配置信息
			tfosmt95c["TABLE_NAME"] = tbl;
			tfosmt95c.Query("TABLE_NAME");
			tfosmt95c.MergeTo(bcls_ret->Tables["TFOSMT95C"]);
		}
		else
		{
			strcpy(s.msg, "未传入PAGE_ID或TABLE_NAME！");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//循环执行图表配置
		for (int i = 0; i < bcls_ret->Tables["TFOSMT95C"].Rows.get_Count(); i++)
		{
			Log::Trace("", __FUNCTION__, "tbl[{0}]", __LINE__, tbl);
			//列配置信息
			tbl = bcls_ret->Tables["TFOSMT95C"].Rows[i]["TABLE_NAME"].ToString();

			bcls_ret->Tables.Add("C_" + tbl);
			bcls_ret->Tables["C_" + tbl].Clone(tfosmt95c);
			tfosmt95c.MergeFrom(bcls_ret->Tables["TFOSMT95C"].Rows[i]);
			tfosmt95c.MergeTo(bcls_ret->Tables["C_" + tbl]);

			bcls_ret->Tables.Add("S_" + tbl);
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr =
					" SELECT * FROM TFOSMT95A WHERE TABLE_NAME = @TABLE_NAME"
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
					" SELECT * FROM TFOSMT95B WHERE TABLE_NAME = @TABLE_NAME"
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
				sqlstr = " select ROWNUM row_no,BACK_S1,BACK_S2,BACK_S3,BACK_S4,BACK_S5,BACK_S6 from TWMSMFOSM01 where PROD_DATE= to_char(to_date('" + date_time + "','yyyyMMdd')-1,'yyyyMMdd') ";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();
			}
			else if (tbl == "TFOSMT02")
			{
				//稳定周期
				bcls_ret->Tables.Add(tbl);
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-03：取查询日期前推1日的日期字符串;日历日边界、参数与结果列保持不变。
// 改写原因：DB2 日期天数后缀改为整数天运算；DAYS() 天数差改用 DATEDIFF(DAY,起点,终点)；空值搜索 DECODE 改为标准 CASE,不依赖 NULL 相等匹配的未记载语义；二元标量 MAX 改为空值守卫的 GREATEST,空值传播与 DB2 标量 MAX 一致；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " SELECT A.CODE DEP_NAME, C.BREAKDN_DATE, C.REMARK REMARK_BFR, B.REMARK,"
						// " DECODE(B.BREAKDN_DATE, NULL, DAYS(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS) - DAYS(TO_DATE(C.BREAKDN_DATE,'YYYYMMDD')), 0) OPERATE_CYCLE,"
						// " MAX(C.TOTAL_OPERATE_CYCLE, DECODE(B.REMARK, NULL, DAYS(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS) - DAYS(TO_DATE(C.BREAKDN_DATE,'YYYYMMDD')), B.TOTAL_OPERATE_CYCLE)) TOTAL_OPERATE_CYCLE"
						// " FROM TEP0002 A"
						// " LEFT JOIN TFOSMT02A B ON A.CODE = B.DEP_NAME AND B.BREAKDN_DATE = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS,'YYYYMMDD')"
						// " LEFT JOIN"
						// " (SELECT DEP_NAME, REMARK, BREAKDN_DATE, TOTAL_OPERATE_CYCLE FROM TFOSMT02A"
						// " WHERE (DEP_NAME, BREAKDN_DATE) IN"
						// " (SELECT DEP_NAME, MAX(BREAKDN_DATE) FROM TFOSMT02A WHERE BREAKDN_DATE < TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS,'YYYYMMDD') GROUP BY DEP_NAME)"
						// " ) C ON A.CODE = C.DEP_NAME"
						// " LEFT JOIN TFOSMT02B D ON A.CODE = D.DEP_NAME"
						// " WHERE A.CODE_CLASS = 'FOSMT1'"
						// " AND A.CODE_DESC_2_CONTENT != ' '"
						// " ORDER BY A.CODE_DESC_2_CONTENT"
						// ;
// DM8 SQL：
					sqlstr =
						" SELECT A.CODE DEP_NAME, C.BREAKDN_DATE, C.REMARK REMARK_BFR, B.REMARK,"
						" CASE WHEN B.BREAKDN_DATE IS NULL THEN DATEDIFF(DAY, TO_DATE(C.BREAKDN_DATE,'YYYYMMDD'), TO_DATE(@DATE_TIME,'YYYYMMDD') - 1) ELSE 0 END OPERATE_CYCLE,"
						" CASE WHEN C.TOTAL_OPERATE_CYCLE IS NULL OR CASE WHEN B.REMARK IS NULL THEN DATEDIFF(DAY, TO_DATE(C.BREAKDN_DATE,'YYYYMMDD'), TO_DATE(@DATE_TIME,'YYYYMMDD') - 1) ELSE B.TOTAL_OPERATE_CYCLE END IS NULL THEN NULL ELSE GREATEST(C.TOTAL_OPERATE_CYCLE, CASE WHEN B.REMARK IS NULL THEN DATEDIFF(DAY, TO_DATE(C.BREAKDN_DATE,'YYYYMMDD'), TO_DATE(@DATE_TIME,'YYYYMMDD') - 1) ELSE B.TOTAL_OPERATE_CYCLE END) END TOTAL_OPERATE_CYCLE"
						" FROM TEP0002 A"
						" LEFT JOIN TFOSMT02A B ON A.CODE = B.DEP_NAME AND B.BREAKDN_DATE = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMMDD')"
						" LEFT JOIN"
						" (SELECT DEP_NAME, REMARK, BREAKDN_DATE, TOTAL_OPERATE_CYCLE FROM TFOSMT02A"
						" WHERE (DEP_NAME, BREAKDN_DATE) IN"
						" (SELECT DEP_NAME, MAX(BREAKDN_DATE) FROM TFOSMT02A WHERE BREAKDN_DATE < TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMMDD') GROUP BY DEP_NAME)"
						" ) C ON A.CODE = C.DEP_NAME"
						" LEFT JOIN TFOSMT02B D ON A.CODE = D.DEP_NAME"
						" WHERE A.CODE_CLASS = 'FOSMT1'"
						" AND A.CODE_DESC_2_CONTENT != ' '"
						" ORDER BY A.CODE_DESC_2_CONTENT"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();
			}
			else if (tbl == "TFOSMT03A")
			{
				//本月生产情况
				bcls_ret->Tables.Add(tbl);
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
					// DM8 适配 CHANGE-02：生成查询日期之前的三天，保留原连接、排序和参数。
					// 改写原因：日期加减改用整数天，SYSIBM.SYSDUMMY1 改为 DUAL；YYYYMMDD 输入尚未实测。
					// 本共用分支面向 DM8，其他 DB_KIND 标签也会执行此 SQL；递归查询尚未实测。
					// 原 SQL（完整保留）：
					// sqlstr =
						// " WITH A(LEVEL, TIME) AS"
						// " (SELECT 1, TO_DATE(@DATE_TIME,'YYYYMMDD') - 3 DAY"
						// " FROM SYSIBM.SYSDUMMY1"
						// " WHERE 1 = 1"
						// " UNION ALL"
						// " SELECT LEVEL + 1,TIME + 1 DAY"
						// " FROM SYSIBM.SYSDUMMY1, A"
						// " WHERE A.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY)"
						// " SELECT TO_CHAR(A.TIME,'YYYYMMDD') DATE_TIME, B.*"
						// " FROM A"
						// " LEFT JOIN TFOSMT03A B ON TO_CHAR(A.TIME,'YYYYMMDD') = B.DATE_TIME"
						// " WHERE 1 = 1"
						// " ORDER BY TO_CHAR(A.TIME,'YYYYMMDD')"
						// ;
					// DM8 SQL：
					sqlstr =
						" WITH A(LEVEL, TIME) AS"
						" (SELECT 1, TO_DATE(@DATE_TIME,'YYYYMMDD') - 3"
						" FROM DUAL"
						" WHERE 1 = 1"
						" UNION ALL"
						" SELECT LEVEL + 1,TIME + 1"
						" FROM DUAL, A"
						" WHERE A.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD') - 1)"
						" SELECT TO_CHAR(A.TIME,'YYYYMMDD') DATE_TIME, B.*"
						" FROM A"
						" LEFT JOIN TFOSMT03A B ON TO_CHAR(A.TIME,'YYYYMMDD') = B.DATE_TIME"
						" WHERE 1 = 1"
						" ORDER BY TO_CHAR(A.TIME,'YYYYMMDD')"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();

				//合计行
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-04：取查询日期前推1日的日期字符串;日历日边界、参数与结果列保持不变。
// 改写原因：DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " SELECT"
						// " SUM(PLAN_CHARGE_1) PLAN_CHARGE_1, SUM(PLAN_CHARGE_2) PLAN_CHARGE_2,"
						// " SUM(PRODUCT_CHARGE_1) PRODUCT_CHARGE_1, SUM(PRODUCT_CHARGE_2) PRODUCT_CHARGE_2,"
						// " SUM(SLAB_WT_1) SLAB_WT_1, SUM(SLAB_WT_2) SLAB_WT_2,"
						// " SUM(CC_REMAIN_NUM_1) CC_REMAIN_NUM_1, SUM(CC_REMAIN_NUM_2) CC_REMAIN_NUM_2,"
						// " SUM(TOTAL_SLAB_WT) TOTAL_SLAB_WT, SUM(PLAN_PROD_DAY) PLAN_PROD_DAY, SUM(REAL_PRODUCT_DAY) REAL_PRODUCT_DAY,"
						// " DECODE(SUM(PRODUCT_CHARGE_1), 0, 0, ROUND(100.0 * SUM(CC_REMAIN_NUM_1) / SUM(PRODUCT_CHARGE_1),1)) CC_REMAIN_RATE_1,"
						// " DECODE(SUM(PRODUCT_CHARGE_2), 0, 0, ROUND(100.0 * SUM(CC_REMAIN_NUM_2) / SUM(PRODUCT_CHARGE_2),1)) CC_REMAIN_RATE_2,"
						// " 1.000 * SUM(SLAB_WT_2) / SUM(TOTAL_SLAB_WT) PRODUCT_RATE,"
						// " MAX(TIME_PROGRESS) TIME_PROGRESS,"
						// " DECODE(MAX(CAST(B1.REMARK AS INTEGER) + CAST(B2.REMARK AS INTEGER)), 0, 0, SUM(TOTAL_SLAB_WT) / MAX(CAST(B1.REMARK AS INTEGER) + CAST(B2.REMARK AS INTEGER)) * 100) PROD_PROGRESS,"
						// " DECODE(MAX(CAST(B1.REMARK AS INTEGER)), 0, 0, SUM(SLAB_WT_1) / MAX(CAST(B1.REMARK AS INTEGER)) * 100) PROD_PROGRESS_1,"
						// " DECODE(MAX(CAST(B2.REMARK AS INTEGER)), 0, 0, SUM(SLAB_WT_2) / MAX(CAST(B2.REMARK AS INTEGER)) * 100) PROD_PROGRESS_2,"
						// " MAX(B1.REMARK) PLAN_SLAB_WT_1, MAX(B2.REMARK) PLAN_SLAB_WT_2,"
						// " MAX(CAST(B1.REMARK AS INTEGER) + CAST(B2.REMARK AS INTEGER)) PLAN_SLAB_WT,"
						// " '合计' DATE_TIME"
						// " FROM TFOSMT03A A"
						// " LEFT JOIN TFOSMT96B B1 ON B1.DATE_TIME = SUBSTR(TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY, 'YYYYMMDD'), 1, 6)"
						// " AND B1.ITEM_ENAME = 'FOSMT03-107'"
						// " LEFT JOIN TFOSMT96B B2 ON B2.DATE_TIME = SUBSTR(TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY, 'YYYYMMDD'), 1, 6)"
						// " AND B2.ITEM_ENAME = 'FOSMT03-108'"
						// " WHERE 1 = 1"
						// " AND A.DATE_TIME < @DATE_TIME"
						// " AND A.DATE_TIME >= SUBSTR(TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY, 'YYYYMMDD'), 1, 6) || '01'"
						// ;
// DM8 SQL：
					sqlstr =
						" SELECT"
						" SUM(PLAN_CHARGE_1) PLAN_CHARGE_1, SUM(PLAN_CHARGE_2) PLAN_CHARGE_2,"
						" SUM(PRODUCT_CHARGE_1) PRODUCT_CHARGE_1, SUM(PRODUCT_CHARGE_2) PRODUCT_CHARGE_2,"
						" SUM(SLAB_WT_1) SLAB_WT_1, SUM(SLAB_WT_2) SLAB_WT_2,"
						" SUM(CC_REMAIN_NUM_1) CC_REMAIN_NUM_1, SUM(CC_REMAIN_NUM_2) CC_REMAIN_NUM_2,"
						" SUM(TOTAL_SLAB_WT) TOTAL_SLAB_WT, SUM(PLAN_PROD_DAY) PLAN_PROD_DAY, SUM(REAL_PRODUCT_DAY) REAL_PRODUCT_DAY,"
						" DECODE(SUM(PRODUCT_CHARGE_1), 0, 0, ROUND(100.0 * SUM(CC_REMAIN_NUM_1) / SUM(PRODUCT_CHARGE_1),1)) CC_REMAIN_RATE_1,"
						" DECODE(SUM(PRODUCT_CHARGE_2), 0, 0, ROUND(100.0 * SUM(CC_REMAIN_NUM_2) / SUM(PRODUCT_CHARGE_2),1)) CC_REMAIN_RATE_2,"
						" 1.000 * SUM(SLAB_WT_2) / SUM(TOTAL_SLAB_WT) PRODUCT_RATE,"
						" MAX(TIME_PROGRESS) TIME_PROGRESS,"
						" DECODE(MAX(CAST(B1.REMARK AS INTEGER) + CAST(B2.REMARK AS INTEGER)), 0, 0, SUM(TOTAL_SLAB_WT) / MAX(CAST(B1.REMARK AS INTEGER) + CAST(B2.REMARK AS INTEGER)) * 100) PROD_PROGRESS,"
						" DECODE(MAX(CAST(B1.REMARK AS INTEGER)), 0, 0, SUM(SLAB_WT_1) / MAX(CAST(B1.REMARK AS INTEGER)) * 100) PROD_PROGRESS_1,"
						" DECODE(MAX(CAST(B2.REMARK AS INTEGER)), 0, 0, SUM(SLAB_WT_2) / MAX(CAST(B2.REMARK AS INTEGER)) * 100) PROD_PROGRESS_2,"
						" MAX(B1.REMARK) PLAN_SLAB_WT_1, MAX(B2.REMARK) PLAN_SLAB_WT_2,"
						" MAX(CAST(B1.REMARK AS INTEGER) + CAST(B2.REMARK AS INTEGER)) PLAN_SLAB_WT,"
						" '合计' DATE_TIME"
						" FROM TFOSMT03A A"
						" LEFT JOIN TFOSMT96B B1 ON B1.DATE_TIME = SUBSTR(TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1, 'YYYYMMDD'), 1, 6)"
						" AND B1.ITEM_ENAME = 'FOSMT03-107'"
						" LEFT JOIN TFOSMT96B B2 ON B2.DATE_TIME = SUBSTR(TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1, 'YYYYMMDD'), 1, 6)"
						" AND B2.ITEM_ENAME = 'FOSMT03-108'"
						" WHERE 1 = 1"
						" AND A.DATE_TIME < @DATE_TIME"
						" AND A.DATE_TIME >= SUBSTR(TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1, 'YYYYMMDD'), 1, 6) || '01'"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				EIClass blk;
				cmd_inq.ExecuteQuery(blk.Tables[0]);
				cmd_inq.Close();

				bcls_ret->Tables[tbl].Columns.Add(DT_DECIMAL, "PROD_PROGRESS");
				bcls_ret->Tables[tbl].Columns.Add(DT_DECIMAL, "PROD_PROGRESS_1");
				bcls_ret->Tables[tbl].Columns.Add(DT_DECIMAL, "PROD_PROGRESS_2");
				bcls_ret->Tables[tbl].Columns.Add(DT_DECIMAL, "PLAN_SLAB_WT");
				bcls_ret->Tables[tbl].Columns.Add(DT_DECIMAL, "PLAN_SLAB_WT_1");
				bcls_ret->Tables[tbl].Columns.Add(DT_DECIMAL, "PLAN_SLAB_WT_2");
				CDataRow& row = bcls_ret->Tables[tbl].Rows.Add();
				row.Merge(blk.Tables[0].Rows[0]);

				//产量进度
				bcls_ret->Tables[tbl].Rows[2]["PROD_PROGRESS"] = bcls_ret->Tables[tbl].Rows[3]["PROD_PROGRESS"].ToDecimal();
				bcls_ret->Tables[tbl].Rows[2]["PROD_PROGRESS_1"] = bcls_ret->Tables[tbl].Rows[3]["PROD_PROGRESS_1"].ToDecimal();
				bcls_ret->Tables[tbl].Rows[2]["PROD_PROGRESS_2"] = bcls_ret->Tables[tbl].Rows[3]["PROD_PROGRESS_2"].ToDecimal();

				if (bcls_ret->Tables[tbl].Rows[3]["PLAN_SLAB_WT"].ToDecimal() > 0)
				{
					bcls_ret->Tables[tbl].Rows[1]["PROD_PROGRESS"] = (bcls_ret->Tables[tbl].Rows[3]["TOTAL_SLAB_WT"].ToDecimal() - bcls_ret->Tables[tbl].Rows[2]["TOTAL_SLAB_WT"].ToDecimal()) / bcls_ret->Tables[tbl].Rows[3]["PLAN_SLAB_WT"].ToDecimal() * 100;
					bcls_ret->Tables[tbl].Rows[0]["PROD_PROGRESS"] = (bcls_ret->Tables[tbl].Rows[3]["TOTAL_SLAB_WT"].ToDecimal() - bcls_ret->Tables[tbl].Rows[2]["TOTAL_SLAB_WT"].ToDecimal() - bcls_ret->Tables[tbl].Rows[1]["TOTAL_SLAB_WT"].ToDecimal()) / bcls_ret->Tables[tbl].Rows[3]["PLAN_SLAB_WT"].ToDecimal() * 100;
				}
				if (bcls_ret->Tables[tbl].Rows[3]["PLAN_SLAB_WT_1"].ToDecimal() > 0)
				{
					bcls_ret->Tables[tbl].Rows[1]["PROD_PROGRESS_1"] = (bcls_ret->Tables[tbl].Rows[3]["SLAB_WT_1"].ToDecimal() - bcls_ret->Tables[tbl].Rows[2]["SLAB_WT_1"].ToDecimal()) / bcls_ret->Tables[tbl].Rows[3]["PLAN_SLAB_WT_1"].ToDecimal() * 100;
					bcls_ret->Tables[tbl].Rows[0]["PROD_PROGRESS_1"] = (bcls_ret->Tables[tbl].Rows[3]["SLAB_WT_1"].ToDecimal() - bcls_ret->Tables[tbl].Rows[2]["SLAB_WT_1"].ToDecimal() - bcls_ret->Tables[tbl].Rows[1]["SLAB_WT_1"].ToDecimal()) / bcls_ret->Tables[tbl].Rows[3]["PLAN_SLAB_WT_1"].ToDecimal() * 100;

				}
				if (bcls_ret->Tables[tbl].Rows[3]["PLAN_SLAB_WT_2"].ToDecimal() > 0)
				{
					bcls_ret->Tables[tbl].Rows[1]["PROD_PROGRESS_2"] = (bcls_ret->Tables[tbl].Rows[3]["SLAB_WT_2"].ToDecimal() - bcls_ret->Tables[tbl].Rows[2]["SLAB_WT_2"].ToDecimal()) / bcls_ret->Tables[tbl].Rows[3]["PLAN_SLAB_WT_2"].ToDecimal() * 100;
					bcls_ret->Tables[tbl].Rows[0]["PROD_PROGRESS_2"] = (bcls_ret->Tables[tbl].Rows[3]["SLAB_WT_2"].ToDecimal() - bcls_ret->Tables[tbl].Rows[2]["SLAB_WT_2"].ToDecimal() - bcls_ret->Tables[tbl].Rows[1]["SLAB_WT_2"].ToDecimal()) / bcls_ret->Tables[tbl].Rows[3]["PLAN_SLAB_WT_2"].ToDecimal() * 100;
				}
			}
			else if (tbl == "TFOSMT03B")
			{
				//二炼钢生产计划与实绩炉数
				bcls_ret->Tables.Add(tbl);
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-05：生成查询日期之前7天的日期序列;日期边界、参数与结果列保持不变。
// 改写原因：SYSIBM 辅助表改为 DUAL；DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " WITH A(LEVEL, TIME) AS"
						// " (SELECT 1, TO_DATE(@DATE_TIME,'YYYYMMDD') - 7 DAY"
						// " FROM SYSIBM.SYSDUMMY1"
						// " WHERE 1 = 1"
						// " UNION ALL"
						// " SELECT LEVEL + 1,TIME + 1 DAY"
						// " FROM SYSIBM.SYSDUMMY1, A"
						// " WHERE A.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY)"
						// " SELECT TO_CHAR(A.TIME, 'YYYYMMDD') DATE_TIME,"
						// " COALESCE(B.PLAN_CHARGE_2, 0) PLAN_CHARGE, COALESCE(B.PRODUCT_CHARGE_2, 0) PRODUCT_CHARGE"
						// " FROM A"
						// " LEFT JOIN TFOSMT03A B ON TO_CHAR(A.TIME,'YYYYMMDD') = B.DATE_TIME"
						// " WHERE 1 = 1"
						// ;
// DM8 SQL：
					sqlstr =
						" WITH A(LEVEL, TIME) AS"
						" (SELECT 1, TO_DATE(@DATE_TIME,'YYYYMMDD') - 7"
						" FROM DUAL"
						" WHERE 1 = 1"
						" UNION ALL"
						" SELECT LEVEL + 1,TIME + 1"
						" FROM DUAL, A"
						" WHERE A.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD') - 1)"
						" SELECT TO_CHAR(A.TIME, 'YYYYMMDD') DATE_TIME,"
						" COALESCE(B.PLAN_CHARGE_2, 0) PLAN_CHARGE, COALESCE(B.PRODUCT_CHARGE_2, 0) PRODUCT_CHARGE"
						" FROM A"
						" LEFT JOIN TFOSMT03A B ON TO_CHAR(A.TIME,'YYYYMMDD') = B.DATE_TIME"
						" WHERE 1 = 1"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();
			}
			else if (tbl == "TFOSMT03C")
			{
				//板坯库存趋势图
				bcls_ret->Tables.Add(tbl);
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-06：生成查询日期之前7天的日期序列;日期边界、参数与结果列保持不变。
// 改写原因：SYSIBM 辅助表改为 DUAL；DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " WITH A(LEVEL, TIME) AS"
						// " (SELECT 1, TO_DATE(@DATE_TIME,'YYYYMMDD') - 7 DAY"
						// " FROM SYSIBM.SYSDUMMY1"
						// " WHERE 1 = 1"
						// " UNION ALL"
						// " SELECT LEVEL + 1,TIME + 1 DAY"
						// " FROM SYSIBM.SYSDUMMY1, A"
						// " WHERE A.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY)"
						// " SELECT TO_CHAR(A.TIME,'YYYYMMDD') DATE_TIME,"
						// " ROUND(COALESCE(B.STOCK_TOTAL_WT_1, 0),0) STOCK_TOTAL_WT_1, ROUND(COALESCE(B.STOCK_TOTAL_WT_2, 0),0) STOCK_TOTAL_WT_2"
						// " FROM A"
						// " LEFT JOIN TFOSMT03A B ON TO_CHAR(A.TIME,'YYYYMMDD') = B.DATE_TIME"
						// " WHERE 1 = 1"
						// ;
// DM8 SQL：
					sqlstr =
						" WITH A(LEVEL, TIME) AS"
						" (SELECT 1, TO_DATE(@DATE_TIME,'YYYYMMDD') - 7"
						" FROM DUAL"
						" WHERE 1 = 1"
						" UNION ALL"
						" SELECT LEVEL + 1,TIME + 1"
						" FROM DUAL, A"
						" WHERE A.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD') - 1)"
						" SELECT TO_CHAR(A.TIME,'YYYYMMDD') DATE_TIME,"
						" ROUND(COALESCE(B.STOCK_TOTAL_WT_1, 0),0) STOCK_TOTAL_WT_1, ROUND(COALESCE(B.STOCK_TOTAL_WT_2, 0),0) STOCK_TOTAL_WT_2"
						" FROM A"
						" LEFT JOIN TFOSMT03A B ON TO_CHAR(A.TIME,'YYYYMMDD') = B.DATE_TIME"
						" WHERE 1 = 1"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();
			}
			else if (tbl == "TFOSMT03D")
			{
				//铸余比例图
				bcls_ret->Tables.Add(tbl);
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-07：生成查询日期之前7天的日期序列;日期边界、参数与结果列保持不变。
// 改写原因：SYSIBM 辅助表改为 DUAL；DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " WITH A(LEVEL, TIME) AS"
						// " (SELECT 1, TO_DATE(@DATE_TIME,'YYYYMMDD') - 7 DAY"
						// " FROM SYSIBM.SYSDUMMY1"
						// " WHERE 1 = 1"
						// " UNION ALL"
						// " SELECT LEVEL + 1,TIME + 1 DAY"
						// " FROM SYSIBM.SYSDUMMY1, A"
						// " WHERE A.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY)"
						// " SELECT TO_CHAR(A.TIME,'YYYYMMDD') DATE_TIME,"
						// " CAST(ROUND(COALESCE(B.CC_REMAIN_RATE_1, 0),1) AS DECIMAL(4,1)) CC_REMAIN_RATE_1,"
						// " CAST(ROUND(COALESCE(B.CC_REMAIN_RATE_2, 0),1) AS DECIMAL(4,1)) CC_REMAIN_RATE_2"
						// " FROM A"
						// " LEFT JOIN TFOSMT03A B ON TO_CHAR(A.TIME,'YYYYMMDD') = B.DATE_TIME"
						// " WHERE 1 = 1"
						// ;
// DM8 SQL：
					sqlstr =
						" WITH A(LEVEL, TIME) AS"
						" (SELECT 1, TO_DATE(@DATE_TIME,'YYYYMMDD') - 7"
						" FROM DUAL"
						" WHERE 1 = 1"
						" UNION ALL"
						" SELECT LEVEL + 1,TIME + 1"
						" FROM DUAL, A"
						" WHERE A.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD') - 1)"
						" SELECT TO_CHAR(A.TIME,'YYYYMMDD') DATE_TIME,"
						" CAST(ROUND(COALESCE(B.CC_REMAIN_RATE_1, 0),1) AS DECIMAL(4,1)) CC_REMAIN_RATE_1,"
						" CAST(ROUND(COALESCE(B.CC_REMAIN_RATE_2, 0),1) AS DECIMAL(4,1)) CC_REMAIN_RATE_2"
						" FROM A"
						" LEFT JOIN TFOSMT03A B ON TO_CHAR(A.TIME,'YYYYMMDD') = B.DATE_TIME"
						" WHERE 1 = 1"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();
			}
			else if (tbl == "TFOSMT03E1")
			{
				//一炼钢生产情况
				bcls_ret->Tables.Add(tbl);
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-08：生成查询日期之前3天的日期序列;日期边界、参数与结果列保持不变。
// 改写原因：SYSIBM 辅助表改为 DUAL；DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " WITH A(LEVEL, TIME) AS"
						// " (SELECT 1, TO_DATE(@DATE_TIME,'YYYYMMDD') - 3 DAY"
						// " FROM SYSIBM.SYSDUMMY1"
						// " WHERE 1 = 1"
						// " UNION ALL"
						// " SELECT LEVEL + 1,TIME + 1 DAY"
						// " FROM SYSIBM.SYSDUMMY1, A"
						// " WHERE A.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY)"
						// " SELECT TO_CHAR(A.TIME,'YYYYMMDD') DATE_TIME,"
						// " B.SMELT_CHARGE_1 SMELT_CHARGE, B.DIR_CHARGE_RATE_1 DIR_CHARGE_RATE, B.SHALLOW_CHARGE_RATE_1 SHALLOW_CHARGE_RATE,"
						// " B.TOL_BOF_STEEL_WT_1 TOL_BOF_STEEL_WT, B.AVG_STEEL_WT_1 AVG_STEEL_WT, B.AVG_SLAB_WT_1 AVG_SLAB_WT,"
						// " B.AVG_BOF_IRON_WT_1 AVG_BOF_IRON_WT, B.MOLTIRON_WT1 MOLTIRON_WT,"
						// " B.AVG_TPD_IRON_WT_1 AVG_TPD_IRON_WT, CAST(ROUND(1.0000 * B.IRON_SLAB_RATE_1 / 100, 4) AS DECIMAL(5,4)) IRON_SLAB_RATE"
						// " FROM A"
						// " LEFT JOIN TFOSMT03A B ON TO_CHAR(A.TIME,'YYYYMMDD') = B.DATE_TIME"
						// " WHERE 1 = 1"
						// ;
// DM8 SQL：
					sqlstr =
						" WITH A(LEVEL, TIME) AS"
						" (SELECT 1, TO_DATE(@DATE_TIME,'YYYYMMDD') - 3"
						" FROM DUAL"
						" WHERE 1 = 1"
						" UNION ALL"
						" SELECT LEVEL + 1,TIME + 1"
						" FROM DUAL, A"
						" WHERE A.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD') - 1)"
						" SELECT TO_CHAR(A.TIME,'YYYYMMDD') DATE_TIME,"
						" B.SMELT_CHARGE_1 SMELT_CHARGE, B.DIR_CHARGE_RATE_1 DIR_CHARGE_RATE, B.SHALLOW_CHARGE_RATE_1 SHALLOW_CHARGE_RATE,"
						" B.TOL_BOF_STEEL_WT_1 TOL_BOF_STEEL_WT, B.AVG_STEEL_WT_1 AVG_STEEL_WT, B.AVG_SLAB_WT_1 AVG_SLAB_WT,"
						" B.AVG_BOF_IRON_WT_1 AVG_BOF_IRON_WT, B.MOLTIRON_WT1 MOLTIRON_WT,"
						" B.AVG_TPD_IRON_WT_1 AVG_TPD_IRON_WT, CAST(ROUND(1.0000 * B.IRON_SLAB_RATE_1 / 100, 4) AS DECIMAL(5,4)) IRON_SLAB_RATE"
						" FROM A"
						" LEFT JOIN TFOSMT03A B ON TO_CHAR(A.TIME,'YYYYMMDD') = B.DATE_TIME"
						" WHERE 1 = 1"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();

				//合计行
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-09：取查询日期前推1日的日期字符串;日历日边界、参数与结果列保持不变。
// 改写原因：DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " SELECT"
						// " SUM(SMELT_CHARGE_1) SMELT_CHARGE,"
						// " SUM(DIR_CHARGE_RATE_1 * SMELT_CHARGE_1) / SUM(SMELT_CHARGE_1) DIR_CHARGE_RATE,"
						// " SUM(SHALLOW_CHARGE_RATE_1 * SMELT_CHARGE_1) / SUM(SMELT_CHARGE_1) SHALLOW_CHARGE_RATE,"
						// " SUM(TOL_BOF_STEEL_WT_1) TOL_BOF_STEEL_WT,"
						// " SUM(TOL_BOF_STEEL_WT_1) / SUM(SMELT_CHARGE_1) AVG_STEEL_WT,"
						// " SUM(SLAB_WT_1) / SUM(PRODUCT_CHARGE_1) AVG_SLAB_WT,"
						// " SUM(MOLTIRON_WT1) / SUM(SMELT_CHARGE_1) AVG_BOF_IRON_WT,"
						// " SUM(MOLTIRON_WT1) MOLTIRON_WT,"
						// " SUM(AVG_TPD_IRON_WT_1 * TPD_WT_1) / SUM(TPD_WT_1) AVG_TPD_IRON_WT,"
						// " 1.0000 * (1.0000 * SUM(AVG_TPD_IRON_WT_1 * TPD_WT_1) / SUM(TPD_WT_1))"
						// " / (SUM(SLAB_WT_1) / SUM(PRODUCT_CHARGE_1))"
						// " IRON_SLAB_RATE,"
						// " '合计' DATE_TIME"
						// " FROM TFOSMT03A A"
						// " WHERE 1 = 1"
						// " AND A.DATE_TIME < @DATE_TIME"
						// " AND A.DATE_TIME >= SUBSTR(TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY, 'YYYYMMDD'), 1, 6) || '01'"
						// ;
// DM8 SQL：
					sqlstr =
						" SELECT"
						" SUM(SMELT_CHARGE_1) SMELT_CHARGE,"
						" SUM(DIR_CHARGE_RATE_1 * SMELT_CHARGE_1) / SUM(SMELT_CHARGE_1) DIR_CHARGE_RATE,"
						" SUM(SHALLOW_CHARGE_RATE_1 * SMELT_CHARGE_1) / SUM(SMELT_CHARGE_1) SHALLOW_CHARGE_RATE,"
						" SUM(TOL_BOF_STEEL_WT_1) TOL_BOF_STEEL_WT,"
						" SUM(TOL_BOF_STEEL_WT_1) / SUM(SMELT_CHARGE_1) AVG_STEEL_WT,"
						" SUM(SLAB_WT_1) / SUM(PRODUCT_CHARGE_1) AVG_SLAB_WT,"
						" SUM(MOLTIRON_WT1) / SUM(SMELT_CHARGE_1) AVG_BOF_IRON_WT,"
						" SUM(MOLTIRON_WT1) MOLTIRON_WT,"
						" SUM(AVG_TPD_IRON_WT_1 * TPD_WT_1) / SUM(TPD_WT_1) AVG_TPD_IRON_WT,"
						" 1.0000 * (1.0000 * SUM(AVG_TPD_IRON_WT_1 * TPD_WT_1) / SUM(TPD_WT_1))"
						" / (SUM(SLAB_WT_1) / SUM(PRODUCT_CHARGE_1))"
						" IRON_SLAB_RATE,"
						" '合计' DATE_TIME"
						" FROM TFOSMT03A A"
						" WHERE 1 = 1"
						" AND A.DATE_TIME < @DATE_TIME"
						" AND A.DATE_TIME >= SUBSTR(TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1, 'YYYYMMDD'), 1, 6) || '01'"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				EIClass blk;
				cmd_inq.ExecuteQuery(blk.Tables[0]);
				cmd_inq.Close();

				CDataRow& row = bcls_ret->Tables[tbl].Rows.Add();
				row.Merge(blk.Tables[0].Rows[0]);
			}
			else if (tbl == "TFOSMT03E2")
			{
				//二炼钢生产情况
				bcls_ret->Tables.Add(tbl);
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-10：生成查询日期之前3天的日期序列;日期边界、参数与结果列保持不变。
// 改写原因：SYSIBM 辅助表改为 DUAL；DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " WITH A(LEVEL, TIME) AS"
						// " (SELECT 1, TO_DATE(@DATE_TIME,'YYYYMMDD') - 3 DAY"
						// " FROM SYSIBM.SYSDUMMY1"
						// " WHERE 1 = 1"
						// " UNION ALL"
						// " SELECT LEVEL + 1,TIME + 1 DAY"
						// " FROM SYSIBM.SYSDUMMY1, A"
						// " WHERE A.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY)"
						// " SELECT TO_CHAR(A.TIME,'YYYYMMDD') DATE_TIME,"
						// " B.SMELT_CHARGE_2 SMELT_CHARGE, B.DIR_CHARGE_RATE_2 DIR_CHARGE_RATE, B.SHALLOW_CHARGE_RATE_2 SHALLOW_CHARGE_RATE,"
						// " B.TOL_BOF_STEEL_WT_2 TOL_BOF_STEEL_WT, B.AVG_STEEL_WT_2 AVG_STEEL_WT, B.AVG_SLAB_WT_2 AVG_SLAB_WT,"
						// " B.AVG_BOF_IRON_WT_2 AVG_BOF_IRON_WT, B.MOLTIRON_WT2 MOLTIRON_WT,"
						// " B.AVG_TPD_IRON_WT_2 AVG_TPD_IRON_WT, CAST(ROUND(1.0000 * B.IRON_SLAB_RATE_2 / 100, 4) AS DECIMAL(5,4)) IRON_SLAB_RATE"
						// " FROM A"
						// " LEFT JOIN TFOSMT03A B ON TO_CHAR(A.TIME,'YYYYMMDD') = B.DATE_TIME"
						// " WHERE 1 = 1"
						// ;
// DM8 SQL：
					sqlstr =
						" WITH A(LEVEL, TIME) AS"
						" (SELECT 1, TO_DATE(@DATE_TIME,'YYYYMMDD') - 3"
						" FROM DUAL"
						" WHERE 1 = 1"
						" UNION ALL"
						" SELECT LEVEL + 1,TIME + 1"
						" FROM DUAL, A"
						" WHERE A.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD') - 1)"
						" SELECT TO_CHAR(A.TIME,'YYYYMMDD') DATE_TIME,"
						" B.SMELT_CHARGE_2 SMELT_CHARGE, B.DIR_CHARGE_RATE_2 DIR_CHARGE_RATE, B.SHALLOW_CHARGE_RATE_2 SHALLOW_CHARGE_RATE,"
						" B.TOL_BOF_STEEL_WT_2 TOL_BOF_STEEL_WT, B.AVG_STEEL_WT_2 AVG_STEEL_WT, B.AVG_SLAB_WT_2 AVG_SLAB_WT,"
						" B.AVG_BOF_IRON_WT_2 AVG_BOF_IRON_WT, B.MOLTIRON_WT2 MOLTIRON_WT,"
						" B.AVG_TPD_IRON_WT_2 AVG_TPD_IRON_WT, CAST(ROUND(1.0000 * B.IRON_SLAB_RATE_2 / 100, 4) AS DECIMAL(5,4)) IRON_SLAB_RATE"
						" FROM A"
						" LEFT JOIN TFOSMT03A B ON TO_CHAR(A.TIME,'YYYYMMDD') = B.DATE_TIME"
						" WHERE 1 = 1"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();

				//合计行
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-11：取查询日期前推1日的日期字符串;日历日边界、参数与结果列保持不变。
// 改写原因：DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " SELECT"
						// " SUM(SMELT_CHARGE_2) SMELT_CHARGE,"
						// " SUM(DIR_CHARGE_RATE_2 * SMELT_CHARGE_2) / SUM(SMELT_CHARGE_2) DIR_CHARGE_RATE,"
						// " SUM(SHALLOW_CHARGE_RATE_2 * SMELT_CHARGE_2) / SUM(SMELT_CHARGE_2) SHALLOW_CHARGE_RATE,"
						// " SUM(TOL_BOF_STEEL_WT_2) TOL_BOF_STEEL_WT,"
						// " SUM(TOL_BOF_STEEL_WT_2) / SUM(SMELT_CHARGE_2) AVG_STEEL_WT,"
						// " SUM(SLAB_WT_2) / SUM(PRODUCT_CHARGE_2) AVG_SLAB_WT,"
						// " SUM(MOLTIRON_WT2) / SUM(SMELT_CHARGE_2) AVG_BOF_IRON_WT,"
						// " SUM(MOLTIRON_WT2) MOLTIRON_WT,"
						// " SUM(AVG_TPD_IRON_WT_2 * TPD_WT_2) / SUM(TPD_WT_2) AVG_TPD_IRON_WT,"
						// " 1.0000 * (1.0000 * SUM(AVG_TPD_IRON_WT_2 * TPD_WT_2) / SUM(TPD_WT_2))"
						// " / (SUM(SLAB_WT_2) / SUM(PRODUCT_CHARGE_2))"
						// " IRON_SLAB_RATE,"
						// " '合计' DATE_TIME"
						// " FROM TFOSMT03A A"
						// " WHERE 1 = 1"
						// " AND A.DATE_TIME < @DATE_TIME"
						// " AND A.DATE_TIME >= SUBSTR(TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY, 'YYYYMMDD'), 1, 6) || '01'"
						// ;
// DM8 SQL：
					sqlstr =
						" SELECT"
						" SUM(SMELT_CHARGE_2) SMELT_CHARGE,"
						" SUM(DIR_CHARGE_RATE_2 * SMELT_CHARGE_2) / SUM(SMELT_CHARGE_2) DIR_CHARGE_RATE,"
						" SUM(SHALLOW_CHARGE_RATE_2 * SMELT_CHARGE_2) / SUM(SMELT_CHARGE_2) SHALLOW_CHARGE_RATE,"
						" SUM(TOL_BOF_STEEL_WT_2) TOL_BOF_STEEL_WT,"
						" SUM(TOL_BOF_STEEL_WT_2) / SUM(SMELT_CHARGE_2) AVG_STEEL_WT,"
						" SUM(SLAB_WT_2) / SUM(PRODUCT_CHARGE_2) AVG_SLAB_WT,"
						" SUM(MOLTIRON_WT2) / SUM(SMELT_CHARGE_2) AVG_BOF_IRON_WT,"
						" SUM(MOLTIRON_WT2) MOLTIRON_WT,"
						" SUM(AVG_TPD_IRON_WT_2 * TPD_WT_2) / SUM(TPD_WT_2) AVG_TPD_IRON_WT,"
						" 1.0000 * (1.0000 * SUM(AVG_TPD_IRON_WT_2 * TPD_WT_2) / SUM(TPD_WT_2))"
						" / (SUM(SLAB_WT_2) / SUM(PRODUCT_CHARGE_2))"
						" IRON_SLAB_RATE,"
						" '合计' DATE_TIME"
						" FROM TFOSMT03A A"
						" WHERE 1 = 1"
						" AND A.DATE_TIME < @DATE_TIME"
						" AND A.DATE_TIME >= SUBSTR(TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1, 'YYYYMMDD'), 1, 6) || '01'"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				EIClass blk;
				cmd_inq.ExecuteQuery(blk.Tables[0]);
				cmd_inq.Close();

				CDataRow& row = bcls_ret->Tables[tbl].Rows.Add();
				row.Merge(blk.Tables[0].Rows[0]);
			}
			else if (tbl == "TFOSMT03F1")
			{
				//一炼钢铁坯比
				bcls_ret->Tables.Add(tbl);
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-12：生成查询日期之前7天的日期序列;日期边界、参数与结果列保持不变。
// 改写原因：SYSIBM 辅助表改为 DUAL；DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " WITH A(LEVEL, TIME) AS"
						// " (SELECT 1, TO_DATE(@DATE_TIME,'YYYYMMDD') - 7 DAY"
						// " FROM SYSIBM.SYSDUMMY1"
						// " WHERE 1 = 1"
						// " UNION ALL"
						// " SELECT LEVEL + 1,TIME + 1 DAY"
						// " FROM SYSIBM.SYSDUMMY1, A"
						// " WHERE A.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY)"
						// " SELECT TO_CHAR(A.TIME,'YYYYMMDD') DATE_TIME, B.IRON_SLAB_RATE_1 / 100 IRON_SLAB_RATE"
						// " FROM A"
						// " LEFT JOIN TFOSMT03A B ON TO_CHAR(A.TIME,'YYYYMMDD') = B.DATE_TIME"
						// " WHERE 1 = 1"
						// ;
// DM8 SQL：
					sqlstr =
						" WITH A(LEVEL, TIME) AS"
						" (SELECT 1, TO_DATE(@DATE_TIME,'YYYYMMDD') - 7"
						" FROM DUAL"
						" WHERE 1 = 1"
						" UNION ALL"
						" SELECT LEVEL + 1,TIME + 1"
						" FROM DUAL, A"
						" WHERE A.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD') - 1)"
						" SELECT TO_CHAR(A.TIME,'YYYYMMDD') DATE_TIME, B.IRON_SLAB_RATE_1 / 100 IRON_SLAB_RATE"
						" FROM A"
						" LEFT JOIN TFOSMT03A B ON TO_CHAR(A.TIME,'YYYYMMDD') = B.DATE_TIME"
						" WHERE 1 = 1"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();
			}
			else if (tbl == "TFOSMT03F2")
			{
				//二炼钢铁坯比
				bcls_ret->Tables.Add(tbl);
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-13：生成查询日期之前7天的日期序列;日期边界、参数与结果列保持不变。
// 改写原因：SYSIBM 辅助表改为 DUAL；DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " WITH A(LEVEL, TIME) AS"
						// " (SELECT 1, TO_DATE(@DATE_TIME,'YYYYMMDD') - 7 DAY"
						// " FROM SYSIBM.SYSDUMMY1"
						// " WHERE 1 = 1"
						// " UNION ALL"
						// " SELECT LEVEL + 1,TIME + 1 DAY"
						// " FROM SYSIBM.SYSDUMMY1, A"
						// " WHERE A.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY)"
						// " SELECT TO_CHAR(A.TIME,'YYYYMMDD') DATE_TIME, B.IRON_SLAB_RATE_2 / 100 IRON_SLAB_RATE"
						// " FROM A"
						// " LEFT JOIN TFOSMT03A B ON TO_CHAR(A.TIME,'YYYYMMDD') = B.DATE_TIME"
						// " WHERE 1 = 1"
						// ;
// DM8 SQL：
					sqlstr =
						" WITH A(LEVEL, TIME) AS"
						" (SELECT 1, TO_DATE(@DATE_TIME,'YYYYMMDD') - 7"
						" FROM DUAL"
						" WHERE 1 = 1"
						" UNION ALL"
						" SELECT LEVEL + 1,TIME + 1"
						" FROM DUAL, A"
						" WHERE A.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD') - 1)"
						" SELECT TO_CHAR(A.TIME,'YYYYMMDD') DATE_TIME, B.IRON_SLAB_RATE_2 / 100 IRON_SLAB_RATE"
						" FROM A"
						" LEFT JOIN TFOSMT03A B ON TO_CHAR(A.TIME,'YYYYMMDD') = B.DATE_TIME"
						" WHERE 1 = 1"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();
			}
			else if (tbl == "TFOSMT03G")
			{
				//放铁情况
				bcls_ret->Tables.Add(tbl);
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-14：生成查询日期之前10天的日期序列;日期边界、参数与结果列保持不变。
// 改写原因：SYSIBM 辅助表改为 DUAL；DB2 日期天数后缀改为整数天运算；空值搜索 DECODE 改为标准 CASE,不依赖 NULL 相等匹配的未记载语义；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " WITH A(LEVEL, TIME) AS"
						// " (SELECT 1, TO_DATE(@DATE_TIME,'YYYYMMDD') - 10 DAY"
						// " FROM SYSIBM.SYSDUMMY1"
						// " WHERE 1 = 1"
						// " UNION ALL"
						// " SELECT LEVEL + 1,TIME + 1 DAY"
						// " FROM SYSIBM.SYSDUMMY1, A"
						// " WHERE A.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY)"
						// " SELECT TO_CHAR(A.TIME,'YYYYMMDD') DATE_TIME,"
						// " COALESCE(B1.IRON_RELEASE_WT,0) IRON_RELEASE_WT_1, COALESCE(B1.SEQ_NO,0) DEDUCT_PT_1,"
						// " COALESCE(B2.IRON_RELEASE_WT,0) IRON_RELEASE_WT_2, COALESCE(B2.SEQ_NO,0) DEDUCT_PT_2,"
						// " COALESCE(B3.IRON_RELEASE_WT,0) IRON_RELEASE_WT_3, COALESCE(B3.SEQ_NO,0) DEDUCT_PT_3,"
						// " COALESCE(B4.IRON_RELEASE_WT,0) IRON_RELEASE_WT_4, COALESCE(B4.SEQ_NO,0) DEDUCT_PT_4,"
						// " COALESCE(B5.IRON_RELEASE_WT,0) IRON_RELEASE_WT_5, COALESCE(B5.SEQ_NO,0) DEDUCT_PT_5,"
						// " COALESCE(B6.IRON_RELEASE_WT,0) IRON_RELEASE_WT_6, COALESCE(B6.SEQ_NO,0) DEDUCT_PT_6,"
						// " COALESCE(B7.IRON_RELEASE_WT,0) IRON_RELEASE_WT_7, COALESCE(B7.SEQ_NO,0) DEDUCT_PT_7,"
						// " DECODE(B1.REMARK, NULL, '', TRIM(B1.REMARK)) ||"
						// " DECODE(B2.REMARK, NULL, '', TRIM(B2.REMARK)) ||"
						// " DECODE(B3.REMARK, NULL, '', TRIM(B3.REMARK)) ||"
						// " DECODE(B4.REMARK, NULL, '', TRIM(B4.REMARK)) ||"
						// " DECODE(B5.REMARK, NULL, '', TRIM(B5.REMARK)) ||"
						// " DECODE(B6.REMARK, NULL, '', TRIM(B6.REMARK)) ||"
						// " DECODE(B7.REMARK, NULL, '', TRIM(B7.REMARK))"
						// " REMARK"
						// " FROM A"
						// " LEFT JOIN TFOSMT03B B1 ON TO_CHAR(A.TIME,'YYYYMMDD') = B1.DATE_TIME AND B1.DEP_NAME = '02'"
						// " LEFT JOIN TFOSMT03B B2 ON TO_CHAR(A.TIME,'YYYYMMDD') = B2.DATE_TIME AND B2.DEP_NAME = '03'"
						// " LEFT JOIN TFOSMT03B B3 ON TO_CHAR(A.TIME,'YYYYMMDD') = B3.DATE_TIME AND B3.DEP_NAME = '05'"
						// " LEFT JOIN TFOSMT03B B4 ON TO_CHAR(A.TIME,'YYYYMMDD') = B4.DATE_TIME AND B4.DEP_NAME = '04'"
						// " LEFT JOIN TFOSMT03B B5 ON TO_CHAR(A.TIME,'YYYYMMDD') = B5.DATE_TIME AND B5.DEP_NAME = '01'"
						// " LEFT JOIN TFOSMT03B B6 ON TO_CHAR(A.TIME,'YYYYMMDD') = B6.DATE_TIME AND B6.DEP_NAME = '07'"
						// " LEFT JOIN TFOSMT03B B7 ON TO_CHAR(A.TIME,'YYYYMMDD') = B7.DATE_TIME AND B7.DEP_NAME = '10'"
						// " WHERE 1 = 1"
						// ;
// DM8 SQL：
					sqlstr =
						" WITH A(LEVEL, TIME) AS"
						" (SELECT 1, TO_DATE(@DATE_TIME,'YYYYMMDD') - 10"
						" FROM DUAL"
						" WHERE 1 = 1"
						" UNION ALL"
						" SELECT LEVEL + 1,TIME + 1"
						" FROM DUAL, A"
						" WHERE A.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD') - 1)"
						" SELECT TO_CHAR(A.TIME,'YYYYMMDD') DATE_TIME,"
						" COALESCE(B1.IRON_RELEASE_WT,0) IRON_RELEASE_WT_1, COALESCE(B1.SEQ_NO,0) DEDUCT_PT_1,"
						" COALESCE(B2.IRON_RELEASE_WT,0) IRON_RELEASE_WT_2, COALESCE(B2.SEQ_NO,0) DEDUCT_PT_2,"
						" COALESCE(B3.IRON_RELEASE_WT,0) IRON_RELEASE_WT_3, COALESCE(B3.SEQ_NO,0) DEDUCT_PT_3,"
						" COALESCE(B4.IRON_RELEASE_WT,0) IRON_RELEASE_WT_4, COALESCE(B4.SEQ_NO,0) DEDUCT_PT_4,"
						" COALESCE(B5.IRON_RELEASE_WT,0) IRON_RELEASE_WT_5, COALESCE(B5.SEQ_NO,0) DEDUCT_PT_5,"
						" COALESCE(B6.IRON_RELEASE_WT,0) IRON_RELEASE_WT_6, COALESCE(B6.SEQ_NO,0) DEDUCT_PT_6,"
						" COALESCE(B7.IRON_RELEASE_WT,0) IRON_RELEASE_WT_7, COALESCE(B7.SEQ_NO,0) DEDUCT_PT_7,"
						" CASE WHEN B1.REMARK IS NULL THEN '' ELSE TRIM(B1.REMARK) END ||"
						" CASE WHEN B2.REMARK IS NULL THEN '' ELSE TRIM(B2.REMARK) END ||"
						" CASE WHEN B3.REMARK IS NULL THEN '' ELSE TRIM(B3.REMARK) END ||"
						" CASE WHEN B4.REMARK IS NULL THEN '' ELSE TRIM(B4.REMARK) END ||"
						" CASE WHEN B5.REMARK IS NULL THEN '' ELSE TRIM(B5.REMARK) END ||"
						" CASE WHEN B6.REMARK IS NULL THEN '' ELSE TRIM(B6.REMARK) END ||"
						" CASE WHEN B7.REMARK IS NULL THEN '' ELSE TRIM(B7.REMARK) END"
						" REMARK"
						" FROM A"
						" LEFT JOIN TFOSMT03B B1 ON TO_CHAR(A.TIME,'YYYYMMDD') = B1.DATE_TIME AND B1.DEP_NAME = '02'"
						" LEFT JOIN TFOSMT03B B2 ON TO_CHAR(A.TIME,'YYYYMMDD') = B2.DATE_TIME AND B2.DEP_NAME = '03'"
						" LEFT JOIN TFOSMT03B B3 ON TO_CHAR(A.TIME,'YYYYMMDD') = B3.DATE_TIME AND B3.DEP_NAME = '05'"
						" LEFT JOIN TFOSMT03B B4 ON TO_CHAR(A.TIME,'YYYYMMDD') = B4.DATE_TIME AND B4.DEP_NAME = '04'"
						" LEFT JOIN TFOSMT03B B5 ON TO_CHAR(A.TIME,'YYYYMMDD') = B5.DATE_TIME AND B5.DEP_NAME = '01'"
						" LEFT JOIN TFOSMT03B B6 ON TO_CHAR(A.TIME,'YYYYMMDD') = B6.DATE_TIME AND B6.DEP_NAME = '07'"
						" LEFT JOIN TFOSMT03B B7 ON TO_CHAR(A.TIME,'YYYYMMDD') = B7.DATE_TIME AND B7.DEP_NAME = '10'"
						" WHERE 1 = 1"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.ExecuteReader();
				CDecimal iron_release_wt_1;
				CDecimal iron_release_wt_2;
				CDecimal iron_release_wt_3;
				CDecimal iron_release_wt_4;
				CDecimal iron_release_wt_5;
				CDecimal iron_release_wt_6;
				CDecimal iron_release_wt_7;
				CDecimal deduct_pt_1;
				CDecimal deduct_pt_2;
				CDecimal deduct_pt_3;
				CDecimal deduct_pt_4;
				CDecimal deduct_pt_5;
				CDecimal deduct_pt_6;
				CDecimal deduct_pt_7;
				while (cmd_inq.Read())
				{
					iron_release_wt_1 = iron_release_wt_1 + cmd_inq.GetDecimal(2);
					iron_release_wt_2 = iron_release_wt_2 + cmd_inq.GetDecimal(4);
					iron_release_wt_3 = iron_release_wt_3 + cmd_inq.GetDecimal(6);
					iron_release_wt_4 = iron_release_wt_4 + cmd_inq.GetDecimal(8);
					iron_release_wt_5 = iron_release_wt_5 + cmd_inq.GetDecimal(10);
					iron_release_wt_6 = iron_release_wt_6 + cmd_inq.GetDecimal(12);
					iron_release_wt_7 = iron_release_wt_7 + cmd_inq.GetDecimal(14);

					deduct_pt_1 = deduct_pt_1 + cmd_inq.GetDecimal(3);
					deduct_pt_2 = deduct_pt_2 + cmd_inq.GetDecimal(5);
					deduct_pt_3 = deduct_pt_3 + cmd_inq.GetDecimal(7);
					deduct_pt_4 = deduct_pt_4 + cmd_inq.GetDecimal(9);
					deduct_pt_5 = deduct_pt_5 + cmd_inq.GetDecimal(11);
					deduct_pt_6 = deduct_pt_6 + cmd_inq.GetDecimal(13);
					deduct_pt_7 = deduct_pt_7 + cmd_inq.GetDecimal(15);
				}
				cmd_inq.Close();

				CDataRow& row = bcls_ret->Tables[tbl].Rows.Add();
				row["DATE_TIME"] = "合计";
				row["IRON_RELEASE_WT_1"] = iron_release_wt_1;
				row["IRON_RELEASE_WT_2"] = iron_release_wt_2;
				row["IRON_RELEASE_WT_3"] = iron_release_wt_3;
				row["IRON_RELEASE_WT_4"] = iron_release_wt_4;
				row["IRON_RELEASE_WT_5"] = iron_release_wt_5;
				row["IRON_RELEASE_WT_6"] = iron_release_wt_6;
				row["IRON_RELEASE_WT_7"] = iron_release_wt_7;
				row["DEDUCT_PT_1"] = deduct_pt_1;
				row["DEDUCT_PT_2"] = deduct_pt_2;
				row["DEDUCT_PT_3"] = deduct_pt_3;
				row["DEDUCT_PT_4"] = deduct_pt_4;
				row["DEDUCT_PT_5"] = deduct_pt_5;
				row["DEDUCT_PT_6"] = deduct_pt_6;
				row["DEDUCT_PT_7"] = deduct_pt_7;
			}
			else if (tbl == "TFOSMT04A1")
			{
				//一炼钢生产调整
				bcls_ret->Tables.Add(tbl);
				CDateTime dt = CDateTime::Parse(date_time);
				if (dt.DayOfWeek() == 1)
				{
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
// DM8 适配 CHANGE-15：取查询日期前推3日的日期字符串;日历日边界、参数与结果列保持不变。
// 改写原因：DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
						// sqlstr =
							// " SELECT A.FACTORY_DIV, A.DATE_TIME, A.SHIFT_NO, A.SHIFT_GROUP,"
							// " A.PLAN_CHARGE, A.SMELT_CHARGE, A.PRODUCT_CHARGE, B.ADJUST_CHARGE,"
							// " REMARK_1 PERIOD_TIME,"
							// " B.REMARK, B.REMARK_2 STOP_TOTAL_TIME, B.DEP_NAME"
							// " FROM TFOSMT04A A"
							// " LEFT JOIN TFOSMT04B B ON A.FACTORY_DIV = B.FACTORY_DIV AND A.DATE_TIME = B.DATE_TIME AND A.SHIFT_NO = B.SHIFT_NO"
							// " WHERE A.FACTORY_DIV = 'A10'"
							// " AND ((A.DATE_TIME = @DATE_TIME AND A.SHIFT_NO = '1')"
							// " OR (A.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 3 DAY,'YYYYMMDD') AND A.SHIFT_NO <> '1')"
							// " OR A.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 2 DAY,'YYYYMMDD')"
							// " OR A.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY,'YYYYMMDD'))"
							// " ORDER BY A.DATE_TIME, A.SHIFT_NO, B.CHARGE_NO"
							// ;
// DM8 SQL：
						sqlstr =
							" SELECT A.FACTORY_DIV, A.DATE_TIME, A.SHIFT_NO, A.SHIFT_GROUP,"
							" A.PLAN_CHARGE, A.SMELT_CHARGE, A.PRODUCT_CHARGE, B.ADJUST_CHARGE,"
							" REMARK_1 PERIOD_TIME,"
							" B.REMARK, B.REMARK_2 STOP_TOTAL_TIME, B.DEP_NAME"
							" FROM TFOSMT04A A"
							" LEFT JOIN TFOSMT04B B ON A.FACTORY_DIV = B.FACTORY_DIV AND A.DATE_TIME = B.DATE_TIME AND A.SHIFT_NO = B.SHIFT_NO"
							" WHERE A.FACTORY_DIV = 'A10'"
							" AND ((A.DATE_TIME = @DATE_TIME AND A.SHIFT_NO = '1')"
							" OR (A.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 3,'YYYYMMDD') AND A.SHIFT_NO <> '1')"
							" OR A.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 2,'YYYYMMDD')"
							" OR A.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMMDD'))"
							" ORDER BY A.DATE_TIME, A.SHIFT_NO, B.CHARGE_NO"
							;
						break;
					}
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
// DM8 适配 CHANGE-16：取查询日期前推1日的日期字符串;日历日边界、参数与结果列保持不变。
// 改写原因：DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
						// sqlstr =
							// " SELECT A.FACTORY_DIV, A.DATE_TIME, A.SHIFT_NO, A.SHIFT_GROUP,"
							// " A.PLAN_CHARGE, A.SMELT_CHARGE, A.PRODUCT_CHARGE, B.ADJUST_CHARGE,"
							// " REMARK_1 PERIOD_TIME,"
							// " B.REMARK, B.REMARK_2 STOP_TOTAL_TIME, B.DEP_NAME"
							// " FROM TFOSMT04A A"
							// " LEFT JOIN TFOSMT04B B ON A.FACTORY_DIV = B.FACTORY_DIV AND A.DATE_TIME = B.DATE_TIME AND A.SHIFT_NO = B.SHIFT_NO"
							// " WHERE A.FACTORY_DIV = 'A10'"
							// " AND ((A.DATE_TIME = @DATE_TIME AND A.SHIFT_NO = '1')"
							// " OR (A.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY,'YYYYMMDD') AND A.SHIFT_NO <> '1'))"
							// " ORDER BY A.DATE_TIME, A.SHIFT_NO, B.CHARGE_NO"
							// ;
// DM8 SQL：
						sqlstr =
							" SELECT A.FACTORY_DIV, A.DATE_TIME, A.SHIFT_NO, A.SHIFT_GROUP,"
							" A.PLAN_CHARGE, A.SMELT_CHARGE, A.PRODUCT_CHARGE, B.ADJUST_CHARGE,"
							" REMARK_1 PERIOD_TIME,"
							" B.REMARK, B.REMARK_2 STOP_TOTAL_TIME, B.DEP_NAME"
							" FROM TFOSMT04A A"
							" LEFT JOIN TFOSMT04B B ON A.FACTORY_DIV = B.FACTORY_DIV AND A.DATE_TIME = B.DATE_TIME AND A.SHIFT_NO = B.SHIFT_NO"
							" WHERE A.FACTORY_DIV = 'A10'"
							" AND ((A.DATE_TIME = @DATE_TIME AND A.SHIFT_NO = '1')"
							" OR (A.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMMDD') AND A.SHIFT_NO <> '1'))"
							" ORDER BY A.DATE_TIME, A.SHIFT_NO, B.CHARGE_NO"
							;
						break;
					}
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();
			}
			else if (tbl == "TFOSMT04A2")
			{
				//二炼钢生产调整
				bcls_ret->Tables.Add(tbl);
				CDateTime dt = CDateTime::Parse(date_time);
				if (dt.DayOfWeek() == 1)
				{
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
// DM8 适配 CHANGE-17：取查询日期前推3日的日期字符串;日历日边界、参数与结果列保持不变。
// 改写原因：DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
						// sqlstr =
							// " SELECT A.FACTORY_DIV, A.DATE_TIME, A.SHIFT_NO, A.SHIFT_GROUP,"
							// " A.PLAN_CHARGE, A.SMELT_CHARGE, A.PRODUCT_CHARGE, B.ADJUST_CHARGE,"
							// " REMARK_1 PERIOD_TIME,"
							// " B.REMARK, B.REMARK_2 STOP_TOTAL_TIME, B.DEP_NAME"
							// " FROM TFOSMT04A A"
							// " LEFT JOIN TFOSMT04B B ON A.FACTORY_DIV = B.FACTORY_DIV AND A.DATE_TIME = B.DATE_TIME AND A.SHIFT_NO = B.SHIFT_NO"
							// " WHERE A.FACTORY_DIV = 'A20'"
							// " AND ((A.DATE_TIME = @DATE_TIME AND A.SHIFT_NO = '1')"
							// " OR (A.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 3 DAY,'YYYYMMDD') AND A.SHIFT_NO <> '1')"
							// " OR A.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 2 DAY,'YYYYMMDD')"
							// " OR A.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY,'YYYYMMDD'))"
							// " ORDER BY A.DATE_TIME, A.SHIFT_NO, B.CHARGE_NO"
							// ;
// DM8 SQL：
						sqlstr =
							" SELECT A.FACTORY_DIV, A.DATE_TIME, A.SHIFT_NO, A.SHIFT_GROUP,"
							" A.PLAN_CHARGE, A.SMELT_CHARGE, A.PRODUCT_CHARGE, B.ADJUST_CHARGE,"
							" REMARK_1 PERIOD_TIME,"
							" B.REMARK, B.REMARK_2 STOP_TOTAL_TIME, B.DEP_NAME"
							" FROM TFOSMT04A A"
							" LEFT JOIN TFOSMT04B B ON A.FACTORY_DIV = B.FACTORY_DIV AND A.DATE_TIME = B.DATE_TIME AND A.SHIFT_NO = B.SHIFT_NO"
							" WHERE A.FACTORY_DIV = 'A20'"
							" AND ((A.DATE_TIME = @DATE_TIME AND A.SHIFT_NO = '1')"
							" OR (A.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 3,'YYYYMMDD') AND A.SHIFT_NO <> '1')"
							" OR A.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 2,'YYYYMMDD')"
							" OR A.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMMDD'))"
							" ORDER BY A.DATE_TIME, A.SHIFT_NO, B.CHARGE_NO"
							;
						break;
					}
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
// DM8 适配 CHANGE-18：取查询日期前推1日的日期字符串;日历日边界、参数与结果列保持不变。
// 改写原因：DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
						// sqlstr =
							// " SELECT A.FACTORY_DIV, A.DATE_TIME, A.SHIFT_NO, A.SHIFT_GROUP,"
							// " A.PLAN_CHARGE, A.SMELT_CHARGE, A.PRODUCT_CHARGE, B.ADJUST_CHARGE,"
							// " REMARK_1 PERIOD_TIME,"
							// " B.REMARK, B.REMARK_2 STOP_TOTAL_TIME, B.DEP_NAME"
							// " FROM TFOSMT04A A"
							// " LEFT JOIN TFOSMT04B B ON A.FACTORY_DIV = B.FACTORY_DIV AND A.DATE_TIME = B.DATE_TIME AND A.SHIFT_NO = B.SHIFT_NO"
							// " WHERE A.FACTORY_DIV = 'A20'"
							// " AND ((A.DATE_TIME = @DATE_TIME AND A.SHIFT_NO = '1')"
							// " OR (A.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY,'YYYYMMDD') AND A.SHIFT_NO <> '1'))"
							// " ORDER BY A.DATE_TIME, A.SHIFT_NO, B.CHARGE_NO"
							// ;
// DM8 SQL：
						sqlstr =
							" SELECT A.FACTORY_DIV, A.DATE_TIME, A.SHIFT_NO, A.SHIFT_GROUP,"
							" A.PLAN_CHARGE, A.SMELT_CHARGE, A.PRODUCT_CHARGE, B.ADJUST_CHARGE,"
							" REMARK_1 PERIOD_TIME,"
							" B.REMARK, B.REMARK_2 STOP_TOTAL_TIME, B.DEP_NAME"
							" FROM TFOSMT04A A"
							" LEFT JOIN TFOSMT04B B ON A.FACTORY_DIV = B.FACTORY_DIV AND A.DATE_TIME = B.DATE_TIME AND A.SHIFT_NO = B.SHIFT_NO"
							" WHERE A.FACTORY_DIV = 'A20'"
							" AND ((A.DATE_TIME = @DATE_TIME AND A.SHIFT_NO = '1')"
							" OR (A.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMMDD') AND A.SHIFT_NO <> '1'))"
							" ORDER BY A.DATE_TIME, A.SHIFT_NO, B.CHARGE_NO"
							;
						break;
					}
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();
			}
			else if (tbl == "TFOSMT05A1")
			{
				//一炼钢保留情况
				bcls_ret->Tables.Add(tbl);
				CDateTime dt = CDateTime::Parse(date_time);
				if (dt.DayOfWeek() == 1)
				{
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
// DM8 适配 CHANGE-19：取查询日期前推3日的日期字符串;日历日边界、参数与结果列保持不变。
// 改写原因：DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
						// sqlstr =
							// " SELECT ROW_NUMBER() OVER (ORDER BY DATE_TIME, PROD_TIME, HEAT_NO, SEQ_NO) SEQ_NO,"
							// " DATE_TIME, SHIFT_NO, SHIFT_GROUP, HEAT_NO, ST_NO, ELM_DESC, TO_NUMBER(MAIN_MIN) MAIN_MIN, TO_NUMBER(MAIN_MAX) MAIN_MAX,"
							// " TO_NUMBER(ELM_BOF_VALUE) ELM_BOF_VALUE_O,"
							// " TO_NUMBER(ELM_SR_VALUE) ELM_SR_VALUE_O,"
							// " TO_NUMBER(ELM_CC_VALUE) ELM_CC_VALUE_O,"
							// " DEP_NAME, REMARK"
							// " FROM TFOSMT05A A"
							// " WHERE FACTORY_DIV = 'A10'"
							// " AND ((A.DATE_TIME = @DATE_TIME AND A.SHIFT_NO = '1')"
							// " OR (A.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 3 DAY,'YYYYMMDD') AND A.SHIFT_NO <> '1')"
							// " OR A.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 2 DAY,'YYYYMMDD')"
							// " OR A.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY,'YYYYMMDD'))"
							// " ORDER BY DATE_TIME, PROD_TIME, HEAT_NO, SEQ_NO"
							// ;
// DM8 SQL：
						sqlstr =
							" SELECT ROW_NUMBER() OVER (ORDER BY DATE_TIME, PROD_TIME, HEAT_NO, SEQ_NO) SEQ_NO,"
							" DATE_TIME, SHIFT_NO, SHIFT_GROUP, HEAT_NO, ST_NO, ELM_DESC, TO_NUMBER(MAIN_MIN) MAIN_MIN, TO_NUMBER(MAIN_MAX) MAIN_MAX,"
							" TO_NUMBER(ELM_BOF_VALUE) ELM_BOF_VALUE_O,"
							" TO_NUMBER(ELM_SR_VALUE) ELM_SR_VALUE_O,"
							" TO_NUMBER(ELM_CC_VALUE) ELM_CC_VALUE_O,"
							" DEP_NAME, REMARK"
							" FROM TFOSMT05A A"
							" WHERE FACTORY_DIV = 'A10'"
							" AND ((A.DATE_TIME = @DATE_TIME AND A.SHIFT_NO = '1')"
							" OR (A.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 3,'YYYYMMDD') AND A.SHIFT_NO <> '1')"
							" OR A.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 2,'YYYYMMDD')"
							" OR A.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMMDD'))"
							" ORDER BY DATE_TIME, PROD_TIME, HEAT_NO, SEQ_NO"
							;
						break;
					}
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
// DM8 适配 CHANGE-20：取查询日期前推1日的日期字符串;日历日边界、参数与结果列保持不变。
// 改写原因：DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
						// sqlstr =
							// " SELECT ROW_NUMBER() OVER (ORDER BY DATE_TIME, PROD_TIME, HEAT_NO, SEQ_NO) SEQ_NO,"
							// " DATE_TIME, SHIFT_NO, SHIFT_GROUP, HEAT_NO, ST_NO, ELM_DESC, TO_NUMBER(MAIN_MIN) MAIN_MIN, TO_NUMBER(MAIN_MAX) MAIN_MAX,"
							// " TO_NUMBER(ELM_BOF_VALUE) ELM_BOF_VALUE_O,"
							// " TO_NUMBER(ELM_SR_VALUE) ELM_SR_VALUE_O,"
							// " TO_NUMBER(ELM_CC_VALUE) ELM_CC_VALUE_O,"
							// " DEP_NAME, REMARK"
							// " FROM TFOSMT05A A"
							// " WHERE FACTORY_DIV = 'A10'"
							// " AND ((A.DATE_TIME = @DATE_TIME AND A.SHIFT_NO = '1')"
							// " OR (A.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY,'YYYYMMDD') AND A.SHIFT_NO <> '1'))"
							// " ORDER BY DATE_TIME, PROD_TIME, HEAT_NO, SEQ_NO"
							// ;
// DM8 SQL：
						sqlstr =
							" SELECT ROW_NUMBER() OVER (ORDER BY DATE_TIME, PROD_TIME, HEAT_NO, SEQ_NO) SEQ_NO,"
							" DATE_TIME, SHIFT_NO, SHIFT_GROUP, HEAT_NO, ST_NO, ELM_DESC, TO_NUMBER(MAIN_MIN) MAIN_MIN, TO_NUMBER(MAIN_MAX) MAIN_MAX,"
							" TO_NUMBER(ELM_BOF_VALUE) ELM_BOF_VALUE_O,"
							" TO_NUMBER(ELM_SR_VALUE) ELM_SR_VALUE_O,"
							" TO_NUMBER(ELM_CC_VALUE) ELM_CC_VALUE_O,"
							" DEP_NAME, REMARK"
							" FROM TFOSMT05A A"
							" WHERE FACTORY_DIV = 'A10'"
							" AND ((A.DATE_TIME = @DATE_TIME AND A.SHIFT_NO = '1')"
							" OR (A.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMMDD') AND A.SHIFT_NO <> '1'))"
							" ORDER BY DATE_TIME, PROD_TIME, HEAT_NO, SEQ_NO"
							;
						break;
					}
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();

				bcls_ret->Tables[tbl].Columns.Add(DT_STRING, "SPECIFICATION_RANGE");
				bcls_ret->Tables[tbl].Columns.Add(DT_STRING, "ELM_BOF_VALUE");
				bcls_ret->Tables[tbl].Columns.Add(DT_STRING, "ELM_SR_VALUE");
				bcls_ret->Tables[tbl].Columns.Add(DT_STRING, "ELM_CC_VALUE");
				for (int i = 0; i < bcls_ret->Tables[tbl].Rows.get_Count(); i++)
				{
					CDecimal value = bcls_ret->Tables[tbl].Rows[i]["MAIN_MIN"].ToDecimal();
					if (value == 0)
					{
						bcls_ret->Tables[tbl].Rows[i]["SPECIFICATION_RANGE"] = "0";
					}
					else
					{
						CString v_str = value.ToString();
						for (int j = v_str.GetLength(); j > 0; j--)
						{
							if (v_str.Substring(j - 1, 1) == "0")
							{
								v_str = v_str.Substring(0, j - 1);
							}
							else
							{
								break;
							}
						}
						bcls_ret->Tables[tbl].Rows[i]["SPECIFICATION_RANGE"] = v_str;
					}
					value = bcls_ret->Tables[tbl].Rows[i]["MAIN_MAX"].ToDecimal();
					if (value == 0)
					{
						bcls_ret->Tables[tbl].Rows[i]["SPECIFICATION_RANGE"] = bcls_ret->Tables[tbl].Rows[i]["SPECIFICATION_RANGE"].ToString() + "-0";
					}
					else
					{
						CString v_str = value.ToString();
						for (int j = v_str.GetLength(); j > 0; j--)
						{
							if (v_str.Substring(j - 1, 1) == "0")
							{
								v_str = v_str.Substring(0, j - 1);
							}
							else
							{
								break;
							}
						}
						bcls_ret->Tables[tbl].Rows[i]["SPECIFICATION_RANGE"] = bcls_ret->Tables[tbl].Rows[i]["SPECIFICATION_RANGE"].ToString() + "-" + v_str;
					}

					CString col = "ELM_BOF_VALUE";
					value = bcls_ret->Tables[tbl].Rows[i][col + "_O"].ToDecimal();
					if (value == 0)
					{
						bcls_ret->Tables[tbl].Rows[i][col] = "/";
					}
					else
					{
						CString v_str = value.ToString();
						for (int j = v_str.GetLength(); j > 0; j--)
						{
							if (v_str.Substring(j - 1, 1) == "0")
							{
								v_str = v_str.Substring(0, j - 1);
							}
							else
							{
								break;
							}
						}
						bcls_ret->Tables[tbl].Rows[i][col] = v_str;
					}

					col = "ELM_SR_VALUE";
					value = bcls_ret->Tables[tbl].Rows[i][col + "_O"].ToDecimal();
					if (value == 0)
					{
						bcls_ret->Tables[tbl].Rows[i][col] = "/";
					}
					else
					{
						CString v_str = value.ToString();
						for (int j = v_str.GetLength(); j > 0; j--)
						{
							if (v_str.Substring(j - 1, 1) == "0")
							{
								v_str = v_str.Substring(0, j - 1);
							}
							else
							{
								break;
							}
						}
						bcls_ret->Tables[tbl].Rows[i][col] = v_str;
					}
					col = "ELM_CC_VALUE";
					value = bcls_ret->Tables[tbl].Rows[i][col + "_O"].ToDecimal();
					if (value == 0)
					{
						bcls_ret->Tables[tbl].Rows[i][col] = "/";
					}
					else
					{
						CString v_str = value.ToString();
						for (int j = v_str.GetLength(); j > 0; j--)
						{
							if (v_str.Substring(j - 1, 1) == "0")
							{
								v_str = v_str.Substring(0, j - 1);
							}
							else
							{
								break;
							}
						}
						bcls_ret->Tables[tbl].Rows[i][col] = v_str;
					}
				}
			}
			else if (tbl == "TFOSMT05A2")
			{
				//二炼钢保留情况
				bcls_ret->Tables.Add(tbl);
				CDateTime dt = CDateTime::Parse(date_time);
				if (dt.DayOfWeek() == 1)
				{
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
// DM8 适配 CHANGE-21：取查询日期前推3日的日期字符串;日历日边界、参数与结果列保持不变。
// 改写原因：DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
						// sqlstr =
							// " SELECT ROW_NUMBER() OVER (ORDER BY DATE_TIME, PROD_TIME, HEAT_NO, SEQ_NO) SEQ_NO,"
							// " DATE_TIME, SHIFT_NO, SHIFT_GROUP, HEAT_NO, ST_NO, ELM_DESC, TO_NUMBER(MAIN_MIN) MAIN_MIN, TO_NUMBER(MAIN_MAX) MAIN_MAX,"
							// " TO_NUMBER(ELM_BOF_VALUE) ELM_BOF_VALUE_O,"
							// " TO_NUMBER(ELM_SR_VALUE) ELM_SR_VALUE_O,"
							// " TO_NUMBER(ELM_CC_VALUE) ELM_CC_VALUE_O,"
							// " DEP_NAME, REMARK"
							// " FROM TFOSMT05A A"
							// " WHERE FACTORY_DIV = 'A20'"
							// " AND ((A.DATE_TIME = @DATE_TIME AND A.SHIFT_NO = '1')"
							// " OR (A.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 3 DAY,'YYYYMMDD') AND A.SHIFT_NO <> '1')"
							// " OR A.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 2 DAY,'YYYYMMDD')"
							// " OR A.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY,'YYYYMMDD'))"
							// " ORDER BY DATE_TIME, PROD_TIME, HEAT_NO, SEQ_NO"
							// ;
// DM8 SQL：
						sqlstr =
							" SELECT ROW_NUMBER() OVER (ORDER BY DATE_TIME, PROD_TIME, HEAT_NO, SEQ_NO) SEQ_NO,"
							" DATE_TIME, SHIFT_NO, SHIFT_GROUP, HEAT_NO, ST_NO, ELM_DESC, TO_NUMBER(MAIN_MIN) MAIN_MIN, TO_NUMBER(MAIN_MAX) MAIN_MAX,"
							" TO_NUMBER(ELM_BOF_VALUE) ELM_BOF_VALUE_O,"
							" TO_NUMBER(ELM_SR_VALUE) ELM_SR_VALUE_O,"
							" TO_NUMBER(ELM_CC_VALUE) ELM_CC_VALUE_O,"
							" DEP_NAME, REMARK"
							" FROM TFOSMT05A A"
							" WHERE FACTORY_DIV = 'A20'"
							" AND ((A.DATE_TIME = @DATE_TIME AND A.SHIFT_NO = '1')"
							" OR (A.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 3,'YYYYMMDD') AND A.SHIFT_NO <> '1')"
							" OR A.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 2,'YYYYMMDD')"
							" OR A.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMMDD'))"
							" ORDER BY DATE_TIME, PROD_TIME, HEAT_NO, SEQ_NO"
							;
						break;
					}
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
// DM8 适配 CHANGE-22：取查询日期前推1日的日期字符串;日历日边界、参数与结果列保持不变。
// 改写原因：DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
						// sqlstr =
							// " SELECT ROW_NUMBER() OVER (ORDER BY DATE_TIME, PROD_TIME, HEAT_NO, SEQ_NO) SEQ_NO,"
							// " DATE_TIME, SHIFT_NO, SHIFT_GROUP, HEAT_NO, ST_NO, ELM_DESC, TO_NUMBER(MAIN_MIN) MAIN_MIN, TO_NUMBER(MAIN_MAX) MAIN_MAX,"
							// " TO_NUMBER(ELM_BOF_VALUE) ELM_BOF_VALUE_O,"
							// " TO_NUMBER(ELM_SR_VALUE) ELM_SR_VALUE_O,"
							// " TO_NUMBER(ELM_CC_VALUE) ELM_CC_VALUE_O,"
							// " DEP_NAME, REMARK"
							// " FROM TFOSMT05A A"
							// " WHERE FACTORY_DIV = 'A20'"
							// " AND ((A.DATE_TIME = @DATE_TIME AND A.SHIFT_NO = '1')"
							// " OR (A.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY,'YYYYMMDD') AND A.SHIFT_NO <> '1'))"
							// " ORDER BY DATE_TIME, PROD_TIME, HEAT_NO, SEQ_NO"
							// ;
// DM8 SQL：
						sqlstr =
							" SELECT ROW_NUMBER() OVER (ORDER BY DATE_TIME, PROD_TIME, HEAT_NO, SEQ_NO) SEQ_NO,"
							" DATE_TIME, SHIFT_NO, SHIFT_GROUP, HEAT_NO, ST_NO, ELM_DESC, TO_NUMBER(MAIN_MIN) MAIN_MIN, TO_NUMBER(MAIN_MAX) MAIN_MAX,"
							" TO_NUMBER(ELM_BOF_VALUE) ELM_BOF_VALUE_O,"
							" TO_NUMBER(ELM_SR_VALUE) ELM_SR_VALUE_O,"
							" TO_NUMBER(ELM_CC_VALUE) ELM_CC_VALUE_O,"
							" DEP_NAME, REMARK"
							" FROM TFOSMT05A A"
							" WHERE FACTORY_DIV = 'A20'"
							" AND ((A.DATE_TIME = @DATE_TIME AND A.SHIFT_NO = '1')"
							" OR (A.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMMDD') AND A.SHIFT_NO <> '1'))"
							" ORDER BY DATE_TIME, PROD_TIME, HEAT_NO, SEQ_NO"
							;
						break;
					}
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();

				bcls_ret->Tables[tbl].Columns.Add(DT_STRING, "SPECIFICATION_RANGE");
				bcls_ret->Tables[tbl].Columns.Add(DT_STRING, "ELM_BOF_VALUE");
				bcls_ret->Tables[tbl].Columns.Add(DT_STRING, "ELM_SR_VALUE");
				bcls_ret->Tables[tbl].Columns.Add(DT_STRING, "ELM_CC_VALUE");
				for (int i = 0; i < bcls_ret->Tables[tbl].Rows.get_Count(); i++)
				{
					CDecimal value = bcls_ret->Tables[tbl].Rows[i]["MAIN_MIN"].ToDecimal();
					if (value == 0)
					{
						bcls_ret->Tables[tbl].Rows[i]["SPECIFICATION_RANGE"] = "0";
					}
					else
					{
						CString v_str = value.ToString();
						for (int j = v_str.GetLength(); j > 0; j--)
						{
							if (v_str.Substring(j - 1, 1) == "0")
							{
								v_str = v_str.Substring(0, j - 1);
							}
							else
							{
								break;
							}
						}
						bcls_ret->Tables[tbl].Rows[i]["SPECIFICATION_RANGE"] = v_str;
					}
					value = bcls_ret->Tables[tbl].Rows[i]["MAIN_MAX"].ToDecimal();
					if (value == 0)
					{
						bcls_ret->Tables[tbl].Rows[i]["SPECIFICATION_RANGE"] = bcls_ret->Tables[tbl].Rows[i]["SPECIFICATION_RANGE"].ToString() + "-0";
					}
					else
					{
						CString v_str = value.ToString();
						for (int j = v_str.GetLength(); j > 0; j--)
						{
							if (v_str.Substring(j - 1, 1) == "0")
							{
								v_str = v_str.Substring(0, j - 1);
							}
							else
							{
								break;
							}
						}
						bcls_ret->Tables[tbl].Rows[i]["SPECIFICATION_RANGE"] = bcls_ret->Tables[tbl].Rows[i]["SPECIFICATION_RANGE"].ToString() + "-" + v_str;
					}

					CString col = "ELM_BOF_VALUE";
					value = bcls_ret->Tables[tbl].Rows[i][col + "_O"].ToDecimal();
					if (value == 0)
					{
						bcls_ret->Tables[tbl].Rows[i][col] = "/";
					}
					else
					{
						CString v_str = value.ToString();
						for (int j = v_str.GetLength(); j > 0; j--)
						{
							if (v_str.Substring(j - 1, 1) == "0")
							{
								v_str = v_str.Substring(0, j - 1);
							}
							else
							{
								break;
							}
						}
						bcls_ret->Tables[tbl].Rows[i][col] = v_str;
					}

					col = "ELM_SR_VALUE";
					value = bcls_ret->Tables[tbl].Rows[i][col + "_O"].ToDecimal();
					if (value == 0)
					{
						bcls_ret->Tables[tbl].Rows[i][col] = "/";
					}
					else
					{
						CString v_str = value.ToString();
						for (int j = v_str.GetLength(); j > 0; j--)
						{
							if (v_str.Substring(j - 1, 1) == "0")
							{
								v_str = v_str.Substring(0, j - 1);
							}
							else
							{
								break;
							}
						}
						bcls_ret->Tables[tbl].Rows[i][col] = v_str;
					}
					col = "ELM_CC_VALUE";
					value = bcls_ret->Tables[tbl].Rows[i][col + "_O"].ToDecimal();
					if (value == 0)
					{
						bcls_ret->Tables[tbl].Rows[i][col] = "/";
					}
					else
					{
						CString v_str = value.ToString();
						for (int j = v_str.GetLength(); j > 0; j--)
						{
							if (v_str.Substring(j - 1, 1) == "0")
							{
								v_str = v_str.Substring(0, j - 1);
							}
							else
							{
								break;
							}
						}
						bcls_ret->Tables[tbl].Rows[i][col] = v_str;
					}
				}
			}
			else if (tbl == "TFOSMT05B1")
			{
				//一炼钢保留炉数跟踪（每日）
				bcls_ret->Tables.Add(tbl);
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-23：生成查询日期之前7天的日期序列;日期边界、参数与结果列保持不变。
// 改写原因：SYSIBM 辅助表改为 DUAL；DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " WITH A(LEVEL, TIME) AS"
						// " (SELECT 1, TO_DATE(@DATE_TIME,'YYYYMMDD') - 7 DAY"
						// " FROM SYSIBM.SYSDUMMY1"
						// " WHERE 1 = 1"
						// " UNION ALL"
						// " SELECT LEVEL + 1,TIME + 1 DAY"
						// " FROM SYSIBM.SYSDUMMY1, A"
						// " WHERE A.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY)"
						// " SELECT TO_CHAR(A.TIME,'YYYYMMDD') DATE_TIME,"
						// " COUNT(DISTINCT HEAT_NO) COUNT_PONO"
						// " FROM A"
						// " LEFT JOIN TFOSMT05A B ON TO_CHAR(A.TIME,'YYYYMMDD') = B.DATE_TIME AND B.FACTORY_DIV = 'A10'"
						// " WHERE 1 = 1"
						// " GROUP BY A.TIME"
						// " ORDER BY A.TIME"
						// ;
// DM8 SQL：
					sqlstr =
						" WITH A(LEVEL, TIME) AS"
						" (SELECT 1, TO_DATE(@DATE_TIME,'YYYYMMDD') - 7"
						" FROM DUAL"
						" WHERE 1 = 1"
						" UNION ALL"
						" SELECT LEVEL + 1,TIME + 1"
						" FROM DUAL, A"
						" WHERE A.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD') - 1)"
						" SELECT TO_CHAR(A.TIME,'YYYYMMDD') DATE_TIME,"
						" COUNT(DISTINCT HEAT_NO) COUNT_PONO"
						" FROM A"
						" LEFT JOIN TFOSMT05A B ON TO_CHAR(A.TIME,'YYYYMMDD') = B.DATE_TIME AND B.FACTORY_DIV = 'A10'"
						" WHERE 1 = 1"
						" GROUP BY A.TIME"
						" ORDER BY A.TIME"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();
			}
			else if (tbl == "TFOSMT05B2")
			{
				//二炼钢保留炉数跟踪（每日）
				bcls_ret->Tables.Add(tbl);
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-24：生成查询日期之前7天的日期序列;日期边界、参数与结果列保持不变。
// 改写原因：SYSIBM 辅助表改为 DUAL；DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " WITH A(LEVEL, TIME) AS"
						// " (SELECT 1, TO_DATE(@DATE_TIME,'YYYYMMDD') - 7 DAY"
						// " FROM SYSIBM.SYSDUMMY1"
						// " WHERE 1 = 1"
						// " UNION ALL"
						// " SELECT LEVEL + 1,TIME + 1 DAY"
						// " FROM SYSIBM.SYSDUMMY1, A"
						// " WHERE A.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY)"
						// " SELECT TO_CHAR(A.TIME,'YYYYMMDD') DATE_TIME,"
						// " COUNT(DISTINCT HEAT_NO) COUNT_PONO"
						// " FROM A"
						// " LEFT JOIN TFOSMT05A B ON TO_CHAR(A.TIME,'YYYYMMDD') = B.DATE_TIME AND B.FACTORY_DIV = 'A20'"
						// " WHERE 1 = 1"
						// " GROUP BY A.TIME"
						// " ORDER BY A.TIME"
						// ;
// DM8 SQL：
					sqlstr =
						" WITH A(LEVEL, TIME) AS"
						" (SELECT 1, TO_DATE(@DATE_TIME,'YYYYMMDD') - 7"
						" FROM DUAL"
						" WHERE 1 = 1"
						" UNION ALL"
						" SELECT LEVEL + 1,TIME + 1"
						" FROM DUAL, A"
						" WHERE A.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD') - 1)"
						" SELECT TO_CHAR(A.TIME,'YYYYMMDD') DATE_TIME,"
						" COUNT(DISTINCT HEAT_NO) COUNT_PONO"
						" FROM A"
						" LEFT JOIN TFOSMT05A B ON TO_CHAR(A.TIME,'YYYYMMDD') = B.DATE_TIME AND B.FACTORY_DIV = 'A20'"
						" WHERE 1 = 1"
						" GROUP BY A.TIME"
						" ORDER BY A.TIME"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();
			}
			else if (tbl == "TFOSMT05C1")
			{
				//一炼钢保留元素炉数统计（月度）
				bcls_ret->Tables.Add(tbl);
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-25：取查询日期前推1日的日期字符串;日历日边界、参数与结果列保持不变。
// 改写原因：DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " SELECT ELM_DESC, COUNT(ELM_DESC) COUNT_PONO"
						// " FROM TFOSMT05A"
						// " WHERE FACTORY_DIV = 'A10'"
						// " AND DATE_TIME >= SUBSTR(TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS,'YYYYMMDD'),1,6) || '01'"
						// " AND DATE_TIME <= TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS,'YYYYMMDD')"
						// " GROUP BY ELM_DESC"
						// " ORDER BY ELM_DESC"
						// ;
// DM8 SQL：
					sqlstr =
						" SELECT ELM_DESC, COUNT(ELM_DESC) COUNT_PONO"
						" FROM TFOSMT05A"
						" WHERE FACTORY_DIV = 'A10'"
						" AND DATE_TIME >= SUBSTR(TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMMDD'),1,6) || '01'"
						" AND DATE_TIME <= TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMMDD')"
						" GROUP BY ELM_DESC"
						" ORDER BY ELM_DESC"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();
			}
			else if (tbl == "TFOSMT05C2")
			{
				//二炼钢保留元素炉数统计（月度）
				bcls_ret->Tables.Add(tbl);
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-26：取查询日期前推1日的日期字符串;日历日边界、参数与结果列保持不变。
// 改写原因：DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " SELECT ELM_DESC, COUNT(ELM_DESC) COUNT_PONO"
						// " FROM TFOSMT05A"
						// " WHERE FACTORY_DIV = 'A20'"
						// " AND DATE_TIME >= SUBSTR(TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS,'YYYYMMDD'),1,6) || '01'"
						// " AND DATE_TIME <= TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS,'YYYYMMDD')"
						// " GROUP BY ELM_DESC"
						// " ORDER BY ELM_DESC"
						// ;
// DM8 SQL：
					sqlstr =
						" SELECT ELM_DESC, COUNT(ELM_DESC) COUNT_PONO"
						" FROM TFOSMT05A"
						" WHERE FACTORY_DIV = 'A20'"
						" AND DATE_TIME >= SUBSTR(TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMMDD'),1,6) || '01'"
						" AND DATE_TIME <= TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMMDD')"
						" GROUP BY ELM_DESC"
						" ORDER BY ELM_DESC"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();
			}
			else if (tbl == "TFOSMT05D1")
			{
				//一炼钢改钢情况
				bcls_ret->Tables.Add(tbl);
				CDateTime dt = CDateTime::Parse(date_time);
				if (dt.DayOfWeek() == 1)
				{
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
// DM8 适配 CHANGE-27：取查询日期前推4日的日期字符串;日历日边界、参数与结果列保持不变。
// 改写原因：DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
						// sqlstr =
							// " SELECT DATE_TIME, SHIFT_GROUP, HEAT_NO, OLD_ST_NO, ST_NO, REMARK"
							// " FROM TFOSMT05B"
							// " WHERE FACTORY_DIV = 'A10'"
							// " AND DATE_TIME > TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 4 DAY,'YYYYMMDD')"
							// " AND DATE_TIME <= @DATE_TIME"
							// ;
// DM8 SQL：
						sqlstr =
							" SELECT DATE_TIME, SHIFT_GROUP, HEAT_NO, OLD_ST_NO, ST_NO, REMARK"
							" FROM TFOSMT05B"
							" WHERE FACTORY_DIV = 'A10'"
							" AND DATE_TIME > TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 4,'YYYYMMDD')"
							" AND DATE_TIME <= @DATE_TIME"
							;
						break;
					}
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
// DM8 适配 CHANGE-28：取查询日期前推2日的日期字符串;日历日边界、参数与结果列保持不变。
// 改写原因：DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
						// sqlstr =
							// " SELECT DATE_TIME, SHIFT_GROUP, HEAT_NO, OLD_ST_NO, ST_NO, REMARK"
							// " FROM TFOSMT05B"
							// " WHERE FACTORY_DIV = 'A10'"
							// " AND DATE_TIME > TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 2 DAY,'YYYYMMDD')"
							// " AND DATE_TIME <= @DATE_TIME"
							// ;
// DM8 SQL：
						sqlstr =
							" SELECT DATE_TIME, SHIFT_GROUP, HEAT_NO, OLD_ST_NO, ST_NO, REMARK"
							" FROM TFOSMT05B"
							" WHERE FACTORY_DIV = 'A10'"
							" AND DATE_TIME > TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 2,'YYYYMMDD')"
							" AND DATE_TIME <= @DATE_TIME"
							;
						break;
					}
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();
			}
			else if (tbl == "TFOSMT05D2")
			{
				//二炼钢改钢情况
				bcls_ret->Tables.Add(tbl);
				CDateTime dt = CDateTime::Parse(date_time);
				if (dt.DayOfWeek() == 1)
				{
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
// DM8 适配 CHANGE-29：取查询日期前推4日的日期字符串;日历日边界、参数与结果列保持不变。
// 改写原因：DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
						// sqlstr =
							// " SELECT DATE_TIME, SHIFT_GROUP, HEAT_NO, OLD_ST_NO, ST_NO, REMARK"
							// " FROM TFOSMT05B"
							// " WHERE FACTORY_DIV = 'A20'"
							// " AND DATE_TIME > TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 4 DAY,'YYYYMMDD')"
							// " AND DATE_TIME <= @DATE_TIME"
							// ;
// DM8 SQL：
						sqlstr =
							" SELECT DATE_TIME, SHIFT_GROUP, HEAT_NO, OLD_ST_NO, ST_NO, REMARK"
							" FROM TFOSMT05B"
							" WHERE FACTORY_DIV = 'A20'"
							" AND DATE_TIME > TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 4,'YYYYMMDD')"
							" AND DATE_TIME <= @DATE_TIME"
							;
						break;
					}
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
// DM8 适配 CHANGE-30：取查询日期前推2日的日期字符串;日历日边界、参数与结果列保持不变。
// 改写原因：DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
						// sqlstr =
							// " SELECT DATE_TIME, SHIFT_GROUP, HEAT_NO, OLD_ST_NO, ST_NO, REMARK"
							// " FROM TFOSMT05B"
							// " WHERE FACTORY_DIV = 'A20'"
							// " AND DATE_TIME > TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 2 DAY,'YYYYMMDD')"
							// " AND DATE_TIME <= @DATE_TIME"
							// ;
// DM8 SQL：
						sqlstr =
							" SELECT DATE_TIME, SHIFT_GROUP, HEAT_NO, OLD_ST_NO, ST_NO, REMARK"
							" FROM TFOSMT05B"
							" WHERE FACTORY_DIV = 'A20'"
							" AND DATE_TIME > TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 2,'YYYYMMDD')"
							" AND DATE_TIME <= @DATE_TIME"
							;
						break;
					}
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();
			}
			else if (tbl == "TFOSMT05E")
			{
				//恒拉速率指标（二炼钢）
				bcls_ret->Tables.Add(tbl);
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-31：取查询日期前推1日的日期字符串;日历日边界、参数与结果列保持不变。
// 改写原因：DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " WITH T AS ( SELECT DATE_TIME, SHIFT_GROUP, CC_MACH_NO, PRODUCT_CHARGE, QUALIFIED_CHARGE,"
						// " 100 * CAST(ROUND(DECODE(PRODUCT_CHARGE,0,0,CAST(QUALIFIED_CHARGE AS DECIMAL)/CAST(PRODUCT_CHARGE AS DECIMAL)), 4) AS DECIMAL(5,4)) QUALIFIED_RATE, REMARK"
						// " FROM TFOSMT05C"
						// " WHERE FACTORY_DIV = 'A20'"
						// " AND DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS,'YYYYMMDD') )"
						// " SELECT * FROM ("
						// " SELECT *"
						// " FROM T"
						// " WHERE SHIFT_GROUP = 'A'"
						// " UNION ALL"
						// " SELECT DATE_TIME, SHIFT_GROUP, '合计' CC_MACH_NO, SUM(PRODUCT_CHARGE), SUM(QUALIFIED_CHARGE),"
						// " 100 * CAST(ROUND(DECODE(SUM(PRODUCT_CHARGE),0,0,CAST(SUM(QUALIFIED_CHARGE) AS DECIMAL)/CAST(SUM(PRODUCT_CHARGE) AS DECIMAL)), 4) AS DECIMAL(5,4)) QUALIFIED_RATE, ' ' REMARK"
						// " FROM T"
						// " WHERE SHIFT_GROUP = 'A'"
						// " GROUP BY DATE_TIME, SHIFT_GROUP"
						// " "
						// " UNION ALL"
						// " SELECT *"
						// " FROM T"
						// " WHERE SHIFT_GROUP = 'B'"
						// " UNION ALL"
						// " SELECT DATE_TIME, SHIFT_GROUP, '合计' CC_MACH_NO, SUM(PRODUCT_CHARGE), SUM(QUALIFIED_CHARGE),"
						// " 100 * CAST(ROUND(DECODE(SUM(PRODUCT_CHARGE),0,0,CAST(SUM(QUALIFIED_CHARGE) AS DECIMAL)/CAST(SUM(PRODUCT_CHARGE) AS DECIMAL)), 4) AS DECIMAL(5,4)) QUALIFIED_RATE, ' ' REMARK"
						// " FROM T"
						// " WHERE SHIFT_GROUP = 'B'"
						// " GROUP BY DATE_TIME, SHIFT_GROUP"
						// " "
						// " UNION ALL"
						// " SELECT *"
						// " FROM T"
						// " WHERE SHIFT_GROUP = 'C'"
						// " UNION ALL"
						// " SELECT DATE_TIME, SHIFT_GROUP, '合计' CC_MACH_NO, SUM(PRODUCT_CHARGE), SUM(QUALIFIED_CHARGE),"
						// " 100 * CAST(ROUND(DECODE(SUM(PRODUCT_CHARGE),0,0,CAST(SUM(QUALIFIED_CHARGE) AS DECIMAL)/CAST(SUM(PRODUCT_CHARGE) AS DECIMAL)), 4) AS DECIMAL(5,4)) QUALIFIED_RATE, ' ' REMARK"
						// " FROM T"
						// " WHERE SHIFT_GROUP = 'C'"
						// " GROUP BY DATE_TIME, SHIFT_GROUP"
						// " "
						// " UNION ALL"
						// " SELECT *"
						// " FROM T"
						// " WHERE SHIFT_GROUP = 'D'"
						// " UNION ALL"
						// " SELECT DATE_TIME, SHIFT_GROUP, '合计' CC_MACH_NO, SUM(PRODUCT_CHARGE), SUM(QUALIFIED_CHARGE),"
						// " 100 * CAST(ROUND(DECODE(SUM(PRODUCT_CHARGE),0,0,CAST(SUM(QUALIFIED_CHARGE) AS DECIMAL)/CAST(SUM(PRODUCT_CHARGE) AS DECIMAL)), 4) AS DECIMAL(5,4)) QUALIFIED_RATE, ' ' REMARK"
						// " FROM T"
						// " WHERE SHIFT_GROUP = 'D'"
						// " GROUP BY DATE_TIME, SHIFT_GROUP"
						// " "
						// " UNION ALL"
						// " SELECT TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS,'YYYYMMDD') DATE_TIME, '当日合计' SHIFT_GROUP, '当日合计' CC_MACH_NO, SUM(PRODUCT_CHARGE), SUM(QUALIFIED_CHARGE),"
						// " 100 * CAST(ROUND(DECODE(SUM(PRODUCT_CHARGE),0,0,CAST(SUM(QUALIFIED_CHARGE) AS DECIMAL)/CAST(SUM(PRODUCT_CHARGE) AS DECIMAL)), 4) AS DECIMAL(5,4)) QUALIFIED_RATE, ' ' REMARK"
						// " FROM T"
						// " "
						// " UNION ALL"
						// " SELECT TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS,'YYYYMMDD') DATE_TIME, '月度累计' SHIFT_GROUP, '月度累计' CC_MACH_NO, SUM(PRODUCT_CHARGE), SUM(QUALIFIED_CHARGE),"
						// " 100 * CAST(ROUND(DECODE(SUM(PRODUCT_CHARGE),0,0,CAST(SUM(QUALIFIED_CHARGE) AS DECIMAL)/CAST(SUM(PRODUCT_CHARGE) AS DECIMAL)), 4) AS DECIMAL(5,4)) QUALIFIED_RATE, ' ' REMARK"
						// " FROM TFOSMT05C"
						// " WHERE FACTORY_DIV = 'A20'"
						// " AND SUBSTR(DATE_TIME, 1, 6) = SUBSTR(TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS,'YYYYMMDD'), 1, 6)"
						// " "
						// " )"
						// " ORDER BY SHIFT_GROUP, CC_MACH_NO"
						// ;
// DM8 SQL：
					sqlstr =
						" WITH T AS ( SELECT DATE_TIME, SHIFT_GROUP, CC_MACH_NO, PRODUCT_CHARGE, QUALIFIED_CHARGE,"
						" 100 * CAST(ROUND(DECODE(PRODUCT_CHARGE,0,0,CAST(QUALIFIED_CHARGE AS DECIMAL)/CAST(PRODUCT_CHARGE AS DECIMAL)), 4) AS DECIMAL(5,4)) QUALIFIED_RATE, REMARK"
						" FROM TFOSMT05C"
						" WHERE FACTORY_DIV = 'A20'"
						" AND DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMMDD') )"
						" SELECT * FROM ("
						" SELECT *"
						" FROM T"
						" WHERE SHIFT_GROUP = 'A'"
						" UNION ALL"
						" SELECT DATE_TIME, SHIFT_GROUP, '合计' CC_MACH_NO, SUM(PRODUCT_CHARGE), SUM(QUALIFIED_CHARGE),"
						" 100 * CAST(ROUND(DECODE(SUM(PRODUCT_CHARGE),0,0,CAST(SUM(QUALIFIED_CHARGE) AS DECIMAL)/CAST(SUM(PRODUCT_CHARGE) AS DECIMAL)), 4) AS DECIMAL(5,4)) QUALIFIED_RATE, ' ' REMARK"
						" FROM T"
						" WHERE SHIFT_GROUP = 'A'"
						" GROUP BY DATE_TIME, SHIFT_GROUP"
						" "
						" UNION ALL"
						" SELECT *"
						" FROM T"
						" WHERE SHIFT_GROUP = 'B'"
						" UNION ALL"
						" SELECT DATE_TIME, SHIFT_GROUP, '合计' CC_MACH_NO, SUM(PRODUCT_CHARGE), SUM(QUALIFIED_CHARGE),"
						" 100 * CAST(ROUND(DECODE(SUM(PRODUCT_CHARGE),0,0,CAST(SUM(QUALIFIED_CHARGE) AS DECIMAL)/CAST(SUM(PRODUCT_CHARGE) AS DECIMAL)), 4) AS DECIMAL(5,4)) QUALIFIED_RATE, ' ' REMARK"
						" FROM T"
						" WHERE SHIFT_GROUP = 'B'"
						" GROUP BY DATE_TIME, SHIFT_GROUP"
						" "
						" UNION ALL"
						" SELECT *"
						" FROM T"
						" WHERE SHIFT_GROUP = 'C'"
						" UNION ALL"
						" SELECT DATE_TIME, SHIFT_GROUP, '合计' CC_MACH_NO, SUM(PRODUCT_CHARGE), SUM(QUALIFIED_CHARGE),"
						" 100 * CAST(ROUND(DECODE(SUM(PRODUCT_CHARGE),0,0,CAST(SUM(QUALIFIED_CHARGE) AS DECIMAL)/CAST(SUM(PRODUCT_CHARGE) AS DECIMAL)), 4) AS DECIMAL(5,4)) QUALIFIED_RATE, ' ' REMARK"
						" FROM T"
						" WHERE SHIFT_GROUP = 'C'"
						" GROUP BY DATE_TIME, SHIFT_GROUP"
						" "
						" UNION ALL"
						" SELECT *"
						" FROM T"
						" WHERE SHIFT_GROUP = 'D'"
						" UNION ALL"
						" SELECT DATE_TIME, SHIFT_GROUP, '合计' CC_MACH_NO, SUM(PRODUCT_CHARGE), SUM(QUALIFIED_CHARGE),"
						" 100 * CAST(ROUND(DECODE(SUM(PRODUCT_CHARGE),0,0,CAST(SUM(QUALIFIED_CHARGE) AS DECIMAL)/CAST(SUM(PRODUCT_CHARGE) AS DECIMAL)), 4) AS DECIMAL(5,4)) QUALIFIED_RATE, ' ' REMARK"
						" FROM T"
						" WHERE SHIFT_GROUP = 'D'"
						" GROUP BY DATE_TIME, SHIFT_GROUP"
						" "
						" UNION ALL"
						" SELECT TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMMDD') DATE_TIME, '当日合计' SHIFT_GROUP, '当日合计' CC_MACH_NO, SUM(PRODUCT_CHARGE), SUM(QUALIFIED_CHARGE),"
						" 100 * CAST(ROUND(DECODE(SUM(PRODUCT_CHARGE),0,0,CAST(SUM(QUALIFIED_CHARGE) AS DECIMAL)/CAST(SUM(PRODUCT_CHARGE) AS DECIMAL)), 4) AS DECIMAL(5,4)) QUALIFIED_RATE, ' ' REMARK"
						" FROM T"
						" "
						" UNION ALL"
						" SELECT TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMMDD') DATE_TIME, '月度累计' SHIFT_GROUP, '月度累计' CC_MACH_NO, SUM(PRODUCT_CHARGE), SUM(QUALIFIED_CHARGE),"
						" 100 * CAST(ROUND(DECODE(SUM(PRODUCT_CHARGE),0,0,CAST(SUM(QUALIFIED_CHARGE) AS DECIMAL)/CAST(SUM(PRODUCT_CHARGE) AS DECIMAL)), 4) AS DECIMAL(5,4)) QUALIFIED_RATE, ' ' REMARK"
						" FROM TFOSMT05C"
						" WHERE FACTORY_DIV = 'A20'"
						" AND SUBSTR(DATE_TIME, 1, 6) = SUBSTR(TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMMDD'), 1, 6)"
						" "
						" )"
						" ORDER BY SHIFT_GROUP, CC_MACH_NO"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();
			}
			else if (tbl == "TFOSMT05F")
			{
				//3#CC、4#CC符合率（每日）
				bcls_ret->Tables.Add(tbl);
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-32：取查询日期前推7日的日期字符串;日历日边界、参数与结果列保持不变。
// 改写原因：DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " SELECT A.DATE_TIME,"
						// " DECODE(SUM(A.PRODUCT_CHARGE),0,0,ROUND(100.00 * SUM(A.QUALIFIED_CHARGE) / SUM(A.PRODUCT_CHARGE), 2)) QUALIFIED_RATE_1,"
						// " DECODE(SUM(B.PRODUCT_CHARGE),0,0,ROUND(100.00 * SUM(B.QUALIFIED_CHARGE) / SUM(B.PRODUCT_CHARGE), 2)) QUALIFIED_RATE_2"
						// " FROM TFOSMT05C A"
						// " LEFT JOIN TFOSMT05C B ON A.DATE_TIME = B.DATE_TIME AND B.CC_MACH_NO = '4' AND B.FACTORY_DIV = 'A20'"
						// " WHERE A.FACTORY_DIV = 'A20'"
						// " AND A.CC_MACH_NO = '3'"
						// " AND A.DATE_TIME > TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 7 DAY,'YYYYMMDD')"
						// " AND A.DATE_TIME <= @DATE_TIME"
						// " GROUP BY A.DATE_TIME"
						// ;
// DM8 SQL：
					sqlstr =
						" SELECT A.DATE_TIME,"
						" DECODE(SUM(A.PRODUCT_CHARGE),0,0,ROUND(100.00 * SUM(A.QUALIFIED_CHARGE) / SUM(A.PRODUCT_CHARGE), 2)) QUALIFIED_RATE_1,"
						" DECODE(SUM(B.PRODUCT_CHARGE),0,0,ROUND(100.00 * SUM(B.QUALIFIED_CHARGE) / SUM(B.PRODUCT_CHARGE), 2)) QUALIFIED_RATE_2"
						" FROM TFOSMT05C A"
						" LEFT JOIN TFOSMT05C B ON A.DATE_TIME = B.DATE_TIME AND B.CC_MACH_NO = '4' AND B.FACTORY_DIV = 'A20'"
						" WHERE A.FACTORY_DIV = 'A20'"
						" AND A.CC_MACH_NO = '3'"
						" AND A.DATE_TIME > TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 7,'YYYYMMDD')"
						" AND A.DATE_TIME <= @DATE_TIME"
						" GROUP BY A.DATE_TIME"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();
			}
			else if (tbl == "TFOSMT05G")
			{
				//3#CC、4#CC月度符合率（分班组）
				bcls_ret->Tables.Add(tbl);
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-33：取查询日期前推1日的日期字符串;日历日边界、参数与结果列保持不变。
// 改写原因：DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " SELECT SHIFT_GROUP, SUM(PRODUCT_CHARGE), SUM(QUALIFIED_CHARGE),"
						// " DECODE(SUM(PRODUCT_CHARGE),0,0,ROUND(100.00 * SUM(QUALIFIED_CHARGE) / SUM(PRODUCT_CHARGE), 2)) QUALIFIED_RATE"
						// " FROM TFOSMT05C"
						// " WHERE FACTORY_DIV = 'A20'"
						// " AND SUBSTR(DATE_TIME, 1, 6) = SUBSTR(TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS,'YYYYMMDD'), 1, 6)"
						// " GROUP BY SHIFT_GROUP"
						// " ORDER BY SHIFT_GROUP"
						// ;
// DM8 SQL：
					sqlstr =
						" SELECT SHIFT_GROUP, SUM(PRODUCT_CHARGE), SUM(QUALIFIED_CHARGE),"
						" DECODE(SUM(PRODUCT_CHARGE),0,0,ROUND(100.00 * SUM(QUALIFIED_CHARGE) / SUM(PRODUCT_CHARGE), 2)) QUALIFIED_RATE"
						" FROM TFOSMT05C"
						" WHERE FACTORY_DIV = 'A20'"
						" AND SUBSTR(DATE_TIME, 1, 6) = SUBSTR(TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMMDD'), 1, 6)"
						" GROUP BY SHIFT_GROUP"
						" ORDER BY SHIFT_GROUP"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();
			}
			else if (tbl == "TFOSMT05H")
			{
				//改判情况
				bcls_ret->Tables.Add(tbl);
				CDateTime dt = CDateTime::Parse(date_time);
				if (dt.DayOfWeek() == 1)
				{
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
// DM8 适配 CHANGE-34：取查询日期前推4日的日期字符串;日历日边界、参数与结果列保持不变。
// 改写原因：DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
						// sqlstr =
							// " SELECT *"
							// " FROM TFOSMT05D"
							// " WHERE 1 = 1"
							// " AND DATE_TIME > TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 4 DAY,'YYYYMMDD')"
							// " AND DATE_TIME <= @DATE_TIME"
							// " ORDER BY DATE_TIME, FACTORY_DIV, PONO"
							// ;
// DM8 SQL：
						sqlstr =
							" SELECT *"
							" FROM TFOSMT05D"
							" WHERE 1 = 1"
							" AND DATE_TIME > TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 4,'YYYYMMDD')"
							" AND DATE_TIME <= @DATE_TIME"
							" ORDER BY DATE_TIME, FACTORY_DIV, PONO"
							;
						break;
					}
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
// DM8 适配 CHANGE-35：取查询日期前推2日的日期字符串;日历日边界、参数与结果列保持不变。
// 改写原因：DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
						// sqlstr =
							// " SELECT *"
							// " FROM TFOSMT05D"
							// " WHERE 1 = 1"
							// " AND DATE_TIME > TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 2 DAY,'YYYYMMDD')"
							// " AND DATE_TIME <= @DATE_TIME"
							// " ORDER BY DATE_TIME, FACTORY_DIV, PONO"
							// ;
// DM8 SQL：
						sqlstr =
							" SELECT *"
							" FROM TFOSMT05D"
							" WHERE 1 = 1"
							" AND DATE_TIME > TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 2,'YYYYMMDD')"
							" AND DATE_TIME <= @DATE_TIME"
							" ORDER BY DATE_TIME, FACTORY_DIV, PONO"
							;
						break;
					}
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();
			}
			else if (tbl == "TFOSMT06")
			{
				//炼钢废次降
				bcls_ret->Tables.Add(tbl);
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-36：取查询日期前推1日的日期字符串;日历日边界、参数与结果列保持不变。
// 改写原因：DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " WITH T AS ("
						// " SELECT A.FACTORY_DIV, A.ITEM_ENAME, A.ITEM_CNAME, A.REMARK RATE_TARGET,"
						// " TO_CHAR(ROW_NUMBER() OVER (PARTITION BY FACTORY_DIV ORDER BY SEQ_NO)) DEP_NAME, A.SEQ_NO"
						// " FROM TFOSMT96B A"
						// " WHERE A.TABLE_NAME = 'FOSMT06-1'"
						// " AND A.DATE_TIME = SUBSTR(@DATE_TIME,1,4)"
						// " ORDER BY A.SEQ_NO),"
						// " S AS ("
						// " SELECT FACTORY_DIV, DEP_NAME, SUM(SLAB_WT_1) SLAB_WT_1, SUM(SLAB_WT_2) SLAB_WT_2"
						// " FROM TFOSMT06A"
						// " WHERE DATE_TIME >= SUBSTR(TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS,'YYYYMMDD'),1,6) || '01'"
						// " AND DATE_TIME <= TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS,'YYYYMMDD')"
						// " GROUP BY FACTORY_DIV, DEP_NAME)"
						// " SELECT T.*,"
						// " B1.RATE RATE_1,B2.RATE RATE_2,B3.RATE RATE_3,B4.RATE RATE_4,B5.RATE RATE_5, B5.LACK_RATE RATE_AVG"
						// " FROM T"
						// " LEFT JOIN TFOSMT06A B1 ON T.FACTORY_DIV = B1.FACTORY_DIV AND T.DEP_NAME = B1.DEP_NAME AND B1.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 5 DAY,'YYYYMMDD')"
						// " LEFT JOIN TFOSMT06A B2 ON T.FACTORY_DIV = B2.FACTORY_DIV AND T.DEP_NAME = B2.DEP_NAME AND B2.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 4 DAY,'YYYYMMDD')"
						// " LEFT JOIN TFOSMT06A B3 ON T.FACTORY_DIV = B3.FACTORY_DIV AND T.DEP_NAME = B3.DEP_NAME AND B3.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 3 DAY,'YYYYMMDD')"
						// " LEFT JOIN TFOSMT06A B4 ON T.FACTORY_DIV = B4.FACTORY_DIV AND T.DEP_NAME = B4.DEP_NAME AND B4.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 2 DAY,'YYYYMMDD')"
						// " LEFT JOIN TFOSMT06A B5 ON T.FACTORY_DIV = B5.FACTORY_DIV AND T.DEP_NAME = B5.DEP_NAME AND B5.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY,'YYYYMMDD')"
						// " LEFT JOIN S ON T.FACTORY_DIV = S.FACTORY_DIV AND B1.DEP_NAME = S.DEP_NAME"
						// " ORDER BY T.SEQ_NO"
						// ;
// DM8 SQL：
					sqlstr =
						" WITH T AS ("
						" SELECT A.FACTORY_DIV, A.ITEM_ENAME, A.ITEM_CNAME, A.REMARK RATE_TARGET,"
						" TO_CHAR(ROW_NUMBER() OVER (PARTITION BY FACTORY_DIV ORDER BY SEQ_NO)) DEP_NAME, A.SEQ_NO"
						" FROM TFOSMT96B A"
						" WHERE A.TABLE_NAME = 'FOSMT06-1'"
						" AND A.DATE_TIME = SUBSTR(@DATE_TIME,1,4)"
						" ORDER BY A.SEQ_NO),"
						" S AS ("
						" SELECT FACTORY_DIV, DEP_NAME, SUM(SLAB_WT_1) SLAB_WT_1, SUM(SLAB_WT_2) SLAB_WT_2"
						" FROM TFOSMT06A"
						" WHERE DATE_TIME >= SUBSTR(TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMMDD'),1,6) || '01'"
						" AND DATE_TIME <= TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMMDD')"
						" GROUP BY FACTORY_DIV, DEP_NAME)"
						" SELECT T.*,"
						" B1.RATE RATE_1,B2.RATE RATE_2,B3.RATE RATE_3,B4.RATE RATE_4,B5.RATE RATE_5, B5.LACK_RATE RATE_AVG"
						" FROM T"
						" LEFT JOIN TFOSMT06A B1 ON T.FACTORY_DIV = B1.FACTORY_DIV AND T.DEP_NAME = B1.DEP_NAME AND B1.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 5,'YYYYMMDD')"
						" LEFT JOIN TFOSMT06A B2 ON T.FACTORY_DIV = B2.FACTORY_DIV AND T.DEP_NAME = B2.DEP_NAME AND B2.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 4,'YYYYMMDD')"
						" LEFT JOIN TFOSMT06A B3 ON T.FACTORY_DIV = B3.FACTORY_DIV AND T.DEP_NAME = B3.DEP_NAME AND B3.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 3,'YYYYMMDD')"
						" LEFT JOIN TFOSMT06A B4 ON T.FACTORY_DIV = B4.FACTORY_DIV AND T.DEP_NAME = B4.DEP_NAME AND B4.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 2,'YYYYMMDD')"
						" LEFT JOIN TFOSMT06A B5 ON T.FACTORY_DIV = B5.FACTORY_DIV AND T.DEP_NAME = B5.DEP_NAME AND B5.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMMDD')"
						" LEFT JOIN S ON T.FACTORY_DIV = S.FACTORY_DIV AND B1.DEP_NAME = S.DEP_NAME"
						" ORDER BY T.SEQ_NO"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();
			}
			else if (tbl == "TFOSMT07A")
			{
				//资源综合回收利用一
				bcls_ret->Tables.Add(tbl);
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-37：生成查询日期之前3天的日期序列;日期边界、参数与结果列保持不变。
// 改写原因：SYSIBM 辅助表改为 DUAL；DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " WITH A(LEVEL, TIME) AS"
						// " (SELECT 1, TO_DATE(@DATE_TIME,'YYYYMMDD') - 3 DAY"
						// " FROM SYSIBM.SYSDUMMY1"
						// " WHERE 1 = 1"
						// " UNION ALL"
						// " SELECT LEVEL + 1,TIME + 1 DAY"
						// " FROM SYSIBM.SYSDUMMY1, A"
						// " WHERE A.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY),"
						// " C AS ("
						// " SELECT MAT_CODE, DEVO_WT DEVO_WT"
						// " FROM TFOSMT07A"
						// " WHERE DATE_TIME >= TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY,'YYYYMM') || '01' AND DATE_TIME < @DATE_TIME"
						// " )"
						// " SELECT TO_CHAR(A.TIME,'YYYYMMDD') DATE_TIME,"
						// " COALESCE(B1.DEVO_WT,0) DEVO_WT_1, COALESCE(B2.DEVO_WT,0) DEVO_WT_2, COALESCE(B3.DEVO_WT,0) DEVO_WT_3,"
						// " COALESCE(B1.DEVO_WT,0) + COALESCE(B2.DEVO_WT,0) + COALESCE(B3.DEVO_WT,0) DEVO_WT_4,"
						// " COALESCE(B4.DEVO_WT,0) DEVO_WT_5, COALESCE(B5.DEVO_WT,0) DEVO_WT_6,"
						// " COALESCE(B8.DEVO_WT,0) DEVO_WT_7, COALESCE(B9.DEVO_WT,0) DEVO_WT_8,"
						// " COALESCE(B4.DEVO_WT,0) + COALESCE(B5.DEVO_WT,0) + COALESCE(B8.DEVO_WT,0) + COALESCE(B9.DEVO_WT,0) DEVO_WT_9,"
						// " COALESCE(B6.DEVO_WT,0) DEVO_WT_10, COALESCE(B7.DEVO_WT,0) DEVO_WT_11,"
						// " COALESCE(B6.DEVO_WT,0) + COALESCE(B7.DEVO_WT,0) DEVO_WT_12"
						// " FROM A"
						// " LEFT JOIN TFOSMT07A B1 ON TO_CHAR(A.TIME,'YYYYMMDD') = B1.DATE_TIME AND B1.MAT_CODE = '01'"
						// " LEFT JOIN TFOSMT07A B2 ON TO_CHAR(A.TIME,'YYYYMMDD') = B2.DATE_TIME AND B2.MAT_CODE = '02'"
						// " LEFT JOIN TFOSMT07A B3 ON TO_CHAR(A.TIME,'YYYYMMDD') = B3.DATE_TIME AND B3.MAT_CODE = '03'"
						// " LEFT JOIN TFOSMT07A B4 ON TO_CHAR(A.TIME,'YYYYMMDD') = B4.DATE_TIME AND B4.MAT_CODE = '05'"
						// " LEFT JOIN TFOSMT07A B5 ON TO_CHAR(A.TIME,'YYYYMMDD') = B5.DATE_TIME AND B5.MAT_CODE = '06'"
						// " LEFT JOIN TFOSMT07A B6 ON TO_CHAR(A.TIME,'YYYYMMDD') = B6.DATE_TIME AND B6.MAT_CODE = '08'"
						// " LEFT JOIN TFOSMT07A B7 ON TO_CHAR(A.TIME,'YYYYMMDD') = B7.DATE_TIME AND B7.MAT_CODE = '09'"
						// " LEFT JOIN TFOSMT07A B8 ON TO_CHAR(A.TIME,'YYYYMMDD') = B8.DATE_TIME AND B8.MAT_CODE = '25'"
						// " LEFT JOIN TFOSMT07A B9 ON TO_CHAR(A.TIME,'YYYYMMDD') = B9.DATE_TIME AND B9.MAT_CODE = '26'"
						// " UNION ALL"
						// " SELECT '合计',"
						// " SUM(CASE WHEN C.MAT_CODE = '01' THEN C.DEVO_WT ELSE 0 END),"
						// " SUM(CASE WHEN C.MAT_CODE = '02' THEN C.DEVO_WT ELSE 0 END),"
						// " SUM(CASE WHEN C.MAT_CODE = '03' THEN C.DEVO_WT ELSE 0 END),"
						// " SUM(CASE WHEN C.MAT_CODE IN ('01','02','03') THEN C.DEVO_WT ELSE 0 END),"
						// " SUM(CASE WHEN C.MAT_CODE = '05' THEN C.DEVO_WT ELSE 0 END),"
						// " SUM(CASE WHEN C.MAT_CODE = '06' THEN C.DEVO_WT ELSE 0 END),"
						// " SUM(CASE WHEN C.MAT_CODE = '25' THEN C.DEVO_WT ELSE 0 END),"
						// " SUM(CASE WHEN C.MAT_CODE = '26' THEN C.DEVO_WT ELSE 0 END),"
						// " SUM(CASE WHEN C.MAT_CODE IN ('05','06','25','26') THEN C.DEVO_WT ELSE 0 END),"
						// " SUM(CASE WHEN C.MAT_CODE = '08' THEN C.DEVO_WT ELSE 0 END),"
						// " SUM(CASE WHEN C.MAT_CODE = '09' THEN C.DEVO_WT ELSE 0 END),"
						// " SUM(CASE WHEN C.MAT_CODE IN ('08','09') THEN C.DEVO_WT ELSE 0 END)"
						// " FROM C"
						// " ORDER BY DATE_TIME"
						// ;
// DM8 SQL：
					sqlstr =
						" WITH A(LEVEL, TIME) AS"
						" (SELECT 1, TO_DATE(@DATE_TIME,'YYYYMMDD') - 3"
						" FROM DUAL"
						" WHERE 1 = 1"
						" UNION ALL"
						" SELECT LEVEL + 1,TIME + 1"
						" FROM DUAL, A"
						" WHERE A.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD') - 1),"
						" C AS ("
						" SELECT MAT_CODE, DEVO_WT DEVO_WT"
						" FROM TFOSMT07A"
						" WHERE DATE_TIME >= TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMM') || '01' AND DATE_TIME < @DATE_TIME"
						" )"
						" SELECT TO_CHAR(A.TIME,'YYYYMMDD') DATE_TIME,"
						" COALESCE(B1.DEVO_WT,0) DEVO_WT_1, COALESCE(B2.DEVO_WT,0) DEVO_WT_2, COALESCE(B3.DEVO_WT,0) DEVO_WT_3,"
						" COALESCE(B1.DEVO_WT,0) + COALESCE(B2.DEVO_WT,0) + COALESCE(B3.DEVO_WT,0) DEVO_WT_4,"
						" COALESCE(B4.DEVO_WT,0) DEVO_WT_5, COALESCE(B5.DEVO_WT,0) DEVO_WT_6,"
						" COALESCE(B8.DEVO_WT,0) DEVO_WT_7, COALESCE(B9.DEVO_WT,0) DEVO_WT_8,"
						" COALESCE(B4.DEVO_WT,0) + COALESCE(B5.DEVO_WT,0) + COALESCE(B8.DEVO_WT,0) + COALESCE(B9.DEVO_WT,0) DEVO_WT_9,"
						" COALESCE(B6.DEVO_WT,0) DEVO_WT_10, COALESCE(B7.DEVO_WT,0) DEVO_WT_11,"
						" COALESCE(B6.DEVO_WT,0) + COALESCE(B7.DEVO_WT,0) DEVO_WT_12"
						" FROM A"
						" LEFT JOIN TFOSMT07A B1 ON TO_CHAR(A.TIME,'YYYYMMDD') = B1.DATE_TIME AND B1.MAT_CODE = '01'"
						" LEFT JOIN TFOSMT07A B2 ON TO_CHAR(A.TIME,'YYYYMMDD') = B2.DATE_TIME AND B2.MAT_CODE = '02'"
						" LEFT JOIN TFOSMT07A B3 ON TO_CHAR(A.TIME,'YYYYMMDD') = B3.DATE_TIME AND B3.MAT_CODE = '03'"
						" LEFT JOIN TFOSMT07A B4 ON TO_CHAR(A.TIME,'YYYYMMDD') = B4.DATE_TIME AND B4.MAT_CODE = '05'"
						" LEFT JOIN TFOSMT07A B5 ON TO_CHAR(A.TIME,'YYYYMMDD') = B5.DATE_TIME AND B5.MAT_CODE = '06'"
						" LEFT JOIN TFOSMT07A B6 ON TO_CHAR(A.TIME,'YYYYMMDD') = B6.DATE_TIME AND B6.MAT_CODE = '08'"
						" LEFT JOIN TFOSMT07A B7 ON TO_CHAR(A.TIME,'YYYYMMDD') = B7.DATE_TIME AND B7.MAT_CODE = '09'"
						" LEFT JOIN TFOSMT07A B8 ON TO_CHAR(A.TIME,'YYYYMMDD') = B8.DATE_TIME AND B8.MAT_CODE = '25'"
						" LEFT JOIN TFOSMT07A B9 ON TO_CHAR(A.TIME,'YYYYMMDD') = B9.DATE_TIME AND B9.MAT_CODE = '26'"
						" UNION ALL"
						" SELECT '合计',"
						" SUM(CASE WHEN C.MAT_CODE = '01' THEN C.DEVO_WT ELSE 0 END),"
						" SUM(CASE WHEN C.MAT_CODE = '02' THEN C.DEVO_WT ELSE 0 END),"
						" SUM(CASE WHEN C.MAT_CODE = '03' THEN C.DEVO_WT ELSE 0 END),"
						" SUM(CASE WHEN C.MAT_CODE IN ('01','02','03') THEN C.DEVO_WT ELSE 0 END),"
						" SUM(CASE WHEN C.MAT_CODE = '05' THEN C.DEVO_WT ELSE 0 END),"
						" SUM(CASE WHEN C.MAT_CODE = '06' THEN C.DEVO_WT ELSE 0 END),"
						" SUM(CASE WHEN C.MAT_CODE = '25' THEN C.DEVO_WT ELSE 0 END),"
						" SUM(CASE WHEN C.MAT_CODE = '26' THEN C.DEVO_WT ELSE 0 END),"
						" SUM(CASE WHEN C.MAT_CODE IN ('05','06','25','26') THEN C.DEVO_WT ELSE 0 END),"
						" SUM(CASE WHEN C.MAT_CODE = '08' THEN C.DEVO_WT ELSE 0 END),"
						" SUM(CASE WHEN C.MAT_CODE = '09' THEN C.DEVO_WT ELSE 0 END),"
						" SUM(CASE WHEN C.MAT_CODE IN ('08','09') THEN C.DEVO_WT ELSE 0 END)"
						" FROM C"
						" ORDER BY DATE_TIME"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();

				//处理去年月均实绩
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-38：取查询日期前推1日的日期字符串;日历日边界、参数与结果列保持不变。
// 改写原因：DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " SELECT '去年月均实绩' DATE_TIME,"
						// " MAX(CASE WHEN ITEM_ENAME = 'FOSMT07-101' THEN TO_NUMBER(REMARK) END) DEVO_WT_1,"
						// " MAX(CASE WHEN ITEM_ENAME = 'FOSMT07-102' THEN TO_NUMBER(REMARK) END) DEVO_WT_2,"
						// " MAX(CASE WHEN ITEM_ENAME = 'FOSMT07-103' THEN TO_NUMBER(REMARK) END) DEVO_WT_3,"
						// " SUM(CASE WHEN ITEM_ENAME IN ('FOSMT07-101','FOSMT07-102','FOSMT07-103') THEN TO_NUMBER(REMARK) END) DEVO_WT_4,"
						// " MAX(CASE WHEN ITEM_ENAME = 'FOSMT07-104' THEN TO_NUMBER(REMARK) END) DEVO_WT_5,"
						// " MAX(CASE WHEN ITEM_ENAME = 'FOSMT07-105' THEN TO_NUMBER(REMARK) END) DEVO_WT_6,"
						// " MAX(CASE WHEN ITEM_ENAME = 'FOSMT07-106' THEN TO_NUMBER(REMARK) END) DEVO_WT_7,"
						// " MAX(CASE WHEN ITEM_ENAME = 'FOSMT07-107' THEN TO_NUMBER(REMARK) END) DEVO_WT_8,"
						// " SUM(CASE WHEN ITEM_ENAME IN ('FOSMT07-104','FOSMT07-105','FOSMT07-106','FOSMT07-107') THEN TO_NUMBER(REMARK) END) DEVO_WT_9,"
						// " MAX(CASE WHEN ITEM_ENAME = 'FOSMT07-108' THEN TO_NUMBER(REMARK) END) DEVO_WT_10,"
						// " MAX(CASE WHEN ITEM_ENAME = 'FOSMT07-109' THEN TO_NUMBER(REMARK) END) DEVO_WT_11,"
						// " SUM(CASE WHEN ITEM_ENAME IN ('FOSMT07-108','FOSMT07-109') THEN TO_NUMBER(REMARK) END) DEVO_WT_12"
						// " FROM TFOSMT96B"
						// " WHERE DATE_TIME = SUBSTR(TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS,'YYYYMMDD'),1,4) AND TABLE_NAME = 'FOSMT07-1'"
						// " GROUP BY DATE_TIME"
						// ;
// DM8 SQL：
					sqlstr =
						" SELECT '去年月均实绩' DATE_TIME,"
						" MAX(CASE WHEN ITEM_ENAME = 'FOSMT07-101' THEN TO_NUMBER(REMARK) END) DEVO_WT_1,"
						" MAX(CASE WHEN ITEM_ENAME = 'FOSMT07-102' THEN TO_NUMBER(REMARK) END) DEVO_WT_2,"
						" MAX(CASE WHEN ITEM_ENAME = 'FOSMT07-103' THEN TO_NUMBER(REMARK) END) DEVO_WT_3,"
						" SUM(CASE WHEN ITEM_ENAME IN ('FOSMT07-101','FOSMT07-102','FOSMT07-103') THEN TO_NUMBER(REMARK) END) DEVO_WT_4,"
						" MAX(CASE WHEN ITEM_ENAME = 'FOSMT07-104' THEN TO_NUMBER(REMARK) END) DEVO_WT_5,"
						" MAX(CASE WHEN ITEM_ENAME = 'FOSMT07-105' THEN TO_NUMBER(REMARK) END) DEVO_WT_6,"
						" MAX(CASE WHEN ITEM_ENAME = 'FOSMT07-106' THEN TO_NUMBER(REMARK) END) DEVO_WT_7,"
						" MAX(CASE WHEN ITEM_ENAME = 'FOSMT07-107' THEN TO_NUMBER(REMARK) END) DEVO_WT_8,"
						" SUM(CASE WHEN ITEM_ENAME IN ('FOSMT07-104','FOSMT07-105','FOSMT07-106','FOSMT07-107') THEN TO_NUMBER(REMARK) END) DEVO_WT_9,"
						" MAX(CASE WHEN ITEM_ENAME = 'FOSMT07-108' THEN TO_NUMBER(REMARK) END) DEVO_WT_10,"
						" MAX(CASE WHEN ITEM_ENAME = 'FOSMT07-109' THEN TO_NUMBER(REMARK) END) DEVO_WT_11,"
						" SUM(CASE WHEN ITEM_ENAME IN ('FOSMT07-108','FOSMT07-109') THEN TO_NUMBER(REMARK) END) DEVO_WT_12"
						" FROM TFOSMT96B"
						" WHERE DATE_TIME = SUBSTR(TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMMDD'),1,4) AND TABLE_NAME = 'FOSMT07-1'"
						" GROUP BY DATE_TIME"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				EIClass blk;
				cmd_inq.ExecuteQuery(blk.Tables[0]);
				cmd_inq.Close();

				CDataRow& row = bcls_ret->Tables[tbl].Rows.Add();
				row.Merge(blk.Tables[0].Rows[0]);
			}
			else if (tbl == "TFOSMT07B")
			{
				//资源综合回收利用二
				bcls_ret->Tables.Add(tbl);
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-39：生成查询日期之前3天的日期序列;日期边界、参数与结果列保持不变。
// 改写原因：SYSIBM 辅助表改为 DUAL；DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " WITH A(LEVEL, TIME) AS"
						// " (SELECT 1, TO_DATE(@DATE_TIME,'YYYYMMDD') - 3 DAY"
						// " FROM SYSIBM.SYSDUMMY1"
						// " WHERE 1 = 1"
						// " UNION ALL"
						// " SELECT LEVEL + 1,TIME + 1 DAY"
						// " FROM SYSIBM.SYSDUMMY1, A"
						// " WHERE A.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY),"
						// " C AS ("
						// " SELECT MAT_CODE, DEVO_WT DEVO_WT"
						// " FROM TFOSMT07A"
						// " WHERE DATE_TIME >= TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY,'YYYYMM') || '01' AND DATE_TIME < @DATE_TIME"
						// " )"
						// " SELECT TO_CHAR(A.TIME,'YYYYMMDD') DATE_TIME,"
						// " COALESCE(B1.DEVO_WT,0) DEVO_WT_1, COALESCE(B2.DEVO_WT,0) DEVO_WT_2, COALESCE(B3.DEVO_WT,0) DEVO_WT_3,"
						// " COALESCE(B1.DEVO_WT,0) + COALESCE(B2.DEVO_WT,0) + COALESCE(B3.DEVO_WT,0) DEVO_WT_4,"
						// " COALESCE(B4.DEVO_WT,0) DEVO_WT_5, COALESCE(B5.DEVO_WT,0) DEVO_WT_6,COALESCE(B6.DEVO_WT,0) DEVO_WT_7,"
						// " COALESCE(B4.DEVO_WT,0) + COALESCE(B5.DEVO_WT,0) + COALESCE(B6.DEVO_WT,0) DEVO_WT_8,"
						// " COALESCE(B7.DEVO_WT,0) DEVO_WT_9, COALESCE(B8.DEVO_WT,0) DEVO_WT_10,"
						// " COALESCE(B7.DEVO_WT,0) + COALESCE(B8.DEVO_WT,0) DEVO_WT_11,"
						// " COALESCE(B9.DEVO_WT,0) DEVO_WT_12, COALESCE(BA.DEVO_WT,0) DEVO_WT_13,"
						// " COALESCE(B9.DEVO_WT,0) + COALESCE(BA.DEVO_WT,0) DEVO_WT_14"
						// " FROM A"
						// " LEFT JOIN TFOSMT07A B1 ON TO_CHAR(A.TIME,'YYYYMMDD') = B1.DATE_TIME AND B1.MAT_CODE = '11'"
						// " LEFT JOIN TFOSMT07A B2 ON TO_CHAR(A.TIME,'YYYYMMDD') = B2.DATE_TIME AND B2.MAT_CODE = '12'"
						// " LEFT JOIN TFOSMT07A B3 ON TO_CHAR(A.TIME,'YYYYMMDD') = B3.DATE_TIME AND B3.MAT_CODE = '13'"
						// " LEFT JOIN TFOSMT07A B4 ON TO_CHAR(A.TIME,'YYYYMMDD') = B4.DATE_TIME AND B4.MAT_CODE = '15'"
						// " LEFT JOIN TFOSMT07A B5 ON TO_CHAR(A.TIME,'YYYYMMDD') = B5.DATE_TIME AND B5.MAT_CODE = '16'"
						// " LEFT JOIN TFOSMT07A B6 ON TO_CHAR(A.TIME,'YYYYMMDD') = B6.DATE_TIME AND B6.MAT_CODE = '17'"
						// " LEFT JOIN TFOSMT07A B7 ON TO_CHAR(A.TIME,'YYYYMMDD') = B7.DATE_TIME AND B7.MAT_CODE = '19'"
						// " LEFT JOIN TFOSMT07A B8 ON TO_CHAR(A.TIME,'YYYYMMDD') = B8.DATE_TIME AND B8.MAT_CODE = '20'"
						// " LEFT JOIN TFOSMT07A B9 ON TO_CHAR(A.TIME,'YYYYMMDD') = B9.DATE_TIME AND B9.MAT_CODE = '22'"
						// " LEFT JOIN TFOSMT07A BA ON TO_CHAR(A.TIME,'YYYYMMDD') = BA.DATE_TIME AND BA.MAT_CODE = '23'"
						// " UNION ALL"
						// " SELECT '合计',"
						// " SUM(CASE WHEN C.MAT_CODE = '11' THEN C.DEVO_WT ELSE 0 END),"
						// " SUM(CASE WHEN C.MAT_CODE = '12' THEN C.DEVO_WT ELSE 0 END),"
						// " SUM(CASE WHEN C.MAT_CODE = '13' THEN C.DEVO_WT ELSE 0 END),"
						// " SUM(CASE WHEN C.MAT_CODE IN ('11','12','13') THEN C.DEVO_WT ELSE 0 END),"
						// " SUM(CASE WHEN C.MAT_CODE = '15' THEN C.DEVO_WT ELSE 0 END),"
						// " SUM(CASE WHEN C.MAT_CODE = '16' THEN C.DEVO_WT ELSE 0 END),"
						// " SUM(CASE WHEN C.MAT_CODE = '17' THEN C.DEVO_WT ELSE 0 END),"
						// " SUM(CASE WHEN C.MAT_CODE IN ('15','16','17') THEN C.DEVO_WT ELSE 0 END),"
						// " SUM(CASE WHEN C.MAT_CODE = '19' THEN C.DEVO_WT ELSE 0 END),"
						// " SUM(CASE WHEN C.MAT_CODE = '20' THEN C.DEVO_WT ELSE 0 END),"
						// " SUM(CASE WHEN C.MAT_CODE IN ('19','20') THEN C.DEVO_WT ELSE 0 END),"
						// " SUM(CASE WHEN C.MAT_CODE = '22' THEN C.DEVO_WT ELSE 0 END),"
						// " SUM(CASE WHEN C.MAT_CODE = '23' THEN C.DEVO_WT ELSE 0 END),"
						// " SUM(CASE WHEN C.MAT_CODE IN ('22','23') THEN C.DEVO_WT ELSE 0 END)"
						// " FROM C"
						// " ORDER BY DATE_TIME"
						// ;
// DM8 SQL：
					sqlstr =
						" WITH A(LEVEL, TIME) AS"
						" (SELECT 1, TO_DATE(@DATE_TIME,'YYYYMMDD') - 3"
						" FROM DUAL"
						" WHERE 1 = 1"
						" UNION ALL"
						" SELECT LEVEL + 1,TIME + 1"
						" FROM DUAL, A"
						" WHERE A.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD') - 1),"
						" C AS ("
						" SELECT MAT_CODE, DEVO_WT DEVO_WT"
						" FROM TFOSMT07A"
						" WHERE DATE_TIME >= TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMM') || '01' AND DATE_TIME < @DATE_TIME"
						" )"
						" SELECT TO_CHAR(A.TIME,'YYYYMMDD') DATE_TIME,"
						" COALESCE(B1.DEVO_WT,0) DEVO_WT_1, COALESCE(B2.DEVO_WT,0) DEVO_WT_2, COALESCE(B3.DEVO_WT,0) DEVO_WT_3,"
						" COALESCE(B1.DEVO_WT,0) + COALESCE(B2.DEVO_WT,0) + COALESCE(B3.DEVO_WT,0) DEVO_WT_4,"
						" COALESCE(B4.DEVO_WT,0) DEVO_WT_5, COALESCE(B5.DEVO_WT,0) DEVO_WT_6,COALESCE(B6.DEVO_WT,0) DEVO_WT_7,"
						" COALESCE(B4.DEVO_WT,0) + COALESCE(B5.DEVO_WT,0) + COALESCE(B6.DEVO_WT,0) DEVO_WT_8,"
						" COALESCE(B7.DEVO_WT,0) DEVO_WT_9, COALESCE(B8.DEVO_WT,0) DEVO_WT_10,"
						" COALESCE(B7.DEVO_WT,0) + COALESCE(B8.DEVO_WT,0) DEVO_WT_11,"
						" COALESCE(B9.DEVO_WT,0) DEVO_WT_12, COALESCE(BA.DEVO_WT,0) DEVO_WT_13,"
						" COALESCE(B9.DEVO_WT,0) + COALESCE(BA.DEVO_WT,0) DEVO_WT_14"
						" FROM A"
						" LEFT JOIN TFOSMT07A B1 ON TO_CHAR(A.TIME,'YYYYMMDD') = B1.DATE_TIME AND B1.MAT_CODE = '11'"
						" LEFT JOIN TFOSMT07A B2 ON TO_CHAR(A.TIME,'YYYYMMDD') = B2.DATE_TIME AND B2.MAT_CODE = '12'"
						" LEFT JOIN TFOSMT07A B3 ON TO_CHAR(A.TIME,'YYYYMMDD') = B3.DATE_TIME AND B3.MAT_CODE = '13'"
						" LEFT JOIN TFOSMT07A B4 ON TO_CHAR(A.TIME,'YYYYMMDD') = B4.DATE_TIME AND B4.MAT_CODE = '15'"
						" LEFT JOIN TFOSMT07A B5 ON TO_CHAR(A.TIME,'YYYYMMDD') = B5.DATE_TIME AND B5.MAT_CODE = '16'"
						" LEFT JOIN TFOSMT07A B6 ON TO_CHAR(A.TIME,'YYYYMMDD') = B6.DATE_TIME AND B6.MAT_CODE = '17'"
						" LEFT JOIN TFOSMT07A B7 ON TO_CHAR(A.TIME,'YYYYMMDD') = B7.DATE_TIME AND B7.MAT_CODE = '19'"
						" LEFT JOIN TFOSMT07A B8 ON TO_CHAR(A.TIME,'YYYYMMDD') = B8.DATE_TIME AND B8.MAT_CODE = '20'"
						" LEFT JOIN TFOSMT07A B9 ON TO_CHAR(A.TIME,'YYYYMMDD') = B9.DATE_TIME AND B9.MAT_CODE = '22'"
						" LEFT JOIN TFOSMT07A BA ON TO_CHAR(A.TIME,'YYYYMMDD') = BA.DATE_TIME AND BA.MAT_CODE = '23'"
						" UNION ALL"
						" SELECT '合计',"
						" SUM(CASE WHEN C.MAT_CODE = '11' THEN C.DEVO_WT ELSE 0 END),"
						" SUM(CASE WHEN C.MAT_CODE = '12' THEN C.DEVO_WT ELSE 0 END),"
						" SUM(CASE WHEN C.MAT_CODE = '13' THEN C.DEVO_WT ELSE 0 END),"
						" SUM(CASE WHEN C.MAT_CODE IN ('11','12','13') THEN C.DEVO_WT ELSE 0 END),"
						" SUM(CASE WHEN C.MAT_CODE = '15' THEN C.DEVO_WT ELSE 0 END),"
						" SUM(CASE WHEN C.MAT_CODE = '16' THEN C.DEVO_WT ELSE 0 END),"
						" SUM(CASE WHEN C.MAT_CODE = '17' THEN C.DEVO_WT ELSE 0 END),"
						" SUM(CASE WHEN C.MAT_CODE IN ('15','16','17') THEN C.DEVO_WT ELSE 0 END),"
						" SUM(CASE WHEN C.MAT_CODE = '19' THEN C.DEVO_WT ELSE 0 END),"
						" SUM(CASE WHEN C.MAT_CODE = '20' THEN C.DEVO_WT ELSE 0 END),"
						" SUM(CASE WHEN C.MAT_CODE IN ('19','20') THEN C.DEVO_WT ELSE 0 END),"
						" SUM(CASE WHEN C.MAT_CODE = '22' THEN C.DEVO_WT ELSE 0 END),"
						" SUM(CASE WHEN C.MAT_CODE = '23' THEN C.DEVO_WT ELSE 0 END),"
						" SUM(CASE WHEN C.MAT_CODE IN ('22','23') THEN C.DEVO_WT ELSE 0 END)"
						" FROM C"
						" ORDER BY DATE_TIME"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();

				//处理去年月均实绩
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-40：取查询日期前推1日的日期字符串;日历日边界、参数与结果列保持不变。
// 改写原因：DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " SELECT '去年月均实绩' DATE_TIME,"
						// " MAX(CASE WHEN ITEM_ENAME = 'FOSMT07-110' THEN TO_NUMBER(REMARK) END) DEVO_WT_1,"
						// " MAX(CASE WHEN ITEM_ENAME = 'FOSMT07-111' THEN TO_NUMBER(REMARK) END) DEVO_WT_2,"
						// " MAX(CASE WHEN ITEM_ENAME = 'FOSMT07-112' THEN TO_NUMBER(REMARK) END) DEVO_WT_3,"
						// " SUM(CASE WHEN ITEM_ENAME IN ('FOSMT07-110','FOSMT07-111','FOSMT07-112') THEN TO_NUMBER(REMARK) END) DEVO_WT_4,"
						// " MAX(CASE WHEN ITEM_ENAME = 'FOSMT07-113' THEN TO_NUMBER(REMARK) END) DEVO_WT_5,"
						// " MAX(CASE WHEN ITEM_ENAME = 'FOSMT07-114' THEN TO_NUMBER(REMARK) END) DEVO_WT_6,"
						// " MAX(CASE WHEN ITEM_ENAME = 'FOSMT07-115' THEN TO_NUMBER(REMARK) END) DEVO_WT_7,"
						// " SUM(CASE WHEN ITEM_ENAME IN ('FOSMT07-113','FOSMT07-114','FOSMT07-115') THEN TO_NUMBER(REMARK) END) DEVO_WT_8,"
						// " MAX(CASE WHEN ITEM_ENAME = 'FOSMT07-116' THEN TO_NUMBER(REMARK) END) DEVO_WT_9,"
						// " MAX(CASE WHEN ITEM_ENAME = 'FOSMT07-117' THEN TO_NUMBER(REMARK) END) DEVO_WT_10,"
						// " SUM(CASE WHEN ITEM_ENAME IN ('FOSMT07-116','FOSMT07-117') THEN TO_NUMBER(REMARK) END) DEVO_WT_11,"
						// " MAX(CASE WHEN ITEM_ENAME = 'FOSMT07-118' THEN TO_NUMBER(REMARK) END) DEVO_WT_12,"
						// " MAX(CASE WHEN ITEM_ENAME = 'FOSMT07-119' THEN TO_NUMBER(REMARK) END) DEVO_WT_13,"
						// " SUM(CASE WHEN ITEM_ENAME IN ('FOSMT07-118','FOSMT07-119') THEN TO_NUMBER(REMARK) END) DEVO_WT_14"
						// " FROM TFOSMT96B"
						// " WHERE DATE_TIME = SUBSTR(TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS,'YYYYMMDD'),1,4) AND TABLE_NAME = 'FOSMT07-1'"
						// " GROUP BY DATE_TIME"
						// ;
// DM8 SQL：
					sqlstr =
						" SELECT '去年月均实绩' DATE_TIME,"
						" MAX(CASE WHEN ITEM_ENAME = 'FOSMT07-110' THEN TO_NUMBER(REMARK) END) DEVO_WT_1,"
						" MAX(CASE WHEN ITEM_ENAME = 'FOSMT07-111' THEN TO_NUMBER(REMARK) END) DEVO_WT_2,"
						" MAX(CASE WHEN ITEM_ENAME = 'FOSMT07-112' THEN TO_NUMBER(REMARK) END) DEVO_WT_3,"
						" SUM(CASE WHEN ITEM_ENAME IN ('FOSMT07-110','FOSMT07-111','FOSMT07-112') THEN TO_NUMBER(REMARK) END) DEVO_WT_4,"
						" MAX(CASE WHEN ITEM_ENAME = 'FOSMT07-113' THEN TO_NUMBER(REMARK) END) DEVO_WT_5,"
						" MAX(CASE WHEN ITEM_ENAME = 'FOSMT07-114' THEN TO_NUMBER(REMARK) END) DEVO_WT_6,"
						" MAX(CASE WHEN ITEM_ENAME = 'FOSMT07-115' THEN TO_NUMBER(REMARK) END) DEVO_WT_7,"
						" SUM(CASE WHEN ITEM_ENAME IN ('FOSMT07-113','FOSMT07-114','FOSMT07-115') THEN TO_NUMBER(REMARK) END) DEVO_WT_8,"
						" MAX(CASE WHEN ITEM_ENAME = 'FOSMT07-116' THEN TO_NUMBER(REMARK) END) DEVO_WT_9,"
						" MAX(CASE WHEN ITEM_ENAME = 'FOSMT07-117' THEN TO_NUMBER(REMARK) END) DEVO_WT_10,"
						" SUM(CASE WHEN ITEM_ENAME IN ('FOSMT07-116','FOSMT07-117') THEN TO_NUMBER(REMARK) END) DEVO_WT_11,"
						" MAX(CASE WHEN ITEM_ENAME = 'FOSMT07-118' THEN TO_NUMBER(REMARK) END) DEVO_WT_12,"
						" MAX(CASE WHEN ITEM_ENAME = 'FOSMT07-119' THEN TO_NUMBER(REMARK) END) DEVO_WT_13,"
						" SUM(CASE WHEN ITEM_ENAME IN ('FOSMT07-118','FOSMT07-119') THEN TO_NUMBER(REMARK) END) DEVO_WT_14"
						" FROM TFOSMT96B"
						" WHERE DATE_TIME = SUBSTR(TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMMDD'),1,4) AND TABLE_NAME = 'FOSMT07-1'"
						" GROUP BY DATE_TIME"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				EIClass blk;
				cmd_inq.ExecuteQuery(blk.Tables[0]);
				cmd_inq.Close();

				CDataRow& row = bcls_ret->Tables[tbl].Rows.Add();
				row.Merge(blk.Tables[0].Rows[0]);
			}
			else if (tbl == "TFOSMT08")
			{
				//KPI指标
				bcls_ret->Tables.Add(tbl);
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-41：取查询日期前推1日的日期字符串;日历日边界、参数与结果列保持不变。
// 改写原因：DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " WITH T AS ("
						// " SELECT ITEM_CNAME, UNIT, PROC_DIV, SEQ_NO, ITEM_ENAME,"
						// " SUBSTR(TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS,'YYYYMMDD'), 1, 4) DATE_TIME"
						// " FROM TFOSMT96A"
						// " WHERE TABLE_NAME = 'FOSMT08-1' AND FACTORY_DIV = 'A10' AND PROC_DIV = 'Y'"
						// " ), S AS ("
						// " SELECT ITEM_ENAME, SUM(REMARK_1) REMARK_1, SUM(REMARK_2) REMARK_2"
						// " FROM TFOSMT08A"
						// " WHERE DATE_TIME >= TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS,'YYYYMM') || '01' AND DATE_TIME <= TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS,'YYYYMMDD')"
						// " GROUP BY ITEM_ENAME"
						// " )"
						// " SELECT ROW_NUMBER() OVER () SEQ_NO, T.ITEM_CNAME, T.UNIT, T.PROC_DIV, T.DATE_TIME,"
						// " TO_NUMBER(COALESCE(A.REMARK,0)) INDEX_TARGET_1, TO_NUMBER(COALESCE(B.REMARK,0)) INDEX_TARGET_2,"
						// " TO_NUMBER(COALESCE(C.INDEX_DAY,0)) INDEX_DAY_1,"
						// " CASE WHEN T.ITEM_ENAME = 'FOSMT08-127' THEN C2.REMARK_1"
						// " ELSE DECODE(C2.REMARK_2,0,0,C2.REMARK_1 / C2.REMARK_2)"
						// " END INDEX_TOL_1,"
						// " TO_NUMBER(COALESCE(D.INDEX_DAY,0)) INDEX_DAY_2,"
						// " CASE WHEN T.ITEM_ENAME = 'FOSMT08-127' THEN D2.REMARK_1"
						// " ELSE DECODE(D2.REMARK_2,0,0,D2.REMARK_1 / D2.REMARK_2)"
						// " END INDEX_TOL_2,"
						// " CASE WHEN T.ITEM_ENAME = 'FOSMT08-127' THEN C2.REMARK_1 + D2.REMARK_1"
						// " ELSE DECODE((C2.REMARK_2 + D2.REMARK_2),0,0,(C2.REMARK_1 + D2.REMARK_1) / (C2.REMARK_2 + D2.REMARK_2))"
						// " END INDEX_TOL"
						// " FROM T"
						// " LEFT JOIN TFOSMT96B A ON T.ITEM_CNAME = A.ITEM_CNAME AND A.FACTORY_DIV = 'A10' AND T.DATE_TIME = A.DATE_TIME"
						// " LEFT JOIN TFOSMT96B B ON T.ITEM_CNAME = B.ITEM_CNAME AND B.FACTORY_DIV = 'A20' AND T.DATE_TIME = B.DATE_TIME"
						// " LEFT JOIN TFOSMT08A C ON A.ITEM_ENAME = C.ITEM_ENAME AND C.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS,'YYYYMMDD')"
						// " LEFT JOIN S C2 ON A.ITEM_ENAME = C2.ITEM_ENAME"
						// " LEFT JOIN TFOSMT08A D ON B.ITEM_ENAME = D.ITEM_ENAME AND D.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS,'YYYYMMDD')"
						// " LEFT JOIN S D2 ON B.ITEM_ENAME = D2.ITEM_ENAME"
						// " ORDER BY T.SEQ_NO, T.ITEM_ENAME"
						// ;
// DM8 SQL：
					sqlstr =
						" WITH T AS ("
						" SELECT ITEM_CNAME, UNIT, PROC_DIV, SEQ_NO, ITEM_ENAME,"
						" SUBSTR(TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMMDD'), 1, 4) DATE_TIME"
						" FROM TFOSMT96A"
						" WHERE TABLE_NAME = 'FOSMT08-1' AND FACTORY_DIV = 'A10' AND PROC_DIV = 'Y'"
						" ), S AS ("
						" SELECT ITEM_ENAME, SUM(REMARK_1) REMARK_1, SUM(REMARK_2) REMARK_2"
						" FROM TFOSMT08A"
						" WHERE DATE_TIME >= TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMM') || '01' AND DATE_TIME <= TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMMDD')"
						" GROUP BY ITEM_ENAME"
						" )"
						" SELECT ROW_NUMBER() OVER () SEQ_NO, T.ITEM_CNAME, T.UNIT, T.PROC_DIV, T.DATE_TIME,"
						" TO_NUMBER(COALESCE(A.REMARK,0)) INDEX_TARGET_1, TO_NUMBER(COALESCE(B.REMARK,0)) INDEX_TARGET_2,"
						" TO_NUMBER(COALESCE(C.INDEX_DAY,0)) INDEX_DAY_1,"
						" CASE WHEN T.ITEM_ENAME = 'FOSMT08-127' THEN C2.REMARK_1"
						" ELSE DECODE(C2.REMARK_2,0,0,C2.REMARK_1 / C2.REMARK_2)"
						" END INDEX_TOL_1,"
						" TO_NUMBER(COALESCE(D.INDEX_DAY,0)) INDEX_DAY_2,"
						" CASE WHEN T.ITEM_ENAME = 'FOSMT08-127' THEN D2.REMARK_1"
						" ELSE DECODE(D2.REMARK_2,0,0,D2.REMARK_1 / D2.REMARK_2)"
						" END INDEX_TOL_2,"
						" CASE WHEN T.ITEM_ENAME = 'FOSMT08-127' THEN C2.REMARK_1 + D2.REMARK_1"
						" ELSE DECODE((C2.REMARK_2 + D2.REMARK_2),0,0,(C2.REMARK_1 + D2.REMARK_1) / (C2.REMARK_2 + D2.REMARK_2))"
						" END INDEX_TOL"
						" FROM T"
						" LEFT JOIN TFOSMT96B A ON T.ITEM_CNAME = A.ITEM_CNAME AND A.FACTORY_DIV = 'A10' AND T.DATE_TIME = A.DATE_TIME"
						" LEFT JOIN TFOSMT96B B ON T.ITEM_CNAME = B.ITEM_CNAME AND B.FACTORY_DIV = 'A20' AND T.DATE_TIME = B.DATE_TIME"
						" LEFT JOIN TFOSMT08A C ON A.ITEM_ENAME = C.ITEM_ENAME AND C.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMMDD')"
						" LEFT JOIN S C2 ON A.ITEM_ENAME = C2.ITEM_ENAME"
						" LEFT JOIN TFOSMT08A D ON B.ITEM_ENAME = D.ITEM_ENAME AND D.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMMDD')"
						" LEFT JOIN S D2 ON B.ITEM_ENAME = D2.ITEM_ENAME"
						" ORDER BY T.SEQ_NO, T.ITEM_ENAME"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();
			}
			else if (tbl == "TFOSMT09")
			{
				//主要能源介质实绩
				bcls_ret->Tables.Add(tbl);
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-42：日期按天前推过滤;过滤边界、参数与结果列保持不变。
// 改写原因：DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " SELECT A.ITEM_ENAME ENERGY_CODE, SUBSTR(A.ITEM_CNAME,1,LENGTH(A.ITEM_CNAME)-8) ENERGY_CNAME,"
						// " B0.REMARK PRICE, A.UNIT, B1.REMARK VALUE_REAL, B2.REMARK VALUE_TARGET,"
						// " COALESCE(TO_NUMBER(C1.REMARK),0) VALUE_DAY_1, COALESCE(TO_NUMBER(C2.REMARK),0) VALUE_DAY_2,"
						// " COALESCE(TO_NUMBER(C3.REMARK),0) VALUE_DAY_3,"
						// " CASE WHEN A.ITEM_ENAME IN ('FOSMT09-103','FOSMT09-105','FOSMT09-123')"
						// " THEN ROUND(COALESCE(DECODE(D.REMARK_2,0,0,D.REMARK_1 / D.REMARK_2),0),0)"
						// " WHEN A.ITEM_ENAME IN ('FOSMT09-131', 'FOSMT09-133')"
						// " THEN ROUND(D.REMARK, 0)"
						// " ELSE ROUND(COALESCE(DECODE(D.REMARK_2,0,0,D.REMARK_1 / D.REMARK_2),0),2)"
						// " END VALUE_MONTH"
						// " FROM TFOSMT96A A"
						// " LEFT JOIN TFOSMT09C B0 ON A.ITEM_ENAME = B0.ITEM_ENAME"
						// " LEFT JOIN TFOSMT96B B1 ON SUBSTR(A.ITEM_CNAME,1,LENGTH(A.ITEM_CNAME)-8) = SUBSTR(B1.ITEM_CNAME,1,LENGTH(B1.ITEM_CNAME)-8)"
						// " AND MOD(SUBSTR(B1.ITEM_ENAME,11,1),2) = 1 AND B1.TABLE_NAME = 'FOSMT09-1' AND B1.DATE_TIME = TO_CHAR((TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS),'YYYY')"
						// " LEFT JOIN TFOSMT96B B2 ON SUBSTR(A.ITEM_CNAME,1,LENGTH(A.ITEM_CNAME)-8) = SUBSTR(B2.ITEM_CNAME,1,LENGTH(B2.ITEM_CNAME)-8)"
						// " AND MOD(SUBSTR(B2.ITEM_ENAME,11,1),2) = 0 AND B2.TABLE_NAME = 'FOSMT09-1' AND B2.DATE_TIME = TO_CHAR((TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS),'YYYY')"
						// " LEFT JOIN TFOSMT09A C1 ON A.ITEM_ENAME = C1.ITEM_ENAME AND C1.DATE_TIME = TO_CHAR((TO_DATE(@DATE_TIME,'YYYYMMDD') - 3 DAYS),'YYYYMMDD')"
						// " LEFT JOIN TFOSMT09A C2 ON A.ITEM_ENAME = C2.ITEM_ENAME AND C2.DATE_TIME = TO_CHAR((TO_DATE(@DATE_TIME,'YYYYMMDD') - 2 DAYS),'YYYYMMDD')"
						// " LEFT JOIN TFOSMT09A C3 ON A.ITEM_ENAME = C3.ITEM_ENAME AND C3.DATE_TIME = TO_CHAR((TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS),'YYYYMMDD')"
						// " LEFT JOIN (SELECT ITEM_ENAME,AVG(REMARK) AS REMARK, SUM(TO_NUMBER(REMARK_1)) REMARK_1, SUM(TO_NUMBER(REMARK_2)) REMARK_2 FROM TFOSMT09A"
						// " WHERE SUBSTR(DATE_TIME,1,6) = TO_CHAR((TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS),'YYYYMM') GROUP BY ITEM_ENAME) D"
						// " ON A.ITEM_ENAME = D.ITEM_ENAME"
						// " WHERE MOD(SUBSTR(A.ITEM_ENAME,11,1),2) = 1"
						// " AND A.TABLE_NAME = 'FOSMT09-1'"
						// " ORDER BY A.SEQ_NO"
						// ;
// DM8 SQL：
					sqlstr =
						" SELECT A.ITEM_ENAME ENERGY_CODE, SUBSTR(A.ITEM_CNAME,1,LENGTH(A.ITEM_CNAME)-8) ENERGY_CNAME,"
						" B0.REMARK PRICE, A.UNIT, B1.REMARK VALUE_REAL, B2.REMARK VALUE_TARGET,"
						" COALESCE(TO_NUMBER(C1.REMARK),0) VALUE_DAY_1, COALESCE(TO_NUMBER(C2.REMARK),0) VALUE_DAY_2,"
						" COALESCE(TO_NUMBER(C3.REMARK),0) VALUE_DAY_3,"
						" CASE WHEN A.ITEM_ENAME IN ('FOSMT09-103','FOSMT09-105','FOSMT09-123')"
						" THEN ROUND(COALESCE(DECODE(D.REMARK_2,0,0,D.REMARK_1 / D.REMARK_2),0),0)"
						" WHEN A.ITEM_ENAME IN ('FOSMT09-131', 'FOSMT09-133')"
						" THEN ROUND(D.REMARK, 0)"
						" ELSE ROUND(COALESCE(DECODE(D.REMARK_2,0,0,D.REMARK_1 / D.REMARK_2),0),2)"
						" END VALUE_MONTH"
						" FROM TFOSMT96A A"
						" LEFT JOIN TFOSMT09C B0 ON A.ITEM_ENAME = B0.ITEM_ENAME"
						" LEFT JOIN TFOSMT96B B1 ON SUBSTR(A.ITEM_CNAME,1,LENGTH(A.ITEM_CNAME)-8) = SUBSTR(B1.ITEM_CNAME,1,LENGTH(B1.ITEM_CNAME)-8)"
						" AND MOD(SUBSTR(B1.ITEM_ENAME,11,1),2) = 1 AND B1.TABLE_NAME = 'FOSMT09-1' AND B1.DATE_TIME = TO_CHAR((TO_DATE(@DATE_TIME,'YYYYMMDD') - 1),'YYYY')"
						" LEFT JOIN TFOSMT96B B2 ON SUBSTR(A.ITEM_CNAME,1,LENGTH(A.ITEM_CNAME)-8) = SUBSTR(B2.ITEM_CNAME,1,LENGTH(B2.ITEM_CNAME)-8)"
						" AND MOD(SUBSTR(B2.ITEM_ENAME,11,1),2) = 0 AND B2.TABLE_NAME = 'FOSMT09-1' AND B2.DATE_TIME = TO_CHAR((TO_DATE(@DATE_TIME,'YYYYMMDD') - 1),'YYYY')"
						" LEFT JOIN TFOSMT09A C1 ON A.ITEM_ENAME = C1.ITEM_ENAME AND C1.DATE_TIME = TO_CHAR((TO_DATE(@DATE_TIME,'YYYYMMDD') - 3),'YYYYMMDD')"
						" LEFT JOIN TFOSMT09A C2 ON A.ITEM_ENAME = C2.ITEM_ENAME AND C2.DATE_TIME = TO_CHAR((TO_DATE(@DATE_TIME,'YYYYMMDD') - 2),'YYYYMMDD')"
						" LEFT JOIN TFOSMT09A C3 ON A.ITEM_ENAME = C3.ITEM_ENAME AND C3.DATE_TIME = TO_CHAR((TO_DATE(@DATE_TIME,'YYYYMMDD') - 1),'YYYYMMDD')"
						" LEFT JOIN (SELECT ITEM_ENAME,AVG(REMARK) AS REMARK, SUM(TO_NUMBER(REMARK_1)) REMARK_1, SUM(TO_NUMBER(REMARK_2)) REMARK_2 FROM TFOSMT09A"
						" WHERE SUBSTR(DATE_TIME,1,6) = TO_CHAR((TO_DATE(@DATE_TIME,'YYYYMMDD') - 1),'YYYYMM') GROUP BY ITEM_ENAME) D"
						" ON A.ITEM_ENAME = D.ITEM_ENAME"
						" WHERE MOD(SUBSTR(A.ITEM_ENAME,11,1),2) = 1"
						" AND A.TABLE_NAME = 'FOSMT09-1'"
						" ORDER BY A.SEQ_NO"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();
			}
			else if (tbl == "TFOSMT10A")
			{
				//倒罐脱硫
				bcls_ret->Tables.Add(tbl);
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-43：生成查询日期之前3天的日期序列;日期边界、参数与结果列保持不变。
// 改写原因：SYSIBM 辅助表改为 DUAL；DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " WITH T(LEVEL, TIME) AS"
						// " (SELECT 1, TO_DATE(@DATE_TIME,'YYYYMMDD') - 3 DAY"
						// " FROM SYSIBM.SYSDUMMY1"
						// " WHERE 1 = 1"
						// " UNION ALL"
						// " SELECT LEVEL + 1,TIME + 1 DAY"
						// " FROM SYSIBM.SYSDUMMY1, T"
						// " WHERE T.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY)"
						// " SELECT TO_CHAR(T.TIME,'YYYYMMDD') DATE_TIME,"
						// " A.IRON_S IRON_S_1, A.IRON_P IRON_P_1, A.IRON_TEMP IRON_TEMP_1, A.MOLTIRON_WT MOLTIRON_WT_1,"
						// " A.SCRAPE_SLAG_WGT SCRAPE_SLAG_WGT_1, A.RATE RATE_1, A.RATIO_NUM RATIO_NUM_1,"
						// " B.IRON_S IRON_S_2, B.IRON_P IRON_P_2, B.IRON_TEMP IRON_TEMP_2, B.MOLTIRON_WT MOLTIRON_WT_2,"
						// " B.SCRAPE_SLAG_WGT SCRAPE_SLAG_WGT_2, B.RATE RATE_2, B.RATIO_NUM RATIO_NUM_2"
						// " FROM T"
						// " LEFT JOIN TFOSMT10A A ON TO_CHAR(T.TIME,'YYYYMMDD') = A.DATE_TIME AND A.FACTORY_DIV = 'A10'"
						// " LEFT JOIN TFOSMT10A B ON TO_CHAR(T.TIME,'YYYYMMDD') = B.DATE_TIME AND B.FACTORY_DIV = 'A20'"
						// " WHERE 1 = 1"
						// ;
// DM8 SQL：
					sqlstr =
						" WITH T(LEVEL, TIME) AS"
						" (SELECT 1, TO_DATE(@DATE_TIME,'YYYYMMDD') - 3"
						" FROM DUAL"
						" WHERE 1 = 1"
						" UNION ALL"
						" SELECT LEVEL + 1,TIME + 1"
						" FROM DUAL, T"
						" WHERE T.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD') - 1)"
						" SELECT TO_CHAR(T.TIME,'YYYYMMDD') DATE_TIME,"
						" A.IRON_S IRON_S_1, A.IRON_P IRON_P_1, A.IRON_TEMP IRON_TEMP_1, A.MOLTIRON_WT MOLTIRON_WT_1,"
						" A.SCRAPE_SLAG_WGT SCRAPE_SLAG_WGT_1, A.RATE RATE_1, A.RATIO_NUM RATIO_NUM_1,"
						" B.IRON_S IRON_S_2, B.IRON_P IRON_P_2, B.IRON_TEMP IRON_TEMP_2, B.MOLTIRON_WT MOLTIRON_WT_2,"
						" B.SCRAPE_SLAG_WGT SCRAPE_SLAG_WGT_2, B.RATE RATE_2, B.RATIO_NUM RATIO_NUM_2"
						" FROM T"
						" LEFT JOIN TFOSMT10A A ON TO_CHAR(T.TIME,'YYYYMMDD') = A.DATE_TIME AND A.FACTORY_DIV = 'A10'"
						" LEFT JOIN TFOSMT10A B ON TO_CHAR(T.TIME,'YYYYMMDD') = B.DATE_TIME AND B.FACTORY_DIV = 'A20'"
						" WHERE 1 = 1"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();

				//处理合计行
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-44：取查询日期前推1日的日期字符串;日历日边界、参数与结果列保持不变。
// 改写原因：SYSIBM 辅助表改为 DUAL；DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " SELECT T.DATE_TIME,"
						// " AVG(A.IRON_S) IRON_S_1, AVG(A.IRON_P) IRON_P_1, AVG(A.IRON_TEMP) IRON_TEMP_1, AVG(A.MOLTIRON_WT) MOLTIRON_WT_1,"
						// " AVG(A.SCRAPE_SLAG_WGT) SCRAPE_SLAG_WGT_1, AVG(A.RATE) RATE_1, AVG(A.RATIO_NUM) RATIO_NUM_1,"
						// " AVG(B.IRON_S) IRON_S_2, AVG(B.IRON_P) IRON_P_2, AVG(B.IRON_TEMP) IRON_TEMP_2, AVG(B.MOLTIRON_WT) MOLTIRON_WT_2,"
						// " AVG(B.SCRAPE_SLAG_WGT) SCRAPE_SLAG_WGT_2, AVG(B.RATE) RATE_2, AVG(B.RATIO_NUM) RATIO_NUM_2"
						// " FROM (SELECT '合计' DATE_TIME FROM SYSIBM.DUAL) T"
						// " LEFT JOIN TFOSMT10A A ON SUBSTR(A.DATE_TIME,1,6) = SUBSTR(TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS,'YYYYMMDD'),1,6) AND A.FACTORY_DIV = 'A10'"
						// " LEFT JOIN TFOSMT10A B ON SUBSTR(B.DATE_TIME,1,6) = SUBSTR(TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS,'YYYYMMDD'),1,6) AND B.FACTORY_DIV = 'A20' AND A.DATE_TIME = B.DATE_TIME"
						// " WHERE 1 = 1"
						// " GROUP BY T.DATE_TIME"
						// ;
// DM8 SQL：
					sqlstr =
						" SELECT T.DATE_TIME,"
						" AVG(A.IRON_S) IRON_S_1, AVG(A.IRON_P) IRON_P_1, AVG(A.IRON_TEMP) IRON_TEMP_1, AVG(A.MOLTIRON_WT) MOLTIRON_WT_1,"
						" AVG(A.SCRAPE_SLAG_WGT) SCRAPE_SLAG_WGT_1, AVG(A.RATE) RATE_1, AVG(A.RATIO_NUM) RATIO_NUM_1,"
						" AVG(B.IRON_S) IRON_S_2, AVG(B.IRON_P) IRON_P_2, AVG(B.IRON_TEMP) IRON_TEMP_2, AVG(B.MOLTIRON_WT) MOLTIRON_WT_2,"
						" AVG(B.SCRAPE_SLAG_WGT) SCRAPE_SLAG_WGT_2, AVG(B.RATE) RATE_2, AVG(B.RATIO_NUM) RATIO_NUM_2"
						" FROM (SELECT '合计' DATE_TIME FROM DUAL) T"
						" LEFT JOIN TFOSMT10A A ON SUBSTR(A.DATE_TIME,1,6) = SUBSTR(TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMMDD'),1,6) AND A.FACTORY_DIV = 'A10'"
						" LEFT JOIN TFOSMT10A B ON SUBSTR(B.DATE_TIME,1,6) = SUBSTR(TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMMDD'),1,6) AND B.FACTORY_DIV = 'A20' AND A.DATE_TIME = B.DATE_TIME"
						" WHERE 1 = 1"
						" GROUP BY T.DATE_TIME"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				EIClass blk;
				cmd_inq.ExecuteQuery(blk.Tables[0]);
				cmd_inq.Close();

				CDataRow& row = bcls_ret->Tables[tbl].Rows.Add();
				row.Merge(blk.Tables[0].Rows[0]);
			}
			else if (tbl == "TFOSMT10B1")
			{
				//2高炉
				bcls_ret->Tables.Add(tbl);
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-45：取查询日期前推1日的日期字符串;日历日边界、参数与结果列保持不变。
// 改写原因：DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " SELECT SEQ_NO DATE_TIME, A.ELM_CODE, A.ELM_VALUE"
						// " FROM TFOSMT10I A"
						// " WHERE FACTORY_DIV = 'A10'"
						// " AND DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS,'YYYYMMDD')"
						// " AND STATION_NO = '2'"
						// " ORDER BY SEQ_NO"
						// ;
// DM8 SQL：
					sqlstr =
						" SELECT SEQ_NO DATE_TIME, A.ELM_CODE, A.ELM_VALUE"
						" FROM TFOSMT10I A"
						" WHERE FACTORY_DIV = 'A10'"
						" AND DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMMDD')"
						" AND STATION_NO = '2'"
						" ORDER BY SEQ_NO"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();
			}
			else if (tbl == "TFOSMT10B2")
			{
				//4高炉
				bcls_ret->Tables.Add(tbl);
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-46：取查询日期前推1日的日期字符串;日历日边界、参数与结果列保持不变。
// 改写原因：DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " SELECT SEQ_NO DATE_TIME, A.ELM_CODE, A.ELM_VALUE"
						// " FROM TFOSMT10I A"
						// " WHERE FACTORY_DIV = 'A20'"
						// " AND DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS,'YYYYMMDD')"
						// " AND STATION_NO = '4'"
						// " ORDER BY SEQ_NO"
						// ;
// DM8 SQL：
					sqlstr =
						" SELECT SEQ_NO DATE_TIME, A.ELM_CODE, A.ELM_VALUE"
						" FROM TFOSMT10I A"
						" WHERE FACTORY_DIV = 'A20'"
						" AND DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMMDD')"
						" AND STATION_NO = '4'"
						" ORDER BY SEQ_NO"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();
			}
			else if (tbl == "TFOSMT10B3")
			{
				//5高炉
				bcls_ret->Tables.Add(tbl);
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-47：取查询日期前推1日的日期字符串;日历日边界、参数与结果列保持不变。
// 改写原因：DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " SELECT SEQ_NO DATE_TIME, A.ELM_CODE, A.ELM_VALUE"
						// " FROM TFOSMT10I A"
						// " WHERE FACTORY_DIV = 'A20'"
						// " AND DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS,'YYYYMMDD')"
						// " AND STATION_NO = '5'"
						// " ORDER BY SEQ_NO"
						// ;
// DM8 SQL：
					sqlstr =
						" SELECT SEQ_NO DATE_TIME, A.ELM_CODE, A.ELM_VALUE"
						" FROM TFOSMT10I A"
						" WHERE FACTORY_DIV = 'A20'"
						" AND DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMMDD')"
						" AND STATION_NO = '5'"
						" ORDER BY SEQ_NO"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();
			}
			else if (tbl == "TFOSMT10C")
			{
				//废钢加入量
				bcls_ret->Tables.Add(tbl);
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-48：生成查询日期之前3天的日期序列;日期边界、参数与结果列保持不变。
// 改写原因：SYSIBM 辅助表改为 DUAL；DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " WITH T(LEVEL, TIME) AS"
						// " (SELECT 1, TO_DATE(@DATE_TIME,'YYYYMMDD') - 3 DAY"
						// " FROM SYSIBM.SYSDUMMY1"
						// " WHERE 1 = 1"
						// " UNION ALL"
						// " SELECT LEVEL + 1,TIME + 1 DAY"
						// " FROM SYSIBM.SYSDUMMY1, T"
						// " WHERE T.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY)"
						// " SELECT A.DATE_TIME, A.RAW_IRON_WT RAW_IRON_WT_1, A.SCRAP_STEEL_IN SCRAP_STEEL_IN_1,"
						// " A.SCRAP_STEEL_BOF SCRAP_STEEL_BOF_1, A.SCRAP_STEEL_HEAT SCRAP_STEEL_HEAT_1,"
						// " A.RATE RATE_1, A.SCRAP_STEEL_SR SCRAP_STEEL_SR_1,"
						// " B.RAW_IRON_WT RAW_IRON_WT_2, B.SCRAP_STEEL_IN SCRAP_STEEL_IN_2,"
						// " B.SCRAP_STEEL_BOF SCRAP_STEEL_BOF_2, B.SCRAP_STEEL_HEAT SCRAP_STEEL_HEAT_2,"
						// " B.RATE RATE_2, B.SCRAP_STEEL_SR SCRAP_STEEL_SR_2, B.SCRAP_STEEL_TPD,"
						// " C.SCRAP_STEEL_TPC, C.RATIO_NUM"
						// " FROM T"
						// " LEFT JOIN TFOSMT10B A ON TO_CHAR(T.TIME,'YYYYMMDD') = A.DATE_TIME AND A.FACTORY_DIV = 'A10'"
						// " LEFT JOIN TFOSMT10B B ON TO_CHAR(T.TIME,'YYYYMMDD') = B.DATE_TIME AND B.FACTORY_DIV = 'A20'"
						// " LEFT JOIN TFOSMT10B C ON TO_CHAR(T.TIME,'YYYYMMDD') = C.DATE_TIME AND C.FACTORY_DIV = 'A00'"
						// " WHERE 1 = 1"
						// ;
// DM8 SQL：
					sqlstr =
						" WITH T(LEVEL, TIME) AS"
						" (SELECT 1, TO_DATE(@DATE_TIME,'YYYYMMDD') - 3"
						" FROM DUAL"
						" WHERE 1 = 1"
						" UNION ALL"
						" SELECT LEVEL + 1,TIME + 1"
						" FROM DUAL, T"
						" WHERE T.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD') - 1)"
						" SELECT A.DATE_TIME, A.RAW_IRON_WT RAW_IRON_WT_1, A.SCRAP_STEEL_IN SCRAP_STEEL_IN_1,"
						" A.SCRAP_STEEL_BOF SCRAP_STEEL_BOF_1, A.SCRAP_STEEL_HEAT SCRAP_STEEL_HEAT_1,"
						" A.RATE RATE_1, A.SCRAP_STEEL_SR SCRAP_STEEL_SR_1,"
						" B.RAW_IRON_WT RAW_IRON_WT_2, B.SCRAP_STEEL_IN SCRAP_STEEL_IN_2,"
						" B.SCRAP_STEEL_BOF SCRAP_STEEL_BOF_2, B.SCRAP_STEEL_HEAT SCRAP_STEEL_HEAT_2,"
						" B.RATE RATE_2, B.SCRAP_STEEL_SR SCRAP_STEEL_SR_2, B.SCRAP_STEEL_TPD,"
						" C.SCRAP_STEEL_TPC, C.RATIO_NUM"
						" FROM T"
						" LEFT JOIN TFOSMT10B A ON TO_CHAR(T.TIME,'YYYYMMDD') = A.DATE_TIME AND A.FACTORY_DIV = 'A10'"
						" LEFT JOIN TFOSMT10B B ON TO_CHAR(T.TIME,'YYYYMMDD') = B.DATE_TIME AND B.FACTORY_DIV = 'A20'"
						" LEFT JOIN TFOSMT10B C ON TO_CHAR(T.TIME,'YYYYMMDD') = C.DATE_TIME AND C.FACTORY_DIV = 'A00'"
						" WHERE 1 = 1"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();

				//处理合计行
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-49：生成查询日期之前1天的日期序列;日期边界、参数与结果列保持不变。
// 改写原因：SYSIBM 辅助表改为 DUAL；DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " WITH T(LEVEL, TIME) AS"
						// " (SELECT 1, TO_DATE(TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY,'YYYYMM') || '01','YYYYMMDD')"
						// " FROM SYSIBM.SYSDUMMY1"
						// " WHERE 1 = 1"
						// " UNION ALL"
						// " SELECT LEVEL + 1,TIME + 1 DAY"
						// " FROM SYSIBM.SYSDUMMY1, T"
						// " WHERE T.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY)"
						// " SELECT '合计' DATE_TIME, SUM(A.RAW_IRON_WT) RAW_IRON_WT_1, SUM(A.SCRAP_STEEL_IN) SCRAP_STEEL_IN_1,"
						// " SUM(A.SCRAP_STEEL_BOF) SCRAP_STEEL_BOF_1, SUM(A.SCRAP_STEEL_HEAT) SCRAP_STEEL_HEAT_1,"
						// " ROUND(AVG(A.RATE),2) RATE_1, SUM(A.SCRAP_STEEL_SR) SCRAP_STEEL_SR_1,"
						// " SUM(B.RAW_IRON_WT) RAW_IRON_WT_2, SUM(B.SCRAP_STEEL_IN) SCRAP_STEEL_IN_2,"
						// " SUM(B.SCRAP_STEEL_BOF) SCRAP_STEEL_BOF_2, SUM(B.SCRAP_STEEL_HEAT) SCRAP_STEEL_HEAT_2,"
						// " ROUND(AVG(B.RATE),2) RATE_2, SUM(B.SCRAP_STEEL_SR) SCRAP_STEEL_SR_2, SUM(B.SCRAP_STEEL_TPD) SCRAP_STEEL_TPD,"
						// " SUM(C.SCRAP_STEEL_TPC) SCRAP_STEEL_TPC, ROUND(AVG(C.RATIO_NUM),2) RATIO_NUM"
						// " FROM T"
						// " LEFT JOIN TFOSMT10B A ON TO_CHAR(T.TIME,'YYYYMMDD') = A.DATE_TIME AND A.FACTORY_DIV = 'A10'"
						// " LEFT JOIN TFOSMT10B B ON TO_CHAR(T.TIME,'YYYYMMDD') = B.DATE_TIME AND B.FACTORY_DIV = 'A20'"
						// " LEFT JOIN TFOSMT10B C ON TO_CHAR(T.TIME,'YYYYMMDD') = C.DATE_TIME AND C.FACTORY_DIV = 'A00'"
						// " WHERE 1 = 1"
						// ;
// DM8 SQL：
					sqlstr =
						" WITH T(LEVEL, TIME) AS"
						" (SELECT 1, TO_DATE(TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMM') || '01','YYYYMMDD')"
						" FROM DUAL"
						" WHERE 1 = 1"
						" UNION ALL"
						" SELECT LEVEL + 1,TIME + 1"
						" FROM DUAL, T"
						" WHERE T.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD') - 1)"
						" SELECT '合计' DATE_TIME, SUM(A.RAW_IRON_WT) RAW_IRON_WT_1, SUM(A.SCRAP_STEEL_IN) SCRAP_STEEL_IN_1,"
						" SUM(A.SCRAP_STEEL_BOF) SCRAP_STEEL_BOF_1, SUM(A.SCRAP_STEEL_HEAT) SCRAP_STEEL_HEAT_1,"
						" ROUND(AVG(A.RATE),2) RATE_1, SUM(A.SCRAP_STEEL_SR) SCRAP_STEEL_SR_1,"
						" SUM(B.RAW_IRON_WT) RAW_IRON_WT_2, SUM(B.SCRAP_STEEL_IN) SCRAP_STEEL_IN_2,"
						" SUM(B.SCRAP_STEEL_BOF) SCRAP_STEEL_BOF_2, SUM(B.SCRAP_STEEL_HEAT) SCRAP_STEEL_HEAT_2,"
						" ROUND(AVG(B.RATE),2) RATE_2, SUM(B.SCRAP_STEEL_SR) SCRAP_STEEL_SR_2, SUM(B.SCRAP_STEEL_TPD) SCRAP_STEEL_TPD,"
						" SUM(C.SCRAP_STEEL_TPC) SCRAP_STEEL_TPC, ROUND(AVG(C.RATIO_NUM),2) RATIO_NUM"
						" FROM T"
						" LEFT JOIN TFOSMT10B A ON TO_CHAR(T.TIME,'YYYYMMDD') = A.DATE_TIME AND A.FACTORY_DIV = 'A10'"
						" LEFT JOIN TFOSMT10B B ON TO_CHAR(T.TIME,'YYYYMMDD') = B.DATE_TIME AND B.FACTORY_DIV = 'A20'"
						" LEFT JOIN TFOSMT10B C ON TO_CHAR(T.TIME,'YYYYMMDD') = C.DATE_TIME AND C.FACTORY_DIV = 'A00'"
						" WHERE 1 = 1"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				EIClass blk;
				cmd_inq.ExecuteQuery(blk.Tables[0]);
				cmd_inq.Close();

				CDataRow& row = bcls_ret->Tables[tbl].Rows.Add();
				row.Merge(blk.Tables[0].Rows[0]);
			}
			else if (tbl == "TFOSMT10D")
			{
				//硫指标
				bcls_ret->Tables.Add(tbl);
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-50：生成查询日期之前3天的日期序列;日期边界、参数与结果列保持不变。
// 改写原因：SYSIBM 辅助表改为 DUAL；DB2 日期天数后缀改为整数天运算；二元标量 MAX 改为空值守卫的 GREATEST,空值传播与 DB2 标量 MAX 一致；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " WITH T(LEVEL, TIME) AS"
						// " (SELECT 1, TO_DATE(@DATE_TIME,'YYYYMMDD') - 3 DAY"
						// " FROM SYSIBM.SYSDUMMY1"
						// " WHERE 1 = 1"
						// " UNION ALL"
						// " SELECT LEVEL + 1,TIME + 1 DAY"
						// " FROM SYSIBM.SYSDUMMY1, T"
						// " WHERE T.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY)"
						// " SELECT A.DATE_TIME, A.PRODUCT_CHARGE PRODUCT_CHARGE_1, A.QUALIFIED_CHARGE QUALIFIED_CHARGE_1,"
						// " DECODE(A.PRODUCT_CHARGE,0,0, ROUND(100.00 * A.QUALIFIED_CHARGE / A.PRODUCT_CHARGE,2)) QUALIFIED_RATE_1, MAX(0,A.ACT_COSTING) ACT_COSTING_1,"
						// " B.PRODUCT_CHARGE PRODUCT_CHARGE_2, B.QUALIFIED_CHARGE QUALIFIED_CHARGE_2,"
						// " DECODE(B.PRODUCT_CHARGE,0,0,ROUND(100.00 * B.QUALIFIED_CHARGE / B.PRODUCT_CHARGE,2)) QUALIFIED_RATE_2, MAX(0,B.ACT_COSTING) ACT_COSTING_2,"
						// " MAX(0,A.ACT_COSTING) + MAX(0,B.ACT_COSTING) ACT_COSTING_TOTAL"
						// " FROM T"
						// " LEFT JOIN TFOSMT10C A ON TO_CHAR(T.TIME,'YYYYMMDD') = A.DATE_TIME AND A.FACTORY_DIV = 'A10'"
						// " LEFT JOIN TFOSMT10C B ON TO_CHAR(T.TIME,'YYYYMMDD') = B.DATE_TIME AND B.FACTORY_DIV = 'A20'"
						// " WHERE 1 = 1"
						// ;
// DM8 SQL：
					sqlstr =
						" WITH T(LEVEL, TIME) AS"
						" (SELECT 1, TO_DATE(@DATE_TIME,'YYYYMMDD') - 3"
						" FROM DUAL"
						" WHERE 1 = 1"
						" UNION ALL"
						" SELECT LEVEL + 1,TIME + 1"
						" FROM DUAL, T"
						" WHERE T.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD') - 1)"
						" SELECT A.DATE_TIME, A.PRODUCT_CHARGE PRODUCT_CHARGE_1, A.QUALIFIED_CHARGE QUALIFIED_CHARGE_1,"
						" DECODE(A.PRODUCT_CHARGE,0,0, ROUND(100.00 * A.QUALIFIED_CHARGE / A.PRODUCT_CHARGE,2)) QUALIFIED_RATE_1, CASE WHEN 0 IS NULL OR A.ACT_COSTING IS NULL THEN NULL ELSE GREATEST(0, A.ACT_COSTING) END ACT_COSTING_1,"
						" B.PRODUCT_CHARGE PRODUCT_CHARGE_2, B.QUALIFIED_CHARGE QUALIFIED_CHARGE_2,"
						" DECODE(B.PRODUCT_CHARGE,0,0,ROUND(100.00 * B.QUALIFIED_CHARGE / B.PRODUCT_CHARGE,2)) QUALIFIED_RATE_2, CASE WHEN 0 IS NULL OR B.ACT_COSTING IS NULL THEN NULL ELSE GREATEST(0, B.ACT_COSTING) END ACT_COSTING_2,"
						" CASE WHEN 0 IS NULL OR A.ACT_COSTING IS NULL THEN NULL ELSE GREATEST(0, A.ACT_COSTING) END + CASE WHEN 0 IS NULL OR B.ACT_COSTING IS NULL THEN NULL ELSE GREATEST(0, B.ACT_COSTING) END ACT_COSTING_TOTAL"
						" FROM T"
						" LEFT JOIN TFOSMT10C A ON TO_CHAR(T.TIME,'YYYYMMDD') = A.DATE_TIME AND A.FACTORY_DIV = 'A10'"
						" LEFT JOIN TFOSMT10C B ON TO_CHAR(T.TIME,'YYYYMMDD') = B.DATE_TIME AND B.FACTORY_DIV = 'A20'"
						" WHERE 1 = 1"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();

				//处理合计行
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-51：生成查询日期之前1天的日期序列;日期边界、参数与结果列保持不变。
// 改写原因：SYSIBM 辅助表改为 DUAL；DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " WITH T(LEVEL, TIME) AS"
						// " (SELECT 1, TO_DATE(TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY,'YYYYMM') || '01','YYYYMMDD')"
						// " FROM SYSIBM.SYSDUMMY1"
						// " WHERE 1 = 1"
						// " UNION ALL"
						// " SELECT LEVEL + 1,TIME + 1 DAY"
						// " FROM SYSIBM.SYSDUMMY1, T"
						// " WHERE T.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY)"
						// " SELECT '合计' DATE_TIME, SUM(A.PRODUCT_CHARGE) PRODUCT_CHARGE_1, SUM(A.QUALIFIED_CHARGE) QUALIFIED_CHARGE_1,"
						// " ROUND(100.00 * SUM(A.QUALIFIED_CHARGE) / SUM(A.PRODUCT_CHARGE),2) QUALIFIED_RATE_1,"
						// " CASE WHEN SUM(A.ACT_COSTING) < 0 THEN 0 ELSE SUM(A.ACT_COSTING) END ACT_COSTING_1,"
						// " SUM(B.PRODUCT_CHARGE) PRODUCT_CHARGE_2, SUM(B.QUALIFIED_CHARGE) QUALIFIED_CHARGE_2,"
						// " ROUND(100.00 * SUM(B.QUALIFIED_CHARGE) / SUM(B.PRODUCT_CHARGE),2) QUALIFIED_RATE_2,"
						// " CASE WHEN SUM(B.ACT_COSTING) < 0 THEN 0 ELSE SUM(B.ACT_COSTING) END ACT_COSTING_2,"
						// " CASE WHEN SUM(A.ACT_COSTING) < 0 THEN 0 ELSE SUM(A.ACT_COSTING) END + CASE WHEN SUM(B.ACT_COSTING) < 0 THEN 0 ELSE SUM(B.ACT_COSTING) END ACT_COSTING_TOTAL"
						// " FROM T"
						// " LEFT JOIN TFOSMT10C A ON TO_CHAR(T.TIME,'YYYYMMDD') = A.DATE_TIME AND A.FACTORY_DIV = 'A10'"
						// " LEFT JOIN TFOSMT10C B ON TO_CHAR(T.TIME,'YYYYMMDD') = B.DATE_TIME AND B.FACTORY_DIV = 'A20'"
						// " WHERE 1 = 1"
						// ;
// DM8 SQL：
					sqlstr =
						" WITH T(LEVEL, TIME) AS"
						" (SELECT 1, TO_DATE(TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMM') || '01','YYYYMMDD')"
						" FROM DUAL"
						" WHERE 1 = 1"
						" UNION ALL"
						" SELECT LEVEL + 1,TIME + 1"
						" FROM DUAL, T"
						" WHERE T.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD') - 1)"
						" SELECT '合计' DATE_TIME, SUM(A.PRODUCT_CHARGE) PRODUCT_CHARGE_1, SUM(A.QUALIFIED_CHARGE) QUALIFIED_CHARGE_1,"
						" ROUND(100.00 * SUM(A.QUALIFIED_CHARGE) / SUM(A.PRODUCT_CHARGE),2) QUALIFIED_RATE_1,"
						" CASE WHEN SUM(A.ACT_COSTING) < 0 THEN 0 ELSE SUM(A.ACT_COSTING) END ACT_COSTING_1,"
						" SUM(B.PRODUCT_CHARGE) PRODUCT_CHARGE_2, SUM(B.QUALIFIED_CHARGE) QUALIFIED_CHARGE_2,"
						" ROUND(100.00 * SUM(B.QUALIFIED_CHARGE) / SUM(B.PRODUCT_CHARGE),2) QUALIFIED_RATE_2,"
						" CASE WHEN SUM(B.ACT_COSTING) < 0 THEN 0 ELSE SUM(B.ACT_COSTING) END ACT_COSTING_2,"
						" CASE WHEN SUM(A.ACT_COSTING) < 0 THEN 0 ELSE SUM(A.ACT_COSTING) END + CASE WHEN SUM(B.ACT_COSTING) < 0 THEN 0 ELSE SUM(B.ACT_COSTING) END ACT_COSTING_TOTAL"
						" FROM T"
						" LEFT JOIN TFOSMT10C A ON TO_CHAR(T.TIME,'YYYYMMDD') = A.DATE_TIME AND A.FACTORY_DIV = 'A10'"
						" LEFT JOIN TFOSMT10C B ON TO_CHAR(T.TIME,'YYYYMMDD') = B.DATE_TIME AND B.FACTORY_DIV = 'A20'"
						" WHERE 1 = 1"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				EIClass blk;
				cmd_inq.ExecuteQuery(blk.Tables[0]);
				cmd_inq.Close();

				CDataRow& row = bcls_ret->Tables[tbl].Rows.Add();
				row.Merge(blk.Tables[0].Rows[0]);
			}
			else if (tbl == "TFOSMT10E")
			{
				//铁水装入异常信息
				bcls_ret->Tables.Add(tbl);
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-52：日期按天前推过滤;过滤边界、参数与结果列保持不变。
// 改写原因：DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " SELECT A.DATE_TIME, A.ADJUST_CHARGE ADJUST_CHARGE_1, A.MOLTIRON_WT MOLTIRON_WT_1, A.REMARK REMARK_1,"
						// " B.ADJUST_CHARGE ADJUST_CHARGE_2, B.MOLTIRON_WT MOLTIRON_WT_2, B.REMARK REMARK_2, A.MOLTIRON_WT + B.MOLTIRON_WT MOLTIRON_WT_TOTAL"
						// " FROM TFOSMT10D A"
						// " LEFT JOIN TFOSMT10D B ON A.DATE_TIME = B.DATE_TIME AND B.FACTORY_DIV = 'A20'"
						// " WHERE A.FACTORY_DIV = 'A10'"
						// " AND A.DATE_TIME >= TO_CHAR((TO_DATE(@DATE_TIME,'YYYYMMDD') - 10 DAYS),'YYYYMMDD')"
						// " AND A.DATE_TIME <= TO_CHAR((TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS),'YYYYMMDD')"
						// ;
// DM8 SQL：
					sqlstr =
						" SELECT A.DATE_TIME, A.ADJUST_CHARGE ADJUST_CHARGE_1, A.MOLTIRON_WT MOLTIRON_WT_1, A.REMARK REMARK_1,"
						" B.ADJUST_CHARGE ADJUST_CHARGE_2, B.MOLTIRON_WT MOLTIRON_WT_2, B.REMARK REMARK_2, A.MOLTIRON_WT + B.MOLTIRON_WT MOLTIRON_WT_TOTAL"
						" FROM TFOSMT10D A"
						" LEFT JOIN TFOSMT10D B ON A.DATE_TIME = B.DATE_TIME AND B.FACTORY_DIV = 'A20'"
						" WHERE A.FACTORY_DIV = 'A10'"
						" AND A.DATE_TIME >= TO_CHAR((TO_DATE(@DATE_TIME,'YYYYMMDD') - 10),'YYYYMMDD')"
						" AND A.DATE_TIME <= TO_CHAR((TO_DATE(@DATE_TIME,'YYYYMMDD') - 1),'YYYYMMDD')"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();

				//处理合计行
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-53：日期按天前推过滤;过滤边界、参数与结果列保持不变。
// 改写原因：DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " SELECT '合计' DATE_TIME,"
						// " SUM(A.ADJUST_CHARGE) ADJUST_CHARGE_1, SUM(A.MOLTIRON_WT) MOLTIRON_WT_1, '' REMARK_1,"
						// " SUM(B.ADJUST_CHARGE) ADJUST_CHARGE_2, SUM(B.MOLTIRON_WT) MOLTIRON_WT_2, '' REMARK_2,"
						// " SUM(A.MOLTIRON_WT + B.MOLTIRON_WT) MOLTIRON_WT_TOTAL"
						// " FROM TFOSMT10D A"
						// " LEFT JOIN TFOSMT10D B ON A.DATE_TIME = B.DATE_TIME AND B.FACTORY_DIV = 'A20'"
						// " WHERE A.FACTORY_DIV = 'A10'"
						// " AND A.DATE_TIME >= TO_CHAR((TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS),'YYYYMM') || '01'"
						// " AND A.DATE_TIME <= TO_CHAR((TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS),'YYYYMMDD')"
						// ;
// DM8 SQL：
					sqlstr =
						" SELECT '合计' DATE_TIME,"
						" SUM(A.ADJUST_CHARGE) ADJUST_CHARGE_1, SUM(A.MOLTIRON_WT) MOLTIRON_WT_1, '' REMARK_1,"
						" SUM(B.ADJUST_CHARGE) ADJUST_CHARGE_2, SUM(B.MOLTIRON_WT) MOLTIRON_WT_2, '' REMARK_2,"
						" SUM(A.MOLTIRON_WT + B.MOLTIRON_WT) MOLTIRON_WT_TOTAL"
						" FROM TFOSMT10D A"
						" LEFT JOIN TFOSMT10D B ON A.DATE_TIME = B.DATE_TIME AND B.FACTORY_DIV = 'A20'"
						" WHERE A.FACTORY_DIV = 'A10'"
						" AND A.DATE_TIME >= TO_CHAR((TO_DATE(@DATE_TIME,'YYYYMMDD') - 1),'YYYYMM') || '01'"
						" AND A.DATE_TIME <= TO_CHAR((TO_DATE(@DATE_TIME,'YYYYMMDD') - 1),'YYYYMMDD')"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				EIClass blk;
				cmd_inq.ExecuteQuery(blk.Tables[0]);
				cmd_inq.Close();

				CDataRow& row = bcls_ret->Tables[tbl].Rows.Add();
				row.Merge(blk.Tables[0].Rows[0]);
			}
			else if (tbl == "TFOSMT10F")
			{
				//发热剂使用情况
				bcls_ret->Tables.Add(tbl);
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-54：生成查询日期之前10天的日期序列;日期边界、参数与结果列保持不变。
// 改写原因：SYSIBM 辅助表改为 DUAL；DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " WITH A(LEVEL, TIME) AS"
						// " (SELECT 1, TO_DATE(@DATE_TIME,'YYYYMMDD') - 10 DAY"
						// " FROM SYSIBM.SYSDUMMY1"
						// " WHERE 1 = 1"
						// " UNION ALL"
						// " SELECT LEVEL + 1,TIME + 1 DAY"
						// " FROM SYSIBM.SYSDUMMY1, A"
						// " WHERE A.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY)"
						// " SELECT TO_CHAR(A.TIME,'YYYYMMDD') DATE_TIME,"
						// " B1.RATE RATE_1, B2.RATE RATE_2, B3.RATE RATE_3, B4.RATE RATE_4,"
						// " B5.RATE RATE_5, B6.RATE RATE_6, B7.RATE RATE_7, B8.RATE RATE_8,"
						// " C1.RATE RATE_9, C2.RATE RATE_10, C3.RATE RATE_11, C4.RATE RATE_12"
						// " FROM A"
						// " LEFT JOIN TFOSMT10E B1 ON TO_CHAR(A.TIME,'YYYYMMDD') = B1.DATE_TIME AND B1.FACTORY_DIV = 'A10' AND B1.MAT_CODE = '1'"
						// " LEFT JOIN TFOSMT10E B2 ON TO_CHAR(A.TIME,'YYYYMMDD') = B2.DATE_TIME AND B2.FACTORY_DIV = 'A10' AND B2.MAT_CODE = '2'"
						// " LEFT JOIN TFOSMT10E B3 ON TO_CHAR(A.TIME,'YYYYMMDD') = B3.DATE_TIME AND B3.FACTORY_DIV = 'A10' AND B3.MAT_CODE = '3'"
						// " LEFT JOIN TFOSMT10E B4 ON TO_CHAR(A.TIME,'YYYYMMDD') = B4.DATE_TIME AND B4.FACTORY_DIV = 'A10' AND B4.MAT_CODE = '5'"
						// " LEFT JOIN TFOSMT10E B5 ON TO_CHAR(A.TIME,'YYYYMMDD') = B5.DATE_TIME AND B5.FACTORY_DIV = 'A20' AND B5.MAT_CODE = '1'"
						// " LEFT JOIN TFOSMT10E B6 ON TO_CHAR(A.TIME,'YYYYMMDD') = B6.DATE_TIME AND B6.FACTORY_DIV = 'A20' AND B6.MAT_CODE = '2'"
						// " LEFT JOIN TFOSMT10E B7 ON TO_CHAR(A.TIME,'YYYYMMDD') = B7.DATE_TIME AND B7.FACTORY_DIV = 'A20' AND B7.MAT_CODE = '3'"
						// " LEFT JOIN TFOSMT10E B8 ON TO_CHAR(A.TIME,'YYYYMMDD') = B8.DATE_TIME AND B8.FACTORY_DIV = 'A20' AND B8.MAT_CODE = '5'"
						// " LEFT JOIN TFOSMT10E C1 ON TO_CHAR(A.TIME,'YYYYMMDD') = C1.DATE_TIME AND C1.FACTORY_DIV = 'A00' AND C1.MAT_CODE = '1'"
						// " LEFT JOIN TFOSMT10E C2 ON TO_CHAR(A.TIME,'YYYYMMDD') = C2.DATE_TIME AND C2.FACTORY_DIV = 'A00' AND C2.MAT_CODE = '2'"
						// " LEFT JOIN TFOSMT10E C3 ON TO_CHAR(A.TIME,'YYYYMMDD') = C3.DATE_TIME AND C3.FACTORY_DIV = 'A00' AND C3.MAT_CODE = '3'"
						// " LEFT JOIN TFOSMT10E C4 ON TO_CHAR(A.TIME,'YYYYMMDD') = C4.DATE_TIME AND C4.FACTORY_DIV = 'A00' AND C4.MAT_CODE = '5'"
						// ;
// DM8 SQL：
					sqlstr =
						" WITH A(LEVEL, TIME) AS"
						" (SELECT 1, TO_DATE(@DATE_TIME,'YYYYMMDD') - 10"
						" FROM DUAL"
						" WHERE 1 = 1"
						" UNION ALL"
						" SELECT LEVEL + 1,TIME + 1"
						" FROM DUAL, A"
						" WHERE A.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD') - 1)"
						" SELECT TO_CHAR(A.TIME,'YYYYMMDD') DATE_TIME,"
						" B1.RATE RATE_1, B2.RATE RATE_2, B3.RATE RATE_3, B4.RATE RATE_4,"
						" B5.RATE RATE_5, B6.RATE RATE_6, B7.RATE RATE_7, B8.RATE RATE_8,"
						" C1.RATE RATE_9, C2.RATE RATE_10, C3.RATE RATE_11, C4.RATE RATE_12"
						" FROM A"
						" LEFT JOIN TFOSMT10E B1 ON TO_CHAR(A.TIME,'YYYYMMDD') = B1.DATE_TIME AND B1.FACTORY_DIV = 'A10' AND B1.MAT_CODE = '1'"
						" LEFT JOIN TFOSMT10E B2 ON TO_CHAR(A.TIME,'YYYYMMDD') = B2.DATE_TIME AND B2.FACTORY_DIV = 'A10' AND B2.MAT_CODE = '2'"
						" LEFT JOIN TFOSMT10E B3 ON TO_CHAR(A.TIME,'YYYYMMDD') = B3.DATE_TIME AND B3.FACTORY_DIV = 'A10' AND B3.MAT_CODE = '3'"
						" LEFT JOIN TFOSMT10E B4 ON TO_CHAR(A.TIME,'YYYYMMDD') = B4.DATE_TIME AND B4.FACTORY_DIV = 'A10' AND B4.MAT_CODE = '5'"
						" LEFT JOIN TFOSMT10E B5 ON TO_CHAR(A.TIME,'YYYYMMDD') = B5.DATE_TIME AND B5.FACTORY_DIV = 'A20' AND B5.MAT_CODE = '1'"
						" LEFT JOIN TFOSMT10E B6 ON TO_CHAR(A.TIME,'YYYYMMDD') = B6.DATE_TIME AND B6.FACTORY_DIV = 'A20' AND B6.MAT_CODE = '2'"
						" LEFT JOIN TFOSMT10E B7 ON TO_CHAR(A.TIME,'YYYYMMDD') = B7.DATE_TIME AND B7.FACTORY_DIV = 'A20' AND B7.MAT_CODE = '3'"
						" LEFT JOIN TFOSMT10E B8 ON TO_CHAR(A.TIME,'YYYYMMDD') = B8.DATE_TIME AND B8.FACTORY_DIV = 'A20' AND B8.MAT_CODE = '5'"
						" LEFT JOIN TFOSMT10E C1 ON TO_CHAR(A.TIME,'YYYYMMDD') = C1.DATE_TIME AND C1.FACTORY_DIV = 'A00' AND C1.MAT_CODE = '1'"
						" LEFT JOIN TFOSMT10E C2 ON TO_CHAR(A.TIME,'YYYYMMDD') = C2.DATE_TIME AND C2.FACTORY_DIV = 'A00' AND C2.MAT_CODE = '2'"
						" LEFT JOIN TFOSMT10E C3 ON TO_CHAR(A.TIME,'YYYYMMDD') = C3.DATE_TIME AND C3.FACTORY_DIV = 'A00' AND C3.MAT_CODE = '3'"
						" LEFT JOIN TFOSMT10E C4 ON TO_CHAR(A.TIME,'YYYYMMDD') = C4.DATE_TIME AND C4.FACTORY_DIV = 'A00' AND C4.MAT_CODE = '5'"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();

				//处理合计行
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-55：生成查询日期之前1天的日期序列;日期边界、参数与结果列保持不变。
// 改写原因：SYSIBM 辅助表改为 DUAL；DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " WITH A(LEVEL, TIME) AS"
						// " (SELECT 1, TO_DATE(TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY,'YYYYMM') || '01','YYYYMMDD')"
						// " FROM SYSIBM.SYSDUMMY1"
						// " WHERE 1 = 1"
						// " UNION ALL"
						// " SELECT LEVEL + 1,TIME + 1 DAY"
						// " FROM SYSIBM.SYSDUMMY1, A"
						// " WHERE A.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY)"
						// " SELECT '合计' DATE_TIME,"
						// " ROUND(AVG(B1.RATE),2) RATE_1, ROUND(AVG(B2.RATE),2) RATE_2, ROUND(AVG(B3.RATE),2) RATE_3, ROUND(AVG(B4.RATE),2) RATE_4,"
						// " ROUND(AVG(B5.RATE),2) RATE_5, ROUND(AVG(B6.RATE),2) RATE_6, ROUND(AVG(B7.RATE),2) RATE_7, ROUND(AVG(B8.RATE),2) RATE_8,"
						// " ROUND(AVG(C1.RATE),2) RATE_9, ROUND(AVG(C2.RATE),2) RATE_10, ROUND(AVG(C3.RATE),2) RATE_11, ROUND(AVG(C4.RATE),2) RATE_12"
						// " FROM A"
						// " LEFT JOIN TFOSMT10E B1 ON TO_CHAR(A.TIME,'YYYYMMDD') = B1.DATE_TIME AND B1.FACTORY_DIV = 'A10' AND B1.MAT_CODE = '1'"
						// " LEFT JOIN TFOSMT10E B2 ON TO_CHAR(A.TIME,'YYYYMMDD') = B2.DATE_TIME AND B2.FACTORY_DIV = 'A10' AND B2.MAT_CODE = '2'"
						// " LEFT JOIN TFOSMT10E B3 ON TO_CHAR(A.TIME,'YYYYMMDD') = B3.DATE_TIME AND B3.FACTORY_DIV = 'A10' AND B3.MAT_CODE = '3'"
						// " LEFT JOIN TFOSMT10E B4 ON TO_CHAR(A.TIME,'YYYYMMDD') = B4.DATE_TIME AND B4.FACTORY_DIV = 'A10' AND B4.MAT_CODE = '5'"
						// " LEFT JOIN TFOSMT10E B5 ON TO_CHAR(A.TIME,'YYYYMMDD') = B5.DATE_TIME AND B5.FACTORY_DIV = 'A20' AND B5.MAT_CODE = '1'"
						// " LEFT JOIN TFOSMT10E B6 ON TO_CHAR(A.TIME,'YYYYMMDD') = B6.DATE_TIME AND B6.FACTORY_DIV = 'A20' AND B6.MAT_CODE = '2'"
						// " LEFT JOIN TFOSMT10E B7 ON TO_CHAR(A.TIME,'YYYYMMDD') = B7.DATE_TIME AND B7.FACTORY_DIV = 'A20' AND B7.MAT_CODE = '3'"
						// " LEFT JOIN TFOSMT10E B8 ON TO_CHAR(A.TIME,'YYYYMMDD') = B8.DATE_TIME AND B8.FACTORY_DIV = 'A20' AND B8.MAT_CODE = '5'"
						// " LEFT JOIN TFOSMT10E C1 ON TO_CHAR(A.TIME,'YYYYMMDD') = C1.DATE_TIME AND C1.FACTORY_DIV = 'A00' AND C1.MAT_CODE = '1'"
						// " LEFT JOIN TFOSMT10E C2 ON TO_CHAR(A.TIME,'YYYYMMDD') = C2.DATE_TIME AND C2.FACTORY_DIV = 'A00' AND C2.MAT_CODE = '2'"
						// " LEFT JOIN TFOSMT10E C3 ON TO_CHAR(A.TIME,'YYYYMMDD') = C3.DATE_TIME AND C3.FACTORY_DIV = 'A00' AND C3.MAT_CODE = '3'"
						// " LEFT JOIN TFOSMT10E C4 ON TO_CHAR(A.TIME,'YYYYMMDD') = C4.DATE_TIME AND C4.FACTORY_DIV = 'A00' AND C4.MAT_CODE = '5'"
						// ;
// DM8 SQL：
					sqlstr =
						" WITH A(LEVEL, TIME) AS"
						" (SELECT 1, TO_DATE(TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMM') || '01','YYYYMMDD')"
						" FROM DUAL"
						" WHERE 1 = 1"
						" UNION ALL"
						" SELECT LEVEL + 1,TIME + 1"
						" FROM DUAL, A"
						" WHERE A.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD') - 1)"
						" SELECT '合计' DATE_TIME,"
						" ROUND(AVG(B1.RATE),2) RATE_1, ROUND(AVG(B2.RATE),2) RATE_2, ROUND(AVG(B3.RATE),2) RATE_3, ROUND(AVG(B4.RATE),2) RATE_4,"
						" ROUND(AVG(B5.RATE),2) RATE_5, ROUND(AVG(B6.RATE),2) RATE_6, ROUND(AVG(B7.RATE),2) RATE_7, ROUND(AVG(B8.RATE),2) RATE_8,"
						" ROUND(AVG(C1.RATE),2) RATE_9, ROUND(AVG(C2.RATE),2) RATE_10, ROUND(AVG(C3.RATE),2) RATE_11, ROUND(AVG(C4.RATE),2) RATE_12"
						" FROM A"
						" LEFT JOIN TFOSMT10E B1 ON TO_CHAR(A.TIME,'YYYYMMDD') = B1.DATE_TIME AND B1.FACTORY_DIV = 'A10' AND B1.MAT_CODE = '1'"
						" LEFT JOIN TFOSMT10E B2 ON TO_CHAR(A.TIME,'YYYYMMDD') = B2.DATE_TIME AND B2.FACTORY_DIV = 'A10' AND B2.MAT_CODE = '2'"
						" LEFT JOIN TFOSMT10E B3 ON TO_CHAR(A.TIME,'YYYYMMDD') = B3.DATE_TIME AND B3.FACTORY_DIV = 'A10' AND B3.MAT_CODE = '3'"
						" LEFT JOIN TFOSMT10E B4 ON TO_CHAR(A.TIME,'YYYYMMDD') = B4.DATE_TIME AND B4.FACTORY_DIV = 'A10' AND B4.MAT_CODE = '5'"
						" LEFT JOIN TFOSMT10E B5 ON TO_CHAR(A.TIME,'YYYYMMDD') = B5.DATE_TIME AND B5.FACTORY_DIV = 'A20' AND B5.MAT_CODE = '1'"
						" LEFT JOIN TFOSMT10E B6 ON TO_CHAR(A.TIME,'YYYYMMDD') = B6.DATE_TIME AND B6.FACTORY_DIV = 'A20' AND B6.MAT_CODE = '2'"
						" LEFT JOIN TFOSMT10E B7 ON TO_CHAR(A.TIME,'YYYYMMDD') = B7.DATE_TIME AND B7.FACTORY_DIV = 'A20' AND B7.MAT_CODE = '3'"
						" LEFT JOIN TFOSMT10E B8 ON TO_CHAR(A.TIME,'YYYYMMDD') = B8.DATE_TIME AND B8.FACTORY_DIV = 'A20' AND B8.MAT_CODE = '5'"
						" LEFT JOIN TFOSMT10E C1 ON TO_CHAR(A.TIME,'YYYYMMDD') = C1.DATE_TIME AND C1.FACTORY_DIV = 'A00' AND C1.MAT_CODE = '1'"
						" LEFT JOIN TFOSMT10E C2 ON TO_CHAR(A.TIME,'YYYYMMDD') = C2.DATE_TIME AND C2.FACTORY_DIV = 'A00' AND C2.MAT_CODE = '2'"
						" LEFT JOIN TFOSMT10E C3 ON TO_CHAR(A.TIME,'YYYYMMDD') = C3.DATE_TIME AND C3.FACTORY_DIV = 'A00' AND C3.MAT_CODE = '3'"
						" LEFT JOIN TFOSMT10E C4 ON TO_CHAR(A.TIME,'YYYYMMDD') = C4.DATE_TIME AND C4.FACTORY_DIV = 'A00' AND C4.MAT_CODE = '5'"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				EIClass blk;
				cmd_inq.ExecuteQuery(blk.Tables[0]);
				cmd_inq.Close();

				CDataRow& row = bcls_ret->Tables[tbl].Rows.Add();
				row.Merge(blk.Tables[0].Rows[0]);
			}
			else if (tbl == "TFOSMT10G")
			{
				//精炼生产情况
				bcls_ret->Tables.Add(tbl);
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-56：生成查询日期之前10天的日期序列;日期边界、参数与结果列保持不变。
// 改写原因：SYSIBM 辅助表改为 DUAL；DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " WITH A(LEVEL, TIME) AS"
						// " (SELECT 1, TO_DATE(@DATE_TIME,'YYYYMMDD') - 10 DAY"
						// " FROM SYSIBM.SYSDUMMY1"
						// " WHERE 1 = 1"
						// " UNION ALL"
						// " SELECT LEVEL + 1,TIME + 1 DAY"
						// " FROM SYSIBM.SYSDUMMY1, A"
						// " WHERE A.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY)"
						// " SELECT TO_CHAR(A.TIME,'YYYYMMDD') DATE_TIME, B1.PRODUCT_CHARGE PRODUCT_CHARGE_1,"
						// " B1.SMELT_CHARGE SMELT_CHARGE_L1, B1.RATE RATE_L1, B1.RATIO_NUM RATIO_NUM_L1, B1.TOTAL_DURATION TOTAL_DURATION_L1,"
						// " B2.SMELT_CHARGE SMELT_CHARGE_R1, B2.RATE RATE_R1,"
						// " B3.PRODUCT_CHARGE PRODUCT_CHARGE_2,"
						// " B3.SMELT_CHARGE SMELT_CHARGE_L2, B3.RATE RATE_L2, B3.RATIO_NUM RATIO_NUM_L2, B3.TOTAL_DURATION TOTAL_DURATION_L2,"
						// " B4.SMELT_CHARGE SMELT_CHARGE_R2, B4.RATE RATE_R2"
						// " FROM A"
						// " LEFT JOIN TFOSMT10F B1 ON TO_CHAR(A.TIME,'YYYYMMDD') = B1.DATE_TIME AND B1.FACTORY_DIV = 'A10' AND B1.STATION_ID = 'L'"
						// " LEFT JOIN TFOSMT10F B2 ON TO_CHAR(A.TIME,'YYYYMMDD') = B2.DATE_TIME AND B2.FACTORY_DIV = 'A10' AND B2.STATION_ID = 'R'"
						// " LEFT JOIN TFOSMT10F B3 ON TO_CHAR(A.TIME,'YYYYMMDD') = B3.DATE_TIME AND B3.FACTORY_DIV = 'A20' AND B3.STATION_ID = 'L'"
						// " LEFT JOIN TFOSMT10F B4 ON TO_CHAR(A.TIME,'YYYYMMDD') = B4.DATE_TIME AND B4.FACTORY_DIV = 'A20' AND B4.STATION_ID = 'R'"
						// ;
// DM8 SQL：
					sqlstr =
						" WITH A(LEVEL, TIME) AS"
						" (SELECT 1, TO_DATE(@DATE_TIME,'YYYYMMDD') - 10"
						" FROM DUAL"
						" WHERE 1 = 1"
						" UNION ALL"
						" SELECT LEVEL + 1,TIME + 1"
						" FROM DUAL, A"
						" WHERE A.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD') - 1)"
						" SELECT TO_CHAR(A.TIME,'YYYYMMDD') DATE_TIME, B1.PRODUCT_CHARGE PRODUCT_CHARGE_1,"
						" B1.SMELT_CHARGE SMELT_CHARGE_L1, B1.RATE RATE_L1, B1.RATIO_NUM RATIO_NUM_L1, B1.TOTAL_DURATION TOTAL_DURATION_L1,"
						" B2.SMELT_CHARGE SMELT_CHARGE_R1, B2.RATE RATE_R1,"
						" B3.PRODUCT_CHARGE PRODUCT_CHARGE_2,"
						" B3.SMELT_CHARGE SMELT_CHARGE_L2, B3.RATE RATE_L2, B3.RATIO_NUM RATIO_NUM_L2, B3.TOTAL_DURATION TOTAL_DURATION_L2,"
						" B4.SMELT_CHARGE SMELT_CHARGE_R2, B4.RATE RATE_R2"
						" FROM A"
						" LEFT JOIN TFOSMT10F B1 ON TO_CHAR(A.TIME,'YYYYMMDD') = B1.DATE_TIME AND B1.FACTORY_DIV = 'A10' AND B1.STATION_ID = 'L'"
						" LEFT JOIN TFOSMT10F B2 ON TO_CHAR(A.TIME,'YYYYMMDD') = B2.DATE_TIME AND B2.FACTORY_DIV = 'A10' AND B2.STATION_ID = 'R'"
						" LEFT JOIN TFOSMT10F B3 ON TO_CHAR(A.TIME,'YYYYMMDD') = B3.DATE_TIME AND B3.FACTORY_DIV = 'A20' AND B3.STATION_ID = 'L'"
						" LEFT JOIN TFOSMT10F B4 ON TO_CHAR(A.TIME,'YYYYMMDD') = B4.DATE_TIME AND B4.FACTORY_DIV = 'A20' AND B4.STATION_ID = 'R'"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();

				//处理合计行
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-57：生成查询日期之前1天的日期序列;日期边界、参数与结果列保持不变。
// 改写原因：SYSIBM 辅助表改为 DUAL；DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " WITH A(LEVEL, TIME) AS"
						// " (SELECT 1, TO_DATE(TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY,'YYYYMM') || '01','YYYYMMDD')"
						// " FROM SYSIBM.SYSDUMMY1"
						// " WHERE 1 = 1"
						// " UNION ALL"
						// " SELECT LEVEL + 1,TIME + 1 DAY"
						// " FROM SYSIBM.SYSDUMMY1, A"
						// " WHERE A.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY)"
						// " SELECT '合计' DATE_TIME, SUM(B1.PRODUCT_CHARGE) PRODUCT_CHARGE_1,"
						// " SUM(B1.SMELT_CHARGE) SMELT_CHARGE_L1, ROUND(100.0 * SUM(B1.SMELT_CHARGE) / SUM(B1.PRODUCT_CHARGE),2) RATE_L1,"
						// " ROUND(100.0 * SUM(0.01 * B1.RATIO_NUM * B1.PRODUCT_CHARGE) / SUM(B1.PRODUCT_CHARGE),2) RATIO_NUM_L1,"
						// " ROUND(AVG(B1.TOTAL_DURATION),1) TOTAL_DURATION_L1,"
						// " SUM(B2.SMELT_CHARGE) SMELT_CHARGE_R1, ROUND(100.0 * SUM(B2.SMELT_CHARGE) / SUM(B2.PRODUCT_CHARGE),2) RATE_R1,"
						// " SUM(B3.PRODUCT_CHARGE) PRODUCT_CHARGE_2,"
						// " SUM(B3.SMELT_CHARGE) SMELT_CHARGE_L2, ROUND(100.0 * SUM(B3.SMELT_CHARGE) / SUM(B3.PRODUCT_CHARGE),2) RATE_L2,"
						// " ROUND(100.0 * SUM(0.01 * B3.RATIO_NUM * B3.PRODUCT_CHARGE) / SUM(B3.PRODUCT_CHARGE),2) RATIO_NUM_L2,"
						// " ROUND(AVG(B3.TOTAL_DURATION),1) TOTAL_DURATION_L2,"
						// " SUM(B4.SMELT_CHARGE) SMELT_CHARGE_R2, ROUND(100.0 * SUM(B4.SMELT_CHARGE) / SUM(B4.PRODUCT_CHARGE),2) RATE_R2"
						// " FROM A"
						// " LEFT JOIN TFOSMT10F B1 ON TO_CHAR(A.TIME,'YYYYMMDD') = B1.DATE_TIME AND B1.FACTORY_DIV = 'A10' AND B1.STATION_ID = 'L'"
						// " LEFT JOIN TFOSMT10F B2 ON TO_CHAR(A.TIME,'YYYYMMDD') = B2.DATE_TIME AND B2.FACTORY_DIV = 'A10' AND B2.STATION_ID = 'R'"
						// " LEFT JOIN TFOSMT10F B3 ON TO_CHAR(A.TIME,'YYYYMMDD') = B3.DATE_TIME AND B3.FACTORY_DIV = 'A20' AND B3.STATION_ID = 'L'"
						// " LEFT JOIN TFOSMT10F B4 ON TO_CHAR(A.TIME,'YYYYMMDD') = B4.DATE_TIME AND B4.FACTORY_DIV = 'A20' AND B4.STATION_ID = 'R'"
						// ;
// DM8 SQL：
					sqlstr =
						" WITH A(LEVEL, TIME) AS"
						" (SELECT 1, TO_DATE(TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMM') || '01','YYYYMMDD')"
						" FROM DUAL"
						" WHERE 1 = 1"
						" UNION ALL"
						" SELECT LEVEL + 1,TIME + 1"
						" FROM DUAL, A"
						" WHERE A.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD') - 1)"
						" SELECT '合计' DATE_TIME, SUM(B1.PRODUCT_CHARGE) PRODUCT_CHARGE_1,"
						" SUM(B1.SMELT_CHARGE) SMELT_CHARGE_L1, ROUND(100.0 * SUM(B1.SMELT_CHARGE) / SUM(B1.PRODUCT_CHARGE),2) RATE_L1,"
						" ROUND(100.0 * SUM(0.01 * B1.RATIO_NUM * B1.PRODUCT_CHARGE) / SUM(B1.PRODUCT_CHARGE),2) RATIO_NUM_L1,"
						" ROUND(AVG(B1.TOTAL_DURATION),1) TOTAL_DURATION_L1,"
						" SUM(B2.SMELT_CHARGE) SMELT_CHARGE_R1, ROUND(100.0 * SUM(B2.SMELT_CHARGE) / SUM(B2.PRODUCT_CHARGE),2) RATE_R1,"
						" SUM(B3.PRODUCT_CHARGE) PRODUCT_CHARGE_2,"
						" SUM(B3.SMELT_CHARGE) SMELT_CHARGE_L2, ROUND(100.0 * SUM(B3.SMELT_CHARGE) / SUM(B3.PRODUCT_CHARGE),2) RATE_L2,"
						" ROUND(100.0 * SUM(0.01 * B3.RATIO_NUM * B3.PRODUCT_CHARGE) / SUM(B3.PRODUCT_CHARGE),2) RATIO_NUM_L2,"
						" ROUND(AVG(B3.TOTAL_DURATION),1) TOTAL_DURATION_L2,"
						" SUM(B4.SMELT_CHARGE) SMELT_CHARGE_R2, ROUND(100.0 * SUM(B4.SMELT_CHARGE) / SUM(B4.PRODUCT_CHARGE),2) RATE_R2"
						" FROM A"
						" LEFT JOIN TFOSMT10F B1 ON TO_CHAR(A.TIME,'YYYYMMDD') = B1.DATE_TIME AND B1.FACTORY_DIV = 'A10' AND B1.STATION_ID = 'L'"
						" LEFT JOIN TFOSMT10F B2 ON TO_CHAR(A.TIME,'YYYYMMDD') = B2.DATE_TIME AND B2.FACTORY_DIV = 'A10' AND B2.STATION_ID = 'R'"
						" LEFT JOIN TFOSMT10F B3 ON TO_CHAR(A.TIME,'YYYYMMDD') = B3.DATE_TIME AND B3.FACTORY_DIV = 'A20' AND B3.STATION_ID = 'L'"
						" LEFT JOIN TFOSMT10F B4 ON TO_CHAR(A.TIME,'YYYYMMDD') = B4.DATE_TIME AND B4.FACTORY_DIV = 'A20' AND B4.STATION_ID = 'R'"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				EIClass blk;
				cmd_inq.ExecuteQuery(blk.Tables[0]);
				cmd_inq.Close();

				CDataRow& row = bcls_ret->Tables[tbl].Rows.Add();
				row.Merge(blk.Tables[0].Rows[0]);
			}
			else if (tbl == "TFOSMT10H")
			{
				//铁坯比
				bcls_ret->Tables.Add(tbl);
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-58：生成查询日期之前3天的日期序列;日期边界、参数与结果列保持不变。
// 改写原因：SYSIBM 辅助表改为 DUAL；DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " WITH T(LEVEL, TIME) AS"
						// " (SELECT 1, TO_DATE(@DATE_TIME,'YYYYMMDD') - 3 DAY"
						// " FROM SYSIBM.SYSDUMMY1"
						// " WHERE 1 = 1"
						// " UNION ALL"
						// " SELECT LEVEL + 1,TIME + 1 DAY"
						// " FROM SYSIBM.SYSDUMMY1, T"
						// " WHERE T.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY)"
						// " SELECT TO_CHAR(T.TIME,'YYYYMMDD') DATE_TIME, '' AAA, SLAB_WT, TO_CHAR(CAST(MOLTIRON_WT AS DECIMAL(10,0))) MOLTIRON_WT, TO_CHAR(CAST(SCRAP_STEEL_TPC AS DECIMAL(10,0))) SCRAP_STEEL_TPC, MAT_WT, STOCK_TOTAL_WT,"
						// " TO_CHAR(IRON_STEEL_RATE1) IRON_STEEL_RATE1, IRON_STEEL_RATE2, TO_CHAR(DAYS) AS DAYS, RATE_FINISH_DAY"
						// " FROM T"
						// " LEFT JOIN TFOSMT10G A ON A.DATE_TIME = TO_CHAR(T.TIME,'YYYYMMDD')"
						// " WHERE 1 = 1"
						// ;
// DM8 SQL：
					sqlstr =
						" WITH T(LEVEL, TIME) AS"
						" (SELECT 1, TO_DATE(@DATE_TIME,'YYYYMMDD') - 3"
						" FROM DUAL"
						" WHERE 1 = 1"
						" UNION ALL"
						" SELECT LEVEL + 1,TIME + 1"
						" FROM DUAL, T"
						" WHERE T.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD') - 1)"
						" SELECT TO_CHAR(T.TIME,'YYYYMMDD') DATE_TIME, '' AAA, SLAB_WT, TO_CHAR(CAST(MOLTIRON_WT AS DECIMAL(10,0))) MOLTIRON_WT, TO_CHAR(CAST(SCRAP_STEEL_TPC AS DECIMAL(10,0))) SCRAP_STEEL_TPC, MAT_WT, STOCK_TOTAL_WT,"
						" TO_CHAR(IRON_STEEL_RATE1) IRON_STEEL_RATE1, IRON_STEEL_RATE2, TO_CHAR(DAYS) AS DAYS, RATE_FINISH_DAY"
						" FROM T"
						" LEFT JOIN TFOSMT10G A ON A.DATE_TIME = TO_CHAR(T.TIME,'YYYYMMDD')"
						" WHERE 1 = 1"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();

				//处理合计行
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-59：取查询日期前推1日的日期字符串;日历日边界、参数与结果列保持不变。
// 改写原因：DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " SELECT"
						// " '月度合计' DATE_TIME, '' AAA,"
						// " SLAB_WT, TO_CHAR(CAST(MOLTIRON_WT AS DECIMAL(10,0))) MOLTIRON_WT, TO_CHAR(CAST(SCRAP_STEEL_TPC AS DECIMAL(10,0))) SCRAP_STEEL_TPC,"
						// " MAT_WT, STOCK_TOTAL_WT,"
						// " TO_CHAR(IRON_STEEL_RATE1) IRON_STEEL_RATE1,"
						// " IRON_STEEL_RATE2,"
						// " TO_CHAR(A.DAYS) AS DAYS, RATE_FINISH_DAY"
						// " FROM TFOSMT10H A"
						// " WHERE DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS,'YYYYMMDD')"
						// ;
// DM8 SQL：
					sqlstr =
						" SELECT"
						" '月度合计' DATE_TIME, '' AAA,"
						" SLAB_WT, TO_CHAR(CAST(MOLTIRON_WT AS DECIMAL(10,0))) MOLTIRON_WT, TO_CHAR(CAST(SCRAP_STEEL_TPC AS DECIMAL(10,0))) SCRAP_STEEL_TPC,"
						" MAT_WT, STOCK_TOTAL_WT,"
						" TO_CHAR(IRON_STEEL_RATE1) IRON_STEEL_RATE1,"
						" IRON_STEEL_RATE2,"
						" TO_CHAR(A.DAYS) AS DAYS, RATE_FINISH_DAY"
						" FROM TFOSMT10H A"
						" WHERE DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMMDD')"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				EIClass blk;
				cmd_inq.ExecuteQuery(blk.Tables[0]);
				cmd_inq.Close();

				CDataRow& row = bcls_ret->Tables[tbl].Rows.Add();
				row.Merge(blk.Tables[0].Rows[0]);

				//年度累计
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-60：取查询日期前推1日的日期字符串;日历日边界、参数与结果列保持不变。
// 改写原因：DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " SELECT '年度累计（万吨）' DATE_TIME, '' AAA,"
						// " ROUND(A.SLAB_WT / 10000,2) + TO_NUMBER(B.REMARK) SLAB_WT,"
						// " TO_CHAR(CAST(ROUND(A.MOLTIRON_WT / 10000,2) + TO_NUMBER(C.REMARK) AS DECIMAL(10,2))) MOLTIRON_WT,"
						// " TO_CHAR(CAST(ROUND(A.SCRAP_STEEL_TPC / 10000,2) + TO_NUMBER(D.REMARK) AS DECIMAL(10,2))) SCRAP_STEEL_TPC,"
						// " ROUND(A.MAT_WT / 10000,2) + TO_NUMBER(E.REMARK) MAT_WT,"
						// " ROUND(A.STOCK_TOTAL_WT / 10000,2) + TO_NUMBER(F.REMARK) STOCK_TOTAL_WT,"
						// " TO_CHAR(CAST(ROUND(1.0000 * (ROUND(A.SLAB_WT / 10000,2) + TO_NUMBER(B.REMARK))"
						// " / (ROUND(A.MAT_WT / 10000,2) + TO_NUMBER(E.REMARK)),4) AS DECIMAL(5,4))) IRON_STEEL_RATE1,"
						// " CAST(ROUND(1.0000 * ROUND(T.MOLTIRON_WT1 / 10000,2)"
						// " / (ROUND(A.STOCK_TOTAL_WT / 10000,2) + TO_NUMBER(F.REMARK)),4) AS DECIMAL(5,4)) IRON_STEEL_RATE2,"
						// " TO_CHAR(A.DAYS) AS DAYS, A.RATE_FINISH_DAY"
						// " FROM TFOSMT10H A"
						// " LEFT JOIN TFOSMT96B B ON B.ITEM_ENAME = 'FOSMT10-603' AND B.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS,'YYYYMM')"
						// " LEFT JOIN TFOSMT96B C ON C.ITEM_ENAME = 'FOSMT10-604' AND C.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS,'YYYYMM')"
						// " LEFT JOIN TFOSMT96B D ON D.ITEM_ENAME = 'FOSMT10-605' AND D.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS,'YYYYMM')"
						// " LEFT JOIN TFOSMT96B E ON E.ITEM_ENAME = 'FOSMT10-606' AND E.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS,'YYYYMM')"
						// " LEFT JOIN TFOSMT96B F ON F.ITEM_ENAME = 'FOSMT10-607' AND F.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS,'YYYYMM')"
						// " LEFT JOIN (SELECT SUM(MOLTIRON_WT1) MOLTIRON_WT1 FROM TFOSMT10G WHERE DATE_TIME < @DATE_TIME"
						// " AND DATE_TIME >= TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS,'YYYY') || '0101') T ON 1 = 1"
						// " WHERE A.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS,'YYYYMMDD')"
						// ;
// DM8 SQL：
					sqlstr =
						" SELECT '年度累计（万吨）' DATE_TIME, '' AAA,"
						" ROUND(A.SLAB_WT / 10000,2) + TO_NUMBER(B.REMARK) SLAB_WT,"
						" TO_CHAR(CAST(ROUND(A.MOLTIRON_WT / 10000,2) + TO_NUMBER(C.REMARK) AS DECIMAL(10,2))) MOLTIRON_WT,"
						" TO_CHAR(CAST(ROUND(A.SCRAP_STEEL_TPC / 10000,2) + TO_NUMBER(D.REMARK) AS DECIMAL(10,2))) SCRAP_STEEL_TPC,"
						" ROUND(A.MAT_WT / 10000,2) + TO_NUMBER(E.REMARK) MAT_WT,"
						" ROUND(A.STOCK_TOTAL_WT / 10000,2) + TO_NUMBER(F.REMARK) STOCK_TOTAL_WT,"
						" TO_CHAR(CAST(ROUND(1.0000 * (ROUND(A.SLAB_WT / 10000,2) + TO_NUMBER(B.REMARK))"
						" / (ROUND(A.MAT_WT / 10000,2) + TO_NUMBER(E.REMARK)),4) AS DECIMAL(5,4))) IRON_STEEL_RATE1,"
						" CAST(ROUND(1.0000 * ROUND(T.MOLTIRON_WT1 / 10000,2)"
						" / (ROUND(A.STOCK_TOTAL_WT / 10000,2) + TO_NUMBER(F.REMARK)),4) AS DECIMAL(5,4)) IRON_STEEL_RATE2,"
						" TO_CHAR(A.DAYS) AS DAYS, A.RATE_FINISH_DAY"
						" FROM TFOSMT10H A"
						" LEFT JOIN TFOSMT96B B ON B.ITEM_ENAME = 'FOSMT10-603' AND B.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMM')"
						" LEFT JOIN TFOSMT96B C ON C.ITEM_ENAME = 'FOSMT10-604' AND C.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMM')"
						" LEFT JOIN TFOSMT96B D ON D.ITEM_ENAME = 'FOSMT10-605' AND D.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMM')"
						" LEFT JOIN TFOSMT96B E ON E.ITEM_ENAME = 'FOSMT10-606' AND E.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMM')"
						" LEFT JOIN TFOSMT96B F ON F.ITEM_ENAME = 'FOSMT10-607' AND F.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMM')"
						" LEFT JOIN (SELECT SUM(MOLTIRON_WT1) MOLTIRON_WT1 FROM TFOSMT10G WHERE DATE_TIME < @DATE_TIME"
						" AND DATE_TIME >= TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYY') || '0101') T ON 1 = 1"
						" WHERE A.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMMDD')"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				EIClass blk2;
				cmd_inq.ExecuteQuery(blk2.Tables[0]);
				cmd_inq.Close();

				CDataRow& row2 = bcls_ret->Tables[tbl].Rows.Add();
				row2.Merge(blk2.Tables[0].Rows[0]);

				//年产进度
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-61：取查询日期前推1日的日期字符串;日历日边界、参数与结果列保持不变。
// 改写原因：SYSIBM 辅助表改为 DUAL；DB2 日期天数后缀改为整数天运算；ROWNUM 别名改为 RN,避免与 DM 伪列关键字冲突；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " SELECT DATE_TIME,AAA,SLAB_WT,MOLTIRON_WT,SCRAP_STEEL_TPC,STOCK_TOTAL_WT,IRON_STEEL_RATE1,IRON_STEEL_RATE2,T.DAYS,RATE_FINISH_DAY FROM"
						// " (SELECT '1' ROWNUM,"
						// " '年产进度' DATE_TIME, '' AAA,"
						// " 0 SLAB_WT,"
						// " '' MOLTIRON_WT, '' SCRAP_STEEL_TPC,"
						// " 0 STOCK_TOTAL_WT,"
						// " '比月度目标' IRON_STEEL_RATE1,"
						// " 0 IRON_STEEL_RATE2,"
						// " '比年产目标' AS DAYS, 0 RATE_FINISH_DAY"
						// " FROM (SELECT * FROM SYSIBM.DUAL)"
						// " UNION ALL"
						// " SELECT '2','铁厂','钢厂',0,'','',0,'',0,'',0 FROM SYSIBM.DUAL"
						// " UNION ALL"
						// " SELECT '3',B1.REMARK,B2.REMARK,0,'','',0,'',0,'',0 FROM SYSIBM.DUAL"
						// " LEFT JOIN TFOSMT96B B1 ON B1.ITEM_ENAME = 'FOSMT10-601' AND B1.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS,'YYYY')"
						// " LEFT JOIN TFOSMT96B B2 ON B2.ITEM_ENAME = 'FOSMT10-602' AND B2.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS,'YYYY')"
						// " ) T"
						// " ORDER BY ROWNUM"
						// ;
// DM8 SQL：
					sqlstr =
						" SELECT DATE_TIME,AAA,SLAB_WT,MOLTIRON_WT,SCRAP_STEEL_TPC,STOCK_TOTAL_WT,IRON_STEEL_RATE1,IRON_STEEL_RATE2,T.DAYS,RATE_FINISH_DAY FROM"
						" (SELECT '1' RN,"
						" '年产进度' DATE_TIME, '' AAA,"
						" 0 SLAB_WT,"
						" '' MOLTIRON_WT, '' SCRAP_STEEL_TPC,"
						" 0 STOCK_TOTAL_WT,"
						" '比月度目标' IRON_STEEL_RATE1,"
						" 0 IRON_STEEL_RATE2,"
						" '比年产目标' AS DAYS, 0 RATE_FINISH_DAY"
						" FROM (SELECT * FROM DUAL)"
						" UNION ALL"
						" SELECT '2','铁厂','钢厂',0,'','',0,'',0,'',0 FROM DUAL"
						" UNION ALL"
						" SELECT '3',B1.REMARK,B2.REMARK,0,'','',0,'',0,'',0 FROM DUAL"
						" LEFT JOIN TFOSMT96B B1 ON B1.ITEM_ENAME = 'FOSMT10-601' AND B1.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYY')"
						" LEFT JOIN TFOSMT96B B2 ON B2.ITEM_ENAME = 'FOSMT10-602' AND B2.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYY')"
						" ) T"
						" ORDER BY RN"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				EIClass blk3;
				cmd_inq.ExecuteQuery(blk3.Tables[0]);
				cmd_inq.Close();

				CDataRow& row3 = bcls_ret->Tables[tbl].Rows.Add();
				row3.Merge(blk3.Tables[0].Rows[0]);
				CDataRow& row4 = bcls_ret->Tables[tbl].Rows.Add();
				row4.Merge(blk3.Tables[0].Rows[1]);
				CDataRow& row5 = bcls_ret->Tables[tbl].Rows.Add();
				row5.Merge(blk3.Tables[0].Rows[2]);
				row3["SLAB_WT"] = (bcls_ret->Tables[tbl].Rows[4]["SLAB_WT"].ToDecimal() / row5["DATE_TIME"].ToDecimal() * 100).Round(2);
				row3["MAT_WT"] = (bcls_ret->Tables[tbl].Rows[4]["MAT_WT"].ToDecimal() / row5["AAA"].ToDecimal() * 100).Round(2);
				row3["STOCK_TOTAL_WT"] = (bcls_ret->Tables[tbl].Rows[4]["STOCK_TOTAL_WT"].ToDecimal() / row5["AAA"].ToDecimal() * 100).Round(2);

				//比年产目标
				row3["RATE_FINISH_DAY"] = ((bcls_ret->Tables[tbl].Rows[4]["STOCK_TOTAL_WT"].ToDecimal() / row5["AAA"].ToDecimal()
					- bcls_ret->Tables[tbl].Rows[4]["RATE_FINISH_DAY"].ToDecimal() / 100) * row5["AAA"].ToDecimal()).Round(2);
				//比月度目标
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-62：取查询日期前推1日的日期字符串;日历日边界、参数与结果列保持不变。
// 改写原因：DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " SELECT ROUND((ROUND((B.SLAB_WT / C.REMARK) * 100,2) - A.TIME_PROGRESS) * C.REMARK / 100,2)"
						// " FROM TFOSMT03A A"
						// " LEFT JOIN (SELECT ROUND((0.999 * (SUM(SLAB_WT_1) + SUM(SLAB_WT_2))) / 10000,2) SLAB_WT FROM TFOSMT03A"
						// " WHERE DATE_TIME >= TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS,'YYYYMM') || '01' AND DATE_TIME <= TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS,'YYYYMMDD')) B ON 1 = 1"
						// " LEFT JOIN (SELECT SUM(TO_NUMBER(REMARK)) / 10000 REMARK FROM TFOSMT96B"
						// " WHERE DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS,'YYYYMM') AND ITEM_ENAME IN ('FOSMT03-107','FOSMT03-108'))"
						// " C ON 1 = 1"
						// " WHERE A.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS,'YYYYMMDD')"
						// ;
// DM8 SQL：
					sqlstr =
						" SELECT ROUND((ROUND((B.SLAB_WT / C.REMARK) * 100,2) - A.TIME_PROGRESS) * C.REMARK / 100,2)"
						" FROM TFOSMT03A A"
						" LEFT JOIN (SELECT ROUND((0.999 * (SUM(SLAB_WT_1) + SUM(SLAB_WT_2))) / 10000,2) SLAB_WT FROM TFOSMT03A"
						" WHERE DATE_TIME >= TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMM') || '01' AND DATE_TIME <= TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMMDD')) B ON 1 = 1"
						" LEFT JOIN (SELECT SUM(TO_NUMBER(REMARK)) / 10000 REMARK FROM TFOSMT96B"
						" WHERE DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMM') AND ITEM_ENAME IN ('FOSMT03-107','FOSMT03-108'))"
						" C ON 1 = 1"
						" WHERE A.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1,'YYYYMMDD')"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					row3["IRON_STEEL_RATE2"] = cmd_inq.GetDecimal(1).Round(2);
				}
				cmd_inq.Close();
			}
			else if (tbl == "TFOSMT10J")
			{
				//单位成本
				bcls_ret->Tables.Add(tbl);
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-63：取查询日期前推7日的日期字符串;日历日边界、参数与结果列保持不变。
// 改写原因：DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " SELECT A.CODE, A2.CODE FACTORY_DIV, A.CODE_DESC_1_CONTENT ITEM_CNAME,"
						// " COALESCE(C.UNIT_COST, 0) REMARK,"
						// " COALESCE(B1.UNIT_COST, 0) REMARK_1,"
						// " COALESCE(B2.UNIT_COST, 0) REMARK_2,"
						// " COALESCE(B3.UNIT_COST, 0) REMARK_3,"
						// " COALESCE(B4.UNIT_COST, 0) REMARK_4,"
						// " COALESCE(B5.UNIT_COST, 0) REMARK_5,"
						// " COALESCE(B6.UNIT_COST, 0) REMARK_6,"
						// " COALESCE(B7.UNIT_COST, 0) REMARK_7,"
						// " COALESCE(ROUND(D.REMARK_MON, 2), 0) REMARK_MON,"
						// " COALESCE(ROUND(E.REMARK_YEAR, 2), 0) REMARK_YEAR"
						// " FROM TEP0002 A"
						// " LEFT JOIN TEP0002 A2 ON A2.CODE_CLASS = 'FOSMT5' AND A2.CODE IN ('A10','A20')"
						// " LEFT JOIN TFOSMT10J B1 ON A.CODE = B1.ITEM_ENAME AND B1.FACTORY_DIV = A2.CODE AND B1.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 7 DAY, 'YYYYMMDD')"
						// " LEFT JOIN TFOSMT10J B2 ON A.CODE = B2.ITEM_ENAME AND B2.FACTORY_DIV = A2.CODE AND B2.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 6 DAY, 'YYYYMMDD')"
						// " LEFT JOIN TFOSMT10J B3 ON A.CODE = B3.ITEM_ENAME AND B3.FACTORY_DIV = A2.CODE AND B3.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 5 DAY, 'YYYYMMDD')"
						// " LEFT JOIN TFOSMT10J B4 ON A.CODE = B4.ITEM_ENAME AND B4.FACTORY_DIV = A2.CODE AND B4.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 4 DAY, 'YYYYMMDD')"
						// " LEFT JOIN TFOSMT10J B5 ON A.CODE = B5.ITEM_ENAME AND B5.FACTORY_DIV = A2.CODE AND B5.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 3 DAY, 'YYYYMMDD')"
						// " LEFT JOIN TFOSMT10J B6 ON A.CODE = B6.ITEM_ENAME AND B6.FACTORY_DIV = A2.CODE AND B6.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 2 DAY, 'YYYYMMDD')"
						// " LEFT JOIN TFOSMT10J B7 ON A.CODE = B7.ITEM_ENAME AND B7.FACTORY_DIV = A2.CODE AND B7.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY, 'YYYYMMDD')"
						// " LEFT JOIN TFOSMT10J C ON A.CODE = C.ITEM_ENAME AND C.FACTORY_DIV = A2.CODE AND C.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY, 'YYYYMM')"
						// " LEFT JOIN ("
						// " SELECT ITEM_ENAME, FACTORY_DIV, SUM(TO_NUMBER(REMARK1)), SUM(TO_NUMBER(REMARK2)),"
						// " CASE WHEN ITEM_ENAME = '06' THEN"
						// " DECODE(SUM(TO_NUMBER(REMARK2)), 0, 0, SUM(TO_NUMBER(REMARK1)) / SUM(TO_NUMBER(REMARK2)))"
						// " - DECODE(SUM(TO_NUMBER(REMARK4)), 0, 0, SUM(TO_NUMBER(REMARK3)) / SUM(TO_NUMBER(REMARK4)))"
						// " - DECODE(SUM(TO_NUMBER(REMARK6)), 0, 0, SUM(TO_NUMBER(REMARK5)) / SUM(TO_NUMBER(REMARK6)))"
						// " WHEN ITEM_ENAME = '01' THEN"
						// " DECODE(SUM(TO_NUMBER(REMARK2)), 0, 0, SUM(TO_NUMBER(REMARK1)) / SUM(TO_NUMBER(REMARK2)))"
						// " - DECODE(SUM(TO_NUMBER(REMARK4)), 0, 0, SUM(TO_NUMBER(REMARK3)) / SUM(TO_NUMBER(REMARK4)))"
						// " ELSE DECODE(SUM(TO_NUMBER(REMARK2)), 0, 0, SUM(TO_NUMBER(REMARK1)) / SUM(TO_NUMBER(REMARK2)))"
						// " END REMARK_MON"
						// " FROM TFOSMT10J"
						// " WHERE DATE_TIME <= TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY, 'YYYYMMDD')"
						// " AND DATE_TIME >= TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY, 'YYYYMM') || '01'"
						// " AND LENGTH(DATE_TIME) = 8"
						// " GROUP BY ITEM_ENAME, FACTORY_DIV"
						// " ) D ON A.CODE = D.ITEM_ENAME AND D.FACTORY_DIV = A2.CODE"
						// " LEFT JOIN ("
						// " SELECT ITEM_ENAME, FACTORY_DIV, SUM(TO_NUMBER(REMARK1)), SUM(TO_NUMBER(REMARK2)),"
						// " CASE WHEN ITEM_ENAME = '06' THEN"
						// " DECODE(SUM(TO_NUMBER(REMARK2)), 0, 0, SUM(TO_NUMBER(REMARK1)) / SUM(TO_NUMBER(REMARK2)))"
						// " - DECODE(SUM(TO_NUMBER(REMARK4)), 0, 0, SUM(TO_NUMBER(REMARK3)) / SUM(TO_NUMBER(REMARK4)))"
						// " - DECODE(SUM(TO_NUMBER(REMARK6)), 0, 0, SUM(TO_NUMBER(REMARK5)) / SUM(TO_NUMBER(REMARK6)))"
						// " WHEN ITEM_ENAME = '01' THEN"
						// " DECODE(SUM(TO_NUMBER(REMARK2)), 0, 0, SUM(TO_NUMBER(REMARK1)) / SUM(TO_NUMBER(REMARK2)))"
						// " - DECODE(SUM(TO_NUMBER(REMARK4)), 0, 0, SUM(TO_NUMBER(REMARK3)) / SUM(TO_NUMBER(REMARK4)))"
						// " ELSE DECODE(SUM(TO_NUMBER(REMARK2)), 0, 0, SUM(TO_NUMBER(REMARK1)) / SUM(TO_NUMBER(REMARK2)))"
						// " END REMARK_YEAR"
						// " FROM TFOSMT10J"
						// " WHERE DATE_TIME <= TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY, 'YYYYMMDD')"
						// " AND DATE_TIME >= TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY, 'YYYY') || '0101'"
						// " AND LENGTH(DATE_TIME) = 8"
						// " GROUP BY ITEM_ENAME, FACTORY_DIV"
						// " ) E ON A.CODE = E.ITEM_ENAME AND E.FACTORY_DIV = A2.CODE"
						// " WHERE A.CODE_CLASS = 'FOSMTD' AND TRIM(A.CODE_DESC_2_CONTENT) <> ''"
						// " ORDER BY A2.CODE, A.CODE_DESC_2_CONTENT, A.CODE"
						// ;
// DM8 SQL：
					sqlstr =
						" SELECT A.CODE, A2.CODE FACTORY_DIV, A.CODE_DESC_1_CONTENT ITEM_CNAME,"
						" COALESCE(C.UNIT_COST, 0) REMARK,"
						" COALESCE(B1.UNIT_COST, 0) REMARK_1,"
						" COALESCE(B2.UNIT_COST, 0) REMARK_2,"
						" COALESCE(B3.UNIT_COST, 0) REMARK_3,"
						" COALESCE(B4.UNIT_COST, 0) REMARK_4,"
						" COALESCE(B5.UNIT_COST, 0) REMARK_5,"
						" COALESCE(B6.UNIT_COST, 0) REMARK_6,"
						" COALESCE(B7.UNIT_COST, 0) REMARK_7,"
						" COALESCE(ROUND(D.REMARK_MON, 2), 0) REMARK_MON,"
						" COALESCE(ROUND(E.REMARK_YEAR, 2), 0) REMARK_YEAR"
						" FROM TEP0002 A"
						" LEFT JOIN TEP0002 A2 ON A2.CODE_CLASS = 'FOSMT5' AND A2.CODE IN ('A10','A20')"
						" LEFT JOIN TFOSMT10J B1 ON A.CODE = B1.ITEM_ENAME AND B1.FACTORY_DIV = A2.CODE AND B1.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 7, 'YYYYMMDD')"
						" LEFT JOIN TFOSMT10J B2 ON A.CODE = B2.ITEM_ENAME AND B2.FACTORY_DIV = A2.CODE AND B2.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 6, 'YYYYMMDD')"
						" LEFT JOIN TFOSMT10J B3 ON A.CODE = B3.ITEM_ENAME AND B3.FACTORY_DIV = A2.CODE AND B3.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 5, 'YYYYMMDD')"
						" LEFT JOIN TFOSMT10J B4 ON A.CODE = B4.ITEM_ENAME AND B4.FACTORY_DIV = A2.CODE AND B4.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 4, 'YYYYMMDD')"
						" LEFT JOIN TFOSMT10J B5 ON A.CODE = B5.ITEM_ENAME AND B5.FACTORY_DIV = A2.CODE AND B5.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 3, 'YYYYMMDD')"
						" LEFT JOIN TFOSMT10J B6 ON A.CODE = B6.ITEM_ENAME AND B6.FACTORY_DIV = A2.CODE AND B6.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 2, 'YYYYMMDD')"
						" LEFT JOIN TFOSMT10J B7 ON A.CODE = B7.ITEM_ENAME AND B7.FACTORY_DIV = A2.CODE AND B7.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1, 'YYYYMMDD')"
						" LEFT JOIN TFOSMT10J C ON A.CODE = C.ITEM_ENAME AND C.FACTORY_DIV = A2.CODE AND C.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1, 'YYYYMM')"
						" LEFT JOIN ("
						" SELECT ITEM_ENAME, FACTORY_DIV, SUM(TO_NUMBER(REMARK1)), SUM(TO_NUMBER(REMARK2)),"
						" CASE WHEN ITEM_ENAME = '06' THEN"
						" DECODE(SUM(TO_NUMBER(REMARK2)), 0, 0, SUM(TO_NUMBER(REMARK1)) / SUM(TO_NUMBER(REMARK2)))"
						" - DECODE(SUM(TO_NUMBER(REMARK4)), 0, 0, SUM(TO_NUMBER(REMARK3)) / SUM(TO_NUMBER(REMARK4)))"
						" - DECODE(SUM(TO_NUMBER(REMARK6)), 0, 0, SUM(TO_NUMBER(REMARK5)) / SUM(TO_NUMBER(REMARK6)))"
						" WHEN ITEM_ENAME = '01' THEN"
						" DECODE(SUM(TO_NUMBER(REMARK2)), 0, 0, SUM(TO_NUMBER(REMARK1)) / SUM(TO_NUMBER(REMARK2)))"
						" - DECODE(SUM(TO_NUMBER(REMARK4)), 0, 0, SUM(TO_NUMBER(REMARK3)) / SUM(TO_NUMBER(REMARK4)))"
						" ELSE DECODE(SUM(TO_NUMBER(REMARK2)), 0, 0, SUM(TO_NUMBER(REMARK1)) / SUM(TO_NUMBER(REMARK2)))"
						" END REMARK_MON"
						" FROM TFOSMT10J"
						" WHERE DATE_TIME <= TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1, 'YYYYMMDD')"
						" AND DATE_TIME >= TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1, 'YYYYMM') || '01'"
						" AND LENGTH(DATE_TIME) = 8"
						" GROUP BY ITEM_ENAME, FACTORY_DIV"
						" ) D ON A.CODE = D.ITEM_ENAME AND D.FACTORY_DIV = A2.CODE"
						" LEFT JOIN ("
						" SELECT ITEM_ENAME, FACTORY_DIV, SUM(TO_NUMBER(REMARK1)), SUM(TO_NUMBER(REMARK2)),"
						" CASE WHEN ITEM_ENAME = '06' THEN"
						" DECODE(SUM(TO_NUMBER(REMARK2)), 0, 0, SUM(TO_NUMBER(REMARK1)) / SUM(TO_NUMBER(REMARK2)))"
						" - DECODE(SUM(TO_NUMBER(REMARK4)), 0, 0, SUM(TO_NUMBER(REMARK3)) / SUM(TO_NUMBER(REMARK4)))"
						" - DECODE(SUM(TO_NUMBER(REMARK6)), 0, 0, SUM(TO_NUMBER(REMARK5)) / SUM(TO_NUMBER(REMARK6)))"
						" WHEN ITEM_ENAME = '01' THEN"
						" DECODE(SUM(TO_NUMBER(REMARK2)), 0, 0, SUM(TO_NUMBER(REMARK1)) / SUM(TO_NUMBER(REMARK2)))"
						" - DECODE(SUM(TO_NUMBER(REMARK4)), 0, 0, SUM(TO_NUMBER(REMARK3)) / SUM(TO_NUMBER(REMARK4)))"
						" ELSE DECODE(SUM(TO_NUMBER(REMARK2)), 0, 0, SUM(TO_NUMBER(REMARK1)) / SUM(TO_NUMBER(REMARK2)))"
						" END REMARK_YEAR"
						" FROM TFOSMT10J"
						" WHERE DATE_TIME <= TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1, 'YYYYMMDD')"
						" AND DATE_TIME >= TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1, 'YYYY') || '0101'"
						" AND LENGTH(DATE_TIME) = 8"
						" GROUP BY ITEM_ENAME, FACTORY_DIV"
						" ) E ON A.CODE = E.ITEM_ENAME AND E.FACTORY_DIV = A2.CODE"
						" WHERE A.CODE_CLASS = 'FOSMTD' AND TRIM(A.CODE_DESC_2_CONTENT) <> ''"
						" ORDER BY A2.CODE, A.CODE_DESC_2_CONTENT, A.CODE"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();

				//综合
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-64：取查询日期前推7日的日期字符串;日历日边界、参数与结果列保持不变。
// 改写原因：DB2 日期天数后缀改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " SELECT A.CODE, A2.CODE FACTORY_DIV, A.CODE_DESC_1_CONTENT ITEM_CNAME,"
						// " COALESCE(C.UNIT_COST, 0) REMARK,"
						// " COALESCE(B1.UNIT_COST, 0) REMARK_1,"
						// " COALESCE(B2.UNIT_COST, 0) REMARK_2,"
						// " COALESCE(B3.UNIT_COST, 0) REMARK_3,"
						// " COALESCE(B4.UNIT_COST, 0) REMARK_4,"
						// " COALESCE(B5.UNIT_COST, 0) REMARK_5,"
						// " COALESCE(B6.UNIT_COST, 0) REMARK_6,"
						// " COALESCE(B7.UNIT_COST, 0) REMARK_7,"
						// " COALESCE(ROUND(D.REMARK_MON, 2), 0) REMARK_MON,"
						// " COALESCE(ROUND(E.REMARK_YEAR, 2), 0) REMARK_YEAR"
						// " FROM TEP0002 A"
						// " LEFT JOIN TEP0002 A2 ON A2.CODE_CLASS = 'FOSMT5' AND A2.CODE = 'A00'"
						// " LEFT JOIN TFOSMT10J B1 ON A.CODE = B1.ITEM_ENAME AND B1.FACTORY_DIV = A2.CODE AND B1.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 7 DAY, 'YYYYMMDD')"
						// " LEFT JOIN TFOSMT10J B2 ON A.CODE = B2.ITEM_ENAME AND B2.FACTORY_DIV = A2.CODE AND B2.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 6 DAY, 'YYYYMMDD')"
						// " LEFT JOIN TFOSMT10J B3 ON A.CODE = B3.ITEM_ENAME AND B3.FACTORY_DIV = A2.CODE AND B3.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 5 DAY, 'YYYYMMDD')"
						// " LEFT JOIN TFOSMT10J B4 ON A.CODE = B4.ITEM_ENAME AND B4.FACTORY_DIV = A2.CODE AND B4.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 4 DAY, 'YYYYMMDD')"
						// " LEFT JOIN TFOSMT10J B5 ON A.CODE = B5.ITEM_ENAME AND B5.FACTORY_DIV = A2.CODE AND B5.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 3 DAY, 'YYYYMMDD')"
						// " LEFT JOIN TFOSMT10J B6 ON A.CODE = B6.ITEM_ENAME AND B6.FACTORY_DIV = A2.CODE AND B6.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 2 DAY, 'YYYYMMDD')"
						// " LEFT JOIN TFOSMT10J B7 ON A.CODE = B7.ITEM_ENAME AND B7.FACTORY_DIV = A2.CODE AND B7.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY, 'YYYYMMDD')"
						// " LEFT JOIN TFOSMT10J C ON A.CODE = C.ITEM_ENAME AND C.FACTORY_DIV = A2.CODE AND C.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY, 'YYYYMM')"
						// " LEFT JOIN ("
						// " SELECT ITEM_ENAME, FACTORY_DIV, SUM(TO_NUMBER(REMARK1)), SUM(TO_NUMBER(REMARK2)),"
						// " CASE WHEN ITEM_ENAME = '06' THEN"
						// " DECODE(SUM(TO_NUMBER(REMARK2)), 0, 0, SUM(TO_NUMBER(REMARK1)) / SUM(TO_NUMBER(REMARK2)))"
						// " - DECODE(SUM(TO_NUMBER(REMARK4)), 0, 0, SUM(TO_NUMBER(REMARK3)) / SUM(TO_NUMBER(REMARK4)))"
						// " - DECODE(SUM(TO_NUMBER(REMARK6)), 0, 0, SUM(TO_NUMBER(REMARK5)) / SUM(TO_NUMBER(REMARK6)))"
						// " WHEN ITEM_ENAME = '01' THEN"
						// " DECODE(SUM(TO_NUMBER(REMARK2)), 0, 0, SUM(TO_NUMBER(REMARK1)) / SUM(TO_NUMBER(REMARK2)))"
						// " - DECODE(SUM(TO_NUMBER(REMARK4)), 0, 0, SUM(TO_NUMBER(REMARK3)) / SUM(TO_NUMBER(REMARK4)))"
						// " ELSE DECODE(SUM(TO_NUMBER(REMARK2)), 0, 0, SUM(TO_NUMBER(REMARK1)) / SUM(TO_NUMBER(REMARK2)))"
						// " END REMARK_MON"
						// " FROM TFOSMT10J"
						// " WHERE DATE_TIME <= TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY, 'YYYYMMDD')"
						// " AND DATE_TIME >= TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY, 'YYYYMM') || '01'"
						// " AND LENGTH(DATE_TIME) = 8"
						// " GROUP BY ITEM_ENAME, FACTORY_DIV"
						// " ) D ON A.CODE = D.ITEM_ENAME AND D.FACTORY_DIV = A2.CODE"
						// " LEFT JOIN ("
						// " SELECT ITEM_ENAME, FACTORY_DIV, SUM(TO_NUMBER(REMARK1)), SUM(TO_NUMBER(REMARK2)),"
						// " CASE WHEN ITEM_ENAME = '06' THEN"
						// " DECODE(SUM(TO_NUMBER(REMARK2)), 0, 0, SUM(TO_NUMBER(REMARK1)) / SUM(TO_NUMBER(REMARK2)))"
						// " - DECODE(SUM(TO_NUMBER(REMARK4)), 0, 0, SUM(TO_NUMBER(REMARK3)) / SUM(TO_NUMBER(REMARK4)))"
						// " - DECODE(SUM(TO_NUMBER(REMARK6)), 0, 0, SUM(TO_NUMBER(REMARK5)) / SUM(TO_NUMBER(REMARK6)))"
						// " WHEN ITEM_ENAME = '01' THEN"
						// " DECODE(SUM(TO_NUMBER(REMARK2)), 0, 0, SUM(TO_NUMBER(REMARK1)) / SUM(TO_NUMBER(REMARK2)))"
						// " - DECODE(SUM(TO_NUMBER(REMARK4)), 0, 0, SUM(TO_NUMBER(REMARK3)) / SUM(TO_NUMBER(REMARK4)))"
						// " ELSE DECODE(SUM(TO_NUMBER(REMARK2)), 0, 0, SUM(TO_NUMBER(REMARK1)) / SUM(TO_NUMBER(REMARK2)))"
						// " END REMARK_YEAR"
						// " FROM TFOSMT10J"
						// " WHERE DATE_TIME <= TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY, 'YYYYMMDD')"
						// " AND DATE_TIME >= TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY, 'YYYY') || '0101'"
						// " AND LENGTH(DATE_TIME) = 8"
						// " GROUP BY ITEM_ENAME, FACTORY_DIV"
						// " ) E ON A.CODE = E.ITEM_ENAME AND E.FACTORY_DIV = A2.CODE"
						// " WHERE A.CODE_CLASS = 'FOSMTD' AND A.CODE = '06' AND A2.CODE = 'A00'"
						// " ORDER BY A2.CODE, A.CODE_DESC_2_CONTENT, A.CODE"
						// ;
// DM8 SQL：
					sqlstr =
						" SELECT A.CODE, A2.CODE FACTORY_DIV, A.CODE_DESC_1_CONTENT ITEM_CNAME,"
						" COALESCE(C.UNIT_COST, 0) REMARK,"
						" COALESCE(B1.UNIT_COST, 0) REMARK_1,"
						" COALESCE(B2.UNIT_COST, 0) REMARK_2,"
						" COALESCE(B3.UNIT_COST, 0) REMARK_3,"
						" COALESCE(B4.UNIT_COST, 0) REMARK_4,"
						" COALESCE(B5.UNIT_COST, 0) REMARK_5,"
						" COALESCE(B6.UNIT_COST, 0) REMARK_6,"
						" COALESCE(B7.UNIT_COST, 0) REMARK_7,"
						" COALESCE(ROUND(D.REMARK_MON, 2), 0) REMARK_MON,"
						" COALESCE(ROUND(E.REMARK_YEAR, 2), 0) REMARK_YEAR"
						" FROM TEP0002 A"
						" LEFT JOIN TEP0002 A2 ON A2.CODE_CLASS = 'FOSMT5' AND A2.CODE = 'A00'"
						" LEFT JOIN TFOSMT10J B1 ON A.CODE = B1.ITEM_ENAME AND B1.FACTORY_DIV = A2.CODE AND B1.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 7, 'YYYYMMDD')"
						" LEFT JOIN TFOSMT10J B2 ON A.CODE = B2.ITEM_ENAME AND B2.FACTORY_DIV = A2.CODE AND B2.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 6, 'YYYYMMDD')"
						" LEFT JOIN TFOSMT10J B3 ON A.CODE = B3.ITEM_ENAME AND B3.FACTORY_DIV = A2.CODE AND B3.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 5, 'YYYYMMDD')"
						" LEFT JOIN TFOSMT10J B4 ON A.CODE = B4.ITEM_ENAME AND B4.FACTORY_DIV = A2.CODE AND B4.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 4, 'YYYYMMDD')"
						" LEFT JOIN TFOSMT10J B5 ON A.CODE = B5.ITEM_ENAME AND B5.FACTORY_DIV = A2.CODE AND B5.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 3, 'YYYYMMDD')"
						" LEFT JOIN TFOSMT10J B6 ON A.CODE = B6.ITEM_ENAME AND B6.FACTORY_DIV = A2.CODE AND B6.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 2, 'YYYYMMDD')"
						" LEFT JOIN TFOSMT10J B7 ON A.CODE = B7.ITEM_ENAME AND B7.FACTORY_DIV = A2.CODE AND B7.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1, 'YYYYMMDD')"
						" LEFT JOIN TFOSMT10J C ON A.CODE = C.ITEM_ENAME AND C.FACTORY_DIV = A2.CODE AND C.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1, 'YYYYMM')"
						" LEFT JOIN ("
						" SELECT ITEM_ENAME, FACTORY_DIV, SUM(TO_NUMBER(REMARK1)), SUM(TO_NUMBER(REMARK2)),"
						" CASE WHEN ITEM_ENAME = '06' THEN"
						" DECODE(SUM(TO_NUMBER(REMARK2)), 0, 0, SUM(TO_NUMBER(REMARK1)) / SUM(TO_NUMBER(REMARK2)))"
						" - DECODE(SUM(TO_NUMBER(REMARK4)), 0, 0, SUM(TO_NUMBER(REMARK3)) / SUM(TO_NUMBER(REMARK4)))"
						" - DECODE(SUM(TO_NUMBER(REMARK6)), 0, 0, SUM(TO_NUMBER(REMARK5)) / SUM(TO_NUMBER(REMARK6)))"
						" WHEN ITEM_ENAME = '01' THEN"
						" DECODE(SUM(TO_NUMBER(REMARK2)), 0, 0, SUM(TO_NUMBER(REMARK1)) / SUM(TO_NUMBER(REMARK2)))"
						" - DECODE(SUM(TO_NUMBER(REMARK4)), 0, 0, SUM(TO_NUMBER(REMARK3)) / SUM(TO_NUMBER(REMARK4)))"
						" ELSE DECODE(SUM(TO_NUMBER(REMARK2)), 0, 0, SUM(TO_NUMBER(REMARK1)) / SUM(TO_NUMBER(REMARK2)))"
						" END REMARK_MON"
						" FROM TFOSMT10J"
						" WHERE DATE_TIME <= TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1, 'YYYYMMDD')"
						" AND DATE_TIME >= TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1, 'YYYYMM') || '01'"
						" AND LENGTH(DATE_TIME) = 8"
						" GROUP BY ITEM_ENAME, FACTORY_DIV"
						" ) D ON A.CODE = D.ITEM_ENAME AND D.FACTORY_DIV = A2.CODE"
						" LEFT JOIN ("
						" SELECT ITEM_ENAME, FACTORY_DIV, SUM(TO_NUMBER(REMARK1)), SUM(TO_NUMBER(REMARK2)),"
						" CASE WHEN ITEM_ENAME = '06' THEN"
						" DECODE(SUM(TO_NUMBER(REMARK2)), 0, 0, SUM(TO_NUMBER(REMARK1)) / SUM(TO_NUMBER(REMARK2)))"
						" - DECODE(SUM(TO_NUMBER(REMARK4)), 0, 0, SUM(TO_NUMBER(REMARK3)) / SUM(TO_NUMBER(REMARK4)))"
						" - DECODE(SUM(TO_NUMBER(REMARK6)), 0, 0, SUM(TO_NUMBER(REMARK5)) / SUM(TO_NUMBER(REMARK6)))"
						" WHEN ITEM_ENAME = '01' THEN"
						" DECODE(SUM(TO_NUMBER(REMARK2)), 0, 0, SUM(TO_NUMBER(REMARK1)) / SUM(TO_NUMBER(REMARK2)))"
						" - DECODE(SUM(TO_NUMBER(REMARK4)), 0, 0, SUM(TO_NUMBER(REMARK3)) / SUM(TO_NUMBER(REMARK4)))"
						" ELSE DECODE(SUM(TO_NUMBER(REMARK2)), 0, 0, SUM(TO_NUMBER(REMARK1)) / SUM(TO_NUMBER(REMARK2)))"
						" END REMARK_YEAR"
						" FROM TFOSMT10J"
						" WHERE DATE_TIME <= TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1, 'YYYYMMDD')"
						" AND DATE_TIME >= TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1, 'YYYY') || '0101'"
						" AND LENGTH(DATE_TIME) = 8"
						" GROUP BY ITEM_ENAME, FACTORY_DIV"
						" ) E ON A.CODE = E.ITEM_ENAME AND E.FACTORY_DIV = A2.CODE"
						" WHERE A.CODE_CLASS = 'FOSMTD' AND A.CODE = '06' AND A2.CODE = 'A00'"
						" ORDER BY A2.CODE, A.CODE_DESC_2_CONTENT, A.CODE"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				EIClass blk;
				cmd_inq.ExecuteQuery(blk.Tables[0]);
				cmd_inq.Close();

				CDataRow& row = bcls_ret->Tables[tbl].Rows.Add();
				row.Merge(blk.Tables[0].Rows[0]);
			}
			else if (tbl == "TFOSMT11")
			{
				//检修计划
				bcls_ret->Tables.Add(tbl);
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
// DM8 适配 CHANGE-65：取查询日期前推1日的日期字符串;日历日边界、参数与结果列保持不变。
// 改写原因：DB2 日期天数后缀改为整数天运算；表达式级天数标注(如 DAYOFWEEK(...) DAYS)同样改为整数天运算；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr =
						// " SELECT FACTORY_DIV, STATION_ID STATION_NAME,"
						// " REMARK_7 STATION_NO_1, REMARK_1 STATION_NO_2, REMARK_2 STATION_NO_3,"
						// " REMARK_3 STATION_NO_4, REMARK_4 STATION_NO_5, REMARK_5 STATION_NO_6,"
						// " REMARK_6 STATION_NO_7, REMARK_8 PERIOD_TIME, REMARK"
						// " FROM TFOSMT11A"
						// " WHERE DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS - DAYOFWEEK(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS) DAYS + 1 DAYS, 'YYYYMMDD')"
						// " ORDER BY FACTORY_DIV, STATION_ID"
						// ;
// DM8 SQL：
					sqlstr =
						" SELECT FACTORY_DIV, STATION_ID STATION_NAME,"
						" REMARK_7 STATION_NO_1, REMARK_1 STATION_NO_2, REMARK_2 STATION_NO_3,"
						" REMARK_3 STATION_NO_4, REMARK_4 STATION_NO_5, REMARK_5 STATION_NO_6,"
						" REMARK_6 STATION_NO_7, REMARK_8 PERIOD_TIME, REMARK"
						" FROM TFOSMT11A"
						" WHERE DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 - DAYOFWEEK(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1) + 1, 'YYYYMMDD')"
						" ORDER BY FACTORY_DIV, STATION_ID"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();
			}
			else if (tbl == "TFOSMT12")
			{
				//工作安排
				bcls_ret->Tables.Add(tbl);
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
					sqlstr =
						" SELECT CODE,CODE_DESC_1_CONTENT DEP_NAME"
						" FROM TEP0002 WHERE CODE_CLASS = 'FOSMT1'"
						" AND CODE_DESC_4_CONTENT <> ' '"
						" ORDER BY CODE_DESC_4_CONTENT"
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
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