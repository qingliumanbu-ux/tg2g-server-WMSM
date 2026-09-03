/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:      JQ
Version:     1.1.1
Date:        2016-11-23
Description: 板坯库备料命令生成函数
**************************************************/

//框架头文件
#include "WM_Utility.h"
//#include "twma7.h"
//#include "twm04.h"
//#include "twma2.h"
//#include "twm000m.h"
//#include "twm000f.h"

BM2_FUNCTION_IMPORT
int f_wm00_pileinfocal(CString stock_no, CString stock_place_no, EIClass * bcls_ret, CDbConnection * conn);  //垛位最大高度、重量修正
int f_wmsm_craneCmd_seq_update(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);                //更新命令流水号
int f_wmsm_craneCmd_layer_update(CString stock_place_no, CString vehicle_no, CString flag, EIClass * bcls_ret, CDbConnection * conn);
int f_wmsm_cranecmd_update(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);


BM2_FUNCTION_EXPORT
int f_wmsm_craneCmd_S_2C_do(CString stock_place_no_fin, EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	/*程序内部变量*/
	int doFlag = 0;
	int seq = 0;
	int de_flag = 0;
	int de_count = 0;
	CString sqlstr = " ";
	CString dateTime = "";                     //系统时间

	/*数据存放块*/
	CDataTable cmd_mat;                        //存放材料
	CDataTable up_dtMat;                       //存放材料
	CDataTable dtMat;                          //存放材料
	CDataTable dtMa1t;                          //存放材料
	CDataTable dtstock_place_no;

	/*数据库操作类定义*/
	CDbCommand cmd_inq_do(conn);
	CDbCommand cmd_inq_do1(conn);

	/*业务变量*/
	CString stock_place_no_to = " ";
	CString stock_place_no = " ";
	CString stock_no_to = " ";
	int maxseqno = 999999999;
	CDecimal cmd_seq = 0;

	/*定义表实体对象*/
	//CTWMA7 twma7(conn);
	//CTWMA2 twma2(conn);
	//CTWM04 twm04(conn);
	//CTWM000M twm000m(conn);
	//CTWM000F twm000f(conn);
	//CTWM04 twm04_cars(conn);
	//CTWM04 twm04_tem(conn);
	CModel twma7 = CModel("TWMA7");
	CModel twma2 = CModel("TWMA2");
	CModel twm04 = CModel("TWM04");
	CModel twm000m = CModel("TWM000M");
	CModel twm000f = CModel("TWM000F");
	CModel twm04_cars = CModel("TWM04");
	CModel twm04_tem = CModel("TWM04");

	/* Database operations class definition */
	CDbCommand cmd_inq(conn);


	try
	{
		//项目自定义日志
		CTracer log(__FUNCTION__);

		//取系统时间
		dateTime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		if (stock_place_no_fin.Trim() == "")
		{
			strcpy(s.msg, "Incoming position is empty."); //传入库位号位空
			throw CApplicationException(-1, s.msg, log.Location);
		}
		Log::Trace("", __FUNCTION__, "传入库位号[{0}]", stock_place_no_fin);

		twm04["STOCK_PLACE_NO"] = stock_place_no_fin;
		if (!twm04.Query("STOCK_PLACE_NO"))
		{
			strcpy(s.msg, "Incoming position no. is not exist."); //传入库位号位不存在
			throw CApplicationException(-1, s.msg, log.Location);
		}

		dtstock_place_no.Columns.Add(DT_STRING, "STOCK_PLACE_NO");

		EIClass bcls_rec_update;
		bcls_rec_update.Tables[0].set_TableName("WM00_CMDUPDATE");
		bcls_rec_update.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
		bcls_rec_update.Tables[0].Columns.Add(DT_STRING, "STOCK_OPER_ORDER_NEW");
		bcls_rec_update.Tables[0].Columns.Add(DT_STRING, "STOCK_OPER_ORDER_FIN");
		bcls_rec_update.Tables[0].Columns.Add(DT_STRING, "STOCK_NO_TO");
		bcls_rec_update.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO_TO");
		bcls_rec_update.Tables[0].Columns.Add(DT_STRING, "UNIT_CODE");
		bcls_rec_update.Tables[0].Rows.Add();

		//获取材料
		sqlstr = "select * from twm000f where stock_place_no_fin='" + stock_place_no_fin + "' order by plan_exec_seq_no desc";
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq_do.SetCommandText(sqlstr);
		cmd_inq_do.ExecuteQuery(cmd_mat);
		cmd_inq_do.Close();

		sqlstr = " delete from twma7 where crane_inst_status = '0' and stock_place_no_fin = '" + stock_place_no_fin + "'";
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq_do1.SetCommandText(sqlstr);
		cmd_inq_do1.ExecuteNonQuery();
		cmd_inq_do1.Close();

		for (int i = 0; i < cmd_mat.Rows.get_Count(); i++)
		{
			Log::Trace("", __FUNCTION__, "#######写材料{0}的吊车命令########", cmd_mat.Rows[i]["MAT_NO"].ToString());
			twma2["MAT_NO"] = cmd_mat.Rows[i]["MAT_NO"].ToString();
			twma7["MAT_NO"] = cmd_mat.Rows[i]["MAT_NO"].ToString();
			twma2.Query("MAT_NO");
			cmd_mat.Rows[i]["STOCK_NO"] = twma2["STOCK_NO"].ToString();
			cmd_mat.Rows[i]["STOCK_PLACE_NO"] = twma2["STOCK_PLACE_NO"].ToString();
			cmd_mat.Rows[i]["YARD_LAYER_FROM"] = twma2["LAYERNO"].ToDecimal();
			twm04_tem["STOCK_PLACE_NO"] = twma2["STOCK_PLACE_NO"].ToString();
			twm04_tem.Query("STOCK_PLACE_NO");
			if (twm04_tem["ENTRANCE_EXIT_DIV"].ToString() == "1")
			{
				Log::Trace("", __FUNCTION__, "entrance");
				if (twma7.Query("MAT_NO"))
				{
					twma7.Delete();
				}
				de_count++;
				continue;
			}

			if (twma7.Query("MAT_NO") &&
				twma7["STOCK_OPER_ORDER"].ToString() == "2C")
			{
				return doFlag;
			}
			twma7.Reset();
			twma7["MAT_NO"] = cmd_mat.Rows[i]["MAT_NO"].ToString();

			if (twma2["STOCK_NO"].ToString() == "S02")
			{
				Log::Trace("", __FUNCTION__, "和目的业务类型同库区，判断所在库位类型");
				if (twm04_tem["STOCK_PLACE_TYPE"].ToString() == "7")
				{
					Log::Trace("", __FUNCTION__, "材料在备料区，跳过");
					de_flag = 1;
					continue;
				}
				else if (twm04_tem["STOCK_PLACE_TYPE"].ToString() == "D" &&
					(twm04_tem["DEV_DIV"].ToString() == "1" ||
					twm04_tem["DEV_DIV"].ToString() == "2" ||
					twm04_tem["DEV_DIV"].ToString() == "3"))
				{
					Log::Trace("", __FUNCTION__, "材料在过跨台车、火车或卡车上，判断是否为计划中的第一个材料");
					if (i == 0 || de_flag == 1)
					{
						Log::Trace("", __FUNCTION__, "第一个材料不存在命令huo材料在备料区，生成备料命令");
						cmd_mat.Rows[i]["STOCK_PLACE_NO_TO"] = stock_place_no_fin;
						cmd_mat.Rows[i]["STOCK_NO_TO"] = "S02";
						cmd_mat.Rows[i]["MOVE_TYPE"] = "2C";
						cmd_mat.Rows[i]["DEPILER_FLAG"] = "1";
						if (de_flag == 1)
						{
							de_flag = 0;
						}
						continue;
					}
					else
					{
						Log::Trace("", __FUNCTION__, "不是第一个材料，判断上一个材料是否已生成备料命令");
						if (cmd_mat.Rows[i - 1]["DEPILER_FLAG"].ToString().Trim() == "1")
						{
							Log::Trace("", __FUNCTION__, "上一个材料已生成备料命令，判断是否和上一个材料在一个跺位上");
							if (cmd_mat.Rows[i]["STOCK_PLACE_NO"].ToString() == cmd_mat.Rows[i - 1]["STOCK_PLACE_NO"].ToString())
							{
								Log::Trace("", __FUNCTION__, "和上一个材料在一个跺位上,判断是否满足组吊条件");
								if (cmd_mat.Rows[i]["YARD_LAYER_FROM"].ToDecimal() - 1 == cmd_mat.Rows[i - 1]["YARD_LAYER_FROM"].ToDecimal()
									&& cmd_mat.Rows[i]["GROUP_NUM"].ToDecimal() < 3)
								{
									Log::Trace("", __FUNCTION__, "满足组吊条件，生成组吊备料命令");
									cmd_mat.Rows[i]["STOCK_PLACE_NO_TO"] = stock_place_no_fin;
									cmd_mat.Rows[i]["STOCK_NO_TO"] = "S02";
									cmd_mat.Rows[i]["MOVE_TYPE"] = "2C";
									cmd_mat.Rows[i]["DEPILER_FLAG"] = "1";
									for (int t = i; t > 0; t--)
									{
										if (cmd_mat.Rows[i - 1]["GROUP_FLAG"].ToString() != "1")
										{
											cmd_mat.Rows[i]["GROUP_NUM"] = 2;
											cmd_mat.Rows[i]["GROUP_NO"] = cmd_mat.Rows[i - 1]["PLAN_EXEC_SEQ_NO"].ToString();
											cmd_mat.Rows[i]["GROUP_FLAG"] = "1";
											cmd_mat.Rows[i - 1]["GROUP_NUM"] = 2;
											cmd_mat.Rows[i - 1]["GROUP_NO"] = cmd_mat.Rows[i - 1]["PLAN_EXEC_SEQ_NO"].ToString();
											cmd_mat.Rows[i - 1]["GROUP_FLAG"] = "1";
											continue;
										}
										else
										{
											cmd_mat.Rows[i]["GROUP_NUM"] = cmd_mat.Rows[i - 1]["GROUP_NUM"].ToDecimal() + 1;
											cmd_mat.Rows[i]["GROUP_NO"] = cmd_mat.Rows[i - 1]["GROUP_NO"].ToString();
											cmd_mat.Rows[i]["GROUP_FLAG"] = "1";
											if (cmd_mat.Rows[t - 1]["GROUP_NO"].ToString() == cmd_mat.Rows[i - 1]["GROUP_NO"].ToString())
											{
												cmd_mat.Rows[t - 1]["GROUP_NUM"] = cmd_mat.Rows[t - 1]["GROUP_NUM"].ToDecimal() + 1;
												continue;
											}
											else
											{
												break;
											}
										}
									}
								}
								else
								{
									if (twma7.Query("MAT_NO"))
									{
										Log::Trace("", __FUNCTION__, "不满足组吊条件，且存在命令，替换为入库命令");
										bcls_rec_update.Tables[0].Rows[0]["MAT_NO"] = cmd_mat.Rows[i]["MAT_NO"].ToString();
										bcls_rec_update.Tables[0].Rows[0]["STOCK_OPER_ORDER_NEW"] = "1B";
										bcls_rec_update.Tables[0].Rows[0]["STOCK_NO_TO"] = twma2["STOCK_NO"].ToString();
										bcls_rec_update.Tables[0].Rows[0]["STOCK_PLACE_NO_TO"] = " ";
										doFlag = f_wmsm_cranecmd_update(&bcls_rec_update, bcls_ret, conn);
										if (doFlag != 0)
										{
											throw CApplicationException(-1, s.msg, log.Location);
										}
									}
									else
									{
										Log::Trace("", __FUNCTION__, "不满足组吊条件，且不存在命令，生成入库命令");
										cmd_mat.Rows[i]["STOCK_PLACE_NO_TO"] = "recommend";
										cmd_mat.Rows[i]["STOCK_NO_TO"] = "S02";
										cmd_mat.Rows[i]["MOVE_TYPE"] = "32";
										continue;
									}
								}
							}
							else
							{
								Log::Trace("", __FUNCTION__, "和上一个材料在不一个跺位上,生成到S02正常库位的命令");
								cmd_mat.Rows[i]["STOCK_PLACE_NO_TO"] = "recommend";
								cmd_mat.Rows[i]["STOCK_NO_TO"] = "S02";
								cmd_mat.Rows[i]["MOVE_TYPE"] = "32";
								continue;
							}
						}
						else
						{
							Log::Trace("", __FUNCTION__, "上一个材料未生成备料命令，判断材料库位类型");
							if (twm04_tem["STOCK_PLACE_TYPE"].ToString() == "D" &&
								twm04_tem["DEV_DIV"].ToString() == "3")
							{
								Log::Trace("", __FUNCTION__, "材料在过跨台车上，生成到S02正常库位的命令");
								cmd_mat.Rows[i]["STOCK_PLACE_NO_TO"] = "recommend";
								cmd_mat.Rows[i]["STOCK_NO_TO"] = "S02";
								cmd_mat.Rows[i]["MOVE_TYPE"] = "32";
							}
							else
							{
								Log::Trace("", __FUNCTION__, "材料在火车或卡车上，生成入库命令");
								cmd_mat.Rows[i]["STOCK_PLACE_NO_TO"] = "recommend";
								cmd_mat.Rows[i]["STOCK_NO_TO"] = "S02";
								cmd_mat.Rows[i]["MOVE_TYPE"] = "1B";
							}
							continue;
						}
					}
				}
				else
				{
					Log::Trace("", __FUNCTION__, "材料{0}在S02库区的正常库位", cmd_mat.Rows[i]["MAT_NO"].ToString());
					if (i == 0 || de_flag == 1)
					{
						Log::Trace("", __FUNCTION__, "第一个材料，材料不存在命令，生成备料命令");
						cmd_mat.Rows[i]["STOCK_PLACE_NO_TO"] = stock_place_no_fin;
						cmd_mat.Rows[i]["STOCK_NO_TO"] = "S02";
						cmd_mat.Rows[i]["MOVE_TYPE"] = "2C";
						cmd_mat.Rows[i]["DEPILER_FLAG"] = "1";
						if (de_flag == 1)
						{
							de_flag = 0;
						}
						continue;
					}
					else
					{
						if (cmd_mat.Rows[i - 1]["DEPILER_FLAG"].ToString().Trim() == "1")
						{
							Log::Trace("", __FUNCTION__, "上一个材料已生成备料命令，判断是否和上一个材料在一个跺位上");
							if (cmd_mat.Rows[i]["STOCK_PLACE_NO"].ToString() == cmd_mat.Rows[i - 1]["STOCK_PLACE_NO"].ToString())
							{
								Log::Trace("", __FUNCTION__, "和上一个材料在一个跺位上,判断是否满足组吊条件");
								Log::Trace("", __FUNCTION__, "{0}", cmd_mat.Rows[i - 1]["GROUP_NUM"].ToDecimal());
								if (cmd_mat.Rows[i]["YARD_LAYER_FROM"].ToDecimal() - 1 == cmd_mat.Rows[i - 1]["YARD_LAYER_FROM"].ToDecimal()
									&& cmd_mat.Rows[i - 1]["GROUP_NUM"].ToDecimal() < 3)
								{
									Log::Trace("", __FUNCTION__, "满足组吊条件，生成组吊备料命令");
									cmd_mat.Rows[i]["STOCK_PLACE_NO_TO"] = stock_place_no_fin;
									cmd_mat.Rows[i]["STOCK_NO_TO"] = "S02";
									cmd_mat.Rows[i]["MOVE_TYPE"] = "2C";
									cmd_mat.Rows[i]["DEPILER_FLAG"] = "1";
									for (int t = i; t > 0; t--)
									{
										if (cmd_mat.Rows[i - 1]["GROUP_FLAG"].ToString() != "1")
										{
											cmd_mat.Rows[i]["GROUP_NUM"] = 2;
											cmd_mat.Rows[i]["GROUP_NO"] = cmd_mat.Rows[i - 1]["PLAN_EXEC_SEQ_NO"].ToString();
											cmd_mat.Rows[i]["GROUP_FLAG"] = "1";
											cmd_mat.Rows[i - 1]["GROUP_NUM"] = 2;
											cmd_mat.Rows[i - 1]["GROUP_NO"] = cmd_mat.Rows[i - 1]["PLAN_EXEC_SEQ_NO"].ToString();
											cmd_mat.Rows[i - 1]["GROUP_FLAG"] = "1";
											break;
										}
										else
										{
											cmd_mat.Rows[i]["GROUP_NUM"] = cmd_mat.Rows[i - 1]["GROUP_NUM"].ToDecimal() + 1;
											cmd_mat.Rows[i]["GROUP_NO"] = cmd_mat.Rows[i - 1]["GROUP_NO"].ToString();
											cmd_mat.Rows[i]["GROUP_FLAG"] = "1";
											if (cmd_mat.Rows[t]["GROUP_NO"].ToString() == cmd_mat.Rows[i - 1]["GROUP_NO"].ToString())
											{
												cmd_mat.Rows[t]["GROUP_NUM"] = cmd_mat.Rows[t - 1]["GROUP_NUM"].ToDecimal() + 1;
												continue;
											}
											else
											{
												break;
											}
										}
									}
								}
								else
								{
									Log::Trace("", __FUNCTION__, "不满足组吊条件，不生成命令，跳过");
									continue;
								}
							}
							else
							{
								Log::Trace("", __FUNCTION__, "和上一个材料在不一个跺位上,不生成命令，跳过");
								continue;
							}
						}
						else
						{
							Log::Trace("", __FUNCTION__, "上一个材料未生成备料命令，跳过");
							continue;
						}
					}
				}
			}
			else
			{
				Log::Trace("", __FUNCTION__, "和目的业务类型不同库区，判断材料是否在过跨台车上。");
				if (twm04_tem["STOCK_PLACE_TYPE"].ToString() == "D" &&
					twm04_tem["DEV_DIV"].ToString() == "3")
				{
					twm04_cars["STOCK_NO"].ToString() = "S02";
					twm04_cars["STOCK_PLACE_TYPE"].ToString() = "D";
					twm04_cars["DEV_DIV"].ToString() = "3";
					twm04_cars.Query("STOCK_NO,STOCK_PLACE_TYPE,DEV_DIV");
					if (twma7.Query("MAT_NO") &&
						twma7["STOCK_PLACE_NO_TO"].ToString().Trim() != "")
					{
						Log::Trace("", __FUNCTION__, "材料在过跨台车上,且存在命令,替换为备料命令");
						bcls_rec_update.Tables[0].Rows[0]["MAT_NO"] = cmd_mat.Rows[i]["MAT_NO"].ToString();
						bcls_rec_update.Tables[0].Rows[0]["STOCK_OPER_ORDER_NEW"] = "32";
						bcls_rec_update.Tables[0].Rows[0]["STOCK_NO_TO"] = "S02";
						bcls_rec_update.Tables[0].Rows[0]["STOCK_OPER_ORDER_FIN"] = "2C";
						bcls_rec_update.Tables[0].Rows[0]["STOCK_PLACE_NO_TO"] = twm04_cars["STOCK_PLACE_NO"].ToString();
						bcls_rec_update.Tables[0].Rows[0]["UNIT_CODE"] = "C201";
						doFlag = f_wmsm_cranecmd_update(&bcls_rec_update, bcls_ret, conn);
						if (doFlag != 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}
						continue;
					}
					else
					{
						if (twma7.Query("MAT_NO") &&
							twma7["STOCK_PLACE_NO_TO"].ToString().Trim() == "")
						{
							twma7.Delete();
						}
						cmd_mat.Rows[i]["STOCK_PLACE_NO_TO"] = twm04_cars["STOCK_PLACE_NO"].ToString();
						cmd_mat.Rows[i]["STOCK_NO_TO"] = "S02";
						cmd_mat.Rows[i]["MOVE_TYPE"] = "32";
						Log::Trace("", __FUNCTION__, "材料在过跨台车上,生成材料{0}到过跨台车{1}的命令", cmd_mat.Rows[i]["MAT_NO"].ToString(), twm04_cars["STOCK_PLACE_NO"].ToString());
						continue;
					}
				}
				else
				{
					Log::Trace("", __FUNCTION__, "材料不在过跨台车上");
					if (twma7.QueryCount("MAT_NO") > 0)
					{
						twm04_cars["STOCK_NO"] = twma2["STOCK_NO"].ToString();
						twm04_cars["STOCK_PLACE_TYPE"] = "D";
						twm04_cars["DEV_DIV"] = "3";
						twm04_cars.Query("STOCK_NO,STOCK_PLACE_TYPE,DEV_DIV");
						Log::Trace("", __FUNCTION__, "材料已存在命令，更新为上料过跨命令");
						stock_place_no_to = twma7["STOCK_PLACE_NO_TO"].ToString();
						stock_no_to = twma7["STOCK_NO"].ToString();
						twma7["STOCK_PLACE_NO_FIN"] = stock_place_no_fin;
						twma7["STOCK_OPER_ORDER_FIN"] = "2C";
						twma7["STOCK_OPER_ORDER"] = "32";
						twma7["UNIT_CODE"] = "C201";
						twma7["STOCK_NO_FIN"] = "S02";
						twma7["STOCK_PLACE_NO_TO"] = twm04_cars["STOCK_PLACE_NO"].ToString();
						twma7["CRANE_CMDGRPNO"] = 0;
						twma7["STOCK_PLACE_NO_FROM"] = twma2["STOCK_PLACE_NO"].ToString();
						twma7["STOCK_NO_FROM"] = twma2["STOCK_NO"].ToString();
						twma7["YARD_LAYER_FROM"] = twma2["LAYERNO"].ToDecimal();
						twma7.Update("YARD_LAYER_FROM,STOCK_OPER_ORDER,STOCK_PLACE_NO_FROM,STOCK_PLACE_NO_TO,STOCK_NO_FROM,STOCK_PLACE_NO_FIN,STOCK_OPER_ORDER_FIN,UNIT_CODE,STOCK_NO_FIN,CRANE_CMDGRPNO", "MAT_NO");
						doFlag = f_wm00_pileinfocal(stock_no_to, stock_place_no_to, bcls_ret, conn);
						if (doFlag != 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}
						continue;
					}
					else
					{
						twm04_cars["STOCK_NO"] = twm04_tem["STOCK_NO"].ToString();
						twm04_cars["STOCK_PLACE_TYPE"] = "D";
						twm04_cars["DEV_DIV"] = "3";
						twm04_cars.Query("STOCK_NO,STOCK_PLACE_TYPE,DEV_DIV");
						cmd_mat.Rows[i]["STOCK_PLACE_NO_TO"] = twm04_cars["STOCK_PLACE_NO"].ToString();
						cmd_mat.Rows[i]["STOCK_NO_TO"] = "S01";
						cmd_mat.Rows[i]["MOVE_TYPE"] = "32";
						Log::Trace("", __FUNCTION__, "生成材料{0}到过跨台车{1}的命令", cmd_mat.Rows[i]["MAT_NO"].ToString(), twm04_cars["STOCK_PLACE_NO"].ToString());
						continue;
					}
					break;
				}
			}
		}

		Log::Trace("", __FUNCTION__, "---------------生成材料----------------");
		for (int i = cmd_mat.Rows.get_Count() - 1; i >= 0; i--)
		{
			Log::Trace("", __FUNCTION__, "生成材料{0}", cmd_mat.Rows[i]["MAT_NO"].ToString());
			if (cmd_mat.Rows[i]["STOCK_PLACE_NO_TO"].ToString().Trim() != "")
			{
				Log::Trace("", __FUNCTION__, "生成材料{0}命令", cmd_mat.Rows[i]["MAT_NO"].ToString());
				if (cmd_mat.Rows[i]["STOCK_PLACE_NO_TO"].ToString().Trim() == "recommend")
				{
					cmd_mat.Rows[i]["STOCK_PLACE_NO_TO"] = " ";
				}
				twma2["MAT_NO"] = cmd_mat.Rows[i]["MAT_NO"].ToString();
				twma2.Query("MAT_NO");

				dtstock_place_no.Rows.Add();
				dtstock_place_no.Rows[seq]["STOCK_PLACE_NO"] = twma2["STOCK_PLACE_NO"].ToString();
				seq++;

				sqlstr =
					"select mat_no,stock_place_no,layerno from twma2 a where stock_place_no='" +
					twma2["STOCK_PLACE_NO"].ToString() +
					"'and layerno>='" + twma2["LAYERNO"].ToString() +
					"'and a.mat_no not in (select mat_no from twma7 b where a.mat_no = b.mat_no and a.mat_no!='" +
					cmd_mat.Rows[i]["MAT_NO"].ToString() + "') order by a.layerno desc";
				Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
				cmd_inq_do.SetCommandText(sqlstr);
				cmd_inq_do.ExecuteQuery(up_dtMat);
				cmd_inq_do.Close();
				Log::Trace("", __FUNCTION__, "材料{0}上层个数：{1}", cmd_mat.Rows[i]["MAT_NO"].ToString(), up_dtMat.Rows.get_Count() - 1);
				for (int j = 0; j < up_dtMat.Rows.get_Count(); j++)
				{
					dtMat.Rows.Clear();
					Log::Trace("", __FUNCTION__, "生成材料{0}命令", up_dtMat.Rows[j]["MAT_NO"].ToString());

					sqlstr = "SELECT a.mat_no,a.stock_no,a.hall_no,a.stock_place_no,a.layerno,b.mat_kind,"
						"b.mat_shape_flag,b.mat_act_thick,b.mat_act_width,b.mat_act_len,b.mat_act_wt "
						" FROM twma2 a,twma1 b WHERE a.mat_no = '" + up_dtMat.Rows[j]["MAT_NO"].ToString() + "' AND a.mat_no = b.mat_no";
					Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
					cmd_inq_do.SetCommandText(sqlstr);
					cmd_inq_do.ExecuteQuery(dtMat);
					cmd_inq_do.Close();
					if (dtMat.Rows.get_Count() == 0)
					{
						sprintf(s.msg, "Material[%s] is not in yard.", (const char*)cmd_mat.Rows[i]["MAT_NO"].ToString());
						throw CApplicationException(-1, s.msg, s.svc_name);
					}

					//新增行车命令数据表行
					twma7.Reset();
					twma7["REC_CREATOR"] = s.userid;
					twma7["REC_CREATE_TIME"] = dateTime;
					twma7["MAT_NO"] = up_dtMat.Rows[j]["MAT_NO"].ToString();


					twma7["CRANE_INST_STATUS"] = "0";
					twma7["CRANE_INST_CODE"] = "2";

					Log::Trace("", __FUNCTION__, "写材料源库位信息");
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
						twma7["STOCK_NO"] = cmd_mat.Rows[i]["STOCK_NO_TO"].ToString();
						twma7["STOCK_NO_FROM"] = cmd_mat.Rows[i]["STOCK_NO_TO"].ToString();
						twma7["STOCK_PLACE_NO_TO"] = cmd_mat.Rows[i]["STOCK_PLACE_NO_TO"].ToString();
						twma7["STOCK_OPER_ORDER"] = cmd_mat.Rows[i]["MOVE_TYPE"];
						twma7["STOCK_OPER_ORDER_FIN"] = "2C";
						twma7["STOCK_PLACE_NO_FIN"] = cmd_mat.Rows[i]["STOCK_PLACE_NO_FIN"].ToString();
						twma7["UNIT_CODE"] = "C201";
						twma7["STOCK_NO_FIN"] = "S02";
						twma7["CRANE_CMDGRPNO"] = cmd_mat.Rows[i]["GROUP_NO"].ToDecimal();
						twma7["MOVE_TYPE"] = cmd_mat.Rows[i]["MOVE_TYPE"];
					}
					else
					{
						twma7["STOCK_NO"] = dtMat.Rows[0]["STOCK_NO"];
						twma7["STOCK_NO_FROM"] = dtMat.Rows[0]["STOCK_NO"];
						twma7["STOCK_PLACE_NO_TO"] = " ";
						twma7["STOCK_OPER_ORDER"] = "31";
						twma7["UNIT_CODE"] = " ";
						twma7["STOCK_NO_FIN"] = twma7["STOCK_NO"].ToString();
						twma7["CRANE_CMDGRPNO"] = 0;
						twma7["MOVE_TYPE"] = twma7["STOCK_OPER_ORDER"].ToString();
					}
					if (twma7.QueryCount("MAT_NO") > 0)
					{
						twma7.Update("STOCK_NO_FROM,STOCK_PLACE_NO_FROM,STOCK_OPER_ORDER,STOCK_PLACE_NO_TO,STOCK_OPER_ORDER_FIN,STOCK_PLACE_NO_FIN,UNIT_CODE,STOCK_NO_FIN,CRANE_CMDGRPNO,MOVE_TYPE", "MAT_NO");
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
						twma7.Insert();
					}

				}
			}
		}
		for (int i = 0; i < dtstock_place_no.Rows.get_Count(); i++)
		{
			stock_place_no = dtstock_place_no.Rows[i]["STOCK_PLACE_NO"].ToString();
			if (i == 0 || stock_place_no != dtstock_place_no.Rows[i]["STOCK_PLACE_NO"].ToString().Trim())
			{
				//不能循环调用，暂时注释，以后再修改程序
				//doFlag = f_wmsm_craneCmd_layer_update(stock_place_no, " ", "1", bcls_ret, conn);
				if (doFlag != 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
		}

		if (de_count == cmd_mat.Rows.get_Count() && de_count != 0)
		{
			Log::Trace("", __FUNCTION__, "All masterials has be in HSM");
			//twm000m.STOCK_PLACE_NO = stock_place_no_fin;
			//twm000m.PLAN_NUM = 0;
			//twm000m.STACKING_NUM = 5;
			//twm000m.PLAN_EXEC_SEQ_NO = 0;
			//twm000m.Update("STACKING_NUM,PLAN_EXEC_SEQ_NO,PLAN_NUM", "STOCK_PLACE_NO");

			sqlstr = "delete from twm000f where stock_place_no_fin='" + stock_place_no_fin + "'";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();

			sqlstr = "select mat_no from twm000f where stock_place_no_fin=' ' and rownum<6 order by plan_exec_seq_no";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			while (cmd_inq.Read())
			{
				twm000f["MAT_NO"] = cmd_inq.GetString(1);
				twm000f["STOCK_PLACE_NO_FIN"] = stock_place_no_fin;
				twm000f.Update("STOCK_PLACE_NO_FIN", "MAT_NO");
			}
			cmd_inq.Close();
			doFlag = f_wmsm_craneCmd_S_2C_do(stock_place_no_fin, bcls_rec, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
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
	return doFlag;
}

