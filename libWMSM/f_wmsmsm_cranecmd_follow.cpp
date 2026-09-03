/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:      JQ
Version:     1.1.1
Date:         2017-3-16
Description: 板坯吊车命令后续处理函数
**************************************************/

/* C/C++ 的标准头文件部分 */
#include "stdafx.h"		// 框架头，不可删除 

int f_wmsmsm_cranecmd_delete(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_wmsmsm_cranecmd_update(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn); //替换命令
int f_wmsmsm_temarea_out(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_wmsmsm_cranecmd_logic(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//推荐逻辑区
//int f_wms_auto(EIClass* blkIn, EIClass* blkOut, CDbConnection* dbConn);
int f_auto(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

BM2_FUNCTION_EXPORT
int f_wmsmsm_cranecmd_follow(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{

	//程序内部变量
	int doFlag = 0;
	CString sqlstr = " ";
	CString matNo = " ";

	//数据存放块
	CDataTable cmd_mat;                             //存放材料
	CDataTable cmd_mat_K;                             //存放材料
	CDataTable cmd_mat_plan;                             //存放材料
	//数据块
	CDataTable logic_rule;                         //twma8

	EIClass bcls_rec_delete;
	bcls_rec_delete.Tables[0].set_TableName("CMD_DELETE");
	bcls_rec_delete.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
	bcls_rec_delete.Tables[0].Rows.Add();

	EIClass bcls_rec_out;
	bcls_rec_out.Tables[0].set_TableName("TEMAREA_OUT");
	bcls_rec_out.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO");
	bcls_rec_out.Tables[0].Rows.Add();

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
	bcls_rec_update.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO_FIN");
	bcls_rec_update.Tables[0].Columns.Add(DT_STRING, "LOGIC_STOCK_NO");
	bcls_rec_update.Tables[0].Columns.Add(DT_STRING, "DEV_DIV");
	bcls_rec_update.Tables[0].Columns.Add(DT_STRING, "SEND_FLAG");
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
	bcls_rec_auto.Tables[0].Columns.Add(DT_DECIMAL, "BASE_X");
	bcls_rec_auto.Tables[0].Columns.Add(DT_DECIMAL, "BASE_Y");
	bcls_rec_auto.Tables[0].Columns.Add(DT_STRING, "STNO_RULE_FLAG");
	bcls_rec_auto.Tables[0].Columns.Add(DT_STRING, "ORDER_RULE_FLAG");
	bcls_rec_auto.Tables[0].Columns.Add(DT_DECIMAL, "WIDTH_DIFF");
	bcls_rec_auto.Tables[0].Rows.Add();

	EIClass bcls_rec_auto_1;
	bcls_rec_auto_1.Tables[0].set_TableName("AUTO_INFO_IN");
	bcls_rec_auto_1.Tables[0].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");
	bcls_rec_auto_1.Tables[0].Columns.Add(DT_STRING, "HALL_NO");
	bcls_rec_auto_1.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
	bcls_rec_auto_1.Tables[0].Rows.Add();

	

	try
	{
		//项目自定义日志
		CTracer log(__FUNCTION__);

		// 判断是否存在指定块
		if (bcls_rec->Tables.IndexOf("WM00_FOLLOW") < 0 ||
			bcls_rec->Tables["WM00_FOLLOW"].Rows.get_Count() == 0)
		{
			strcpy(s.msg, "函数f_wmsmsm_cranecmd_follow中找不到接收块名[WM00_FOLLOW]");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		logic_rule.Rows.Clear();
		//获取材料信息
		//sqlstr = "SELECT * "
		//	" FROM TWMA8";
		//Db::QueryTable(sqlstr, logic_rule);

		////获取库位推荐参数
		//CDataTable   Table_Flag;
		//sqlstr = "SELECT FLAG_1 AS H_FLAG,FLAG_2 AS G_FLAG,FLAG_3 AS F_FLAG FROM TWMA8";
		//Db::QueryTable(sqlstr, Table_Flag);

		Log::Trace("", __FUNCTION__, "Rows.get_Count：【{0}】", bcls_rec->Tables["WM00_FOLLOW"].Rows.get_Count());
		for (int i = 0; i < bcls_rec->Tables["WM00_FOLLOW"].Rows.get_Count(); i++)
		{
			matNo = bcls_rec->Tables["WM00_FOLLOW"].Rows[i]["MAT_NO"].ToString().Trim();
			Log::Trace("", __FUNCTION__, "材料号：【{0}】", matNo);
			Log::Trace("", __FUNCTION__, "材料号");
			//判断是否传入材料号
			if (matNo == "")
			{
				strcpy(s.msg, "传入材料号为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			sqlstr = "UPDATE TWMA7 SET CRANE_INST_STATUS='0' WHERE MAT_NO='" + matNo + "'";
			Db::Execute(sqlstr);

			sqlstr = "SELECT A.MAT_NO,A.MAT_ACT_WT,A.MAT_ACT_THICK,C.MAX_HEIGHT,A.CMD_SEQ,' ' AS LOGIC_STOCK_NO_TO,C.LOGIC_STOCK_NO,A.STOCK_PLACE_NO_TO,A.STOCK_OPER_ORDER,A.CRANE_INST_STATUS,"
				" B.LAYERNO,C.STOCK_NO,C.HALL_NO,C.STOCK_PLACE_NO,C.STOCK_PLACE_TYPE,C.COLUMN_NO,C.DEV_DIV,A.STOCK_NO_FIN,"
				" A.HALL_NO_FIN,A.STOCK_OPER_ORDER_FIN,A.STOCK_PLACE_NO_FIN,C.TR_NO,C.X_FROM,C.Y_FROM,C.ENTRANCE_EXIT_DIV,C.STOCK_STATUS,C.MAX_WT"
				" FROM TWMA2 B "
				" LEFT JOIN TWMA7 A ON A.MAT_NO = B.MAT_NO"
				" LEFT JOIN TWM04 C ON C.STOCK_PLACE_NO = B.STOCK_PLACE_NO"
				" WHERE B.MAT_NO='" + matNo + "'";
			Log::Trace("", __FUNCTION__, "{0}", sqlstr);
			Db::QueryTable(sqlstr, cmd_mat);
			if (cmd_mat.Rows.get_Count() == 0)
			{
				Log::Trace("", __FUNCTION__, "传入材料信息不存在");
				break;
			}
			if (cmd_mat.Rows[0]["STOCK_OPER_ORDER"].ToString().Trim() == "")
			{
			/*	Log::Trace("", __FUNCTION__, "材料不存在命令");
				doFlag = f_wmsmsm_temarea_out(bcls_rec, bcls_ret, conn);
				if (doFlag != 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}*/
				continue;
			}
			Log::Trace("", __FUNCTION__, "2");
			if (cmd_mat.Rows[0]["STOCK_OPER_ORDER"].ToString().Trim().Substring(0, 1) == "1")
			{
				Log::Trace("", __FUNCTION__, "入库命令，删命令");
				bcls_rec_delete.Tables[0].Rows[0]["MAT_NO"] = matNo;
				doFlag = f_wmsmsm_cranecmd_delete(&bcls_rec_delete, bcls_ret, conn);
				if (doFlag != 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}

				//二切临时区域，垛位堆满以后生成出来的命令	
				if (cmd_mat.Rows[0]["LOGIC_STOCK_NO"].ToString().Trim() == "07")
				{
					//判断垛位是否堆满
					/*doFlag = f_wmsmsm_temarea_out(bcls_rec, bcls_ret, conn);
					if (doFlag != 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}*/
					continue;
				}
				continue;
			}

			if (cmd_mat.Rows[0]["STOCK_OPER_ORDER"].ToString().Trim() == "2E")
			{
				if (cmd_mat.Rows[0]["STOCK_PLACE_TYPE"].ToString().Trim() == "D"
					&&cmd_mat.Rows[0]["DEV_DIV"].ToString().Trim() == "1")
				{
					Log::Trace("", __FUNCTION__, "材料已到目标垛位，删命令");
					bcls_rec_delete.Tables[0].Rows[0]["MAT_NO"] = matNo;
					doFlag = f_wmsmsm_cranecmd_delete(&bcls_rec_delete, bcls_ret, conn);
					if (doFlag != 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
					continue;
				}

			}

			//上料命令到达B开头的库位，或者12开头的库位结束
			if (cmd_mat.Rows[0]["STOCK_OPER_ORDER"].ToString().Trim() == "2B")
			{
				if ((cmd_mat.Rows[0]["DEV_DIV"].ToString() == "4"
					&& cmd_mat.Rows[0]["ENTRANCE_EXIT_DIV"].ToString() == "1")
					|| cmd_mat.Rows[0]["STOCK_PLACE_NO"].ToString().Substring(0, 1) == "B"
					|| cmd_mat.Rows[0]["LOGIC_STOCK_NO"].ToString().Trim() == "13"
					)
				{
					Log::Trace("", __FUNCTION__, "材料已到上料垛位，删命令");
					bcls_rec_delete.Tables[0].Rows[0]["MAT_NO"] = matNo;
					doFlag = f_wmsmsm_cranecmd_delete(&bcls_rec_delete, bcls_ret, conn);
					if (doFlag != 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
					continue;
				}
				else
				{
					bcls_rec_update.Tables[0].Rows[0]["MAT_NO"] = matNo;
					bcls_rec_update.Tables[0].Rows[0]["STOCK_NO"] = cmd_mat.Rows[0]["STOCK_NO"];
					bcls_rec_update.Tables[0].Rows[0]["STOCK_NO_FROM"] = cmd_mat.Rows[0]["STOCK_NO"];
					bcls_rec_update.Tables[0].Rows[0]["HALL_NO_FR"] = cmd_mat.Rows[0]["HALL_NO"];
					bcls_rec_update.Tables[0].Rows[0]["YARD_LAYER_FROM"] = cmd_mat.Rows[0]["LAYERNO"];
					bcls_rec_update.Tables[0].Rows[0]["STOCK_PLACE_NO_FROM"] = cmd_mat.Rows[0]["STOCK_PLACE_NO"];
					bcls_rec_update.Tables[0].Rows[0]["STOCK_OPER_ORDER_NEW"] = "2B";
					bcls_rec_update.Tables[0].Rows[0]["CRANE_INST_STATUS"] = cmd_mat.Rows[0]["CRANE_INST_STATUS"].ToString();
					bcls_rec_update.Tables[0].Rows[0]["STOCK_NO_TO"] = cmd_mat.Rows[0]["STOCK_NO_FIN"].ToString();
					bcls_rec_update.Tables[0].Rows[0]["HALL_NO_TO"] = cmd_mat.Rows[0]["HALL_NO_FIN"].ToString();
					bcls_rec_update.Tables[0].Rows[0]["STOCK_PLACE_NO_TO_OLD"] = cmd_mat.Rows[0]["STOCK_PLACE_NO_TO"].ToString();
					bcls_rec_update.Tables[0].Rows[0]["STOCK_PLACE_NO_TO_NEW"] = cmd_mat.Rows[0]["STOCK_PLACE_NO_FIN"].ToString();
					bcls_rec_update.Tables[0].Rows[0]["STOCK_PLACE_NO_FIN"] = cmd_mat.Rows[0]["STOCK_PLACE_NO_FIN"].ToString();
					bcls_rec_update.Tables[0].Rows[0]["DEV_DIV"] = cmd_mat.Rows[0]["DEV_DIV"].ToString();
					bcls_rec_update.Tables[0].Rows[0]["LOGIC_STOCK_NO"] = cmd_mat.Rows[0]["LOGIC_STOCK_NO_TO"].ToString();

					if (bcls_rec->Tables["WM00_FOLLOW"].Columns.Contains("SEND_FLAG"))
						bcls_rec_update.Tables[0].Rows[0]["SEND_FLAG"] = bcls_rec->Tables["WM00_FOLLOW"].Rows[i]["SEND_FLAG"].ToString();

					doFlag = f_wmsmsm_cranecmd_update(&bcls_rec_update, bcls_ret, conn);
					if (doFlag != 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
					continue;
				}

				//if (cmd_mat.Rows[0]["STOCK_PLACE_NO"].ToString().Substring(0, 1) == "B" 
				//	|| cmd_mat.Rows[0]["STOCK_PLACE_NO"].ToString().Substring(0, 2) == "12"
				//	|| cmd_mat.Rows[0]["STOCK_PLACE_NO"].ToString().Trim() == "CUT"
				//	|| cmd_mat.Rows[0]["STOCK_PLACE_NO"].ToString().Trim()== "")
				//{
				//	Log::Trace("", __FUNCTION__, "材料已到上料垛位，删命令");
				//	bcls_rec_delete.Tables[0].Rows[0]["MAT_NO"] = matNo;
				//	doFlag = f_wmsmsm_cranecmd_delete(&bcls_rec_delete, bcls_ret, conn);
				//	if (doFlag != 0)
				//	{
				//		throw CApplicationException(-1, s.msg, log.Location);
				//	}
				//	continue;
				//}
				////判断是否为手动切割
				//sqlstr = "SELECT PLAN_NO FROM TOPHPMMS1 WHERE MAT_NO ='" + matNo + "'";
				//Db::QueryTable(sqlstr, cmd_mat_plan);
				//if (cmd_mat_plan.Rows.get_Count() > 0
				//	&& cmd_mat_plan.Rows[0]["PLAN_NO"].ToString().Trim().GetLength() > 3
				//	&& cmd_mat_plan.Rows[0]["PLAN_NO"].ToString().Trim().Substring(0, 2) == "MB"
				//	&& (cmd_mat.Rows[0]["LOGIC_STOCK_NO"].ToString().Trim() == "13"
				//	|| cmd_mat.Rows[0]["STOCK_PLACE_NO"].ToString().Trim() == "1103"))
				//{
				//	Log::Trace("", __FUNCTION__, "材料已到手工切割区垛位，删命令");
				//	bcls_rec_delete.Tables[0].Rows[0]["MAT_NO"] = matNo;
				//	doFlag = f_wmsmsm_cranecmd_delete(&bcls_rec_delete, bcls_ret, conn);
				//	if (doFlag != 0)
				//	{
				//		throw CApplicationException(-1, s.msg, log.Location);
				//	}
				//	continue;
				//}
			}

			if (cmd_mat.Rows[0]["STOCK_OPER_ORDER"].ToString().Trim().Substring(0, 1) == "3")
			{
				Log::Trace("", __FUNCTION__, "倒跺命令，删命令");
				bcls_rec_delete.Tables[0].Rows[0]["MAT_NO"] = matNo;
				doFlag = f_wmsmsm_cranecmd_delete(&bcls_rec_delete, bcls_ret, conn);
				if (doFlag != 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}

				//二切临时区域，垛位堆满以后生成出来的命令	
				if (cmd_mat.Rows[0]["LOGIC_STOCK_NO"].ToString().Trim() == "07")
				{
					//判断垛位是否堆满
					/*doFlag = f_wmsmsm_temarea_out(bcls_rec, bcls_ret, conn);
					if (doFlag != 0)
					{
					throw CApplicationException(-1, s.msg, log.Location);
					}*/
					continue;
				}
				continue;
			}
			//备料命令到达备料区的库位结束： 为了配合有人操作，临时加的一段代码
			if ((cmd_mat.Rows[0]["STOCK_OPER_ORDER"].ToString().Trim() == "2C" || cmd_mat.Rows[0]["STOCK_OPER_ORDER_FIN"].ToString().Trim() == "2C")
				&& (cmd_mat.Rows[0]["LOGIC_STOCK_NO"].ToString().Trim() == "08" || cmd_mat.Rows[0]["LOGIC_STOCK_NO"].ToString().Trim() == "12"))
			{
				Log::Trace("", __FUNCTION__, "材料已到备料垛位，删命令");
				bcls_rec_delete.Tables[0].Rows[0]["MAT_NO"] = matNo;
				doFlag = f_wmsmsm_cranecmd_delete(&bcls_rec_delete, bcls_ret, conn);
				if (doFlag != 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
				continue;

			}

			//过跨命令卸下到台车上时，更新台车任务批次
			if (cmd_mat.Rows[0]["STOCK_OPER_ORDER"].ToString().Trim() == "32")
			{
				/*if (cmd_mat.Rows[0]["STOCK_PLACE_TYPE"].ToString() == "D"
					&&cmd_mat.Rows[0]["DEV_DIV"].ToString() == "3")
				{
					sqlstr = "UPDATE TWM05 SET BATCH_TASK_NO=" + cmd_mat.Rows[0]["BATCH_NO"].ToString()
						+ " WHERE TR_NO='" + cmd_mat.Rows[0]["STOCK_PLACE_NO"].ToString().Trim().Substring(0, 4) + "'";
					Db::Execute(sqlstr);
				}*/
				Log::Trace("", __FUNCTION__, "4");
			}

			if (cmd_mat.Rows[0]["HALL_NO_FIN"].ToString().Trim() == cmd_mat.Rows[0]["HALL_NO"].ToString().Trim())
			{
				Log::Trace("", __FUNCTION__, "材料已到目标跨");
				if (cmd_mat.Rows[0]["STOCK_PLACE_TYPE"].ToString() == "D"&&
					cmd_mat.Rows[0]["DEV_DIV"].ToString() == "3")
				{
					//材料在过跨台车上，生成到目标垛位的命令
					bcls_rec_update.Tables[0].Rows[0]["MAT_NO"] = matNo;
					bcls_rec_update.Tables[0].Rows[0]["STOCK_NO"] = cmd_mat.Rows[0]["STOCK_NO"];
					bcls_rec_update.Tables[0].Rows[0]["STOCK_NO_FROM"] = cmd_mat.Rows[0]["STOCK_NO"];
					bcls_rec_update.Tables[0].Rows[0]["HALL_NO_FR"] = cmd_mat.Rows[0]["HALL_NO"];
					bcls_rec_update.Tables[0].Rows[0]["YARD_LAYER_FROM"] = cmd_mat.Rows[0]["LAYERNO"];
					bcls_rec_update.Tables[0].Rows[0]["STOCK_PLACE_NO_FROM"] = cmd_mat.Rows[0]["STOCK_PLACE_NO"];
					bcls_rec_update.Tables[0].Rows[0]["STOCK_OPER_ORDER_NEW"] = cmd_mat.Rows[0]["STOCK_OPER_ORDER_FIN"];
					bcls_rec_update.Tables[0].Rows[0]["CRANE_INST_STATUS"] = cmd_mat.Rows[0]["CRANE_INST_STATUS"].ToString();
					bcls_rec_update.Tables[0].Rows[0]["STOCK_NO_TO"] = cmd_mat.Rows[0]["STOCK_NO_FIN"].ToString();
					bcls_rec_update.Tables[0].Rows[0]["HALL_NO_TO"] = cmd_mat.Rows[0]["HALL_NO_FIN"].ToString();
					bcls_rec_update.Tables[0].Rows[0]["STOCK_PLACE_NO_TO_OLD"] = cmd_mat.Rows[0]["STOCK_PLACE_NO_TO"].ToString();
					bcls_rec_update.Tables[0].Rows[0]["STOCK_PLACE_NO_TO_NEW"] = cmd_mat.Rows[0]["STOCK_PLACE_NO_FIN"].ToString();
					bcls_rec_update.Tables[0].Rows[0]["STOCK_PLACE_NO_FIN"] = cmd_mat.Rows[0]["STOCK_PLACE_NO_FIN"].ToString();
					bcls_rec_update.Tables[0].Rows[0]["DEV_DIV"] = cmd_mat.Rows[0]["DEV_DIV"].ToString();
					bcls_rec_update.Tables[0].Rows[0]["LOGIC_STOCK_NO"] = cmd_mat.Rows[0]["LOGIC_STOCK_NO_TO"].ToString();
					if (bcls_rec->Tables["WM00_FOLLOW"].Columns.Contains("SEND_FLAG"))
						bcls_rec_update.Tables[0].Rows[0]["SEND_FLAG"] = bcls_rec->Tables["WM00_FOLLOW"].Rows[i]["SEND_FLAG"].ToString();

					if (bcls_rec_update.Tables[0].Rows[0]["STOCK_PLACE_NO_TO_NEW"].ToString().Trim() == "")
					{
						
							bcls_rec_auto_1.Tables[0].Rows[0]["MAT_NO"] = cmd_mat.Rows[0]["MAT_NO"].ToString();
							bcls_rec_auto_1.Tables[0].Rows[0]["STOCK_OPER_ORDER"] = cmd_mat.Rows[0]["STOCK_OPER_ORDER_FIN"];
							bcls_rec_auto_1.Tables[0].Rows[0]["HALL_NO"] = "H";
							doFlag = f_auto(&bcls_rec_auto_1, &bcls_ret_logic, conn);
							if (doFlag != 0)
							{
								throw CApplicationException(-1, s.msg, log.Location);
							}
							bcls_rec_update.Tables[0].Rows[0]["LOGIC_STOCK_NO"] = bcls_ret_logic.Tables[0].Rows[0]["LOGIC_STOCK_NO"].ToString();
							bcls_rec_update.Tables[0].Rows[0]["STOCK_PLACE_NO_TO_NEW"] = bcls_ret_logic.Tables[0].Rows[0]["STOCK_PLACE_NO"].ToString();

					}

					Log::Trace("", __FUNCTION__, "5");
					doFlag = f_wmsmsm_cranecmd_update(&bcls_rec_update, bcls_ret, conn);
					if (doFlag != 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				else
				{
					if (cmd_mat.Rows[0]["LOGIC_STOCK_NO"].ToString().Trim() != "08")
					{
						if (cmd_mat.Rows[0]["STOCK_OPER_ORDER"].ToString().Trim() == "30" 
							|| cmd_mat.Rows[0]["STOCK_PLACE_NO_FIN"].ToString().Trim() == ""
							|| cmd_mat.Rows[0]["STOCK_OPER_ORDER"].ToString().Trim() == "31"
							|| cmd_mat.Rows[0]["STOCK_OPER_ORDER"].ToString().Trim() == "32"
							|| cmd_mat.Rows[0]["STOCK_PLACE_NO"].ToString().Trim() == cmd_mat.Rows[0]["STOCK_PLACE_NO_FIN"].ToString().Trim())
						{
							//材料已到目标垛位，删命令
							Log::Trace("", __FUNCTION__, "材料已到目标垛位，删命令");
							bcls_rec_delete.Tables[0].Rows[0]["MAT_NO"] = matNo;
							doFlag = f_wmsmsm_cranecmd_delete(&bcls_rec_delete, bcls_ret, conn);
							if (doFlag != 0)
							{
								throw CApplicationException(-1, s.msg, log.Location);
							}
							//continue;
						}
						else
						{
							//材料未到目标垛位，生成到目标垛位的命令
							Log::Trace("", __FUNCTION__, "材料未到目标垛位，生成到目标垛位的命令");
							bcls_rec_update.Tables[0].Rows[0]["MAT_NO"] = matNo;
							bcls_rec_update.Tables[0].Rows[0]["STOCK_NO"] = cmd_mat.Rows[0]["STOCK_NO"];
							bcls_rec_update.Tables[0].Rows[0]["STOCK_NO_FROM"] = cmd_mat.Rows[0]["STOCK_NO"];
							bcls_rec_update.Tables[0].Rows[0]["HALL_NO_FR"] = cmd_mat.Rows[0]["HALL_NO"];
							bcls_rec_update.Tables[0].Rows[0]["YARD_LAYER_FROM"] = cmd_mat.Rows[0]["LAYERNO"];
							bcls_rec_update.Tables[0].Rows[0]["STOCK_PLACE_NO_FROM"] = cmd_mat.Rows[0]["STOCK_PLACE_NO"];
							bcls_rec_update.Tables[0].Rows[0]["STOCK_OPER_ORDER_NEW"] = cmd_mat.Rows[0]["STOCK_OPER_ORDER_FIN"];
							bcls_rec_update.Tables[0].Rows[0]["CRANE_INST_STATUS"] = cmd_mat.Rows[0]["CRANE_INST_STATUS"].ToString();
							bcls_rec_update.Tables[0].Rows[0]["STOCK_NO_TO"] = cmd_mat.Rows[0]["STOCK_NO_FIN"].ToString();
							bcls_rec_update.Tables[0].Rows[0]["HALL_NO_TO"] = cmd_mat.Rows[0]["HALL_NO_FIN"].ToString();
							bcls_rec_update.Tables[0].Rows[0]["STOCK_PLACE_NO_TO_OLD"] = cmd_mat.Rows[0]["STOCK_PLACE_NO_TO"].ToString();
							bcls_rec_update.Tables[0].Rows[0]["STOCK_PLACE_NO_TO_NEW"] = cmd_mat.Rows[0]["STOCK_PLACE_NO_FIN"].ToString();
							bcls_rec_update.Tables[0].Rows[0]["STOCK_PLACE_NO_FIN"] = cmd_mat.Rows[0]["STOCK_PLACE_NO_FIN"].ToString();
							bcls_rec_update.Tables[0].Rows[0]["DEV_DIV"] = cmd_mat.Rows[0]["DEV_DIV"].ToString();
							bcls_rec_update.Tables[0].Rows[0]["LOGIC_STOCK_NO"] = cmd_mat.Rows[0]["LOGIC_STOCK_NO_TO"].ToString();

							if (bcls_rec->Tables["WM00_FOLLOW"].Columns.Contains("SEND_FLAG"))
								bcls_rec_update.Tables[0].Rows[0]["SEND_FLAG"] = bcls_rec->Tables["WM00_FOLLOW"].Rows[i]["SEND_FLAG"].ToString();

							if (bcls_rec_update.Tables[0].Rows[0]["STOCK_PLACE_NO_TO_NEW"].ToString().Trim() == "")
							{
								
									bcls_rec_auto_1.Tables[0].Rows[0]["MAT_NO"] = cmd_mat.Rows[0]["MAT_NO"].ToString();
									bcls_rec_auto_1.Tables[0].Rows[0]["STOCK_OPER_ORDER"] = cmd_mat.Rows[0]["STOCK_OPER_ORDER_FIN"];
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
						}
					}
					else
					{
						//材料已到备料区，删除命令
						Log::Trace("", __FUNCTION__, "材料已到备料区，删除命令");
						bcls_rec_delete.Tables[0].Rows[0]["MAT_NO"] = matNo;
						doFlag = f_wmsmsm_cranecmd_delete(&bcls_rec_delete, bcls_ret, conn);
						if (doFlag != 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}
						//continue;
					}

				}


			}
			else
			{
				Log::Trace("", __FUNCTION__, "材料未到目标跨");
				if (cmd_mat.Rows[0]["STOCK_PLACE_TYPE"].ToString().Trim() == "D"
					&&cmd_mat.Rows[0]["DEV_DIV"].ToString().Trim() == "3")
				{
					//材料在过跨台车上
					sqlstr = "SELECT STOCK_PLACE_NO FROM TWM04 WHERE STOCK_PLACE_TYPE='D' AND DEV_DIV='3' AND HALL_NO='"
						+ cmd_mat.Rows[0]["HALL_NO_FIN"].ToString().Trim()
						+ "' AND COLUMN_NO='" + cmd_mat.Rows[0]["COLUMN_NO"].ToString().Trim() + "'"
						+ " AND TR_NO='" + cmd_mat.Rows[0]["TR_NO"].ToString().Trim() + "'";
					Log::Trace("", __FUNCTION__, "sqlstr={0}", sqlstr);
					bcls_rec_update.Tables[0].Rows[0]["MAT_NO"] = matNo;
					bcls_rec_update.Tables[0].Rows[0]["STOCK_NO"] = cmd_mat.Rows[0]["STOCK_NO"];
					bcls_rec_update.Tables[0].Rows[0]["STOCK_NO_FROM"] = cmd_mat.Rows[0]["STOCK_NO"];
					bcls_rec_update.Tables[0].Rows[0]["HALL_NO_FR"] = cmd_mat.Rows[0]["HALL_NO"];
					bcls_rec_update.Tables[0].Rows[0]["YARD_LAYER_FROM"] = cmd_mat.Rows[0]["LAYERNO"];
					bcls_rec_update.Tables[0].Rows[0]["STOCK_PLACE_NO_FROM"] = cmd_mat.Rows[0]["STOCK_PLACE_NO"];
					bcls_rec_update.Tables[0].Rows[0]["STOCK_OPER_ORDER_FIN"] = cmd_mat.Rows[0]["STOCK_OPER_ORDER_FIN"];
					bcls_rec_update.Tables[0].Rows[0]["STOCK_OPER_ORDER_NEW"] = "32";
					bcls_rec_update.Tables[0].Rows[0]["CRANE_INST_STATUS"] = cmd_mat.Rows[0]["CRANE_INST_STATUS"].ToString();
					bcls_rec_update.Tables[0].Rows[0]["STOCK_NO_TO"] = cmd_mat.Rows[0]["STOCK_NO_FIN"].ToString();
					bcls_rec_update.Tables[0].Rows[0]["HALL_NO_TO"] = cmd_mat.Rows[0]["HALL_NO_FIN"].ToString();
					bcls_rec_update.Tables[0].Rows[0]["STOCK_PLACE_NO_TO_OLD"] = cmd_mat.Rows[0]["STOCK_PLACE_NO_TO"].ToString();
					bcls_rec_update.Tables[0].Rows[0]["STOCK_PLACE_NO_TO_NEW"] = Db::QueryCString(sqlstr);
					bcls_rec_update.Tables[0].Rows[0]["STOCK_PLACE_NO_FIN"] = cmd_mat.Rows[0]["STOCK_PLACE_NO_FIN"].ToString();
					bcls_rec_update.Tables[0].Rows[0]["DEV_DIV"] = cmd_mat.Rows[0]["DEV_DIV"].ToString();
					if (bcls_rec->Tables["WM00_FOLLOW"].Columns.Contains("SEND_FLAG"))
						bcls_rec_update.Tables[0].Rows[0]["SEND_FLAG"] = bcls_rec->Tables["WM00_FOLLOW"].Rows[i]["SEND_FLAG"].ToString();

					//doFlag = f_wmsmsm_cranecmd_update(&bcls_rec_update, bcls_ret, conn);
					if (doFlag != 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				else
				{
					//材料未在过跨台车上
					bcls_rec_update.Tables[0].Rows[0]["MAT_NO"] = matNo;
					bcls_rec_update.Tables[0].Rows[0]["STOCK_NO"] = cmd_mat.Rows[0]["STOCK_NO"];
					bcls_rec_update.Tables[0].Rows[0]["STOCK_NO_FROM"] = cmd_mat.Rows[0]["STOCK_NO"];
					bcls_rec_update.Tables[0].Rows[0]["HALL_NO_FR"] = cmd_mat.Rows[0]["HALL_NO"];
					bcls_rec_update.Tables[0].Rows[0]["YARD_LAYER_FROM"] = cmd_mat.Rows[0]["LAYERNO"];
					bcls_rec_update.Tables[0].Rows[0]["STOCK_PLACE_NO_FROM"] = cmd_mat.Rows[0]["STOCK_PLACE_NO"];
					bcls_rec_update.Tables[0].Rows[0]["STOCK_OPER_ORDER_NEW"] = "32";
					bcls_rec_update.Tables[0].Rows[0]["CRANE_INST_STATUS"] = cmd_mat.Rows[0]["CRANE_INST_STATUS"].ToString();
					bcls_rec_update.Tables[0].Rows[0]["STOCK_NO_TO"] = cmd_mat.Rows[0]["STOCK_NO_FIN"].ToString();
					bcls_rec_update.Tables[0].Rows[0]["HALL_NO_TO"] = cmd_mat.Rows[0]["HALL_NO_FIN"].ToString();
					bcls_rec_update.Tables[0].Rows[0]["STOCK_PLACE_NO_TO_OLD"] = cmd_mat.Rows[0]["STOCK_PLACE_NO_TO"].ToString();
					bcls_rec_update.Tables[0].Rows[0]["STOCK_OPER_ORDER_FIN"] = cmd_mat.Rows[0]["STOCK_OPER_ORDER_FIN"].ToString();
					bcls_rec_update.Tables[0].Rows[0]["STOCK_PLACE_NO_FIN"] = cmd_mat.Rows[0]["STOCK_PLACE_NO_FIN"].ToString();
					bcls_rec_update.Tables[0].Rows[0]["DEV_DIV"] = cmd_mat.Rows[0]["DEV_DIV"].ToString();
					if (bcls_rec->Tables["WM00_FOLLOW"].Columns.Contains("SEND_FLAG"))
						bcls_rec_update.Tables[0].Rows[0]["SEND_FLAG"] = bcls_rec->Tables["WM00_FOLLOW"].Rows[i]["SEND_FLAG"].ToString();

					//doFlag = f_wmsmsm_cranecmd_update(&bcls_rec_update, bcls_ret, conn);
					if (doFlag != 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
			}
			//二切临时区域，垛位堆满以后生成出来的命令	
			if (cmd_mat.Rows[0]["LOGIC_STOCK_NO"].ToString().Trim() == "07")
			{
				/*doFlag = f_wmsmsm_temarea_out(bcls_rec, bcls_ret, conn);
				if (doFlag != 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}*/
			}
		}
	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "Database processing error. sqlcode=[{0}].", arguments, 1);
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
	Log::Trace("", __FUNCTION__, "函数f_wmsmsm_temarea_out正常结束[{0}]", doFlag);
	return doFlag;
}




//twma7.Reset();
//twma7["MAT_NO"] = matNo;
//
//sqlstr = "select a.mat_no,c.stock_place_type,c.logic_stock_no,c.dev_div,b.stock_oper_order as stock_oper_order_a0,d.stock_oper_order as stock_oper_order_a7,e.stock_place_no_fin,a.stock_no,d.stock_no_to "
//" from twm04 c, twma2 a"
//" left join twma0 b on a.mat_no = b.mat_no"
//" left join twma7 d on a.mat_no = d.mat_no"
//" left join twm000f e on a.mat_no = e.mat_no"
//" where c.stock_place_no = a.stock_place_no and a.mat_no = '" + matNo + "' order by a.layerno";
//Db::QueryTable(sqlstr, cmd_mat);
//if (cmd_mat.Rows.get_Count() == 0)
//{
//	Log::Trace("", __FUNCTION__, "材料A2表信息不存在,删除命令");
//	twma7.Delete();
//	continue;
//}
//else
//{
//	if (cmd_mat.Rows[0]["STOCK_PLACE_NO_FIN"].ToString().Trim())
//	{
//
//	}
//
//
//	if (cmd_mat.Rows[0]["STOCK_PLACE_NO_FIN"].ToString().Trim() != "")
//	{
//		Log::Trace("", __FUNCTION__, "材料存在上料队列");
//		if (cmd_mat.Rows[0]["LOGIC_STOCK_NO"].ToString().Trim() == "08" || cmd_mat.Rows[0]["LOGIC_STOCK_NO"].ToString().Trim() == "12")
//		{
//			Log::Trace("", __FUNCTION__, "材料已到备料区");
//			bcls_rec_delete.Tables[0].Rows[0]["MAT_NO"] = matNo;
//			doFlag = f_wmsmsm_cranecmd_delete(&bcls_rec_delete, bcls_ret, conn);
//			if (doFlag != 0)
//			{
//				throw CApplicationException(-1, s.msg, log.Location);
//			}
//			sqlstr = "DELETE FROM TWM000F WHERE MAT_NO='" + matNo + "'";
//			Db::Execute(sqlstr);
//			continue;
//		}
//		else if (cmd_mat.Rows[0]["DEV_DIV"].ToString().Trim() == "4"&&cmd_mat.Rows[0]["STOCK_OPER_ORDER_A7"].ToString() == "2B")
//		{
//			Log::Trace("", __FUNCTION__, "材料已上料");
//			bcls_rec_delete.Tables[0].Rows[0]["MAT_NO"] = matNo;
//			doFlag = f_wmsmsm_cranecmd_delete(&bcls_rec_delete, bcls_ret, conn);
//			if (doFlag != 0)
//			{
//				throw CApplicationException(-1, s.msg, log.Location);
//			}
//			continue;
//		}
//		else
//		{
//			Log::Trace("", __FUNCTION__, "材料未完成备料");
//			if (cmd_mat.Rows[0]["STOCK_PLACE_NO_FIN"].ToString().Trim() != "")
//			{
//				bcls_depiler.Tables[0].Rows[0]["STOCK_PLACE_NO_FIN"] = cmd_mat.Rows[0]["STOCK_PLACE_NO_FIN"].ToString();
//				doFlag = f_wmsmsm_cranecmd_depiler_do(&bcls_depiler, bcls_ret, conn);
//				if (doFlag < 0)
//				{
//					throw CApplicationException(-1, s.msg, log.Location);
//				}
//			}
//			continue;
//		}
//	}
//	else if (cmd_mat.Rows[0]["STOCK_OPER_ORDER_A0"].ToString().Trim() == "2E")
//	{
//		Log::Trace("", __FUNCTION__, "材料存在发货队列");
//		if (cmd_mat.Rows[0]["DEV_DIV"].ToString() == "1" || cmd_mat.Rows[0]["DEV_DIV"].ToString() == "2")
//		{
//			Log::Trace("", __FUNCTION__, "材料已在火车或卡车上，删命令");
//			bcls_rec_delete.Tables[0].Rows[0]["MAT_NO"] = matNo;
//			doFlag = f_wmsmsm_cranecmd_delete(&bcls_rec_delete, bcls_ret, conn);
//			if (doFlag != 0)
//			{
//				throw CApplicationException(-1, s.msg, log.Location);
//			}
//		}
//		else
//		{
//			Log::Trace("", __FUNCTION__, "材料不在火车或卡车上");
//			bcls_rec_shipping.Tables[0].Rows[0]["MAT_NO"] = matNo;
//			doFlag = f_wmsmsm_cranecmd_shipping_do(&bcls_rec_shipping, bcls_ret, conn);
//			if (doFlag != 0)
//			{
//				throw CApplicationException(-1, s.msg, log.Location);
//			}
//		}
//	}
//	else if (cmd_mat.Rows[0]["STOCK_OPER_ORDER_A0"].ToString().Substring(0.1) == "1")
//	{
//		Log::Trace("", __FUNCTION__, "材料存在入库队列");
//		if (cmd_mat.Rows[0]["DEV_DIV"].ToString() == "1" || cmd_mat.Rows[0]["DEV_DIV"].ToString() == "2")
//		{
//			Log::Trace("", __FUNCTION__, "材料在火车或卡车上");
//			bcls_rec_make.Tables[0].Rows.Add();
//			bcls_rec_make.Tables[0].Rows[0]["MAT_NO"] = matNo;
//			bcls_rec_make.Tables[0].Rows[0]["STOCK_OPER_ORDER"] = cmd_mat.Rows[0]["STOCK_OPER_ORDER_A0"].ToString();
//			doFlag = f_wmsmsm_cranecmd_make(&bcls_rec_make, bcls_ret, conn);
//			if (doFlag != 0)
//			{
//				throw CApplicationException(-1, s.msg, log.Location);
//			}
//		}
//		else
//		{
//			Log::Trace("", __FUNCTION__, "材料已到库内");
//			bcls_rec_delete.Tables[0].Rows[0]["MAT_NO"] = matNo;
//			doFlag = f_wmsmsm_cranecmd_delete(&bcls_rec_delete, bcls_ret, conn);
//			if (doFlag != 0)
//			{
//				throw CApplicationException(-1, s.msg, log.Location);
//			}
//			continue;
//		}
//
//	}
//	else if (cmd_mat.Rows[0]["STOCK_OPER_ORDER_A0"].ToString().Trim() == "")
//	{
//		Log::Trace("", __FUNCTION__, "材料不存在队列");
//		if (cmd_mat.Rows[0]["STOCK_OPER_ORDER_A7"].ToString().Trim() == "32")
//		{
//			Log::Trace("", __FUNCTION__, "过跨命令");
//			if (cmd_mat.Rows[0]["STOCK_NO"].ToString() == cmd_mat.Rows[0]["STOCK_NO_TO"].ToString())
//			{
//				Log::Trace("", __FUNCTION__, "已到目的跨");
//				//是否在过跨台车上
//				if (cmd_mat.Rows[0]["DEV_DIV"].ToString() == "3")
//				{
//					//生成到目标库区的命令
//					bcls_rec_make.Tables[0].Rows.Add();
//					bcls_rec_make.Tables[0].Rows[0]["MAT_NO"] = matNo;
//					bcls_rec_make.Tables[0].Rows[0]["STOCK_OPER_ORDER"] = cmd_mat.Rows[0]["STOCK_OPER_ORDER_A7"].ToString();
//					doFlag = f_wmsmsm_cranecmd_make(&bcls_rec_make, bcls_ret, conn);
//					if (doFlag != 0)
//					{
//						throw CApplicationException(-1, s.msg, log.Location);
//					}
//				}
//				else
//				{
//					bcls_rec_delete.Tables[0].Rows[0]["MAT_NO"] = matNo;
//					doFlag = f_wmsmsm_cranecmd_delete(&bcls_rec_delete, bcls_ret, conn);
//					if (doFlag != 0)
//					{
//						throw CApplicationException(-1, s.msg, log.Location);
//					}
//					continue;
//				}
//			}
//			else
//			{
//				Log::Trace("", __FUNCTION__, "未到目的跨");
//				//生成到过跨台车的命令
//
//
//
//			}
//			continue;
//		}
//		else if (cmd_mat.Rows[0]["STOCK_OPER_ORDER_A7"].ToString().Trim() = "")
//		{
//			Log::Trace("", __FUNCTION__, "不是过跨命令，删除");
//			bcls_rec_delete.Tables[0].Rows[0]["MAT_NO"] = matNo;
//			doFlag = f_wmsmsm_cranecmd_delete(&bcls_rec_delete, bcls_ret, conn);
//			if (doFlag != 0)
//			{
//				throw CApplicationException(-1, s.msg, log.Location);
//			}
//			continue;
//		}
//		else
//		{
//			Log::Trace("", __FUNCTION__, "材料不存在队列且不存在命令");
//			continue;
//		}
//
//	}
//	else
//	{
//		strcpy(s.msg, "材料队列不存在");
//		throw CApplicationException(-1, s.msg, log.Location);
//	}
//}