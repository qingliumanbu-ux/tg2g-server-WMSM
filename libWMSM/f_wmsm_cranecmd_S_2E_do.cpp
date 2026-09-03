/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:      JQ
Version:     1.1.1
Date:        2016-11-23
Description: 板坯库发货命令生成函数
**************************************************/

//框架头文件
#include "WM_Utility.h"
//#include "twma7.h"
//#include "twm04.h"
//#include "twma2.h"
//#include "twma0.h"
//#include "twm00a8.h"

int f_wmsm_craneCmd_seq_update(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);                //更新命令流水号
int f_wmsm_cranecmd_update(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);

BM2_FUNCTION_EXPORT
int f_wmsm_cranecmd_S_2E_do(CString mat_no, EIClass * bcls_ret, CDbConnection * conn)
{
	/*程序内部变量*/
	int doFlag = 0;
	CString sqlstr = " ";
	CString dateTime = " ";
	int maxseqno = 999999999;

	/*业务变量*/
	int seqcmd = 0;
	CString shipping_stock_place = " ";
	CString shipping_stock_no = " ";

	/*定义表实体对象*/
	//CTWM00A8 twm00a8(conn);
	//CTWMA7   twma7(conn);
	//CTWMA0   twma0(conn);
	//CTWMA2   twma2(conn);
	//CTWM04   twm04(conn);
	//CTWM04   twm04_car(conn);
	CModel twm00a8 = CModel("TWM00A8");
	CModel twma7 = CModel("TWMA7");
	CModel twma0 = CModel("TWMA0");
	CModel twma2 = CModel("TWMA2");
	CModel twm04 = CModel("TWM04");
	CModel twm04_car = CModel("TWM04");

	/*数据存放块*/
	CDataTable dtCraneCmd;
	CDataTable up_dtMat;
	CDataTable dtMat;

	/*数据库操作类定义*/
	CDbCommand cmd_inq(conn);

	try
	{
		//项目自定义日志
		CTracer log(__FUNCTION__);

		//设置dtCraneCmd列名
		WM_Utility::SetDataTableColName("TWMA7", dtCraneCmd, conn);

		EIClass bcls_rec_update;
		bcls_rec_update.Tables[0].set_TableName("WM00_CMDUPDATE");
		bcls_rec_update.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
		bcls_rec_update.Tables[0].Columns.Add(DT_STRING, "STOCK_OPER_ORDER_NEW");
		bcls_rec_update.Tables[0].Columns.Add(DT_STRING, "STOCK_NO_TO");
		bcls_rec_update.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO_TO");
		bcls_rec_update.Tables[0].Columns.Add(DT_STRING, "VEHICLE_NO");
		bcls_rec_update.Tables[0].Rows.Add();

		twma0["MAT_NO"] = mat_no;
		twma2["MAT_NO"] = mat_no;
		Log::Trace("", __FUNCTION__, "传入材料：{0}", mat_no);
		if (!(twma0.Query("MAT_NO") &&
			twma0["STOCK_OPER_ORDER"].ToString() == "2E"))
		{
			Log::Trace("", __FUNCTION__, "材料不存在发货队列");
			return doFlag;
		}
		if (!twma2.Query("MAT_NO"))
		{
			Log::Trace("", __FUNCTION__, "材料不在库内");
			return doFlag;
		}
		twm04["STOCK_PLACE_NO"] = twma2["STOCK_PLACE_NO"].ToString();
		if (!twm04.Query("STOCK_PLACE_NO"))
		{
			Log::Trace("", __FUNCTION__, "材料所在库位不存在或为空");
			return doFlag;
		}

		if (twma0["TRANS_TOOL"].ToString().Trim() == "1")
		{
			Log::Trace("", __FUNCTION__, "运输工具为卡车");
			twm00a8["VEHICLE_NO"] = twma0["VEHICLE_NO"].ToString();
			if (!twm00a8.Query("VEHICLE_NO"))
			{
				Log::Trace("", __FUNCTION__, "没有发货位置");
				return doFlag;
			}
		}
		else if (twma0["TRANS_TOOL"].ToString().Trim() == "2")
		{
			Log::Trace("", __FUNCTION__, "运输工具为火车");
			twm00a8["VEHICLE_NO"] = twma0["VEHICLE_NO"].ToString();
			if (twm00a8.Query("VEHICLE_NO"))
			{
				shipping_stock_place = twm00a8["STOCK_PLACE_NO"].ToString();
				shipping_stock_no = twm00a8["STOCK_NO"].ToString();
			}
			else
			{
				Log::Trace("", __FUNCTION__, "没有发货位置");
				return doFlag;
			}
		}
		else
		{
			strcpy(s.msg, "Transportation tool is wrong.");
			throw CApplicationException(-1, s.msg, log.Location);
		}

#pragma region   写火车发货命令
		if (twma0["TRANS_TOOL"].ToString().Trim() == "2")
		{
			twma7.Reset();
			twma7["MAT_NO"] = mat_no;
			if (twma2["STOCK_NO"].ToString() == shipping_stock_no)
			{
				Log::Trace("", __FUNCTION__, "材料在当前库区");
				if (twma2["STOCK_PLACE_NO"].ToString() == shipping_stock_place)
				{
					Log::Trace("", __FUNCTION__, "材料已在火车上");
					return doFlag;
				}
				else
				{
					if (twma7.Query("MAT_NO"))
					{
						if (twma7["STOCK_OPER_ORDER"].ToString() == "2E")
						{
							Log::Trace("", __FUNCTION__, "材料已存在发货命令");
							return doFlag;
						}
						else
						{
							if (twma7["MAIN_MAT_NO"].ToString() == "1")
							{
								Log::Trace("", __FUNCTION__, "材料存在人工生成的命令,更新最终库位类型");
								twma7["REC_ERASE_TIME"] = dateTime;
								twma7["REC_ERASOR"] = s.userid;
								twma7["STOCK_PLACE_NO_FIN"] = shipping_stock_place;
								twma7["STOCK_OPER_ORDER_FIN"] = "2E";
								twma7.Update("REC_ERASE_TIME,REC_ERASOR,STOCK_PLACE_NO_FIN,STOCK_OPER_ORDER_FIN", "MAT_NO");
								return doFlag;
							}
							else
							{
								Log::Trace("", __FUNCTION__, "材料存在非人工生成，改为发货命令");
								bcls_rec_update.Tables[0].Rows[0]["MAT_NO"] = mat_no;
								bcls_rec_update.Tables[0].Rows[0]["STOCK_OPER_ORDER_NEW"] = "2E";
								bcls_rec_update.Tables[0].Rows[0]["STOCK_NO_TO"] = shipping_stock_no;
								bcls_rec_update.Tables[0].Rows[0]["STOCK_PLACE_NO_TO"] = shipping_stock_place;
								bcls_rec_update.Tables[0].Rows[0]["VEHICLE_NO"] = twma0["VEHICLE_NO"].ToString();
								doFlag = f_wmsm_cranecmd_update(&bcls_rec_update, bcls_ret, conn);
								if (doFlag != 0)
								{
									throw CApplicationException(-1, s.msg, log.Location);
								}
								return doFlag;
							}

						}
					}
					else
					{
						Log::Trace("", __FUNCTION__, "材料不存在命令，生成发货命令");
						dtCraneCmd.Rows.Add();
						dtCraneCmd.Rows[seqcmd]["MAT_NO"] = mat_no;
						dtCraneCmd.Rows[seqcmd]["STOCK_PLACE_NO_TO"] = shipping_stock_place;
						dtCraneCmd.Rows[seqcmd]["STOCK_PLACE_NO_FROM"] = twma2["STOCK_PLACE_NO"].ToString();
						dtCraneCmd.Rows[seqcmd]["YARD_LAYER_FROM"] = twma2["LAYERNO"].ToString();
						dtCraneCmd.Rows[seqcmd]["STOCK_OPER_ORDER_FIN"] = "2E";
						dtCraneCmd.Rows[seqcmd]["STOCK_OPER_ORDER"] = "2E";
						dtCraneCmd.Rows[seqcmd]["STOCK_PLACE_NO_FIN"] = shipping_stock_place;
						seqcmd++;
					}
				}
			}
			else
			{
				Log::Trace("", __FUNCTION__, "材料不在当前库区");
				if (twm04["DEV_DIV"].ToString().Trim() == "3"&&twm04["STOCK_PLACE_TYPE"].ToString().Trim() == "D")
				{
					if (twma7.Query("MAT_NO") && twma7["STOCK_PLACE_NO_TO"].ToString().Trim() != "")
					{
						Log::Trace("", __FUNCTION__, "材料在过跨台车上,且存在命令,更新最终库位类型");
						twma7["REC_ERASE_TIME"] = dateTime;
						twma7["REC_ERASOR"] = s.userid;
						twma7["STOCK_PLACE_NO_FIN"] = shipping_stock_place;
						twma7["STOCK_OPER_ORDER_FIN"] = "2E";
						twma7.Update("REC_ERASE_TIME,REC_ERASOR,STOCK_PLACE_NO_FIN,STOCK_OPER_ORDER_FIN", "MAT_NO");
						return doFlag;
					}
					else
					{
						if (twma7.Query("MAT_NO") && twma7["STOCK_PLACE_NO_TO"].ToString().Trim() == "")
						{
							twma7.Delete();
						}
						Log::Trace("", __FUNCTION__, "材料在过跨台车上,且不存在命令,生成过跨命令");
						twm04_car["STOCK_NO"] = shipping_stock_no;
						twm04_car["DEV_DIV"] = "3";
						twm04_car["STOCK_PLACE_TYPE"] = "D";
						twm04_car.Query("STOCK_NO,DEV_DIV,STOCK_PLACE_TYPE");
						dtCraneCmd.Rows.Add();
						dtCraneCmd.Rows[seqcmd]["MAT_NO"] = mat_no;
						dtCraneCmd.Rows[seqcmd]["STOCK_PLACE_NO_TO"] = twm04_car["STOCK_PLACE_NO"].ToString();
						dtCraneCmd.Rows[seqcmd]["STOCK_PLACE_NO_FROM"] = twma2["STOCK_PLACE_NO"].ToString();
						dtCraneCmd.Rows[seqcmd]["YARD_LAYER_FROM"] = twma2["LAYERNO"].ToDecimal();
						dtCraneCmd.Rows[seqcmd]["STOCK_OPER_ORDER_FIN"] = "2E";
						dtCraneCmd.Rows[seqcmd]["STOCK_OPER_ORDER"] = "32";
						dtCraneCmd.Rows[seqcmd]["STOCK_PLACE_NO_FIN"] = shipping_stock_place;
						seqcmd++;
					}
				}
				else
				{
					Log::Trace("", __FUNCTION__, "材料不在过跨台车上，生成过跨命令");
					dtCraneCmd.Rows.Add();
					dtCraneCmd.Rows[seqcmd]["MAT_NO"] = mat_no;
					dtCraneCmd.Rows[seqcmd]["STOCK_PLACE_NO_TO"] = "STC1";
					dtCraneCmd.Rows[seqcmd]["STOCK_PLACE_NO_FIN"] = shipping_stock_place;
					dtCraneCmd.Rows[seqcmd]["STOCK_PLACE_NO_FROM"] = twma2["STOCK_PLACE_NO"].ToString();
					dtCraneCmd.Rows[seqcmd]["YARD_LAYER_FROM"] = twma2["LAYERNO"].ToDecimal();
					dtCraneCmd.Rows[seqcmd]["STOCK_OPER_ORDER_FIN"] = "2E";
					dtCraneCmd.Rows[seqcmd]["STOCK_OPER_ORDER"] = "32";
					seqcmd++;
				}
			}
		}


#pragma endregion

#pragma region   写卡车发货命令
		if (twma0["TRANS_TOOL"].ToString().Trim() == "1")
		{
			twma7["MAT_NO"] = mat_no;
			if (twm04["STOCK_PLACE_TYPE"].ToString().Trim() == "D" &&
				twm04["DEV_DIV"].ToString().Trim() == "1")
			{
				Log::Trace("", __FUNCTION__, "材料已在卡车上");
				return doFlag;
			}
			twm04_car["STOCK_NO"] = twma2["STOCK_NO"].ToString();
			twm04_car["DEV_DIV"] = "1";
			twm04_car["STOCK_PLACE_TYPE"] = "D";
			twm04_car.Query("STOCK_NO,DEV_DIV,STOCK_PLACE_TYPE");
			if (twma7.Query("MAT_NO"))
			{
				if (twma7["STOCK_OPER_ORDER"].ToString().Trim() == "2E")
				{
					Log::Trace("", __FUNCTION__, "材料存在发货命令");
					return doFlag;
				}
				else
				{
					if (twma7["MAIN_MAT_NO"].ToString() == "1")
					{
						Log::Trace("", __FUNCTION__, "材料存在人工生成的命令,更新最终库位类型");
						twma7["REC_ERASE_TIME"] = dateTime;
						twma7["REC_ERASOR"] = s.userid;
						twma7["STOCK_PLACE_NO_FIN"] = twm04["STOCK_PLACE_NO"].ToString();
						twma7["STOCK_OPER_ORDER_FIN"] = "2E";
						twma7.Update("REC_ERASE_TIME,REC_ERASOR,STOCK_PLACE_NO_FIN,STOCK_OPER_ORDER_FIN", "MAT_NO");
					}
					else
					{
						Log::Trace("", __FUNCTION__, "材料存在非人工生成，修改为发货命令");
						bcls_rec_update.Tables[0].Rows[0]["MAT_NO"] = mat_no;
						bcls_rec_update.Tables[0].Rows[0]["STOCK_OPER_ORDER_NEW"] = "2E";
						bcls_rec_update.Tables[0].Rows[0]["STOCK_NO_TO"] = twm04_car["STOCK_NO"];
						bcls_rec_update.Tables[0].Rows[0]["STOCK_PLACE_NO_TO"] = twm04_car["STOCK_PLACE_NO"];
						bcls_rec_update.Tables[0].Rows[0]["VEHICLE_NO"] = twma0["VEHICLE_NO"];
						doFlag = f_wmsm_cranecmd_update(&bcls_rec_update, bcls_ret, conn);
						if (doFlag != 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}
					}
				}
			}
			else
			{
				Log::Trace("", __FUNCTION__, "材料不存在命令，生成发货命令");
				dtCraneCmd.Rows.Add();
				dtCraneCmd.Rows[seqcmd]["MAT_NO"] = mat_no;
				dtCraneCmd.Rows[seqcmd]["STOCK_PLACE_NO_TO"] = twm04_car["STOCK_PLACE_NO"].ToString();
				dtCraneCmd.Rows[seqcmd]["STOCK_PLACE_NO_FROM"] = twma2["STOCK_PLACE_NO"].ToString();
				dtCraneCmd.Rows[seqcmd]["YARD_LAYER_FROM"] = twma2["LAYERNO"].ToDecimal();
				dtCraneCmd.Rows[seqcmd]["STOCK_OPER_ORDER_FIN"] = "2E";
				dtCraneCmd.Rows[seqcmd]["STOCK_OPER_ORDER"] = "2E";
				dtCraneCmd.Rows[seqcmd]["STOCK_PLACE_NO_FIN"] = twm04_car["STOCK_PLACE_NO"].ToString();
				seqcmd++;
			}
		}

#pragma endregion

#pragma region   生成发货命令
		for (int i = 0; i < dtCraneCmd.Rows.get_Count(); i++)
		{
			sqlstr = "select mat_no,stock_place_no,layerno from twma2 a where stock_place_no='" + dtCraneCmd.Rows[i]["STOCK_PLACE_NO_FROM"].ToString() + "'and layerno>='" + dtCraneCmd.Rows[i]["YARD_LAYER_FROM"].ToString() + "'and a.mat_no not in (select mat_no from twma7 b where a.mat_no = b.mat_no) order by a.layerno desc";
			Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteQuery(up_dtMat);
			cmd_inq.Close();
			Log::Trace("", __FUNCTION__, "材料【{0}】上层个数：{1}", dtCraneCmd.Rows[i]["MAT_NO"].ToString(), up_dtMat.Rows.get_Count() - 1);
			for (int j = 0; j < up_dtMat.Rows.get_Count(); j++)
			{
				twma7.Reset();
				dtMat.Rows.Clear();
				twma7["MAT_NO"] = up_dtMat.Rows[j]["MAT_NO"].ToString();
				Log::Trace("", __FUNCTION__, "生成材料{0}命令", twma7["MAT_NO"].ToString());

				sqlstr = "SELECT a.mat_no,a.stock_no,a.hall_no,a.stock_place_no,a.layerno,b.mat_kind,"
					"b.mat_shape_flag,b.mat_act_thick,b.mat_act_width,b.mat_act_len,b.mat_act_wt "
					" FROM twma2 a,twma1 b WHERE a.mat_no = '" + twma7["MAT_NO"].ToString() + "' AND a.mat_no = b.mat_no";
				Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(dtMat);
				cmd_inq.Close();
				if (dtMat.Rows.get_Count() == 0)
				{
					sprintf(s.msg, "Material [%s] is not in yard.", (const char*)mat_no);
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				//新增行车命令数据表行				
				twma7["REC_CREATOR"] = s.userid;
				twma7["REC_CREATE_TIME"] = dateTime;
				twma7["CMD_SEQ"] = atoi(WM_Utility::GetSeqence("seqTest", conn));
				if (twma7["CMD_SEQ"].ToDecimal() > maxseqno)
				{
					doFlag = f_wmsm_craneCmd_seq_update(bcls_ret, bcls_ret, conn);
					if (doFlag != 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
					twma7["CMD_SEQ"] = atoi(WM_Utility::GetSeqence("seqTest", conn));
				}

				twma7["CRANE_INST_STATUS"] = "0";
				twma7["CRANE_INST_CODE"] = dtCraneCmd.Rows[i]["STOCK_OPER_ORDER"].ToString().Substring(0, 1);

				Log::Trace("", __FUNCTION__, "写材料源库位信息");
				twma7["STOCK_NO"] = dtMat.Rows[0]["STOCK_NO"];
				twma7["STOCK_NO_FIN"] = dtMat.Rows[0]["STOCK_NO"];
				twma7["STOCK_NO_FROM"] = dtMat.Rows[0]["STOCK_NO"];
				twma7["HALL_NO_FR"] = dtMat.Rows[0]["HALL_NO"];
				twma7["STOCK_PLACE_NO_FROM"] = dtMat.Rows[0]["STOCK_PLACE_NO"];
				twma7["YARD_LAYER_FROM"] = dtMat.Rows[0]["LAYERNO"];

				Log::Trace("", __FUNCTION__, "写材料信息");
				twma7["MAT_KIND"] = dtMat.Rows[0]["MAT_KIND"];
				twma7["MAT_SHAPE_FLAG"] = dtMat.Rows[0]["MAT_SHAPE_FLAG"];
				twma7["MAT_ACT_THICK"] = dtMat.Rows[0]["MAT_ACT_THICK"];
				twma7["MAT_ACT_WIDTH"] = dtMat.Rows[0]["MAT_ACT_WIDTH"];
				twma7["MAT_ACT_LEN"] = dtMat.Rows[0]["MAT_ACT_LEN"];
				twma7["MAT_ACT_WT"] = dtMat.Rows[0]["MAT_ACT_WT"];


				Log::Trace("", __FUNCTION__, "写目标库位信息");
				if (j == up_dtMat.Rows.get_Count() - 1)
				{
					twma7["STOCK_PLACE_NO_TO"] = dtCraneCmd.Rows[i]["STOCK_PLACE_NO_TO"].ToString();
					twma7["STOCK_OPER_ORDER"] = dtCraneCmd.Rows[i]["STOCK_OPER_ORDER"].ToString();
					twma7["STOCK_OPER_ORDER_FIN"] = "2E";
					twma7["VEHICLE_NO"] = twma0["VEHICLE_NO"].ToString();
					twma7["STOCK_PLACE_NO_FIN"] = dtCraneCmd.Rows[i]["STOCK_PLACE_NO_FIN"].ToString();
				}
				else
				{
					twma7["STOCK_PLACE_NO_TO"] = " ";
					twma7["STOCK_OPER_ORDER"] = "31";
				}
				twma7["MOVE_TYPE"] = twma7["STOCK_OPER_ORDER"].ToString();
				if (twma7.QueryCount("MAT_NO") > 0)
				{
					Log::Trace("", __FUNCTION__, "材料{0}有命令", twma7["MAT_NO"].ToString());
				}
				else
				{
					twma7.Insert();
				}

			}
		}
#pragma endregion

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
	return doFlag;
}
