/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:      张凌辉
Version:     1.1.1
Date:        2016-09-26 10:35:08
Description: 吊车命令做成函数
**************************************************/

#include "WM_Utility.h"
#include "h_wms0_pub.h"

BM2_FUNCTION_IMPORT
//int f_wmsm_upperMat_autoCmd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);		//上层物料自动倒垛命令生成函数

BM2_FUNCTION_EXPORT
int f_wmsm_craneCmdMake(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	//程序内部变量
	int doFlag = 0;
	CString sqlstr = " ";
	CDecimal v_count = 0;

	//应用变量
	int cmdSeq = 0;
	CString matNo = " ";
	CString dateTime = " ";
	CString sqlWhere = " ";
	CString stockOperOrder = " ";
	CString v_table_name = "";

	CDecimal cmdGrpNo = 0;

	//定义行车命令数据表
	CDataTable dtCraneCmd;

	//定义材料数据表
	CDataTable dtMat;

	//数据库操作类定义
	CDbCommand cmd_inq(conn);

	//定义表实体对象

	try
	{
		//判断是否存在指定块
		if (bcls_rec->Tables.IndexOf("WM00_CMD") < 0 ||
			bcls_rec->Tables["WM00_CMD"].Rows.get_Count() == 0)
		{
			WM_Utility::PrintLog("Incoming data block WM00_CMD is not exist.");
			return doFlag;
		}

		//设置dtCraneCmd列名
		WM_Utility::SetDataTableColName("TWMA7", dtCraneCmd, conn);

		//取系统时间
		dateTime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		//循环获取传入行车命令块数据
		for (int i = 0; i < bcls_rec->Tables["WM00_CMD"].Rows.get_Count(); i++)
		{

			matNo = bcls_rec->Tables["WM00_CMD"].Rows[i]["MAT_NO"];
			WM_Utility::PrintLog("matNo", matNo);


			sqlstr = " SELECT COUNT(1) FROM TMMSM01"
				" WHERE MAT_NO = @mat_no";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("mat_no", matNo);
			v_count = cmd_inq.ExecuteScalar();
			if (v_count == 1)
			{
				v_table_name = "TMMSM01";
			}
			else
			{
				sqlstr = " SELECT COUNT(1) FROM TMMHR01"
					" WHERE MAT_NO = @mat_no";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("mat_no", matNo);
				v_count = cmd_inq.ExecuteScalar();
				if (v_count == 1)
				{
					v_table_name = "TMMHR01";
				}
				else
				{
					sprintf(s.msg, "板坯/热卷主档找不到材料号.");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}
			

			if (matNo.Trim() == "")
			{
				strcpy(s.msg, "No material no.");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			stockOperOrder = bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_OPER_ORDER"];
			WM_Utility::PrintLog("stockOperOrder", stockOperOrder);

			if (stockOperOrder.Trim() == "")
			{
				strcpy(s.msg, "No movement type.");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			sqlWhere = "mat_no = '" + matNo + "'";
			if (WM_Utility::QueryDataCount("TWMA7", sqlWhere, conn) > 0)
			{
				Log::Trace("", __FUNCTION__, "材料号[{0}]已存在行车命令，更新最终命令数据", matNo);

				CDataTable dtUpdItem;
				dtUpdItem.Columns.Add(DT_STRING, "STOCK_NO_FIN");
				dtUpdItem.Columns.Add(DT_STRING, "HALL_NO_FIN");
				dtUpdItem.Columns.Add(DT_STRING, "STOCK_PLACE_NO_FIN");
				dtUpdItem.Columns.Add(DT_STRING, "STOCK_OPER_ORDER_FIN");
				dtUpdItem.Rows.Add();
				dtUpdItem.Rows[0]["STOCK_NO_FIN"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_NO_TO"];
				dtUpdItem.Rows[0]["HALL_NO_FIN"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_NO_TO"];
				dtUpdItem.Rows[0]["STOCK_PLACE_NO_FIN"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_PLACE_NO_TO"];
				dtUpdItem.Rows[0]["STOCK_OPER_ORDER_FIN"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_OPER_ORDER"];

				if (!WM_Utility::TableDataUpdate("TWMA7", dtUpdItem.Rows[0], sqlWhere, conn))
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				continue;
			}

			//新增行车命令数据表行
			dtCraneCmd.Rows.Add();
			dtCraneCmd.Rows[cmdSeq]["REC_CREATOR"] = s.userid;
			dtCraneCmd.Rows[cmdSeq]["REC_CREATE_TIME"] = dateTime;
			dtCraneCmd.Rows[cmdSeq]["MAT_NO"] = matNo;
			dtCraneCmd.Rows[cmdSeq]["STOCK_OPER_ORDER"] = stockOperOrder;
			dtCraneCmd.Rows[cmdSeq]["CRANE_INST_STATUS"] = "0";
			dtCraneCmd.Rows[cmdSeq]["CRANE_INST_CODE"] = stockOperOrder.Substring(0, 1);

			if (bcls_rec->Tables["WM00_CMD"].Columns.Contains("VEHICLE_NO"))
			{
				dtCraneCmd.Rows[cmdSeq]["VEHICLE_NO"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["VEHICLE_NO"].ToString().Trim();
			}
			else
			{
				dtCraneCmd.Rows[cmdSeq]["VEHICLE_NO"] = " ";
			}

			if (dtCraneCmd.Rows[cmdSeq]["CRANE_INST_CODE"].ToString() == "1")
			{
				WM_Utility::PrintLog("入库命令");

				sqlstr = "SELECT mat_no,mat_kind,mat_shape_flag,mat_act_thick,mat_act_width,mat_act_len,mat_act_wt"
					" FROM " + v_table_name + " WHERE mat_no = '" + matNo + "'";

				if (!WM_Utility::QueryData(sqlstr, dtMat, conn))
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				if (dtMat.Rows.get_Count() == 0)
				{
					sprintf(s.msg, "Material [%s] is not exist.", (const char*)matNo);
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				if (!bcls_rec->Tables["WM00_CMD"].Columns.Contains("STOCK_NO_TO") ||
					bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_NO_TO"].ToString().Trim() == "")
				{
					sprintf(s.msg, "No target yard for material [%s].", (const char*)matNo);
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				WM_Utility::PrintLog("写材料源库位信息");
				dtCraneCmd.Rows[cmdSeq]["STOCK_NO"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_NO_TO"];
				dtCraneCmd.Rows[cmdSeq]["STOCK_NO_FROM"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_NO_FROM"];

				if (bcls_rec->Tables["WM00_CMD"].Columns.Contains("HALL_NO_FROM"))
				{
					dtCraneCmd.Rows[cmdSeq]["HALL_NO_FR"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["HALL_NO_FROM"];
				}
				else
				{
					dtCraneCmd.Rows[cmdSeq]["HALL_NO_FR"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_NO_FROM"];
				}

				dtCraneCmd.Rows[cmdSeq]["STOCK_PLACE_NO_FROM"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_PLACE_NO_FROM"];
				dtCraneCmd.Rows[cmdSeq]["YARD_LAYER_FROM"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["LAYERNO_FROM"];
			}
			else
			{
				WM_Utility::PrintLog("非入库命令");

				sqlstr = "SELECT a.mat_no,a.stock_no,a.hall_no,a.stock_place_no,a.layerno,b.mat_kind,"
					"b.mat_shape_flag,b.mat_act_thick,b.mat_act_width,b.mat_act_len,b.mat_act_wt "
					" FROM twma2 a," + v_table_name + " b WHERE a.mat_no = '" + matNo + "' AND a.mat_no = b.mat_no";

				if (!WM_Utility::QueryData(sqlstr, dtMat, conn))
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				if (dtMat.Rows.get_Count() == 0)
				{
					sprintf(s.msg, "Material [%s] is not in yard.", (const char*)matNo);
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				//写材料源库位信息
				dtCraneCmd.Rows[cmdSeq]["STOCK_NO"] = dtMat.Rows[0]["STOCK_NO"];
				dtCraneCmd.Rows[cmdSeq]["STOCK_NO_FROM"] = dtMat.Rows[0]["STOCK_NO"];
				dtCraneCmd.Rows[cmdSeq]["HALL_NO_FR"] = dtMat.Rows[0]["HALL_NO"];
				dtCraneCmd.Rows[cmdSeq]["STOCK_PLACE_NO_FROM"] = dtMat.Rows[0]["STOCK_PLACE_NO"];
				dtCraneCmd.Rows[cmdSeq]["YARD_LAYER_FROM"] = dtMat.Rows[0]["LAYERNO"];

				WM_Utility::AddColValue(bcls_rec->Tables["WM00_CMD"], i, "STOCK_PLACE_NO_FROM", dtMat.Rows[0]["STOCK_PLACE_NO"].ToString());
				WM_Utility::AddColValue(bcls_rec->Tables["WM00_CMD"], i, "LAYERNO_FROM", dtMat.Rows[0]["LAYERNO"].ToDecimal());
			}

			WM_Utility::PrintLog("写材料信息");
			dtCraneCmd.Rows[cmdSeq]["MAT_KIND"] = dtMat.Rows[0]["MAT_KIND"];
			dtCraneCmd.Rows[cmdSeq]["MAT_SHAPE_FLAG"] = dtMat.Rows[0]["MAT_SHAPE_FLAG"];
			dtCraneCmd.Rows[cmdSeq]["MAT_ACT_THICK"] = dtMat.Rows[0]["MAT_ACT_THICK"];
			dtCraneCmd.Rows[cmdSeq]["MAT_ACT_WIDTH"] = dtMat.Rows[0]["MAT_ACT_WIDTH"];
			dtCraneCmd.Rows[cmdSeq]["MAT_ACT_LEN"] = dtMat.Rows[0]["MAT_ACT_LEN"];
			dtCraneCmd.Rows[cmdSeq]["MAT_ACT_WT"] = dtMat.Rows[0]["MAT_ACT_WT"];

			if (bcls_rec->Tables["WM00_CMD"].Columns.Contains("UNIT_CODE"))
			{
				dtCraneCmd.Rows[cmdSeq]["UNIT_CODE"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["UNIT_CODE"];
			}

			if (bcls_rec->Tables["WM00_CMD"].Columns.Contains("MOVE_TYPE") &&
				bcls_rec->Tables["WM00_CMD"].Rows[i]["MOVE_TYPE"].ToString().Trim() != "")
			{
				dtCraneCmd.Rows[cmdSeq]["MOVE_TYPE"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["MOVE_TYPE"];
			}
			else
			{
				dtCraneCmd.Rows[cmdSeq]["MOVE_TYPE"] = stockOperOrder;
			}

			//写目标库位信息
			if (!bcls_rec->Tables["WM00_CMD"].Columns.Contains("STOCK_NO_TO") ||
				bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_NO_TO"].ToString().Trim() == "")
			{
				dtCraneCmd.Rows[cmdSeq]["STOCK_NO_TO"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_NO_FROM"];
			}
			else
			{
				dtCraneCmd.Rows[cmdSeq]["STOCK_NO_TO"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_NO_TO"];
			}

			if (!bcls_rec->Tables["WM00_CMD"].Columns.Contains("HALL_NO_TO") ||
				bcls_rec->Tables["WM00_CMD"].Rows[i]["HALL_NO_TO"].ToString().Trim() == "")
			{
				dtCraneCmd.Rows[cmdSeq]["HALL_NO_TO"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_NO_TO"];
			}
			else
			{
				dtCraneCmd.Rows[cmdSeq]["HALL_NO_TO"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["HALL_NO_TO"];
			}

			//入库命令或者倒垛命令,传入目的库位为空,调用库位推荐函数
			if ((dtCraneCmd.Rows[cmdSeq]["CRANE_INST_CODE"].ToString() == "1" ||
				dtCraneCmd.Rows[cmdSeq]["CRANE_INST_CODE"].ToString() == "3") &&
				bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_PLACE_NO_TO"].ToString().Trim() == "")
			{
				WM_Utility::PrintLog("入库命令或者倒垛命令,传入目的库位为空,调用库位推荐函数");

				// 初始化 库位推荐用 输入/输出块
				EIClass in, out;
				//f_wms_auto_init(&in, &out, conn);

				// 设置库位推荐输入信息
				// 设置输入参数块				

				CDataTable& blkParams = in.Tables[WMS_BLK_IN_PARAMS];

				// 入出库区分(I：入库、O：出库、M：倒垛)
				if (dtCraneCmd.Rows[cmdSeq]["CRANE_INST_CODE"].ToString() == "1" ||
					dtCraneCmd.Rows[cmdSeq]["STOCK_OPER_ORDER"].ToString() == "32")
				{
					blkParams.Rows[0][WMS_COL_JOB_IO_DIV] = "I";
				}
				else
				{
					blkParams.Rows[0][WMS_COL_JOB_IO_DIV] = "M";
				}

				blkParams.Rows[0][WMS_COL_JOB_ADJUST_FLAG] = "N";       // 库区作业顺序是否可调(N:不可调、Y:可调整)

				// 作业库区号(入库时：入库目标库区、 倒垛时：倒垛库区、 出库时：出库起始库)

				blkParams.Rows[0][WMS_COL_JOB_STOCK_NO] = bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_NO_FROM"];

				// 材料形状区分(P:板类、C:卷类)
				if (dtMat.Rows[0]["MAT_SHAPE_FLAG"].ToString() == "3")
				{
					blkParams.Rows[0][WMS_COL_JOB_MAT_SHAPE_DIV] = "C";
				}
				else
				{
					blkParams.Rows[0][WMS_COL_JOB_MAT_SHAPE_DIV] = "P";
				}
				Log::Trace("", __FUNCTION__, "333333333");
				// 设置材料参数块（可设置多块材料）
				int currRow = -1;
				CDataTable& blkMats = in.Tables[WMS_BLK_IN_MATS];

				++currRow;
				blkMats.Rows.Add();
				blkMats.Rows[currRow][WMS_COL_MAT_GRP_NO] = matNo;					// 材料组号（1个材料组可含有一个或多个材料。当含有多个材料时，这些材料将作为一个整体叠放在同一个垛位上【适用于钢板】。当只有一块材料时，通常 材料组号=材料号。）
				blkMats.Rows[currRow][WMS_COL_MAT_NO] = matNo;						// 材料号
				blkMats.Rows[currRow][WMS_COL_MAT_KIND] = " ";                      // 材料种类(HP:中厚板、CR：冷轧、HR：热轧 等等)【印度项目不用，传空格】
				blkMats.Rows[currRow][WMS_COL_JOB_LARGE_DIV] = stockOperOrder;      // 库区作业大分类：库业务类型(代码WM10)
				blkMats.Rows[currRow][WMS_COL_JOB_MIDDLE_DIV] = " ";                // 库区作业中分类：【印度项目不用，传空格】
				Log::Trace("", __FUNCTION__, "44444444");
				// 材料入库所用设备垛位。【倒垛、出库时，无需设置（传空格）】
				if (blkParams.Rows[0][WMS_COL_JOB_IO_DIV].ToString() == "I")
				{
					blkMats.Rows[currRow][WMS_COL_FROM_STOCK_DEV_NO] = dtCraneCmd.Rows[cmdSeq]["STOCK_PLACE_NO_FROM"];
				}
				else
				{
					blkMats.Rows[currRow][WMS_COL_FROM_STOCK_DEV_NO] = " ";
				}

				Log::Trace("", __FUNCTION__, "---------------打印库位推荐传入块数据----------------");
				Log::Trace("", __FUNCTION__, "WMS_BLK_IN_PARAMS：");
				Log::Trace("", __FUNCTION__, "WMS_COL_JOB_IO_DIV={0}", blkParams.Rows[0][WMS_COL_JOB_IO_DIV].ToString());
				Log::Trace("", __FUNCTION__, "WMS_COL_JOB_ADJUST_FLAG={0}", blkParams.Rows[0][WMS_COL_JOB_ADJUST_FLAG].ToString());
				Log::Trace("", __FUNCTION__, "WMS_COL_JOB_STOCK_NO={0}", blkParams.Rows[0][WMS_COL_JOB_STOCK_NO].ToString());
				Log::Trace("", __FUNCTION__, "WMS_COL_JOB_MAT_SHAPE_DIV={0}", blkParams.Rows[0][WMS_COL_JOB_MAT_SHAPE_DIV].ToString());

				Log::Trace("", __FUNCTION__, "WMS_BLK_IN_MATS：");
				Log::Trace("", __FUNCTION__, "WMS_COL_MAT_GRP_NO={0}", blkMats.Rows[currRow][WMS_COL_MAT_GRP_NO].ToString());
				Log::Trace("", __FUNCTION__, "WMS_COL_MAT_NO={0}", blkMats.Rows[currRow][WMS_COL_MAT_NO].ToString());
				Log::Trace("", __FUNCTION__, "WMS_COL_MAT_KIND={0}", blkMats.Rows[currRow][WMS_COL_MAT_KIND].ToString());
				Log::Trace("", __FUNCTION__, "WMS_COL_JOB_LARGE_DIV={0}", blkMats.Rows[currRow][WMS_COL_JOB_LARGE_DIV].ToString());
				Log::Trace("", __FUNCTION__, "WMS_COL_JOB_MIDDLE_DIV={0}", blkMats.Rows[currRow][WMS_COL_JOB_MIDDLE_DIV].ToString());
				Log::Trace("", __FUNCTION__, "WMS_COL_FROM_STOCK_DEV_NO={0}", blkMats.Rows[currRow][WMS_COL_FROM_STOCK_DEV_NO].ToString());

				//执行库位推荐
			//	doFlag = f_wms_auto(&in, &out, conn);
				if (doFlag == 0)
				{
					WM_Utility::PrintLog("库位推荐成功");
					WM_Utility::PrintLog("打印推荐结果目标垛位块");
					WM_Utility::PrintDataTable(out.Tables[WMS_BLK_OUT_TARGETS]);
					WM_Utility::PrintLog("推荐结果材料移动块");
					WM_Utility::PrintDataTable(out.Tables[WMS_BLK_OUT_MOVES]);

					bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_PLACE_NO_TO"] = out.Tables[WMS_BLK_OUT_TARGETS].Rows[0]["TO_STOCK_PLACE_NO"];
				}
				else
				{
					WM_Utility::PrintLog("库位推荐失败");
				}
			}

			dtCraneCmd.Rows[cmdSeq]["STOCK_PLACE_NO_TO"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_PLACE_NO_TO"];

			if (bcls_rec->Tables["WM00_CMD"].Columns.Contains("LAYERNO_TO") &&
				bcls_rec->Tables["WM00_CMD"].Rows[i]["LAYERNO_TO"].ToString().Trim() != "")
			{
				dtCraneCmd.Rows[cmdSeq]["YARD_LAYER_TO"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["LAYERNO_TO"];
			}

			dtCraneCmd.Rows[cmdSeq]["CMD_METHOD"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["CMD_METHOD"];

			//行车号分配
			//dtCraneCmd.Rows[cmdSeq]["CRANE_NO"]

			//写台车号信息
			if (stockOperOrder == "32")
			{
				CString trStockPlaceNo = "";

				sqlstr = "SELECT b.stock_place_no FROM twm09 a,twm04 b WHERE stock_no_from = '" +
					dtCraneCmd.Rows[cmdSeq]["STOCK_NO_FROM"].ToString() + "' AND tr_from_span = '" +
					dtCraneCmd.Rows[cmdSeq]["HALL_NO_FR"].ToString() + "' AND stock_no_to = '" +
					dtCraneCmd.Rows[cmdSeq]["STOCK_NO_TO"].ToString() + "' AND tr_to_span = '" +
					dtCraneCmd.Rows[cmdSeq]["HALL_NO_TO"].ToString() + "' AND a.tr_no = b.tr_no"
					" AND a.stock_no_from = b.stock_no AND a.tr_from_span = b.hall_no";

				Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				while (cmd_inq.Read())
				{
					trStockPlaceNo = cmd_inq.GetString(1);
				}
				cmd_inq.Close();

				if (trStockPlaceNo.Trim() == "")
				{
					strcpy(s.msg, "Cannot find STC's position.");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				dtCraneCmd.Rows[cmdSeq]["STOCK_NO_FIN"] = dtCraneCmd.Rows[cmdSeq]["STOCK_NO_TO"];
				dtCraneCmd.Rows[cmdSeq]["STOCK_NO_TO"] = dtCraneCmd.Rows[cmdSeq]["STOCK_NO_FROM"];
				dtCraneCmd.Rows[cmdSeq]["HALL_NO_FIN"] = dtCraneCmd.Rows[cmdSeq]["HALL_NO_TO"];
				dtCraneCmd.Rows[cmdSeq]["HALL_NO_TO"] = dtCraneCmd.Rows[cmdSeq]["HALL_NO_FR"];
				dtCraneCmd.Rows[cmdSeq]["STOCK_PLACE_NO_FIN"] = dtCraneCmd.Rows[cmdSeq]["STOCK_PLACE_NO_TO"];
				dtCraneCmd.Rows[cmdSeq]["STOCK_PLACE_NO_TO"] = trStockPlaceNo;
			}
			if (!bcls_rec->Tables["WM00_CMD"].Columns.Contains("UPPER_FLAG"))
			{
				//非f_wm00_upperMat_autoCmd本身调用的时候,命令的主材料号标识为1
				dtCraneCmd.Rows[cmdSeq]["MAIN_MAT_NO"] = "1";
			}
			else
			{
				dtCraneCmd.Rows[cmdSeq]["MAIN_MAT_NO"] = " ";
			}

			cmdSeq++;
		}

		if (!bcls_rec->Tables["WM00_CMD"].Columns.Contains("UPPER_FLAG"))
		{
			//非f_wm00_upperMat_autoCmd本身调用的时候,生成上层物料自动倒垛命令
			//因新框架不能循环调用，暂时注释，以后再修改
			//doFlag = f_wmsm_upperMat_autoCmd(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}

		//打印dtCraneCmd内容
		//WM_Utility::PrintDataTable(dtCraneCmd);

		//删除目的库位和当前库位相同的命令,生成行车命令序号:Oracel数据库取Sequences，其他的使用框架的EPGetNextSeq
		for (int i = 0; i < dtCraneCmd.Rows.get_Count(); i++)
		{
			if (dtCraneCmd.Rows[i]["STOCK_PLACE_NO_TO"].ToString() == dtCraneCmd.Rows[i]["STOCK_PLACE_NO_FROM"].ToString())
			{
				Log::Trace("", __FUNCTION__, "材料{0}目的库位和当前库位相同，不产生命令", dtCraneCmd.Rows[i]["MAT_NO"].ToString());
				dtCraneCmd.Rows[0].Delete();
			}
			else
			{
				dtCraneCmd.Rows[i]["CMD_SEQ"] = WM_Utility::GetSeqence("WM_CMD_SEQ", conn);

				//写行车命令组号		
				if (dtCraneCmd.Rows[i]["CMD_METHOD"].ToString() == "2")
				{
					WM_Utility::PrintLog("一吊多块，取首个材料的序号为组号");

					if (cmdGrpNo == 0)
					{
						cmdGrpNo = dtCraneCmd.Rows[i]["CMD_SEQ"];
						WM_Utility::PrintLog("cmdGrpNo", cmdGrpNo);
					}

					dtCraneCmd.Rows[i]["CRANE_CMDGRPNO"] = cmdGrpNo;
				}
				else
				{
					cmdGrpNo = 0;
				}
			}
		}
		//dtCraneCmd插入数据库
		if (!WM_Utility::TableDataInsert("TWMA7", dtCraneCmd, conn))
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "Database processing error. sqlcode=[{0}]." /* _RES("GCRSS0000006")*//*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		Log::Trace("", __FUNCTION__, "数据库SQL出错信息	= [{0}]", str);
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch (const CApplicationException& ex)
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

	return(doFlag);
}

