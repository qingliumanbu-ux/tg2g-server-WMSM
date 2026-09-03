/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         SHIYONG
Version:		1.0
Date:			2016-02-01
Description:	直供出坯处理功能
**************************************************/
#include "stdafx.h"
//#include "tmmsm96.h"
//#include "tmmsm01.h"
//#include "twma0.h"
//#include "twma1.h"
//#include "twma2.h"

BM2_FUNCTION_EXPORT
int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
//int f_wm00_confm(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

//int f_ymsm_smv3(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
//int f_wm00_get_stock_info(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_wm00_queue(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
//int f_mmsm_get_temp(CString MAT_KIND, CString MAT_SHAPE_FLAG, CString PROD_TIME, CDecimal &CURRENT_TEMP, CDbConnection * conn);



BM2_FUNCTION_IMPORT
int f_wmsmsm_stock_in(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);
BM2_FUNCTION_IMPORT
int f_wmsmsm_stock_out(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);
BM2_FUNCTION_IMPORT
int f_wmsmsm_ps0099(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);
//BM2_FUNCTION_IMPORT
//int f_pshp_dhcr_del_sm(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);

int f_wmsmsm13_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	int doFlag = 0;
	CString sqlstr = " ";
	CString procDiv = "";
	CString unitCode = "";
	CDecimal matActThick = 0;
	CDecimal matActWidth = 0;
	CString stockNoTo = " ";
	CString vehicle_no = " ";

	CString v_datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


	//CTMMSM96 tmmsm96(conn);
	//CTMMSM01 tmmsm01(conn);
	//CTWMA0 twma0(conn);
	//CTWMA1 twma1(conn);
	//CTWMA2 twma2(conn);

	CModel tmmsm96 = CModel("TMMSM96");
	CModel tmmsm01 = CModel("TMMSM01");
	CModel twma0 = CModel("TWMA0");
	CModel twma1 = CModel("TMMSM01");
	CModel twma2 = CModel("TWMA2");
	CModel twm01("TWM01");



	CDbCommand cmd_inq(conn);

	EIClass bcls_rec_PSHP;
	bcls_rec_PSHP.Tables[0].set_TableName("PSHP");
	bcls_rec_PSHP.Tables[0].Columns.Add(DT_STRING, "MAT_NO");


	//调用仓库入库主函数
	EIClass bcls_stock_in;
	bcls_stock_in.Tables[0].set_TableName("WM_STOCK");
	bcls_stock_in.Tables[0].Columns.Add(twma0);
	bcls_stock_in.Tables[0].Columns.Add(twma2);
	bcls_stock_in.Tables[0].Rows.Clear();

	//调用仓库出库主函数
	EIClass bcls_stock_out;
	bcls_stock_out.Tables[0].set_TableName("WM_STOCK");
	bcls_stock_out.Tables[0].Columns.Add(twma0);
	bcls_stock_out.Tables[0].Columns.Add(twma2);
	bcls_stock_out.Tables[0].Rows.Clear();


	//入库队列
	EIClass bcls_stock_que;
	bcls_stock_que.Tables.Add("WM00QUE");
	bcls_stock_que.Tables["WM00QUE"].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");
	bcls_stock_que.Tables["WM00QUE"].Columns.Add(DT_STRING, "STOCK_OPER_ORDER_DIV");
	bcls_stock_que.Tables["WM00QUE"].Columns.Add(DT_STRING, "MAT_NO");
	bcls_stock_que.Tables["WM00QUE"].Columns.Add(DT_STRING, "MAT_NUM");
	bcls_stock_que.Tables["WM00QUE"].Columns.Add(DT_STRING, "PLAN_NO");
	bcls_stock_que.Tables["WM00QUE"].Columns.Add(DT_STRING, "PLAN_EXEC_SEQ_NO");
	bcls_stock_que.Tables["WM00QUE"].Columns.Add(DT_STRING, "STOCK_NO");
	bcls_stock_que.Tables["WM00QUE"].Columns.Add(DT_STRING, "TRANS_TOOL");
	bcls_stock_que.Tables["WM00QUE"].Columns.Add(DT_STRING, "PRE_UNIT_CODE");
	bcls_stock_que.Tables["WM00QUE"].Columns.Add(DT_STRING, "NEXT_UNIT_CODE");
	bcls_stock_que.Tables["WM00QUE"].Columns.Add(DT_STRING, "MAT_DESTION");
	bcls_stock_que.Tables["WM00QUE"].Columns.Add(DT_STRING, "OPER_FLAG");
	bcls_stock_que.Tables["WM00QUE"].Columns.Add(DT_STRING, "FROM_STOCK_NO");
	bcls_stock_que.Tables["WM00QUE"].Rows.Clear();


	//调用计划函数
	EIClass bcls_rec_wmps99;
	bcls_rec_wmps99.Tables[0].set_TableName("WMPS99");
	bcls_rec_wmps99.Tables[0].Columns.Add(twma1);
	bcls_rec_wmps99.Tables[0].Columns.Add(twma2);
	bcls_rec_wmps99.Tables[0].Rows.Clear();


	try
	{
		if (!bcls_rec->Tables.Contains("MM0099"))
		{
			bcls_rec->Tables.Add("MM0099");
		}

		////出入库实绩
		//if (!bcls_rec->Tables.Contains("SMV3"))
		//{
		//	bcls_rec->Tables.Add("SMV3");
		//	bcls_rec->Tables["SMV3"].Columns.Add(DT_STRING, "PROC_DIV");
		//	bcls_rec->Tables["SMV3"].Columns.Add(DT_STRING, "MAT_NO");
		//	bcls_rec->Tables["SMV3"].Columns.Add(DT_STRING, "MEASURE_WT_FLAG");
		//	bcls_rec->Tables["SMV3"].Columns.Add(DT_STRING, "VEHICLE_NO");
		//	bcls_rec->Tables["SMV3"].Columns.Add(DT_STRING, "FACTORY_DIV");
		//	bcls_rec->Tables["SMV3"].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");
		//	bcls_rec->Tables["SMV3"].Columns.Add(DT_STRING, "STOCK_NO_FROM");
		//	bcls_rec->Tables["SMV3"].Columns.Add(DT_STRING, "STOCK_NO_TO");
		//	bcls_rec->Tables["SMV3"].Columns.Add(DT_STRING, "TRNP_BILL_NO");
		//}

		procDiv = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString().Trim().ToUpper();

		if (procDiv == "END")
		{
#pragma region


			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				tmmsm01.Reset();
				twma2.Reset();

				tmmsm01["MAT_NO"] = bcls_rec->Tables[0].Rows[i]["MAT_NO"].ToString().Trim();
				if (!tmmsm01.Query())
				{
					strcpy(s.msg, "查询材料" + tmmsm01["MAT_NO"].ToString() + "信息出错。");
					throw CApplicationException(-1, s.msg, log.Location);
				}

				if (bcls_rec->Tables[0].Columns.Contains("VEHICLE_NO") == true)
				{
					vehicle_no = bcls_rec->Tables[0].Rows[0]["VEHICLE_NO"].ToString().Trim();
					Log::Trace("", __FUNCTION__, "汽运出库 vehicle_no【{0}】", vehicle_no);
				}

				twma2["MAT_NO"] = tmmsm01["MAT_NO"];
				twma2.Query("MAT_NO");


				//if (tmmsm01["HOT_SEND_FLAG"].ToString() == "0" ||
				//	tmmsm01["HOT_SEND_FLAG"].ToString().Trim() == "")
				//{
				//	strcpy(s.msg, "该材料" + tmmsm01["MAT_NO"].ToString() + "不能直送,请选择下线入库操作。");
				//	throw CApplicationException(-1, s.msg, log.Location);
				//}

				if (tmmsm01["SLABTOP_FLAG"].ToString().Trim() != "0" &&
					tmmsm01["SLABTOP_FLAG"].ToString().Trim() != "")
				{
					strcpy(s.msg, "该材料" + tmmsm01["MAT_NO"].ToString() + "已经出坯开始，不能进行出库操作。");
					throw CApplicationException(-1, s.msg, log.Location);
				}


				if (twma2["STOCK_PLACE_NO"].ToString().SubstringNE(0, 2) != "GD" &&
					twma2["STOCK_PLACE_NO"].ToString().Trim() != "")
				{
					strcpy(s.msg, "该材料信息已不在辊道，不能进行出坯结束操作。");
					throw CApplicationException(-1, s.msg, log.Location);
				}

				if (tmmsm01["MAT_DESTION"].ToString().Trim() == "")
				{
					strcpy(s.msg, "该材料去向不能为空。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmmsm01["MAT_DESTION"].ToString().Trim() == "20") //外供去向
				{
					strcpy(s.msg, "该材料是外卖去向，不能出库。");
					throw CApplicationException(-1, s.msg, log.Location);
				}

				//根据去向找目标库区
				stockNoTo = " ";
				sqlstr =
					" SELECT STOCK_NO FROM TWM000E"
					" WHERE EVENT_ID = '2X'"
					" AND NEXT_UNIT_CODE = @mat_destion";

				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("mat_destion", tmmsm01["MAT_DESTION"].ToString());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					stockNoTo = cmd_inq.GetString(1);
				}
				cmd_inq.Close();

				Log::Trace("", __FUNCTION__, "stockNoTo【{0}】", stockNoTo);

				if (stockNoTo.Trim() == "")
				{
					sprintf(s.msg, "去向【%s】未配置目标库区（TWM000E）。", (const char*)tmmsm01["MAT_DESTION"]);
					throw CApplicationException(-1, s.msg, log.Location);
				}

				//补入库
				sqlstr =
					" SELECT STOCK_NO FROM TWM000E"
					" WHERE EVENT_ID = @stock_oper_order"
					" AND UNIT_CODE = @unit_code";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("stock_oper_order", "1B");
				cmd_inq.Parameters.Set("unit_code", tmmsm01["UNIT_CODE"].ToString());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					twma2["STOCK_NO"] = cmd_inq.GetString(1);
				}
				else
				{
					sprintf(s.msg, "机组【%s】未配置目标库区（TWM000E）。", (const char*)tmmsm01["UNIT_CODE"]);
					throw CApplicationException(-1, s.msg, log.Location);
				}
				cmd_inq.Close();

				Log::Trace("", __FUNCTION__, "tmmsm01.STOCK_NO[{0}]", twma2["STOCK_NO"].ToString());


				sqlstr =
					" SELECT STOCK_PLACE_NO FROM TWM04"
					" WHERE STOCK_NO = @stock_no";
					//" AND UNIT_CODE = @unit_code"
					//" AND ENTRANCE_EXIT_DIV = '2'";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("stock_no", twma2["STOCK_NO"].ToString());
				cmd_inq.Parameters.Set("unit_code", tmmsm01["UNIT_CODE"].ToString());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					twma2["STOCK_PLACE_NO"] = cmd_inq.GetString(1);
				}
				else
				{
					sprintf(s.msg, "机组【%s】未配置热送辊道库位（TWM04）。", (const char*)tmmsm01["UNIT_CODE"]);
					throw CApplicationException(-1, s.msg, log.Location);
				}
				cmd_inq.Close();

				Log::Trace("", __FUNCTION__, "twma2.STOCK_PLACE_NO[{0}]", twma2["STOCK_PLACE_NO"].ToString());



				//调用仓库入库主函数
				bcls_stock_in.Tables["WM_STOCK"].Rows.Clear();
				bcls_stock_in.Tables["WM_STOCK"].Rows.Add();
				bcls_stock_in.Tables["WM_STOCK"].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"].ToString();
				bcls_stock_in.Tables["WM_STOCK"].Rows[0]["STOCK_OPER_ORDER"] = "1Z";//热送出库 模拟入库业务类型
				bcls_stock_in.Tables["WM_STOCK"].Rows[0]["STOCK_NO"] = twma2["STOCK_NO"].ToString();
				bcls_stock_in.Tables["WM_STOCK"].Rows[0]["STOCK_PLACE_NO"] = twma2["STOCK_PLACE_NO"].ToString();
				bcls_stock_in.Tables["WM_STOCK"].Rows[0]["ROWNO"] = " ";
				bcls_stock_in.Tables["WM_STOCK"].Rows[0]["COLUMN_NO"] = " ";
				bcls_stock_in.Tables["WM_STOCK"].Rows[0]["LAYERNO"] = 0;
				bcls_stock_in.Tables["WM_STOCK"].Rows[0]["STOCK_PLACE_POSITION"] = " ";



				doFlag = f_wmsmsm_stock_in(&bcls_stock_in, bcls_ret, conn);
				if (doFlag != 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}


				//调用仓库出库主函数
				bcls_stock_out.Tables["WM_STOCK"].Rows.Clear();
				bcls_stock_out.Tables["WM_STOCK"].Rows.Add();
				bcls_stock_out.Tables["WM_STOCK"].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"].ToString();
				bcls_stock_out.Tables["WM_STOCK"].Rows[0]["STOCK_OPER_ORDER"] = "2X";
				bcls_stock_out.Tables["WM_STOCK"].Rows[0]["STOCK_NO"] = stockNoTo;
				bcls_stock_out.Tables["WM_STOCK"].Rows[0]["STOCK_PLACE_NO"] = " ";
				bcls_stock_out.Tables["WM_STOCK"].Rows[0]["ROWNO"] = " ";
				bcls_stock_out.Tables["WM_STOCK"].Rows[0]["COLUMN_NO"] = " ";
				bcls_stock_out.Tables["WM_STOCK"].Rows[0]["LAYERNO"] = 0;
				bcls_stock_out.Tables["WM_STOCK"].Rows[0]["STOCK_PLACE_POSITION"] = " ";
				bcls_stock_out.Tables["WM_STOCK"].Rows[0]["VEHICLE_NO"] = vehicle_no;



				doFlag = f_wmsmsm_stock_out(&bcls_stock_out, bcls_ret, conn);
				if (doFlag != 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}

				twm01["STOCK_NO"] = stockNoTo;
				if (twm01.Query("STOCK_NO"))
				{
					Log::Trace("", __FUNCTION__, "twm01 有目标库区记录,同产线出库，生成入库队列。");

					//下分厂入库队列
					bcls_stock_que.Tables["WM00QUE"].Rows.Clear();
					bcls_stock_que.Tables["WM00QUE"].Rows.Add();
					bcls_stock_que.Tables["WM00QUE"].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"].ToString();
					bcls_stock_que.Tables["WM00QUE"].Rows[0]["MAT_NUM"] = tmmsm01["MAT_NUM"].ToDecimal();
					bcls_stock_que.Tables["WM00QUE"].Rows[0]["PLAN_NO"] = " ";
					bcls_stock_que.Tables["WM00QUE"].Rows[0]["PLAN_EXEC_SEQ_NO"] = 0;
					bcls_stock_que.Tables["WM00QUE"].Rows[0]["STOCK_NO"] = stockNoTo;
					bcls_stock_que.Tables["WM00QUE"].Rows[0]["TRANS_TOOL"] = " ";
					bcls_stock_que.Tables["WM00QUE"].Rows[0]["PRE_UNIT_CODE"] = tmmsm01["UNIT_CODE"].ToString();
					bcls_stock_que.Tables["WM00QUE"].Rows[0]["NEXT_UNIT_CODE"] = tmmsm01["NEXT_UNIT_CODE"].ToString();
					bcls_stock_que.Tables["WM00QUE"].Rows[0]["OPER_FLAG"] = "I";
					bcls_stock_que.Tables["WM00QUE"].Rows[0]["STOCK_OPER_ORDER"] = "1X";
					bcls_stock_que.Tables["WM00QUE"].Rows[0]["STOCK_OPER_ORDER_DIV"] = " ";
					bcls_stock_que.Tables["WM00QUE"].Rows[0]["FROM_STOCK_NO"] = twma2["STOCK_NO"].ToString();

					doFlag = f_wm00_queue(&bcls_stock_que, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				else
				{
					Log::Trace("", __FUNCTION__, "twm01 无目标库区记录,跨产线出库，不生成入库队列。");

				}

			}
#pragma endregion
		}
		
		else if (procDiv == "IN")
		{
#pragma region
			CString slatUnladeCause = bcls_rec->Tables[0].Rows[0]["SLAT_UNLADE_CAUSE"].ToString().Trim();



			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				twma1.Reset();
				twma2.Reset();

				twma1["MAT_NO"] = bcls_rec->Tables[0].Rows[i]["MAT_NO"].ToString().Trim();
				twma2["STOCK_PLACE_NO"] = bcls_rec->Tables[0].Rows[0]["STOCK_PLACE_NO"].ToString().Trim();

				if (!twma1.Query())
				{
					strcpy(s.msg, "查询材料" + twma1["MAT_NO"].ToString() + "信息出错。");
					throw CApplicationException(-1, s.msg, log.Location);
				}

				tmmsm01["MAT_NO"] = twma1["MAT_NO"].ToString();
				if (!tmmsm01.Query())
				{
					strcpy(s.msg, "查询材料信息出错，可能改材料已经出库。");
					throw CApplicationException(-1, s.msg, log.Location);
				}

				if (tmmsm01["HOT_CHARGE_FLAG"].ToString() == "2" && tmmsm01["MAT_DESTION"].ToString() == "18" && tmmsm01["PONO_SLAB_2"].ToString().Trim() == "")
				{

					bcls_rec_PSHP.Tables[0].Rows.Clear();
					bcls_rec_PSHP.Tables[0].Rows.Add();
					bcls_rec_PSHP.Tables[0].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"];

					//doFlag = f_pshp_dhcr_del_sm(&bcls_rec_PSHP, bcls_ret, conn);
					//if (doFlag < 0)
					//{
					//	throw CApplicationException(-1, s.msg, log.Location);
					//}

				}
				//if (twma1["HOT_SEND_FLAG"].ToString() == "0" ||
				//	twma1["HOT_SEND_FLAG"].ToString().Trim() == "")
				//{
				//	strcpy(s.msg, "该材料" + tmmsm01["MAT_NO"].ToString() + "不能直送,请选择下线入库操作。");
				//	throw CApplicationException(-1, s.msg, log.Location);
				//}

				if (tmmsm01["SLABTOP_FLAG"].ToString().Trim() != "0" &&
					tmmsm01["SLABTOP_FLAG"].ToString().Trim() != "")
				{
					strcpy(s.msg, "该材料" + tmmsm01["MAT_NO"].ToString() + "已经出坯开始，不能进行入库库操作。");
					throw CApplicationException(-1, s.msg, log.Location);
				}

				sqlstr =
					" SELECT STOCK_NO FROM TWM04"
					" WHERE STOCK_PLACE_NO = @stock_place_no";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("stock_place_no", twma2["STOCK_PLACE_NO"].ToString());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					twma2["STOCK_NO"] = cmd_inq.GetString(1);
				}
				else
				{
					strcpy(s.msg, "传入的库位号不能为空，或未配置。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				cmd_inq.Close();

				tmmsm01["HOT_SEND_FLAG"] = "0";
				tmmsm01["HOT_CHARGE_FLAG"] = "0";
				tmmsm01.Update("HOT_CHARGE_FLAG,HOT_SEND_FLAG,SLAT_UNLADE_CAUSE", "MAT_NO");
				//调用仓库入库主函数
				bcls_stock_in.Tables["WM_STOCK"].Rows.Clear();
				bcls_stock_in.Tables["WM_STOCK"].Rows.Add();
				bcls_stock_in.Tables["WM_STOCK"].Rows[0]["MAT_NO"] = twma1["MAT_NO"].ToString();
				bcls_stock_in.Tables["WM_STOCK"].Rows[0]["STOCK_OPER_ORDER"] = "1B";
				bcls_stock_in.Tables["WM_STOCK"].Rows[0]["STOCK_OPER_ORDER_DIV"] = "C";
				bcls_stock_in.Tables["WM_STOCK"].Rows[0]["STOCK_NO"] = twma2["STOCK_NO"].ToString();
				bcls_stock_in.Tables["WM_STOCK"].Rows[0]["STOCK_PLACE_NO"] = twma2["STOCK_PLACE_NO"].ToString();
				bcls_stock_in.Tables["WM_STOCK"].Rows[0]["ROWNO"] = " ";
				bcls_stock_in.Tables["WM_STOCK"].Rows[0]["COLUMN_NO"] = " ";
				bcls_stock_in.Tables["WM_STOCK"].Rows[0]["LAYERNO"] = 0;
				bcls_stock_in.Tables["WM_STOCK"].Rows[0]["STOCK_PLACE_POSITION"] = " ";

				doFlag = f_wmsmsm_stock_in(&bcls_stock_in, bcls_ret, conn);
				if (doFlag != 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}


				//8. PS0099（）
				bcls_rec_wmps99.Tables["WMPS99"].Rows.Clear();
				bcls_rec_wmps99.Tables["WMPS99"].Rows.Add();
				bcls_rec_wmps99.Tables["WMPS99"].Rows[0]["MAT_NO"] = twma1["MAT_NO"].ToString();
				bcls_rec_wmps99.Tables["WMPS99"].Rows[0]["STOCK_OPER_ORDER"] = "C";

				/*doFlag = f_wmsmsm_ps0099(&bcls_rec_wmps99, bcls_ret, conn);
				if (doFlag != 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}*/
			}

			//for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			//{
			//	tmmsm01.Reset();
			//	tmmsm01.MAT_NO = bcls_rec->Tables[0].Rows[i]["MAT_NO"].ToString().Trim();

			//	if (!tmmsm01.Query())
			//	{
			//		strcpy(s.msg, "查询材料信息出错，可能改材料已经出库。");
			//		throw CApplicationException(-1, s.msg, log.Location);
			//	}


			//	if (tmmsm01.STOCK_NO.Trim() == "")
			//	{
			//		sqlstr =
			//			" SELECT STOCK_NO FROM TWM04"
			//			" WHERE STOCK_PLACE_NO = @stock_place_no";
			//		cmd_inq.SetCommandText(sqlstr);
			//		cmd_inq.Parameters.Set("stock_place_no", tmmsm01.STOCK_PLACE_NO);
			//		cmd_inq.ExecuteReader();
			//		if (cmd_inq.Read())
			//		{
			//			tmmsm01.STOCK_NO = cmd_inq.GetString(1);
			//		}
			//		else
			//		{
			//			strcpy(s.msg, "传入的库位号不能为空。");
			//			throw CApplicationException(-1, s.msg, log.Location);
			//		}
			//		cmd_inq.Close();
			//	}

			//	if ((tmmsm01.HOT_SEND_FLAG.Trim() == "1" ||
			//		tmmsm01.HOT_CHARGE_FLAG.Trim() == "2") &&
			//		slatUnladeCause.Trim() == "")
			//	{
			//		strcpy(s.msg, "热送或热装材料下线需要选择下线理由");
			//		throw CApplicationException(-1, s.msg, log.Location);
			//	}

			//	cmd_inq.SetCommandText(
			//		" SELECT CUT_FIN_FLAG FROM TPSSM11"
			//		" INNER JOIN TMMSM33"
			//		" ON TPSSM11.HEAT_NO = TMMSM33.HEAT_NO"
			//		" WHERE TPSSM11.HEAT_NO = @tmmsm01.HEAT_NO"
			//		" AND TMMSM33.MAT_NO = @tmmsm01.MAT_NO");
			//	if (tmmsm01.MAT_SHAPE_FLAG == "2" &&
			//		tmmsm01.MAT_NO.GetLength() == 11)
			//	{
			//		cmd_inq.Parameters.Set("tmmsm01.HEAT_NO", tmmsm01.HEAT_NO);
			//		cmd_inq.Parameters.Set("tmmsm01.MAT_NO", tmmsm01.MAT_NO);
			//		cmd_inq.ExecuteReader();
			//		if (cmd_inq.Read())
			//		{
			//			if (cmd_inq.GetString(1) != "1")
			//			{
			//				strcpy(s.msg, "炉次母材料在入库操作前必须保证该炉切断完成。");
			//				throw CApplicationException(-1, s.msg, log.Location);
			//			}
			//		}
			//		cmd_inq.Close();
			//	}

			//	if (tmmsm01.IN_FLAG.Trim() == "1")
			//	{
			//		strcpy(s.msg, "该材料已经入库。");
			//		throw CApplicationException(-1, s.msg, log.Location);
			//	}

			//	//if (tmmsm01.STOCK_PLACE_NO.Trim() != "GD")
			//	//{
			//	//	strcpy(s.msg, "该材料信息已不在辊道，不能进行入库操作。");
			//	//	throw CApplicationException(-1, s.msg, log.Location);
			//	//}

			//	if (tmmsm01.HOLD_FLAG.Trim() != "0")
			//	{
			//		strcpy(s.msg, "该材料信息已封锁，不能进行入库操作。");
			//		throw CApplicationException(-1, s.msg, log.Location);
			//	}

			//	if (tmmsm01.SLABTOP_FLAG.Trim() != "0" &&
			//		tmmsm01.SLABTOP_FLAG.Trim() != "")
			//	{
			//		strcpy(s.msg, "该材料已经出坯开始，不能进行入库操作。");
			//		throw CApplicationException(-1, s.msg, log.Location);
			//	}

			//	if (bcls_rec->Tables.IndexOf("WM00_CONFM") < 0)
			//	{
			//		bcls_rec->Tables.Add("WM00_CONFM");
			//		bcls_rec->Tables["WM00_CONFM"].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");
			//		bcls_rec->Tables["WM00_CONFM"].Columns.Add(DT_STRING, "STOCK_OPER_ORDER_DIV");
			//		bcls_rec->Tables["WM00_CONFM"].Columns.Add(DT_STRING, "MAT_KIND");
			//		bcls_rec->Tables["WM00_CONFM"].Columns.Add(DT_STRING, "MAT_NO");
			//		bcls_rec->Tables["WM00_CONFM"].Columns.Add(DT_DECIMAL, "MAT_NUM");
			//		bcls_rec->Tables["WM00_CONFM"].Columns.Add(DT_STRING, "PLAN_NO");
			//		bcls_rec->Tables["WM00_CONFM"].Columns.Add(DT_STRING, "STOCK_NO");
			//		bcls_rec->Tables["WM00_CONFM"].Columns.Add(DT_STRING, "MAT_DEST");
			//		bcls_rec->Tables["WM00_CONFM"].Columns.Add(DT_STRING, "STOCK_PLACE_NO");
			//	}
			//	if (bcls_rec->Tables["WM00_CONFM"].Rows.get_Count() <= 0)
			//	{
			//		bcls_rec->Tables["WM00_CONFM"].Rows.Add();
			//	}

			//	bcls_rec->Tables["WM00_CONFM"].Rows[0]["STOCK_OPER_ORDER"] = "1X";
			//	bcls_rec->Tables["WM00_CONFM"].Rows[0]["STOCK_OPER_ORDER_DIV"] = "1";
			//	bcls_rec->Tables["WM00_CONFM"].Rows[0]["MAT_KIND"] = tmmsm01.MAT_KIND;
			//	bcls_rec->Tables["WM00_CONFM"].Rows[0]["MAT_NO"] = tmmsm01.MAT_NO;
			//	bcls_rec->Tables["WM00_CONFM"].Rows[0]["MAT_NUM"] = tmmsm01.MAT_NUM;
			//	bcls_rec->Tables["WM00_CONFM"].Rows[0]["STOCK_NO"] = tmmsm01.STOCK_NO;
			//	bcls_rec->Tables["WM00_CONFM"].Rows[0]["STOCK_PLACE_NO"] = bcls_rec->Tables[0].Rows[0]["STOCK_PLACE_NO"].ToString().Trim();
			//	Log::Trace("", "", "STOCK_PLACE_NO = {0}", bcls_rec->Tables[0].Rows[0]["STOCK_PLACE_NO"].ToString().Trim());

			//	doFlag = f_wm00_confm(bcls_rec, bcls_ret, conn);
			//	if (doFlag < 0)
			//	{
			//		throw CApplicationException(-1, s.msg, log.Location);
			//	}

			//	if (bcls_rec->Tables.IndexOf("WM00_STOCKINFO") < 0)
			//	{
			//		bcls_rec->Tables.Add("WM00_STOCKINFO");
			//		bcls_rec->Tables["WM00_STOCKINFO"].Columns.Add(DT_STRING, "MAT_NO");
			//	}
			//	if (bcls_rec->Tables["WM00_STOCKINFO"].Rows.get_Count() <= 0) bcls_rec->Tables["WM00_STOCKINFO"].Rows.Add();

			//	bcls_rec->Tables["WM00_STOCKINFO"].Rows[0]["MAT_NO"] = tmmsm01.MAT_NO;
			//	Log::Trace("", "", "MAT_NO = {0}", tmmsm01.MAT_NO);
			//	doFlag = f_wm00_get_stock_info(bcls_rec, bcls_ret, conn);
			//	if (doFlag < 0)
			//	{
			//		throw CApplicationException(-1, s.msg, log.Location);
			//	}

			//	tmmsm96.ROWNO = bcls_ret->Tables[0].Rows[0]["ROWNO"].ToString();
			//	tmmsm96.COLUMN_NO = bcls_ret->Tables[0].Rows[0]["COLUMN_NO"].ToString();
			//	tmmsm96.LAYERNO = bcls_ret->Tables[0].Rows[0]["LAYERNO"].ToDecimal();
			//	tmmsm96.FIELDNO = bcls_ret->Tables[0].Rows[0]["FIELDNO"].ToString();
			//	tmmsm96.HALL_NO = bcls_ret->Tables[0].Rows[0]["HALL_NO"].ToString();

			//	tmmsm96.EVENT_ID = "WM01";
			//	tmmsm96.EVENT_LINE_TYPE = "00";
			//	tmmsm96.SYSTEM_ID = "WMSM";
			//	tmmsm96.FUNC_ID = s.svc_name;
			//	tmmsm96.EVENT_DESC = "辊道下线入库";
			//	tmmsm96.FORM_NAME = s.formname;
			//	tmmsm96.MAT_NO = tmmsm01.MAT_NO;
			//	tmmsm96.IN_STOCK_TIME = CDateTime::Now().ToString("yyyyMMddHHmmss");
			//	tmmsm96.MAT_LINE_TYPE = tmmsm01.MAT_LINE_TYPE;
			//	tmmsm96.STOCK_NO = tmmsm01.STOCK_NO;
			//	tmmsm96.STOCK_PLACE_NO = bcls_rec->Tables[0].Rows[0]["STOCK_PLACE_NO"].ToString().Trim();
			//	tmmsm96.FACTORY_DIV = tmmsm01.FACTORY_DIV;
			//	tmmsm96.MergeTo(bcls_rec->Tables["MM0099"], false);
			//}

			//doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
			//if (doFlag != 0)
			//{
			//	throw CApplicationException(-1, s.msg, log.Location);
			//}
#pragma endregion
		}
	
	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "数据库处理出错，sqlcode=[{0}],请联系开发人员", arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}


