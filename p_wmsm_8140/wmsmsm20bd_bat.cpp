/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:
Version:		1.0
Date:			2016-03-05
Description:	日收发数据汇总
**************************************************/

//框架头文件
#include "stdafx.h"

//程序用头文件




/*<remark>=========================================================
///<summary>
///板坯出库功能
///<para>
///2.排序方式：
///</para>
///<para>数据库表：TWMA0 倒躲队列；TWMA1 物料主档表
///<returns>执行预材料预入库功能</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wmsmsm20bd_bat)
int f_wmsmsm20bd_bat(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	int count = 0;
	CString date_time = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CString v_factory_div = "";
	CString v_stock_no = "";
	CString s_date_from = "";
	CString s_date_to = "";
	CString s_factory_div = "";
	CString s_stock_no = "";
	CString s_seq_no = "";

	CDateTime dt_date;


	CString cal_time_from = "";
	CString cal_time_to = "";

	/* 实体类定义 */
	//CTWMA4 twma4(conn);
	//CTWM20A1 twm20a1(conn);
	//CTWM20A2 twm20a2(conn);
	//CTWM20A3 twm20a3(conn);
	//CTWM20BD twm20bd(conn);
	CModel twma4 = CModel("TWMA4");
	CModel twm20a1 = CModel("TWM20A1");
	CModel twm20a2 = CModel("TWM20A2");
	CModel twm20a3 = CModel("TWM20A3");
	CModel twm20bd = CModel("TWM20BD");


	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlstr_item = "";
	CString sqlstr_condition = "";
	CString sqlstr_group = "";


	/* 数据库操作类定义 */
	CDbCommand comm(conn);
	CDbCommand cmd_inq(conn);



	try
	{
		if (bcls_rec->Tables[0].Columns.Contains("FACTORY_DIV"))
		{
			v_factory_div = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString();
		}
		if (bcls_rec->Tables[0].Columns.Contains("STOCK_NO"))
		{
			v_stock_no = bcls_rec->Tables[0].Rows[0]["STOCK_NO"].ToString();
		}
		Log::Trace("", __FUNCTION__, "v_factory_div\t[{0}]", v_factory_div);
		Log::Trace("", __FUNCTION__, "v_stock_no\t[{0}]", v_stock_no);

		if (bcls_rec->Tables[0].Columns.Contains("DATE_FROM") &&
			bcls_rec->Tables[0].Columns.Contains("DATE_TO"))
		{
			s_date_from = bcls_rec->Tables[0].Rows[0]["DATE_FROM"].ToString().Substring(0, 8);
			s_date_to = bcls_rec->Tables[0].Rows[0]["DATE_TO"].ToString().Substring(0, 8);
		}
		else
		{
			s_date_from = CDateTime::Now().AddDays(-1).ToString("yyyyMMdd");
			s_date_to = CDateTime::Now().AddDays(-1).ToString("yyyyMMdd");
		}

		Log::Trace("", __FUNCTION__, "s_date_from\t[{0}]", s_date_from);
		Log::Trace("", __FUNCTION__, "date_to\t[{0}]", s_date_to);

		dt_date = CDateTime(
			atoi((const char*)s_date_from.Substring(0, 4)),
			atoi((const char*)s_date_from.Substring(4, 2)),
			atoi((const char*)s_date_from.Substring(6, 2)));

		Log::Trace("", __FUNCTION__, "dt_date\t[{0}]", dt_date.ToString("yyyyMMdd"));



		while (dt_date.ToString("yyyyMMdd") <= s_date_to)
		{
			cal_time_from = dt_date.ToString("yyyyMMdd") + "000000";
			cal_time_to = dt_date.ToString("yyyyMMdd") + "235959";
			Log::Trace("", __FUNCTION__, "cal_time_from\t[{0}]", cal_time_from);
			Log::Trace("", __FUNCTION__, "cal_time_to\t[{0}]", cal_time_to);


#pragma region 删除当前交易周期数据,如果有调整量，不删除
			sqlstr =
				" DELETE FROM TWM20BD T"
				" WHERE REPORT_DATE = @report_date"
				" AND ADJ_WT <> 0"
				" AND ADJ_NUM <> 0"
				" AND EXISTS (SELECT NULL FROM TWM01 T2"
				" WHERE T.STOCK_NO = T2.STOCK_NO"
				" AND T2.MAT_LINE_TYPE = 'SM'"
				" AND T2.MAT_KIND = 'SM')";

			if (v_factory_div.Trim() != "")
			{
				sqlstr += " AND FACTROY_DIV = @factory_div";
			}
			if (v_stock_no.Trim() != "")
			{
				sqlstr += " AND STOCK_NO = @stock_no";
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("factory_div", v_factory_div);
			cmd_inq.Parameters.Set("stock_no", v_stock_no);
			cmd_inq.Parameters.Set("report_date", cal_time_from.Substring(0, 8));
			cmd_inq.ExecuteNonQuery();


			sqlstr =
				" UPDATE TWM20BD T1 SET"
				" INSTOCK_01 = 0,"
				" INSTOCK_02 = 0,"
				" INSTOCK_03 = 0,"
				" INSTOCK_04 = 0,"
				" INSTOCK_05 = 0,"
				" INSTOCK_06 = 0,"
				" INSTOCK_07 = 0,"
				" INSTOCK_08 = 0,"
				" INSTOCK_09 = 0,"
				" INSTOCK_10 = 0,"
				" INSTOCK_11 = 0,"
				" INSTOCK_12 = 0,"
				" INSTOCK_13 = 0,"
				" INSTOCK_14 = 0,"
				" INSTOCK_15 = 0,"
				" INSTOCK_16 = 0,"
				" INSTOCK_17 = 0,"
				" INSTOCK_18 = 0,"
				" INSTOCK_19 = 0,"
				" INSTOCK_20 = 0,"
				" OUTSTOCK_01 = 0,"
				" OUTSTOCK_02 = 0,"
				" OUTSTOCK_03 = 0,"
				" OUTSTOCK_04 = 0,"
				" OUTSTOCK_05 = 0,"
				" OUTSTOCK_06 = 0,"
				" OUTSTOCK_07 = 0,"
				" OUTSTOCK_08 = 0,"
				" OUTSTOCK_09 = 0,"
				" OUTSTOCK_10 = 0,"
				" OUTSTOCK_11 = 0,"
				" OUTSTOCK_12 = 0,"
				" OUTSTOCK_13 = 0,"
				" OUTSTOCK_14 = 0,"
				" OUTSTOCK_15 = 0,"
				" OUTSTOCK_16 = 0,"
				" OUTSTOCK_17 = 0,"
				" OUTSTOCK_18 = 0,"
				" OUTSTOCK_19 = 0,"
				" OUTSTOCK_20 = 0,"
				" INSTOCK_NUM_01 = 0,"
				" INSTOCK_NUM_02 = 0,"
				" INSTOCK_NUM_03 = 0,"
				" INSTOCK_NUM_04 = 0,"
				" INSTOCK_NUM_05 = 0,"
				" INSTOCK_NUM_06 = 0,"
				" INSTOCK_NUM_07 = 0,"
				" INSTOCK_NUM_08 = 0,"
				" INSTOCK_NUM_09 = 0,"
				" INSTOCK_NUM_10 = 0,"
				" INSTOCK_NUM_11 = 0,"
				" INSTOCK_NUM_12 = 0,"
				" INSTOCK_NUM_13 = 0,"
				" INSTOCK_NUM_14 = 0,"
				" INSTOCK_NUM_15 = 0,"
				" INSTOCK_NUM_16 = 0,"
				" INSTOCK_NUM_17 = 0,"
				" INSTOCK_NUM_18 = 0,"
				" INSTOCK_NUM_19 = 0,"
				" INSTOCK_NUM_20 = 0,"
				" OUTSTOCK_NUM_01 = 0,"
				" OUTSTOCK_NUM_02 = 0,"
				" OUTSTOCK_NUM_03 = 0,"
				" OUTSTOCK_NUM_04 = 0,"
				" OUTSTOCK_NUM_05 = 0,"
				" OUTSTOCK_NUM_06 = 0,"
				" OUTSTOCK_NUM_07 = 0,"
				" OUTSTOCK_NUM_08 = 0,"
				" OUTSTOCK_NUM_09 = 0,"
				" OUTSTOCK_NUM_10 = 0,"
				" OUTSTOCK_NUM_11 = 0,"
				" OUTSTOCK_NUM_12 = 0,"
				" OUTSTOCK_NUM_13 = 0,"
				" OUTSTOCK_NUM_14 = 0,"
				" OUTSTOCK_NUM_15 = 0,"
				" OUTSTOCK_NUM_16 = 0,"
				" OUTSTOCK_NUM_17 = 0,"
				" OUTSTOCK_NUM_18 = 0,"
				" OUTSTOCK_NUM_19 = 0,"
				" OUTSTOCK_NUM_20 = 0,"
				" INIT_WT = 0,"
				" INIT_NUM = 0,"
				" END_WT = 0,"
				" END_NUM = 0"
				" WHERE REPORT_DATE = @report_date"
				" AND ADJ_WT <> 0"
				" AND ADJ_NUM <> 0"
				" AND EXISTS (SELECT NULL FROM TWM01 T2"
				" WHERE T1.STOCK_NO = T2.STOCK_NO"
				" AND T2.MAT_LINE_TYPE = 'SM'"
				" AND T2.MAT_KIND = 'SM')";

			if (v_factory_div.Trim() != "")
			{
				sqlstr += " AND FACTROY_DIV = @factory_div";
			}
			if (v_stock_no.Trim() != "")
			{
				sqlstr += " AND STOCK_NO = @stock_no";
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("factory_div", v_factory_div);
			cmd_inq.Parameters.Set("stock_no", v_stock_no);
			cmd_inq.Parameters.Set("report_date", cal_time_from.Substring(0, 8));
			cmd_inq.ExecuteNonQuery();
#pragma endregion

#pragma region 获取上一个交易周期的期末 作为本交易周期期初
			sqlstr =
				" SELECT"
				" T.END_WT AS INIT_WT, T.END_NUM AS INIT_NUM,"
				" T.*"
				" FROM TWM20BD T"
				" WHERE REPORT_DATE = @report_date"
				" AND END_WT <> 0"
				" AND EXISTS (SELECT NULL FROM TWM01 T2"
				" WHERE T1.STOCK_NO = T2.STOCK_NO"
				" AND T2.MAT_LINE_TYPE = 'SM'"
				" AND T2.MAT_KIND = 'SM')";
			if (v_factory_div.Trim() != "")
			{
				sqlstr += " AND FACTROY_DIV = @factory_div";
			}
			if (v_stock_no.Trim() != "")
			{
				sqlstr += " AND STOCK_NO = @stock_no";
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("report_date", dt_date.AddDays(-1).ToString("yyyyMMdd"));
			cmd_inq.Parameters.Set("factory_div", v_factory_div);
			cmd_inq.Parameters.Set("stock_no", v_stock_no);
			cmd_inq.ExecuteReader();
			while (cmd_inq.Read())
			{
				twm20bd.Reset();
				cmd_inq.Fetch(twm20bd);
				twm20bd["REPORT_DATE"] = cal_time_from.Substring(0, 8);
				twm20bd["REC_CREATOR"] = s.userid;


				count = twm20bd.QueryCount(
					"REPORT_DATE,"
					"FACTORY_DIV,"
					"STOCK_NO,"
					"ITEM_ENAME,"
					"CONDITION_01,"
					"CONDITION_02,"
					"CONDITION_03,"
					"CONDITION_04,"
					"CONDITION_05,"
					"CONDITION_06,"
					"CONDITION_07,"
					"CONDITION_08,"
					"CONDITION_09,"
					"CONDITION_10,"
					"CONDITION_11,"
					"CONDITION_12,"
					"CONDITION_13,"
					"CONDITION_14,"
					"CONDITION_15,"
					"CONDITION_16,"
					"CONDITION_17,"
					"CONDITION_18,"
					"CONDITION_19,"
					"CONDITION_20,"
					"CONDITION_21,"
					"CONDITION_22,"
					"CONDITION_23,"
					"CONDITION_24,"
					"CONDITION_25,"
					"CONDITION_26,"
					"CONDITION_27,"
					"CONDITION_28,"
					"CONDITION_29,"
					"CONDITION_30"
					);

				if (count == 0)
				{
					twm20bd.Insert();
				}
				else
				{
					twm20bd.Update(
						"REC_CREATOR,"
						"REC_CREATE_TIME,"
						"INIT_WT,"
						"INIT_NUM"
						,
						"REPORT_DATE,"
						"FACTORY_DIV,"
						"STOCK_NO,"
						"ITEM_ENAME,"
						"CONDITION_01,"
						"CONDITION_02,"
						"CONDITION_03,"
						"CONDITION_04,"
						"CONDITION_05,"
						"CONDITION_06,"
						"CONDITION_07,"
						"CONDITION_08,"
						"CONDITION_09,"
						"CONDITION_10,"
						"CONDITION_11,"
						"CONDITION_12,"
						"CONDITION_13,"
						"CONDITION_14,"
						"CONDITION_15,"
						"CONDITION_16,"
						"CONDITION_17,"
						"CONDITION_18,"
						"CONDITION_19,"
						"CONDITION_20,"
						"CONDITION_21,"
						"CONDITION_22,"
						"CONDITION_23,"
						"CONDITION_24,"
						"CONDITION_25,"
						"CONDITION_26,"
						"CONDITION_27,"
						"CONDITION_28,"
						"CONDITION_29,"
						"CONDITION_30");
				}

			}
			cmd_inq.Close();
#pragma endregion


			sqlstr =
				" SELECT FACTORY_DIV, STOCK_NO FROM TWM20A2 T1"
				" WHERE EXISTS(SELECT NULL FROM TWM20A3 T2"
				" WHERE T1.FACTORY_DIV = T2.FACTORY_DIV"
				" AND T1.STOCK_NO = T2.STOCK_NO)"
				" AND EXISTS (SELECT NULL FROM TWM01 T2"
				" WHERE T1.STOCK_NO = T2.STOCK_NO"
				" AND T2.MAT_LINE_TYPE = 'SM'"
				" AND T2.MAT_KIND = 'SM')";
			if (v_factory_div.Trim() != "")
			{
				sqlstr += " AND FACTROY_DIV = @factory_div";
			}
			if (v_stock_no.Trim() != "")
			{
				sqlstr += " AND STOCK_NO = @stock_no";
			}

			comm.SetCommandText(sqlstr);
			comm.Parameters.Set("factory_div", v_factory_div);
			comm.Parameters.Set("stock_no", v_stock_no);
			comm.ExecuteReader();
			while (comm.Read())
			{
				s_factory_div = comm.GetString(1);
				s_stock_no = comm.GetString(2);

				Log::Trace("", __FUNCTION__, "s_factory_div\t[{0}]", s_factory_div);
				Log::Trace("", __FUNCTION__, "s_stock_no\t[{0}]", s_stock_no);

#pragma region 拼sqlstr
				sqlstr_item = " SELECT";
				sqlstr_condition = " FROM TWMA4 WHERE 1 = 1";
				sqlstr_group = " GROUP BY 1,";

				sqlstr =
					" SELECT * FROM TWM20A1"
					" WHERE FACTORY_DIV = @factory_div"
					" AND STOCK_NO = @stock_no"
					" ORDER BY SEQ_NO";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("factory_div", s_factory_div);
				cmd_inq.Parameters.Set("stock_no", s_stock_no);
				cmd_inq.ExecuteReader();
				while (cmd_inq.Read())
				{
					cmd_inq.Fetch(twm20a1);

					s_seq_no.Format("%2d", twm20a1["SEQ_NO"].ToDecimal());
					Log::Trace("", __FUNCTION__, "testssssss seq_no[{0}]", s_seq_no);
					s_seq_no = twm20a1["SEQ_NO"].ToString();
					if (s_seq_no.GetLength() == 1)
					{
						s_seq_no = "0" + s_seq_no;
					}

					sqlstr_item += " ";
					sqlstr_item += twm20a1["ITEM_ENAME"].ToString();
					sqlstr_item += " AS CONDITION_";
					sqlstr_item += s_seq_no;
					sqlstr_item += ",";

					sqlstr_group += " ";
					sqlstr_group += twm20a1["ITEM_ENAME"].ToString();
					sqlstr_group += ",";
				}
				cmd_inq.Close();


				if (s_factory_div.Trim() != "")
				{
					sqlstr_condition += " AND FACTORY_DIV = @factory_div";
				}
				if (s_stock_no.Trim() != "")
				{
					sqlstr_condition += " AND STOCK_NO = @stock_no";
				}
				sqlstr_condition += " AND REC_CREATE_TIME >= @time_from";
				sqlstr_condition += " AND REC_CREATE_TIME <= @time_to";


				sqlstr =
					" SELECT * FROM TWM20A2"
					" WHERE FACTORY_DIV = @factory_div"
					" AND STOCK_NO = @stock_no"
					" ORDER BY SEQ_NO";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("factory_div", s_factory_div);
				cmd_inq.Parameters.Set("stock_no", s_stock_no);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					cmd_inq.Fetch(twm20a2);
				}
				else
				{
					twm20a2["ITEM_ENAME"] = "MAT_ACT_WT";
				}
				cmd_inq.Close();



				sqlstr =
					" SELECT * FROM TWM20A3"
					" WHERE FACTORY_DIV = @factory_div"
					" AND STOCK_NO = @stock_no"
					" ORDER BY IN_OUT_DIV, SEQ_NO";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("factory_div", s_factory_div);
				cmd_inq.Parameters.Set("stock_no", s_stock_no);
				cmd_inq.ExecuteReader();
				while (cmd_inq.Read())
				{
					cmd_inq.Fetch(twm20a3);


					s_seq_no = twm20a3["SEQ_NO"].ToString();
					if (s_seq_no.GetLength() == 1)
					{
						s_seq_no = "0" + s_seq_no;
					}

					sqlstr_item += " SUM(CASE WHEN (";
					sqlstr_item += twm20a3["CND_RELATION"].ToString();
					sqlstr_item += ") THEN ";
					sqlstr_item += twm20a2["ITEM_ENAME"].ToString();
					sqlstr_item += " ELSE 0 END) AS";
					if (twm20a3["IN_OUT_DIV"].ToString().Trim() == "1")
					{
						sqlstr_item += " INSTOCK_";
					}
					else if (twm20a3["IN_OUT_DIV"].ToString().Trim() == "2")
					{
						sqlstr_item += " OUTSTOCK_";
					}
					sqlstr_item += s_seq_no;
					sqlstr_item += ",";



					sqlstr_item += " SUM(CASE WHEN (";
					sqlstr_item += twm20a3["CND_RELATION"].ToString();
					sqlstr_item += ") THEN MAT_NUM ELSE 0 END) AS";
					if (twm20a3["IN_OUT_DIV"].ToString().Trim() == "1")
					{
						sqlstr_item += " INSTOCK_NUM_";
					}
					else if (twm20a3["IN_OUT_DIV"].ToString().Trim() == "2")
					{
						sqlstr_item += " OUTSTOCK_NUM_";
					}
					sqlstr_item += s_seq_no;
					sqlstr_item += ",";
				}
				cmd_inq.Close();

				sqlstr_item = sqlstr_item.TrimRight(',');
				sqlstr_group = sqlstr_group.TrimRight(',');
				sqlstr = sqlstr_item + sqlstr_condition + sqlstr_group;
#pragma endregion

				Log::Trace("", __FUNCTION__, "sqlstr\t[{0}]", sqlstr);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("factory_div", s_factory_div);
				cmd_inq.Parameters.Set("stock_no", s_stock_no);
				cmd_inq.Parameters.Set("time_from", cal_time_from);
				cmd_inq.Parameters.Set("time_to", cal_time_to);
				cmd_inq.ExecuteReader();
				while (cmd_inq.Read())
				{
					twm20bd.Reset();
					cmd_inq.Fetch(twm20bd);
					twm20bd["REPORT_DATE"] = cal_time_from.Substring(0, 8);
					twm20bd["REC_CREATE_TIME"] = date_time;
					twm20bd["REC_CREATOR"] = s.userid;
					twm20bd["FACTORY_DIV"] = s_factory_div;
					twm20bd["STOCK_NO"] = s_stock_no;


#pragma region 新增或更新当天数据
					count = twm20bd.QueryCount(
						"REPORT_DATE,"
						"FACTORY_DIV,"
						"STOCK_NO,"
						"ITEM_ENAME,"
						"CONDITION_01,"
						"CONDITION_02,"
						"CONDITION_03,"
						"CONDITION_04,"
						"CONDITION_05,"
						"CONDITION_06,"
						"CONDITION_07,"
						"CONDITION_08,"
						"CONDITION_09,"
						"CONDITION_10,"
						"CONDITION_11,"
						"CONDITION_12,"
						"CONDITION_13,"
						"CONDITION_14,"
						"CONDITION_15,"
						"CONDITION_16,"
						"CONDITION_17,"
						"CONDITION_18,"
						"CONDITION_19,"
						"CONDITION_20,"
						"CONDITION_21,"
						"CONDITION_22,"
						"CONDITION_23,"
						"CONDITION_24,"
						"CONDITION_25,"
						"CONDITION_26,"
						"CONDITION_27,"
						"CONDITION_28,"
						"CONDITION_29,"
						"CONDITION_30"
						);

					if (count == 0)
					{
						twm20bd.Insert();
					}
					else
					{
						twm20bd.Update(
							"REC_CREATOR,"
							"REC_CREATE_TIME,"
							"INSTOCK_01,"
							"INSTOCK_02,"
							"INSTOCK_03,"
							"INSTOCK_04,"
							"INSTOCK_05,"
							"INSTOCK_06,"
							"INSTOCK_07,"
							"INSTOCK_08,"
							"INSTOCK_09,"
							"INSTOCK_10,"
							"INSTOCK_11,"
							"INSTOCK_12,"
							"INSTOCK_13,"
							"INSTOCK_14,"
							"INSTOCK_15,"
							"INSTOCK_16,"
							"INSTOCK_17,"
							"INSTOCK_18,"
							"INSTOCK_19,"
							"INSTOCK_20,"
							"OUTSTOCK_01,"
							"OUTSTOCK_02,"
							"OUTSTOCK_03,"
							"OUTSTOCK_04,"
							"OUTSTOCK_05,"
							"OUTSTOCK_06,"
							"OUTSTOCK_07,"
							"OUTSTOCK_08,"
							"OUTSTOCK_09,"
							"OUTSTOCK_10,"
							"OUTSTOCK_11,"
							"OUTSTOCK_12,"
							"OUTSTOCK_13,"
							"OUTSTOCK_14,"
							"OUTSTOCK_15,"
							"OUTSTOCK_16,"
							"OUTSTOCK_17,"
							"OUTSTOCK_18,"
							"OUTSTOCK_19,"
							"OUTSTOCK_20,"
							"INSTOCK_NUM_01,"
							"INSTOCK_NUM_02,"
							"INSTOCK_NUM_03,"
							"INSTOCK_NUM_04,"
							"INSTOCK_NUM_05,"
							"INSTOCK_NUM_06,"
							"INSTOCK_NUM_07,"
							"INSTOCK_NUM_08,"
							"INSTOCK_NUM_09,"
							"INSTOCK_NUM_10,"
							"INSTOCK_NUM_11,"
							"INSTOCK_NUM_12,"
							"INSTOCK_NUM_13,"
							"INSTOCK_NUM_14,"
							"INSTOCK_NUM_15,"
							"INSTOCK_NUM_16,"
							"INSTOCK_NUM_17,"
							"INSTOCK_NUM_18,"
							"INSTOCK_NUM_19,"
							"INSTOCK_NUM_20,"
							"OUTSTOCK_NUM_01,"
							"OUTSTOCK_NUM_02,"
							"OUTSTOCK_NUM_03,"
							"OUTSTOCK_NUM_04,"
							"OUTSTOCK_NUM_05,"
							"OUTSTOCK_NUM_06,"
							"OUTSTOCK_NUM_07,"
							"OUTSTOCK_NUM_08,"
							"OUTSTOCK_NUM_09,"
							"OUTSTOCK_NUM_10,"
							"OUTSTOCK_NUM_11,"
							"OUTSTOCK_NUM_12,"
							"OUTSTOCK_NUM_13,"
							"OUTSTOCK_NUM_14,"
							"OUTSTOCK_NUM_15,"
							"OUTSTOCK_NUM_16,"
							"OUTSTOCK_NUM_17,"
							"OUTSTOCK_NUM_18,"
							"OUTSTOCK_NUM_19,"
							"OUTSTOCK_NUM_20"
							,
							"REPORT_DATE,"
							"FACTORY_DIV,"
							"STOCK_NO,"
							"ITEM_ENAME,"
							"CONDITION_01,"
							"CONDITION_02,"
							"CONDITION_03,"
							"CONDITION_04,"
							"CONDITION_05,"
							"CONDITION_06,"
							"CONDITION_07,"
							"CONDITION_08,"
							"CONDITION_09,"
							"CONDITION_10,"
							"CONDITION_11,"
							"CONDITION_12,"
							"CONDITION_13,"
							"CONDITION_14,"
							"CONDITION_15,"
							"CONDITION_16,"
							"CONDITION_17,"
							"CONDITION_18,"
							"CONDITION_19,"
							"CONDITION_20,"
							"CONDITION_21,"
							"CONDITION_22,"
							"CONDITION_23,"
							"CONDITION_24,"
							"CONDITION_25,"
							"CONDITION_26,"
							"CONDITION_27,"
							"CONDITION_28,"
							"CONDITION_29,"
							"CONDITION_30");
					}
#pragma endregion


				}
				cmd_inq.Close();
			}
			comm.Close();

#pragma region 计算当天期末数据
			sqlstr =
				" UPDATE TWM20BD T1 SET"
				" END_WT = INIT_WT"
				" + INSTOCK_01"
				" + INSTOCK_02"
				" + INSTOCK_03"
				" + INSTOCK_04"
				" + INSTOCK_05"
				" + INSTOCK_06"
				" + INSTOCK_07"
				" + INSTOCK_08"
				" + INSTOCK_09"
				" + INSTOCK_10"
				" + INSTOCK_11"
				" + INSTOCK_12"
				" + INSTOCK_13"
				" + INSTOCK_14"
				" + INSTOCK_15"
				" + INSTOCK_16"
				" + INSTOCK_17"
				" + INSTOCK_18"
				" + INSTOCK_19"
				" + INSTOCK_20"
				" - OUTSTOCK_01"
				" - OUTSTOCK_02"
				" - OUTSTOCK_03"
				" - OUTSTOCK_04"
				" - OUTSTOCK_05"
				" - OUTSTOCK_06"
				" - OUTSTOCK_07"
				" - OUTSTOCK_08"
				" - OUTSTOCK_09"
				" - OUTSTOCK_10"
				" - OUTSTOCK_11"
				" - OUTSTOCK_12"
				" - OUTSTOCK_13"
				" - OUTSTOCK_14"
				" - OUTSTOCK_15"
				" - OUTSTOCK_16"
				" - OUTSTOCK_17"
				" - OUTSTOCK_18"
				" - OUTSTOCK_19"
				" - OUTSTOCK_20"
				" + ADJ_WT"
				","
				" END_NUM = INIT_NUM"
				" + INSTOCK_NUM_01"
				" + INSTOCK_NUM_02"
				" + INSTOCK_NUM_03"
				" + INSTOCK_NUM_04"
				" + INSTOCK_NUM_05"
				" + INSTOCK_NUM_06"
				" + INSTOCK_NUM_07"
				" + INSTOCK_NUM_08"
				" + INSTOCK_NUM_09"
				" + INSTOCK_NUM_10"
				" + INSTOCK_NUM_11"
				" + INSTOCK_NUM_12"
				" + INSTOCK_NUM_13"
				" + INSTOCK_NUM_14"
				" + INSTOCK_NUM_15"
				" + INSTOCK_NUM_16"
				" + INSTOCK_NUM_17"
				" + INSTOCK_NUM_18"
				" + INSTOCK_NUM_19"
				" + INSTOCK_NUM_20"
				" - OUTSTOCK_NUM_01"
				" - OUTSTOCK_NUM_02"
				" - OUTSTOCK_NUM_03"
				" - OUTSTOCK_NUM_04"
				" - OUTSTOCK_NUM_05"
				" - OUTSTOCK_NUM_06"
				" - OUTSTOCK_NUM_07"
				" - OUTSTOCK_NUM_08"
				" - OUTSTOCK_NUM_09"
				" - OUTSTOCK_NUM_10"
				" - OUTSTOCK_NUM_11"
				" - OUTSTOCK_NUM_12"
				" - OUTSTOCK_NUM_13"
				" - OUTSTOCK_NUM_14"
				" - OUTSTOCK_NUM_15"
				" - OUTSTOCK_NUM_16"
				" - OUTSTOCK_NUM_17"
				" - OUTSTOCK_NUM_18"
				" - OUTSTOCK_NUM_19"
				" - OUTSTOCK_NUM_20"
				" + ADJ_NUM"
				" WHERE REPORT_DATE = @report_date"
				" AND EXISTS (SELECT NULL FROM TWM01 T2"
				" WHERE T1.STOCK_NO = T2.STOCK_NO"
				" AND T2.MAT_LINE_TYPE = 'SM'"
				" AND T2.MAT_KIND = 'SM')"
				;
			if (v_factory_div.Trim() != "")
			{
				sqlstr += " AND FACTROY_DIV = @factory_div";
			}
			if (v_stock_no.Trim() != "")
			{
				sqlstr += " AND STOCK_NO = @stock_no";
			}
			comm.SetCommandText(sqlstr);
			comm.Parameters.Set("report_date", cal_time_from.Substring(0, 8));
			comm.Parameters.Set("factory_div", v_factory_div);
			comm.Parameters.Set("stock_no", v_stock_no);
			comm.ExecuteNonQuery();

#pragma endregion
			dt_date = dt_date.AddDays(1);
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
		Log::Trace("", __FUNCTION__, "s.flag[{0}]", s.flag);
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

