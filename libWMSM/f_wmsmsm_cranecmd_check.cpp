/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:      JQ
Version:     1.1.1
Date:        2017-3-16
Description: 垛位命令检查
**************************************************/

/* C/C++ 的标准头文件部分 */
#include "stdafx.h"		// 框架头，不可删除 

//int f_wmsmsm_cranecmd_make(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
//int f_wmsmsm_cranecmd_delete(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
//int f_wmsmsm_u1dl03_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_wmsmsm_cranecmd_group(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

BM2_FUNCTION_EXPORT
int f_wmsmsm_cranecmd_check(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{

	//程序内部变量
	int doFlag = 0; 
	int cmdFlag = 0;
	int seqno = 0;
	int seqno1 = 0;
	CString sqlstr = " "; 
	CString send_flag = " ";

	//数据存放块
	CDataTable cmd_mat;                             //存放材料
	CDataTable cmd_layerno;

	CModel twma7 = CModel("TWMA7");
	
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

	try
	{
		//项目自定义日志
		CTracer log(__FUNCTION__);

		//检验传入数据块
		if (!bcls_rec->Tables.Contains("CMD_CHECK") || bcls_rec->Tables["CMD_CHECK"].Rows.get_Count() == 0)
		{
			sprintf(s.msg, "函数f_wmsmsm_cranecmd_check中找不到接收块名[CMD_CHECK]或值为空");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		for (int i = 0; i < bcls_rec->Tables["CMD_CHECK"].Rows.get_Count(); i++)
		{
			Log::Trace("", __FUNCTION__, "传入材料：【{0}】", bcls_rec->Tables["CMD_CHECK"].Rows[i]["MAT_NO"].ToString());
			if (bcls_rec->Tables["CMD_CHECK"].Rows[i]["MAT_NO"].ToString().Trim() == "")
			{
				continue;
			}
			if (bcls_rec->Tables["CMD_CHECK"].Columns.Contains("SEND_FLAG"))
				send_flag = bcls_rec->Tables["CMD_CHECK"].Rows[i]["SEND_FLAG"].ToString();
			Log::Trace("", __FUNCTION__, "传入send_flag：【{0}】", send_flag);

			//检查命令组吊号
			twma7["MAT_NO"] = bcls_rec->Tables["CMD_CHECK"].Rows[i]["MAT_NO"].ToString().Trim();
			if (!twma7.Query("MAT_NO") )
			{
				Log::Trace("", __FUNCTION__, "传入材料没有命令");
				continue;
			}

			if (twma7["CRANE_INST_STATUS"].ToString().Trim() != "0")
			{
				Log::Trace("", __FUNCTION__, "传入材料命令状态不为0");
				continue;
			}
			bcls_rec_send.Tables[0].Rows.Clear();

			//判断是否有台车到台车的指令，有则不检查
			CDecimal count = Db::QueryCDecimal("SELECT COUNT(1) FROM TWMA7 A,TWMA2 B WHERE A.MAT_NO=B.MAT_NO AND A.STOCK_PLACE_NO_FROM!=B.STOCK_PLACE_NO AND B.STOCK_PLACE_NO='" + twma7["STOCK_PLACE_NO_FROM"].ToString().Trim()+"'");
			Log::Trace("", __FUNCTION__, "count{0}", count);
			if (count>0)
			{
				Log::Trace("", __FUNCTION__, "有台车到台车的指令不检查");
				continue;
			}

			if (twma7["CRANE_CMDGRPNO"].ToDecimal()==0)
			{
				Log::Trace("", __FUNCTION__, "传入命令没有组吊号");
				//计算整个库位组吊号
				bcls_rec_group.Tables["CMD_GROUP"].Rows[0]["STOCK_PLACE_NO"] = twma7["STOCK_PLACE_NO_FROM"].ToString().Trim();

				doFlag = f_wmsmsm_cranecmd_group(&bcls_rec_group, bcls_ret, conn);
				if (doFlag != 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
				


			}
			else
			{
				Log::Trace("", __FUNCTION__, "传入命令有组吊号");
				//判断是否修改目标位置
				sqlstr = " SELECT COUNT(1) FROM(SELECT DISTINCT STOCK_PLACE_NO_TO FROM TWMA7 WHERE CRANE_CMDGRPNO = INT('" + twma7["CRANE_CMDGRPNO"].ToString() + "'))";
					Log::Trace("", __FUNCTION__, "sqlstr={0}", sqlstr);
				if (Db::QueryCDecimal(sqlstr)==1)
				{
					Log::Trace("", __FUNCTION__, "没有修改目标位置");
					continue;
				}

				sqlstr = " SELECT * FROM ("
					" SELECT COUNT(1) AS COUNT_UP FROM TWMA7 WHERE CRANE_CMDGRPNO = INT('" + twma7["CRANE_CMDGRPNO"].ToString() + "') AND YARD_LAYER_FROM>INT('" + twma7["YARD_LAYER_FROM"].ToString() + "')"
					" ) A, (SELECT COUNT(1) AS COUNT_DOWN FROM TWMA7 WHERE CRANE_CMDGRPNO = INT('" + twma7["CRANE_CMDGRPNO"].ToString() + "') AND YARD_LAYER_FROM<INT('" + twma7["YARD_LAYER_FROM"].ToString() + "')) B";
				Log::Trace("", __FUNCTION__, "sqlstr={0}", sqlstr);
				Db::QueryTable(sqlstr, cmd_layerno);

				if (cmd_layerno.Rows[0]["COUNT_UP"].ToDecimal() > 0
					&& cmd_layerno.Rows[0]["COUNT_DOWN"].ToDecimal() > 0)
				{
					Log::Trace("", __FUNCTION__, "材料在中间");
					Log::Trace("", __FUNCTION__, "COUNT_UP {0}", cmd_layerno.Rows[0]["COUNT_UP"].ToDecimal());
					Log::Trace("", __FUNCTION__, "COUNT_DOWN {0}", cmd_layerno.Rows[0]["COUNT_DOWN"].ToDecimal());
					if (cmd_layerno.Rows[0]["COUNT_UP"].ToDecimal() < 2
						&& cmd_layerno.Rows[0]["COUNT_DOWN"].ToDecimal() < 2)
					{
						//全部替换为0
						sqlstr = "SELECT * FROM TWMA7 WHERE CRANE_CMDGRPNO = INT('" + twma7["CRANE_CMDGRPNO"].ToString() + "')";
						Db::QueryTable(sqlstr, cmd_mat);
					}
					else
					{
						if (cmd_layerno.Rows[0]["COUNT_UP"].ToDecimal() >= 2)
						{
							//下层替换为0
							sqlstr = "SELECT * FROM TWMA7 WHERE CRANE_CMDGRPNO = INT('" + twma7["CRANE_CMDGRPNO"].ToString() + "') AND YARD_LAYER_FROM<=INT('" + twma7["YARD_LAYER_FROM"].ToString() + "')";
							Db::QueryTable(sqlstr, cmd_mat);
						}
						else
						{
							//上层替换为0
							sqlstr = "SELECT * FROM TWMA7 WHERE CRANE_CMDGRPNO = INT('" + twma7["CRANE_CMDGRPNO"].ToString() + "') AND YARD_LAYER_FROM>=INT('" + twma7["YARD_LAYER_FROM"].ToString() + "')";
							Db::QueryTable(sqlstr, cmd_mat);
						}
					}
				}
				else
				{
					Log::Trace("", __FUNCTION__, "材料不在中间");
					if (cmd_layerno.Rows[0]["COUNT_UP"].ToDecimal() > 2
						|| cmd_layerno.Rows[0]["COUNT_DOWN"].ToDecimal() > 2)
					{
						//替换一个为0
						sqlstr = "SELECT * FROM TWMA7 WHERE CRANE_CMDGRPNO = INT('" + twma7["CRANE_CMDGRPNO"].ToString() + "') AND YARD_LAYER_FROM=INT('" + twma7["YARD_LAYER_FROM"].ToString() + "')";
						Db::QueryTable(sqlstr, cmd_mat);
					}
					else
					{
						//全部替换为0
						sqlstr = "SELECT * FROM TWMA7 WHERE CRANE_CMDGRPNO = INT('" + twma7["CRANE_CMDGRPNO"].ToString() + "')";
						Db::QueryTable(sqlstr, cmd_mat);
					}
				}
				Log::Trace("", __FUNCTION__, "sqlstr={0}", sqlstr);

				for (int tt = 0; tt < cmd_mat.Rows.get_Count(); tt++)
				{
					twma7.Reset();
					twma7["MAT_NO"] = cmd_mat.Rows[tt]["MAT_NO"].ToString();
					twma7["CRANE_CMDGRPNO"] = 0;
					twma7.Update("CRANE_CMDGRPNO", "MAT_NO");

					bcls_rec_send.Tables[0].Rows.Add();
					bcls_rec_send.Tables[0].Rows[tt]["mat_no"] = cmd_mat.Rows[tt]["MAT_NO"].ToString();
					bcls_rec_send.Tables[0].Rows[tt]["stock_oper_order"] = cmd_mat.Rows[tt]["STOCK_OPER_ORDER"].ToString();
					bcls_rec_send.Tables[0].Rows[tt]["cmd_seq"] = cmd_mat.Rows[tt]["CMD_SEQ"].ToDecimal();
					bcls_rec_send.Tables[0].Rows[tt]["crane_cmdgrpno"] = twma7["CRANE_CMDGRPNO"].ToDecimal();
					bcls_rec_send.Tables[0].Rows[tt]["batch_no"] = cmd_mat.Rows[tt]["BATCH_NO"].ToDecimal();
					bcls_rec_send.Tables[0].Rows[tt]["stock_place_no_from"] = cmd_mat.Rows[tt]["STOCK_PLACE_NO_FROM"].ToString();
					bcls_rec_send.Tables[0].Rows[tt]["yard_layer_from"] = cmd_mat.Rows[tt]["YARD_LAYER_FROM"].ToString();
					bcls_rec_send.Tables[0].Rows[tt]["stock_place_no_to"] = cmd_mat.Rows[tt]["STOCK_PLACE_NO_TO"].ToString();
					bcls_rec_send.Tables[0].Rows[tt]["stock_oper_order_fin"] = cmd_mat.Rows[tt]["STOCK_OPER_ORDER_FIN"].ToString();
					bcls_rec_send.Tables[0].Rows[tt]["remark"] = "check";
				}
				if (cmd_mat.Rows.get_Count() > 0 && send_flag != "1")
				{
					/*doFlag = f_wmsmsm_u1dl03_snd(&bcls_rec_send, bcls_ret, conn);
					if (doFlag != 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}*/
				}
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