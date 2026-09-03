/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:      QL
Version:     1.1.1
Date:        2017-08-04
Description: 吊车命令吊起函数
**************************************************/

//#include "WM_Utility.h"
#include "stdafx.h"
 
BM2_FUNCTION_IMPORT		//垛位清空

int f_auto(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

int f_wm00_pileinfocal(CString stock_no, CString stock_place_no, EIClass * bcls_ret, CDbConnection * conn);

BM2_FUNCTION_EXPORT
int f_wmsmsm_crane_up(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	//程序内部变量
	int doFlag = 0;
	CString sqlstr = " ";

	//应用变量
	int cmdSeq = 0;
	CString matNo = " ";
	CString sqlWhere = " ";
	CString sqlWherePlace = " ";
	CString shiftGroup = " ";
	CString stock_logic_no = " ";
	CString shiftNo = " ";
	//CString flag_op = " ";
	CString stockNoFr = " ";
	CString stockPlaceNoFr = " ";
	CString tcNo = " ";
	CString logic_stock = " ";
	CString dateTime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	//定义表实体对象
	CModel hwma7 = CModel("HWMA7");
	CModel twma7 = CModel("TWMA7");
	CModel tmmsm01 = CModel("TMMSM01");
	CModel twm04 = CModel("TWM04");

	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	


	//调用 向加热炉L2发送板坯吊出B炉实绩
	EIClass bcls_sec_u1c13r;
	bcls_sec_u1c13r.Tables[0].set_TableName("U1C13R");
	bcls_sec_u1c13r.Tables[0].Columns.Add(DT_STRING, "mat_no");
	bcls_sec_u1c13r.Tables[0].Rows.Clear();

	//调用 向二切L2发送吊起实绩电文
	EIClass bcls_rec_u1f104;
	bcls_rec_u1f104.Tables[0].set_TableName("U1F104");
	bcls_rec_u1f104.Tables[0].Columns.Add(DT_STRING, "OPER_FLAG");
	bcls_rec_u1f104.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
	bcls_rec_u1f104.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO");
	bcls_rec_u1f104.Tables[0].Rows.Clear();

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
	//推荐逻辑区域
	EIClass bcls_rec_auto_1;
	bcls_rec_auto_1.Tables[0].set_TableName("AUTO_INFO_IN");
	bcls_rec_auto_1.Tables[0].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");
	bcls_rec_auto_1.Tables[0].Columns.Add(DT_STRING, "HALL_NO");
	bcls_rec_auto_1.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
	bcls_rec_auto_1.Tables[0].Rows.Add();

	//推荐逻辑区域结果
	EIClass bcls_ret_logic;


	try
	{
		//判断是否存在指定块
		if (bcls_rec->Tables.IndexOf("WM00_UP") < 0 ||
			bcls_rec->Tables["WM00_UP"].Rows.get_Count() == 0)
		{
			//WM_Utility::PrintLog("Incoming data block WM00_CMD is not exist."); //没有传入行车命令数据 
			//return doFlag;

			sprintf(s.msg, "函数f_wmsmsm_crane_up中找不到接收块名[WM00_UP]或值为空");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//获取班组班次信息
		f_epep_get_shift_group("SM", dateTime, shiftNo, shiftGroup, conn);

		////定义吊车命令履历表字段
		//WM_Utility::SetDataTableColName("hwma7", dtCraneCmdTrace, conn);
		//dtCraneCmdTrace.Rows.Add();

		//循环获取传入行车命令块数据
		for (int i = 0; i < bcls_rec->Tables["WM00_UP"].Rows.get_Count(); i++)
		{
			matNo = bcls_rec->Tables["WM00_UP"].Rows[i]["MAT_NO"];

			//stockPlaceNoFr = bcls_rec->Tables["WM00_UP"].Rows[i]["STOCK_PLACE_NO_FR"];
		/*	if (bcls_rec->Tables["WM00_UP"].Columns.Contains("FLAG_OP"))
			{
				flag_op = bcls_rec->Tables["WM00_UP"].Rows[i]["FLAG_OP"].ToString().Trim();
			}*/
			//WM_Utility::PrintLog("matNo", matNo);
			Log::Debug("", __FUNCTION__, "MAT_NO  = [{0}]", matNo);
		//	Log::Debug("", __FUNCTION__, "stockPlaceNoFr  = [{0}]", stockPlaceNoFr);
			
			if (matNo.Trim() == "")
			{
				strcpy(s.msg, "没有传入材料号");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		/*	if (stockPlaceNoFr.Trim() == "")
			{
				strcpy(s.msg, "没有传入起始位置");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}*/
			//数据校验

			tmmsm01["MAT_NO"] = matNo;
			
			if (tmmsm01.QueryCount("MAT_NO") < 1 )
			{
				sprintf(s.msg, "材料【%s】不存在。", (const char*)tmmsm01["MAT_NO"]);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			
			twma7["MAT_NO"] = matNo;
			if (twma7.QueryCount("MAT_NO") < 1)
			{
				//sprintf(s.msg, "材料【%s】的吊车命令不存在。", (const char*)twma7["MAT_NO"]);
				//throw CApplicationException(-1, s.msg, log.Location);
			}
		    // 吊车命令状态    R:选定确认    S : 吊上     E : 卸下     C : 选定取消
			twma7["MAT_NO"] = matNo;
			twma7.Query("MAT_NO");

			Log::Debug("", __FUNCTION__, "CRANE_INST_STATUS  = [{0}]", twma7["CRANE_INST_STATUS"].ToString().Trim());
			if (twma7["CRANE_INST_STATUS"].ToString().Trim() == "S" || twma7["CRANE_INST_STATUS"].ToString().Trim() == "E")
			{
				sprintf(s.msg, "材料 [%s] 的当前吊车命令状态不允许吊起操作。", (const char*)matNo);
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			//判断材料上方是否有材料压坯       发送吊起电文给相关L2

		stockPlaceNoFr = twma7["STOCK_PLACE_NO_FROM"];

			if (stockPlaceNoFr.Trim().GetLength() == 5 && stockPlaceNoFr.Trim().Substring(0, 1) == "B")
			{
			//	layerno = stockPlaceNoTo.Trim().Substring(4, 1);
				Log::Debug("", __FUNCTION__, "stockPlaceNoFr.Trim().Substring(0, 1) = [{0}]", stockPlaceNoFr.Trim().Substring(0, 1));
				stockPlaceNoFr = stockPlaceNoFr.Trim().Substring(0, 4);
			}
			
			twm04["STOCK_PLACE_NO"] = stockPlaceNoFr;
			if (!twm04.Query("STOCK_PLACE_NO"))
			{
				sprintf(s.msg, "吊起位置 [%s] 不存在。", (const char*)stockPlaceNoFr);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			stock_logic_no = twm04["LOGIC_STOCK_NO"];
			//Log::Debug("", __FUNCTION__, "flag_op = [{0}]", flag_op);
			//if (flag_op != "1")
			//{
				if (twm04["DEV_DIV"].ToString().Trim() == "1")
				{
					sqlstr = "SELECT COUNT(1) FROM twma2 a WHERE EXISTS(SELECT 1 FROM twma2 b WHERE a.stock_no = b.stock_no"
						" AND a.vehicle_no = b.vehicle_no"
						" AND a.stock_place_no = b.stock_place_no AND INT(a.layerno) > INT(b.layerno) AND b.mat_no = '" + matNo + "')"
						" AND a.stock_place_no in (select stock_place_no from twm04 where dev_div = '1')"
						" AND a.mat_no not in (SELECT mat_no FROM twma7 WHERE crane_inst_status='S')"; //added by tawei
				}
				else
				{
					sqlstr = "SELECT COUNT(1) FROM twma2 a WHERE EXISTS(SELECT 1 FROM twma2 b WHERE a.stock_no = b.stock_no"
						" AND a.stock_place_no = b.stock_place_no AND INT(a.layerno) > INT(b.layerno) AND b.mat_no = '" + matNo + "')"
						" AND a.mat_no not in (SELECT mat_no FROM twma7 WHERE crane_inst_status='S')"; //added by tawei
				}
				//if (WM_Utility::QueryDataCount(sqlstr, conn) > 0)
				Log::Trace("", "", "sqlstr：{0}", sqlstr);
				if (Db::QueryCDecimal(sqlstr) > 0)
				{
					sprintf(s.msg, "材料 [%s] 上方有材料，无法吊起！", (const char*)matNo);
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			//}
			//修改吊车命令状态
			twma7["MAT_NO"] = matNo;

			twma7["REC_REVISE_TIME"] = datetime;
			twma7["REC_REVISOR"] = s.userid;
			twma7["CRANE_INST_STATUS"] = "S";
			twma7.Update("CRANE_INST_STATUS,REC_REVISE_TIME,REC_REVISOR", "MAT_NO");
			
			//写吊车命令执行履历

			hwma7.Reset();
		/*	twma4.CopyFrom(twma1_old);
			dtCraneCmdTrace.Rows[0].Merge(dtCraneCmd.Rows[0]);*/
			hwma7.CopyFrom(twma7);
			hwma7["REC_CREATOR"] = s.userid;
			hwma7["REC_CREATE_TIME"] = dateTime;
			hwma7["REC_REVISOR"] = " ";
			hwma7["REC_REVISE_TIME"] = " ";
			hwma7["SHIFT_GROUP"] = shiftGroup;
			hwma7["SHIFT_NO"] = shiftNo;
			hwma7["CLIENT_IP"] = s.fore_ip;
			hwma7["SVC_NAME"] = s.svc_name;
			hwma7["REMARK"] = "Lift up";

			hwma7.Insert();


			Log::Trace("", __FUNCTION__, "matNo=[{0}]", matNo);

			if (twma7["MAT_NO"])
			//doFlag = f_wm00_move_to_null(matNo, "1", conn);  //1:lift up,  2:lift down
			if (doFlag < 0)
			{
				//sprintf(s.msg, " ");
				//throw CApplicationException(-1, s.msg, log.Location);
				sprintf(s.msg, "[%s] Error occured when clear the from-position", (const char*)matNo);
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}

		//吊起位是临时区时，对入库命令没有推荐出跺位的重新推荐
		if (stock_logic_no=="07"
			&&Db::QueryCDecimal("select count(1) from twm04 where logic_stock_no='07' and stock_status!='0'")>0)
		{
			sqlstr = "select * from twma7 where stock_oper_order='1B' and stock_place_no_to='' and CRANE_INST_STATUS='0'";
			Log::Trace("", __FUNCTION__, "sqlstr=[{0}]", sqlstr);
			CDataTable T_rem;
			Db::QueryTable(sqlstr, T_rem);
			for (int i = 0; i < T_rem.Rows.get_Count(); i++)
			{
				bcls_rec_auto_1.Tables[0].Rows[0]["MAT_NO"] = T_rem.Rows[i]["MAT_NO"].ToString();
				bcls_rec_auto_1.Tables[0].Rows[0]["STOCK_OPER_ORDER"] = T_rem.Rows[i]["STOCK_OPER_ORDER"];
				bcls_rec_auto_1.Tables[0].Rows[0]["HALL_NO"] = T_rem.Rows[i]["HALL_NO_TO"];
				doFlag = f_auto(&bcls_rec_auto_1, &bcls_ret_logic, conn);
				if (doFlag != 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
				twma7.Reset();
				twma7["MAT_NO"] = T_rem.Rows[i]["MAT_NO"].ToString();
				twma7["LOGIC_STOCK_NO"] = bcls_ret_logic.Tables[0].Rows[0]["LOGIC_STOCK_NO"].ToString();
				twma7["STOCK_PLACE_NO_TO"] = bcls_ret_logic.Tables[0].Rows[0]["STOCK_PLACE_NO"].ToString();
				twma7.Update("STOCK_PLACE_NO_TO,LOGIC_STOCK_NO", "MAT_NO");

				bcls_rec_send.Tables[0].Rows.Add();
				bcls_rec_send.Tables[0].Rows[i]["mat_no"] = T_rem.Rows[i]["MAT_NO"].ToString();
				bcls_rec_send.Tables[0].Rows[i]["stock_oper_order"] = T_rem.Rows[i]["STOCK_OPER_ORDER"].ToString();
				bcls_rec_send.Tables[0].Rows[i]["cmd_seq"] = T_rem.Rows[i]["CMD_SEQ"].ToDecimal();
				bcls_rec_send.Tables[0].Rows[i]["crane_cmdgrpno"] = T_rem.Rows[i]["CRANE_CMDGRPNO"].ToDecimal();
				bcls_rec_send.Tables[0].Rows[i]["batch_no"] = T_rem.Rows[i]["BATCH_TASK_NO"].ToDecimal();
				bcls_rec_send.Tables[0].Rows[i]["stock_place_no_from"] = T_rem.Rows[i]["STOCK_PLACE_NO_FROM"].ToString();
				bcls_rec_send.Tables[0].Rows[i]["yard_layer_from"] = T_rem.Rows[i]["YARD_LAYER_FROM"].ToString();
				bcls_rec_send.Tables[0].Rows[i]["stock_place_no_to"] = twma7["STOCK_PLACE_NO_TO"].ToString();
				bcls_rec_send.Tables[0].Rows[i]["stock_oper_order_fin"] = T_rem.Rows[i]["STOCK_OPER_ORDER_FIN"].ToString();
				bcls_rec_send.Tables[0].Rows[i]["remark"] = "up";

				doFlag = f_wm00_pileinfocal(T_rem.Rows[i]["STOCK_NO_TO"].ToString(), twma7["STOCK_PLACE_NO_TO"].ToString(), bcls_ret, conn);
				if (doFlag != 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
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
