/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   吴新
Version:
Date:     2016-04-13
Description: 出入库履历查询
**************************************************/

/*<remark>=========================================================
/// <summary>
/// 出入库履历查询
/// <para>
/// 根据传入的库号、材料号，查询履历

/// </summary>
/// <param name="STOCK_NO">库号    </param>
/// <param name="MAT_NO">材料号    </param>
/// <param name="STOCK_NO_CLASS">库号分类	</param>
/// <param name="TRANSFER_PLAN_NO">转库计划号    </param>
/// <returns>转库计划</returns>
===========================================================</remark>*/

#include "stdafx.h" //框架头
//程序头文件
//#include "twma4.h"

int f_epes_get_auth_other(const char *iuser, int irestype, EIClass *bcls_ret, CDbConnection * conn);


// service入口
BM2F_ENTERACE(wmsmsma4_inq)

int f_wmsmsma4_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */
	//CTWMA4 twma4(conn);
	CModel twma4 = CModel("TWMA4");

	/* 业务变量 */

	CDecimal d_thick_fr = 0;
	CDecimal d_thick_to = 0;
	CString v_time_fr = "";
	CString v_time_to = "";

	/* 数据库SQL操作字符串 */
	CString sqlstr = "", sqlstr1 = " ";
	CString sqlwhere = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	//系统的分页类信息。
	CPageInfo pageInfo;

	try
	{
		//获取库区授权
		/*CString stock_no_auth = "' '";
		EIClass *bcls_auth = new EIClass;
		if (f_epes_get_auth_other(s.userid, 5, bcls_auth, conn) != 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
		for (int fetchRowCount = 0; fetchRowCount < bcls_auth->Tables[0].Rows.get_Count(); fetchRowCount++)
		{
			stock_no_auth += ", '" + bcls_auth->Tables[0].Rows[fetchRowCount]["name"].ToString() + "' ";
		}
		delete bcls_auth;*/


		//分页信息
		CDataTable& table = bcls_ret->Tables.Add("PAGEINFO");
		table.Columns.Add(DT_DECIMAL, "recordsum");

		



		twma4.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		twma4.TrimOrBlank();


		if (bcls_rec->Tables[0].Columns.Contains("MAT_THICK_FR"))
			d_thick_fr = bcls_rec->Tables[0].Rows[0]["MAT_THICK_FR"].ToDecimal();
		if (bcls_rec->Tables[0].Columns.Contains("MAT_THICK_TO"))
			d_thick_to = bcls_rec->Tables[0].Rows[0]["MAT_THICK_TO"].ToDecimal();

		if (bcls_rec->Tables[0].Columns.Contains("TIME_FR"))
			v_time_fr = bcls_rec->Tables[0].Rows[0]["TIME_FR"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("TIME_TO"))
			v_time_to = bcls_rec->Tables[0].Rows[0]["TIME_TO"].ToString();


		/* ***** 打印输入参数 ***** */


		/* 设置开始时刻和结束时刻 */
		if (v_time_fr != "")
		{
			v_time_fr = v_time_fr.SubstringNE(0, 8) + "000000";
		}
		if (v_time_to != "")
		{
			v_time_to = v_time_to.SubstringNE(0, 8) + "235959";
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


		if (twma4["STOCK_NO"].ToString().Trim() != "")
		{
			sqlwhere += " AND STOCK_NO = @stock_no";
		}

		if (twma4["STOCK_OPER_ORDER"].ToString().Trim() != "")
		{
			sqlwhere += " AND STOCK_OPER_ORDER = @stock_oper_order";
		}

		if (twma4["ORDER_NO"].ToString().Trim() != "")
		{
			sqlwhere += " AND ORDER_NO LIKE @order_no";
		}

		if (twma4["HEAT_NO"].ToString().Trim() != "")
		{
			sqlwhere += " AND HEAT_NO LIKE @heat_no";
		}

		if (twma4["PONO"].ToString().Trim() != "")
		{
			sqlwhere += " AND PONO LIKE @pono";
		}

		if (twma4["SG_SIGN"].ToString().Trim() != "")
		{
			sqlwhere += " AND SG_SIGN LIKE @sg_sign";
		}

		if (twma4["ST_NO"].ToString().Trim() != "")
		{
			sqlwhere += " AND ST_NO LIKE @st_no";
		}

		if (twma4["MAT_LINE_TYPE"].ToString().Trim() != "")
		{
			sqlwhere += " AND MAT_LINE_TYPE LIKE @mat_line_type";
		}

		sqlwhere += " AND MAT_ACT_THICK BETWEEN @thick_fr AND @thick_to ";

		if (twma4["MAT_NO"].ToString().Trim() != "")
		{
			sqlwhere += " AND MAT_NO IN ( ";
			sqlwhere += twma4["MAT_NO"].ToString();
			sqlwhere += " )";
		}

		if (v_time_fr != "")
		{
			sqlwhere += " AND EVENT_TIME >= @time_fr";
		}
		if (v_time_to != "")
		{
			sqlwhere += " AND EVENT_TIME <= @time_to";
		}

	/*	sqlwhere +=
			" AND STOCK_NO IN (" + stock_no_auth + ")";*/


		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			sqlstr = " SELECT COUNT(1) FROM TWMA4 WHERE 1=1 ";
			break;
		}
		sqlstr = sqlstr + sqlwhere;
		Log::Debug("", __FUNCTION__, "1111111111111111111111111sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stock_no", twma4["STOCK_NO"].ToString());
		cmd_inq.Parameters.Set("userid", s.userid);
		cmd_inq.Parameters.Set("stock_oper_order", twma4["STOCK_OPER_ORDER"].ToString());
		cmd_inq.Parameters.Set("order_no", twma4["ORDER_NO"].ToString() + "%");
		cmd_inq.Parameters.Set("heat_no", twma4["HEAT_NO"].ToString() + "%");
		cmd_inq.Parameters.Set("pono", twma4["PONO"].ToString() + "%");
		cmd_inq.Parameters.Set("sg_sign", twma4["SG_SIGN"].ToString() + "%");
		cmd_inq.Parameters.Set("st_no", twma4["ST_NO"].ToString() + "%");
		cmd_inq.Parameters.Set("mat_line_type", twma4["MAT_LINE_TYPE"].ToString());
		cmd_inq.Parameters.Set("mat_kind1", twma4["MAT_KIND"].ToString());
		cmd_inq.Parameters.Set("thick_fr", d_thick_fr);
		cmd_inq.Parameters.Set("thick_to", d_thick_to);
		cmd_inq.Parameters.Set("time_fr", v_time_fr);
		cmd_inq.Parameters.Set("time_to", v_time_to);
		rowCount = cmd_inq.ExecuteScalar();

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			sqlstr = " SELECT * FROM TWMA4 WHERE 1=1 ";

			break;
		}
		sqlwhere += " ORDER BY EVENT_TIME DESC";
		sqlstr = sqlstr + sqlwhere;


		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stock_no", twma4["STOCK_NO"].ToString());
		cmd_inq.Parameters.Set("userid", s.userid);
		cmd_inq.Parameters.Set("stock_oper_order", twma4["STOCK_OPER_ORDER"].ToString());
		cmd_inq.Parameters.Set("order_no", twma4["ORDER_NO"].ToString() + "%");
		cmd_inq.Parameters.Set("heat_no", twma4["HEAT_NO"].ToString() + "%");
		cmd_inq.Parameters.Set("pono", twma4["PONO"].ToString() + "%");
		cmd_inq.Parameters.Set("sg_sign", twma4["SG_SIGN"].ToString() + "%");
		cmd_inq.Parameters.Set("st_no", twma4["ST_NO"].ToString() + "%");
		cmd_inq.Parameters.Set("mat_line_type", twma4["MAT_LINE_TYPE"].ToString());
		cmd_inq.Parameters.Set("mat_kind1", twma4["MAT_KIND"].ToString());
		cmd_inq.Parameters.Set("thick_fr", d_thick_fr);
		cmd_inq.Parameters.Set("thick_to", d_thick_to);
		cmd_inq.Parameters.Set("time_fr", v_time_fr);
		cmd_inq.Parameters.Set("time_to", v_time_to);
		cmd_inq.ExecuteReader();



		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], pageInfo.RecordFrom, pageInfo.PageSize);


		//while (cmd_inq.Read())
		//{
		//	fetchRowCount++;

		//	if (fetchRowCount > (pageInfo.RecordFrom + pageInfo.PageSize))
		//	{
		//		Log::Trace("", __FUNCTION__, "超上限，break");
		//		break;
		//	}
		//	if (!((fetchRowCount > pageInfo.RecordFrom) && (fetchRowCount <= (pageInfo.RecordFrom + pageInfo.PageSize))))
		//	{
		//		continue;
		//	}

		//	twma4.Reset();
		//	cmd_inq.Fetch(twma4);
		//	twma4.MergeTo(bcls_ret->Tables[0], false);
		//}
		//cmd_inq.Close();

		//返回记录总数
		CDataRow& row1 = bcls_ret->Tables["PAGEINFO"].Rows.Add();
		row1["recordsum"] = rowCount.ToInt32();
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

