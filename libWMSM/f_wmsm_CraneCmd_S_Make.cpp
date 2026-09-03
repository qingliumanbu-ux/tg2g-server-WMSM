/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:      jinquan
Version:     1.1.1
Date:        2016-12-05 10:35:08
Description: 板坯吊车命令做成函数
**************************************************/

#include "WM_Utility.h"
//#include "twma7.h"
//#include "twma2.h"
#include "h_wms0_pub.h"

BM2_FUNCTION_IMPORT
int f_wm00_pileinfocal(CString stock_no, CString stock_place_no, EIClass * bcls_ret, CDbConnection * conn);  //垛位最大高度、重量修正
int f_wmsm_craneCmd_seq_update(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);                //更新命令流水号

BM2_FUNCTION_EXPORT
int f_wmsm_CraneCmd_S_Make(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/*程序内部变量*/
	int doFlag = 0;
	CString sqlstr = " ";
	CDecimal v_count = 0;
	CString v_table_name = "";

	/*数据库对象*/
	//CTWMA7 twma7(conn);
	//CTWMA7 twma7_tem(conn);
	//CTWMA2 twma2(conn);
	CModel twma7 = CModel("TWMA7");
	CModel twma7_tem = CModel("TWMA7");
	CModel twma2 = CModel("TWMA2");

	/*业务变量*/
	int cmdSeq = 0;
	int maxseqno = 999999999;
	CString matNo = " ";
	CString dateTime = " ";
	CString sqlWhere = " ";
	CString stockOperOrder = " ";
	CString crane_inst_code = " ";

	/*数据块*/
	CDataTable dtCraneCmd;  //定义行车命令数据表
	CDataTable dtmat;       //存放材料
	CDataTable dtmat2;       //存放材料
	CDataTable dtmat1;      //存放材料
	CDataTable up_dtMat;    //存放上层材料
	CDataTable dtStockNo;   //存放库区
	CDataTable update_stock;

	/*数据库操作类定义*/
	CDbCommand cmd_inq_make(conn);

	try
	{
		//判断是否存在指定块
		if (bcls_rec->Tables.IndexOf("WM00_CMD") < 0 ||
			bcls_rec->Tables["WM00_CMD"].Rows.get_Count() == 0)
		{
			Log::Trace("", __FUNCTION__, "没有传入行车命令数据");
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
			Log::Trace("", __FUNCTION__, "传入材料号：【{0}】", matNo);
			if (matNo.Trim() == "")
			{
				strcpy(s.msg, "Mat No. is empty.");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			sqlstr = " SELECT COUNT(1) FROM TMMSM01"
				" WHERE MAT_NO = @mat_no";
			cmd_inq_make.SetCommandText(sqlstr);
			cmd_inq_make.Parameters.Set("mat_no", matNo);
			v_count = cmd_inq_make.ExecuteScalar();
			if (v_count == 1)
			{
				v_table_name = "TMMSM01";
			}
			else
			{
				sqlstr = " SELECT COUNT(1) FROM TMMHR01"
					" WHERE MAT_NO = @mat_no";
				cmd_inq_make.SetCommandText(sqlstr);
				cmd_inq_make.Parameters.Set("mat_no", matNo);
				v_count = cmd_inq_make.ExecuteScalar();
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

			stockOperOrder = bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_OPER_ORDER"];
			Log::Trace("", __FUNCTION__, "传入库业务类型：[{0}]", stockOperOrder);
			if (stockOperOrder.Trim() == "")
			{
				strcpy(s.msg, "Movement type is empty.");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			crane_inst_code = stockOperOrder.Substring(0, 1);
			if (crane_inst_code == "1")
			{
				dtmat.Rows.Clear();
				Log::Trace("", __FUNCTION__, "材料[{0}]入库命令", matNo);

				sqlstr = "SELECT mat_no,mat_kind,mat_shape_flag,mat_act_thick,mat_act_width,mat_act_len,mat_act_wt"
					" FROM " + v_table_name + " WHERE mat_no = '" + matNo + "'";
				Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
				cmd_inq_make.SetCommandText(sqlstr);
				cmd_inq_make.ExecuteQuery(dtmat);
				cmd_inq_make.Close();

				if (dtmat.Rows.get_Count() == 0)
				{
					sprintf(s.msg, "Mat No.[%s] is not exist.", (const char*)matNo);
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				if (!bcls_rec->Tables["WM00_CMD"].Columns.Contains("STOCK_NO_TO") ||
					bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_NO_TO"].ToString().Trim() == "")
				{
					sprintf(s.msg, "There's no position to material [%s] for yard inputting.", (const char*)matNo);
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				twma7.Reset();
				twma7["REC_CREATE_TIME"] = dateTime;
				twma7["REC_CREATOR"] = s.userid;
				twma7["MAT_NO"] = matNo;
				twma7["STOCK_OPER_ORDER"] = stockOperOrder;
				twma7["CRANE_INST_CODE"] = crane_inst_code;
				twma7["CRANE_INST_STATUS"] = "0";
				twma7["MOVE_TYPE"] = stockOperOrder;
				twma7["STOCK_OPER_ORDER_FIN"] = stockOperOrder;

				Log::Trace("", __FUNCTION__, "写材料信息");
				twma7["MAT_KIND"] = dtmat.Rows[0]["MAT_KIND"].ToString();
				twma7["MAT_SHAPE_FLAG"] = dtmat.Rows[0]["MAT_SHAPE_FLAG"].ToString();
				twma7["MAT_ACT_THICK"] = dtmat.Rows[0]["MAT_ACT_THICK"].ToDecimal();
				twma7["MAT_ACT_WIDTH"] = dtmat.Rows[0]["MAT_ACT_THICK"].ToDecimal();
				twma7["MAT_ACT_LEN"] = dtmat.Rows[0]["MAT_ACT_LEN"].ToDecimal();
				twma7["MAT_ACT_WT"] = dtmat.Rows[0]["MAT_ACT_WT"].ToDecimal();

				Log::Trace("", __FUNCTION__, "写源库位信息");
				twma7["STOCK_NO"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_NO_TO"].ToString();
				twma7["STOCK_NO_FROM"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_NO_TO"].ToString();
				twma7["STOCK_PLACE_NO_FROM"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_PLACE_NO_FROM"].ToString();
				twma7["YARD_LAYER_FROM"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["LAYERNO_FROM"].ToDecimal();
				if (bcls_rec->Tables["WM00_CMD"].Columns.Contains("HALL_NO_FR"))
				{
					twma7["HALL_NO_FR"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["HALL_NO_FR"].ToString();
				}
				else
				{
					twma7["HALL_NO_FR"] = twma7["STOCK_NO_FROM"].ToString();
				}
				if (bcls_rec->Tables["WM00_CMD"].Columns.Contains("VEHICLE_NO"))
				{
					twma7["VEHICLE_NO"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["VEHICLE_NO"].ToString().Trim();
				}
				else
				{
					twma7["VEHICLE_NO"] = " ";
				}
				Log::Trace("", __FUNCTION__, "写目标库位信息");

				twma7["STOCK_NO_TO"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_NO_TO"].ToString();
				twma7["STOCK_NO_FIN"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_NO_TO"].ToString();
				if (bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_PLACE_NO_TO"].ToString().Trim() == "")
				{
					twma7["STOCK_PLACE_NO_TO"] = " ";
				}
				else
				{
					twma7["STOCK_PLACE_NO_TO"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_PLACE_NO_TO"].ToString();
				}

				if (bcls_rec->Tables["WM00_CMD"].Columns.Contains("HALL_NO_TO") && bcls_rec->Tables["WM00_CMD"].Rows[i]["HALL_NO_TO"].ToString().Trim() != "")
				{
					twma7["HALL_NO_TO"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["HALL_NO_TO"].ToString();
				}
				else
				{
					twma7["HALL_NO_TO"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_NO_TO"].ToString();
				}
				if (bcls_rec->Tables["WM00_CMD"].Columns.Contains("HALL_NO_FIN") && bcls_rec->Tables["WM00_CMD"].Rows[i]["HALL_NO_FIN"].ToString().Trim() != "")
				{
					twma7["HALL_NO_FIN"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["HALL_NO_FIN"].ToString();
				}
				else
				{
					twma7["HALL_NO_FIN"] = twma7["STOCK_NO_TO"].ToString();
				}
				twma7_tem.Reset();
				twma7_tem["MAT_NO"] = matNo;
				if (twma7_tem.Query("MAT_NO"))
				{
					if (twma7_tem["CRANE_INST_STATUS"].ToString() == "0")
					{
						if (twma7_tem["STOCK_OPER_ORDER_FIN"].ToString() != "2C")
						{
							Log::Trace("", __FUNCTION__, "材料存在命令,更新命令");
							twma7["REC_ERASOR"] = s.userid;
							twma7["REC_ERASE_TIME"] = dateTime;
							twma7.Update("REC_ERASOR,REC_ERASE_TIME,STOCK_PLACE_NO_FROM,STOCK_NO,STOCK_NO_FROM,STOCK_NO_TO,STOCK_NO_FIN,STOCK_PLACE_NO_TO,HALL_NO_TO,HALL_NO_FIN,STOCK_OPER_ORDER,VEHICLE_NO", "MAT_NO");

							doFlag = f_wm00_pileinfocal(twma7_tem["STOCK_NO"].ToString(), twma7_tem["STOCK_PLACE_NO_TO"].ToString(), bcls_ret, conn);
							if (doFlag != 0)
							{
								throw CApplicationException(-1, s.msg, log.Location);
							}
						}
						else
						{
							Log::Trace("", __FUNCTION__, "已存在命令，且不能替换，跳过");
							continue;
						}
					}
					else
					{
						Log::Trace("", __FUNCTION__, "材料命令状态不为0，不能修改，跳过");
						continue;
					}
				}
				else
				{
					if (bcls_rec->Tables[0].Columns.Contains("CMD_SEQ") && bcls_rec->Tables[0].Rows[i]["CMD_SEQ"].ToString() != "0")
					{
						twma7["CMD_SEQ"] = bcls_rec->Tables[0].Rows[i]["CMD_SEQ"];
					}
					else
					{
						twma7["CMD_SEQ"] = atoi(WM_Utility::GetSeqence("seqTest", conn));
						if (twma7["CMD_SEQ"].ToDecimal() > maxseqno)
						{
							doFlag = f_wmsm_craneCmd_seq_update(bcls_rec, bcls_ret, conn);
							if (doFlag != 0)
							{
								throw CApplicationException(-1, s.msg, log.Location);
							}
							twma7["CMD_SEQ"] = atoi(WM_Utility::GetSeqence("seqTest", conn));
						}
					}
					twma7.Insert();
				}
			}
			else
			{
				Log::Trace("", __FUNCTION__, "材料[{0}]非入库命令", matNo);
				twma2.Reset();
				twma2["MAT_NO"] = matNo;
				twma2.Query("MAT_NO");
				dtmat.Rows.Clear();
				sqlstr = "SELECT mat_no,mat_kind,mat_shape_flag,mat_act_thick,mat_act_width,mat_act_len,mat_act_wt"
					" FROM " + v_table_name + " WHERE mat_no = '" + matNo + "'";
				Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
				cmd_inq_make.SetCommandText(sqlstr);
				cmd_inq_make.ExecuteQuery(dtmat);
				cmd_inq_make.Close();

				if (dtmat.Rows.get_Count() == 0)
				{
					sprintf(s.msg, "Mat No.[%s] is not exist.", (const char*)matNo);
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				if (!bcls_rec->Tables["WM00_CMD"].Columns.Contains("STOCK_NO_TO") ||
					bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_NO_TO"].ToString().Trim() == "")
				{
					sprintf(s.msg, "There's no To Yard No. for material [%s].", (const char*)matNo);
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				twma7.Reset();
				twma7["REC_CREATE_TIME"] = dateTime;
				twma7["REC_CREATOR"] = s.userid;
				twma7["MAT_NO"] = matNo;
				twma7["STOCK_OPER_ORDER"] = stockOperOrder;
				twma7["CRANE_INST_CODE"] = crane_inst_code;
				twma7["CRANE_INST_STATUS"] = "0";
				twma7["MOVE_TYPE"] = stockOperOrder;
				twma7["STOCK_NO"] = twma2["STOCK_NO"].ToString();
				if (bcls_rec->Tables["WM00_CMD"].Columns.Contains("STOCK_OPER_ORDER_FIN") && bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_OPER_ORDER_FIN"].ToString().Trim() != "")
				{
					twma7["STOCK_OPER_ORDER_FIN"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_OPER_ORDER_FIN"].ToString();
				}
				else
				{
					twma7["STOCK_OPER_ORDER_FIN"] = stockOperOrder;
				}

				Log::Trace("", __FUNCTION__, "写材料信息");
				twma7["MAT_KIND"] = dtmat.Rows[0]["MAT_KIND"].ToString();
				twma7["MAT_SHAPE_FLAG"] = dtmat.Rows[0]["MAT_SHAPE_FLAG"].ToString();
				twma7["MAT_ACT_THICK"] = dtmat.Rows[0]["MAT_ACT_THICK"].ToDecimal();
				twma7["MAT_ACT_WIDTH"] = dtmat.Rows[0]["MAT_ACT_THICK"].ToDecimal();
				twma7["MAT_ACT_LEN"] = dtmat.Rows[0]["MAT_ACT_LEN"].ToDecimal();
				twma7["MAT_ACT_WT"] = dtmat.Rows[0]["MAT_ACT_WT"].ToDecimal();

				Log::Trace("", __FUNCTION__, "写源库位信息");
				twma7["STOCK_NO_FROM"] = twma2["STOCK_NO"].ToString();
				twma7["STOCK_PLACE_NO_FROM"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_PLACE_NO_FROM"].ToString();
				twma7["YARD_LAYER_FROM"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["YARD_LAYER_FROM"].ToDecimal();
				if (bcls_rec->Tables["WM00_CMD"].Columns.Contains("HALL_NO_FR") && bcls_rec->Tables["WM00_CMD"].Rows[i]["HALL_NO_FR"].ToString().Trim() != "")
				{
					twma7["HALL_NO_FR"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["HALL_NO_FR"].ToString();
				}
				else
				{
					twma7["HALL_NO_FR"] = twma7["STOCK_NO_FROM"];
				}
				if (bcls_rec->Tables["WM00_CMD"].Columns.Contains("VEHICLE_NO") && bcls_rec->Tables["WM00_CMD"].Rows[i]["VEHICLE_NO"].ToString().Trim() != "")
				{
					twma7["VEHICLE_NO"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["VEHICLE_NO"].ToString().Trim();
				}
				else
				{
					twma7["VEHICLE_NO"] = " ";
				}
				Log::Trace("", __FUNCTION__, "写目标库位信息");
				twma7["STOCK_NO_TO"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_NO_TO"].ToString();
				if (bcls_rec->Tables["WM00_CMD"].Columns.Contains("STOCK_NO_FIN") && bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_NO_FIN"].ToString().Trim() != "")
				{
					twma7["STOCK_NO_FIN"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_NO_FIN"].ToString();
				}
				else
				{
					twma7["STOCK_NO_FIN"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_NO_TO"].ToString();
				}

				if (bcls_rec->Tables["WM00_CMD"].Columns.Contains("HALL_NO_TO") && bcls_rec->Tables["WM00_CMD"].Rows[i]["HALL_NO_TO"].ToString().Trim() != "")
				{
					twma7["HALL_NO_TO"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["HALL_NO_TO"].ToString();
				}
				else
				{
					twma7["HALL_NO_TO"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_NO_TO"].ToString().Trim();
				}

				if (bcls_rec->Tables["WM00_CMD"].Columns.Contains("HALL_NO_FIN") && bcls_rec->Tables["WM00_CMD"].Rows[i]["HALL_NO_FIN"].ToString().Trim() != "")
				{
					twma7["HALL_NO_FIN"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["HALL_NO_FIN"].ToString();
				}
				else
				{
					twma7["HALL_NO_FIN"] = twma7["STOCK_NO_TO"].ToString();
				}

				if (bcls_rec->Tables["WM00_CMD"].Columns.Contains("MAIN_MAT_NO") && bcls_rec->Tables["WM00_CMD"].Rows[i]["MAIN_MAT_NO"].ToString().Trim() == "1")
				{
					twma7["MAIN_MAT_NO"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["MAIN_MAT_NO"].ToString();
				}
				else
				{
					twma7["MAIN_MAT_NO"] = "0";
				}

				if (stockOperOrder == "2B")
				{
					Log::Trace("", __FUNCTION__, "材料为上料命令");
					twma7_tem.Reset();
					twma7_tem["MAT_NO"] = matNo;
					if (twma7_tem.Query("MAT_NO"))
					{
						if (twma7_tem["CRANE_INST_STATUS"].ToString() == "0" ||
							twma7_tem["MAIN_MAT_NO"].ToString() != "1")
						{
							Log::Trace("", __FUNCTION__, "材料命令状态为0且不是人工生成的命令，改为上料命令");
							twma7["REC_ERASOR"] = s.userid;
							twma7["REC_ERASE_TIME"] = dateTime;
							twma7["UNIT_CODE"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["UNIT_CODE"].ToString();
							twma7["STOCK_PLACE_NO_TO"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_PLACE_NO_TO"].ToString();
							twma7.Update("REC_ERASOR,REC_ERASE_TIME,STOCK_PLACE_NO_FROM,STOCK_NO_TO,STOCK_NO_FIN,STOCK_PLACE_NO_TO,HALL_NO_TO,HALL_NO_FIN,STOCK_OPER_ORDER,UNIT_CODE,STOCK_OPER_ORDER_FIN", "MAT_NO");
							doFlag = f_wm00_pileinfocal(twma7_tem["STOCK_NO"].ToString(),
								twma7_tem["STOCK_PLACE_NO_TO"].ToString(), bcls_ret, conn);
							if (doFlag != 0)
							{
								throw CApplicationException(-1, s.msg, log.Location);
							}
							continue;

						}
						else
						{
							Log::Trace("", __FUNCTION__, "材料命令状态不为0，更新最终库位和最终库位类型");
							twma7["REC_ERASOR"] = s.userid;
							twma7["REC_ERASE_TIME"] = dateTime;
							twma7["STOCK_PLACE_NO_TO"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_PLACE_NO_TO"].ToString();
							twma7["STOCK_PLACE_NO_FIN"] = twma7["STOCK_PLACE_NO_TO"];
							twma7["UNIT_CODE"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["UNIT_CODE"].ToString();
							twma7.Update("REC_ERASOR,REC_ERASE_TIME,STOCK_NO_FIN,STOCK_PLACE_NO_FIN,STOCK_OPER_ORDER_FIN,UNIT_CODE", "MAT_NO");
							continue;
						}
					}
					else
					{
						Log::Trace("", __FUNCTION__, "材料不存在命令，生成上料命令");
						if (bcls_rec->Tables[0].Columns.Contains("CMD_SEQ") && bcls_rec->Tables[0].Rows[i]["CMD_SEQ"].ToDecimal() != 0)
						{
							twma7["CMD_SEQ"] = bcls_rec->Tables[0].Rows[i]["CMD_SEQ"];
						}
						else
						{
							twma7["CMD_SEQ"] = atoi(WM_Utility::GetSeqence("seqTest", conn));
							if (twma7["CMD_SEQ"].ToDecimal() > maxseqno)
							{
								doFlag = f_wmsm_craneCmd_seq_update(bcls_rec, bcls_ret, conn);
								if (doFlag != 0)
								{
									throw CApplicationException(-1, s.msg, log.Location);
								}
								twma7["CMD_SEQ"] = atoi(WM_Utility::GetSeqence("seqTest", conn));
							}
						}
						twma7["UNIT_CODE"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["UNIT_CODE"].ToString();
						twma7["STOCK_PLACE_NO_TO"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_PLACE_NO_TO"].ToString();
						twma7.Insert();
					}
				}
				else if (stockOperOrder == "2C")
				{
					Log::Trace("", __FUNCTION__, "材料为备料命令");
					continue;
				}
				else if (stockOperOrder == "2E")
				{
					Log::Trace("", __FUNCTION__, "材料为发货命令");
					continue;
				}
				else if (stockOperOrder == "32")
				{
					Log::Trace("", __FUNCTION__, "材料为过跨命令");
					twma7_tem.Reset();
					twma7_tem["MAT_NO"] = matNo;
					if (twma7_tem.Query("MAT_NO"))
					{
						if (twma7_tem["CRANE_INST_STATUS"].ToString() == "0" &&
							twma7_tem["MAIN_MAT_NO"].ToString() != "1" &&
							(twma7_tem["STOCK_OPER_ORDER"].ToString() == "31" || twma7_tem["STOCK_OPER_ORDER"].ToString() == "32"))
						{
							Log::Trace("", __FUNCTION__, "改为过跨命令");
							twma7["REC_ERASOR"] = s.userid;
							twma7["REC_ERASE_TIME"] = dateTime;
							twma7["STOCK_PLACE_NO_TO"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_PLACE_NO_TO"].ToString();
							twma7["UNIT_CODE"] = " ";
							if (bcls_rec->Tables["WM00_CMD"].Columns.Contains("STOCK_PLACE_NO_FIN") && bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_PLACE_NO_FIN"].ToString().Trim() != "")
							{
								twma7["STOCK_PLACE_NO_FIN"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_PLACE_NO_FIN"].ToString();
							}
							else
							{
								twma7["STOCK_PLACE_NO_FIN"] = " ";
							}

							twma7.Update("REC_ERASOR,REC_ERASE_TIME,STOCK_PLACE_NO_FROM,STOCK_NO_TO,STOCK_NO_FIN,STOCK_PLACE_NO_TO,HALL_NO_TO,HALL_NO_FIN,STOCK_OPER_ORDER,UNIT_CODE,STOCK_PLACE_NO_FIN,STOCK_NO,STOCK_NO_FROM", "MAT_NO");
							doFlag = f_wm00_pileinfocal(twma7_tem["STOCK_NO"].ToString(),
								twma7_tem["STOCK_PLACE_NO_TO"].ToString(), bcls_ret, conn);
							if (doFlag != 0)
							{
								throw CApplicationException(-1, s.msg, log.Location);
							}
							continue;
						}
						else
						{
							if (twma7_tem["MAIN_MAT_NO"].ToString() == "1")
							{
								Log::Trace("", __FUNCTION__, "材料命令状态不为0，更新最终库位和最终库位类型");
								twma7["REC_ERASOR"] = s.userid;
								twma7["REC_ERASE_TIME"] = dateTime;
								twma7["STOCK_PLACE_NO_TO"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_PLACE_NO_TO"].ToString();
								if (bcls_rec->Tables["WM00_CMD"].Columns.Contains("STOCK_PLACE_NO_FIN") && bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_PLACE_NO_FIN"].ToString().Trim() != "")
								{
									twma7["STOCK_PLACE_NO_FIN"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_PLACE_NO_FIN"].ToString();
								}
								else
								{
									twma7["STOCK_PLACE_NO_FIN"] = " ";
								}
								twma7.Update("REC_ERASOR,REC_ERASE_TIME,STOCK_PLACE_NO_FROM,STOCK_NO_FROM,STOCK_NO_FIN,STOCK_PLACE_NO_FIN,STOCK_OPER_ORDER_FIN", "MAT_NO");
								continue;
							}
							else
							{
								continue;
							}
						}
					}
					else
					{
						Log::Trace("", __FUNCTION__, "材料不存在命令，生成过跨命令");
						Log::Trace("", __FUNCTION__, "上层倒跺");
						sqlstr = "select mat_no,stock_place_no,layerno from twma2 a where stock_place_no='" +
							twma2["STOCK_PLACE_NO"].ToString() +
							"'and layerno>='" + twma2["LAYERNO"].ToString() +
							"'and a.mat_no not in (select mat_no from twma7 b where a.mat_no = b.mat_no) order by a.layerno desc";
						Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
						cmd_inq_make.SetCommandText(sqlstr);
						cmd_inq_make.ExecuteQuery(up_dtMat);
						cmd_inq_make.Close();
						Log::Trace("", __FUNCTION__, "材料{0}上层个数：{1}", twma7_tem["MAT_NO"].ToString(), up_dtMat.Rows.get_Count() - 1);
						for (int j = 0; j < up_dtMat.Rows.get_Count(); j++)
						{
							dtmat2.Rows.Clear();
							if (j == up_dtMat.Rows.get_Count() - 1)
							{
								Log::Trace("", __FUNCTION__, "生成材料{0}命令", twma7_tem["MAT_NO"].ToString());
							}
							else
							{
								Log::Trace("", __FUNCTION__, "生成材料{0}上层倒跺命令", up_dtMat.Rows[j]["MAT_NO"].ToString());
							}
							sqlstr = "SELECT a.mat_no,a.stock_no,a.hall_no,a.stock_place_no,a.layerno,b.mat_kind,"
								"b.mat_shape_flag,b.mat_act_thick,b.mat_act_width,b.mat_act_len,b.mat_act_wt "
								" FROM twma2 a," + v_table_name + " b WHERE a.mat_no = '" + up_dtMat.Rows[j]["MAT_NO"].ToString() + "' AND a.mat_no = b.mat_no";
							Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
							cmd_inq_make.SetCommandText(sqlstr);
							cmd_inq_make.ExecuteQuery(dtmat2);
							cmd_inq_make.Close();
							if (dtmat2.Rows.get_Count() == 0)
							{
								sprintf(s.msg, "Material [%s] is not in yard.", (const char*)twma7["MAT_NO"]);
								throw CApplicationException(-1, s.msg, s.svc_name);
							}

							//新增行车命令数据表行
							twma7.Reset();
							twma7["REC_CREATOR"] = s.userid;
							twma7["REC_CREATE_TIME"] = dateTime;
							if (bcls_rec->Tables[0].Columns.Contains("CMD_SEQ") && bcls_rec->Tables[0].Rows[i]["CMD_SEQ"].ToString() != "0")
							{
								twma7["CMD_SEQ"] = bcls_rec->Tables[0].Rows[i]["CMD_SEQ"];
							}
							else
							{
								twma7["CMD_SEQ"] = atoi(WM_Utility::GetSeqence("seqTest", conn));
								if (twma7["CMD_SEQ"].ToDecimal() > maxseqno)
								{
									doFlag = f_wmsm_craneCmd_seq_update(bcls_rec, bcls_ret, conn);
									if (doFlag != 0)
									{
										throw CApplicationException(-1, s.msg, log.Location);
									}
									twma7["CMD_SEQ"] = atoi(WM_Utility::GetSeqence("seqTest", conn));
								}
							}
							if (j == up_dtMat.Rows.get_Count() - 1)
							{
								twma7["MAT_NO"] = twma7_tem["MAT_NO"];
							}
							else
							{
								twma7["MAT_NO"] = up_dtMat.Rows[j]["MAT_NO"].ToString();
							}

							twma7["CRANE_INST_STATUS"] = "0";
							twma7["CRANE_INST_CODE"] = stockOperOrder.Substring(0, 1);

							Log::Trace("", __FUNCTION__, "写材料源库位信息");
							twma7["STOCK_NO"] = dtmat2.Rows[0]["STOCK_NO"];
							twma7["STOCK_NO_FROM"] = dtmat2.Rows[0]["STOCK_NO"];
							twma7["HALL_NO_FR"] = dtmat2.Rows[0]["HALL_NO"];
							twma7["STOCK_PLACE_NO_FROM"] = dtmat2.Rows[0]["STOCK_PLACE_NO"];
							twma7["YARD_LAYER_FROM"] = dtmat2.Rows[0]["LAYERNO"];

							Log::Trace("", __FUNCTION__, "写材料信息");
							twma7["MAT_KIND"] = dtmat2.Rows[0]["MAT_KIND"];
							twma7["MAT_SHAPE_FLAG"] = dtmat2.Rows[0]["MAT_SHAPE_FLAG"];
							twma7["MAT_ACT_THICK"] = dtmat2.Rows[0]["MAT_ACT_THICK"];
							twma7["MAT_ACT_WIDTH"] = dtmat2.Rows[0]["MAT_ACT_WIDTH"];
							twma7["MAT_ACT_LEN"] = dtmat2.Rows[0]["MAT_ACT_LEN"];
							twma7["MAT_ACT_WT"] = dtmat2.Rows[0]["MAT_ACT_WT"];
							twma7["MOVE_TYPE"] = twma7_tem["MOVE_TYPE"].ToString();

							Log::Trace("", __FUNCTION__, "写目标库位信息");
							if (j == up_dtMat.Rows.get_Count() - 1)
							{
								twma7["STOCK_PLACE_NO_TO"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_PLACE_NO_TO"].ToString();

								twma7["STOCK_OPER_ORDER"] = "32";
								twma7["STOCK_OPER_ORDER_FIN"] = "32";
								if (bcls_rec->Tables["WM00_CMD"].Columns.Contains("STOCK_PLACE_NO_FIN") && bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_PLACE_NO_FIN"].ToString().Trim() != "")
								{
									twma7["STOCK_PLACE_NO_FIN"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_PLACE_NO_FIN"].ToString();
								}
								else
								{
									twma7["STOCK_PLACE_NO_FIN"] = " ";
								}
								twma7["STOCK_NO_TO"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_NO_TO"].ToString();
								if (bcls_rec->Tables["WM00_CMD"].Columns.Contains("STOCK_NO_FIN") && bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_NO_FIN"].ToString().Trim() != "")
								{
									twma7["STOCK_NO_FIN"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_NO_FIN"].ToString();
								}
								else
								{
									twma7["STOCK_NO_FIN"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_NO_TO"].ToString();
								}
								if (bcls_rec->Tables["WM00_CMD"].Columns.Contains("MAIN_MAT_NO") && bcls_rec->Tables["WM00_CMD"].Rows[i]["MAIN_MAT_NO"].ToString().Trim() == "1")
								{
									twma7["MAIN_MAT_NO"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["MAIN_MAT_NO"].ToString();
								}
								else
								{
									twma7["MAIN_MAT_NO"] = "0";
								}
							}
							else
							{
								twma7["STOCK_NO_TO"] = dtmat2.Rows[0]["STOCK_NO"];
								twma7["STOCK_PLACE_NO_TO"] = " ";
								twma7["STOCK_OPER_ORDER"] = "31";
								twma7["UNIT_CODE"] = " ";
							}
							Log::Trace("", __FUNCTION__, "twma7.STOCK_PLACE_NO_FIN {0}", twma7["STOCK_PLACE_NO_FIN"].ToString());
							if (twma7.QueryCount("MAT_NO") > 0)
							{
								Log::Trace("", __FUNCTION__, "材料{0}有命令", twma7["MAT_NO"].ToString());
								twma7.Update("STOCK_PLACE_NO_TO,STOCK_OPER_ORDER,STOCK_OPER_ORDER_FIN,UNIT_CODE,CRANE_CMDGRPNO", "MAT_NO");
								doFlag = f_wm00_pileinfocal(twma7_tem["STOCK_NO"].ToString(),
									twma7_tem["STOCK_PLACE_NO_TO"].ToString(), bcls_ret, conn);
								if (doFlag != 0)
								{
									throw CApplicationException(-1, s.msg, log.Location);
								}
							}
							else
							{
								twma7.Insert();
							}
						}
					}

				}
				else
				{
					Log::Trace("", __FUNCTION__, "材料为其他命令，有命令跳过，没命令生成");
					twma7_tem.Reset();
					twma7_tem["MAT_NO"] = matNo;
					if (twma7_tem.Query("MAT_NO"))
					{
						Log::Trace("", __FUNCTION__, "材料已存在命令，跳过");
						continue;
					}
					else
					{
						Log::Trace("", __FUNCTION__, "材料不存在命令，生成命令");
						twma2.Reset();
						twma2["MAT_NO"] = matNo;
						twma2.Query("MAT_NO");
						Log::Trace("", __FUNCTION__, "上层倒跺");
						sqlstr = "select mat_no,stock_place_no,layerno from twma2 a where stock_place_no='" +
							twma2["STOCK_PLACE_NO"].ToString() +
							"'and layerno>='" + twma2["LAYERNO"].ToString() +
							"'and a.mat_no not in (select mat_no from twma7 b where a.mat_no = b.mat_no) order by a.layerno desc";
						Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
						cmd_inq_make.SetCommandText(sqlstr);
						cmd_inq_make.ExecuteQuery(up_dtMat);
						cmd_inq_make.Close();
						Log::Trace("", __FUNCTION__, "材料{0}上层个数：{1}", twma7_tem["MAT_NO"].ToString(), up_dtMat.Rows.get_Count() - 1);
						for (int j = 0; j < up_dtMat.Rows.get_Count(); j++)
						{
							dtmat2.Rows.Clear();
							if (j == up_dtMat.Rows.get_Count() - 1)
							{
								Log::Trace("", __FUNCTION__, "生成材料{0}命令", twma7_tem["MAT_NO"].ToString());
							}
							else
							{
								Log::Trace("", __FUNCTION__, "生成材料{0}上层倒跺命令", up_dtMat.Rows[j]["MAT_NO"].ToString());
							}
							sqlstr = "SELECT a.mat_no,a.stock_no,a.hall_no,a.stock_place_no,a.layerno,b.mat_kind,"
								"b.mat_shape_flag,b.mat_act_thick,b.mat_act_width,b.mat_act_len,b.mat_act_wt "
								" FROM twma2 a," + v_table_name + " b WHERE a.mat_no = '" + up_dtMat.Rows[j]["MAT_NO"].ToString() + "' AND a.mat_no = b.mat_no";
							Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
							cmd_inq_make.SetCommandText(sqlstr);
							cmd_inq_make.ExecuteQuery(dtmat2);
							cmd_inq_make.Close();
							if (dtmat2.Rows.get_Count() == 0)
							{
								sprintf(s.msg, "Material [%s] is not in yard.", (const char*)twma7["MAT_NO"]);
								throw CApplicationException(-1, s.msg, s.svc_name);
							}

							//新增行车命令数据表行
							twma7.Reset();
							twma7["REC_CREATOR"] = s.userid;
							twma7["REC_CREATE_TIME"] = dateTime;
							if (bcls_rec->Tables[0].Columns.Contains("CMD_SEQ") && bcls_rec->Tables[0].Rows[i]["CMD_SEQ"].ToString() != "0")
							{
								twma7["CMD_SEQ"] = bcls_rec->Tables[0].Rows[i]["CMD_SEQ"];
							}
							else
							{
								twma7["CMD_SEQ"] = atoi(WM_Utility::GetSeqence("seqTest", conn));
								if (twma7["CMD_SEQ"].ToDecimal() > maxseqno)
								{
									doFlag = f_wmsm_craneCmd_seq_update(bcls_rec, bcls_ret, conn);
									if (doFlag != 0)
									{
										throw CApplicationException(-1, s.msg, log.Location);
									}
									twma7["CMD_SEQ"] = atoi(WM_Utility::GetSeqence("seqTest", conn));
								}
							}
							if (j == up_dtMat.Rows.get_Count() - 1)
							{
								twma7["MAT_NO"] = twma7_tem["MAT_NO"].ToString();
							}
							else
							{
								twma7["MAT_NO"] = up_dtMat.Rows[j]["MAT_NO"].ToString();
							}

							twma7["CRANE_INST_STATUS"] = "0";
							twma7["CRANE_INST_CODE"] = stockOperOrder.Substring(0, 1);

							Log::Trace("", __FUNCTION__, "写材料源库位信息");
							twma7["STOCK_NO"] = dtmat2.Rows[0]["STOCK_NO"];
							twma7["STOCK_NO_FROM"] = dtmat2.Rows[0]["STOCK_NO"];
							twma7["HALL_NO_FR"] = dtmat2.Rows[0]["HALL_NO"];
							twma7["STOCK_PLACE_NO_FROM"] = dtmat2.Rows[0]["STOCK_PLACE_NO"];
							twma7["YARD_LAYER_FROM"] = dtmat2.Rows[0]["LAYERNO"];

							Log::Trace("", __FUNCTION__, "写材料信息");
							twma7["MAT_KIND"] = dtmat2.Rows[0]["MAT_KIND"];
							twma7["MAT_SHAPE_FLAG"] = dtmat2.Rows[0]["MAT_SHAPE_FLAG"];
							twma7["MAT_ACT_THICK"] = dtmat2.Rows[0]["MAT_ACT_THICK"];
							twma7["MAT_ACT_WIDTH"] = dtmat2.Rows[0]["MAT_ACT_WIDTH"];
							twma7["MAT_ACT_LEN"] = dtmat2.Rows[0]["MAT_ACT_LEN"];
							twma7["MAT_ACT_WT"] = dtmat2.Rows[0]["MAT_ACT_WT"];
							twma7["MOVE_TYPE"] = stockOperOrder;

							Log::Trace("", __FUNCTION__, "写目标库位信息");
							if (j == up_dtMat.Rows.get_Count() - 1)
							{
								if (bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_PLACE_NO_TO"].ToString().Trim() == "")
								{
									twma7["STOCK_PLACE_NO_TO"] = " ";
								}
								else
								{
									twma7["STOCK_PLACE_NO_TO"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_PLACE_NO_TO"].ToString();
								}
								twma7["STOCK_OPER_ORDER"] = stockOperOrder;
								twma7["STOCK_OPER_ORDER_FIN"] = stockOperOrder;
								if (bcls_rec->Tables["WM00_CMD"].Columns.Contains("MAIN_MAT_NO") && bcls_rec->Tables["WM00_CMD"].Rows[i]["MAIN_MAT_NO"].ToString().Trim() == "1")
								{
									twma7["MAIN_MAT_NO"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["MAIN_MAT_NO"].ToString();
								}
								else
								{
									twma7["MAIN_MAT_NO"] = "0";
								}
							}
							else
							{
								twma7["STOCK_PLACE_NO_TO"] = " ";
								twma7["STOCK_OPER_ORDER"] = "31";
								twma7["UNIT_CODE"] = " ";
							}
							if (twma7.QueryCount("MAT_NO") > 0)
							{
								Log::Trace("", __FUNCTION__, "材料{0}有命令", twma7["MAT_NO"].ToString());
								twma7.Update("STOCK_PLACE_NO_TO,STOCK_OPER_ORDER,STOCK_OPER_ORDER_FIN,UNIT_CODE,CRANE_CMDGRPNO,MAIN_MAT_NO", "MAT_NO");
								doFlag = f_wm00_pileinfocal(twma7_tem["STOCK_NO"].ToString(),
									twma7_tem["STOCK_PLACE_NO_TO"].ToString(), bcls_ret, conn);
								if (doFlag != 0)
								{
									throw CApplicationException(-1, s.msg, log.Location);
								}
							}
							else
							{
								twma7.Insert();
							}
						}
						continue;
					}
				}
			}
		}

#pragma region  推荐倒跺库位
		//sqlstr = "select distinct b.stock_no FROM TWMA7 a, TWMA2 b WHERE a.MAT_NO = b.MAT_NO AND  a.STOCK_PLACE_NO_TO=' ' AND a.stock_oper_order like '3%' AND b.stock_no!=' '";
		//Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		//cmd_inq_make.SetCommandText(sqlstr);
		//cmd_inq_make.ExecuteQuery(dtStockNo);
		//cmd_inq_make.Close();
		//Log::Trace("", __FUNCTION__, "倒跺库区个数：{0}", dtStockNo.Rows.get_Count());

		//sqlstr = " SELECT a.MAT_NO, a.STOCK_PLACE_NO_TO, b.STOCK_NO FROM TWMA7 a, TWMA2 b WHERE a.MAT_NO = b.MAT_NO AND STOCK_PLACE_NO_TO = ' ' AND stock_oper_order like '3%' order by CMD_SEQ  ";
		//Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		//cmd_inq_make.SetCommandText(sqlstr);
		//cmd_inq_make.ExecuteQuery(dtmat1);
		//cmd_inq_make.Close();
		//Log::Trace("", __FUNCTION__, "推荐倒跺材料个数：{0}", dtmat1.Rows.get_Count());
		//for (int i = 0; i < dtStockNo.Rows.get_Count(); i++)
		//{
		//	// 初始化 库位推荐用 输入/输出块

		//	EIClass in, out;
		//	f_wms_auto_init(&in, &out, conn);
		//	CDataTable& blkParams = in.Tables[WMS_BLK_IN_PARAMS];
		//	blkParams.Rows[0][WMS_COL_JOB_IO_DIV] = "M";         	                                      // 入出库区分(I：入库、O：出库、M：倒垛)	
		//	blkParams.Rows[0][WMS_COL_JOB_ADJUST_FLAG] = "N";                                             // 库区作业顺序是否可调(N:不可调、Y:可调整)	
		//	blkParams.Rows[0][WMS_COL_JOB_STOCK_NO] = dtStockNo.Rows[i]["STOCK_NO"];                      // 作业库区号(入库时：入库目标库区、 倒垛时：倒垛库区、 出库时：出库起始库)
		//	blkParams.Rows[0][WMS_COL_JOB_MAT_SHAPE_DIV] = "P";                                           // 材料形状区分(P:板类、C:卷类)
		//	Log::Trace("", __FUNCTION__, "---------------打印库位推荐传入块数据----------------");
		//	Log::Trace("", __FUNCTION__, "WMS_BLK_IN_PARAMS：");
		//	Log::Trace("", __FUNCTION__, "WMS_COL_JOB_IO_DIV={0}", blkParams.Rows[0][WMS_COL_JOB_IO_DIV].ToString());
		//	Log::Trace("", __FUNCTION__, "WMS_COL_JOB_ADJUST_FLAG={0}", blkParams.Rows[0][WMS_COL_JOB_ADJUST_FLAG].ToString());
		//	Log::Trace("", __FUNCTION__, "WMS_COL_JOB_STOCK_NO={0}", blkParams.Rows[0][WMS_COL_JOB_STOCK_NO].ToString());
		//	Log::Trace("", __FUNCTION__, "WMS_COL_JOB_MAT_SHAPE_DIV={0}", blkParams.Rows[0][WMS_COL_JOB_MAT_SHAPE_DIV].ToString());
		//	int currRow = -1;
		//	CDataTable& blkMats = in.Tables[WMS_BLK_IN_MATS];
		//	for (int j = 0; j < dtmat1.Rows.get_Count(); j++)
		//	{
		//		Log::Trace("", __FUNCTION__, "mat[{0}]", dtmat1.Rows[j]["MAT_NO"].ToString());
		//		Log::Trace("", __FUNCTION__, "mat_STOCK_NO[{0}]", dtmat1.Rows[j]["STOCK_NO"].ToString());
		//		Log::Trace("", __FUNCTION__, "STOCK_NO[{0}]", dtStockNo.Rows[i]["STOCK_NO"].ToString());
		//		Log::Trace("", __FUNCTION__, "STOCK_PLACE_NO_TO[{0}]", dtmat1.Rows[j]["STOCK_PLACE_NO_TO"].ToString());
		//		if (dtmat1.Rows[j]["STOCK_NO"].ToString() == dtStockNo.Rows[i]["STOCK_NO"].ToString() && dtmat1.Rows[j]["STOCK_PLACE_NO_TO"].ToString().Trim() == "")
		//		{
		//			++currRow;
		//			blkMats.Rows.Add();
		//			blkMats.Rows[currRow][WMS_COL_MAT_GRP_NO] = dtmat1.Rows[j]["MAT_NO"].ToString();					// 材料组号（1个材料组可含有一个或多个材料。当含有多个材料时，这些材料将作为一个整体叠放在同一个垛位上【适用于钢板】。当只有一块材料时，通常 材料组号=材料号。）
		//			blkMats.Rows[currRow][WMS_COL_MAT_NO] = dtmat1.Rows[j]["MAT_NO"].ToString();						// 材料号
		//			blkMats.Rows[currRow][WMS_COL_MAT_KIND] = " ";                                                      // 材料种类(HP:中厚板、CR：冷轧、HR：热轧 等等)【印度项目不用，传空格】
		//			blkMats.Rows[currRow][WMS_COL_JOB_LARGE_DIV] = "31";                                                // 库区作业大分类：库业务类型(代码WM10)
		//			blkMats.Rows[currRow][WMS_COL_JOB_MIDDLE_DIV] = " ";                                                // 库区作业中分类：【印度项目不用，传空格】
		//			blkMats.Rows[currRow][WMS_COL_FROM_STOCK_DEV_NO] = " ";
		//			Log::Trace("", __FUNCTION__, "---------------打印库位推荐传入块数据----------------");
		//			Log::Trace("", __FUNCTION__, "WMS_BLK_IN_MATS：");
		//			Log::Trace("", __FUNCTION__, "WMS_COL_MAT_GRP_NO={0}", blkMats.Rows[currRow][WMS_COL_MAT_GRP_NO].ToString());
		//			Log::Trace("", __FUNCTION__, "WMS_COL_MAT_NO={0}", blkMats.Rows[currRow][WMS_COL_MAT_NO].ToString());
		//			Log::Trace("", __FUNCTION__, "WMS_COL_MAT_KIND={0}", blkMats.Rows[currRow][WMS_COL_MAT_KIND].ToString());
		//			Log::Trace("", __FUNCTION__, "WMS_COL_JOB_LARGE_DIV={0}", blkMats.Rows[currRow][WMS_COL_JOB_LARGE_DIV].ToString());
		//			Log::Trace("", __FUNCTION__, "WMS_COL_JOB_MIDDLE_DIV={0}", blkMats.Rows[currRow][WMS_COL_JOB_MIDDLE_DIV].ToString());
		//			Log::Trace("", __FUNCTION__, "WMS_COL_FROM_STOCK_DEV_NO={0}", blkMats.Rows[currRow][WMS_COL_FROM_STOCK_DEV_NO].ToString());
		//		}
		//		if (j == dtmat1.Rows.get_Count() - 1)
		//		{
		//			//执行库位推荐
		//			doFlag = f_wms_auto(&in, &out, conn);
		//			if (doFlag == 0)
		//			{
		//				WM_Utility::PrintLog("库位推荐成功");
		//				WM_Utility::PrintLog("打印推荐结果目标垛位块");
		//				WM_Utility::PrintDataTable(out.Tables[WMS_BLK_OUT_TARGETS]);
		//				WM_Utility::PrintLog("推荐结果材料移动块");
		//				WM_Utility::PrintDataTable(out.Tables[WMS_BLK_OUT_MOVES]);
		//				for (int t = 0; t < out.Tables[WMS_BLK_OUT_TARGETS].Rows.get_Count(); t++)
		//				{
		//					twma7.Reset();
		//					twma7["MAT_NO"] = out.Tables[WMS_BLK_OUT_TARGETS].Rows[t]["MAT_NO"].ToString();
		//					twma7["STOCK_PLACE_NO_TO"] = out.Tables[WMS_BLK_OUT_TARGETS].Rows[t]["TO_STOCK_PLACE_NO"].ToString();
		//					twma7["STOCK_NO"] = out.Tables[WMS_BLK_OUT_TARGETS].Rows[t]["TO_STOCK_NO"].ToString();
		//					twma7.Update("STOCK_PLACE_NO_TO", "MAT_NO");
		//					doFlag = f_wm00_pileinfocal(twma7["STOCK_NO"].ToString(),
		//						twma7["STOCK_PLACE_NO_TO"].ToString(), bcls_ret, conn);
		//					if (doFlag != 0)
		//					{
		//						throw CApplicationException(-1, s.msg, log.Location);
		//					}
		//				}
		//			}
		//			else
		//			{
		//				WM_Utility::PrintLog("库位推荐失败");
		//			}
		//		}
		//	}

		//}
#pragma endregion 

#pragma region  推荐入库库位
		//sqlstr = "select distinct stock_no from twmA7 WHERE STOCK_PLACE_NO_TO=' ' AND stock_oper_order like '1%' AND stock_no!=' '";
		//Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		//cmd_inq_make.SetCommandText(sqlstr);
		//cmd_inq_make.ExecuteQuery(dtStockNo);
		//cmd_inq_make.Close();
		//Log::Trace("", __FUNCTION__, "入库库区个数：{0}", dtStockNo.Rows.get_Count());

		//sqlstr = "SELECT MAT_NO,STOCK_PLACE_NO_TO,STOCK_NO FROM TWMA7 WHERE STOCK_PLACE_NO_TO=' ' AND stock_oper_order like '1%' order by CMD_SEQ  ";
		//Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		//cmd_inq_make.SetCommandText(sqlstr);
		//cmd_inq_make.ExecuteQuery(dtmat1);
		//cmd_inq_make.Close();
		//Log::Trace("", __FUNCTION__, "推荐入库材料个数：{0}", dtmat1.Rows.get_Count());
		//for (int i = 0; i < dtStockNo.Rows.get_Count(); i++)
		//{
		//	// 初始化 库位推荐用 输入/输出块
		//	EIClass in, out;
		//	f_wms_auto_init(&in, &out, conn);

		//	CDataTable& blkParams = in.Tables[WMS_BLK_IN_PARAMS];
		//	blkParams.Rows[0][WMS_COL_JOB_IO_DIV] = "I";         	                                      // 入出库区分(I：入库、O：出库、M：倒垛)	
		//	blkParams.Rows[0][WMS_COL_JOB_ADJUST_FLAG] = "N";                                             // 库区作业顺序是否可调(N:不可调、Y:可调整)	
		//	blkParams.Rows[0][WMS_COL_JOB_STOCK_NO] = dtStockNo.Rows[i]["STOCK_NO"];                      // 作业库区号(入库时：入库目标库区、 倒垛时：倒垛库区、 出库时：出库起始库)
		//	blkParams.Rows[0][WMS_COL_JOB_MAT_SHAPE_DIV] = "P";                                           // 材料形状区分(P:板类、C:卷类)
		//	Log::Trace("", __FUNCTION__, "---------------打印库位推荐传入块数据----------------");
		//	Log::Trace("", __FUNCTION__, "WMS_BLK_IN_PARAMS：");
		//	Log::Trace("", __FUNCTION__, "WMS_COL_JOB_IO_DIV={0}", blkParams.Rows[0][WMS_COL_JOB_IO_DIV].ToString());
		//	Log::Trace("", __FUNCTION__, "WMS_COL_JOB_ADJUST_FLAG={0}", blkParams.Rows[0][WMS_COL_JOB_ADJUST_FLAG].ToString());
		//	Log::Trace("", __FUNCTION__, "WMS_COL_JOB_STOCK_NO={0}", blkParams.Rows[0][WMS_COL_JOB_STOCK_NO].ToString());
		//	Log::Trace("", __FUNCTION__, "WMS_COL_JOB_MAT_SHAPE_DIV={0}", blkParams.Rows[0][WMS_COL_JOB_MAT_SHAPE_DIV].ToString());

		//	int currRow = -1;
		//	CDataTable& blkMats = in.Tables[WMS_BLK_IN_MATS];
		//	for (int j = 0; j < dtmat1.Rows.get_Count(); j++)
		//	{
		//		Log::Trace("", __FUNCTION__, "mat[{0}]", dtmat1.Rows[j]["MAT_NO"].ToString());
		//		Log::Trace("", __FUNCTION__, "mat_STOCK_NO[{0}]", dtmat1.Rows[j]["STOCK_NO"].ToString());
		//		Log::Trace("", __FUNCTION__, "STOCK_NO[{0}]", dtStockNo.Rows[i]["STOCK_NO"].ToString());
		//		Log::Trace("", __FUNCTION__, "STOCK_PLACE_NO_TO[{0}]", dtmat1.Rows[j]["STOCK_PLACE_NO_TO"].ToString());
		//		if (dtmat1.Rows[j]["STOCK_NO"].ToString() == dtStockNo.Rows[i]["STOCK_NO"].ToString() && dtmat1.Rows[j]["STOCK_PLACE_NO_TO"].ToString().Trim() == "")
		//		{
		//			++currRow;
		//			blkMats.Rows.Add();
		//			blkMats.Rows[currRow][WMS_COL_MAT_GRP_NO] = dtmat1.Rows[j]["MAT_NO"].ToString();					// 材料组号（1个材料组可含有一个或多个材料。当含有多个材料时，这些材料将作为一个整体叠放在同一个垛位上【适用于钢板】。当只有一块材料时，通常 材料组号=材料号。）
		//			blkMats.Rows[currRow][WMS_COL_MAT_NO] = dtmat1.Rows[j]["MAT_NO"].ToString();						// 材料号
		//			blkMats.Rows[currRow][WMS_COL_MAT_KIND] = " ";                                                      // 材料种类(HP:中厚板、CR：冷轧、HR：热轧 等等)【印度项目不用，传空格】
		//			blkMats.Rows[currRow][WMS_COL_JOB_LARGE_DIV] = "1E";                                                // 库区作业大分类：库业务类型(代码WM10)
		//			blkMats.Rows[currRow][WMS_COL_JOB_MIDDLE_DIV] = " ";                                                // 库区作业中分类：【印度项目不用，传空格】
		//			blkMats.Rows[currRow][WMS_COL_FROM_STOCK_DEV_NO] = " ";
		//			Log::Trace("", __FUNCTION__, "---------------打印库位推荐传入块数据----------------");
		//			Log::Trace("", __FUNCTION__, "WMS_BLK_IN_MATS：");
		//			Log::Trace("", __FUNCTION__, "WMS_COL_MAT_GRP_NO={0}", blkMats.Rows[currRow][WMS_COL_MAT_GRP_NO].ToString());
		//			Log::Trace("", __FUNCTION__, "WMS_COL_MAT_NO={0}", blkMats.Rows[currRow][WMS_COL_MAT_NO].ToString());
		//			Log::Trace("", __FUNCTION__, "WMS_COL_MAT_KIND={0}", blkMats.Rows[currRow][WMS_COL_MAT_KIND].ToString());
		//			Log::Trace("", __FUNCTION__, "WMS_COL_JOB_LARGE_DIV={0}", blkMats.Rows[currRow][WMS_COL_JOB_LARGE_DIV].ToString());
		//			Log::Trace("", __FUNCTION__, "WMS_COL_JOB_MIDDLE_DIV={0}", blkMats.Rows[currRow][WMS_COL_JOB_MIDDLE_DIV].ToString());
		//			Log::Trace("", __FUNCTION__, "WMS_COL_FROM_STOCK_DEV_NO={0}", blkMats.Rows[currRow][WMS_COL_FROM_STOCK_DEV_NO].ToString());
		//		}
		//		if (j == dtmat1.Rows.get_Count() - 1)
		//		{
		//			//执行库位推荐
		//			doFlag = f_wms_auto(&in, &out, conn);
		//			if (doFlag == 0)
		//			{
		//				WM_Utility::PrintLog("库位推荐成功");
		//				WM_Utility::PrintLog("打印推荐结果目标垛位块");
		//				WM_Utility::PrintDataTable(out.Tables[WMS_BLK_OUT_TARGETS]);
		//				WM_Utility::PrintLog("推荐结果材料移动块");
		//				WM_Utility::PrintDataTable(out.Tables[WMS_BLK_OUT_MOVES]);
		//				for (int t = 0; t < out.Tables[WMS_BLK_OUT_TARGETS].Rows.get_Count(); t++)
		//				{
		//					twma7.Reset();
		//					twma7["MAT_NO"] = out.Tables[WMS_BLK_OUT_TARGETS].Rows[t]["MAT_NO"].ToString();
		//					twma7["STOCK_PLACE_NO_TO"] = out.Tables[WMS_BLK_OUT_TARGETS].Rows[t]["TO_STOCK_PLACE_NO"].ToString();
		//					twma7["STOCK_NO"] = out.Tables[WMS_BLK_OUT_TARGETS].Rows[t]["TO_STOCK_NO"].ToString();
		//					twma7.Update("STOCK_PLACE_NO_TO", "MAT_NO");
		//					doFlag = f_wm00_pileinfocal(twma7["STOCK_NO"].ToString(),
		//						twma7["STOCK_PLACE_NO_TO"].ToString(), bcls_ret, conn);
		//					if (doFlag != 0)
		//					{
		//						throw CApplicationException(-1, s.msg, log.Location);
		//					}
		//				}
		//			}
		//			else
		//			{
		//				WM_Utility::PrintLog("库位推荐失败");
		//			}
		//		}
		//	}

		//}
#pragma endregion 

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

