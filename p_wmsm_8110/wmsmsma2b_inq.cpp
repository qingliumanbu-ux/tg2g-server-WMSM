/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         JHZHAO
Version:		1.0
Date:			2016-03-05
Description:	板坯入库队列信息查询
**************************************************/

//框架头文件
#include "stdafx.h"

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

BM2F_ENTERACE(wmsmsma2b_inq);

int f_wmsmsma2b_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDecimal d_thick_fr = 0;
	CDecimal d_thick_to = 0;
	CString v_transfer_flag = "";
	int fetchRowCount = 0;
	CDecimal rowCount = 0;

	/* 实体类定义 */
	//CTWMA1 twma1_q(conn);
	//CTWMA1 twma1(conn);
	CModel twma1_q = CModel("TMMSM01");
	CModel twma1 = CModel("TMMSM01");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";
	CString sqlgroup = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	//系统的分页类信息。
	CPageInfo pageInfo;

	//返回数据信息
	bcls_ret->Tables[0].Columns.Add(twma1);

	try
	{

		// 获取前台传入参数

		//获取库区授权
		CString stock_no_auth = "' '";
		EIClass *bcls_auth = new EIClass;
		if (f_epes_get_auth_other(s.userid, 5, bcls_auth, conn) != 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
		for (int fetchRowCount = 0; fetchRowCount < bcls_auth->Tables[0].Rows.get_Count(); fetchRowCount++)
		{
			stock_no_auth += ", '" + bcls_auth->Tables[0].Rows[fetchRowCount]["name"].ToString() + "' ";
		}
		delete bcls_auth;


		//分页信息
		CDataTable& table = bcls_ret->Tables.Add("PAGEINFO");
		table.Columns.Add(DT_DECIMAL, "recordsum");

		//2)获取分页信息
		if (bcls_rec->Tables.Contains("PageInfo"))
		{
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		else
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = -1;  //每页记录数量
		}

		Log::Trace("", __FUNCTION__, "pageInfo.RecordFrom[{0}]pageInfo.PageSize[{1}]", pageInfo.RecordFrom, pageInfo.PageSize);


		twma1_q.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		twma1_q.TrimOrBlank();

		twma1_q["MAT_LINE_TYPE"] = "SM";
		twma1_q["MAT_KIND"] = "SM";

		if (bcls_rec->Tables[0].Columns.Contains("MAT_THICK_FR"))
			d_thick_fr = bcls_rec->Tables[0].Rows[0]["MAT_THICK_FR"].ToDecimal();
		if (bcls_rec->Tables[0].Columns.Contains("MAT_THICK_TO"))
			d_thick_to = bcls_rec->Tables[0].Rows[0]["MAT_THICK_TO"].ToDecimal();


		Log::Trace("", __FUNCTION__, "twma1_q.STOCK_NO\t[{0}]", twma1_q["STOCK_NO"].ToString());
		Log::Trace("", __FUNCTION__, "twma1_q.MAT_NO=[{0}]", twma1_q["MAT_NO"].ToString());
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
		Log::Trace("", __FUNCTION__, "v_transfer_flag\t[{0}]", v_transfer_flag);



		if (twma1_q["STOCK_NO"].ToString().Trim() != "")
		{
			sqlwhere += " AND T2.STOCK_NO = @stock_no ";
		}
		if (twma1_q["ORDER_NO"].ToString().Trim() != "")
		{
			sqlwhere += " AND t2.ORDER_NO LIKE @order_no ";
		}
		if (twma1_q["TRANSFER_PLAN_NO"].ToString().Trim() != "")
		{
			sqlwhere += " AND t2.TRANSFER_PLAN_NO LIKE @transfer_plan_no ";
		}
		if (twma1_q["HEAT_NO"].ToString().Trim() != "")
		{
			sqlwhere += " AND t2.HEAT_NO LIKE @heat_no ";
		}
		if (twma1_q["PONO"].ToString().Trim() != "")
		{
			sqlwhere += " AND t2.PONO LIKE @pono ";
		}
		if (twma1_q["SG_SIGN"].ToString().Trim() != "")
		{
			sqlwhere += " AND t2.SG_SIGN LIKE @sg_sign ";
		}
		if (twma1_q["ST_NO"].ToString().Trim() != "")
		{
			sqlwhere += " AND t2.ST_NO LIKE @st_no ";
		}
		if (twma1_q["MAT_NO"].ToString().Trim() != "")
		{
			sqlwhere += " AND t2.MAT_NO IN ( ";
			sqlwhere += twma1_q["MAT_NO"].ToString();
			sqlwhere += " )";
		}
		if (twma1_q["MAT_LINE_TYPE"].ToString().Trim() != "")
		{
			sqlwhere +=
				" AND EXISTS(SELECT NULL FROM TWM01"
				" WHERE TWM01.STOCK_NO = T2.STOCK_NO"
				" AND TWM01.MAT_LINE_TYPE = @mat_line_type)";
		}
		if (twma1_q["MAT_KIND"].ToString().Trim() != "")
		{
			sqlwhere +=
				" AND EXISTS(SELECT NULL FROM TWM01"
				" WHERE TWM01.STOCK_NO = T2.STOCK_NO"
				" AND TWM01.MAT_KIND = @mat_kind)";
		}


		sqlwhere += " AND t2.MAT_ACT_THICK BETWEEN @thick_fr AND @thick_to";




		if (d_thick_fr > 9999)
		{
			d_thick_fr = 9999;
		}
		if (d_thick_to == 0 ||
			d_thick_to > 9999)
		{
			d_thick_to = 9999;
		}



		sqlwhere +=
			" AND T2.STOCK_NO IN (" + stock_no_auth + ")";


		sqlstr =
			" SELECT COUNT(t2.MAT_NO)"
			" FROM TMMSM01 t2"
			" WHERE EXISTS(SELECT NULL FROM TWMA2 T1"
			" WHERE T1.MAT_NO = T2.MAT_NO)"
			" AND T2.IN_FLAG = '1'";
		sqlstr = sqlstr + sqlwhere;
		Log::Trace("", __FUNCTION__, "sqlstr[{0}]", sqlstr);
		Log::Trace("", __FUNCTION__, "sqlcount[{0}]", sqlstr);

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stock_no", twma1_q["STOCK_NO"].ToString());
		cmd_inq.Parameters.Set("userid", s.userid);
		cmd_inq.Parameters.Set("stock_oper_order", twma1_q["STOCK_OPER_ORDER"].ToString());
		cmd_inq.Parameters.Set("order_no", twma1_q["ORDER_NO"].ToString() + "%");
		cmd_inq.Parameters.Set("transfer_plan_no", twma1_q["TRANSFER_PLAN_NO"].ToString() + "%");
		cmd_inq.Parameters.Set("heat_no", twma1_q["HEAT_NO"].ToString() + "%");
		cmd_inq.Parameters.Set("pono", twma1_q["PONO"].ToString() + "%");
		cmd_inq.Parameters.Set("sg_sign", twma1_q["SG_SIGN"].ToString() + "%");
		cmd_inq.Parameters.Set("st_no", twma1_q["ST_NO"].ToString() + "%");
		cmd_inq.Parameters.Set("mat_line_type", twma1_q["MAT_LINE_TYPE"].ToString());
		cmd_inq.Parameters.Set("mat_kind", twma1_q["MAT_KIND"].ToString());
		cmd_inq.Parameters.Set("thick_fr", d_thick_fr);
		cmd_inq.Parameters.Set("thick_to", d_thick_to);

		rowCount = cmd_inq.ExecuteScalar();

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			sqlstr =
				" SELECT"
				" T2.*"
				" FROM TMMSM01 T2"
				" WHERE EXISTS(SELECT NULL FROM TWMA2 T1"
				" WHERE T1.MAT_NO = T2.MAT_NO)"
				" AND T2.IN_FLAG = '1'"
				;
			break;
		}




		sqlstr = sqlstr + sqlwhere;
		Log::Trace("", __FUNCTION__, "sqlst[{0}]", sqlstr);

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stock_no", twma1_q["STOCK_NO"].ToString());
		cmd_inq.Parameters.Set("userid", s.userid);
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
		cmd_inq.ExecuteReader();
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);


		//返回记录总数
		CDataRow& row1 = bcls_ret->Tables["PAGEINFO"].Rows.Add();
		row1["recordsum"] = rowCount.ToInt32();
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

