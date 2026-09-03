/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         JINQUAN
Version:		1.0
Date:			2016-07-27
Description:	吊车命令卸下
**************************************************/

//框架头文件
#include "stdafx.h"
//#include "twma7.h"
//#include "twmzc.h"



//#include "tmmsm32.h"

//程序用头文件

//函数申明
BM2_FUNCTION_IMPORT
int f_wmsmsm_crane_down(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
//int f_cm_jof11a_snd(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);
//int f_cm_jof11b_snd(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);
int f_wmsmsm_cranecmd_make(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);

/*<remark>=========================================================
///<summary>
///吊车命令卸下
///<para>
///2.排序方式：
///</para>
///<para>
===========================================================</remark>*/

BM2F_ENTERACE(wmsmsma7_unload);

int f_wmsmsma7_unload(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	/* 程序内部变量 */
	int	doFlag = 0;
	int ret = 0;

	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString sqlstr = "";
	CString in_flag = "";     //等于1为在库内
	CString stock_truck = "";
	CString carseq = "";

	CDecimal  cou = 0;


	//定义表实体对象
	//CTWMA0 twma0(conn);
	//CHWMA0 hwma0(conn);
	//CTWMA1 twma1(conn);
	//CTWMA2 twma2(conn);
	//CTWMA7 twma7(conn);
	//CTWMA7 twma7_hmi(conn);
//	CModel twm04("TWM04");
	//CHWM00A7 hwm00a7(conn);

	CModel twma7_hmi = CModel("TWMA7");
	CModel twma7_q = CModel("TWMA7");

	CModel twm04("TWM04");
	//CTWMA7 twma7_q(conn);
	CModel tmmsm01("TMMSM01");
	//CTMMSM32 tmmsm32(conn);
	//CTWMZC twmzc(conn);
	CModel hmmsm01("HMMSM01");

	CDbCommand cmd_inq(conn);

	EIClass bcls_down;
	bcls_down.Tables[0].set_TableName("WM00_DOWN");
	bcls_down.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
	bcls_down.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO");
	bcls_down.Tables["WM00_DOWN"].Rows.Add();


	EIClass bcls_jof11a;
	bcls_jof11a.Tables[0].set_TableName("JOF11A");
	bcls_jof11a.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
	bcls_jof11a.Tables[0].Columns.Add(DT_DECIMAL, "EVENT_ID");
	bcls_jof11a.Tables[0].Columns.Add(DT_STRING, "MACH_CLEAR_DIV");
	bcls_jof11a.Tables[0].Columns.Add(DT_DECIMAL, "MEND_NO");
	bcls_jof11a.Tables[0].Columns.Add(DT_STRING, "DENSITY_1");
	bcls_jof11a.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_UP");
	bcls_jof11a.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO");
	bcls_jof11a.Tables[0].Columns.Add(DT_STRING, "REMARK_1");
	bcls_jof11a.Tables[0].Columns.Add(DT_STRING, "REMARK_2");
	bcls_jof11a.Tables[0].Columns.Add(DT_STRING, "REMARK_3");
	bcls_jof11a.Tables["JOF11A"].Rows.Add();

	EIClass bcls_jof11b;
	bcls_jof11b.Tables[0].set_TableName("JOF11B");
	bcls_jof11b.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
	bcls_jof11b.Tables[0].Columns.Add(DT_DECIMAL, "MEND_NO");
	bcls_jof11b.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO");
	bcls_jof11b.Tables["JOF11B"].Rows.Add();

	//吊运命令生成
	EIClass bcls_rec_make;
	bcls_rec_make.Tables[0].set_TableName("CMD_MAKE");
	bcls_rec_make.Tables["CMD_MAKE"].Columns.Add(DT_STRING, "STOCK_PLACE_NO_TO");
	bcls_rec_make.Tables["CMD_MAKE"].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");
	bcls_rec_make.Tables["CMD_MAKE"].Columns.Add(DT_STRING, "MAT_NO");
	bcls_rec_make.Tables["CMD_MAKE"].Columns.Add(DT_STRING, "HALL_NO_TO");
	bcls_rec_make.Tables["CMD_MAKE"].Columns.Add(DT_STRING, "STOCK_NO_TO");
	bcls_rec_make.Tables["CMD_MAKE"].Columns.Add(DT_STRING, "STOCK_PLACE_NO_FROM");
	bcls_rec_make.Tables["CMD_MAKE"].Columns.Add(DT_STRING, "STOCK_OPER_ORDER_FIN");
	bcls_rec_make.Tables["CMD_MAKE"].Columns.Add(DT_STRING, "STOCK_PLACE_NO_FIN");
	bcls_rec_make.Tables["CMD_MAKE"].Columns.Add(DT_STRING, "HALL_NO_FIN");
	bcls_rec_make.Tables["CMD_MAKE"].Columns.Add(DT_STRING, "MOV_HALL");
	bcls_rec_make.Tables["CMD_MAKE"].Rows.Clear();

	try
	{
		//传入参数检核
		if (bcls_rec->Tables[0].Rows.get_Count() == 0)
		{
			sprintf(s.msg, "Incoming record cannot be empty!"); //调用函数传入记录数不能为0!
			throw CApplicationException(-1, s.msg, log.Location);
		}

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			Log::Trace("", __FUNCTION__, "111111111");
			twma7_hmi.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			if (twma7_hmi["STOCK_PLACE_NO_TO"].ToString().Trim() == "")
			{
				sprintf(s.msg, "Target position cannot be empty!");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			twma7_q["MAT_NO"] = twma7_hmi["MAT_NO"].ToString().Trim();

			if (!twma7_q.Query("MAT_NO"))
			{
				sprintf(s.msg, "[%s] instruction isn't exist!", (const char*)twma7_hmi["MAT_NO"]);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			tmmsm01["MAT_NO"] = twma7_q["MAT_NO"].ToString().Trim();

			if (tmmsm01.QueryCount("MAT_NO") > 0)
			{
				tmmsm01.Query("MAT_NO");
				if (tmmsm01["FACTORY_DIV"].ToString() == "H2")
				{
					sprintf(s.msg, "该材料[%s]已在轧钢库,不能做卸下操作。", (const char*)tmmsm01["MAT_NO"].ToString());
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			//如果卸下位置是台车，判断台车是否就位
			twm04["STOCK_PLACE_NO"] = twma7_hmi["STOCK_PLACE_NO_TO"].ToString().Trim();
			if (!twm04.Query("STOCK_PLACE_NO"))
			{
				sprintf(s.msg, "[%s] target position isn't exist!", (const char*)twma7_hmi["STOCK_PLACE_NO_TO"]);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (twm04["DEV_DIV"].ToString() == "3" &&
				twm04["LIFT_IN_MARK"].ToString() != "1") //transfer car
			{
				sprintf(s.msg, "[%s] transfer car is not here and cannot be loaded now!",
					(const char*)twma7_hmi["STOCK_PLACE_NO_TO"]);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			//M:合并 R : 选定确认 S : 吊上 E : 卸下 C : 选定取消
			
			Log::Trace("", __FUNCTION__, "4444444444");
			bcls_down.Tables["WM00_DOWN"].Rows[0]["MAT_NO"] = twma7_hmi["MAT_NO"].ToString();
			bcls_down.Tables["WM00_DOWN"].Rows[0]["STOCK_PLACE_NO"] = twma7_hmi["STOCK_PLACE_NO_TO"].ToString();

			doFlag = f_wmsmsm_crane_down(&bcls_down, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}


			if (twma7_q["STOCK_OPER_ORDER"].ToString().Trim() == "32")
			{
				if (twma7_q["STOCK_PLACE_NO_TO"].ToString().Trim() == "GDA01")
				{
					stock_truck = "GDB01";
				}
				else if (twma7_q["STOCK_PLACE_NO_TO"].ToString().Trim() == "GDB01")
				{
					stock_truck = "GDB01";
				}
				else if (twma7_q["STOCK_PLACE_NO_TO"].ToString().Trim() == "0101")
				{
					stock_truck = "0104";
				}
				else if (twma7_q["STOCK_PLACE_NO_TO"].ToString().Trim() == "0104")
				{
					stock_truck = "0101";
				}
				else if (twma7_q["STOCK_PLACE_NO_TO"].ToString().Trim() == "0201")
				{
					stock_truck = "0204";
				}
				else if (twma7_q["STOCK_PLACE_NO_TO"].ToString().Trim() == "0204")
				{
					stock_truck = "0201";
				}
				else if (twma7_q["STOCK_PLACE_NO_TO"].ToString().Trim() == "0501")
				{
					stock_truck = "0507";
				}
				else if (twma7_q["STOCK_PLACE_NO_TO"].ToString().Trim() == "0507")
				{
					stock_truck = "0501";
				}
				Log::Trace("", __FUNCTION__, "命令第二起吊位 [{0}]", stock_truck);
				Log::Trace("", __FUNCTION__, "命令第二卸下位 [{0}]", twma7_q["STOCK_PLACE_NO_FIN"].ToString().Trim());
				Log::Trace("", __FUNCTION__, "HALL_NO_FIN [{0}]", twma7_q["HALL_NO_FIN"].ToString().Trim());

				Log::Debug("", __FUNCTION__, "过跨倒垛生成第二条命令");
				bcls_rec_make.Tables["CMD_MAKE"].Rows.Add();
				bcls_rec_make.Tables["CMD_MAKE"].Rows[0]["STOCK_PLACE_NO_TO"] = twma7_q["STOCK_PLACE_NO_FIN"].ToString().Trim();
				bcls_rec_make.Tables["CMD_MAKE"].Rows[0]["STOCK_OPER_ORDER"] = "30"; //过跨倒垛，吊运命令函数中需要
				bcls_rec_make.Tables["CMD_MAKE"].Rows[0]["MAT_NO"] = twma7_q["MAT_NO"].ToString().Trim();
				bcls_rec_make.Tables["CMD_MAKE"].Rows[0]["HALL_NO_TO"] = twma7_q["HALL_NO_FIN"].ToString().Trim();
				bcls_rec_make.Tables["CMD_MAKE"].Rows[0]["STOCK_NO_TO"] = "A21";
				bcls_rec_make.Tables["CMD_MAKE"].Rows[0]["STOCK_PLACE_NO_FROM"] = stock_truck;
				bcls_rec_make.Tables["CMD_MAKE"].Rows[0]["STOCK_OPER_ORDER_FIN"] = " ";
				bcls_rec_make.Tables["CMD_MAKE"].Rows[0]["STOCK_PLACE_NO_FIN"] = " ";
				bcls_rec_make.Tables["CMD_MAKE"].Rows[0]["HALL_NO_FIN"] = " ";
				bcls_rec_make.Tables["CMD_MAKE"].Rows[0]["MOV_HALL"] = "1"; //过跨倒垛生成第二段吊运命令

				ret = f_wmsmsm_cranecmd_make(&bcls_rec_make, bcls_ret, conn);
				if (ret < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			else if (twma7_q["STOCK_OPER_ORDER"].ToString().Trim() == "30")
			{
				//if (twma7_q["STOCK_PLACE_NO_TO == "T1A01" || twma7_q["STOCK_PLACE_NO_TO == "T1B01")
				//{
				//	twmzc.Reset();
				//	tmmsm01.Reset();
				//	hmmsm01.Reset();
				//	twmzc.MAT_NO = twma7_q["MAT_NO;

				//	if (twmzc.QueryCount("MAT_NO") == 1)
				//	{
				//		sprintf(s.msg, "该材料[%s]已装车,不能再装车。", (const char*)twmzc.MAT_NO);
				//		throw CApplicationException(-1, s.msg, log.Location);
				//	}

				//	twmzc.STOCK_OPER_ORDER = "30";
				//	twmzc.COMPANY_CODE = "A2";
				//	twmzc.FACTORY_DIV = "A2";
				//	twmzc.AFFIRM_MARK = "0";
				//	twmzc.REC_CREATE_TIME = datetime;
				//	twmzc.REC_CREATOR = s.userid;
				//	twmzc.MAT_NO = twma7_q["MAT_NO;
				//	twmzc.TRUCK_NO = twma7_q["VEHICLE_NO;
				//	twmzc.SEQ_ID = 1;
				//	twmzc.LOAD_NUM = twmzc.SEQ_ID;

				//	if (twmzc.TRUCK_NO.Trim() == "")
				//	{
				//		sprintf(s.msg, "该车号[%s]为空,不能装车。", (const char*)twmzc.TRUCK_NO);
				//		throw CApplicationException(-1, s.msg, log.Location);
				//	}


				//	twmzc.AIM_STOCK_NO = twma7_q["COMPANY_NAME;

				//	if (twmzc.QueryCount("AFFIRM_MARK,TRUCK_NO") > 0)
				//	{
				//		// 取该车号最大序号+1更新
				//		switch (conn->DatabaseKind)
				//		{
				//		case DB_KIND_DB2:           // DB2 数据库（未开Oracle兼容）
				//		case DB_KIND_DB2_ORACLE:    // DB2 数据库（开Oracle兼容）
				//		case DB_KIND_MSSQL:	        // MS SQL Server数据库
				//		case DB_KIND_ORACLE:        // Oracle 数据库
				//		default:
				//			sqlstr = CString(
				//				" SELECT MAX(SEQ_ID),LOAD_NAME FROM twmzc WHERE  AFFIRM_MARK ='0' and truck_no =@twmzc.TRUCK_NO group by LOAD_NAME "
				//				);
				//			break;
				//		}
				//		Log::Debug("", __FUNCTION__, "111twmzc.TRUCK_NO	= [{0}]", twmzc.TRUCK_NO);
				//		cmd_inq.SetCommandText(sqlstr);
				//		cmd_inq.Parameters.Set("twmzc.TRUCK_NO", twmzc.TRUCK_NO);
				//		cmd_inq.ExecuteReader();
				//		if (cmd_inq.Read())
				//		{
				//			twmzc.SEQ_ID = cmd_inq.GetDecimal(1);
				//			twmzc.LOAD_NAME = cmd_inq.GetString(2);

				//			twmzc.SEQ_ID = twmzc.SEQ_ID + 1;//该车号装有坯子还没出库的情况下进行装坯顺序累加
				//		}
				//		cmd_inq.Close();


				//	}

				//	Log::Debug("", __FUNCTION__, "111twmzc.SEQ_ID	= [{0}]", twmzc.SEQ_ID);

				//	if (twmzc.SEQ_ID == 1)
				//	{
				//		carseq = EPGetNextSeq("CAR_SEQ", conn);
				//		twmzc.LOAD_NAME = twmzc.AIM_STOCK_NO + datetime + carseq;
				//	}

				//	tmmsm01["MAT_NO"] = twmzc.MAT_NO;
				//	hmmsm01["MAT_NO"] = twmzc.MAT_NO;
				//	if (tmmsm01.QueryCount("MAT_NO") > 0)
				//	{
				//		tmmsm01.Query("MAT_NO");
				//		if (tmmsm01["IN_FLAG"].ToString() == "2")
				//		{
				//			sprintf(s.msg, "该材料[%s]已出库,不能再装车。", (const char*)tmmsm01["MAT_NO"].ToString());
				//			throw CApplicationException(-1, s.msg, log.Location);
				//		}

				//		if (twmzc.FACTORY_DIV == "A1")
				//		{
				//			if (tmmsm01["STOCK_NO"].ToString() != "A11")
				//			{
				//				sprintf(s.msg, "该材料[%s]下分厂已入库,不能再装车。", (const char*)tmmsm01["MAT_NO"].ToString());
				//				throw CApplicationException(-1, s.msg, log.Location);
				//			}
				//		}
				//		else if (twmzc.FACTORY_DIV == "A2")
				//		{
				//			if (tmmsm01["STOCK_NO"].ToString() != "A21")
				//			{
				//				sprintf(s.msg, "该材料[%s]下分厂已入库,不能再装车。", (const char*)tmmsm01["MAT_NO"].ToString());
				//				throw CApplicationException(-1, s.msg, log.Location);
				//			}
				//		}


				//		tmmsm01["MERG_MAT_NO"] = twmzc.LOAD_NAME;
				//		tmmsm01.Update("MERG_MAT_NO", "MAT_NO");
				//	}
				//	else if (hmmsm01.QueryCount("MAT_NO") > 0)
				//	{
				//		sprintf(s.msg, "该材料[%s]下分厂已入库,不能再装车。", (const char*)tmmsm01["MAT_NO"].ToString());
				//		throw CApplicationException(-1, s.msg, log.Location);
				//	}


				//	//2020-10-20 by daijun  取物流计划号写入 twmzc表  给物流发送停车位推荐电文JOJL22时用
				//	switch (conn->DatabaseKind)
				//	{
				//	case DB_KIND_DB2:           // DB2 数据库（未开Oracle兼容）
				//	case DB_KIND_DB2_ORACLE:    // DB2 数据库（开Oracle兼容）
				//	case DB_KIND_MSSQL:	        // MS SQL Server数据库
				//	case DB_KIND_ORACLE:        // Oracle 数据库
				//	default:
				//		sqlstr = CString(
				//			" SELECT LOADING_PLAN_NO FROM twm0d WHERE stock_no =@twm0d.STOCK_NO and date_time =@twm0d.DATE_TIME and truck_no =@twm0d.TRUCK_NO"
				//			);
				//		break;
				//	}
				//	Log::Debug("", __FUNCTION__, "222 tmmsm01.STOCK_NO	= [{0}]", tmmsm01["STOCK_NO"].ToString());
				//	Log::Debug("", __FUNCTION__, "222 twmzc.TRUCK_NO	= [{0}]", twmzc.TRUCK_NO);
				//	cmd_inq.SetCommandText(sqlstr);
				//	cmd_inq.Parameters.Set("twm0d.STOCK_NO", tmmsm01["STOCK_NO"].ToString());
				//	cmd_inq.Parameters.Set("twm0d.DATE_TIME", datetime.SubstringNE(0, 8));
				//	cmd_inq.Parameters.Set("twm0d.TRUCK_NO", twmzc.TRUCK_NO);
				//	cmd_inq.ExecuteReader();
				//	if (cmd_inq.Read())
				//	{
				//		twmzc.LOADING_PLAN_NO = cmd_inq.GetString(1);
				//	}
				//	cmd_inq.Close();

				//	Log::Trace("", __FUNCTION__, "twmzc.LOADING_PLAN_NO222[{0}]", twmzc.LOADING_PLAN_NO);
				//	Log::Debug("", __FUNCTION__, "STOCK_OPER_ORDER222	= [{0}]", twmzc.STOCK_OPER_ORDER);
				//	Log::Debug("", __FUNCTION__, "LOAD_NAME222	= [{0}]", twmzc.LOAD_NAME);
				//	Log::Debug("", __FUNCTION__, "MAT_NO222	= [{0}]", twmzc.MAT_NO);
				//	twmzc.Insert();

				//	twmzc.LOAD_NUM = twmzc.QueryCount("LOAD_NAME,AFFIRM_MARK,TRUCK_NO");
				//	twmzc.Update("LOAD_NUM", "LOAD_NAME,AFFIRM_MARK,TRUCK_NO");
				//}

			}
			//补充逻辑：根据材料号如果在修磨命令表能到查生成状态的记录，则下发修磨命令
			//tmmsm32.MAT_NO = twma7_q["MAT_NO;
			//tmmsm32.STATUS_FLAG = "S";  // S 命令生成、  E 命令下发 、 X 修磨完成
			//if (tmmsm32.Query("MAT_NO,STATUS_FLAG") == true)
			//{
			//	Log::Trace("", __FUNCTION__, "tmmsm32.STOCK_PLACE_NO_TO [{0}]", tmmsm32.STOCK_PLACE_NO_TO);
			//	Log::Trace("", __FUNCTION__, "材料卸下位 [{0}]", bcls_rec->Tables[0].Rows[0]["STOCK_PLACE_NO_TO"].ToString().Trim());
			//	//材料卸下位不是修磨上料台 不下发修磨命令
			//	if (tmmsm32.STOCK_PLACE_NO_TO.Trim() == bcls_rec->Tables[0].Rows[0]["STOCK_PLACE_NO_TO"].ToString().Trim())
			//	{
			//		twm04["STOCK_PLACE_NO"] = tmmsm32.STOCK_PLACE_NO_FIN; //修磨完后库房堆放库位对应的跨号
			//		if (!twm04.Query("STOCK_PLACE_NO"))
			//		{
			//			sprintf(s.msg, "无该库位号信息！！！");
			//			throw CApplicationException(-1, s.msg, log.Location);
			//		}
			//		Log::Trace("", __FUNCTION__, "修磨完后库房堆放库位对应的跨号 [{0}]", twm04["HALL_NO"].ToString());

			//		if (twma7_q["STOCK_PLACE_NO_TO.Trim().SubstringNE(0, 2) == "01")
			//		{
			//			if (twm04["HALL_NO"].ToString().Trim() == "A")
			//			{
			//				stock_truck = "0101";
			//			}
			//			else if (twm04["HALL_NO"].ToString().Trim() == "B")
			//			{
			//				stock_truck = "0104";
			//			}
			//			else
			//			{
			//				Log::Trace("", __FUNCTION__, "01 最终跨号不正确 [{0}]", twma7_q["HALL_NO_FIN);
			//			}
			//		}
			//		else if (twma7_q["STOCK_PLACE_NO_TO.Trim().SubstringNE(0, 2) == "02")
			//		{
			//			if (twm04["HALL_NO"].ToString().Trim() == "A")
			//			{
			//				stock_truck = "0201";
			//			}
			//			else if (twm04["HALL_NO"].ToString().Trim() == "B")
			//			{
			//				stock_truck = "0204";
			//			}
			//			else
			//			{
			//				Log::Trace("", __FUNCTION__, "02 最终跨号不正确 [{0}]", twma7_q["HALL_NO_FIN);
			//			}
			//		}
			//		else if (twma7_q["STOCK_PLACE_NO_TO.Trim().SubstringNE(0, 2) == "03")
			//		{
			//			if (twm04["HALL_NO"].ToString().Trim() == "A")
			//			{
			//				stock_truck = "0301";
			//			}
			//			else if (twm04["HALL_NO"].ToString().Trim() == "B")
			//			{
			//				stock_truck = "0311";
			//			}
			//			else
			//			{
			//				Log::Trace("", __FUNCTION__, "03 最终跨号不正确 [{0}]", twma7_q["HALL_NO_FIN);
			//			}
			//		}
			//		else if (twma7_q["STOCK_PLACE_NO_TO.Trim().SubstringNE(0, 2) == "04")
			//		{
			//			if (twm04["HALL_NO"].ToString().Trim() == "A")
			//			{
			//				stock_truck = "0401";
			//			}
			//			else if (twm04["HALL_NO"].ToString().Trim() == "B")
			//			{
			//				stock_truck = "0411";
			//			}
			//			else
			//			{
			//				Log::Trace("", __FUNCTION__, "04 最终跨号不正确 [{0}]", twma7_q["HALL_NO_FIN);
			//			}
			//		}
			//		else if (twma7_q["STOCK_PLACE_NO_TO.Trim().SubstringNE(0, 2) == "05")
			//		{
			//			if (twm04["HALL_NO"].ToString().Trim() == "A")
			//			{
			//				stock_truck = "0501";
			//			}
			//			else if (twm04["HALL_NO"].ToString().Trim() == "B")
			//			{
			//				stock_truck = "0507";
			//			}
			//			else
			//			{
			//				Log::Trace("", __FUNCTION__, "05 最终跨号不正确 [{0}]", twma7_q["HALL_NO_FIN);
			//			}
			//		}

			//		Log::Trace("", __FUNCTION__, "jof11b stock_truck [{0}]", stock_truck);
			//		Log::Trace("", __FUNCTION__, "jof11b tmmsm32.MEND_MODE [{0}]", tmmsm32.MEND_MODE);
			//		Log::Trace("", __FUNCTION__, "jof11b tmmsm32.SLB_INDICA [{0}]", tmmsm32.SLB_INDICA);

			//		bcls_jof11a.Tables["JOF11A"].Rows[0]["MAT_NO"] = twma7_q["MAT_NO;
			//		bcls_jof11a.Tables["JOF11A"].Rows[0]["EVENT_ID"] = 1;
			//		bcls_jof11a.Tables["JOF11A"].Rows[0]["MEND_NO"] = tmmsm32.MEND_NO;
			//		bcls_jof11a.Tables["JOF11A"].Rows[0]["MACH_CLEAR_DIV"] = tmmsm32.MEND_MODE; //修磨方式  0倒垛，其他修磨
			//		bcls_jof11a.Tables["JOF11A"].Rows[0]["STOCK_PLACE_UP"] = tmmsm32.STOCK_PLACE_NO_TO;
			//		bcls_jof11a.Tables["JOF11A"].Rows[0]["STOCK_PLACE_NO"] = stock_truck;  //下线位置代码（预定） 只有4位
			//		bcls_jof11a.Tables["JOF11A"].Rows[0]["REMARK_1"] = tmmsm32.SLB_INDICA;  //全磨备注代码
			//		bcls_jof11a.Tables["JOF11A"].Rows[0]["REMARK_2"] = tmmsm32.BACK_C2;  //修磨深度
			//		bcls_jof11a.Tables["JOF11A"].Rows[0]["REMARK_3"] = tmmsm32.BACK_C3;  //缺陷代码

			//		//doFlag = f_cm_jof11a_snd(&bcls_jof11a, bcls_ret, conn);
			//		if (doFlag != 0)
			//		{
			//			sprintf(s.msg, "f_cm_jof11a_snd 调用失败");
			//			throw CApplicationException(-1, s.msg, log.Location);
			//		}

			//		bcls_jof11b.Tables["JOF11B"].Rows[0]["MAT_NO"] = twma7_q["MAT_NO;
			//		bcls_jof11b.Tables["JOF11B"].Rows[0]["MEND_NO"] = tmmsm32.MEND_NO;
			//		bcls_jof11b.Tables["JOF11B"].Rows[0]["STOCK_PLACE_NO"] = twma7_q["STOCK_PLACE_NO_TO.Trim();  //上料位置代码

			//		//doFlag = f_cm_jof11b_snd(&bcls_jof11b, bcls_ret, conn);
			//		if (doFlag != 0)
			//		{
			//			sprintf(s.msg, "f_cm_jof11a_snd 调用失败");
			//			throw CApplicationException(-1, s.msg, log.Location);
			//		}

			//		Log::Trace("", __FUNCTION__, "tmmsm32.REC_CREATE_TIME[{0}] ", tmmsm32.REC_CREATE_TIME);
			//		//更新修磨命令状态为 E
			//		tmmsm32.STATUS_FLAG = "E";
			//		tmmsm32.REC_REVISE_TIME = datetime;
			//		tmmsm32.REC_REVISOR = s.userid;
			//		int row = tmmsm32.Update("REC_REVISE_TIME,REC_REVISOR,STATUS_FLAG", "MAT_NO,REC_CREATE_TIME");
			//		Log::Trace("", __FUNCTION__, "tmmsm32.Update[{0}] ", row);
			//	} //end if  材料卸下位不是修磨上料台
			//	else
			//	{
			//		Log::Trace("", __FUNCTION__, " 材料卸下位不是修磨上料台 不下发修磨命令.");
			//	}
			//}//end tmmsm32.Query

			/*设置系统返回参数*/
			Log::Trace("", __FUNCTION__, "end");
		}
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "Database processing error，sqlcode = [{0}]." /*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
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
