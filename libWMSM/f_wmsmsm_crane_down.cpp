/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:      张凌辉
Version:     1.1.1
Date:        2016-10-10 14:05:08
Description: 吊车命令卸下函数
**************************************************/

//#include "WM_Utility.h"
#include "stdafx.h"

//调用外部函数
BM2_FUNCTION_IMPORT
int f_wmsmsm_stock_in(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

BM2_FUNCTION_IMPORT
int f_wmsmsm_stock_out(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

BM2_FUNCTION_IMPORT
int f_wmsmsm_stock_move(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

BM2_FUNCTION_IMPORT
int f_wmsmsm_cranecmd_follow(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);

BM2_FUNCTION_IMPORT


BM2_FUNCTION_EXPORT
int f_wmsmsm_crane_down(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	//程序内部变量
	int doFlag = 0;
	CString sqlstr = " ";
	CDbCommand cmd_inq(conn);

	//应用变量
	int cmdSeq = 0; 
	int tj_count = 0;
	CDecimal layerNoFr = 0;

	CString matNo = " ";
	CString matNo_pre = " ";
	CString sqlWhere = " ";
	CString shiftGroup = " ";
	CString shiftNo = " ";
	CString stockNoFr = " ";
	//CString stockPlaceNoFr = " ";
	CString stockNoTo = " ";
	CString stockPlaceNoTo = " ";
	CString tcNo = " ";
	CString hall_no = " ";
	CString stockPlaceType = " ";
	CString devDiv = " ";
	CString stockOperOrder = " ";
	CString enExDiv = " ";
	CString manageAccu = " ";
	CString div = " ";
	CString unit_code = " ";
	CString layerno = " ";
	CString infur_slab_wt = " ";
	CString logic_stock = " ";
	CDecimal mat_act_x = 0;
	CDecimal mat_act_y = 0;
	CDecimal mat_act_z = 0;
	CString op_mode = "";
	CString car_no = "";
	CString dateTime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CString v_mat_no = " ";
	CString v_prod_shift_no = "";
	CString v_prod_shift_group = ""; 
	CString v_prod_date = "";
	int seq = 0;


	//定义行车命令数据表
	CDataTable dtCraneCmd;

	//定义垛位数据表
	CDataTable dtStockPlace;

	//定义行车命令履历数据表
	CDataTable dtCraneCmdTrace;

	//定义材料数据表
	CDataTable dtMat;

	//定义电文数据表
	CDataTable dtMessage;

	//定义修改字段表
	CDataTable dtUpdItem;

	//定义表实体对象
	CModel hwma7 = CModel("HWM00A7");
	CModel twma7 = CModel("TWMA7");
	CModel twma0 = CModel("TWMA0");
	CModel twma2 = CModel("TWMA2");
	CModel twm04 = CModel("TWM04");

	EIClass bcls_stock_auto;
	bcls_stock_auto.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO");

	//调用仓库入库主函数
	EIClass bcls_stock_in;
	bcls_stock_in.Tables[0].set_TableName("WM_STOCK");
	bcls_stock_in.Tables[0].Columns.Add(twma0);
	bcls_stock_in.Tables[0].Columns.Add(twma2);
	bcls_stock_in.Tables[0].Rows.Clear();


	//调用仓库入库主函数
	EIClass bcls_stock_out;
	bcls_stock_out.Tables[0].set_TableName("WM_STOCK");
	bcls_stock_out.Tables[0].Columns.Add(twma0);
	bcls_stock_out.Tables[0].Columns.Add(twma2);
	bcls_stock_out.Tables[0].Rows.Clear();
	
	//调用仓库入库主函数
	EIClass bcls_stock_move;
	bcls_stock_move.Tables[0].set_TableName("WM_STOCK");
	bcls_stock_move.Tables[0].Columns.Add(twma0);
	bcls_stock_move.Tables[0].Columns.Add(twma2);
	bcls_stock_move.Tables[0].Rows.Clear();

	//调用仓库入库主函数
	EIClass bcls_stock_move_1;
	bcls_stock_move_1.Tables[0].set_TableName("WM_STOCK");
	bcls_stock_move_1.Tables[0].Columns.Add(twma0);
	bcls_stock_move_1.Tables[0].Columns.Add(twma2);
	bcls_stock_move_1.Tables[0].Rows.Clear();
	

	//调用 向加热炉L2发送B 装炉实绩
	EIClass bcls_sec_u1c12r;
	bcls_sec_u1c12r.Tables[0].set_TableName("U1C12R");
	bcls_sec_u1c12r.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
	bcls_sec_u1c12r.Tables[0].Columns.Add(DT_STRING, "INFUR_SLAB_WT");
	bcls_sec_u1c12r.Tables[0].Columns.Add(DT_STRING, "SLAB_FUR_BEF_TEMP");

	//调用 向二切L2发送吊起实绩电文
	EIClass bcls_rec_u1f104;
	bcls_rec_u1f104.Tables[0].set_TableName("U1F104");
	bcls_rec_u1f104.Tables[0].Columns.Add(DT_STRING, "OPER_FLAG");
	bcls_rec_u1f104.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
	bcls_rec_u1f104.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO");
	bcls_rec_u1f104.Tables[0].Rows.Clear();

	//调用 向轧机L2发送B炉板坯抽出实绩电文
	EIClass bcls_rec_u1f11r;
	bcls_rec_u1f11r.Tables[0].set_TableName("U1F11R");
	bcls_rec_u1f11r.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
	bcls_rec_u1f11r.Tables[0].Columns.Add(DT_STRING, "TABLE_NO");
	bcls_rec_u1f11r.Tables[0].Rows.Clear();

	EIClass bcls_rec_send;
	bcls_rec_send.Tables[0].set_TableName("U1DL03");
	bcls_rec_send.Tables[0].Columns.Add(DT_STRING, "mat_no");
	bcls_rec_send.Tables[0].Columns.Add(DT_STRING, "origin_mat_no");
	bcls_rec_send.Tables[0].Columns.Add(DT_STRING, "stock_oper_order");
	bcls_rec_send.Tables[0].Columns.Add(DT_DECIMAL, "cmd_seq");
	bcls_rec_send.Tables[0].Columns.Add(DT_DECIMAL, "crane_cmdgrpno");
	bcls_rec_send.Tables[0].Columns.Add(DT_DECIMAL, "batch_no");
	bcls_rec_send.Tables[0].Columns.Add(DT_STRING, "stock_place_no_from");
	bcls_rec_send.Tables[0].Columns.Add(DT_DECIMAL, "yard_layer_from");
	bcls_rec_send.Tables[0].Columns.Add(DT_STRING, "stock_place_no_to");
	bcls_rec_send.Tables[0].Columns.Add(DT_STRING, "stock_oper_order_fin");
	bcls_rec_send.Tables[0].Columns.Add(DT_STRING, "remark");


	EIClass bcls_rec_stock_follow;
	bcls_rec_stock_follow.Tables[0].set_TableName("WM00_FOLLOW");
	bcls_rec_stock_follow.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
	bcls_rec_stock_follow.Tables[0].Rows.Clear();
	

	try
	{
		//判断是否存在指定块
		if (bcls_rec->Tables.IndexOf("WM00_DOWN") < 0 ||
			bcls_rec->Tables["WM00_DOWN"].Rows.get_Count() == 0)
		{
			//WM_Utility::PrintLog("Incoming data block WM00_CMD is not exist."); //没有传入行车命令数据 
			//return doFlag;

			sprintf(s.msg, "函数f_wmsmsm_crane_up中找不到接收块名[WM00_DOWN]或值为空");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//获取班组班次信息
		f_epep_get_shift_group("SM", dateTime, shiftNo, shiftGroup, conn);


		////定义吊车命令履历表字段
		//WM_Utility::SetDataTableColName("HWM00A7", dtCraneCmdTrace, conn);
		//dtCraneCmdTrace.Rows.Add();

		//循环获取传入行车命令块数据
		for (int i = 0; i < bcls_rec->Tables["WM00_DOWN"].Rows.get_Count(); i++)
		{
			matNo = bcls_rec->Tables["WM00_DOWN"].Rows[i]["MAT_NO"];
			stockPlaceNoTo = bcls_rec->Tables["WM00_DOWN"].Rows[i]["STOCK_PLACE_NO"];

			mat_act_x = 0;
			mat_act_y = 0;
			mat_act_z = 0;
			op_mode = "2";
			if (bcls_rec->Tables["WM00_DOWN"].Columns.Contains("MAT_ACT_X"))
			{
				mat_act_x = bcls_rec->Tables["WM00_DOWN"].Rows[i]["MAT_ACT_X"].ToDecimal();
			}
			if (bcls_rec->Tables["WM00_DOWN"].Columns.Contains("MAT_ACT_Y"))
			{
				mat_act_y = bcls_rec->Tables["WM00_DOWN"].Rows[i]["MAT_ACT_Y"].ToDecimal();
			}
			if (bcls_rec->Tables["WM00_DOWN"].Columns.Contains("MAT_ACT_Z"))
			{
				mat_act_z = bcls_rec->Tables["WM00_DOWN"].Rows[i]["MAT_ACT_Z"].ToDecimal();
			}
			if (bcls_rec->Tables["WM00_DOWN"].Columns.Contains("OP_MODE"))
			{
				Log::Debug("", __FUNCTION__, "1111111111111"); 
				op_mode = bcls_rec->Tables["WM00_DOWN"].Rows[i]["OP_MODE"].ToString().Trim();
				Log::Debug("", __FUNCTION__, "11111op_mode  = [{0}]", op_mode);
			}


			
			Log::Debug("", __FUNCTION__, "MAT_NO  = [{0}]", matNo);
			Log::Debug("", __FUNCTION__, "stockPlaceNoTo  = [{0}]", stockPlaceNoTo);
			Log::Debug("", __FUNCTION__, "mat_act_x  = [{0}]", mat_act_x);
			Log::Debug("", __FUNCTION__, "mat_act_y  = [{0}]", mat_act_y);
			Log::Debug("", __FUNCTION__, "mat_act_z  = [{0}]", mat_act_z);
			Log::Debug("", __FUNCTION__, "op_mode  = [{0}]", op_mode);

			if (matNo.Trim() == "")
			{
				strcpy(s.msg, "没有传入材料号");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			/*if (stockPlaceNoTo.Trim() == "")
			{
				strcpy(s.msg, "没有传入目标垛位");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}*/

			//数据校验
		/*	sqlWhere = "mat_no = '" + matNo + "'";
			WM_Utility::QueryData("TWMA7", sqlWhere, dtCraneCmd, conn);*/
			/*if (dtCraneCmd.Rows.get_Count() == 0)
			{
			sprintf(s.msg, "Mat No.[%s]'s crane instruction is not exist.", (const char*)matNo);
			throw CApplicationException(-1, s.msg, s.svc_name);
			}*/

			twma7["MAT_NO"] = matNo;
			if (twma7.QueryCount("MAT_NO") < 1)
			{
				sprintf(s.msg, "材料【%s】的吊车命令不存在。", (const char*)matNo);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			twma7["MAT_NO"] = matNo;
			twma7.Query("MAT_NO");

			Log::Debug("", __FUNCTION__, "STOCK_PLACE_NO_FROM  = [{0}]", twma7["STOCK_PLACE_NO_FROM"].ToString());
			twm04["STOCK_PLACE_NO"] = twma7["STOCK_PLACE_NO_FROM"];
			if (twm04.Query("STOCK_PLACE_NO") 
				&& twm04["STOCK_PLACE_TYPE"].ToString()=="D"
				&& twm04["DEV_DIV"].ToString() == "3")
			{
				car_no = twm04["STOCK_PLACE_NO"].ToString().Substring(0,4);
			}
			Log::Debug("", __FUNCTION__, "car_no  = [{0}]", car_no);
		

			//取行车命令相关数据

			stockOperOrder = twma7["STOCK_OPER_ORDER"].ToString();
			if (stockOperOrder.Trim() == "")stockOperOrder = "30";

			Log::Debug("", __FUNCTION__, "stockPlaceNoTo.GetLength  = [{0}]", stockPlaceNoTo.Trim().GetLength());
			Log::Debug("", __FUNCTION__, "stockPlaceNoFr.Trim().Substring(0, 1) = [{0}]", stockPlaceNoTo.Trim().Substring(0, 1));
			/*if (stockPlaceNoTo.Trim().GetLength() == 5 && stockPlaceNoTo.Trim().Substring(0,1) == "B")
			{
				layerno = stockPlaceNoTo.Trim().Substring(4, 1);
				stockPlaceNoTo = stockPlaceNoTo.Trim().Substring(0, 4);
			}*/

			if (stockPlaceNoTo.Trim() == "")
			{
				stockPlaceNoTo = twma7["STOCK_PLACE_NO_TO"].ToString().Trim();
			}
			Log::Debug("", __FUNCTION__, "layerno  = [{0}]", layerno);
			Log::Debug("", __FUNCTION__, "stockPlaceNoTo2222  = [{0}]", stockPlaceNoTo);

			twm04["STOCK_PLACE_NO"] = stockPlaceNoTo;
			Log::Trace("", __FUNCTION__, "stockPlaceNoTo33333= [{0}]", twm04["STOCK_PLACE_NO"].ToString());
			if (!twm04.Query("STOCK_PLACE_NO"))
			{
				sprintf(s.msg, "卸下位置 [%s] 不存在。", (const char*)stockPlaceNoTo);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			
			logic_stock = twm04["LOGIC_STOCK_NO"];

			/*sqlWhere = "stock_place_no = '" + stockPlaceNoTo + "'";
			WM_Utility::QueryData("TWM04", sqlWhere, dtStockPlace, conn);
			if (dtStockPlace.Rows.get_Count() == 0)
			{
				sprintf(s.msg, "To position[%s] is not exist.", (const char*)stockPlaceNoTo);
				throw CApplicationException(-1, s.msg, s.svc_name);
			}*/

			stockPlaceType = twm04["STOCK_PLACE_TYPE"].ToString().Trim();
			devDiv = twm04["DEV_DIV"].ToString().Trim();//设备区分代码
			stockNoTo = twm04["STOCK_NO"].ToString().Trim();
			enExDiv = twm04["ENTRANCE_EXIT_DIV"].ToString().Trim();//入出口区分
			manageAccu = twm04["MANAGE_ACCU"].ToString().Trim();
			hall_no = twm04["HALL_NO"].ToString().Trim();

			

			//一品一地
			//if (manageAccu.Trim() == "5")
			//{
			//	sqlstr =
			//		" SELECT mat_no "
			//		" FROM TWMA2 "
			//		" WHERE STOCK_PLACE_NO = @stock_place_no";
			//	cmd_inq.SetCommandText(sqlstr);
			//	cmd_inq.Parameters.Set("stock_place_no", stockPlaceNoTo);
			//	cmd_inq.ExecuteReader();
			//	while (cmd_inq.Read())
			//	{
			//		matNo_pre = cmd_inq.GetString(1);
			//		//Clear the from position
			//		doFlag = f_wm00_move_to_null(matNo_pre, "2", conn);  //1:lift up,  2:lift down
			//		if (doFlag < 0)
			//		{
			//			sprintf(s.msg, "[%s] Error occured when clear the to-position", (const char*)matNo);
			//			throw CApplicationException(-1, s.msg, s.svc_name);
			//		}
			//	}
			//	cmd_inq.Close();
			//}

			//调用仓库出库主函数
			bcls_stock_out.Tables["WM_STOCK"].Rows.Add();
			bcls_stock_out.Tables["WM_STOCK"].Rows[i]["MAT_NO"] = matNo;
			bcls_stock_out.Tables["WM_STOCK"].Rows[i]["STOCK_NO"] = stockNoTo;
			bcls_stock_out.Tables["WM_STOCK"].Rows[i]["STOCK_PLACE_NO"] = stockPlaceNoTo;
			bcls_stock_out.Tables["WM_STOCK"].Rows[i]["ROWNO"] = " ";
			bcls_stock_out.Tables["WM_STOCK"].Rows[i]["COLUMN_NO"] = " ";
			bcls_stock_out.Tables["WM_STOCK"].Rows[i]["LAYERNO"] = 0;
			bcls_stock_out.Tables["WM_STOCK"].Rows[i]["STOCK_PLACE_POSITION"] = " ";
			bcls_stock_out.Tables["WM_STOCK"].Rows[i]["MAT_ACT_X"] = mat_act_x;
			bcls_stock_out.Tables["WM_STOCK"].Rows[i]["MAT_ACT_Y"] = mat_act_y;
			bcls_stock_out.Tables["WM_STOCK"].Rows[i]["MAT_ACT_Z"] = mat_act_z;
			//bcls_stock_out.Tables["WM_STOCK"].Rows[i]["OP_MODE"] = op_mode;

			//调用仓库倒垛主函数
			bcls_stock_move.Tables["WM_STOCK"].Rows.Add();
			bcls_stock_move.Tables["WM_STOCK"].Rows[i]["MAT_NO"] = matNo;
			//bcls_stock_move.Tables["WM_STOCK"].Rows[i]["STOCK_OPER_ORDER"] = stock_oper_order;
			bcls_stock_move.Tables["WM_STOCK"].Rows[i]["STOCK_NO"] = stockNoTo;
			bcls_stock_move.Tables["WM_STOCK"].Rows[i]["STOCK_PLACE_NO"] = stockPlaceNoTo;
			bcls_stock_move.Tables["WM_STOCK"].Rows[i]["ROWNO"] = " ";
			bcls_stock_move.Tables["WM_STOCK"].Rows[i]["COLUMN_NO"] = " ";
			bcls_stock_move.Tables["WM_STOCK"].Rows[i]["LAYERNO"] = 0;
			bcls_stock_move.Tables["WM_STOCK"].Rows[i]["STOCK_PLACE_POSITION"] = " ";
			bcls_stock_move.Tables["WM_STOCK"].Rows[i]["MAT_ACT_X"] = mat_act_x;
			bcls_stock_move.Tables["WM_STOCK"].Rows[i]["MAT_ACT_Y"] = mat_act_y;
			bcls_stock_move.Tables["WM_STOCK"].Rows[i]["MAT_ACT_Z"] = mat_act_z;
			//bcls_stock_move.Tables["WM_STOCK"].Rows[i]["OP_MODE"] = op_mode;

			//调用仓库入库主函数
			bcls_stock_in.Tables["WM_STOCK"].Rows.Add();
			bcls_stock_in.Tables["WM_STOCK"].Rows[i]["MAT_NO"] = matNo;
			//bcls_stock_in.Tables["WM_STOCK"].Rows[i]["STOCK_OPER_ORDER"] = twma0["STOCK_OPER_ORDER"];
			bcls_stock_in.Tables["WM_STOCK"].Rows[i]["STOCK_OPER_ORDER_DIV"] = " ";
			bcls_stock_in.Tables["WM_STOCK"].Rows[i]["STOCK_NO"] = stockNoTo;
			bcls_stock_in.Tables["WM_STOCK"].Rows[i]["STOCK_PLACE_NO"] = stockPlaceNoTo;
			bcls_stock_in.Tables["WM_STOCK"].Rows[i]["ROWNO"] = " ";
			bcls_stock_in.Tables["WM_STOCK"].Rows[i]["COLUMN_NO"] = " ";
			bcls_stock_in.Tables["WM_STOCK"].Rows[i]["LAYERNO"] = 0;
			bcls_stock_in.Tables["WM_STOCK"].Rows[i]["STOCK_PLACE_POSITION"] = " ";
			bcls_stock_in.Tables["WM_STOCK"].Rows[i]["MAT_ACT_X"] = mat_act_x;
			bcls_stock_in.Tables["WM_STOCK"].Rows[i]["MAT_ACT_Y"] = mat_act_y;
			bcls_stock_in.Tables["WM_STOCK"].Rows[i]["MAT_ACT_Z"] = mat_act_z;
			//bcls_stock_in.Tables["WM_STOCK"].Rows[i]["OP_MODE"] = op_mode;

			//修改吊车命令状态
			twma7["MAT_NO"] = matNo;

			twma7["REC_REVISE_TIME"] = dateTime;
			twma7["REC_REVISOR"] = s.userid;
			twma7["CRANE_INST_STATUS"] = "E";
			twma7.Update("CRANE_INST_STATUS,REC_REVISE_TIME,REC_REVISOR", "MAT_NO");

			// lift down in yard or on transfer car
			if (stockOperOrder.Trim().Substring(0, 1) == "1")
			{
				// call stock in function
				bcls_stock_in.Tables["WM_STOCK"].Rows[i]["STOCK_OPER_ORDER"] = stockOperOrder;
					
				doFlag = f_wmsmsm_stock_in(&bcls_stock_in, bcls_ret, conn);
				if (doFlag != 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}

				/*doFlag = f_wm00_stock_in(&bcls_stock, bcls_ret, conn);
				if (doFlag != 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}*/
			}
			else
			{
				Log::Debug("", __FUNCTION__, "2222 ");
				// call stock move function
				bcls_stock_move.Tables["WM_STOCK"].Rows[i]["STOCK_OPER_ORDER"] = "30";
					
				doFlag = f_wmsmsm_stock_move(&bcls_stock_move, bcls_ret, conn);
				if (doFlag != 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}

				/*doFlag = f_wm00_stock_move(&bcls_stock, bcls_ret, conn);
				if (doFlag != 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}*/
			}
			

		
			//调用行车命令函数
			for (int i = 0; i < bcls_rec->Tables["WM00_DOWN"].Rows.get_Count(); i++)
			{
				Log::Trace("", __FUNCTION__, "111");
				v_mat_no = bcls_rec->Tables["WM00_DOWN"].Rows[i]["MAT_NO"];
				bcls_rec_stock_follow.Tables["WM00_FOLLOW"].Rows.Add();
				bcls_rec_stock_follow.Tables["WM00_FOLLOW"].Rows[i]["MAT_NO"] = v_mat_no;
				//sprintf(s.msg, " ");
			}
			if (bcls_rec_stock_follow.Tables["WM00_FOLLOW"].Rows.get_Count() > 0)
			{
				doFlag = f_wmsmsm_cranecmd_follow(&bcls_rec_stock_follow, bcls_ret, conn);
				if (doFlag < 0)
				{
					Log::Trace("", __FUNCTION__, "f_wmsmsm_cranecmd_follow 失败。");
					doFlag = 0;
					//throw CApplicationException(-1, s.msg, log.Location);
				}
			}


			//写吊车命令执行履历
			
			hwma7.CopyFrom(twma7);
			hwma7["REC_CREATOR"] = s.userid;
			hwma7["REC_CREATE_TIME"] = dateTime;
			hwma7["REC_REVISOR"] = " ";
			hwma7["REC_REVISE_TIME"] = " ";
			hwma7["SHIFT_GROUP"] = shiftGroup;
			hwma7["SHIFT_NO"] = shiftNo;
			hwma7["CLIENT_IP"] = s.fore_ip;
			hwma7["SVC_NAME"] = s.svc_name;
			hwma7["REMARK"] = "Lift down";
			
			hwma7.Insert();

			//2022-01-19  卸下后，吊运命令应该删除 。 待最后确认
			twma7.Delete();
					
		}
		
		Log::Debug("", __FUNCTION__, "car_no  = [{0}]", car_no);		

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