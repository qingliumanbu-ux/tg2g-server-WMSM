/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:      张凌辉
Version:     1.1.1
Date:        2016-07-22 10:52:08
Description: 行车命令组吊判断函数
**************************************************/

#include "WM_Utility.h"

BM2_FUNCTION_EXPORT
int f_wmsm_craneCmdGrp_judge(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	//程序内部变量
	int doFlag = 0;
	CString sqlstr = " ";

	//应用变量
	int pointRow = 0;
	int curRow = 0;
	int craneGrpNum = 0;

	CString pointStockPlaceNoFr = "";
	CString pointStockPlaceNoTo = "";
	CString curStockPlaceNoFr = "";
	CString curStockPlaceNoTo = "";

	CDecimal pointLayerNoFr = 0;
	CDecimal curLayerNoFr = 0;

	try
	{
		CTracer log(__FUNCTION__);

		if (bcls_rec->Tables.IndexOf("WM00_CMD") < 0)
		{
			strcpy(s.msg, "Incoming data block WM00_CMD is not exist.");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		if (!bcls_rec->Tables["WM00_CMD"].Columns.Contains("STOCK_PLACE_NO_FROM"))
		{
			strcpy(s.msg, "No incoming data of From-position.");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		if (!bcls_rec->Tables["WM00_CMD"].Columns.Contains("LAYERNO_FROM"))
		{
			strcpy(s.msg, "No incoming data of From-layer no.");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		if (!bcls_rec->Tables["WM00_CMD"].Columns.Contains("CMD_METHOD"))
		{
			bcls_rec->Tables["WM00_CMD"].Columns.Add(DT_STRING, "CMD_METHOD");
		}

		while (pointRow < bcls_rec->Tables["WM00_CMD"].Rows.get_Count() &&
			curRow < bcls_rec->Tables["WM00_CMD"].Rows.get_Count())
		{
			if (pointRow == curRow)
			{
				pointStockPlaceNoFr = bcls_rec->Tables["WM00_CMD"].Rows[pointRow]["STOCK_PLACE_NO_FROM"];
				pointLayerNoFr = bcls_rec->Tables["WM00_CMD"].Rows[pointRow]["LAYERNO_FROM"];

				if (bcls_rec->Tables["WM00_CMD"].Columns.Contains("STOCK_PLACE_NO_TO"))
				{
					pointStockPlaceNoTo = bcls_rec->Tables["WM00_CMD"].Rows[pointRow]["STOCK_PLACE_NO_TO"];
				}
				else
				{
					pointStockPlaceNoTo = " ";
				}

				Log::Trace("", __FUNCTION__, "pointRow = [{0}]", pointRow);
				Log::Trace("", __FUNCTION__, "pointStockPlaceNoFr = [{0}]", pointStockPlaceNoFr);
				Log::Trace("", __FUNCTION__, "pointLayerNoFr = [{0}]", pointLayerNoFr);
				Log::Trace("", __FUNCTION__, "pointStockPlaceNoTo = [{0}]", pointStockPlaceNoTo);

				bcls_rec->Tables["WM00_CMD"].Rows[pointRow]["CMD_METHOD"] = "1";

				craneGrpNum = 1;
				curRow++;
			}
			else
			{
				curStockPlaceNoFr = bcls_rec->Tables["WM00_CMD"].Rows[curRow]["STOCK_PLACE_NO_FROM"];
				curLayerNoFr = bcls_rec->Tables["WM00_CMD"].Rows[curRow]["LAYERNO_FROM"];
				
				if (bcls_rec->Tables["WM00_CMD"].Columns.Contains("STOCK_PLACE_NO_TO"))
				{
					curStockPlaceNoTo = bcls_rec->Tables["WM00_CMD"].Rows[curRow]["STOCK_PLACE_NO_TO"];
				}
				else
				{
					curStockPlaceNoTo = " ";
				}

				Log::Trace("", __FUNCTION__, "curRow = [{0}]", curRow);
				Log::Trace("", __FUNCTION__, "curStockPlaceNoFr = [{0}]", curStockPlaceNoFr);
				Log::Trace("", __FUNCTION__, "curLayerNoFr = [{0}]", curLayerNoFr);
				Log::Trace("", __FUNCTION__, "curStockPlaceNoTo = [{0}]", curStockPlaceNoTo);

				if (curStockPlaceNoFr == pointStockPlaceNoFr && curStockPlaceNoTo == pointStockPlaceNoTo &&
					(pointLayerNoFr - curLayerNoFr) == 1 && craneGrpNum < 3)
				{
					Log::Trace("", __FUNCTION__, "符合一次多吊条件");

					bcls_rec->Tables["WM00_CMD"].Rows[pointRow]["CMD_METHOD"] = "2";
					bcls_rec->Tables["WM00_CMD"].Rows[curRow]["CMD_METHOD"] = "2";

					craneGrpNum++;
					curRow++;
				}
				else
				{
					Log::Trace("", __FUNCTION__, "不符合一次多吊条件");
					pointRow = curRow;
				}
			}
		}

		//WM_Utility::PrintLog("组吊判断结束");
		//WM_Utility::PrintDataTable(bcls_rec->Tables["WM00_CMD"]);
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


