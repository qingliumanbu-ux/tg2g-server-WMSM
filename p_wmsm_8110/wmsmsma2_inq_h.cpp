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

BM2F_ENTERACE(wmsmsma2_inq_h);

int f_wmsmsma2_inq_h(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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

		

		twma1_q.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		twma1_q.TrimOrBlank();

	

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

		


		

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			sqlstr =
				" SELECT T2.HEAT_NO,\
				PONO,\
				STOCK_NO,\
				ST_NO,\
				SG_SIGN,\
				ORDER_NO,\
				NVL(MIN(SLAB_CUT_TIME),' ')                                                                            SLAB_CUT_TIME,\
				CASE\
				WHEN MIN(T2.SLAB_CUT_TIME) != ' ' THEN CEIL(\
					(SYSDATE - TO_DATE(MIN(T2.SLAB_CUT_TIME), 'yyyy-mm-dd hh24-mi-ss')) * 24) ELSE 0 end TIME_SPAN_HOUR,\
 CASE\
				WHEN MIN(T2.SLAB_CUT_TIME) != ' ' AND CEIL(\
					(SYSDATE - TO_DATE(MIN(T2.SLAB_CUT_TIME), 'yyyy-mm-dd hh24-mi-ss')) *\
					24) > 72 THEN 1\
				ELSE 0 end                                                                                ZHILIU_FLAG,\
				SUM(T2.MAT_ACT_WT) AS                                                                         MAT_WT,\
				SUM(T2.MAT_NUM)    AS                                                                         MAT_NUM\
				FROM TMMSM01 T2\
				WHERE EXISTS(SELECT NULL FROM TWMA2 T1 WHERE T1.MAT_NO = T2.MAT_NO) "
				;
			break;
		}


		sqlgroup =
			" GROUP BY T2.HEAT_NO, T2.PONO, STOCK_NO, ST_NO, SG_SIGN, ORDER_NO "
			;



		sqlstr = sqlstr + sqlwhere + sqlgroup;
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

		 
		
		cmd_inq.Close();


		bcls_ret->Tables.Add();
		sqlstr = " select A.STOCK_NO, MAX(A.STOCK_CAPACITY_WT) STOCK_CAPACITY_WT, SUM(B.MAT_WT) SUM_WT\
			from twm01 A\
			LEFT JOIN TMMSM01 B ON A.STOCK_NO = B.STOCK_NO\
			WHERE 1 = 1\
			GROUP BY A.STOCK_NO\
			";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[1]);
		cmd_inq.Close();
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

