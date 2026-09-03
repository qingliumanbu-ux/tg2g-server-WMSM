/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         JHZHAO
Version:		1.0
Date:			2016-02-01
Description:	直供出坯确认按熔炼号查询
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件
//#include "tmmsm01.h"
//#include "twma1.h"


//业务头文件


//函数申明

/*<remark>=========================================================
///<summary>
///直供出坯确认按熔炼号查询
///<para>
///</para>
///<para>数据库表：TMMSM01坯料主档表
///<returns>返回符合查询条件的熔炼号信息</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wmsmsm13_inq);

int f_wmsmsm13_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;

	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	CDecimal mat_act_thick_from = 0;
	CDecimal mat_act_thick_to = 0;
	CString v_ingot_code = "";

	CDecimal total_sum_wt = 0;

	/* 实体类定义 */
	//CTMMSM01 tmmsm01(conn);
	//CTWMA1 twma1_q(conn);
	CModel tmmsm01 = CModel("TMMSM01");
	CModel twma1_q = CModel("TMMSM01");


	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlstrCount = "";
	CString sqlwhere = "";//读材料信息
	CString prod_time_f = "";
	CString prod_time_t = "";

	CDecimal total_mat_wt = 0;
	CDecimal total_mat_num = 0;
	CDecimal out_mat_num = 0;
	CDecimal wait_mat_num = 0;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_1(conn);

	//系统的分页类信息。
	CPageInfo pageInfo;

	try
	{
		//分页信息
		CDataTable& table = bcls_ret->Tables.Add("PAGEINFO");
		table.Columns.Add(DT_DECIMAL, "recordsum");

		

		twma1_q.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		twma1_q.TrimOrBlank();

		Log::Trace("", __FUNCTION__, "twma1_q.MAT_NO\t[{0}]", twma1_q["MAT_NO"].ToString());
		Log::Trace("", __FUNCTION__, "twma1_q.FACTORY_DIV\t[{0}]", twma1_q["FACTORY_DIV"].ToString());
		if (bcls_rec->Tables[0].Columns.Contains("INGOT_CODE"))
			v_ingot_code = bcls_rec->Tables[0].Rows[0]["INGOT_CODE"];

		if (bcls_rec->Tables[0].Columns.Contains("MAT_THICK_FROM"))
			mat_act_thick_from = bcls_rec->Tables[0].Rows[0]["MAT_THICK_FROM"];
		if (bcls_rec->Tables[0].Columns.Contains("MAT_THICK_TO"))
			mat_act_thick_to = bcls_rec->Tables[0].Rows[0]["MAT_THICK_TO"];
		if (mat_act_thick_to == 0)
		{
			mat_act_thick_to = 9999;
		}

		sqlstr =
			" SELECT"
			" DISTINCT HEAT_INFO.HEAT_NO, HEAT_INFO.UNIT_CODE, HEAT_INFO.PONO,"
			" HEAT_INFO.MAT_SHAPE_FLAG, HEAT_INFO.ST_NO, HEAT_INFO.SG_SIGN, HEAT_INFO.ORDER_NO,HEAT_INFO.MAT_SHAPE_FLAG "
			" FROM TMMSM01 HEAT_INFO"
			" WHERE 1 = 1"
			//" AND IN_FLAG = '0'"
			//" AND (FACTORY_DIV LIKE 'SU%' OR FACTORY_DIV = ' ')"
			" AND MAT_LINE_TYPE = 'SM'"
			" AND MAT_KIND = 'SM'"
			" AND HOT_SEND_FLAG IN ('1', '2') "
			;

		if (twma1_q["MAT_NO"].ToString().Trim() != "")
		{
			sqlstr += " AND HEAT_INFO.MAT_NO IN ( ";
			sqlstr += twma1_q["MAT_NO"].ToString();
			sqlstr += " )";
		}

		if (twma1_q["ORDER_NO"].ToString().Trim() != "")
		{
			sqlstr += " AND HEAT_INFO.ORDER_NO LIKE @order_no ";
		}
		if (twma1_q["FACTORY_DIV"].ToString().Trim() != "")
		{
			sqlstr += " AND HEAT_INFO.FACTORY_DIV LIKE @factory_div ";
		}
		if (twma1_q["HEAT_NO"].ToString().Trim() != "")
		{
			sqlstr += " AND HEAT_INFO.HEAT_NO LIKE @heat_no ";
		}
		if (twma1_q["PONO"].ToString().Trim() != "")
		{
			sqlstr += " AND HEAT_INFO.PONO LIKE @pono ";
		}
		if (twma1_q["SG_SIGN"].ToString().Trim() != "")
		{
			sqlstr += " AND HEAT_INFO.SG_SIGN LIKE @sg_sign ";
		}
		if (twma1_q["ST_NO"].ToString().Trim() != "")
		{
			sqlstr += " AND HEAT_INFO.ST_NO LIKE @st_no ";
		}
		if (twma1_q["RAW_ORIGIN"].ToString().Trim() != "")
		{
			sqlstr += " AND HEAT_INFO.RAW_ORIGIN = @raw_origin ";
		}
		if (twma1_q["TRANSFER_PLAN_NO"].ToString().Trim() != "")
		{
			sqlstr += " AND HEAT_INFO.transfer_plan_no LIKE @transfer_plan_no ";
		}
		/*if (twma1_q["MAT_SHAPE_FLAG"].ToString().Trim() != "")
		{
			sqlstr += " AND HEAT_INFO.MAT_SHAPE_FLAG = @mat_shape_flag ";
			Log::Trace("", __FUNCTION__, "twma1_q.MAT_SHAPE_FLAG\t[{0}]", twma1_q["MAT_SHAPE_FLAG"].ToString());
		}*/
		if (v_ingot_code.Trim() != "")
		{
			sqlstr += " AND HEAT_INFO.INGOT_CODE LIKE @ingot_code ";
		}

		sqlwhere += " AND HEAT_INFO.MAT_ACT_THICK BETWEEN @mat_act_thick_from AND @mat_act_thick_to";

		sqlstrCount = "SELECT COUNT(1) FROM (" + sqlstr + ")";
		Log::Trace("", __FUNCTION__, "sqlstrCount[{0}]", (const char*)sqlstrCount);
		cmd_inq.SetCommandText(sqlstrCount);
		cmd_inq.Parameters.Set("factory_div", twma1_q["FACTORY_DIV"].ToString());
		cmd_inq.Parameters.Set("stock_no", twma1_q["STOCK_NO"].ToString());
		cmd_inq.Parameters.Set("cs_mat_no", twma1_q["MAT_NO"].ToString());
		cmd_inq.Parameters.Set("order_no", twma1_q["ORDER_NO"].ToString() + "%");
		cmd_inq.Parameters.Set("heat_no", twma1_q["HEAT_NO"].ToString() + "%");
		cmd_inq.Parameters.Set("pono", twma1_q["PONO"].ToString() + "%");
		cmd_inq.Parameters.Set("sg_sign", twma1_q["SG_SIGN"].ToString() + "%");
		cmd_inq.Parameters.Set("st_no", twma1_q["ST_NO"].ToString() + "%");
		cmd_inq.Parameters.Set("raw_origin", twma1_q["RAW_ORIGIN"].ToString());
		cmd_inq.Parameters.Set("mat_act_thick_from", mat_act_thick_from);
		cmd_inq.Parameters.Set("mat_act_thick_to", mat_act_thick_to);
		cmd_inq.Parameters.Set("transfer_plan_no", twma1_q["TRANSFER_PLAN_NO"].ToString() + "%");
		cmd_inq.Parameters.Set("ingot_code", v_ingot_code + "%");
		cmd_inq.Parameters.Set("mat_shape_flag", twma1_q["MAT_SHAPE_FLAG"].ToString());		
		rowCount = cmd_inq.ExecuteScalar();
		cmd_inq.Close();

		Log::Trace("", __FUNCTION__, "rowCount: {0}", rowCount);

		Log::Trace("", __FUNCTION__, "sqlstr[{0}]", (const char*)sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("factory_div", twma1_q["FACTORY_DIV"].ToString());
		cmd_inq.Parameters.Set("stock_no", twma1_q["STOCK_NO"].ToString());
		cmd_inq.Parameters.Set("cs_mat_no", twma1_q["MAT_NO"].ToString());
		cmd_inq.Parameters.Set("order_no", twma1_q["ORDER_NO"].ToString() + "%");
		cmd_inq.Parameters.Set("heat_no", twma1_q["HEAT_NO"].ToString() + "%");
		cmd_inq.Parameters.Set("pono", twma1_q["PONO"].ToString() + "%");
		cmd_inq.Parameters.Set("sg_sign", twma1_q["SG_SIGN"].ToString() + "%");
		cmd_inq.Parameters.Set("st_no", twma1_q["ST_NO"].ToString() + "%");
		cmd_inq.Parameters.Set("raw_origin", twma1_q["RAW_ORIGIN"].ToString());
		cmd_inq.Parameters.Set("mat_act_thick_from", mat_act_thick_from);
		cmd_inq.Parameters.Set("mat_act_thick_to", mat_act_thick_to);
		cmd_inq.Parameters.Set("transfer_plan_no", twma1_q["TRANSFER_PLAN_NO"].ToString() + "%");
		cmd_inq.Parameters.Set("ingot_code", v_ingot_code + "%");
		cmd_inq.Parameters.Set("mat_shape_flag", twma1_q["MAT_SHAPE_FLAG"].ToString());
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], pageInfo.RecordFrom, pageInfo.PageSize);
		cmd_inq.Close();

		//返回记录总数
		CDataRow& row1 = bcls_ret->Tables["PAGEINFO"].Rows.Add();
		row1["recordsum"] = rowCount.ToInt32();


		if (!bcls_ret->Tables[0].Columns.Contains("TOTAL_MAT_NUM"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "TOTAL_MAT_NUM");
		}
		if (!bcls_ret->Tables[0].Columns.Contains("WAIT_MAT_NUM"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "WAIT_MAT_NUM");
		}
		if (!bcls_ret->Tables[0].Columns.Contains("OUT_MAT_NUM"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "OUT_MAT_NUM");
		}

		for (int i = 0; i < bcls_ret->Tables[0].Rows.get_Count(); i++)
		{
			CString v_heat_no = bcls_ret->Tables[0].Rows[i]["HEAT_NO"].ToString();

			sqlstr =
				" SELECT SUM(MAT_NUM) FROM"
				" (SELECT COUNT(MAT_NO) MAT_NUM FROM TMMSM01 WHERE HEAT_NO = @heat_no"
				" UNION ALL SELECT COUNT(MAT_NO) MAT_NUM FROM HMMSM01 WHERE HEAT_NO = @heat_no)";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", v_heat_no);
			bcls_ret->Tables[0].Rows[i]["TOTAL_MAT_NUM"] = cmd_inq.ExecuteScalar();


			sqlstr =
				" SELECT SUM(MAT_NUM) FROM TMMSM01"
				" WHERE HEAT_NO = @heat_no"
				" AND MAT_LINE_TYPE = 'SM'";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", v_heat_no);
			bcls_ret->Tables[0].Rows[i]["WAIT_MAT_NUM"] = cmd_inq.ExecuteScalar();

			sqlstr =
				" SELECT SUM(MAT_NUM) FROM"
				" (SELECT COUNT(MAT_NO) MAT_NUM FROM TMMSM01 WHERE HEAT_NO = @heat_no"
				" AND MAT_LINE_TYPE <> 'SM'"
				" UNION ALL SELECT COUNT(MAT_NO) MAT_NUM FROM HMMSM01 WHERE HEAT_NO = @heat_no)";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", v_heat_no);
			bcls_ret->Tables[0].Rows[i]["OUT_MAT_NUM"] = cmd_inq.ExecuteScalar();
		}

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