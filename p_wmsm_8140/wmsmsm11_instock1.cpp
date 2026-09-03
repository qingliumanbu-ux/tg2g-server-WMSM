/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:
Version:		1.0
Date:			2016-03-05
Description:	板坯入库功能
**************************************************/

//框架头文件
#include "stdafx.h"

//程序用头文件
//#include "twma0.h"//倒垛队列
//#include "twma1.h"//物料信息
//#include "twma2.h"//库位跟踪表


using namespace BM2;
using namespace BM2::Data;
using namespace BM2::Data::DbClient;

//函数申明
BM2_FUNCTION_IMPORT
int f_mmsm99(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);

int f_wmsmsm_stock_in(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);

//int f_cm_7z8t04_snd(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);
/*<remark>=========================================================
///<summary>
///板坯入库功能
///<para>
///2.排序方式：
///</para>
///<para>数据库表：TWMA0 倒躲队列；TWMA1 物料主档表
///<returns>执行预材料预入库功能</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wmsmsm11_instock1);
int f_wmsmsm11_instock1(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	int ii = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CDecimal rowCount = 0;

	/*程序用变量*/
	CString stock_no = "";
	CString stock_place_no = "";
	CString vehicle_no = "";
	CString stock_oper_order_div = "";//业务类型内区分
	CString stock_place_position = "";//位置信息
	CString store_keeper = " ";//现场管料
	CString crane_resp = " ";//行车工
	CString crane_no = " ";//吊车号
	CString slatUnladeCause = "";//下线原因

	CString layerno = "";
	CDecimal layerno1 = 0;


	/* 实体类定义 */
	//CTWMA0 twma0(conn);
	//CTWMA1 twma1(conn);
	//CTWMA2 twma2(conn);
	Log::Trace("", __FUNCTION__, "11111111111111");
	CModel twma0 = CModel("TWMA0");
	Log::Trace("", __FUNCTION__, "222222222222");
	CModel twma1 = CModel("TMMSM01");
	CModel twma2 = CModel("TWMA2");
	CModel twm01 = CModel("TWM01");
	CModel tsi0021 = CModel("TSI0021");


	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";
	CString sqlStr = "";

	CString MEASURE_WT_FLAG = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);


	//调用仓库入库主函数
	EIClass bcls_stock_in;
	bcls_stock_in.Tables[0].set_TableName("WM_STOCK");
	bcls_stock_in.Tables[0].Columns.Add(twma0);
	bcls_stock_in.Tables[0].Columns.Add(twma2);
	bcls_stock_in.Tables[0].Rows.Clear();

	EIClass bcls_LoaCon;
	bcls_LoaCon.Tables[0].set_TableName("7Z8T04");
	bcls_LoaCon.Tables["7Z8T04"].Columns.Add(DT_STRING, "DEAL_FLAG");
	bcls_LoaCon.Tables["7Z8T04"].Columns.Add(DT_STRING, "BUSI_TYPE");
	bcls_LoaCon.Tables["7Z8T04"].Columns.Add(DT_STRING, "TRUCK_NO");
	bcls_LoaCon.Tables["7Z8T04"].Columns.Add(DT_STRING, "TRUCKBOARD_NO");
	bcls_LoaCon.Tables["7Z8T04"].Columns.Add(DT_STRING, "UNLOAD_CODE_AREA");
	bcls_LoaCon.Tables["7Z8T04"].Columns.Add(DT_STRING, "UNLOAD_NAME_AREA");
	bcls_LoaCon.Tables["7Z8T04"].Columns.Add(DT_STRING, "UNLOAD_CODE");
	bcls_LoaCon.Tables["7Z8T04"].Columns.Add(DT_STRING, "UNLOAD_NAME");
	bcls_LoaCon.Tables["7Z8T04"].Columns.Add(DT_STRING, "EMP_CODE");
	bcls_LoaCon.Tables["7Z8T04"].Columns.Add(DT_STRING, "EMP_NAME");
	bcls_LoaCon.Tables["7Z8T04"].Columns.Add(DT_STRING, "PLAN_NO");
	bcls_LoaCon.Tables["7Z8T04"].Columns.Add(DT_STRING, "ATTENTN_NO");
	bcls_LoaCon.Tables["7Z8T04"].Columns.Add(DT_STRING, "MAT_NO");
	bcls_LoaCon.Tables["7Z8T04"].Columns.Add(DT_STRING, "SG_SIGN");
	bcls_LoaCon.Tables["7Z8T04"].Columns.Add(DT_DECIMAL, "MAT_ACT_LEN");
	bcls_LoaCon.Tables["7Z8T04"].Columns.Add(DT_DECIMAL, "MAT_ACT_WIDTH");
	bcls_LoaCon.Tables["7Z8T04"].Columns.Add(DT_DECIMAL, "MAT_ACT_THICK");
	bcls_LoaCon.Tables["7Z8T04"].Columns.Add(DT_DECIMAL, "MAT_ACT_WT");

	EIClass bcls_rec_QM17;//材料质量封锁
	bcls_rec_QM17.Tables[0].set_TableName("MM0099");
	bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "EVENT_ID");
	bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");
	bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "SYSTEM_ID");
	bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "FUNC_ID");
	bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
	//bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "HOLD_REMARK");
	bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "REL_REMARK");
	bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "REL_MAKER");
	bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "REL_TIME");
	bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "HOLD_CAUSE_CODE");
	bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "DEFECT_CLASS");

	try
	{

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			if (bcls_rec->Tables[0].Columns.Contains("SLAT_UNLADE_CAUSE"))
			{
				slatUnladeCause = bcls_rec->Tables[0].Rows[0]["SLAT_UNLADE_CAUSE"].ToString().Trim();
			}

			twma0.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			vehicle_no = bcls_rec->Tables[0].Rows[i]["VEHICLE_NO"].ToString();

			stock_oper_order_div = bcls_rec->Tables[0].Rows[0]["STOCK_OPER_ORDER_DIV"].ToString();

			/*if (bcls_rec->Tables[1].Columns.Contains("STOCK_NO"))
			{
				stock_no = bcls_rec->Tables[0].Rows[0]["STOCK_NO"].ToString();
				stock_place_no = bcls_rec->Tables[0].Rows[0]["STOCK_PLACE_NO"].ToString();
			}
			else
			{*/
				//一品一地库位入库时 取grid中的库位
				stock_no = bcls_rec->Tables[0].Rows[i]["TO_STOCK_NO"].ToString();
				stock_place_no = bcls_rec->Tables[0].Rows[i]["TO_STOCK_PLACE_NO"].ToString();
			//}


			if (bcls_rec->Tables[0].Columns.Contains("STOCK_PLACE_POSITION"))
			{
				stock_place_position = bcls_rec->Tables[0].Rows[0]["STOCK_PLACE_POSITION"].ToString();
			}

			if (stock_place_position.Trim().GetLength() == 0)
			{
				stock_place_position = "1";
			}

			if (bcls_rec->Tables[0].Columns.Contains("STORE_KEEPER"))
			{
				store_keeper = bcls_rec->Tables[0].Rows[0]["STORE_KEEPER"].ToString().Trim();
			}

			if (bcls_rec->Tables[0].Columns.Contains("CRANE_RESP"))
			{
				crane_resp = bcls_rec->Tables[0].Rows[0]["CRANE_RESP"].ToString().Trim();
			}

			if (bcls_rec->Tables[0].Columns.Contains("CRANE_NO"))
			{
				crane_no = bcls_rec->Tables[0].Rows[0]["CRANE_NO"].ToString().Trim();
			}

			Log::Trace("", __FUNCTION__, "参数赋值mat_no：\t[{0}]", twma0["MAT_NO"].ToString());
			Log::Trace("", __FUNCTION__, "参数赋值stock_oper_order_div：\t[{0}]", stock_oper_order_div);
			Log::Trace("", __FUNCTION__, "参数赋值STOCK_PLACE_POSITION：\t[{0}]", stock_place_position);
			Log::Trace("", __FUNCTION__, "参数赋值stock_no：\t[{0}]", stock_no);
			Log::Trace("", __FUNCTION__, "参数赋值stock_place_no：\t[{0}]", stock_place_no);
			Log::Trace("", __FUNCTION__, "参数赋值vehicle_no：\t[{0}]", vehicle_no);
			Log::Trace("", __FUNCTION__, "参数赋值STOCK_OPER_ORDER：\t[{0}]", twma0["STOCK_OPER_ORDER"].ToString());



			sqlstr =
				" SELECT COUNT(1) FROM TWMA0"
				" WHERE MAT_NO = @mat_no"
				" AND STOCK_OPER_ORDER = @stock_oper_order";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("mat_no", twma0["MAT_NO"].ToString());
			cmd_inq.Parameters.Set("stock_oper_order", twma0["STOCK_OPER_ORDER"].ToString());
			rowCount = cmd_inq.ExecuteScalar();

			if (rowCount != 1)
			{
				sprintf(s.msg, "队列已变化，请刷新画面。");
				throw CApplicationException(-1, s.msg, log.Location);
			}



			//查询材料主档、库位跟踪表
			twma1["MAT_NO"] = twma0["MAT_NO"];
			if (!twma1.Query("MAT_NO"))
			{
				sprintf(s.msg, "物料档该材料不存在");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			

			//查询库区定义表
			twm01["STOCK_NO"] = stock_no;
			if (!twm01.Query("STOCK_NO"))
			{
				sprintf(s.msg, "库区号不存在");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			//板坯下线原因
			if (stock_no.Substring(0, 1) == "A"){
				//if (slatUnladeCause == ""&& twma0["STOCK_OPER_ORDER"].ToString().Trim() != "1Q"&&twma0["STOCK_OPER_ORDER"].ToString().Trim() != "1G")
				//{
				//	strcpy(s.msg, "板坯材料" + twma1["MAT_NO"].ToString() + "没有选择下线原因，不允许下线。");
				//	throw CApplicationException(-1, s.msg, log.Location);
				//}

				twma1["SLAT_UNLADE_CAUSE"] = slatUnladeCause;
				twma1.Update("SLAT_UNLADE_CAUSE", "MAT_NO");

				if (twma1["SLAT_UNLADE_CAUSE"].ToString().Trim() >= "30")
				{
					bcls_rec_QM17.Tables[0].Rows.Clear();
					bcls_rec_QM17.Tables[0].Rows.Add();
					bcls_rec_QM17.Tables[0].Rows[0]["EVENT_ID"] = "QM17";
					bcls_rec_QM17.Tables[0].Rows[0]["EVENT_LINE_TYPE"] = "00";
					bcls_rec_QM17.Tables[0].Rows[0]["SYSTEM_ID"] = "MMSM";
					bcls_rec_QM17.Tables[0].Rows[0]["FUNC_ID"] = "f_wmsmsm13_proc";
					bcls_rec_QM17.Tables[0].Rows[0]["MAT_NO"] = twma1["MAT_NO"];
					bcls_rec_QM17.Tables[0].Rows[0]["REL_REMARK"] = "炼钢下线不合封锁";
					bcls_rec_QM17.Tables[0].Rows[0]["REL_MAKER"] = s.userid;
					bcls_rec_QM17.Tables[0].Rows[0]["REL_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
					bcls_rec_QM17.Tables[0].Rows[0]["HOLD_CAUSE_CODE"] = "QMZ7";
					bcls_rec_QM17.Tables[0].Rows[0]["DEFECT_CLASS"] = " ";
					doFlag = f_mmsm99(&bcls_rec_QM17, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
			}

			//判断包装标记，只有做过包装确认的成品才能入成品库
			//PRODUCT_FLAG = ‘1’成品
			//PRODUCT_PACK_FLAG = ‘1’已包装确认
			if (twma1["PRODUCT_FLAG"].ToString().Trim() == "1" &&
				twma1["PRODUCT_PACK_FLAG"].ToString().Trim() != "1" &&
				twm01["STOCK_NO_CLASS"].ToString().Trim() == "2")
			{
				sprintf(s.msg, "还未包装确认，不能入成品库");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			//if (twma1.MAT_WT <= 0)
			//{
			//	sprintf(s.sysmsg, "材料的重量不能为0！");
			//	sprintf(s.msg, "材料的重量不能为0！");
			//	throw CApplicationException(-1, s.msg, log.Location);
			//}

			twma2["MAT_NO"] = twma0["MAT_NO"];
			twma2.Query("MAT_NO");
			layerno1 = twma2["LAYERNO"];




			//更新队列表
			twma0["PROC_STATUS"] = "9";//倒垛处理状态 0-未倒垛 9-预倒垛
			twma0["STOCK_OPER_ORDER_DIV"] = stock_oper_order_div;
			twma0["STOCK_PLACE_POSITION"] = stock_place_position;
			twma0["STORE_KEEPER"] = store_keeper;
			twma0["CRANE_RESP"] = crane_resp;
			twma0["CRANE_NO"] = crane_no;
			twma0["REC_REVISOR"] = CString(s.userid);
			twma0["REC_REVISE_TIME"] = datetime;
			twma0.TrimOrBlank();
			twma0.Update(
				"REC_REVISOR,"
				"REC_REVISE_TIME,"
				"PROC_STATUS,"
				"STOCK_OPER_ORDER_DIV,"
				"STOCK_PLACE_POSITION,"
				"STORE_KEEPER,"
				"CRANE_RESP,"
				"CRANE_NO"
				,
				"MAT_NO,"
				"STOCK_OPER_ORDER");


			//热送坯料
			//1.炼钢侧不用管
			//2.轧钢侧入库 ，辊道直接入炉 计划‘H’
			//2.2非热装， 计划‘C
			if (twma0["STOCK_OPER_ORDER"].ToString().Trim() == "1X" ||
				twma0["STOCK_OPER_ORDER"].ToString().Trim() == "1G")
			{
				//选择库位入库，非热装
				stock_oper_order_div = "C";
			}


			//调用仓库入库主函数
			bcls_stock_in.Tables["WM_STOCK"].Rows.Add();
			bcls_stock_in.Tables["WM_STOCK"].Rows[i]["MAT_NO"] = twma0["MAT_NO"];
			bcls_stock_in.Tables["WM_STOCK"].Rows[i]["STOCK_OPER_ORDER"] = twma0["STOCK_OPER_ORDER"];
			bcls_stock_in.Tables["WM_STOCK"].Rows[i]["STOCK_OPER_ORDER_DIV"] = stock_oper_order_div;
			bcls_stock_in.Tables["WM_STOCK"].Rows[i]["STOCK_NO"] = stock_no;
			bcls_stock_in.Tables["WM_STOCK"].Rows[i]["STOCK_PLACE_NO"] = stock_place_no;
			bcls_stock_in.Tables["WM_STOCK"].Rows[i]["ROWNO"] = " ";
			bcls_stock_in.Tables["WM_STOCK"].Rows[i]["COLUMN_NO"] = " ";
			bcls_stock_in.Tables["WM_STOCK"].Rows[i]["LAYERNO"] = 0;
			bcls_stock_in.Tables["WM_STOCK"].Rows[i]["STOCK_PLACE_POSITION"] = stock_place_position;

			//查询物流计划号存在  该数据项在产品化相关表中不存在
			//Log::Trace("", __FUNCTION__, "物流计划号JL_PLAN_NO=[{0}]", twma1["JL_PLAN_NO"].ToString().Trim());
			//if (twma1["JL_PLAN_NO"].ToString().Trim() != "")
			//{
			//	tsi0021["STOCK_NO"] = stock_no;
			//	tsi0021.Query("STOCK_NO");
			//	Log::Trace("", __FUNCTION__, "开始凝聚发给物流的信息，数量:[{0}]", i + 1);
			//	bcls_LoaCon.Tables["7Z8T04"].Rows.Clear();
			//	bcls_LoaCon.Tables["7Z8T04"].Rows.Add();
			//	bcls_LoaCon.Tables["7Z8T04"].Rows[0]["DEAL_FLAG"] = "I";
			//	bcls_LoaCon.Tables["7Z8T04"].Rows[0]["BUSI_TYPE"] = "1";
			//	bcls_LoaCon.Tables["7Z8T04"].Rows[0]["TRUCK_NO"] = twma1["VEHICLE_NO"];
			//	bcls_LoaCon.Tables["7Z8T04"].Rows[0]["TRUCKBOARD_NO"] = "";
			//	bcls_LoaCon.Tables["7Z8T04"].Rows[0]["UNLOAD_CODE_AREA"] = tsi0021["STOCK_NO"];
			//	bcls_LoaCon.Tables["7Z8T04"].Rows[0]["UNLOAD_CODE_AREA"] = tsi0021["STOCK_DESC"];
			//	bcls_LoaCon.Tables["7Z8T04"].Rows[0]["UNLOAD_CODE"] = "";
			//	bcls_LoaCon.Tables["7Z8T04"].Rows[0]["UNLOAD_NAME"] = "";
			//	bcls_LoaCon.Tables["7Z8T04"].Rows[0]["PLAN_NO"] = twma1["JL_PLAN_NO"];//根据车号查询最新申请是
			//	bcls_LoaCon.Tables["7Z8T04"].Rows[0]["ATTENTN_NO"] = datetime;
			//	bcls_LoaCon.Tables["7Z8T04"].Rows[0]["MAT_NO"] = twma1["MAT_NO"];
			//	bcls_LoaCon.Tables["7Z8T04"].Rows[0]["SG_SIGN"] = twma1["SG_SIGN"];
			//	bcls_LoaCon.Tables["7Z8T04"].Rows[0]["MAT_ACT_LEN"] = twma1["MAT_ACT_LEN"];
			//	bcls_LoaCon.Tables["7Z8T04"].Rows[0]["MAT_ACT_WIDTH"] = twma1["MAT_ACT_WIDTH"];
			//	bcls_LoaCon.Tables["7Z8T04"].Rows[0]["MAT_ACT_THICK"] = twma1["MAT_ACT_THICK"];
			//	bcls_LoaCon.Tables["7Z8T04"].Rows[0]["MAT_ACT_WT"] = twma1["MAT_ACT_WT"];

			//	if (bcls_LoaCon.Tables["7Z8T04"].Rows.get_Count() > 0)
			//	{
			//		/*doFlag = f_cm_7z8t04_snd(&bcls_LoaCon, bcls_ret, conn);
			//		if (doFlag != 0)
			//		{
			//			throw CApplicationException(-1, s.msg, log.Location);
			//		}*/
			//	}

			//}



		}

		doFlag = f_wmsmsm_stock_in(&bcls_stock_in, bcls_ret, conn);
		if (doFlag != 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
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

