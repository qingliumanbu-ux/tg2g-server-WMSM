/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         李振
Version:		1.0
Date:			2016-03-05
Description:	待入库材料信息查询
**************************************************/

//框架头文件
#include "stdafx.h"
//#include "smhs.h"
//程序用头文件
//#include "twma1.h"

//函数申明


/*<remark>=========================================================
///<summary>
///待出库材料信息查询
///<para>
///2.排序方式：入库时间
///</para>
///<para>数据库表：TWMA2 倒躲队列；TWMA1 物料主档表
///<returns>返回符合查询条件的队列信息</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wmsmsm12_inq1);

int f_wmsmsm12_inq1(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
	CString s_table_name("TMMSM01");
	CDecimal d_thick_fr = 0;
	CDecimal d_thick_to = 0;

	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */
	//CTWMA1 twma1_q(conn);
	//CTWMA1 twma1(conn);
	//CTWMA1 twma1_1(conn);

	CModel twma1_q = CModel("TMMSM01");
	CModel twma1 = CModel("TMMSM01");
	CModel twma1_1 = CModel("TMMSM01");
	CModel twma2 = CModel("TWMA2");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	//系统的分页类信息。
	CPageInfo pageInfo;

	try
	{


		//返回数据信息
		bcls_ret->Tables[0].Columns.Add(twma2);
		bcls_ret->Tables[0].Columns.Add(twma1);


		twma1_q.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		twma1_q.TrimOrBlank();


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
		Log::Trace("", __FUNCTION__, "twma1_q.TRANSFER_PLAN_NO\t[{0}]", twma1_q["TRANSFER_PLAN_NO"].ToString());
		Log::Trace("", __FUNCTION__, "d_thick_fr\t[{0}]", d_thick_fr);
		Log::Trace("", __FUNCTION__, "d_thick_to\t[{0}]", d_thick_to);

		twma1_q["MAT_LINE_TYPE"] = "SM";
		twma1_q["MAT_KIND"] = "SM";



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
			sqlwhere += " AND TWMA1.HEAT_NO IN ('";
			sqlwhere += twma1_q["HEAT_NO"].ToString();
			sqlwhere += "')";
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
		if (twma1_q["DEV_CODE"].ToString().Trim() != "")
		{
			sqlwhere += " AND TWMA1.DEV_CODE = '" + twma1_q["DEV_CODE"].ToString() + "'";
		}

		if (twma1_q["MAT_NO"].ToString().Trim() != "")
		{
			sqlwhere += " AND TWMA1.MAT_NO IN ('";
			sqlwhere += twma1_q["MAT_NO"].ToString();
			sqlwhere += "')";
		}
		if (bcls_rec->Tables[0].Columns.Contains("LOAD_FLAG"))//预装车
		{
			if (bcls_rec->Tables[0].Rows[0]["LOAD_FLAG"].ToString() == "P") {
				sqlwhere += " AND TWMA1.PRE_LOAD_FLAG <>'1'  ";
			}

		}
		
		if (bcls_rec->Tables[0].Rows[0]["SLAB_CUT_TIME_F"].ToString().Trim() != "") {
			sqlwhere += " AND TWMA1.SLAB_CUT_TIME >='" + bcls_rec->Tables[0].Rows[0]["SLAB_CUT_TIME_F"].ToString().SubstringNE(0, 8) + "000000" + "'  ";
		}
		if (bcls_rec->Tables[0].Rows[0]["SLAB_CUT_TIME_T"].ToString().Trim() != "") {
			sqlwhere += " AND TWMA1.SLAB_CUT_TIME <='" + bcls_rec->Tables[0].Rows[0]["SLAB_CUT_TIME_T"].ToString().SubstringNE(0, 8) + "235959" + "'  ";
		}


		sqlwhere += " AND TWMA1.MAT_LINE_TYPE = 'SM'  ";

		sqlwhere += " AND TWMA1.MAT_ACT_THICK BETWEEN @thick_fr AND @thick_to ";

	

		if (bcls_rec->Tables[0].Rows[0]["IF_DB"].ToString() == "1") {
			s_table_name = "HMMSM01";
			sqlstr =
				" SELECT TWMA1.* "
				"	FROM " + s_table_name + " TWMA1, TWMSMZD02 T02 "
				" where TWMA1.GUIDE_DEST = T02.CODE AND T02.CODE_CLASS = 'WM02' AND T02.CODE_DESC_3_CONTENT LIKE '%5%'  AND TWMA1.LOGISTICS_STATUS NOT IN ('2','3') ";
		}
		else
		{
			sqlstr =
				" SELECT TWMA1.* "
				" FROM  " + s_table_name + " TWMA1 where 1=1 AND TWMA1.LOGISTICS_STATUS NOT IN ('2','3') AND TWMA1.STOCK_L2 NOT IN ('SS-HSM','CS-HSM','HF') ";
		}



		

		sqlwhere +=
			" ORDER BY TWMA1.STOCK_PLACE_NO ASC,TWMA1.LAYERNO DESC ";
		sqlstr = sqlstr + sqlwhere;
		Log::Trace("", __FUNCTION__, "sqlst[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stock_no", twma1_q["STOCK_NO"].ToString());
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

