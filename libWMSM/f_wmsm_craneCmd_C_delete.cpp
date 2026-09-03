/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:      jinquan
Version:     1.1.1
Date:        2016-12-15
Description: 删除钢卷命令
**************************************************/

//框架头文件
#include "WM_Utility.h"
//#include "twma7.h"
//#include "twma2.h"
//#include "twm04.h"
#include "h_wms0_pub.h"

BM2_FUNCTION_IMPORT
int f_wm00_pileinfocal(CString stock_no, CString stock_place_no, EIClass * bcls_ret, CDbConnection * conn);  //垛位最大高度、重量修正

BM2_FUNCTION_EXPORT
int f_wmsm_craneCmd_C_delete(CString stock_oper_order, CString vehicleno, EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	/*程序内部变量*/
	int doFlag = 0;
	CString sqlstr = " ";
	CString down_flag = " ";
	CString left_flag = " ";
	CString right_flag = " ";
	CString stock_place_no_to = " ";
	CString stock_no_to = " ";

	/*数据库操作类定义*/
	CDbCommand cmd_inq(conn);

	/*定义表实体对象*/
	//CTWMA7 twma7(conn);
	//CTWMA7 twma7_1(conn);
	//CTWM04 twm04(conn);
	//CTWMA2 twma2(conn);
	//CTWM04 twm04_left(conn);
	//CTWM04 twm04_right(conn);
	CModel twma7 = CModel("TWMA7");
	CModel twma7_1 = CModel("TWMA7");
	CModel twm04 = CModel("TWM04");
	CModel twma2 = CModel("TWMA2");
	CModel twm04_left = CModel("TWM04");
	CModel twm04_right = CModel("TWM04");

	/*数据存放块*/
	CDataTable up_dtMat;                              //存放上层材料
	CDataTable down_dtMat;                            //存放下层材料
	CDataTable cmd_dtMat;                             //存放需要删除命令的材料
	CDataTable dtStockNo;
	CDataTable dtmat1;
	try
	{
		//项目自定义日志
		CTracer log(__FUNCTION__);

		if (stock_oper_order.Trim() == "2E")
		{
			sqlstr = " select a.mat_no,a.stock_place_no_from,b.stock_place_type,b.dev_div,a.main_mat_no,a.yard_layer_from from twma7 a,twm04 b"
				" where a.stock_place_no_from=b.stock_place_no and a.stock_oper_order_fin='2E' and a.crane_inst_status='0' AND a.vehicle_no = '" + vehicleno + "'";
			Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteQuery(cmd_dtMat);
			cmd_inq.Close();
		}
		else
		{
			cmd_dtMat = bcls_rec->Tables[0];
		}

		Log::Trace("", __FUNCTION__, "需要删除命令的材料个数：{0}", cmd_dtMat.Rows.get_Count());
		for (int i = 0; i < cmd_dtMat.Rows.get_Count(); i++)
		{
			Log::Trace("", __FUNCTION__, "开始删除命令第【{0}】个命令", i + 1);
			twma2["MAT_NO"] = cmd_dtMat.Rows[i]["MAT_NO"].ToString();
			if (!twma2.Query("MAT_NO") ||
				twma2["STOCK_PLACE_NO"].ToString().Trim() == "")
			{
				Log::Trace("", __FUNCTION__, "材料不在库内，删除");
				twma7["MAT_NO"] = cmd_dtMat.Rows[i]["MAT_NO"].ToString();
				twma7.Query("MAT_NO");
				twma7.Delete();
				doFlag = f_wm00_pileinfocal(twma7["STOCK_NO"].ToString(), twma7["STOCK_PLACE_NO_TO"].ToString(), bcls_ret, conn);
				if (doFlag != 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
				continue;
			}

			if (cmd_dtMat.Rows[i]["STOCK_PLACE_TYPE"].ToString() == "D"&&cmd_dtMat.Rows[i]["DEV_DIV"].ToString() == "3")
			{
				Log::Trace("", __FUNCTION__, "材料{0}在过跨台车上，修改为到当前库区正常库位的命令", cmd_dtMat.Rows[i]["MAT_NO"].ToString());
				twma7.Reset();
				twma7["MAT_NO"] = cmd_dtMat.Rows[i]["MAT_NO"].ToString();
				twma7.Query("MAT_NO");
				stock_place_no_to = twma7["STOCK_PLACE_NO_TO"].ToString();
				stock_no_to = twma7["STOCK_NO"].ToString();
				twma7["STOCK_OPER_ORDER_FIN"] = "32";
				twma7["STOCK_OPER_ORDER"] = "32";
				twma7["STOCK_PLACE_NO_TO"] = " ";
				twma7["STOCK_PLACE_NO_FIN"] = " ";
				twma7["UNIT_CODE"] = " ";
				twma7.Update("STOCK_OPER_ORDER_FIN,STOCK_OPER_ORDER,STOCK_PLACE_NO_TO,STOCK_PLACE_NO_FIN,UNIT_CODE", "MAT_NO");
				doFlag = f_wm00_pileinfocal(stock_no_to, stock_place_no_to, bcls_ret, conn);
				if (doFlag != 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}

				continue;
			}
			else if (cmd_dtMat.Rows[i]["STOCK_PLACE_TYPE"].ToString() == "D" && (cmd_dtMat.Rows[i]["DEV_DIV"].ToString() == "1" || cmd_dtMat.Rows[i]["DEV_DIV"].ToString() == "2" || cmd_dtMat.Rows[i]["DEV_DIV"].ToString() == "7"))
			{
				Log::Trace("", __FUNCTION__, "材料{0}在火车或卡车上，修改为入库命令", cmd_dtMat.Rows[i]["MAT_NO"].ToString());
				twma7.Reset();
				twma7["MAT_NO"] = cmd_dtMat.Rows[i]["MAT_NO"].ToString();
				twma7.Query("MAT_NO");
				stock_place_no_to = twma7["STOCK_PLACE_NO_TO"].ToString();
				stock_no_to = twma7["STOCK_NO"].ToString();
				twma7["STOCK_OPER_ORDER_FIN"] = " ";
				twma7["STOCK_OPER_ORDER"] = "1B";
				twma7["STOCK_PLACE_NO_TO"] = " ";
				twma7["STOCK_PLACE_NO_FIN"] = " ";
				twma7["UNIT_CODE"] = " ";
				twma7.Update("STOCK_OPER_ORDER_FIN,STOCK_OPER_ORDER,STOCK_PLACE_NO_TO,STOCK_PLACE_NO_FIN,UNIT_CODE", "MAT_NO");
				doFlag = f_wm00_pileinfocal(stock_no_to, stock_place_no_to, bcls_ret, conn);
				if (doFlag != 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
				continue;
			}
			else if (cmd_dtMat.Rows[i]["STOCK_PLACE_TYPE"].ToString() == "0")
			{
				Log::Trace("", __FUNCTION__, "材料{0}在正常库位上，判断材料层号", cmd_dtMat.Rows[i]["MAT_NO"].ToString());
				if (cmd_dtMat.Rows[i]["YARD_LAYER_FROM"].ToDecimal() == 1)
				{
					Log::Trace("", __FUNCTION__, "材料在第一层，删除命令");
					twma7.Reset();
					twma7["MAT_NO"] = cmd_dtMat.Rows[i]["MAT_NO"].ToString();
					twma7.Query("MAT_NO");
					twma7.Delete();
					doFlag = f_wm00_pileinfocal(twma7["STOCK_NO"].ToString(), twma7["STOCK_PLACE_NO_TO"].ToString(), bcls_ret, conn);
					if (doFlag != 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}

					Log::Trace("", __FUNCTION__, "处理左上层材料");
					twm04.Reset();
					twm04["STOCK_PLACE_NO"] = cmd_dtMat.Rows[i]["STOCK_PLACE_NO"].ToString();
					twm04.Query("STOCK_PLACE_NO");
					twm04_left["STOCK_COL_NO"] = twm04["STOCK_COL_NO"].ToDecimal() - 1;
					twm04_left["STOCK_ROW_NO"] = twm04["STOCK_ROW_NO"].ToDecimal();
					twm04_left["STOCK_NO"] = twm04["STOCK_NO"].ToString();
					twm04_left["LAYERNO"] = 2;

					if (!twm04_left.Query("STOCK_COL_NO,STOCK_ROW_NO,LAYERNO,STOCK_NO"))
					{
						Log::Trace("", __FUNCTION__, "材料在左上方不能放钢卷,无需处理");
					}
					else
					{
						twma2.Reset();
						twma2["STOCK_PLACE_NO"] = twm04_left["STOCK_PLACE_NO"].ToString();
						if (twma2.Query("STOCK_PLACE_NO"))
						{
							twma7.Reset();
							twma7["MAT_NO"] = twma2["MAT_NO"].ToString();
							if (twma7.Query("MAT_NO"))
							{
								if (twma7["STOCK_OPER_ORDER"].ToString() == "31" &&
									twma7["MAIN_MAT_NO"].ToString() != "1")
								{
									Log::Trace("", __FUNCTION__, "材料在左上方有钢卷且有非人工生成的倒跺命令，删除");
									twma7.Delete();
									doFlag = f_wm00_pileinfocal(twma7["STOCK_NO"].ToString(),
										twma7["STOCK_PLACE_NO_TO"].ToString(), bcls_ret, conn);
									if (doFlag != 0)
									{
										throw CApplicationException(-1, s.msg, log.Location);
									}
								}
								else
								{
									Log::Trace("", __FUNCTION__, "材料在左上方有钢卷且有不能删除的命令，跳过");
								}
							}
							else
							{
								Log::Trace("", __FUNCTION__, "材料在左上方有钢卷且无命令,无需处理");
							}
						}
						else
						{
							Log::Trace("", __FUNCTION__, "材料在左上方没有钢卷,无需处理");
						}
					}
					Log::Trace("", __FUNCTION__, "处理右上层材料");
					twm04_right["STOCK_COL_NO"] = twm04["STOCK_COL_NO"].ToDecimal();
					twm04_right["STOCK_ROW_NO"] = twm04["STOCK_ROW_NO"].ToDecimal();
					twm04_right["STOCK_NO"] = twm04["STOCK_NO"].ToString();
					twm04_right["LAYERNO"] = 2;

					if (!twm04_right.Query("STOCK_COL_NO,STOCK_ROW_NO,LAYERNO,STOCK_NO"))
					{
						Log::Trace("", __FUNCTION__, "材料在右上方不能放钢卷,无需处理");
					}
					else
					{
						twma2.Reset();
						twma2["STOCK_PLACE_NO"] = twm04_right["STOCK_PLACE_NO"].ToString();
						if (twma2.Query("STOCK_PLACE_NO"))
						{
							twma7.Reset();
							twma7["MAT_NO"] = twma2["MAT_NO"].ToString();
							if (twma7.Query("MAT_NO"))
							{
								if (twma7["STOCK_OPER_ORDER"].ToString() == "31"&&
									twma7["MAIN_MAT_NO"].ToString() != "1")
								{
									Log::Trace("", __FUNCTION__, "材料在右上方有钢卷且有非人工生成的倒跺命令，删除");
									twma7.Delete();
									doFlag = f_wm00_pileinfocal(twma7["STOCK_NO"].ToString(),
										twma7["STOCK_PLACE_NO_TO"].ToString(), bcls_ret, conn);
									if (doFlag != 0)
									{
										throw CApplicationException(-1, s.msg, log.Location);
									}
								}
								else
								{
									Log::Trace("", __FUNCTION__, "材料在右上方有钢卷且有不能删除的命令，跳过");
								}
							}
							else
							{
								Log::Trace("", __FUNCTION__, "材料在左上方有钢卷且无命令,无需处理");
							}
						}
						else
						{
							Log::Trace("", __FUNCTION__, "材料在左上方没有钢卷,无需处理");
						}
					}

				}
				else if (cmd_dtMat.Rows[i]["YARD_LAYER_FROM"].ToDecimal() == 2)
				{
					Log::Trace("", __FUNCTION__, "材料在第二层");
					Log::Trace("", __FUNCTION__, "处理左下层材料");
					twm04.Reset();
					twm04["STOCK_PLACE_NO"] = cmd_dtMat.Rows[i]["STOCK_PLACE_NO"].ToString();
					twm04.Query("STOCK_PLACE_NO");
					twm04_left["STOCK_COL_NO"] = twm04["STOCK_COL_NO"].ToDecimal();
					twm04_left["STOCK_ROW_NO"] = twm04["STOCK_ROW_NO"].ToDecimal();
					twm04_left["STOCK_NO"] = twm04["STOCK_NO"].ToString();
					twm04_left["LAYERNO"] = 1;

					if (!twm04_left.Query("STOCK_COL_NO,STOCK_ROW_NO,LAYERNO,STOCK_NO"))
					{
						Log::Trace("", __FUNCTION__, "材料在左下方没有库位，删除出错");
						strcpy(s.msg, "There's no position in material's bottom left and the crane instruction cannot be deleted.");
						throw CApplicationException(-1, s.msg, log.Location);
					}
					else
					{
						Log::Trace("", __FUNCTION__, "左下方{0}", twm04_left["STOCK_PLACE_NO"].ToString());
						twma2.Reset();
						twma2["STOCK_PLACE_NO"] = twm04_left["STOCK_PLACE_NO"].ToString();
						if (twma2.Query("STOCK_PLACE_NO"))
						{
							Log::Trace("", __FUNCTION__, "左下方{0}", twma2["MAT_NO"].ToString());
							twma7.Reset();
							twma7["MAT_NO"] = twma2["MAT_NO"].ToString();
							if (twma7.Query("MAT_NO"))
							{
								if (twma7["STOCK_OPER_ORDER"].ToString() == "31"&&
									twma7["MAIN_MAT_NO"].ToString() != "1")
								{
									Log::Trace("", __FUNCTION__, "材料在左下方有非人工生成的倒跺命令，删除");
									twma7.Delete();
									doFlag = f_wm00_pileinfocal(twma7["STOCK_NO"].ToString(),
										twma7["STOCK_PLACE_NO_TO"].ToString(), bcls_ret, conn);
									if (doFlag != 0)
									{
										throw CApplicationException(-1, s.msg, log.Location);
									}
									left_flag = "0";
								}
								else
								{
									Log::Trace("", __FUNCTION__, "材料在左下方有不能删除的命令");
									left_flag = "1";
								}
							}
							else
							{
								Log::Trace("", __FUNCTION__, "材料在左下方有钢卷且无命令");
								left_flag = "0";
							}
						}
						else
						{
							Log::Trace("", __FUNCTION__, "材料在左下方没有钢卷,删除出错");
							strcpy(s.msg, "There's no material in material's bottom left and the crane instruction cannot be deleted.");
							throw CApplicationException(-1, s.msg, log.Location);
						}
					}


					Log::Trace("", __FUNCTION__, "处理右下层材料");
					twm04_right["STOCK_COL_NO"] = twm04["STOCK_COL_NO"].ToDecimal() + 1;
					twm04_right["STOCK_ROW_NO"] = twm04["STOCK_ROW_NO"].ToDecimal();
					twm04_right["STOCK_NO"] = twm04["STOCK_NO"].ToString();
					twm04_right["LAYERNO"] = 1;

					if (!twm04_right.Query("STOCK_COL_NO,STOCK_ROW_NO,LAYERNO,STOCK_NO"))
					{
						Log::Trace("", __FUNCTION__, "材料在右下方没有库位，删除出错");
						strcpy(s.msg, "There's no position in material's bottom right and the crane instruction cannot be deleted.");
						throw CApplicationException(-1, s.msg, log.Location);
					}
					else
					{
						Log::Trace("", __FUNCTION__, "you下方{0}", twm04_right["STOCK_PLACE_NO"].ToString());
						twma2.Reset();
						twma2["STOCK_PLACE_NO"] = twm04_right["STOCK_PLACE_NO"].ToString();
						if (twma2.Query("STOCK_PLACE_NO"))
						{
							Log::Trace("", __FUNCTION__, "you下方{0}", twma2["MAT_NO"].ToString());
							twma7.Reset();
							twma7["MAT_NO"] = twma2["MAT_NO"].ToString();
							if (twma7.Query("MAT_NO"))
							{
								if (twma7["STOCK_OPER_ORDER"].ToString() == "31"&&
									twma7["MAIN_MAT_NO"].ToString() != "1")
								{
									Log::Trace("", __FUNCTION__, "材料在右下方且有非人工生成的倒跺命令，删除");
									twma7.Delete();
									doFlag = f_wm00_pileinfocal(twma7["STOCK_NO"].ToString(),
										twma7["STOCK_PLACE_NO_TO"].ToString(), bcls_ret, conn);
									if (doFlag != 0)
									{
										throw CApplicationException(-1, s.msg, log.Location);
									}
									right_flag = "0";
								}
								else
								{
									Log::Trace("", __FUNCTION__, "材料在右下方有不能删除的命令");
									right_flag = "1";
								}
							}
							else
							{
								Log::Trace("", __FUNCTION__, "材料在右下方无命令");
								right_flag = "0";
							}
						}
						else
						{
							Log::Trace("", __FUNCTION__, "材料在右下方没有钢卷，删除出错");
							strcpy(s.msg, "There's no material in material's bottom right and the crane instruction cannot be deleted.");
							throw CApplicationException(-1, s.msg, log.Location);
						}
					}
					if (right_flag == "0"&&left_flag == "0")
					{
						Log::Trace("", __FUNCTION__, "下方钢卷都没有命令，删除");
						twma7.Reset();
						twma7["MAT_NO"] = cmd_dtMat.Rows[i]["MAT_NO"].ToString();
						twma7.Query("MAT_NO");
						twma7.Delete();
						doFlag = f_wm00_pileinfocal(twma7["STOCK_NO"].ToString(),
							twma7["STOCK_PLACE_NO_TO"].ToString(), bcls_ret, conn);
						if (doFlag != 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}

					}
					else if (right_flag == "1" || left_flag == "1")
					{
						Log::Trace("", __FUNCTION__, "下方钢卷有命令，改为倒跺命令");
						twma7.Reset();
						twma7["MAT_NO"] = cmd_dtMat.Rows[i]["MAT_NO"].ToString();
						twma7.Query("MAT_NO");
						stock_place_no_to = twma7["STOCK_PLACE_NO_TO"].ToString();
						stock_no_to = twma7["STOCK_NO"].ToString();
						twma7["STOCK_OPER_ORDER_FIN"] = " ";
						twma7["STOCK_OPER_ORDER"] = "31";
						twma7["STOCK_PLACE_NO_TO"] = " ";
						twma7["STOCK_PLACE_NO_FIN"] = " ";
						twma7["UNIT_CODE"] = " ";
						twma7.Update("STOCK_OPER_ORDER_FIN,STOCK_OPER_ORDER,STOCK_PLACE_NO_TO,STOCK_PLACE_NO_FIN,UNIT_CODE", "MAT_NO");
						doFlag = f_wm00_pileinfocal(stock_no_to, stock_place_no_to, bcls_ret, conn);
						if (doFlag != 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}
					}
					else
					{
						sprintf(s.msg, "Function processing error");
						throw CApplicationException(-1, s.msg, log.Location);
					}


				}
				else
				{
					strcpy(s.msg, "Material layer no error");
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			else
			{
				Log::Trace("", __FUNCTION__, "材料{0}删除时发生错误，材料库位类型不存在", cmd_dtMat.Rows[i]["MAT_NO"].ToString());
				strcpy(s.msg, "position type is not exist");
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}

#pragma region  推荐倒跺库位
		//sqlstr =
		//	"select distinct stock_no from twmA7 WHERE STOCK_PLACE_NO_TO=' '"
		//	" AND stock_oper_order like '3%' AND stock_no!=' '";
		//Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		//cmd_inq.SetCommandText(sqlstr);
		//cmd_inq.ExecuteQuery(dtStockNo);
		//cmd_inq.Close();
		//Log::Trace("", __FUNCTION__, "倒跺库区个数：{0}", dtStockNo.Rows.get_Count());

		//sqlstr =
		//	"SELECT MAT_NO,STOCK_PLACE_NO_TO,STOCK_NO FROM TWMA7"
		//	" WHERE STOCK_PLACE_NO_TO=' ' AND stock_oper_order like '3%' order by CMD_SEQ  ";
		//Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		//cmd_inq.SetCommandText(sqlstr);
		//cmd_inq.ExecuteQuery(dtmat1);
		//cmd_inq.Close();
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
		//	blkParams.Rows[0][WMS_COL_JOB_MAT_SHAPE_DIV] = "C";                                           // 材料形状区分(P:板类、C:卷类)
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
		//					twma7["YARD_LAYER_TO"] = out.Tables[WMS_BLK_OUT_TARGETS].Rows[0]["TO_STOCK_LAYER_NO"];
		//					twma7.Update("STOCK_PLACE_NO_TO,YARD_LAYER_TO", "MAT_NO");
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
		//cmd_inq.SetCommandText(sqlstr);
		//cmd_inq.ExecuteQuery(dtStockNo);
		//cmd_inq.Close();
		//Log::Trace("", __FUNCTION__, "入库库区个数：{0}", dtStockNo.Rows.get_Count());

		//sqlstr = "SELECT MAT_NO,STOCK_PLACE_NO_TO,STOCK_NO,STOCK_OPER_ORDER FROM TWMA7 WHERE STOCK_PLACE_NO_TO=' ' AND stock_oper_order like '1%' order by CMD_SEQ  ";
		//Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		//cmd_inq.SetCommandText(sqlstr);
		//cmd_inq.ExecuteQuery(dtmat1);
		//cmd_inq.Close();
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
		//	blkParams.Rows[0][WMS_COL_JOB_MAT_SHAPE_DIV] = "C";                                           // 材料形状区分(P:板类、C:卷类)
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
		//		Log::Trace("", __FUNCTION__, "MAT_NO[{0}]", dtmat1.Rows[j]["MAT_NO"].ToString());
		//		Log::Trace("", __FUNCTION__, "STOCK_NO[{0}]", dtmat1.Rows[j]["STOCK_NO"].ToString());
		//		Log::Trace("", __FUNCTION__, "STOCK_NO[{0}]", dtStockNo.Rows[i]["STOCK_NO"].ToString());
		//		Log::Trace("", __FUNCTION__, "STOCK_PLACE_NO_TO[{0}]", dtmat1.Rows[j]["STOCK_PLACE_NO_TO"].ToString());
		//		Log::Trace("", __FUNCTION__, "STOCK_OPER_ORDER[{0}]", dtmat1.Rows[j]["STOCK_OPER_ORDER"].ToString());
		//		if (dtmat1.Rows[j]["STOCK_NO"].ToString() == dtStockNo.Rows[i]["STOCK_NO"].ToString() && dtmat1.Rows[j]["STOCK_PLACE_NO_TO"].ToString().Trim() == "")
		//		{
		//			++currRow;
		//			blkMats.Rows.Add();
		//			blkMats.Rows[currRow][WMS_COL_MAT_GRP_NO] = dtmat1.Rows[j]["MAT_NO"].ToString();					// 材料组号（1个材料组可含有一个或多个材料。当含有多个材料时，这些材料将作为一个整体叠放在同一个垛位上【适用于钢板】。当只有一块材料时，通常 材料组号=材料号。）
		//			blkMats.Rows[currRow][WMS_COL_MAT_NO] = dtmat1.Rows[j]["MAT_NO"].ToString();						// 材料号
		//			blkMats.Rows[currRow][WMS_COL_MAT_KIND] = " ";                                                      // 材料种类(HP:中厚板、CR：冷轧、HR：热轧 等等)【印度项目不用，传空格】
		//			blkMats.Rows[currRow][WMS_COL_JOB_LARGE_DIV] = dtmat1.Rows[j]["STOCK_OPER_ORDER"].ToString();                                                // 库区作业大分类：库业务类型(代码WM10)
		//			blkMats.Rows[currRow][WMS_COL_JOB_MIDDLE_DIV] = " ";                                                // 库区作业中分类：【印度项目不用，传空格】
		//			if (dtmat1.Rows[j]["STOCK_OPER_ORDER"].ToString() == "1B")
		//			{
		//				blkMats.Rows[currRow][WMS_COL_FROM_STOCK_DEV_NO] = "C201";
		//			}
		//			else
		//			{
		//				blkMats.Rows[currRow][WMS_COL_FROM_STOCK_DEV_NO] = " ";
		//			}

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
		//					twma7["YARD_LAYER_TO"] = out.Tables[WMS_BLK_OUT_TARGETS].Rows[0]["TO_STOCK_LAYER_NO"];
		//					twma7.Update("STOCK_PLACE_NO_TO,YARD_LAYER_TO", "MAT_NO");
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
