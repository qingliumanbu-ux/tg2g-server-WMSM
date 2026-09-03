/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         ljnie
Version:		1.0
Date:			2016/7/6 15:34:36
Description:	库位材料倒垛查询
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件
//#include "twm04.h"
//#include "twma1.h"


//函数申明

int f_epes_get_auth_other(const char *iuser, int irestype, EIClass *bcls_ret, CDbConnection * conn);

/*<remark>=========================================================
///<summary>
///库位材料倒垛查询
///<para>
///2.排序方式：STOCK_NO,HALL_NO
///</para>
///<para>数据库表：TWM04 仓库跨号信息查询；TWMA2
///<returns>返回符合查询条件的仓库跨号信息</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wmsmsm17_inq);

int f_wmsmsm17_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */
	//CTWM04 twm04(conn);
	//CTWMA1 twma1_q(conn);
	CModel twm04 = CModel("TWM04");
	CModel twma1_q = CModel("TMMSM01");

	/* 业务变量 */
	CString stock_no("");
	CString hall_no("");
	CString stock_place_type("");
	CString stock_place_no("");
	CString stock_place_no_fr("");
	CString stock_place_no_to("");
	CString mat_line_type("");
	CString mat_no("");
	CString order_no = "";
	CString pono = "";
	CString heat_no = "";
	CString sg_sign = "";
	CString st_no = "";
	CDecimal d_thick_fr = 0;
	CDecimal d_thick_to = 0;

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";
	CString sqlstr1 = "";
	CString sqlstr2 = "";
	CString sqlorderby = "";
	CString sqlorderby1 = "";
	CString s_userid("");


	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	//系统的分页类信息。
	CPageInfo pageInfo;

	try
	{

		// 获取前台传入参数
		s_userid = s.userid;

		twma1_q.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		twma1_q.TrimOrBlank();

		//获取库区授权
		CString stock_no_auth = "' '";
		EIClass *bcls_auth = new EIClass;
	/*	if (f_epes_get_auth_other(s.userid, 5, bcls_auth, conn) != 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
		for (int fetchRowCount = 0; fetchRowCount < bcls_auth->Tables[0].Rows.get_Count(); fetchRowCount++)
		{
			stock_no_auth += ", '" + bcls_auth->Tables[0].Rows[fetchRowCount]["name"].ToString() + "' ";
		}*/
		delete bcls_auth;

		if (bcls_rec->Tables[0].Columns.Contains("MAT_THICK_FR"))
			d_thick_fr = bcls_rec->Tables[0].Rows[0]["MAT_THICK_FR"].ToDecimal();
		if (bcls_rec->Tables[0].Columns.Contains("MAT_THICK_TO"))
			d_thick_to = bcls_rec->Tables[0].Rows[0]["MAT_THICK_TO"].ToDecimal();

		/* ***** 打印输入参数 ***** */
		Log::Trace("", __FUNCTION__, "twma1_q.STOCK_NO\t[{0}]", twma1_q["STOCK_NO"].ToString());
		Log::Trace("", __FUNCTION__, "twma1_q.ORDER_NO\t[{0}]", twma1_q["ORDER_NO"].ToString());
		Log::Trace("", __FUNCTION__, "twma1_q.HEAT_NO\t[{0}]", twma1_q["HEAT_NO"].ToString());
		Log::Trace("", __FUNCTION__, "twma1_q.PONO\t[{0}]", twma1_q["PONO"].ToString());
		Log::Trace("", __FUNCTION__, "twma1_q.SG_SIGN\t[{0}]", twma1_q["SG_SIGN"].ToString());
		Log::Trace("", __FUNCTION__, "twma1_q.ST_NO\t[{0}]", twma1_q["ST_NO"].ToString());
		Log::Trace("", __FUNCTION__, "twma1_q.MAT_LINE_TYPE\t[{0}]", twma1_q["MAT_LINE_TYPE"].ToString());
		Log::Trace("", __FUNCTION__, "twma1_q.MAT_KIND\t[{0}]", twma1_q["MAT_KIND"].ToString());
		Log::Trace("", __FUNCTION__, "d_thick_fr\t[{0}]", d_thick_fr);
		Log::Trace("", __FUNCTION__, "d_thick_to\t[{0}]", d_thick_to);


		if (d_thick_fr > 9999)
		{
			d_thick_fr = 9999;
		}
		if (d_thick_to == 0 ||
			d_thick_to > 9999)
		{
			d_thick_to = 9999;
		}

		if (twma1_q["STOCK_NO"].ToString().Trim() != "")
		{
			sqlwhere += " AND A.STOCK_NO = @stock_no";
		}

		if (twma1_q["ORDER_NO"].ToString().Trim() != "")
		{
			sqlwhere += " AND C.ORDER_NO LIKE @order_no";
		}

		if (twma1_q["HEAT_NO"].ToString().Trim() != "")
		{
			sqlwhere += " AND C.HEAT_NO LIKE @heat_no";
		}

		if (twma1_q["PONO"].ToString().Trim() != "")
		{
			sqlwhere += " AND C.PONO LIKE @pono";
		}

		if (twma1_q["SG_SIGN"].ToString().Trim() != "")
		{
			sqlwhere += " AND C.SG_SIGN LIKE @sg_sign";
		}

		if (twma1_q["ST_NO"].ToString().Trim() != "")
		{
			sqlwhere += " AND C.ST_NO LIKE @st_no";
		}

		if (twma1_q["MAT_LINE_TYPE"].ToString().Trim() != "")
		{
			sqlwhere += " AND C.MAT_LINE_TYPE LIKE @mat_line_type";
		}

		sqlwhere += " AND C.MAT_ACT_THICK BETWEEN @thick_fr AND @thick_to ";

		if (twma1_q["MAT_NO"].ToString().Trim() != "")
		{
			sqlwhere += " AND C.MAT_NO IN ( ";
			sqlwhere += twma1_q["MAT_NO"].ToString();
			sqlwhere += " )";
		}

		//sqlwhere +=
		//	" AND A.STOCK_NO IN"
		//	" (SELECT STOCK_NO"
		//	" FROM TWM0A"
		//	" WHERE GROUPID IN ("
		//	" SELECT GROUPID FROM TESGROUPMEMBER"
		//	" WHERE MEMBERID IN ("
		//	" SELECT ID FROM TESUSERINFO WHERE ENAME = @userid"
		//	" )"
		//	" )"
		//	" )";
	/*	sqlwhere +=
			" AND A.STOCK_NO IN (" + stock_no_auth + ")";*/

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			sqlstr2 =
				" SELECT A.STOCK_NO, A.STOCK_PLACE_NO, SUM(NVL(C.MAT_NUM, 0)) MAT_NUM, SUM(NVL(C.MAT_ACT_WT, 0)) MAT_WT"
				" FROM TWMA2 A LEFT OUTER JOIN TMMSM01 C"
				" ON  A.MAT_NO = C.MAT_NO"
				" WHERE 1 = 1";
			break;
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
			sqlstr2 =
				" SELECT A.STOCK_NO, A.STOCK_PLACE_NO, SUM(ISNULL(C.MAT_NUM, 0)) MAT_NUM, SUM(ISNULL(C.MAT_ACT_WT, 0)) MAT_WT"
				" FROM TWMA2 A LEFT OUTER JOIN TMMSM01 C"
				" ON  A.MAT_NO = C.MAT_NO"
				" WHERE 1 = 1";
			break;
		case DB_KIND_ORACLE:	        // Oracle 数据库
			sqlstr2 =
				" SELECT A.STOCK_NO, A.STOCK_PLACE_NO, SUM(NVL(C.MAT_NUM, 0)) MAT_NUM, SUM(NVL(C.MAT_ACT_WT, 0)) MAT_WT"
				" FROM TWMA2 A LEFT OUTER JOIN TMMSM01 C"
				" ON  A.MAT_NO = C.MAT_NO"
				" WHERE 1 = 1";
			break;
		}
		sqlorderby1 =
			" GROUP BY A.STOCK_NO, A.STOCK_PLACE_NO"
			" ORDER BY A.STOCK_NO, A.STOCK_PLACE_NO";
		//sqlstr = sqlstr + sqlwhere;
		//sqlstr = sqlstr1 + sqlwhere + sqlorderby;
		sqlstr = sqlstr2 + sqlwhere + sqlorderby1;
		Log::Debug("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stock_no", twma1_q["STOCK_NO"].ToString());
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
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);

		cmd_inq.Close();
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

