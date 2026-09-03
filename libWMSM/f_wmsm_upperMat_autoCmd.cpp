/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:      张凌辉
Version:     1.1.1
Date:        2016-07-22 10:52:08
Description: 上层物料自动倒垛命令生成函数
**************************************************/

#include "stdafx.h"

//外部函数调用
BM2_FUNCTION_IMPORT
int f_wmsm_craneCmdGrp_judge(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);		//行车命令组吊判断

BM2_FUNCTION_IMPORT
int f_wmsm_craneCmdMake(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);			//吊车命令形成

BM2_FUNCTION_EXPORT
int f_wmsm_upperMat_autoCmd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	//程序内部变量
	int doFlag = 0;
	CString sqlstr = " ";

	//应用变量
	int rowNo = 0;
	int existFlag1 = 0;
	int existFlag2 = 0;

	CString stockPlaceNo = "";
	CDecimal layerNo = 0;

	EIClass bcls_rec_cmd;
	EIClass bcls_ret_cmd;

	CDataTable dtUpperMat;

	//数据库操作类定义
	CDbCommand cmd_inq(conn);

	try
	{
		CTracer log(__FUNCTION__);

		if (bcls_rec->Tables.IndexOf("WM00_CMD") < 0)
		{
			strcpy(s.msg, "Incoming data block WM00_CMD is not exist.");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		//if (!bcls_rec->Tables["WM00_CMD"].Columns.Contains("STOCK_PLACE_NO_FROM"))
		//{
		//	strcpy(s.msg, "没有传入源库位号");
		//	throw CApplicationException(-1, s.msg, s.svc_name);
		//}

		//if (!bcls_rec->Tables["WM00_CMD"].Columns.Contains("LAYERNO_FROM"))
		//{
		//	strcpy(s.msg, "没有传入源库位层号");
		//	throw CApplicationException(-1, s.msg, s.svc_name);
		//}

		bcls_rec_cmd.Tables.Add("WM00_CMD");
		bcls_rec_cmd.Tables["WM00_CMD"].Columns.Add(DT_STRING, "MAT_NO");
		bcls_rec_cmd.Tables["WM00_CMD"].Columns.Add(DT_STRING, "STOCK_NO_FROM");
		bcls_rec_cmd.Tables["WM00_CMD"].Columns.Add(DT_STRING, "STOCK_PLACE_NO_FROM");
		bcls_rec_cmd.Tables["WM00_CMD"].Columns.Add(DT_STRING, "LAYERNO_FROM");
		bcls_rec_cmd.Tables["WM00_CMD"].Columns.Add(DT_STRING, "STOCK_NO_TO");
		bcls_rec_cmd.Tables["WM00_CMD"].Columns.Add(DT_STRING, "STOCK_PLACE_NO_TO");
		bcls_rec_cmd.Tables["WM00_CMD"].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");
		bcls_rec_cmd.Tables["WM00_CMD"].Columns.Add(DT_STRING, "MOVE_TYPE");
		bcls_rec_cmd.Tables["WM00_CMD"].Columns.Add(DT_STRING, "UNIT_CODE");
		bcls_rec_cmd.Tables["WM00_CMD"].Columns.Add(DT_STRING, "UPPER_FLAG");

		Log::Trace("", __FUNCTION__, "WM00_CMD RowCount[{0}]", bcls_rec->Tables["WM00_CMD"].Rows.get_Count());
		for (int i = 0; i < bcls_rec->Tables["WM00_CMD"].Rows.get_Count(); i++)
		{
			if (bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_OPER_ORDER"].ToString().Substring(0, 1) == "1")
			{
				continue;
			}

			if (stockPlaceNo.Trim() != bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_PLACE_NO_FROM"].ToString())
			{
				if (bcls_rec_cmd.Tables["WM00_CMD"].Rows.get_Count() > 0)
				{
					//行车命令组吊判断
					doFlag = f_wmsm_craneCmdGrp_judge(&bcls_rec_cmd, &bcls_ret_cmd, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}

					//生成行车命令
					doFlag = f_wmsm_craneCmdMake(&bcls_rec_cmd, &bcls_ret_cmd, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}

					//清空WM00_CMD块内数据
					bcls_rec_cmd.Tables["WM00_CMD"].Rows.Clear();
					rowNo = 0;
				}

				stockPlaceNo = bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_PLACE_NO_FROM"];
				layerNo = bcls_rec->Tables["WM00_CMD"].Rows[i]["LAYERNO_FROM"];

				if (layerNo < 1)
				{
					strcpy(s.msg, "No layer no."); //没有传入源层号
					throw CApplicationException(-1, s.msg, log.Location);
				}

				Log::Trace("", __FUNCTION__, "stockPlaceNo[{0}]", stockPlaceNo);
				Log::Trace("", __FUNCTION__, "layerNo[{0}]", layerNo);
			}

			sqlstr = "SELECT mat_no,stock_no,stock_place_no,layerno FROM twma2 t WHERE stock_place_no = '" + stockPlaceNo +
				"' AND layerno > " + layerNo.ToString() + "AND NOT EXISTS (SELECT 1 FROM twma7 WHERE mat_no = t.mat_no)"
				" ORDER BY layerno DESC";
			Log::Trace("", __FUNCTION__, "sqlstr[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			
			while (cmd_inq.Read())
			{
				existFlag1 = 0;
				Log::Trace("", __FUNCTION__, "33333");
				//在本函数中已经生成倒垛行车命令的材料跳过
				for (int i = 0; i < bcls_rec_cmd.Tables["WM00_CMD"].Rows.get_Count(); i++)
				{
					if (cmd_inq.GetString(1) == bcls_rec_cmd.Tables["WM00_CMD"].Rows[i]["MAT_NO"].ToString())
					{
						existFlag1 = 1;
						break;
					}
				}
				Log::Trace("", __FUNCTION__, "2222222");
				//在主函数中生成行车命令的材料设置最终位置
				for (int i = 0; i < bcls_rec->Tables["WM00_CMD"].Rows.get_Count(); i++)
				{
					if (cmd_inq.GetString(1) == bcls_rec->Tables["WM00_CMD"].Rows[i]["MAT_NO"].ToString())
					{
						existFlag2 = 1;
						break;
					}
				}
				
				if (existFlag1 == 0 && existFlag2 == 0)
				{
					bcls_rec_cmd.Tables["WM00_CMD"].Rows.Add();
					bcls_rec_cmd.Tables["WM00_CMD"].Rows[rowNo]["MAT_NO"] = cmd_inq.GetString(1);
					bcls_rec_cmd.Tables["WM00_CMD"].Rows[rowNo]["STOCK_NO_FROM"] = cmd_inq.GetString(2);
					bcls_rec_cmd.Tables["WM00_CMD"].Rows[rowNo]["STOCK_PLACE_NO_FROM"] = cmd_inq.GetString(3);
					bcls_rec_cmd.Tables["WM00_CMD"].Rows[rowNo]["LAYERNO_FROM"] = cmd_inq.GetDecimal(4);
					bcls_rec_cmd.Tables["WM00_CMD"].Rows[rowNo]["STOCK_OPER_ORDER"] = "31";
					bcls_rec_cmd.Tables["WM00_CMD"].Rows[rowNo]["MOVE_TYPE"] = "31";//bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_OPER_ORDER"];

					if (bcls_rec->Tables["WM00_CMD"].Columns.Contains("UNIT_CODE"))
					{
						bcls_rec_cmd.Tables["WM00_CMD"].Rows[rowNo]["UNIT_CODE"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["UNIT_CODE"];
					}

					bcls_rec_cmd.Tables["WM00_CMD"].Rows[rowNo]["STOCK_NO_TO"] = bcls_rec->Tables["WM00_CMD"].Rows[i]["STOCK_NO_TO"];

					rowNo++;

					Log::Trace("", __FUNCTION__, "[{0}]：材料[{1}],源库位[{2}],层号[{3}],倒垛行车命令",
						rowNo, cmd_inq.GetString(1), cmd_inq.GetString(3), cmd_inq.GetDecimal(4));
				}
			}
			cmd_inq.Close();
		}
		Log::Trace("", __FUNCTION__, "44444444");
		if (bcls_rec_cmd.Tables["WM00_CMD"].Rows.get_Count() > 0)
		{
			Log::Trace("", __FUNCTION__, "555555");
			//行车命令组吊判断
			doFlag = f_wmsm_craneCmdGrp_judge(&bcls_rec_cmd, &bcls_ret_cmd, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}

			//生成行车命令
			doFlag = f_wmsm_craneCmdMake(&bcls_rec_cmd, &bcls_ret_cmd, conn);
			if (doFlag < 0)
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


