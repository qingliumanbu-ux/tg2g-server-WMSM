/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:      jinquan
Version:     1.1.1
Date:        2016-11-25
Description: 删除机组备料和上料命令
**************************************************/

//框架头文件
#include "WM_Utility.h"
#include "h_wms0_pub.h"

BM2_FUNCTION_IMPORT
int f_wm00_pileinfocal(CString stock_no, CString stock_place_no, EIClass * bcls_ret, CDbConnection * conn);  //垛位最大高度、重量修正

BM2_FUNCTION_EXPORT
int f_wmsm_craneCmd_S_delete(CString unit_code, CString stock_oper_order, CString vehicleno, EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	/*程序内部变量*/
	int doFlag = 0;
	CString sqlstr = " ";
	CString down_flag = " ";
	CString stock_place_no_to = " ";
	CString stock_no_to = " ";

	/*数据库操作类定义*/
	CDbCommand cmd_inq(conn);

	/*定义表实体对象*/
	//CTWMA7 twma7(conn); 
	//CTWMA2 twma2(conn);
	CModel twma7 = CModel("TWMA7");
	CModel twma2 = CModel("TWMA2");
	
	/*数据存放块*/
	CDataTable up_dtMat;                              //存放上层材料
	CDataTable down_dtMat;                            //存放下层材料
	CDataTable cmd_dtMat;                             //存放需要删除命令的材料
	CDataTable dtStockNo;     
	CDataTable dtmat1;
	CDataTable update_stock;
	try
	{
		//项目自定义日志
		CTracer log(__FUNCTION__);

		if (stock_oper_order.Trim() == "2C")
		{
			sqlstr = " select a.mat_no,a.stock_place_no_from,b.stock_place_type,b.dev_div,a.main_mat_no,a.yard_layer_from,c.layerno from twma7 a,twm04 b,twma2 c"
				" where a.stock_place_no_from=b.stock_place_no and a.mat_no=c.mat_no and a.stock_oper_order_fin='2C' and a.crane_inst_status='0' and a.unit_code='" + unit_code + "'";
			Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteQuery(cmd_dtMat);
			cmd_inq.Close();
		}
		else if (stock_oper_order.Trim() == "2E") 
		{
			sqlstr = " select a.mat_no,a.stock_place_no_from,a.stock_place_no_to,a.stock_no,b.stock_place_type,b.dev_div,a.main_mat_no,a.yard_layer_from from twma7 a,twm04 b"
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
		for (int i = 0; i < cmd_dtMat.Rows.get_Count();i++)
		{
			Log::Trace("", __FUNCTION__, "开始删除命令第【{0}】个命令", i+1);
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
			if (cmd_dtMat.Rows[i]["STOCK_PLACE_TYPE"].ToString() == "7")
			{
				if (stock_oper_order.Trim() == "2B")
				{
					Log::Trace("", __FUNCTION__, "材料{0}删命令", twma7["MAT_NO"].ToString());
					twma7.Reset();
					twma7["MAT_NO"] = cmd_dtMat.Rows[i]["MAT_NO"].ToString();
					twma7.Query("MAT_NO");
					twma7.Delete();
					doFlag = f_wm00_pileinfocal(twma7["STOCK_NO"].ToString(), twma7["STOCK_PLACE_NO_TO"].ToString(), bcls_ret, conn);
					if (doFlag != 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				else
				{
					Log::Trace("", __FUNCTION__, "材料{0}在备料区，跳过", cmd_dtMat.Rows[i]["MAT_NO"].ToString());
					continue;
				}
				
			}
			else if (cmd_dtMat.Rows[i]["STOCK_PLACE_TYPE"].ToString() == "D"&&cmd_dtMat.Rows[i]["DEV_DIV"].ToString() == "3")
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
				twma7.Update("STOCK_OPER_ORDER_FIN,STOCK_OPER_ORDER,STOCK_PLACE_NO_TO,STOCK_PLACE_NO_FIN,UNIT_CODE","MAT_NO");
				doFlag = f_wm00_pileinfocal(stock_no_to, stock_place_no_to, bcls_ret, conn);
				if (doFlag != 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
				continue;
			}
			else if (cmd_dtMat.Rows[i]["STOCK_PLACE_TYPE"].ToString() == "D" && (cmd_dtMat.Rows[i]["DEV_DIV"].ToString() == "1" || cmd_dtMat.Rows[i]["DEV_DIV"].ToString() == "2"))
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
				Log::Trace("", __FUNCTION__, "材料{0}在正常库位上，判断下层是否有命令", cmd_dtMat.Rows[i]["MAT_NO"].ToString());
				sqlstr = "  select a.mat_no, a.layerno,b.stock_oper_order,b.main_mat_no from twma2 a,twma7 b where a.mat_no=b.mat_no and a.stock_place_no = '" + cmd_dtMat.Rows[i]["STOCK_PLACE_NO_FROM"].ToString() + "' order by layerno" ;
				Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(down_dtMat);
				cmd_inq.Close();
				if (down_dtMat.Rows.get_Count()>0)
				{
					if (cmd_dtMat.Rows[i]["LAYERNO"].ToDecimal() > down_dtMat.Rows[0]["LAYERNO"].ToDecimal() && down_dtMat.Rows[0]["STOCK_OPER_ORDER"].ToString() == "31"&&down_dtMat.Rows[0]["MAIN_MAT_NO"].ToString().Trim() != "1")
					{
						Log::Trace("", __FUNCTION__, "最下层为非人工生成的倒跺命令，删除");
						for (int m = 0; m < down_dtMat.Rows.get_Count(); m++)
						{
							if (cmd_dtMat.Rows[i]["LAYERNO"].ToDecimal() > down_dtMat.Rows[m]["LAYERNO"].ToDecimal() && down_dtMat.Rows[0]["STOCK_OPER_ORDER"].ToString() == "31"&&down_dtMat.Rows[0]["MAIN_MAT_NO"].ToString().Trim() != "1")
							{
								Log::Trace("", __FUNCTION__, "删除材料{0}命令", down_dtMat.Rows[m]["MAT_NO"].ToString());
								twma7.Reset();
								twma7["MAT_NO"] = down_dtMat.Rows[m]["MAT_NO"].ToString();
								twma7.Query("MAT_NO");
								twma7.Delete();
								doFlag = f_wm00_pileinfocal(twma7["STOCK_NO"].ToString(),
									twma7["STOCK_PLACE_NO_TO"].ToString(), bcls_ret, conn);
								if (doFlag != 0)
								{
									throw CApplicationException(-1, s.msg, log.Location);
								}
								continue;
							} 
							else if (cmd_dtMat.Rows[i]["LAYERNO"].ToDecimal() == down_dtMat.Rows[m]["LAYERNO"].ToDecimal())
							{
								Log::Trace("", __FUNCTION__, "下层没有命令");
								down_flag = "1";
								break;
							}
							else
							{
								Log::Trace("", __FUNCTION__, "下层有命令@@");
								down_flag = "0";
								break;
							}
						}
					}
					else if (cmd_dtMat.Rows[i]["LAYERNO"].ToDecimal() == down_dtMat.Rows[0]["LAYERNO"].ToDecimal())
					{
						Log::Trace("", __FUNCTION__, "下层没有命令");
						down_flag = "1";
					}
					else
					{
						Log::Trace("", __FUNCTION__, "下层有命令##");
						down_flag = "0";
					}
				
					if (down_flag == "1")
					{
						Log::Trace("", __FUNCTION__, "下层没有命令，从下往上删");
						sqlstr = " select mat_no,layerno from twma2 "
							" where stock_place_no = '" + cmd_dtMat.Rows[i]["STOCK_PLACE_NO_FROM"].ToString() + "'"
							" and layerno > '" + cmd_dtMat.Rows[i]["LAYERNO"].ToString() + "'"
							" order by layerno asc";
							Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
							cmd_inq.SetCommandText(sqlstr);
							cmd_inq.ExecuteQuery(up_dtMat);
							cmd_inq.Close();
							Log::Trace("", __FUNCTION__, "材料个数{0}", up_dtMat.Rows.get_Count());
							for (int j = -1; j < up_dtMat.Rows.get_Count(); j++)
							{
								twma7.Reset();
								if (j == -1)
								{
									twma7["MAT_NO"] = cmd_dtMat.Rows[i]["MAT_NO"].ToString();
									twma7.Query("MAT_NO");
									Log::Trace("", __FUNCTION__, "材料{0}删命令", twma7["MAT_NO"].ToString());
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
									twma7["MAT_NO"] = up_dtMat.Rows[j]["MAT_NO"].ToString();
									if (twma7.Query("MAT_NO")>0)
									{
										if (twma7["MAIN_MAT_NO"].ToString() != "1" &&
											(twma7["STOCK_OPER_ORDER"].ToString() == "31" ||
											twma7["STOCK_OPER_ORDER"].ToString() == "30"))
										{
											Log::Trace("", __FUNCTION__, "材料{0}删命令", twma7["MAT_NO"].ToString());
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
											Log::Trace("", __FUNCTION__, "材料{0}不能删命令", twma7["MAT_NO"].ToString());
											break;
										}
									}
									else
									{
										Log::Trace("", __FUNCTION__, "材料{0}不存在命令", twma7["MAT_NO"].ToString());
										continue;
									}
								}	

							}
						}
					else if (down_flag == "0")
					{
						Log::Trace("", __FUNCTION__, "下层有命令,替换为倒跺命令");
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
							Log::Trace("", __FUNCTION__, "材料{0}删除时发生错误1", cmd_dtMat.Rows[i]["MAT_NO"].ToString());
					}
					}
					else
					{
						Log::Trace("", __FUNCTION__, "材料{0}删除时发生错误2", cmd_dtMat.Rows[i]["MAT_NO"].ToString());
					}								
			}			
		}

		if (stock_oper_order == "31" || stock_oper_order == "32")
		{
			//sqlstr = "select distinct stock_no from twmA7 WHERE STOCK_PLACE_NO_TO=' ' AND stock_oper_order like '3%'";
			//Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			//cmd_inq.SetCommandText(sqlstr);
			//cmd_inq.ExecuteQuery(dtStockNo);
			//cmd_inq.Close();
			//Log::Trace("", __FUNCTION__, "库区个数：{0}", dtStockNo.Rows.get_Count());

			//sqlstr = "SELECT MAT_NO,STOCK_PLACE_NO_TO,STOCK_NO FROM TWMA7 WHERE STOCK_PLACE_NO_TO=' ' AND stock_oper_order like '3%' order by CMD_SEQ ";
			//Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			//cmd_inq.SetCommandText(sqlstr);
			//cmd_inq.ExecuteQuery(dtmat1);
			//cmd_inq.Close();
			//Log::Trace("", __FUNCTION__, "推荐材料个数：{0}", dtmat1.Rows.get_Count());
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
			//		if (dtmat1.Rows[j]["STOCK_NO"].ToString() == dtStockNo.Rows[i]["STOCK_NO"].ToString() && dtmat1.Rows[j]["STOCK_PLACE_NO_TO"].ToString() == " ")
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
