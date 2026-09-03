/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:      jinquan
Version:     1.1.1
Date:        2016-12-20
Description: 库位层号更新刷新命令
**************************************************/

/* C/C++ 的标准头文件部分 */
#include "WM_Utility.h"	// 框架头，不可删除 
//#include "twma2.h"
//#include "twm04.h"
//#include "twma7.h"

BM2_FUNCTION_IMPORT
int f_wmsm_CraneCmd_S_Make(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);	     //行车命令形成	
int f_wmsm_craneCmd_S_2C_do(CString stock_place_no_fin, EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);   //板坯库备料命令生成函数
int f_wm00_pileinfocal(CString stock_no, CString stock_place_no, EIClass * bcls_ret, CDbConnection * conn);  //垛位最大高度、重量修正

//BM2_FUNCTION_EXPORT
//int f_wmsmsm_craneCmd_layer_update(CString stock_place_no, CString vehicle_no, CString flag, EIClass * bcls_ret, CDbConnection * conn);

BM2_FUNCTION_EXPORT
int f_wmsm_craneCmd_layer_update(CString stock_place_no, CString vehicle_no, CString flag, EIClass * bcls_ret, CDbConnection * conn)
{
	/*程序内部变量*/
	int doFlag = 0;
	int seq = 0;
	CString sqlstr = " ";
	CString down_flag = " ";
	CString dateTime = " ";
	CString delete_flag = "0";
	CString update_flag = "0";
	CString cmd_flag = "0";
	CString cmd_flag_1 = "0";
	CDecimal curseq_cmd = 0;

	/*定义表实体对象*/
	//CTWMA2   twma2(conn);
	//CTWM04   twm04(conn);
	//CTWMA7   twma7(conn);
	CModel twma2 = CModel("TWMA2");
	CModel twm04 = CModel("TWM04");
	CModel twma7 = CModel("TWMA7");

	/*数据库操作类定义*/
	CDbCommand cmd_inq_update(conn);

	CDataTable cmd_seq;
	CDataTable cmd_seq_1;
	CDataTable cmd_seq_2;
	CDataTable cmd_seq_3;
	CDataTable dtStockNo;
	CDataTable dtmat1;

	try
	{
		//项目自定义日志
		CTracer log(__FUNCTION__);

		//设置行车命令生成函数传入块
		EIClass bcls_rec_make;
		bcls_rec_make.Tables[0].set_TableName("WM00_CMD");
		bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
		bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "STOCK_NO_FROM");
		bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO_FROM");
		bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "STOCK_NO_TO");
		bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO_TO");
		bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");
		bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "CMD_METHOD");
		bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "YARD_LAYER_FROM");
		bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "CMD_SEQ");

		twm04["STOCK_PLACE_NO"] = stock_place_no;
		if (!twm04.Query("STOCK_PLACE_NO"))
		{
			strcpy(s.msg, "Position No. is not exist.");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (vehicle_no.Trim() == ""&& flag != "1")
		{
			sqlstr = "select b.mat_no,b.layerno,a.stock_oper_order,a.stock_place_no_fin from twma7 a,twma2 b where a.mat_no=b.mat_no and b.stock_place_no='" + stock_place_no + "' order by b.layerno desc";
			Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			cmd_inq_update.SetCommandText(sqlstr);
			cmd_inq_update.ExecuteQuery(cmd_seq_3);
			cmd_inq_update.Close();
			if (cmd_seq_3.Rows.get_Count() > 1
				&& cmd_seq_3.Rows[0]["STOCK_OPER_ORDER"].ToString().Trim() == "2C"
				&& cmd_seq_3.Rows[1]["STOCK_OPER_ORDER"].ToString().Trim() == "2C"
				&& cmd_seq_3.Rows[0]["STOCK_PLACE_NO_FIN"].ToString().Trim() == cmd_seq_3.Rows[1]["STOCK_PLACE_NO_FIN"].ToString().Trim())
			{
				EIClass * bcls_rec;
				doFlag = f_wmsm_craneCmd_S_2C_do(cmd_seq_3.Rows[0]["STOCK_PLACE_NO_FIN"].ToString(), bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
		}

		Log::Trace("", __FUNCTION__, "检查库位{0}层号是否和命令一致", twm04["STOCK_PLACE_NO"].ToString());
		if (vehicle_no.Trim() == "")
		{
			sqlstr =
				"select a.mat_no,b.cmd_seq,a.layerno from twma2 a LEFT OUTER JOIN twma7 b"
				" ON a.mat_no=b.mat_no WHERE stock_place_no = '" + stock_place_no + "' order by layerno ";
		}
		else
		{
			sqlstr =
				"select a.mat_no,b.cmd_seq,a.layerno from twma2 a LEFT OUTER JOIN twma7 b"
				" ON  a.mat_no=b.mat_no WHERE stock_place_no = '" + stock_place_no + "' and b.vehicle_no ='" + vehicle_no + "' order by layerno  ";
		}
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq_update.SetCommandText(sqlstr);
		cmd_inq_update.ExecuteQuery(cmd_seq_2);
		cmd_inq_update.Close();

		for (int i = 0; i < cmd_seq_2.Rows.get_Count(); i++)
		{
			if (cmd_seq_2.Rows[i]["CMD_SEQ"].ToDecimal() == 0 && cmd_flag_1 == "0")
			{
				continue;
			}

			if (curseq_cmd == 0 || curseq_cmd > cmd_seq_2.Rows[i]["CMD_SEQ"].ToDecimal())
			{
				cmd_flag_1 = "1";
				curseq_cmd = cmd_seq_2.Rows[i]["CMD_SEQ"].ToDecimal();
				continue;
			}
			else
			{
				cmd_flag = "1";
				break;
			}
		}
		if (cmd_flag == "0")
		{
			Log::Trace("", __FUNCTION__, "库位层号和命令一致");
		}
		else if (cmd_flag == "1")
		{
			Log::Trace("", __FUNCTION__, "库位层号和命令不一致");
			if (vehicle_no.Trim() == "")
			{
				sqlstr = "select b.cmd_seq,a.layerno from twma2 a,twma7 b where a.mat_no=b.mat_no and a.stock_place_no='" + stock_place_no + "' and a.layerno=(select min(layerno) from twma2 a,twma7 b where a.mat_no=b.mat_no and a.stock_place_no='" + stock_place_no + "')";
			}
			else
			{
				sqlstr = "select b.cmd_seq,a.layerno from twma2 a,twma7 b where a.mat_no=b.mat_no and a.stock_place_no='" + stock_place_no + "' and b.vehicle_no='" + vehicle_no + "' and a.layerno=(select min(layerno) from twma2 a,twma7 b where a.mat_no=b.mat_no and a.stock_place_no='" + stock_place_no + "' and b.vehicle_no='" + vehicle_no + "')";
			}
			Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			cmd_inq_update.SetCommandText(sqlstr);
			cmd_inq_update.ExecuteQuery(cmd_seq);
			cmd_inq_update.Close();

			Log::Trace("", __FUNCTION__, "最下面一层的cmd_seq = [{0}]", cmd_seq.Rows[0]["CMD_SEQ"].ToString());
			if (vehicle_no.Trim() == "")
			{
				sqlstr = "update twma7  set cmd_seq='" + cmd_seq.Rows[0]["CMD_SEQ"].ToString() + "' where stock_place_no_from='" + stock_place_no + "'";
			}
			else
			{
				sqlstr = "update twma7  set cmd_seq='" + cmd_seq.Rows[0]["CMD_SEQ"].ToString() + "' where stock_place_no_from='" + stock_place_no + "'  and vehicle_no='" + vehicle_no + "'";
			}

			Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			cmd_inq_update.SetCommandText(sqlstr);
			cmd_inq_update.ExecuteNonQuery();
			cmd_inq_update.Close();

			if (vehicle_no.Trim() == "")
			{
				sqlstr = "select mat_no from twma2 where stock_place_no='" + stock_place_no + "' and layerno>='" + cmd_seq.Rows[0]["LAYERNO"].ToString() + "' order by layerno";
			}
			else
			{
				sqlstr = "select mat_no from twma2 where stock_place_no='" + stock_place_no + "' and layerno>='" + cmd_seq.Rows[0]["LAYERNO"].ToString() + "' and vehicle_no='" + vehicle_no + "' order by layerno";
			}
			Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			cmd_inq_update.SetCommandText(sqlstr);
			cmd_inq_update.ExecuteQuery(cmd_seq_1);
			cmd_inq_update.Close();

			for (int i = 0; i < cmd_seq_1.Rows.get_Count(); i++)
			{
				twma7.Reset();
				twma7["MAT_NO"] = cmd_seq_1.Rows[i]["MAT_NO"].ToString();
				if (twma7.Query("MAT_NO"))
				{
					Log::Trace("", __FUNCTION__, "材料{0}存在命令", twma7["MAT_NO"].ToString());
					if (twma7["STOCK_OPER_ORDER"].ToString().Trim() == "31" &&
						twma7["MAIN_MAT_NO"].ToString().Trim() != "1"&&
						delete_flag == "0")
					{
						Log::Trace("", __FUNCTION__, "删除材料{0}命令", twma7["MAT_NO"].ToString());
						twma7.Delete();
					}
					else
					{
						delete_flag = "1";
						twma2["MAT_NO"] = twma7["MAT_NO"].ToString();
						twma2.Query("MAT_NO");
						twma7["YARD_LAYER_FROM"] = twma2["LAYERNO"];
						twma7.Update("YARD_LAYER_FROM", "MAT_NO");
					}
				}
				else
				{
					twma2.Reset();
					twma2["MAT_NO"] = twma7["MAT_NO"];
					twma2.Query("MAT_NO");
					if (vehicle_no.Trim() == "")
					{
						Log::Trace("", __FUNCTION__, "材料{0}不存在命令，生成倒跺命令", twma7["MAT_NO"].ToString());
						bcls_rec_make.Tables[0].Rows.Add();
						bcls_rec_make.Tables[0].Rows[0]["MAT_NO"] = twma2["MAT_NO"].ToString();
						bcls_rec_make.Tables[0].Rows[0]["YARD_LAYER_FROM"] = twma2["LAYERNO"].ToString();
						bcls_rec_make.Tables[0].Rows[0]["STOCK_NO_TO"] = twma2["STOCK_NO"].ToString();
						bcls_rec_make.Tables[0].Rows[0]["STOCK_PLACE_NO_FROM"] = twma2["STOCK_PLACE_NO"].ToString();
						bcls_rec_make.Tables[0].Rows[0]["STOCK_PLACE_NO_TO"] = " ";
						bcls_rec_make.Tables[0].Rows[0]["STOCK_OPER_ORDER"] = "31";
						bcls_rec_make.Tables[0].Rows[0]["CMD_SEQ"] = cmd_seq.Rows[0]["CMD_SEQ"].ToString();
						//调用函数
						doFlag = f_wmsm_CraneCmd_S_Make(&bcls_rec_make, bcls_ret, conn);
						if (doFlag != 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}
					}
					else
					{
						Log::Trace("", __FUNCTION__, "材料{0}不存在命令，生成入库命令", twma7["MAT_NO"].ToString());
						bcls_rec_make.Tables[0].Rows.Add();
						bcls_rec_make.Tables[0].Rows[0]["MAT_NO"] = twma2["MAT_NO"].ToString();
						bcls_rec_make.Tables[0].Rows[0]["YARD_LAYER_FROM"] = twma2["LAYERNO"].ToString();
						bcls_rec_make.Tables[0].Rows[0]["STOCK_NO_TO"] = twma2["STOCK_NO"].ToString();
						bcls_rec_make.Tables[0].Rows[0]["STOCK_PLACE_NO_FROM"] = twma2["STOCK_PLACE_NO"].ToString();
						bcls_rec_make.Tables[0].Rows[0]["STOCK_PLACE_NO_TO"] = " ";
						bcls_rec_make.Tables[0].Rows[0]["STOCK_OPER_ORDER"] = "1B";
						bcls_rec_make.Tables[0].Rows[0]["CMD_SEQ"] = cmd_seq.Rows[0]["CMD_SEQ"].ToString();
						//调用函数
						doFlag = f_wmsm_CraneCmd_S_Make(&bcls_rec_make, bcls_ret, conn);
						if (doFlag != 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}
					}
				}
			}
		}

#pragma region  推荐倒跺库位

		//sqlstr = "select distinct stock_no_from  from twmA7 WHERE STOCK_PLACE_NO_TO=' ' AND stock_oper_order like '3%' AND stock_no_from!=' '";
		//Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		//cmd_inq_update.SetCommandText(sqlstr);
		//cmd_inq_update.ExecuteQuery(dtStockNo);
		//cmd_inq_update.Close();
		//Log::Trace("", __FUNCTION__, "倒跺库区个数：{0}", dtStockNo.Rows.get_Count());

		//sqlstr = "SELECT MAT_NO,STOCK_PLACE_NO_TO,STOCK_NO_FROM FROM TWMA7 WHERE STOCK_PLACE_NO_TO=' ' AND stock_oper_order like '3%' order by CMD_SEQ  ";
		//Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		//cmd_inq_update.SetCommandText(sqlstr);
		//cmd_inq_update.ExecuteQuery(dtmat1);
		//cmd_inq_update.Close();
		//Log::Trace("", __FUNCTION__, "推荐倒跺材料个数：{0}", dtmat1.Rows.get_Count());
		//for (int i = 0; i < dtStockNo.Rows.get_Count(); i++)
		//{
		//	// 初始化 库位推荐用 输入/输出块
		//	EIClass in, out;
		//	f_wms_auto_init(&in, &out, conn);

		//	CDataTable& blkParams = in.Tables[WMS_BLK_IN_PARAMS];
		//	blkParams.Rows[0][WMS_COL_JOB_IO_DIV] = "M";         	                                      // 入出库区分(I：入库、O：出库、M：倒垛)	
		//	blkParams.Rows[0][WMS_COL_JOB_ADJUST_FLAG] = "N";                                             // 库区作业顺序是否可调(N:不可调、Y:可调整)	
		//	blkParams.Rows[0][WMS_COL_JOB_STOCK_NO] = dtStockNo.Rows[i]["STOCK_NO_FROM"];                      // 作业库区号(入库时：入库目标库区、 倒垛时：倒垛库区、 出库时：出库起始库)
		//	blkParams.Rows[0][WMS_COL_JOB_MAT_SHAPE_DIV] = "P";                                           // 材料形状区分(P:板类、C:卷类)
		//	blkParams.Rows[0]["HIGH_MAT_MOVE_FLAG"] = "N";
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
		//		Log::Trace("", __FUNCTION__, "mat_STOCK_NO[{0}]", dtmat1.Rows[j]["STOCK_NO_FROM"].ToString());
		//		Log::Trace("", __FUNCTION__, "STOCK_NO[{0}]", dtStockNo.Rows[i]["STOCK_NO_FROM"].ToString());
		//		Log::Trace("", __FUNCTION__, "STOCK_PLACE_NO_TO[{0}]", dtmat1.Rows[j]["STOCK_PLACE_NO_TO"].ToString());
		//		if (dtmat1.Rows[j]["STOCK_NO_FROM"].ToString() == dtStockNo.Rows[i]["STOCK_NO_FROM"].ToString() && dtmat1.Rows[j]["STOCK_PLACE_NO_TO"].ToString().Trim() == "")
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
		//					doFlag = f_wm00_pileinfocal(twma7["STOCK_NO"].ToString(), twma7["STOCK_PLACE_NO_TO"].ToString(), bcls_ret, conn);
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
