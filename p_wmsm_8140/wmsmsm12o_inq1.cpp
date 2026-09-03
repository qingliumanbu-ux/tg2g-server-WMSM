/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         JHZHAO
Version:		1.0
Date:			2016-03-05
Description:	待入库材料信息查询
**************************************************/

//框架头文件
#include "stdafx.h"
//#include "smhs.h"
//程序用头文件
//#include "twma1.h"
//#include "twma0.h"

//函数申明
int f_epes_get_auth_other(const char *iuser, int irestype, EIClass *bcls_ret, CDbConnection * conn);

/*<remark>=========================================================
///<summary>
///待入库材料信息查询
///<para>
///2.排序方式：队列写入时间
///</para>
///<para>数据库表：TWMA0 倒躲队列；TWMA1 物料主档表
///<returns>返回符合查询条件的队列信息</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wmsmsm12o_inq1);

int f_wmsmsm12o_inq1(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString stock_oper_order = "";
	CString stock_no = "";
	CString heat_no = "";
	CString st_no = "";
	CString mat_no = "";
	CString transfer_plan_no = "";//add by ljnie 2016/6/17 13:55:31 增加转库计划号
	CString mat_no_array = "";
	CString s_userid("");
	CDecimal d_thick_fr = 0;
	CDecimal d_thick_to = 0;

	CDecimal rowCount = 0;
	CDecimal rowSum = 0;
	int fetchRowCount = 0;
	int NOW_NUM = 0;
	int RETURN_NUM = -1;////每页记录数量
	int INDEX_FROM = 0;//页数

	/* 实体类定义 */
	//CTWMA1 twma1_q(conn);
	//CTWMA1 twma1(conn);
	//CTWMA1 twma1_1(conn);
	//CTWMA0 twma0(conn);
	CModel twma1_q = CModel("TMMSM01");
	CModel twma1 = CModel("TMMSM01");
	CModel twma1_1 = CModel("TMMSM01");
	CModel twma0 = CModel("TWMA0");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	//系统的分页类信息。
	CPageInfo pageInfo;

	try
	{

		// 获取前台传入参数 
		s_userid = s.userid;

		//获取库区授权
		CString stock_no_auth = "' '";
		//EIClass *bcls_auth = new EIClass;
		//if (f_epes_get_auth_other(s.userid, 5, bcls_auth, conn) != 0)
		//{
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}
		//for (int fetchRowCount = 0; fetchRowCount < bcls_auth->Tables[0].Rows.get_Count(); fetchRowCount++)
		//{
		//	stock_no_auth += ", '" + bcls_auth->Tables[0].Rows[fetchRowCount]["name"].ToString() + "' ";
		//}
		//delete bcls_auth;

		//返回数据信息
		bcls_ret->Tables[0].Columns.Add(twma0);
		bcls_ret->Tables[0].Columns.Add(twma1);

		//分页信息
		CDataTable& table = bcls_ret->Tables.Add("PAGEINFO");
		table.Columns.Add(DT_DECIMAL, "TOTAL_RECORD");
		table.Columns.Add(DT_DECIMAL, "WEIGHT");

		////2)获取分页信息
		//if (bcls_rec->Tables.Contains("PageInfo"))
		//{
		//	pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		//}
		//else
		//{
		//	pageInfo.RecordFrom = 0;
		//	pageInfo.PageSize = -1;  //每页记录数量
		//}

		//Log::Trace("", __FUNCTION__, "pageInfo.RecordFrom[{0}]pageInfo.PageSize[{1}]", pageInfo.RecordFrom, pageInfo.PageSize);
		//分页
		if (bcls_rec->Tables[0].Columns.Contains("INDEX_FROM"))
		{
			INDEX_FROM = bcls_rec->Tables[0].Rows[0]["INDEX_FROM"];

		}
		if (bcls_rec->Tables[0].Columns.Contains("RETURN_NUM"))
		{
			RETURN_NUM = bcls_rec->Tables[0].Rows[0]["RETURN_NUM"];
		}

		Log::Trace("", __FUNCTION__, "INDEX_FROM[{0}];RETURN_NUM[{1}]", INDEX_FROM, RETURN_NUM);

		twma1_q.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		twma1_q.TrimOrBlank();


		if (bcls_rec->Tables[0].Columns.Contains("MAT_THICK_FR"))
			d_thick_fr = bcls_rec->Tables[0].Rows[0]["MAT_THICK_FR"].ToDecimal();
		if (bcls_rec->Tables[0].Columns.Contains("MAT_THICK_TO"))
			d_thick_to = bcls_rec->Tables[0].Rows[0]["MAT_THICK_TO"].ToDecimal();

		/* ***** 打印输入参数 ***** */
		Log::Trace("", __FUNCTION__, "twma1_q.MAT_NO\t[{0}]", twma1_q["MAT_NO"].ToString());
		Log::Trace("", __FUNCTION__, "twma1_q.STOCK_NO\t[{0}]", twma1_q["STOCK_NO"].ToString());
		Log::Trace("", __FUNCTION__, "twma1_q.ORDER_NO\t[{0}]", twma1_q["ORDER_NO"].ToString());
		Log::Trace("", __FUNCTION__, "twma1_q.HEAT_NO\t[{0}]", twma1_q["HEAT_NO"].ToString());
		Log::Trace("", __FUNCTION__, "twma1_q.PONO\t[{0}]", twma1_q["PONO"].ToString());
		Log::Trace("", __FUNCTION__, "twma1_q.SG_SIGN\t[{0}]", twma1_q["SG_SIGN"].ToString());
		Log::Trace("", __FUNCTION__, "twma1_q.ST_NO\t[{0}]", twma1_q["ST_NO"].ToString());
		Log::Trace("", __FUNCTION__, "twma1_q.MAT_LINE_TYPE\t[{0}]", twma1_q["MAT_LINE_TYPE"].ToString());
		Log::Trace("", __FUNCTION__, "twma1_q.MAT_KIND\t[{0}]", twma1_q["MAT_KIND"].ToString());
		Log::Trace("", __FUNCTION__, "twma1_q.TRANSFER_PLAN_NO\t[{0}]", twma1_q["TRANSFER_PLAN_NO"].ToString());
		Log::Trace("", __FUNCTION__, "d_thick_fr\t[{0}]", d_thick_fr);
		Log::Trace("", __FUNCTION__, "d_thick_to\t[{0}]", d_thick_to);

		twma1_q["MAT_LINE_TYPE"] = "SM";
		twma1_q["MAT_KIND"] = "SM";

		stock_no_auth = twma1_q["STOCK_NO"].ToString();

		if (d_thick_fr > 9999)
		{
			d_thick_fr = 9999;
		}
		if (d_thick_to == 0 ||
			d_thick_to >= 9999)
		{
			d_thick_to = 9999;
		}


		if (twma1_q["STOCK_NO"].ToString().Trim() != "")
		{
			sqlwhere += " AND TWMA1.STOCK_NO = @stock_no";
		}

		if (twma1_q["ORDER_NO"].ToString().Trim() != "")
		{
			sqlwhere += " AND TWMA1.ORDER_NO LIKE @order_no";
		}

		if (twma1_q["HEAT_NO"].ToString().Trim() != "")
		{
			sqlwhere += " AND TWMA1.HEAT_NO LIKE @heat_no";
		}

		if (twma1_q["PONO"].ToString().Trim() != "")
		{
			sqlwhere += " AND TWMA1.PONO LIKE @pono";
		}

		if (twma1_q["SG_SIGN"].ToString().Trim() != "")
		{
			sqlwhere += " AND TWMA1.SG_SIGN LIKE @sg_sign";
		}

		if (twma1_q["ST_NO"].ToString().Trim() != "")
		{
			sqlwhere += " AND TWMA1.ST_NO LIKE @st_no";
		}

		if (twma1_q["TRANSFER_PLAN_NO"].ToString().Trim() != "")
		{
			sqlwhere += " AND TWMA1.TRANSFER_PLAN_NO LIKE @transfer_plan_no";
		}

		if (twma1_q["MAT_NO"].ToString().Trim() != "")
		{
			/*sqlstr += " AND TWMA1.MAT_NO IN ( ";
			sqlstr += mat_no;
			sqlstr += " )";*/
			sqlwhere += " AND TWMA1.MAT_NO LIKE @mat_no";
		}

		sqlwhere += " AND TWMA1.MAT_LINE_TYPE = 'SM'";

		sqlwhere += " AND TWMA1.MAT_ACT_THICK BETWEEN @thick_fr AND @thick_to ";

	/*	sqlwhere +=
			" AND TWMA1.STOCK_NO IN ('" + stock_no_auth + "')";
*/

		sqlstr =
			" SELECT COUNT(*),SUM(MAT_ACT_WT) FROM  TMMSM01 TWMA1"
			" WHERE 1=1 AND mat_status in ('23','29','30') and in_flag ='1' and hot_charge_flag !='2' AND NOT  exists (select null from twma0  where mat_no =twma1.mat_no ) ";// AND TWMA1.FACTORY_DIV ='A1'
		sqlstr = sqlstr + sqlwhere;
		Log::Trace("", __FUNCTION__, "sqlcount[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stock_no", twma1_q["STOCK_NO"].ToString());
		cmd_inq.Parameters.Set("userid", s_userid);
		cmd_inq.Parameters.Set("mat_no", "%" + twma1_q["MAT_NO"].ToString().Trim() + "%");
		cmd_inq.Parameters.Set("stock_oper_order", twma1_q["STOCK_OPER_ORDER"].ToString());
		cmd_inq.Parameters.Set("order_no", twma1_q["ORDER_NO"].ToString() + "%");
		cmd_inq.Parameters.Set("transfer_plan_no", twma1_q["TRANSFER_PLAN_NO"].ToString() + "%");
		cmd_inq.Parameters.Set("heat_no", twma1_q["HEAT_NO"].ToString() + "%");
		cmd_inq.Parameters.Set("pono", twma1_q["PONO"].ToString() + "%");
		cmd_inq.Parameters.Set("sg_sign", twma1_q["SG_SIGN"].ToString() + "%");
		cmd_inq.Parameters.Set("st_no", twma1_q["ST_NO"].ToString() + "%");
		cmd_inq.Parameters.Set("mat_line_type", twma1_q["MAT_LINE_TYPE"].ToString());
		cmd_inq.Parameters.Set("mat_kind1", twma1_q["MAT_KIND"].ToString());
		cmd_inq.Parameters.Set("thick_fr", d_thick_fr);
		cmd_inq.Parameters.Set("thick_to", d_thick_to);
		//rowCount = cmd_inq.ExecuteScalar();
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read()){
			rowCount = cmd_inq.GetDecimal(1);
			rowSum = cmd_inq.GetDecimal(2);
		}
		cmd_inq.Close();


		Log::Trace("", __FUNCTION__, "rowCount[{0}]", rowCount);
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			sqlstr =
				" SELECT TWMA1.ST_NO,TWMA1.PONO,TWMA1.MAT_ACT_THICK,TWMA1.MAT_ACT_WIDTH,"
				"(SELECT CODE_DESC_1_CONTENT FROM TEP0002 WHERE CODE_CLASS = 'M005' AND CODE = TWMA1.MAT_STATUS) MAT_STATUS, "
				" TWMA1.MAT_ACT_LEN,TWMA1.MAT_ACT_WT,TWMA1.HEAT_NO,TWMA1.TRANSFER_PLAN_NO,TWMA1.MAT_THEORY_WT,TWMA1.MAT_DESTION,TWMA1.* "
				" FROM TMMSM01 TWMA1 WHERE 1=1 AND mat_status in ('23','29','30') and in_flag ='1'  and hot_charge_flag !='2' AND FIN_ST_NO !='' AND NOT  exists (select null from twma0  where mat_no =twma1.mat_no ) "; //AND TWMA1.FACTORY_DIV = 'A1'
			break;
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
			sqlstr =
				" SELECT TWMA1.ST_NO,TWMA1.PONO,TWMA1.MAT_ACT_THICK,TWMA1.MAT_ACT_WIDTH,"
				"(SELECT CODE_DESC_1_CONTENT FROM TEP0002 WHERE CODE_CLASS = 'M005' AND CODE = TWMA1.MAT_STATUS) MAT_STATUS, "
				" TWMA1.MAT_ACT_LEN,TWMA1.MAT_ACT_WT,TWMA1.HEAT_NO,TWMA1.TRANSFER_PLAN_NO,TWMA1.MAT_THEORY_WT,TWMA1.MAT_DESTION,TWMA1.* "
				" FROM TMMSM01 TWMA1 WHERE 1=1 AND mat_status in ('23','29','30') and in_flag ='1'  and hot_charge_flag !='2' AND FIN_ST_NO !='' AND NOT  exists (select null from twma0  where mat_no =twma1.mat_no )  ";//AND TWMA1.FACTORY_DIV ='A1' 
			break;
		case DB_KIND_ORACLE:	        // Oracle 数据库
			sqlstr =
				" SELECT TWMA1.ST_NO,TWMA1.PONO,TWMA1.MAT_ACT_THICK,TWMA1.MAT_ACT_WIDTH,"
				"(SELECT CODE_DESC_1_CONTENT FROM TEP0002 WHERE CODE_CLASS = 'M005' AND CODE = TWMA1.MAT_STATUS) MAT_STATUS, "
				" TWMA1.MAT_ACT_LEN,TWMA1.MAT_ACT_WT,TWMA1.HEAT_NO,TWMA1.TRANSFER_PLAN_NO,TWMA1.MAT_THEORY_WT,TWMA1.MAT_DESTION,TWMA1.* "
				" FROM TMMSM01 TWMA1 WHERE 1=1 AND mat_status in ('23','29','30') and in_flag ='1'  and hot_charge_flag !='2' AND FIN_ST_NO !='' AND NOT  exists (select null from twma0  where mat_no =twma1.mat_no ) ";//AND TWMA1.FACTORY_DIV ='A1' 
		default:
			break;
		}

		sqlstr = sqlstr + sqlwhere;
		Log::Trace("", __FUNCTION__, "sqlst[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stock_no", twma1_q["STOCK_NO"].ToString());
		cmd_inq.Parameters.Set("userid", s_userid);
		cmd_inq.Parameters.Set("mat_no", "%" + twma1_q["MAT_NO"].ToString().Trim() + "%");
		cmd_inq.Parameters.Set("stock_oper_order", twma1_q["STOCK_OPER_ORDER"].ToString());
		cmd_inq.Parameters.Set("order_no", twma1_q["ORDER_NO"].ToString() + "%");
		cmd_inq.Parameters.Set("transfer_plan_no", twma1_q["TRANSFER_PLAN_NO"].ToString() + "%");
		cmd_inq.Parameters.Set("heat_no", twma1_q["HEAT_NO"].ToString() + "%");
		cmd_inq.Parameters.Set("pono", twma1_q["PONO"].ToString() + "%");
		cmd_inq.Parameters.Set("sg_sign", twma1_q["SG_SIGN"].ToString() + "%");
		cmd_inq.Parameters.Set("st_no", st_no + "%");
		cmd_inq.Parameters.Set("mat_line_type", twma1_q["MAT_LINE_TYPE"].ToString());
		cmd_inq.Parameters.Set("mat_kind1", twma1_q["MAT_KIND"].ToString());
		cmd_inq.Parameters.Set("thick_fr", d_thick_fr);
		cmd_inq.Parameters.Set("thick_to", d_thick_to);
		//cmd_inq.ExecuteReader();

		NOW_NUM = INDEX_FROM*RETURN_NUM;//当前个数
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], NOW_NUM, RETURN_NUM);
		//while (cmd_inq.Read())
		//{
		//	fetchRowCount++;

		//	if (fetchRowCount > (NOW_NUM + RETURN_NUM))
		//	{
		//		Log::Trace("", __FUNCTION__, "超上线，break");
		//		break;
		//	}
		//	if (!((fetchRowCount > NOW_NUM) &&
		//		(fetchRowCount <= (NOW_NUM + RETURN_NUM))))
		//	{
		//		continue;
		//	}

		//	twma1.Reset();
		//	twma0.Reset();
		//	Log::Trace("", __FUNCTION__, "ST_NO[{0}]", twma1["ST_NO"].ToString());
		//	twma1["ST_NO"] = cmd_inq.GetString(1);
		//	twma1["PONO"] = cmd_inq.GetString(2);
		//	twma1["MAT_ACT_THICK"] = cmd_inq.GetDecimal(3);
		//	twma1["MAT_ACT_WIDTH"] = cmd_inq.GetDecimal(4);
		//	twma1["MAT_ACT_LEN"] = cmd_inq.GetDecimal(5);
		//	twma1["MAT_ACT_WT"] = cmd_inq.GetDecimal(6);
		//	twma1["HEAT_NO"] = cmd_inq.GetString(7);
		//	twma1["TRANSFER_PLAN_NO"] = cmd_inq.GetString(8);//add by ljnie2016/6/17 13:54:53 转库计划号
		//	twma1["MAT_THEORY_WT"] = cmd_inq.GetDecimal(9);
		//	twma1["MAT_DESTION"] = cmd_inq.GetString(10);


		//	cmd_inq.Fetch(twma0);
		//	twma1_1["MAT_NO"] = twma0["MAT_NO"];
		//	twma1_1.Query("MAT_NO");
		//	Log::Trace("", __FUNCTION__, "材料号[{0}]", twma0["MAT_NO"].ToString());
		//	Log::Trace("", __FUNCTION__, "ST_NO[{0}]", twma1["ST_NO"].ToString());
		//	Log::Trace("", __FUNCTION__, "TRANSFER_PLAN_NO[{0}]", twma1["TRANSFER_PLAN_NO"].ToString());
		//	Log::Trace("", __FUNCTION__, "测试出钢记号[{0}]", cmd_inq.GetString(1));

		//	CDataRow& row = bcls_ret->Tables[0].Rows.Add();
		//	//Log::Trace("", __FUNCTION__, "测试出钢记号[{0}]", cmd_inq.GetString(1));
		//	//row.Merge(twma1_1);
		//	//Log::Trace("", __FUNCTION__, "测试出钢记号[{0}]", cmd_inq.GetString(1));

		//	CDataTable dt_temp;
		//	dt_temp.Clear();
		//	twma1_1.MergeTo(dt_temp);
		//	row.Merge(dt_temp.Rows[0]);

		//	row["ST_NO"] = twma1["ST_NO"];
		//	row["PONO"] = twma1["PONO"];
		//	row["HEAT_NO"] = twma1["HEAT_NO"];
		//	row["TRANSFER_PLAN_NO"] = twma1["TRANSFER_PLAN_NO"];
		//	row["MAT_ACT_THICK"] = twma1["MAT_ACT_THICK"];
		//	row["MAT_ACT_WIDTH"] = twma1["MAT_ACT_WIDTH"];
		//	row["MAT_ACT_LEN"] = twma1["MAT_ACT_LEN"];
		//	row["MAT_ACT_WT"] = twma1["MAT_ACT_WT"];
		//	row["MAT_THEORY_WT"] = twma1["MAT_THEORY_WT"];
		//	row["MAT_DESTION"] = twma1["MAT_DESTION"];
		//	
		//	//row.Merge(twma0);

		//	dt_temp.Clear();
		//	twma0.MergeTo(dt_temp);
		//	row.Merge(dt_temp.Rows[0]);
		//	row["STOCK_OPER_ORDER"] = " ";

		//	Log::Trace("", __FUNCTION__, "测试出钢记号[{0}]", cmd_inq.GetString(1));
		//}
		cmd_inq.Close();

		//返回记录总数
		CDataRow& row1 = bcls_ret->Tables["PAGEINFO"].Rows.Add();
		row1["TOTAL_RECORD"] = rowCount.ToInt32();
		row1["WEIGHT"] = rowSum.ToInt32();
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

