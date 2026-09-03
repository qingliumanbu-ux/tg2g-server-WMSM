/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:      JQ
Version:     1.1.1
Date:        2017-3-16
Description: 板坯库命令替换
**************************************************/

/* C/C++ 的标准头文件部分 */
#include "stdafx.h"		// 框架头，不可删除 

BM2_FUNCTION_IMPORT
int f_wm00_pileinfocal(CString stock_no, CString stock_place_no, EIClass * bcls_ret, CDbConnection * conn);  //垛位最大高度、重量修正
int f_wmsmsm_cranecmd_tr_rem(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);  //过跨台车及批次计算
int f_wmsmsm_cranecmd_seq_upt(CString stock_oper_order, EIClass * bcls_ret, CDbConnection * conn);//更新顺序号
//int f_wmsmsm_u1dl03_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_wmsmsm_cranecmd_group(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//计算组吊号
int f_wmsmsm_cranecmd_check(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

BM2_FUNCTION_EXPORT
int f_wmsmsm_cranecmd_update(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	/*程序内部变量*/
	int doFlag = 0;
	CString sqlstr = " ";
	CString dateTime = " ";
	CString sqlstr_update = "UPDATE TWMA7 SET ";
	CString sqlstr_where = " ";
	CString SEQ_NO = " ";
	CDecimal maxseqno = 9999999;
	CDecimal seqno = 0;
	CString stock_no_fin = " ";
	CString stock_place_no_fin = " ";
	int seqno_1 = 0;

	//数据块
	CDataTable Table_mat;                  //存放材料信息
	CDataTable Table_cmd_mat;                  //存放材料信息
	CDataTable Table_mat_send;                  //存放材料信息

	EIClass bcls_rec_tr;
	bcls_rec_tr.Tables[0].set_TableName("CMD_TR_REM");
	bcls_rec_tr.Tables[0].Columns.Add(DT_STRING, "HALL_TO");
	bcls_rec_tr.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
	bcls_rec_tr.Tables[0].Columns.Add(DT_STRING, "HALL_FR");
	bcls_rec_tr.Tables[0].Columns.Add(DT_STRING, "LAYERNO");
	bcls_rec_tr.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO");
	bcls_rec_tr.Tables[0].Columns.Add(DT_STRING, "STOCK_OPER_ORDER_FIN");
	bcls_rec_tr.Tables[0].Columns.Add(DT_DECIMAL, "MAT_THEORY_WT");
	bcls_rec_tr.Tables[0].Columns.Add(DT_DECIMAL, "MAT_THICK");
	bcls_rec_tr.Tables[0].Columns.Add(DT_DECIMAL, "MAT_WIDTH");
	bcls_rec_tr.Tables[0].Columns.Add(DT_DECIMAL, "MAT_LEN");
	bcls_rec_tr.Tables[0].Rows.Add();

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

	EIClass bcls_rec_group;
	bcls_rec_group.Tables[0].set_TableName("CMD_GROUP");
	bcls_rec_group.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO");
	bcls_rec_group.Tables["CMD_GROUP"].Rows.Add();

	//调用cranecmd_check函数
	EIClass cranecmd_check;
	cranecmd_check.Tables[0].set_TableName("CMD_CHECK");
	cranecmd_check.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
	cranecmd_check.Tables[0].Columns.Add(DT_STRING, "SEND_FLAG");
	cranecmd_check.Tables[0].Rows.Add();

	//定义表实体对象
	CModel twma7 = CModel("TWMA7");

	try
	{
		// 系统日志
		CTracer log(__FUNCTION__);  

		//取系统时间
		dateTime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		//判断是否存在指定块
		if (bcls_rec->Tables.IndexOf("CMD_UPDATE") < 0 ||
			bcls_rec->Tables["CMD_UPDATE"].Rows.get_Count() == 0)
		{
			sprintf(s.msg, "函数f_wm00_cranecmd_update中找不到接收块名[CMD_UPDATE]");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		CString send_flag = "0";
		for (int i = 0; i < bcls_rec->Tables["CMD_UPDATE"].Rows.get_Count(); i++)
		{
			Log::Trace("", __FUNCTION__, "替换材料{0}命令", bcls_rec->Tables["CMD_UPDATE"].Rows[i]["MAT_NO"].ToString());
			
			if (bcls_rec->Tables["CMD_UPDATE"].Columns.Contains("SEND_FLAG"))
			send_flag = bcls_rec->Tables["CMD_UPDATE"].Rows[i]["SEND_FLAG"].ToString();

			Log::Trace("", __FUNCTION__, "send_flag:{0}", send_flag);
			if (bcls_rec->Tables["CMD_UPDATE"].Rows[i]["CRANE_INST_STATUS"].ToString()!= "0"
				&&bcls_rec->Tables["CMD_UPDATE"].Rows[i]["STOCK_OPER_ORDER_NEW"].ToString().Trim()!="32")
			{
				sprintf(s.msg, "命令状态不为0");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (bcls_rec->Tables["CMD_UPDATE"].Rows[i]["STOCK_NO_TO"].ToString().Trim() == "")
			{
				sprintf(s.msg, "目标库区为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (bcls_rec->Tables["CMD_UPDATE"].Rows[i]["HALL_NO_TO"].ToString().Trim() == "")
			{
				sprintf(s.msg, "目标跨为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			

			//更新当前材料位置
			sqlstr_update = "UPDATE TWMA7 SET ";
			sqlstr_update = sqlstr_update + " REC_REVISOR ='" + s.userid + "'";
			sqlstr_update = sqlstr_update + ",REC_ERASE_TIME ='" + dateTime + "'";
			sqlstr_update = sqlstr_update + ",STOCK_NO ='" + bcls_rec->Tables["CMD_UPDATE"].Rows[i]["STOCK_NO"].ToString().Trim() + "'";
			sqlstr_update = sqlstr_update + ",STOCK_NO_FROM ='" + bcls_rec->Tables["CMD_UPDATE"].Rows[i]["STOCK_NO_FROM"].ToString().Trim() + "'";
			sqlstr_update = sqlstr_update + ",HALL_NO_FR ='" + bcls_rec->Tables["CMD_UPDATE"].Rows[i]["HALL_NO_FR"].ToString().Trim() + "'";
			sqlstr_update = sqlstr_update + ",STOCK_PLACE_NO_FROM ='" + bcls_rec->Tables["CMD_UPDATE"].Rows[i]["STOCK_PLACE_NO_FROM"].ToString().Trim() + "'";
			sqlstr_update = sqlstr_update + ",YARD_LAYER_FROM =" + bcls_rec->Tables["CMD_UPDATE"].Rows[i]["YARD_LAYER_FROM"].ToString().Trim();
			sqlstr_update = sqlstr_update + ",CRANE_CMDGRPNO =0";
			sqlstr_update = sqlstr_update + ",STOCK_OPER_ORDER ='" + bcls_rec->Tables["CMD_UPDATE"].Rows[i]["STOCK_OPER_ORDER_NEW"].ToString().Trim() + "'";
			sqlstr_update = sqlstr_update + ",MOVE_TYPE ='" + bcls_rec->Tables["CMD_UPDATE"].Rows[i]["STOCK_OPER_ORDER_NEW"].ToString().Trim() + "'";
		
			SEQ_NO = "SEQ_" + bcls_rec->Tables["CMD_UPDATE"].Rows[i]["STOCK_OPER_ORDER_NEW"].ToString().Trim();
			sqlstr = "values nextval for " + SEQ_NO;
			seqno = Db::QueryCDecimal(sqlstr);
			sqlstr_update = sqlstr_update + ",CMD_SEQ =" + seqno.ToString() + "";
			if (seqno== maxseqno)
			{
				doFlag = f_wmsmsm_cranecmd_seq_upt(bcls_rec->Tables["CMD_UPDATE"].Rows[i]["STOCK_OPER_ORDER_NEW"].ToString().Trim(), bcls_ret, conn);
				if (doFlag != 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}					
			}
			
		
			if (bcls_rec->Tables["CMD_UPDATE"].Rows[i]["STOCK_OPER_ORDER_NEW"].ToString().Trim() == "32")
			{
				if (bcls_rec->Tables["CMD_UPDATE"].Rows[i]["DEV_DIV"].ToString().Trim() == "3")
				{
					sqlstr_update = sqlstr_update + ",STOCK_PLACE_NO_TO ='" + bcls_rec->Tables["CMD_UPDATE"].Rows[i]["STOCK_PLACE_NO_TO_NEW"].ToString().Trim() + "'";
					//sqlstr_update = sqlstr_update + ",CRANE_CMDGRPNO =0";
				}
				else 
				{
					sqlstr = "SELECT MAT_NO,HALL_NO_TO,STOCK_OPER_ORDER,HALL_NO_FR,STOCK_PLACE_NO_FROM,STOCK_PLACE_NO_FIN,MAT_ACT_WT,MAT_ACT_THICK,MAT_ACT_WIDTH,MAT_ACT_LEN,YARD_LAYER_FROM  "
						" FROM TWMA7 WHERE MAT_NO='" + bcls_rec->Tables["CMD_UPDATE"].Rows[i]["MAT_NO"].ToString() + "'";
					Db::QueryTable(sqlstr, Table_mat);
					if (Table_mat.Rows.get_Count()>0)
					{
						if (Table_mat.Rows[0]["STOCK_OPER_ORDER"].ToString().Trim() != "32")
						{
							//过跨命令
							bcls_rec_tr.Tables["CMD_TR_REM"].Rows[0]["MAT_NO"] = bcls_rec->Tables["CMD_UPDATE"].Rows[i]["MAT_NO"].ToString();
							bcls_rec_tr.Tables["CMD_TR_REM"].Rows[0]["HALL_TO"] = bcls_rec->Tables["CMD_UPDATE"].Rows[i]["HALL_NO_TO"];
							bcls_rec_tr.Tables["CMD_TR_REM"].Rows[0]["HALL_FR"] = bcls_rec->Tables["CMD_UPDATE"].Rows[i]["HALL_NO_FR"];
							bcls_rec_tr.Tables["CMD_TR_REM"].Rows[0]["STOCK_PLACE_NO"] = bcls_rec->Tables["CMD_UPDATE"].Rows[i]["STOCK_PLACE_NO_FROM"];
							bcls_rec_tr.Tables["CMD_TR_REM"].Rows[0]["STOCK_OPER_ORDER_FIN"] = bcls_rec->Tables["CMD_UPDATE"].Rows[i]["STOCK_OPER_ORDER_FIN"];
							bcls_rec_tr.Tables["CMD_TR_REM"].Rows[0]["MAT_THEORY_WT"] = Table_mat.Rows[0]["MAT_ACT_WT"];
							bcls_rec_tr.Tables["CMD_TR_REM"].Rows[0]["MAT_THICK"] = Table_mat.Rows[0]["MAT_ACT_THICK"];
							bcls_rec_tr.Tables["CMD_TR_REM"].Rows[0]["MAT_WIDTH"] = Table_mat.Rows[0]["MAT_ACT_WIDTH"];
							bcls_rec_tr.Tables["CMD_TR_REM"].Rows[0]["MAT_LEN"] = Table_mat.Rows[0]["MAT_ACT_LEN"];
							bcls_rec_tr.Tables["CMD_TR_REM"].Rows[0]["LAYERNO"] = Table_mat.Rows[0]["YARD_LAYER_FROM"];

							doFlag = f_wmsmsm_cranecmd_tr_rem(&bcls_rec_tr, bcls_ret, conn);
							if (doFlag != 0)
							{
								throw CApplicationException(-1, s.msg, log.Location);
							}
							sqlstr_update = sqlstr_update + ",STOCK_PLACE_NO_TO ='" + bcls_rec->Tables["CMD_UPDATE"].Rows[i]["STOCK_PLACE_NO_TO_NEW"].ToString().Trim() + "'";

							//sqlstr_update = sqlstr_update + ",STOCK_PLACE_NO_TO ='" + bcls_ret->Tables[0].Rows[0]["STOCK_PLACE_NO"].ToString().Trim() + "'";
							//sqlstr_update = sqlstr_update + ",BATCH_NO = " + bcls_ret->Tables[0].Rows[0]["BATCH_NO"].ToString().Trim();
							//bcls_rec->Tables["CMD_UPDATE"].Rows[i]["STOCK_PLACE_NO_TO_NEW"] = bcls_ret->Tables[0].Rows[0]["STOCK_PLACE_NO"].ToString().Trim();
							//sqlstr_update = sqlstr_update + ",CRANE_CMDGRPNO =0";
						}
					}				
				}


			}
			else
			{
				sqlstr_update = sqlstr_update + ",STOCK_PLACE_NO_TO ='" + bcls_rec->Tables["CMD_UPDATE"].Rows[i]["STOCK_PLACE_NO_TO_NEW"].ToString().Trim() + "'";
				if (bcls_rec->Tables["CMD_UPDATE"].Columns.Contains("BATCH_NO") &&
					bcls_rec->Tables["CMD_UPDATE"].Rows[i]["BATCH_NO"].ToDecimal() != 0)
				{
					//sqlstr_update = sqlstr_update + ",BATCH_NO ='" + bcls_rec->Tables["CMD_UPDATE"].Rows[i]["BATCH_NO"].ToString().Trim() + "'";				
				}
				else
				{
					//sqlstr_update = sqlstr_update + ",BATCH_NO =0";
				}
				//sqlstr_update = sqlstr_update + ",CRANE_CMDGRPNO =0";
			}
			
		
			//更新材料目标位置
			sqlstr_update = sqlstr_update + ",STOCK_NO_TO ='" + bcls_rec->Tables["CMD_UPDATE"].Rows[i]["STOCK_NO_TO"].ToString().Trim() + "'";
			sqlstr_update = sqlstr_update + ",HALL_NO_TO ='" + bcls_rec->Tables["CMD_UPDATE"].Rows[i]["HALL_NO_TO"].ToString().Trim() + "'";	

			if (bcls_rec->Tables["CMD_UPDATE"].Columns.Contains("LOGIC_STOCK_NO") &&
				bcls_rec->Tables["CMD_UPDATE"].Rows[i]["LOGIC_STOCK_NO"].ToString().Trim() != "")
			{
				sqlstr_update = sqlstr_update + ",LOGIC_STOCK_NO ='" + bcls_rec->Tables["CMD_UPDATE"].Rows[i]["LOGIC_STOCK_NO"].ToString().Trim() + "'";
			}
			else
			{
				sqlstr_update = sqlstr_update + ",LOGIC_STOCK_NO =' '";
			}



			if (bcls_rec->Tables["CMD_UPDATE"].Columns.Contains("BATCH_TASK_NO") &&
				bcls_rec->Tables["CMD_UPDATE"].Rows[i]["BATCH_TASK_NO"].ToDecimal() != 0)
			{
				sqlstr_update = sqlstr_update + ",BATCH_TASK_NO ='" + bcls_rec->Tables["CMD_UPDATE"].Rows[i]["BATCH_TASK_NO"].ToString().Trim() + "'";
			}

			if (bcls_rec->Tables["CMD_UPDATE"].Columns.Contains("STOCK_OPER_ORDER_FIN") &&
				bcls_rec->Tables["CMD_UPDATE"].Rows[i]["STOCK_OPER_ORDER_FIN"].ToString().Trim() != "")
			{
				sqlstr_update = sqlstr_update + ",STOCK_OPER_ORDER_FIN ='" + bcls_rec->Tables["CMD_UPDATE"].Rows[i]["STOCK_OPER_ORDER_FIN"].ToString().Trim() + "'";			
			}
			else
			{
				sqlstr_update = sqlstr_update + ",STOCK_OPER_ORDER_FIN ='"+bcls_rec->Tables["CMD_UPDATE"].Rows[i]["STOCK_OPER_ORDER_NEW"].ToString().Trim()+ "'";
			}

			if (bcls_rec->Tables["CMD_UPDATE"].Columns.Contains("STOCK_NO_FIN") &&
				bcls_rec->Tables["CMD_UPDATE"].Rows[i]["STOCK_NO_FIN"].ToString().Trim() != "")
			{
				stock_no_fin = bcls_rec->Tables["CMD_UPDATE"].Rows[i]["STOCK_NO_FIN"].ToString().Trim();
				sqlstr_update = sqlstr_update + ",STOCK_NO_FIN ='" + bcls_rec->Tables["CMD_UPDATE"].Rows[i]["STOCK_NO_FIN"].ToString().Trim() + "'";
			}
			else
			{
				stock_no_fin = "";
				sqlstr_update = sqlstr_update + ",STOCK_NO_FIN ='" + bcls_rec->Tables["CMD_UPDATE"].Rows[i]["STOCK_NO_TO"].ToString().Trim() + "'";
			}

			if (bcls_rec->Tables["CMD_UPDATE"].Columns.Contains("HALL_NO_FIN") &&
				bcls_rec->Tables["CMD_UPDATE"].Rows[i]["HALL_NO_FIN"].ToString().Trim() != "")
			{
				sqlstr_update = sqlstr_update + ",HALL_NO_FIN ='" + bcls_rec->Tables["CMD_UPDATE"].Rows[i]["HALL_NO_FIN"].ToString().Trim() + "'";
			}
			else
			{
				sqlstr_update = sqlstr_update + ",HALL_NO_FIN ='" + bcls_rec->Tables["CMD_UPDATE"].Rows[i]["HALL_NO_TO"].ToString().Trim() + "'";
			}

			if (bcls_rec->Tables["CMD_UPDATE"].Columns.Contains("UNIT_CODE") &&
				bcls_rec->Tables["CMD_UPDATE"].Rows[i]["UNIT_CODE"].ToString().Trim() != "")
			{
				sqlstr_update = sqlstr_update + ",UNIT_CODE ='" + bcls_rec->Tables["CMD_UPDATE"].Rows[i]["UNIT_CODE"].ToString().Trim() + "'";
			}
			else
			{
				sqlstr_update = sqlstr_update + ",UNIT_CODE =' '";
			}

			if (bcls_rec->Tables["CMD_UPDATE"].Columns.Contains("STOCK_PLACE_NO_FIN") &&
				bcls_rec->Tables["CMD_UPDATE"].Rows[i]["STOCK_PLACE_NO_FIN"].ToString().Trim() != "")
			{
				stock_place_no_fin = bcls_rec->Tables["CMD_UPDATE"].Rows[i]["STOCK_PLACE_NO_FIN"].ToString().Trim();
				sqlstr_update = sqlstr_update + ",STOCK_PLACE_NO_FIN ='" + bcls_rec->Tables["CMD_UPDATE"].Rows[i]["STOCK_PLACE_NO_FIN"].ToString().Trim() + "'";
			}
			else
			{
				stock_place_no_fin = "";
				sqlstr_update = sqlstr_update + ",STOCK_PLACE_NO_FIN =' '";
			}



			if (bcls_rec->Tables["CMD_UPDATE"].Columns.Contains("VEHICLE_NO"))
			{
				sqlstr_update = sqlstr_update + ",VEHICLE_NO ='" + bcls_rec->Tables["CMD_UPDATE"].Rows[i]["VEHICLE_NO"].ToString().Trim() + "'";
			}
			else
			{
				sqlstr_update = sqlstr_update + ",VEHICLE_NO =' '";
			}
			
			//sqlstr_update = sqlstr_update + ",SEND_FLAG ='0'";
			sqlstr_update = sqlstr_update + " WHERE MAT_NO ='" + bcls_rec->Tables["CMD_UPDATE"].Rows[i]["MAT_NO"].ToString() + "'";
			Log::Trace("", __FUNCTION__, "sqlstr_update[{0}]", sqlstr_update);

			twma7.Reset();
			twma7["MAT_NO"] = bcls_rec->Tables["CMD_UPDATE"].Rows[i]["MAT_NO"].ToString();
			twma7.Query("MAT_NO");
			Log::Trace("", __FUNCTION__, "4。7。4");
			Db::Execute(sqlstr_update);
			Log::Trace("", __FUNCTION__, "4。7。4");
			//更新垛位状态
			doFlag = f_wm00_pileinfocal(twma7["STOCK_NO_FIN"].ToString(), twma7["STOCK_PLACE_NO_FIN"].ToString(), bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (stock_no_fin!="" && stock_place_no_fin!="")
			{
				//更新垛位状态
				doFlag = f_wm00_pileinfocal(stock_no_fin, stock_place_no_fin, bcls_ret, conn);
				if (doFlag != 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}


			if (bcls_rec->Tables["CMD_UPDATE"].Rows[i]["STOCK_PLACE_NO_TO_OLD"].ToString().Trim() != "")
			{
				//更新垛位状态
				doFlag = f_wm00_pileinfocal(bcls_rec->Tables["CMD_UPDATE"].Rows[i]["STOCK_NO"].ToString(), bcls_rec->Tables["CMD_UPDATE"].Rows[i]["STOCK_PLACE_NO_TO_OLD"].ToString(), bcls_ret, conn);
				if (doFlag != 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			if (bcls_rec->Tables["CMD_UPDATE"].Rows[i]["STOCK_PLACE_NO_TO_NEW"].ToString().Trim() != "")
			{
				doFlag = f_wm00_pileinfocal(bcls_rec->Tables["CMD_UPDATE"].Rows[i]["STOCK_NO"].ToString(), bcls_rec->Tables["CMD_UPDATE"].Rows[i]["STOCK_PLACE_NO_TO_NEW"].ToString(), bcls_ret, conn);
				if (doFlag != 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}

			//重算组吊号
			//bcls_rec_group.Tables["CMD_GROUP"].Rows[0]["STOCK_PLACE_NO"] = bcls_rec->Tables["CMD_UPDATE"].Rows[i]["STOCK_PLACE_NO_FROM"].ToString().Trim();
			//doFlag = f_wmsmsm_cranecmd_group(&bcls_rec_group, bcls_ret, conn);
			//if (doFlag != 0)
			//{
			//	throw CApplicationException(-1, s.msg, log.Location);
			//}

			cranecmd_check.Tables["CMD_CHECK"].Rows[0]["MAT_NO"] = bcls_rec->Tables["CMD_UPDATE"].Rows[i]["MAT_NO"].ToString();
			cranecmd_check.Tables["CMD_CHECK"].Rows[0]["SEND_FLAG"]=send_flag;

			doFlag = f_wmsmsm_cranecmd_check(&cranecmd_check, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}


			
		}

		
		


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