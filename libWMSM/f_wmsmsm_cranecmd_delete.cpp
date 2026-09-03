/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:      jinquan
Version:     1.1.1
Date:         2017-3-16
Description: 删除命令
**************************************************/

//框架头文件
#include "stdafx.h"

BM2_FUNCTION_IMPORT
int f_wm00_pileinfocal(CString stock_no, CString stock_place_no, EIClass * bcls_ret, CDbConnection * conn);  //垛位最大高度、重量修正
int f_wmsmsm_cranecmd_update(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);
int f_auto(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
//int f_wmsmsm_u1dl14_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_wmsmsm_cranecmd_logic(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//推荐逻辑区
int f_wms_auto(EIClass* blkIn, EIClass* blkOut, CDbConnection* dbConn);
int f_auto(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

BM2_FUNCTION_EXPORT
int f_wmsmsm_cranecmd_delete(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{

	//程序内部变量
	int doFlag = 0;
	int SEQ_A = 0;
	int SEQ_DELETE = 0;
	CString sqlstr = " ";
	CString down_flag = " ";
	CString hall_no = " ";
	CString logic_stock_no = " ";

	//数据存放块
	CDataTable   dtMat;                              //存放上层材料
	CDataTable   Table_auto;
	CDataTable   Table_MAT;
	//数据块
	CDataTable logic_rule;                         //twma8

	CModel twm04 = CModel("TWM04");
	CModel twma2 = CModel("TWMA2");
	CModel twma7_in = CModel("TWMA7");

	EIClass bcls_rec_update;
	bcls_rec_update.Tables[0].set_TableName("CMD_UPDATE");
	bcls_rec_update.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
	bcls_rec_update.Tables[0].Columns.Add(DT_STRING, "CRANE_INST_STATUS");
	bcls_rec_update.Tables[0].Columns.Add(DT_STRING, "STOCK_NO");
	bcls_rec_update.Tables[0].Columns.Add(DT_STRING, "STOCK_NO_FROM");
	bcls_rec_update.Tables[0].Columns.Add(DT_STRING, "HALL_NO_FR");
	bcls_rec_update.Tables[0].Columns.Add(DT_DECIMAL, "YARD_LAYER_FROM");
	bcls_rec_update.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO_FROM");
	bcls_rec_update.Tables[0].Columns.Add(DT_STRING, "STOCK_OPER_ORDER_NEW");
	bcls_rec_update.Tables[0].Columns.Add(DT_STRING, "STOCK_OPER_ORDER_FIN");
	bcls_rec_update.Tables[0].Columns.Add(DT_STRING, "HALL_NO_TO");
	bcls_rec_update.Tables[0].Columns.Add(DT_STRING, "STOCK_NO_TO");
	bcls_rec_update.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO_TO_NEW");
	bcls_rec_update.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO_TO_OLD");
	bcls_rec_update.Tables[0].Columns.Add(DT_STRING, "LOGIC_STOCK_NO");
	bcls_rec_update.Tables[0].Columns.Add(DT_STRING, "DEV_DIV");
	bcls_rec_update.Tables[0].Rows.Add();

	//推荐库位结果
	EIClass bcls_ret_place;

	//推荐逻辑区域结果
	EIClass bcls_ret_logic;

	EIClass bcls_rec_tr;
	bcls_rec_tr.Tables[0].set_TableName("CMD_TR_REM");
	bcls_rec_tr.Tables[0].Columns.Add(DT_STRING, "HALL_TO");
	bcls_rec_tr.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
	bcls_rec_tr.Tables[0].Columns.Add(DT_STRING, "HALL_FR");
	bcls_rec_tr.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO");
	bcls_rec_tr.Tables[0].Columns.Add(DT_STRING, "STOCK_OPER_ORDER_FIN");
	bcls_rec_tr.Tables[0].Columns.Add(DT_DECIMAL, "MAT_THEORY_WT");
	bcls_rec_tr.Tables[0].Columns.Add(DT_DECIMAL, "MAT_THICK");
	bcls_rec_tr.Tables[0].Columns.Add(DT_DECIMAL, "MAT_WIDTH");
	bcls_rec_tr.Tables[0].Columns.Add(DT_DECIMAL, "MAT_LEN");
	bcls_rec_tr.Tables[0].Rows.Add();

	//推荐逻辑区域
	EIClass bcls_rec_logic;
	bcls_rec_logic.Tables[0].set_TableName("CMD_LOGIC");
	bcls_rec_logic.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
	bcls_rec_logic.Tables[0].Columns.Add(DT_STRING, "HALL_NO");
	bcls_rec_logic.Tables[0].Columns.Add(DT_DECIMAL, "X_FROM");
	bcls_rec_logic.Tables[0].Columns.Add(DT_DECIMAL, "Y_FROM");
	bcls_rec_logic.Tables[0].Columns.Add(DT_STRING, "STOCK_NO");
	bcls_rec_logic.Tables[0].Rows.Add();

	//推荐库位
	EIClass bcls_rec_auto;
	bcls_rec_auto.Tables[0].set_TableName("BLK_INPUT");
	bcls_rec_auto.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
	bcls_rec_auto.Tables[0].Columns.Add(DT_STRING, "HALL_NO");
	bcls_rec_auto.Tables[0].Columns.Add(DT_STRING, "LOGIC_STOCK_NO");
	bcls_rec_auto.Tables[0].Columns.Add(DT_STRING, "STNO_RULE_FLAG");
	bcls_rec_auto.Tables[0].Columns.Add(DT_STRING, "ORDER_RULE_FLAG");
	bcls_rec_auto.Tables[0].Columns.Add(DT_DECIMAL, "BASE_X");
	bcls_rec_auto.Tables[0].Columns.Add(DT_DECIMAL, "BASE_Y");
	bcls_rec_auto.Tables[0].Columns.Add(DT_DECIMAL, "WIDTH_DIFF");
	bcls_rec_auto.Tables[0].Rows.Add();

	EIClass bcls_rec_auto_1;
	bcls_rec_auto_1.Tables[0].set_TableName("AUTO_INFO_IN");
	bcls_rec_auto_1.Tables[0].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");
	bcls_rec_auto_1.Tables[0].Columns.Add(DT_STRING, "HALL_NO");
	bcls_rec_auto_1.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
	bcls_rec_auto_1.Tables[0].Rows.Add();

	EIClass bcls_rec_delete;
	bcls_rec_delete.Tables[0].set_TableName("U1DL14");
	bcls_rec_delete.Tables[0].Columns.Add(DT_STRING, "mat_no");

	try
	{
		//项目自定义日志
		CTracer log(__FUNCTION__);

		//检验传入数据块
		if (!bcls_rec->Tables.Contains("CMD_DELETE") || bcls_rec->Tables["CMD_DELETE"].Rows.get_Count() == 0)
		{
			sprintf(s.msg, "函数f_wmsmsm_cranecmd_delete中找不到接收块名[CMD_DELETE]或值为空");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		logic_rule.Rows.Clear();
		

		for (int i = 0; i < bcls_rec->Tables["CMD_DELETE"].Rows.get_Count(); i++)
		{
			Log::Trace("", __FUNCTION__, "MAT_NO =[{0}]", bcls_rec->Tables["CMD_DELETE"].Rows[i]["MAT_NO"].ToString().Trim());
			if (bcls_rec->Tables["CMD_DELETE"].Rows[i]["MAT_NO"].ToString().Trim() == "")
			{
				sprintf(s.msg, "传入材料号为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			twma2.Reset();
			twma2["MAT_NO"] = bcls_rec->Tables["CMD_DELETE"].Rows[i]["MAT_NO"].ToString();
			twma2.Query("MAT_NO");

			if (twma2["LAYERNO"].ToString().Trim() == "")
			{
				twma2["LAYERNO"] = "0";
			}

			twm04.Reset();
			twm04["STOCK_PLACE_NO"] = twma2["STOCK_PLACE_NO"].ToString();
			twm04.Query("STOCK_PLACE_NO");
			if (twm04["STOCK_PLACE_TYPE"].ToString().Trim() == "D"
				&&twm04["DEV_DIV"].ToString().Trim() == "4")
			{
				Log::Trace("", __FUNCTION__, "材料在辊道上");
				dtMat.Rows.Clear();
				sqlstr = "SELECT A.MAT_NO,B.CMD_SEQ,A.LAYERNO,A.STOCK_NO,A.HALL_NO,A.STOCK_PLACE_NO,B.CRANE_INST_STATUS,B.STOCK_OPER_ORDER_FIN,A.LAYERNO,B.STOCK_PLACE_NO_TO,B.STOCK_PLACE_NO_FROM,B.STOCK_NO_FROM, "
					" B.STOCK_NO_FIN,B.STOCK_PLACE_NO_FIN FROM TWMA2 A ,TWMA7 B "
					" WHERE A.MAT_NO = B.MAT_NO AND A.MAT_NO = '" + twma2["MAT_NO"].ToString() + "'";
				Log::Trace("", __FUNCTION__, "{0}", sqlstr);
				Db::QueryTable(sqlstr, dtMat);

				sqlstr = "DELETE FROM TWMA7 WHERE MAT_NO='" + twma2["MAT_NO"].ToString() + "'";
				Log::Trace("", __FUNCTION__, "{0}", sqlstr);
				Db::Execute(sqlstr);
				bcls_rec_delete.Tables[0].Rows.Add();
				bcls_rec_delete.Tables[0].Rows[SEQ_DELETE]["MAT_NO"] = twma2["MAT_NO"].ToString();
				SEQ_DELETE++;
				doFlag = f_wm00_pileinfocal(dtMat.Rows[0]["STOCK_NO"].ToString(), dtMat.Rows[0]["STOCK_PLACE_NO_TO"].ToString(), bcls_ret, conn);
				if (doFlag != 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
				doFlag = f_wm00_pileinfocal(dtMat.Rows[0]["STOCK_NO_FIN"].ToString(), dtMat.Rows[0]["STOCK_PLACE_NO_FIN"].ToString(), bcls_ret, conn);
				if (doFlag != 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
				doFlag = f_wm00_pileinfocal(dtMat.Rows[0]["STOCK_NO_FROM"].ToString(), dtMat.Rows[0]["STOCK_PLACE_NO_FROM"].ToString(), bcls_ret, conn);
				if (doFlag != 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
				continue;
			}

			if (twm04["STOCK_PLACE_TYPE"].ToString().Trim() == "D"
				&&twm04["DEV_DIV"].ToString().Trim() == "3")
			{
				Log::Trace("", __FUNCTION__, "材料在台车上");
				dtMat.Rows.Clear();
				sqlstr = "SELECT A.MAT_NO,B.CMD_SEQ,A.LAYERNO,A.STOCK_NO,A.HALL_NO,A.STOCK_PLACE_NO,B.CRANE_INST_STATUS,B.STOCK_OPER_ORDER_FIN,A.LAYERNO,B.STOCK_PLACE_NO_TO,B.STOCK_PLACE_NO_FROM,B.STOCK_NO_FROM,B.STOCK_NO_FIN,B.STOCK_PLACE_NO_FIN FROM TWMA2 A ,TWMA7 B "
					" WHERE A.MAT_NO = B.MAT_NO AND A.MAT_NO = '" + twma2["MAT_NO"].ToString() + "'";
				Log::Trace("", __FUNCTION__, "{0}", sqlstr);
				Db::QueryTable(sqlstr, dtMat);

				bcls_rec_update.Tables[0].Rows[0]["MAT_NO"] = dtMat.Rows[0]["MAT_NO"].ToString();
				bcls_rec_update.Tables[0].Rows[0]["STOCK_NO"] = dtMat.Rows[0]["STOCK_NO"];
				bcls_rec_update.Tables[0].Rows[0]["STOCK_NO_FROM"] = dtMat.Rows[0]["STOCK_NO"];
				bcls_rec_update.Tables[0].Rows[0]["HALL_NO_FR"] = dtMat.Rows[0]["HALL_NO"];
				bcls_rec_update.Tables[0].Rows[0]["YARD_LAYER_FROM"] = dtMat.Rows[0]["LAYERNO"];
				bcls_rec_update.Tables[0].Rows[0]["STOCK_PLACE_NO_FROM"] = dtMat.Rows[0]["STOCK_PLACE_NO"];
				bcls_rec_update.Tables[0].Rows[0]["STOCK_OPER_ORDER_NEW"] = "32";
				bcls_rec_update.Tables[0].Rows[0]["CRANE_INST_STATUS"] = dtMat.Rows[0]["CRANE_INST_STATUS"].ToString();
				bcls_rec_update.Tables[0].Rows[0]["STOCK_NO_TO"] = dtMat.Rows[0]["STOCK_NO"].ToString();
				bcls_rec_update.Tables[0].Rows[0]["HALL_NO_TO"] = dtMat.Rows[0]["HALL_NO"].ToString();
				bcls_rec_update.Tables[0].Rows[0]["STOCK_PLACE_NO_TO_OLD"] = dtMat.Rows[0]["STOCK_PLACE_NO_TO"].ToString();
				bcls_rec_update.Tables[0].Rows[0]["STOCK_PLACE_NO_TO_NEW"] = " ";
				bcls_rec_update.Tables[0].Rows[0]["DEV_DIV"] = twm04["DEV_DIV"].ToString();

				//逻辑区域不为空，目标库位为空，推荐库位

				bcls_rec_logic.Tables[0].Rows[0]["MAT_NO"] = dtMat.Rows[0]["MAT_NO"].ToString();
				bcls_rec_logic.Tables[0].Rows[0]["HALL_NO"] = dtMat.Rows[0]["HALL_NO"].ToString();
				bcls_rec_logic.Tables[0].Rows[0]["STOCK_NO"] = dtMat.Rows[0]["STOCK_NO"].ToString();
				bcls_rec_logic.Tables[0].Rows[0]["X_FROM"] = twm04["X_FROM"].ToDecimal();
				bcls_rec_logic.Tables[0].Rows[0]["Y_FROM"] = twm04["Y_FROM"].ToDecimal();
				
				/*if (dtMat.Rows[0]["HALL_NO"].ToString().Trim() == "H" && Table_Flag.Rows[0]["H_FLAG"].ToString().Trim()=="1")
				{
					bcls_rec_auto_1.Tables[0].Rows[0]["MAT_NO"] = dtMat.Rows[0]["MAT_NO"].ToString();
					bcls_rec_auto_1.Tables[0].Rows[0]["STOCK_OPER_ORDER"] = "32";
					bcls_rec_auto_1.Tables[0].Rows[0]["HALL_NO"] = "H";
					doFlag = f_auto(&bcls_rec_auto_1, &bcls_ret_logic, conn);
					if (doFlag != 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
					bcls_rec_update.Tables[0].Rows[0]["LOGIC_STOCK_NO"] = bcls_ret_logic.Tables[0].Rows[0]["LOGIC_STOCK_NO"].ToString();
					bcls_rec_update.Tables[0].Rows[0]["STOCK_PLACE_NO_TO_NEW"] = bcls_ret_logic.Tables[0].Rows[0]["STOCK_PLACE_NO"].ToString();
				}
				
			
			
				doFlag = f_wmsmsm_cranecmd_update(&bcls_rec_update, bcls_ret, conn);
				if (doFlag != 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}

				continue;*/

			}

			//如果卸下到备料区,下层材料有命令删除下层材料命令
			if (twm04["LOGIC_STOCK_NO"].ToString().Trim() == "08"
				|| twm04["LOGIC_STOCK_NO"].ToString().Trim() == "12")
			{
				//判断下层是否有上料命令
				sqlstr = "SELECT COUNT(1) FROM TWMA7 A,TWMA2 B WHERE A.MAT_NO=B.MAT_NO AND A.STOCK_PLACE_NO_FROM ='" + twma2["STOCK_PLACE_NO"].ToString().Trim() + "' AND INT(A.YARD_LAYER_FROM)<INT('" + twma2["LAYERNO"].ToString() + "') AND STOCK_OPER_ORDER='2B'";
				if (Db::QueryCDecimal(sqlstr)>0)
				{
					Log::Trace("", __FUNCTION__, "下层有上料命令");
					dtMat.Rows.Clear();
					sqlstr = "SELECT A.MAT_NO,B.CMD_SEQ,A.LOGIC_STOCK_NO AS LOGIC_STOCK_NO_TO ,A.LAYERNO,C.X_FROM,C.Y_FROM,A.STOCK_NO,A.HALL_NO,A.STOCK_PLACE_NO,B.CRANE_INST_STATUS,B.STOCK_OPER_ORDER,B.STOCK_OPER_ORDER_FIN,A.LAYERNO,B.STOCK_PLACE_NO_TO,B.STOCK_PLACE_NO_FROM,B.STOCK_NO_FROM,B.STOCK_NO_FIN,B.STOCK_PLACE_NO_FIN "
						" FROM TWMA2 A ,TWMA7 B ,TWM04 C"
						" WHERE A.MAT_NO = B.MAT_NO AND C.STOCK_PLACE_NO=A.STOCK_PLACE_NO  AND INT(A.LAYERNO) >= INT(" + twma2["LAYERNO"].ToString() +
						")  AND A.STOCK_PLACE_NO = '" + twma2["STOCK_PLACE_NO"].ToString() + "' ORDER BY INT(A.LAYERNO) ";
					Log::Trace("", __FUNCTION__, "{0}", sqlstr);
					Db::QueryTable(sqlstr, dtMat);

					bcls_rec_update.Tables[0].Rows[0]["MAT_NO"] = dtMat.Rows[0]["MAT_NO"].ToString();
					bcls_rec_update.Tables[0].Rows[0]["STOCK_NO"] = dtMat.Rows[0]["STOCK_NO"];
					bcls_rec_update.Tables[0].Rows[0]["STOCK_NO_FROM"] = dtMat.Rows[0]["STOCK_NO"];
					bcls_rec_update.Tables[0].Rows[0]["HALL_NO_FR"] = dtMat.Rows[0]["HALL_NO"];
					bcls_rec_update.Tables[0].Rows[0]["YARD_LAYER_FROM"] = dtMat.Rows[0]["LAYERNO"];
					bcls_rec_update.Tables[0].Rows[0]["STOCK_PLACE_NO_FROM"] = dtMat.Rows[0]["STOCK_PLACE_NO"];
					bcls_rec_update.Tables[0].Rows[0]["STOCK_OPER_ORDER_NEW"] = "30";
					bcls_rec_update.Tables[0].Rows[0]["CRANE_INST_STATUS"] = dtMat.Rows[0]["CRANE_INST_STATUS"].ToString();
					bcls_rec_update.Tables[0].Rows[0]["STOCK_NO_TO"] = dtMat.Rows[0]["STOCK_NO"].ToString();
					bcls_rec_update.Tables[0].Rows[0]["HALL_NO_TO"] = dtMat.Rows[0]["HALL_NO"].ToString();
					bcls_rec_update.Tables[0].Rows[0]["STOCK_PLACE_NO_TO_OLD"] = dtMat.Rows[0]["STOCK_PLACE_NO_TO"].ToString();
					bcls_rec_update.Tables[0].Rows[0]["STOCK_PLACE_NO_TO_NEW"] = " ";


					bcls_rec_logic.Tables[0].Rows[0]["MAT_NO"] = dtMat.Rows[0]["MAT_NO"].ToString();
					bcls_rec_logic.Tables[0].Rows[0]["HALL_NO"] = dtMat.Rows[0]["HALL_NO"].ToString();
					bcls_rec_logic.Tables[0].Rows[0]["STOCK_NO"] = dtMat.Rows[0]["STOCK_NO"].ToString();
					bcls_rec_logic.Tables[0].Rows[0]["X_FROM"] = dtMat.Rows[0]["X_FROM"].ToDecimal();
					bcls_rec_logic.Tables[0].Rows[0]["Y_FROM"] = dtMat.Rows[0]["Y_FROM"].ToDecimal();

					/*if (dtMat.Rows[0]["HALL_NO"].ToString().Trim() == "H" && Table_Flag.Rows[0]["H_FLAG"].ToString().Trim() == "1")
					{
						bcls_rec_auto_1.Tables[0].Rows[0]["MAT_NO"] = dtMat.Rows[0]["MAT_NO"].ToString();
						bcls_rec_auto_1.Tables[0].Rows[0]["STOCK_OPER_ORDER"] = "30";
						bcls_rec_auto_1.Tables[0].Rows[0]["HALL_NO"] = "H";
						doFlag = f_auto(&bcls_rec_auto_1, &bcls_ret_logic, conn);
						if (doFlag != 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}
						bcls_rec_update.Tables[0].Rows[0]["LOGIC_STOCK_NO"] = bcls_ret_logic.Tables[0].Rows[0]["LOGIC_STOCK_NO"].ToString();
						bcls_rec_update.Tables[0].Rows[0]["STOCK_PLACE_NO_TO_NEW"] = bcls_ret_logic.Tables[0].Rows[0]["STOCK_PLACE_NO"].ToString();
					}
					else if (dtMat.Rows[0]["HALL_NO"].ToString().Trim() == "G" && Table_Flag.Rows[0]["G_FLAG"].ToString().Trim() == "1")
					{
						bcls_rec_auto_1.Tables[0].Rows[0]["MAT_NO"] = dtMat.Rows[0]["MAT_NO"].ToString();
						bcls_rec_auto_1.Tables[0].Rows[0]["STOCK_OPER_ORDER"] = "30";
						bcls_rec_auto_1.Tables[0].Rows[0]["HALL_NO"] = "G";
						doFlag = f_auto(&bcls_rec_auto_1, &bcls_ret_logic, conn);
						if (doFlag != 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}
						bcls_rec_update.Tables[0].Rows[0]["LOGIC_STOCK_NO"] = bcls_ret_logic.Tables[0].Rows[0]["LOGIC_STOCK_NO"].ToString();
						bcls_rec_update.Tables[0].Rows[0]["STOCK_PLACE_NO_TO_NEW"] = bcls_ret_logic.Tables[0].Rows[0]["STOCK_PLACE_NO"].ToString();
					}
					else if (dtMat.Rows[0]["HALL_NO"].ToString().Trim() == "F" && Table_Flag.Rows[0]["F_FLAG"].ToString().Trim() == "1")
					{
						bcls_rec_auto_1.Tables[0].Rows[0]["MAT_NO"] = dtMat.Rows[0]["MAT_NO"].ToString();
						bcls_rec_auto_1.Tables[0].Rows[0]["STOCK_OPER_ORDER"] = "30";
						bcls_rec_auto_1.Tables[0].Rows[0]["HALL_NO"] = "F";
						doFlag = f_auto(&bcls_rec_auto_1, &bcls_ret_logic, conn);
						if (doFlag != 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}
						bcls_rec_update.Tables[0].Rows[0]["LOGIC_STOCK_NO"] = bcls_ret_logic.Tables[0].Rows[0]["LOGIC_STOCK_NO"].ToString();
						bcls_rec_update.Tables[0].Rows[0]["STOCK_PLACE_NO_TO_NEW"] = bcls_ret_logic.Tables[0].Rows[0]["STOCK_PLACE_NO"].ToString();
					}
					else
					{
						doFlag = f_wmsmsm_cranecmd_logic(&bcls_rec_logic, &bcls_ret_logic, conn);
						if (doFlag != 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}
						bcls_rec_update.Tables[0].Rows[0]["LOGIC_STOCK_NO"] = bcls_ret_logic.Tables[0].Rows[0]["LOGIC_STOCK_NO"].ToString();
						bcls_rec_update.Tables[0].Rows[0]["STOCK_PLACE_NO_TO_NEW"] = bcls_ret_logic.Tables[0].Rows[0]["STOCK_PLACE_NO"].ToString();

					}*/
					doFlag = f_wmsmsm_cranecmd_update(&bcls_rec_update, bcls_ret, conn);
					if (doFlag != 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}

					continue;
				}
				else
				{
					Log::Trace("", __FUNCTION__, "下层没有上料命令");
					//删除命令
					twma7_in.Reset();
					twma7_in["MAT_NO"] = twma2["MAT_NO"].ToString();
					twma7_in.Query("MAT_NO");
					sqlstr = "DELETE FROM TWMA7 WHERE MAT_NO='" + twma2["MAT_NO"].ToString() + "'";
					Log::Trace("", __FUNCTION__, "sqlstr {0}", sqlstr);
					Db::Execute(sqlstr);
					bcls_rec_delete.Tables[0].Rows.Add();
					bcls_rec_delete.Tables[0].Rows[SEQ_DELETE]["MAT_NO"] = twma2["MAT_NO"].ToString();
					SEQ_DELETE++;
					doFlag = f_wm00_pileinfocal(twma7_in["STOCK_NO"].ToString(), twma7_in["STOCK_PLACE_NO_TO"].ToString(), bcls_ret, conn);
					if (doFlag != 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
					doFlag = f_wm00_pileinfocal(twma7_in["STOCK_NO_FIN"].ToString(), twma7_in["STOCK_PLACE_NO_FIN"].ToString(), bcls_ret, conn);
					if (doFlag != 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
					doFlag = f_wm00_pileinfocal(twma7_in["STOCK_NO_FROM"].ToString(), twma7_in["STOCK_PLACE_NO_FROM"].ToString(), bcls_ret, conn);
					if (doFlag != 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}

					//删除下层命令
					sqlstr = "SELECT A.MAT_NO,A.STOCK_PLACE_NO_TO,A.STOCK_PLACE_NO_FROM,A.STOCK_NO,A.STOCK_NO_FROM,A.STOCK_NO_FIN,A.STOCK_PLACE_NO_FIN FROM TWMA7 A,TWMA2 B WHERE A.MAT_NO=B.MAT_NO AND A.STOCK_PLACE_NO_FROM ='" + twma2["STOCK_PLACE_NO"].ToString().Trim() + "' AND INT(A.YARD_LAYER_FROM)<=INT('" + twma2["LAYERNO"].ToString() + "')";
					Log::Trace("", __FUNCTION__, "sqlstr {0}", sqlstr);
					Db::QueryTable(sqlstr, Table_MAT);
					for (int t = 0; t < Table_MAT.Rows.get_Count(); t++)
					{
						sqlstr = "DELETE FROM TWMA7 WHERE MAT_NO='" + Table_MAT.Rows[t]["MAT_NO"].ToString() + "'";
						Log::Trace("", __FUNCTION__, "sqlstr {0}", sqlstr);
						Db::Execute(sqlstr);
						bcls_rec_delete.Tables[0].Rows.Add();
						bcls_rec_delete.Tables[0].Rows[SEQ_DELETE]["MAT_NO"] = Table_MAT.Rows[t]["MAT_NO"].ToString();
						SEQ_DELETE++;
						doFlag = f_wm00_pileinfocal(Table_MAT.Rows[t]["STOCK_NO"].ToString(), Table_MAT.Rows[t]["STOCK_PLACE_NO_TO"].ToString(), bcls_ret, conn);
						if (doFlag != 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}
						doFlag = f_wm00_pileinfocal(Table_MAT.Rows[t]["STOCK_NO_FIN"].ToString(), Table_MAT.Rows[t]["STOCK_PLACE_NO_FIN"].ToString(), bcls_ret, conn);
						if (doFlag != 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}
						doFlag = f_wm00_pileinfocal(Table_MAT.Rows[t]["STOCK_NO_FROM"].ToString(), Table_MAT.Rows[t]["STOCK_PLACE_NO_FROM"].ToString(), bcls_ret, conn);
						if (doFlag != 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}
					}


				}
			}
			else
			{
				dtMat.Rows.Clear();
				sqlstr = "SELECT A.MAT_NO,B.CMD_SEQ,A.LOGIC_STOCK_NO AS LOGIC_STOCK_NO_TO ,A.LAYERNO,C.X_FROM,C.Y_FROM,A.STOCK_NO,A.HALL_NO,A.STOCK_PLACE_NO,B.CRANE_INST_STATUS,B.STOCK_OPER_ORDER,B.STOCK_OPER_ORDER_FIN,A.LAYERNO,B.STOCK_PLACE_NO_TO,B.STOCK_PLACE_NO_FROM,B.STOCK_NO_FROM,B.STOCK_NO_FIN,B.STOCK_PLACE_NO_FIN "
					" FROM TWMA2 A ,TWMA7 B ,TWM04 C"
					" WHERE A.MAT_NO = B.MAT_NO AND C.STOCK_PLACE_NO=A.STOCK_PLACE_NO  AND INT(A.LAYERNO) >= INT(" + twma2["LAYERNO"].ToString() +
					")  AND A.STOCK_PLACE_NO = '" + twma2["STOCK_PLACE_NO"].ToString() + "' ORDER BY INT(A.LAYERNO) ";
				Log::Trace("", __FUNCTION__, "{0}", sqlstr);
				Db::QueryTable(sqlstr, dtMat);

				//检查下层是否有命令
				sqlstr = "SELECT COUNT(1) FROM TWMA2 A , TWMA7 B "
					" WHERE A.MAT_NO = B.MAT_NO AND INT(A.LAYERNO)<INT(" + twma2["LAYERNO"].ToString() + ")"
					+ " AND A.STOCK_PLACE_NO = '" + twma2["STOCK_PLACE_NO"].ToString() + "'";
				Log::Trace("", __FUNCTION__, "{0}", sqlstr);
				if (Db::QueryCDecimal(sqlstr) == 0)
				{
					Log::Trace("", __FUNCTION__, "下层没有命令，从下往上删");
					Log::Trace("", __FUNCTION__, "dtMat.Rows.get_Count()", dtMat.Rows.get_Count());
					for (int j = 0; j < dtMat.Rows.get_Count(); j++)
					{
						Log::Trace("", __FUNCTION__, "材料{0}删命令", dtMat.Rows[j]["MAT_NO"].ToString());
						if (j == 0)
						{
							sqlstr = "DELETE FROM TWMA7 WHERE MAT_NO='" + dtMat.Rows[0]["MAT_NO"].ToString() + "'";
							Db::Execute(sqlstr);
							bcls_rec_delete.Tables[0].Rows.Add();
							bcls_rec_delete.Tables[0].Rows[SEQ_DELETE]["MAT_NO"] = dtMat.Rows[0]["MAT_NO"].ToString();
							SEQ_DELETE++;
							doFlag = f_wm00_pileinfocal(dtMat.Rows[j]["STOCK_NO"].ToString(), dtMat.Rows[j]["STOCK_PLACE_NO_TO"].ToString(), bcls_ret, conn);
							if (doFlag != 0)
							{
								throw CApplicationException(-1, s.msg, log.Location);
							}
							doFlag = f_wm00_pileinfocal(dtMat.Rows[j]["STOCK_NO_FIN"].ToString(), dtMat.Rows[j]["STOCK_PLACE_NO_FIN"].ToString(), bcls_ret, conn);
							if (doFlag != 0)
							{
								throw CApplicationException(-1, s.msg, log.Location);
							}
							doFlag = f_wm00_pileinfocal(dtMat.Rows[j]["STOCK_NO_FROM"].ToString(), dtMat.Rows[j]["STOCK_PLACE_NO_FROM"].ToString(), bcls_ret, conn);
							if (doFlag != 0)
							{
								throw CApplicationException(-1, s.msg, log.Location);
							}
							continue;
						}
						else
						{
							if (dtMat.Rows[j]["STOCK_OPER_ORDER"].ToString().Trim() == "30"&&
								dtMat.Rows[j]["CRANE_INST_STATUS"].ToString() == "0")
							{
								sqlstr = "DELETE FROM TWMA7 WHERE MAT_NO='" + dtMat.Rows[j]["MAT_NO"].ToString() + "'";
								Db::Execute(sqlstr);
								bcls_rec_delete.Tables[0].Rows.Add();
								bcls_rec_delete.Tables[0].Rows[SEQ_DELETE]["MAT_NO"] = dtMat.Rows[j]["MAT_NO"].ToString();
								SEQ_DELETE++;
								doFlag = f_wm00_pileinfocal(dtMat.Rows[j]["STOCK_NO"].ToString(), dtMat.Rows[j]["STOCK_PLACE_NO_TO"].ToString(), bcls_ret, conn);
								if (doFlag != 0)
								{
									throw CApplicationException(-1, s.msg, log.Location);
								}
								doFlag = f_wm00_pileinfocal(dtMat.Rows[j]["STOCK_NO_FIN"].ToString(), dtMat.Rows[j]["STOCK_PLACE_NO_FIN"].ToString(), bcls_ret, conn);
								if (doFlag != 0)
								{
									throw CApplicationException(-1, s.msg, log.Location);
								}
								doFlag = f_wm00_pileinfocal(dtMat.Rows[j]["STOCK_NO_FROM"].ToString(), dtMat.Rows[j]["STOCK_PLACE_NO_FROM"].ToString(), bcls_ret, conn);
								if (doFlag != 0)
								{
									throw CApplicationException(-1, s.msg, log.Location);
								}
							}
							else
							{
								Log::Trace("", __FUNCTION__, "材料{0}不能删命令", dtMat.Rows[j]["MAT_NO"].ToString());
								break;
							}
							continue;
						}
					}
				}
				else
				{
					Log::Trace("", __FUNCTION__, "下层有命令,替换为倒垛命令");

					bcls_rec_update.Tables[0].Rows[0]["MAT_NO"] = dtMat.Rows[0]["MAT_NO"].ToString();
					bcls_rec_update.Tables[0].Rows[0]["STOCK_NO"] = dtMat.Rows[0]["STOCK_NO"];
					bcls_rec_update.Tables[0].Rows[0]["STOCK_NO_FROM"] = dtMat.Rows[0]["STOCK_NO"];
					bcls_rec_update.Tables[0].Rows[0]["HALL_NO_FR"] = dtMat.Rows[0]["HALL_NO"];
					bcls_rec_update.Tables[0].Rows[0]["YARD_LAYER_FROM"] = dtMat.Rows[0]["LAYERNO"];
					bcls_rec_update.Tables[0].Rows[0]["STOCK_PLACE_NO_FROM"] = dtMat.Rows[0]["STOCK_PLACE_NO"];
					bcls_rec_update.Tables[0].Rows[0]["STOCK_OPER_ORDER_NEW"] = "30";
					bcls_rec_update.Tables[0].Rows[0]["CRANE_INST_STATUS"] = dtMat.Rows[0]["CRANE_INST_STATUS"].ToString();
					bcls_rec_update.Tables[0].Rows[0]["STOCK_NO_TO"] = dtMat.Rows[0]["STOCK_NO"].ToString();
					bcls_rec_update.Tables[0].Rows[0]["HALL_NO_TO"] = dtMat.Rows[0]["HALL_NO"].ToString();
					bcls_rec_update.Tables[0].Rows[0]["STOCK_PLACE_NO_TO_OLD"] = dtMat.Rows[0]["STOCK_PLACE_NO_TO"].ToString();
					bcls_rec_update.Tables[0].Rows[0]["STOCK_PLACE_NO_TO_NEW"] = " ";

					//if (dtMat.Rows[0]["HALL_NO"].ToString().Trim() == "H" && Table_Flag.Rows[0]["H_FLAG"].ToString().Trim() == "1")
					//{
					//	bcls_rec_auto_1.Tables[0].Rows[0]["MAT_NO"] = dtMat.Rows[0]["MAT_NO"].ToString();
					//	bcls_rec_auto_1.Tables[0].Rows[0]["STOCK_OPER_ORDER"] = "30";
					//	bcls_rec_auto_1.Tables[0].Rows[0]["HALL_NO"] = "H";
					//	doFlag = f_auto(&bcls_rec_auto_1, &bcls_ret_logic, conn);
					//	if (doFlag != 0)
					//	{
					//		throw CApplicationException(-1, s.msg, log.Location);
					//	}
					//	bcls_rec_update.Tables[0].Rows[0]["LOGIC_STOCK_NO"] = bcls_ret_logic.Tables[0].Rows[0]["LOGIC_STOCK_NO"].ToString();
					//	bcls_rec_update.Tables[0].Rows[0]["STOCK_PLACE_NO_TO_NEW"] = bcls_ret_logic.Tables[0].Rows[0]["STOCK_PLACE_NO"].ToString();
					//}
					//else if (dtMat.Rows[0]["HALL_NO"].ToString().Trim() == "G" && Table_Flag.Rows[0]["G_FLAG"].ToString().Trim() == "1")
					//{
					//	bcls_rec_auto_1.Tables[0].Rows[0]["MAT_NO"] = dtMat.Rows[0]["MAT_NO"].ToString();
					//	bcls_rec_auto_1.Tables[0].Rows[0]["STOCK_OPER_ORDER"] = "30";
					//	bcls_rec_auto_1.Tables[0].Rows[0]["HALL_NO"] = "G";
					//	doFlag = f_auto(&bcls_rec_auto_1, &bcls_ret_logic, conn);
					//	if (doFlag != 0)
					//	{
					//		throw CApplicationException(-1, s.msg, log.Location);
					//	}
					//	bcls_rec_update.Tables[0].Rows[0]["LOGIC_STOCK_NO"] = bcls_ret_logic.Tables[0].Rows[0]["LOGIC_STOCK_NO"].ToString();
					//	bcls_rec_update.Tables[0].Rows[0]["STOCK_PLACE_NO_TO_NEW"] = bcls_ret_logic.Tables[0].Rows[0]["STOCK_PLACE_NO"].ToString();
					//}
					//else if (dtMat.Rows[0]["HALL_NO"].ToString().Trim() == "F" && Table_Flag.Rows[0]["F_FLAG"].ToString().Trim() == "1")
					//{
					//	bcls_rec_auto_1.Tables[0].Rows[0]["MAT_NO"] = dtMat.Rows[0]["MAT_NO"].ToString();
					//	bcls_rec_auto_1.Tables[0].Rows[0]["STOCK_OPER_ORDER"] = "30";
					//	bcls_rec_auto_1.Tables[0].Rows[0]["HALL_NO"] = "F";
					//	doFlag = f_auto(&bcls_rec_auto_1, &bcls_ret_logic, conn);
					//	if (doFlag != 0)
					//	{
					//		throw CApplicationException(-1, s.msg, log.Location);
					//	}
					//	bcls_rec_update.Tables[0].Rows[0]["LOGIC_STOCK_NO"] = bcls_ret_logic.Tables[0].Rows[0]["LOGIC_STOCK_NO"].ToString();
					//	bcls_rec_update.Tables[0].Rows[0]["STOCK_PLACE_NO_TO_NEW"] = bcls_ret_logic.Tables[0].Rows[0]["STOCK_PLACE_NO"].ToString();
					//}
					//else
					//{
					//	//逻辑区域不为空，目标库位为空，推荐库位
					//	if (dtMat.Rows[0]["LOGIC_STOCK_NO_TO"].ToString().Trim() != ""
					//		&&bcls_rec_update.Tables[0].Rows[0]["STOCK_PLACE_NO_TO_NEW"].ToString().Trim() == "")
					//	{
					//		bcls_rec_auto.Tables[0].Rows[0]["MAT_NO"] = dtMat.Rows[0]["MAT_NO"].ToString();
					//		bcls_rec_auto.Tables[0].Rows[0]["LOGIC_STOCK_NO"] = dtMat.Rows[0]["LOGIC_STOCK_NO_TO"].ToString();
					//		bcls_rec_auto.Tables[0].Rows[0]["HALL_NO"] = dtMat.Rows[0]["HALL_NO"].ToString();
					//		bcls_rec_auto.Tables[0].Rows[0]["BASE_X"] = dtMat.Rows[0]["X_FROM"].ToDecimal();
					//		bcls_rec_auto.Tables[0].Rows[0]["BASE_Y"] = dtMat.Rows[0]["Y_FROM"].ToDecimal();
					//		bcls_rec_auto.Tables[0].Rows[0]["STNO_RULE_FLAG"] = logic_rule.Rows[0]["STNO_MATCH_RELATION"].ToString();
					//		bcls_rec_auto.Tables[0].Rows[0]["ORDER_RULE_FLAG"] = logic_rule.Rows[0]["ORDER_CONFIRM_FLAG"].ToString();
					//		bcls_rec_auto.Tables[0].Rows[0]["WIDTH_DIFF"] = logic_rule.Rows[0]["WIDTH_DELTA"].ToDecimal();//50;
					//		doFlag = f_wms_auto(&bcls_rec_auto, &bcls_ret_place, conn);
					//		if (doFlag != 0)
					//		{
					//			throw CApplicationException(-1, s.msg, log.Location);
					//		}
					//		bcls_rec_update.Tables[0].Rows[0]["STOCK_PLACE_NO_TO_NEW"] = bcls_ret_place.Tables["BLK_OUTPUT"].Rows[0]["AIM_STOCK_PLACE_NO"].ToString().Trim();
					//	}

					//	//目标库位和目标逻辑区为空时，推荐
					//	if (dtMat.Rows[0]["LOGIC_STOCK_NO_TO"].ToString().Trim() == ""
					//		&&bcls_rec_update.Tables[0].Rows[0]["STOCK_PLACE_NO_TO_NEW"].ToString().Trim() == "")
					//	{
					//		bcls_rec_logic.Tables[0].Rows[0]["MAT_NO"] = dtMat.Rows[0]["MAT_NO"].ToString();
					//		bcls_rec_logic.Tables[0].Rows[0]["HALL_NO"] = dtMat.Rows[0]["HALL_NO"].ToString();
					//		bcls_rec_logic.Tables[0].Rows[0]["STOCK_NO"] = dtMat.Rows[0]["STOCK_NO"].ToString();
					//		bcls_rec_logic.Tables[0].Rows[0]["X_FROM"] = dtMat.Rows[0]["X_FROM"].ToDecimal();
					//		bcls_rec_logic.Tables[0].Rows[0]["Y_FROM"] = dtMat.Rows[0]["Y_FROM"].ToDecimal();

					//		doFlag = f_wmsmsm_cranecmd_logic(&bcls_rec_logic, &bcls_ret_logic, conn);
					//		if (doFlag != 0)
					//		{
					//			throw CApplicationException(-1, s.msg, log.Location);
					//		}
					//		bcls_rec_update.Tables[0].Rows[0]["LOGIC_STOCK_NO"] = bcls_ret_logic.Tables[0].Rows[0]["LOGIC_STOCK_NO"].ToString();
					//		bcls_rec_update.Tables[0].Rows[0]["STOCK_PLACE_NO_TO_NEW"] = bcls_ret_logic.Tables[0].Rows[0]["STOCK_PLACE_NO"].ToString();
					//	}
					//}
					/*doFlag = f_wmsmsm_cranecmd_update(&bcls_rec_update, bcls_ret, conn);
					if (doFlag != 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}*/

					continue;
				}
			}
		}

		////推荐逻辑库区
		//Log::Trace("", __FUNCTION__, "推荐逻辑库区    ");
		//sqlstr = "SELECT A.HALL_NO_TO,A.LOGIC_STOCK_NO,A.STOCK_NO_TO,A.MAT_NO,B.MAT_THEORY_WT,B.MAT_THICK"
		//	" FROM TWMA7 A, TOPHPMMS1 B"
		//	" WHERE A.MAT_NO = B.MAT_NO AND A.LOGIC_STOCK_NO = '' AND A.STOCK_PLACE_NO_TO = '' AND A.SEND_FLAG!='1'";
		//Db::QueryTable(sqlstr, Table_auto);
		//for (int i = 0; i < Table_auto.Rows.get_Count(); i++)
		//{			
		//	bcls_rec_logic.Tables[0].Rows.Add();
		//	bcls_rec_logic.Tables[0].Rows[i]["MAT_NO"] = Table_auto.Rows[i]["MAT_NO"].ToString();
		//	bcls_rec_logic.Tables[0].Rows[i]["HALL_NO"] = Table_auto.Rows[i]["HALL_NO_TO"].ToString();
		//}
		//
		//if (bcls_rec_logic.Tables[0].Rows.get_Count()>0)
		//{	
		//	doFlag = f_wmsmsm_cranecmd_logic(&bcls_rec_logic, bcls_ret, conn);
		//	if (doFlag != 0)
		//	{
		//		throw CApplicationException(-1, s.msg, log.Location);
		//	}			
		//}

		////推荐垛位
		//Log::Trace("", __FUNCTION__, "推荐垛位    ");
		//Table_auto.Clear();
		//sqlstr = "SELECT A.HALL_NO_TO,A.LOGIC_STOCK_NO,A.MAT_NO"
		//	" FROM TWMA7 A, TOPHPMMS1 B"
		//	" WHERE A.MAT_NO = B.MAT_NO AND A.LOGIC_STOCK_NO != '' AND A.STOCK_PLACE_NO_TO = ' ' AND A.SEND_FLAG!='1'"
		//	" GROUP BY A.HALL_NO_TO,A.LOGIC_STOCK_NO,A.MAT_NO"
		//	" ORDER BY A.HALL_NO_TO,A.LOGIC_STOCK_NO ";
		//Db::QueryTable(sqlstr, Table_auto);

		//for (int i = 0; i < Table_auto.Rows.get_Count(); i++)
		//{
		//	if (hall_no.Trim() == ""
		//		|| hall_no.Trim() != Table_auto.Rows[i]["HALL_NO_TO"].ToString()
		//		|| logic_stock_no != Table_auto.Rows[i]["LOGIC_STOCK_NO"].ToString())
		//	{
		//		if (hall_no.Trim() != "")
		//		{
		//			EIClass bcls_ret_place;
		//			doFlag = f_auto(&bcls_rec_auto_1, &bcls_ret_place, conn);
		//			if (doFlag != 0)
		//			{
		//				throw CApplicationException(-1, s.msg, log.Location);
		//			}
		//			
		//			for (int t = 0; t < bcls_ret_place.Tables[0].Rows.get_Count(); t++)
		//			{
		//				twma7_in.Reset();
		//				twma7_in["MAT_NO"] = bcls_ret_place.Tables[0].Rows[t]["MAT_NO"].ToString().Trim();
		//				twma7_in["STOCK_PLACE_NO_TO"] = bcls_ret_place.Tables[0].Rows[t]["STOCK_PLACE_NO"].ToString().Trim();
		//				twma7_in.Update("STOCK_PLACE_NO_TO", "MAT_NO");
		//				if (twma7_in["STOCK_PLACE_NO_TO"].ToString().Trim() != "")
		//				{
		//					//更新垛位状态
		//					doFlag = f_wm00_pileinfocal("P31", twma7_in["STOCK_PLACE_NO_TO"].ToString(), bcls_ret, conn);
		//					if (doFlag != 0)
		//					{
		//						throw CApplicationException(-1, s.msg, log.Location);
		//					}
		//				}
		//			}
		//		}

		//		bcls_rec_auto_1.Tables[0].Rows[0]["LOGIC_STOCK_NO"] = Table_auto.Rows[i]["LOGIC_STOCK_NO"].ToString();
		//		bcls_rec_auto_1.Tables[0].Rows[0]["HALL_NO_TO"] = Table_auto.Rows[i]["HALL_NO_TO"].ToString();
		//		SEQ_A = 0;
		//		bcls_rec_auto_1.Tables[1].Rows.Clear();
		//	}

		//	hall_no = Table_auto.Rows[i]["HALL_NO_TO"].ToString();
		//	logic_stock_no = Table_auto.Rows[i]["LOGIC_STOCK_NO"].ToString();

		//	bcls_rec_auto_1.Tables[1].Rows.Add();
		//	bcls_rec_auto_1.Tables[1].Rows[SEQ_A]["MAT_NO"] = Table_auto.Rows[i]["MAT_NO"].ToString();
		//	SEQ_A++;

		//	if (i == Table_auto.Rows.get_Count() - 1)
		//	{
		//		Log::Trace("", __FUNCTION__, "end");
		//		EIClass bcls_ret_place;
		//		doFlag = f_auto(&bcls_rec_auto_1, &bcls_ret_place, conn);
		//		if (doFlag != 0)
		//		{
		//			throw CApplicationException(-1, s.msg, log.Location);
		//		}
		//		for (int t = 0; t < bcls_ret_place.Tables[0].Rows.get_Count(); t++)
		//		{
		//			twma7_in.Reset();
		//			twma7_in["MAT_NO"] = bcls_ret_place.Tables[0].Rows[t]["MAT_NO"].ToString().Trim();
		//			twma7_in["STOCK_PLACE_NO_TO"] = bcls_ret_place.Tables[0].Rows[t]["STOCK_PLACE_NO"].ToString().Trim();
		//			twma7_in.Update("STOCK_PLACE_NO_TO", "MAT_NO");
		//			if (twma7_in["STOCK_PLACE_NO_TO"].ToString().Trim() != "")
		//			{
		//				//更新垛位状态
		//				doFlag = f_wm00_pileinfocal("P31", twma7_in["STOCK_PLACE_NO_TO"].ToString(), bcls_ret, conn);
		//				if (doFlag != 0)
		//				{
		//					throw CApplicationException(-1, s.msg, log.Location);
		//				}
		//			}
		//		}

		//	}
		//}


		/*doFlag = f_wmsmsm_u1dl14_snd(&bcls_rec_delete, bcls_ret, conn);
		if (doFlag != 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}*/

		Log::Trace("", __FUNCTION__, "f_wmsmsm_cranecmd_delete____________end    ");
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };

		/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/
		CMessageFormat::Format(s.msg, "Database processing error. sqlcode=[{0}].", arguments, 1);
		CString str = ex.GetMsg() + "\r\n" + sqlstr;

		/*返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应*/
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);

		/*数据库异常时返回-1，事务将被回滚*/
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), 399);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), 399);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;



}