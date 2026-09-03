/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         JHZHAO
Version:		1.0
Date:			2016-03-05
Description:	板坯入库队列信息查询
**************************************************/

//框架头文件
#include "stdafx.h"
//#include "smhs.h"
//程序用头文件
//#include "twma0.h"
//#include "twma1.h"
//#include "twm00.h"


//函数申明
int f_epes_get_auth_other(const char *iuser, int irestype, EIClass *bcls_ret, CDbConnection * conn);

/*<remark>=========================================================
///<summary>
///板坯入库队列信息查询
///<para>
///2.排序方式：队列写入时间
///</para>
///<para>数据库表：TWMA0 倒躲队列；TWMA1 物料主档表
///<returns>返回符合查询条件的队列信息</returns>
===========================================================</remark>*/
BM2F_ENTERACE(wmsmsm11_inq1);
int f_wmsmsm11_inq1(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString s_userid("");
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString stock_no = "";
	CString stock_oper_order = "";
	CString heat_no = "";
	CString pono = "";
	CString order_no = "";
	CString mat_no = "";
	CString mat_line_type = " ";
	CString mat_kind = " ";
	CString old_stock_no = "";
	CString st_no = "";
	CDecimal d_from_len = 0, d_to_len = 99999, d_from_width = 0, d_to_width = 99999;
	CDecimal d_thick_fr = 0;
	CDecimal d_thick_to = 0;

	CDecimal rowCount = 0;
	CDecimal rowSum = 0;
	int fetchRowCount = 0;

	int NOW_NUM = 0;
	int RETURN_NUM = -1;////每页记录数量
	int INDEX_FROM = 0;//页数


	/* 实体类定义 */
	CModel twma0 = CModel("TWMA0");
	CModel twma1_q = CModel("TMMSM01");
	CModel twma1 = CModel("TMMSM01");
	CModel twma1_1 = CModel("TMMSM01");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";
	CString sqllast = "";
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);

	//系统的分页类信息。
	CPageInfo pageInfo;

	//返回数据信息
	bcls_ret->Tables[0].Columns.Add(twma0);
	bcls_ret->Tables[0].Columns.Add(twma1);

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


		twma1_q.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		twma1_q.TrimOrBlank();


		if (bcls_rec->Tables[0].Columns.Contains("MAT_THICK_FR"))
			d_thick_fr = bcls_rec->Tables[0].Rows[0]["MAT_THICK_FR"].ToDecimal();
		if (bcls_rec->Tables[0].Columns.Contains("MAT_THICK_TO"))
			d_thick_to = bcls_rec->Tables[0].Rows[0]["MAT_THICK_TO"].ToDecimal();
		stock_no_auth = twma1_q["STOCK_NO"].ToString();
		Log::Trace("", __FUNCTION__, "twma1_q.STOCK_NO\t[{0}]", twma1_q["STOCK_NO"].ToString());
		Log::Trace("", __FUNCTION__, "twma1_q.MAT_NO=[{0}]", twma1_q["MAT_NO"].ToString());
		Log::Trace("", __FUNCTION__, "twma1_q.STOCK_OPER_ORDER\t[{0}]", twma1_q["STOCK_OPER_ORDER"].ToString());
		Log::Trace("", __FUNCTION__, "twma1_q.ORDER_NO\t[{0}]", twma1_q["ORDER_NO"].ToString());
		Log::Trace("", __FUNCTION__, "twma1_q.TRANSFER_PLAN_NO\t[{0}]", twma1_q["TRANSFER_PLAN_NO"].ToString());
		Log::Trace("", __FUNCTION__, "twma1_q.HEAT_NO\t[{0}]", twma1_q["HEAT_NO"].ToString());
		Log::Trace("", __FUNCTION__, "twma1_q.PONO\t[{0}]", twma1_q["PONO"].ToString());
		Log::Trace("", __FUNCTION__, "twma1_q.SG_SIGN\t[{0}]", twma1_q["SG_SIGN"].ToString());
		Log::Trace("", __FUNCTION__, "twma1_q.ST_NO\t[{0}]", twma1_q["ST_NO"].ToString());
		Log::Trace("", __FUNCTION__, "twma1_q.MAT_LINE_TYPE\t[{0}]", twma1_q["MAT_LINE_TYPE"].ToString());
		Log::Trace("", __FUNCTION__, "twma1_q.MAT_KIND\t[{0}]", twma1_q["MAT_KIND"].ToString());
		Log::Trace("", __FUNCTION__, "d_thick_fr\t[{0}]", d_thick_fr);
		Log::Trace("", __FUNCTION__, "d_thick_to\t[{0}]", d_thick_to);


		if (twma1_q["UNIT_CODE"].ToString().Trim() != "")
		{
			sqlwhere += " AND TWMA1.UNIT_CODE = @unit_code ";
		}
		if (twma1_q["STOCK_NO"].ToString().Trim() != "")
		{
			sqlwhere += " AND TWMA0.STOCK_NO = @stock_no ";
		}
		if (twma1_q["STOCK_OPER_ORDER"].ToString().Trim() != "")
		{
			sqlwhere += " AND TWMA0.STOCK_OPER_ORDER = @stock_oper_order ";
		}
		if (twma1_q["ORDER_NO"].ToString().Trim() != "")
		{
			sqlwhere += " AND TWMA1.ORDER_NO LIKE @order_no ";
		}
		if (twma1_q["TRANSFER_PLAN_NO"].ToString().Trim() != "")
		{
			sqlwhere += " AND TWMA1.TRANSFER_PLAN_NO LIKE @transfer_plan_no ";
		}
		if (twma1_q["HEAT_NO"].ToString().Trim() != "")
		{
			sqlwhere += " AND TWMA1.HEAT_NO LIKE @heat_no ";
		}
		if (twma1_q["PONO"].ToString().Trim() != "")
		{
			sqlwhere += " AND TWMA1.PONO LIKE @pono ";
		}
		if (twma1_q["SG_SIGN"].ToString().Trim() != "")
		{
			sqlwhere += " AND TWMA1.SG_SIGN LIKE @sg_sign ";
		}
		if (twma1_q["ST_NO"].ToString().Trim() != "")
		{
			sqlwhere += " AND TWMA1.ST_NO LIKE @st_no ";
		}
		Log::Trace("", __FUNCTION__, "MAT_NO11111=[{0}]", twma1_q["MAT_NO"].ToString());
		if (twma1_q["MAT_NO"].ToString().Trim() != "")
		{
			/*sqlwhere += " AND TWMA0.MAT_NO IN ( '";
			sqlwhere += twma1_q["MAT_NO"].ToString();
			sqlwhere += "' )";*/
			sqlwhere += "AND TWMA0.MAT_NO LIKE '%" + twma1_q["MAT_NO"].ToString() + "%' ";
		}

		//sqlwhere += " AND TWMA1.MAT_ACT_THICK BETWEEN @thick_fr AND @thick_to";
		//sqlwhere += " AND TWMA1.MAT_ACT_THICK >= @thick_fr";
		//sqlwhere += " AND TWMA1.MAT_ACT_THICK <= @thick_to";
		//sqlwhere += " AND TWMA0.MAT_KIND = 'SM'";
		//sqlwhere += " AND TWMA0.MAT_LINE_TYPE = 'SM'";
		sqlwhere +=
			" AND EXISTS (SELECT NULL FROM TWM01 WHERE TWMA0.STOCK_NO = TWM01.STOCK_NO"
			" AND TWM01.MAT_KIND = 'SM')";
		sqlwhere +=
			" AND EXISTS (SELECT NULL FROM TWM01 WHERE TWMA0.STOCK_NO = TWM01.STOCK_NO"
			" AND TWM01.MAT_LINE_TYPE = 'SM')";


		if (d_to_len == 0)
		{
			d_to_len = 999999;
		}
		if (d_to_width == 0)
		{
			d_to_width = 9999;
		}
		if (d_thick_fr > 9999)
		{
			d_thick_fr = 9999;
		}
		if (d_thick_to == 0 ||
			d_thick_to > 9999)
		{
			d_thick_to = 9999;
		}

		//sqlwhere +=
		//	" AND TWMA0.STOCK_NO IN"
		//	"( SELECT STOCK_NO"
		//	" FROM TWM0A "
		//	" WHERE   GROUPID IN ( "
		//	" SELECT GROUPID FROM TESGROUPMEMBER "
		//	"  WHERE MEMBERID IN ( "
		//	" SELECT ID FROM TESUSERINFO WHERE ENAME = @userid "
		//	" ) "
		//	"  ) "
		//	/*	" union "
		//	" select STOCK_NO from twm01 where 'admin' = @userid "*/
		//	"  ) ";

		sqlwhere +=
			" AND TWMA0.STOCK_NO IN ('" + stock_no_auth + "')";
		//" AND TWMA0.STOCK_NO IN ('A11')";

		sqlstr =
			" SELECT COUNT(TWMA0.MAT_NO),SUM(TWMA1.MAT_ACT_WT)"
			" FROM TWMA0,"
			"  TMMSM01 TWMA1"
			" where  TWMA0.MAT_NO = TWMA1.MAT_NO  "
			" and  TWMA0.STOCK_OPER_ORDER LIKE '1%' and twma1.HOT_CHARGE_FLAG !='2' ";
		sqlstr = sqlstr + sqlwhere;
		Log::Trace("", __FUNCTION__, "sqlstr[{0}]", sqlstr);
		Log::Trace("", __FUNCTION__, "sqlcount[{0}]", sqlstr);

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stock_no", twma1_q["STOCK_NO"].ToString());
		cmd_inq.Parameters.Set("unit_code", twma1_q["UNIT_CODE"].ToString());
		cmd_inq.Parameters.Set("userid", s_userid);
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

		//switch (conn->DatabaseKind)
		//{
		//case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		//case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//	sqlstr =
		//		" SELECT TWMA0.* "
		//		" FROM TWMA0"
		//		" LEFT OUTER JOIN TMMSM01 TWMA1"
		//		" ON TWMA0.MAT_NO = TWMA1.MAT_NO  "
		//		" WHERE  TWMA0.STOCK_OPER_ORDER LIKE '1%' ";
		//	break;
		//case DB_KIND_MSSQL:	        // MS SQL Server数据库
		//	sqlstr =
		//		" SELECT TWMA0.* "
		//		" FROM TWMA0"
		//		" LEFT OUTER JOIN TMMSM01 TWMA1"
		//		" ON TWMA0.MAT_NO = TWMA1.MAT_NO  "
		//		" WHERE  AND TWMA0.STOCK_OPER_ORDER LIKE '1%' ";
		//	break;
		//case DB_KIND_ORACLE:	        // Oracle 数据库
		//	sqlstr =
		//		" SELECT TWMA0.* "
		//		" FROM TWMA0"
		//		" LEFT OUTER JOIN TMMSM01 TWMA1"
		//		" ON TWMA0.MAT_NO = TWMA1.MAT_NO  "
		//		" WHERE  TWMA0.STOCK_OPER_ORDER LIKE '1%' ";
		//	break;
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			sqlstr =
				" SELECT (SELECT CODE_DESC_1_CONTENT FROM TEP0002 WHERE CODE_CLASS = 'M005' AND CODE = TWMA1.MAT_STATUS) MAT_STATUS ,"
				"TWMA0.*, TWMA1.* "
				" FROM TWMA0 ,"
				"  TMMSM01 TWMA1"
				" WHERE TWMA0.MAT_NO = TWMA1.MAT_NO  "
				" AND  TWMA0.STOCK_OPER_ORDER LIKE '1%' and twma1.HOT_CHARGE_FLAG !='2' ";
			break;
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
			sqlstr =
				" SELECT (SELECT CODE_DESC_1_CONTENT FROM TEP0002 WHERE CODE_CLASS = 'M005' AND CODE = TWMA1.MAT_STATUS) MAT_STATUS ,"
				"TWMA0.*, TWMA1.* "
				" FROM TWMA0 ,"
				"  TMMSM01 TWMA1"
				" WHERE TWMA0.MAT_NO = TWMA1.MAT_NO  "
				" AND  TWMA0.STOCK_OPER_ORDER LIKE '1%' and twma1.HOT_CHARGE_FLAG !='2'";
			break;
		case DB_KIND_ORACLE:	        // Oracle 数据库
			sqlstr =
				" SELECT (SELECT CODE_DESC_1_CONTENT FROM TEP0002 WHERE CODE_CLASS = 'M005' AND CODE = TWMA1.MAT_STATUS) MAT_STATUS ,"
				"TWMA0.*, TWMA1.* "
				" FROM TWMA0 ,"
				"  TMMSM01 TWMA1"
				" WHERE TWMA0.MAT_NO = TWMA1.MAT_NO  "
				" AND  TWMA0.STOCK_OPER_ORDER LIKE '1%' and twma1.HOT_CHARGE_FLAG !='2' ";
			break;
		default:
			break;
		}

		sqllast = "order by twma0.mat_no asc";

		sqlstr = sqlstr + sqlwhere + sqllast;
		Log::Trace("", __FUNCTION__, "sqlst[{0}]", sqlstr);

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stock_no", twma1_q["STOCK_NO"].ToString());
		cmd_inq.Parameters.Set("unit_code", twma1_q["UNIT_CODE"].ToString());
		cmd_inq.Parameters.Set("userid", s_userid);
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
		
		NOW_NUM = INDEX_FROM*RETURN_NUM;//当前个数
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], NOW_NUM, RETURN_NUM);
		//cmd_inq.ExecuteReader();

		//while (cmd_inq.Read())
		//{
		//	fetchRowCount++;

		//	if (fetchRowCount > (pageInfo.RecordFrom + pageInfo.PageSize))
		//	{
		//		Log::Trace("", __FUNCTION__, "超上限，break");
		//		break;
		//	}
		//	if (!((fetchRowCount > pageInfo.RecordFrom) &&
		//		(fetchRowCount <= (pageInfo.RecordFrom + pageInfo.PageSize))))
		//	{
		//		continue;
		//	}

		//	twma1_1.Reset();
		//	twma0.Reset();

		//	cmd_inq.Fetch(twma0);

		//	twma1_1["MAT_NO"] = twma0["MAT_NO"];
		//	sqlstr =
		//		" SELECT * FROM TMMSM01 TWMA1"
		//		" WHERE MAT_NO = @mat_no";
		//	cmd_inq1.SetCommandText(sqlstr);
		//	cmd_inq1.Parameters.Set("mat_no", twma0["MAT_NO"].ToString());
		//	cmd_inq1.ExecuteReader();

		//	if (cmd_inq1.Read())
		//	{
		//		cmd_inq1.Fetch(twma1_1);
		//	}
		//	cmd_inq1.Close();
		//	//twma1_1.Query("MAT_NO"); 
		//	twma0["UNIT_CODE"] = twma1_1["UNIT_CODE"];//GONGLEI 数据已TWMA1为准。
		//	/*Log::Trace("", __FUNCTION__, "材料号[{0}]", twma0.MAT_NO);
		//	Log::Trace("", __FUNCTION__, "ST_NO[{0}]", twma1.ST_NO);
		//	Log::Trace("", __FUNCTION__, "测试出钢记号[{0}]", cmd_inq.GetString(1));
		//	Log::Trace("", __FUNCTION__, "twma1.MAT_LINE_TYPE[{0}]", twma1.MAT_LINE_TYPE);*/

		//	CDataRow& row = bcls_ret->Tables[0].Rows.Add();
		//	//row.Merge(twma1_1);
		//	//row.Merge(twma0);

		//	CDataTable dt_temp;
		//	dt_temp.Clear();
		//	twma1_1.MergeTo(dt_temp);
		//	row.Merge(dt_temp.Rows[0]);

		//	Log::Trace("", __FUNCTION__, "twma1_1.MAT_NO[{0}]", twma1_1["MAT_NO"].ToString());


		//	dt_temp.Clear();
		//	twma0.MergeTo(dt_temp);
		//	row.Merge(dt_temp.Rows[0]);

		//	if (twma1_1["MAT_NO"].ToString().SubstringNE(7, 2) == "01")
		//	{
		//		twma1_1["REMARK_SM"] = "头坯";
		//	}

		//	row["ST_NO"] = twma1_1["ST_NO"];
		//	row["PONO"] = twma1_1["PONO"];
		//	row["HEAT_NO"] = twma1_1["HEAT_NO"];
		//	row["MAT_ACT_THICK"] = twma1_1["MAT_ACT_THICK"];
		//	row["MAT_ACT_WIDTH"] = twma1_1["MAT_ACT_WIDTH"];
		//	row["MAT_ACT_LEN"] = twma1_1["MAT_ACT_LEN"];
		//	row["MAT_ACT_WT"] = twma1_1["MAT_ACT_WT"];
		//	row["MAT_NUM"] = twma1_1["MAT_NUM"];
		//	row["MEASURE_WT_FLAG"] = twma1_1["MEASURE_WT_FLAG"];
		//	row["OLD_STOCK_NO"] = old_stock_no;
		//	row["SG_SIGN"] = twma1_1["SG_SIGN"];
		//	row["TRANSFER_PLAN_NO"] = twma1_1["TRANSFER_PLAN_NO"];
		//	row["MAT_LINE_TYPE"] = twma1_1["MAT_LINE_TYPE"];
		//	row["UNIT_CODE"] = twma1_1["UNIT_CODE"];
		//	row["REMARK0"] = twma1_1["REMARK_SM"];
		//}
		cmd_inq.Close();

		Log::Trace("", __FUNCTION__, "count[{0}]", bcls_ret->Tables[0].Rows.get_Count());

		//返回记录总数
		CDataRow& row1 = bcls_ret->Tables["PAGEINFO"].Rows.Add();
		row1["TOTAL_RECORD"] = rowCount.ToInt32();
		row1["WEIGHT"] = rowSum.ToInt32();
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };

		/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006"), arguments, 1);
		CString str = ex.GetMsg() + "\r\n" + sqlstr;

		/*返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应*/
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);

		/*数据库异常时返回-1，事务将被回滚*/
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

