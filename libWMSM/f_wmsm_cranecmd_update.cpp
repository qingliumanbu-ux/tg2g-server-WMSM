/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:      JQ
Version:     1.1.1
Date:        2016-1-9
Description: 板坯库命令替换
**************************************************/

/* C/C++ 的标准头文件部分 */
#include "stdafx.h"		// 框架头，不可删除 
//#include "twma7.h"
//#include "twma2.h"
//#include "twma0.h"

int f_wm00_pileinfocal(CString stock_no, CString stock_place_no, EIClass * bcls_ret, CDbConnection * conn);  //垛位最大高度、重量修正

BM2_FUNCTION_EXPORT
int f_wmsm_cranecmd_update(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	/*程序内部变量*/
	int doFlag = 0;
	CString sqlstr = " ";
	CString dateTime = " ";
	CString update = " ";

	/*程序业务变量*/
	CString stock_oper_order_old = " ";
	CString stock_oper_order_new = " ";
	CString stock_no_old = " ";
	CString stock_place_no_old = " ";
	CString stock_no_new = " ";
	CString stock_place_no_new = " ";

	/*定义表实体对象*/
	//CTWMA7   twma7(conn);
	//CTWMA7   twma7_old(conn);
	//CTWMA2   twma2(conn);
	//CTWMA0   twma0(conn);
	CModel twma7 = CModel("TWMA7");
	CModel twma7_old = CModel("TWMA7");
	CModel twma2 = CModel("TWMA2");
	CModel twma0 = CModel("TWMA0");
	
	try
	{
		CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

		//取系统时间
		dateTime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		//判断是否存在指定块
		if (bcls_rec->Tables.IndexOf("WM00_CMDUPDATE") < 0 ||
			bcls_rec->Tables["WM00_CMDUPDATE"].Rows.get_Count() == 0)
		{
			Log::Trace("", __FUNCTION__, "没有传入数据");
			return doFlag;
		}

		for (int i = 0; i < bcls_rec->Tables["WM00_CMDUPDATE"].Rows.get_Count(); i++)
		{			
			stock_oper_order_new = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["STOCK_OPER_ORDER_NEW"].ToString().Trim();
			Log::Trace("", __FUNCTION__, "stock_oper_order_new {0}", stock_oper_order_new);
			twma7_old["MAT_NO"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["MAT_NO"].ToString();
			if (twma7_old.Query("MAT_NO"))
			{
				stock_no_old = twma7_old["STOCK_NO"].ToString();
				stock_place_no_old = twma7_old["STOCK_PLACE_NO_TO"].ToString();
			}
			else
			{
				continue;
			}
			if (stock_oper_order_new == "2C")
			{
				Log::Trace("", __FUNCTION__, "材料{0}替换为备料命令",bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["MAT_NO"].ToString());
				twma2["MAT_NO"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["MAT_NO"].ToString();
				twma2.Query("MAT_NO");

				twma7.Reset();
				twma7["REC_REVISOR"] = s.userid;
				twma7["REC_ERASE_TIME"] = dateTime;
				twma7["MAT_NO"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["MAT_NO"].ToString();
				twma7["STOCK_NO"] = twma2["STOCK_NO"].ToString();
				twma7["HALL_NO_FR"] = twma2["HALL_NO"].ToString();
				twma7["STOCK_NO_FROM"] = twma2["STOCK_NO"].ToString();
				twma7["STOCK_PLACE_NO_FROM"] = twma2["STOCK_PLACE_NO"].ToString();
				twma7["YARD_LAYER_FROM"] = twma2["LAYERNO"].ToDecimal();

				twma7["STOCK_NO_TO"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["STOCK_NO_TO"];
				twma7["STOCK_PLACE_NO_TO"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["STOCK_PLACE_NO_TO"];
				twma7["STOCK_PLACE_NO_FIN"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["STOCK_PLACE_NO_FIN"];
				twma7["STOCK_OPER_ORDER"] = stock_oper_order_new;
				twma7["STOCK_OPER_ORDER_FIN"] = stock_oper_order_new;
				twma7["MOVE_TYPE"] = stock_oper_order_new;
				twma7["UNIT_CODE"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["UNIT_CODE"];
				twma7["VEHICLE_NO"] = " ";
				if (bcls_rec->Tables["WM00_CMDUPDATE"].Columns.Contains("YARD_LAYER_TO"))
				{
					twma7["YARD_LAYER_TO"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["YARD_LAYER_TO"].ToDecimal();
				}
				else
				{
					twma7["YARD_LAYER_TO"] = 0;
				}

				if (bcls_rec->Tables["WM00_CMDUPDATE"].Columns.Contains("CRANE_CMDGRPNO"))
				{
					twma7["CRANE_CMDGRPNO"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["CRANE_CMDGRPNO"].ToDecimal();
				}
				else
				{
					twma7["CRANE_CMDGRPNO"] = 0;
				}

				if (bcls_rec->Tables["WM00_CMDUPDATE"].Columns.Contains("HALL_NO_TO"))
				{
					twma7["HALL_NO_TO"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["HALL_NO_TO"].ToString();
				}
				else
				{
					twma7["HALL_NO_TO"] = " ";
				}

				if (bcls_rec->Tables["WM00_CMDUPDATE"].Columns.Contains("STOCK_NO_FIN"))
				{
					twma7["STOCK_NO_FIN"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["STOCK_NO_FIN"].ToString();
				}
				else
				{
					twma7["STOCK_NO_FIN"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["STOCK_NO_TO"];
				}

				if (bcls_rec->Tables["WM00_CMDUPDATE"].Columns.Contains("HALL_NO_FIN"))
				{
					twma7["HALL_NO_FIN"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["HALL_NO_FIN"].ToString();
				}
				else
				{
					twma7["HALL_NO_FIN"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["STOCK_NO_TO"];
				}
			 update = "REC_REVISOR,REC_ERASE_TIME,STOCK_NO,HALL_NO_FR,STOCK_NO_FROM,STOCK_PLACE_NO_FROM,YARD_LAYER_FROM,STOCK_NO_TO,STOCK_PLACE_NO_TO,STOCK_PLACE_NO_FIN,STOCK_OPER_ORDER,STOCK_OPER_ORDER_FIN,MOVE_TYPE,UNIT_CODE,VEHICLE_NO,YARD_LAYER_TO,CRANE_CMDGRPNO,HALL_NO_TO,STOCK_NO_FIN,HALL_NO_FIN";
				twma7.Update(update, "MAT_NO");
			}
			else if (stock_oper_order_new == "31")
			{
				Log::Trace("", __FUNCTION__, "材料{0}替换为倒跺命令", bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["MAT_NO"].ToString());
				twma2["MAT_NO"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["MAT_NO"].ToString();
				twma2.Query("MAT_NO");

				twma7.Reset();
				twma7["REC_REVISOR"] = s.userid;
				twma7["REC_ERASE_TIME"] = dateTime;
				twma7["MAT_NO"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["MAT_NO"].ToString();
				twma7["STOCK_NO"] = twma2["STOCK_NO"].ToString();
				twma7["HALL_NO_FR"] = twma2["HALL_NO"].ToString();
				twma7["STOCK_NO_FROM"] = twma2["STOCK_NO"].ToString();
				twma7["STOCK_PLACE_NO_FROM"] = twma2["STOCK_PLACE_NO"].ToString();
				twma7["YARD_LAYER_FROM"] = twma2["LAYERNO"].ToDecimal();

				twma7["STOCK_NO_TO"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["STOCK_NO_TO"];
				twma7["STOCK_PLACE_NO_TO"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["STOCK_PLACE_NO_TO"];
				twma7["STOCK_PLACE_NO_FIN"] = " ";
				twma7["STOCK_OPER_ORDER"] = stock_oper_order_new;
				twma7["STOCK_OPER_ORDER_FIN"] = " ";
				twma7["MOVE_TYPE"] = stock_oper_order_new;
				twma7["UNIT_CODE"] = " ";
				twma7["VEHICLE_NO"] = " ";
				if (bcls_rec->Tables["WM00_CMDUPDATE"].Columns.Contains("YARD_LAYER_TO"))
				{
					twma7["YARD_LAYER_TO"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["YARD_LAYER_TO"].ToDecimal();
				}
				else
				{
					twma7["YARD_LAYER_TO"] = 0;
				}

				if (bcls_rec->Tables["WM00_CMDUPDATE"].Columns.Contains("CRANE_CMDGRPNO"))
				{
					twma7["CRANE_CMDGRPNO"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["CRANE_CMDGRPNO"].ToDecimal();
				}
				else
				{
					twma7["CRANE_CMDGRPNO"] = 0;
				}

				if (bcls_rec->Tables["WM00_CMDUPDATE"].Columns.Contains("HALL_NO_TO"))
				{
					twma7["HALL_NO_TO"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["HALL_NO_TO"].ToString();
				}
				else
				{
					twma7["HALL_NO_TO"] = " ";
				}

				if (bcls_rec->Tables["WM00_CMDUPDATE"].Columns.Contains("STOCK_NO_FIN"))
				{
					twma7["STOCK_NO_FIN"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["STOCK_NO_FIN"].ToString();
				}
				else
				{
					twma7["STOCK_NO_FIN"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["STOCK_NO_TO"];
				}

				if (bcls_rec->Tables["WM00_CMDUPDATE"].Columns.Contains("HALL_NO_FIN"))
				{
					twma7["HALL_NO_FIN"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["HALL_NO_FIN"].ToString();
				}
				else
				{
					twma7["HALL_NO_FIN"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["STOCK_NO_TO"];
				}
				 update = "REC_REVISOR,REC_ERASE_TIME,STOCK_NO,HALL_NO_FR,STOCK_NO_FROM,STOCK_PLACE_NO_FROM,YARD_LAYER_FROM,STOCK_NO_TO,STOCK_PLACE_NO_TO,STOCK_PLACE_NO_FIN,STOCK_OPER_ORDER,STOCK_OPER_ORDER_FIN,MOVE_TYPE,UNIT_CODE,VEHICLE_NO,YARD_LAYER_TO,CRANE_CMDGRPNO,HALL_NO_TO,STOCK_NO_FIN,HALL_NO_FIN";
				twma7.Update(update, "MAT_NO");
			}
			else if (stock_oper_order_new == "2E")
			{
				Log::Trace("", __FUNCTION__, "材料{0}替换为发货命令", bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["MAT_NO"].ToString());
				twma2["MAT_NO"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["MAT_NO"].ToString();
				twma2.Query("MAT_NO");

				twma7.Reset();
				twma7["REC_REVISOR"] = s.userid;
				twma7["REC_ERASE_TIME"] = dateTime;
				twma7["MAT_NO"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["MAT_NO"].ToString();
				twma7["STOCK_NO"] = twma2["STOCK_NO"].ToString();
				twma7["HALL_NO_FR"] = twma2["HALL_NO"].ToString();
				twma7["STOCK_NO_FROM"] = twma2["STOCK_NO"].ToString();
				twma7["STOCK_PLACE_NO_FROM"] = twma2["STOCK_PLACE_NO"].ToString();
				twma7["YARD_LAYER_FROM"] = twma2["LAYERNO"].ToDecimal();

				twma7["STOCK_NO_TO"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["STOCK_NO_TO"];
				twma7["STOCK_PLACE_NO_TO"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["STOCK_PLACE_NO_TO"];
				twma7["STOCK_PLACE_NO_FIN"] = twma7["STOCK_PLACE_NO_TO"].ToString();
				twma7["STOCK_OPER_ORDER"] = stock_oper_order_new;
				twma7["STOCK_OPER_ORDER_FIN"] = stock_oper_order_new;
				twma7["MOVE_TYPE"] = stock_oper_order_new;
				twma7["UNIT_CODE"] = " ";
				twma7["VEHICLE_NO"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["VEHICLE_NO"];
				if (bcls_rec->Tables["WM00_CMDUPDATE"].Columns.Contains("YARD_LAYER_TO"))
				{
					twma7["YARD_LAYER_TO"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["YARD_LAYER_TO"].ToDecimal();
				}
				else
				{
					twma7["YARD_LAYER_TO"] = 0;
				}

				if (bcls_rec->Tables["WM00_CMDUPDATE"].Columns.Contains("CRANE_CMDGRPNO"))
				{
					twma7["CRANE_CMDGRPNO"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["CRANE_CMDGRPNO"].ToDecimal();
				}
				else
				{
					twma7["CRANE_CMDGRPNO"] = 0;
				}

				if (bcls_rec->Tables["WM00_CMDUPDATE"].Columns.Contains("HALL_NO_TO"))
				{
					twma7["HALL_NO_TO"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["HALL_NO_TO"].ToString();
				}
				else
				{
					twma7["HALL_NO_TO"] = " ";
				}

				if (bcls_rec->Tables["WM00_CMDUPDATE"].Columns.Contains("STOCK_NO_FIN"))
				{
					twma7["STOCK_NO_FIN"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["STOCK_NO_FIN"].ToString();
				}
				else
				{
					twma7["STOCK_NO_FIN"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["STOCK_NO_TO"];
				}

				if (bcls_rec->Tables["WM00_CMDUPDATE"].Columns.Contains("HALL_NO_FIN"))
				{
					twma7["HALL_NO_FIN"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["HALL_NO_FIN"].ToString();
				}
				else
				{
					twma7["HALL_NO_FIN"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["STOCK_NO_TO"];
				}
				update = "REC_REVISOR,REC_ERASE_TIME,STOCK_NO,HALL_NO_FR,STOCK_NO_FROM,STOCK_PLACE_NO_FROM,YARD_LAYER_FROM,STOCK_NO_TO,STOCK_PLACE_NO_TO,STOCK_PLACE_NO_FIN,STOCK_OPER_ORDER,STOCK_OPER_ORDER_FIN,MOVE_TYPE,UNIT_CODE,VEHICLE_NO,YARD_LAYER_TO,CRANE_CMDGRPNO,HALL_NO_TO,STOCK_NO_FIN,HALL_NO_FIN";
				twma7.Update(update, "MAT_NO");
			}
			else if (stock_oper_order_new.Trim().Substring(0,1) == "1")
			{
				Log::Trace("", __FUNCTION__, "材料{0}替换为入库命令", bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["MAT_NO"].ToString());
				twma2["MAT_NO"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["MAT_NO"].ToString();
				twma2.Query("MAT_NO");

				twma0["MAT_NO"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["MAT_NO"].ToString();

				twma7.Reset();
				twma7["REC_REVISOR"] = s.userid;
				twma7["REC_ERASE_TIME"] = dateTime;
				twma7["MAT_NO"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["MAT_NO"].ToString();
				twma7["STOCK_NO"] = twma2["STOCK_NO"].ToString();
				twma7["HALL_NO_FR"] = twma2["HALL_NO"].ToString();
				twma7["STOCK_NO_FROM"] = twma2["STOCK_NO"].ToString();
				twma7["STOCK_PLACE_NO_FROM"] = twma2["STOCK_PLACE_NO"].ToString();
				twma7["YARD_LAYER_FROM"] = twma2["LAYERNO"].ToDecimal();

				twma7["STOCK_NO_TO"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["STOCK_NO_TO"];
				twma7["STOCK_PLACE_NO_TO"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["STOCK_PLACE_NO_TO"];
				twma7["STOCK_PLACE_NO_FIN"] = " ";
				twma7["STOCK_OPER_ORDER"] = stock_oper_order_new;
				twma7["STOCK_OPER_ORDER_FIN"] = stock_oper_order_new;
				twma7["MOVE_TYPE"] = stock_oper_order_new;
				twma7["UNIT_CODE"] = " ";
				if (twma0.Query("MAT_NO"))
				{
					twma7["VEHICLE_NO"] = twma0["VEHICLE_NO"].ToString();
				}
				else
				{
					twma7["VEHICLE_NO"] = " ";
				}
				
				if (bcls_rec->Tables["WM00_CMDUPDATE"].Columns.Contains("YARD_LAYER_TO"))
				{
					twma7["YARD_LAYER_TO"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["YARD_LAYER_TO"].ToDecimal();
				}
				else
				{
					twma7["YARD_LAYER_TO"] = 0;
				}

				if (bcls_rec->Tables["WM00_CMDUPDATE"].Columns.Contains("CRANE_CMDGRPNO"))
				{
					twma7["CRANE_CMDGRPNO"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["CRANE_CMDGRPNO"].ToDecimal();
				}
				else
				{
					twma7["CRANE_CMDGRPNO"] = 0;
				}

				if (bcls_rec->Tables["WM00_CMDUPDATE"].Columns.Contains("HALL_NO_TO"))
				{
					twma7["HALL_NO_TO"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["HALL_NO_TO"].ToString();
				}
				else
				{
					twma7["HALL_NO_TO"] = " ";
				}

				if (bcls_rec->Tables["WM00_CMDUPDATE"].Columns.Contains("STOCK_NO_FIN"))
				{
					twma7["STOCK_NO_FIN"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["STOCK_NO_FIN"].ToString();
				}
				else
				{
					twma7["STOCK_NO_FIN"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["STOCK_NO_TO"];
				}

				if (bcls_rec->Tables["WM00_CMDUPDATE"].Columns.Contains("HALL_NO_FIN"))
				{
					twma7["HALL_NO_FIN"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["HALL_NO_FIN"].ToString();
				}
				else
				{
					twma7["HALL_NO_FIN"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["STOCK_NO_TO"];
				}
				update = "REC_REVISOR,REC_ERASE_TIME,STOCK_NO,HALL_NO_FR,STOCK_NO_FROM,STOCK_PLACE_NO_FROM,YARD_LAYER_FROM,STOCK_NO_TO,STOCK_PLACE_NO_TO,STOCK_PLACE_NO_FIN,STOCK_OPER_ORDER,STOCK_OPER_ORDER_FIN,MOVE_TYPE,UNIT_CODE,VEHICLE_NO,YARD_LAYER_TO,CRANE_CMDGRPNO,HALL_NO_TO,STOCK_NO_FIN,HALL_NO_FIN";
				twma7.Update(update, "MAT_NO");
			}
			else if (stock_oper_order_new == "32")
			{
				Log::Trace("", __FUNCTION__, "材料{0}替换为guokua命令", bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["MAT_NO"].ToString());
				twma2["MAT_NO"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["MAT_NO"].ToString();
				twma2.Query("MAT_NO");

				twma0["MAT_NO"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["MAT_NO"].ToString();

				twma7.Reset();
				twma7["REC_REVISOR"] = s.userid;
				twma7["REC_ERASE_TIME"] = dateTime;
				twma7["MAT_NO"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["MAT_NO"].ToString();
				twma7["STOCK_NO"] = twma2["STOCK_NO"].ToString();
				twma7["HALL_NO_FR"] = twma2["HALL_NO"].ToString();
				twma7["STOCK_NO_FROM"] = twma2["STOCK_NO"].ToString();
				twma7["STOCK_PLACE_NO_FROM"] = twma2["STOCK_PLACE_NO"].ToString();
				twma7["YARD_LAYER_FROM"] = twma2["LAYERNO"].ToDecimal();

				twma7["STOCK_NO_TO"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["STOCK_NO_TO"];
				twma7["STOCK_PLACE_NO_TO"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["STOCK_PLACE_NO_TO"];
				twma7["STOCK_PLACE_NO_FIN"] = " ";
				twma7["STOCK_OPER_ORDER"] = stock_oper_order_new;
				if (bcls_rec->Tables["WM00_CMDUPDATE"].Columns.Contains("STOCK_OPER_ORDER_FIN") && bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["STOCK_OPER_ORDER_FIN"].ToString().Trim()!="")
				{
					twma7["STOCK_OPER_ORDER_FIN"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["STOCK_OPER_ORDER_FIN"];
				}
				else
				{
					twma7["STOCK_OPER_ORDER_FIN"] = stock_oper_order_new;
				}
				
				twma7["MOVE_TYPE"] = stock_oper_order_new;
				twma7["UNIT_CODE"] = " ";
				if (twma0.Query("MAT_NO"))
				{
					twma7["VEHICLE_NO"] = twma0["VEHICLE_NO"].ToString();
				}
				else
				{
					twma7["VEHICLE_NO"] = " ";
				}

				if (bcls_rec->Tables["WM00_CMDUPDATE"].Columns.Contains("YARD_LAYER_TO"))
				{
					twma7["YARD_LAYER_TO"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["YARD_LAYER_TO"].ToDecimal();
				}
				else
				{
					twma7["YARD_LAYER_TO"] = 0;
				}

				if (bcls_rec->Tables["WM00_CMDUPDATE"].Columns.Contains("CRANE_CMDGRPNO"))
				{
					twma7["CRANE_CMDGRPNO"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["CRANE_CMDGRPNO"].ToDecimal();
				}
				else
				{
					twma7["CRANE_CMDGRPNO"] = 0;
				}

				if (bcls_rec->Tables["WM00_CMDUPDATE"].Columns.Contains("HALL_NO_TO"))
				{
					twma7["HALL_NO_TO"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["HALL_NO_TO"].ToString();
				}
				else
				{
					twma7["HALL_NO_TO"] = " ";
				}

				if (bcls_rec->Tables["WM00_CMDUPDATE"].Columns.Contains("STOCK_NO_FIN"))
				{
					twma7["STOCK_NO_FIN"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["STOCK_NO_FIN"].ToString();
				}
				else
				{
					twma7["STOCK_NO_FIN"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["STOCK_NO_TO"];
				}

				if (bcls_rec->Tables["WM00_CMDUPDATE"].Columns.Contains("HALL_NO_FIN"))
				{
					twma7["HALL_NO_FIN"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["HALL_NO_FIN"].ToString();
				}
				else
				{
					twma7["HALL_NO_FIN"] = bcls_rec->Tables["WM00_CMDUPDATE"].Rows[i]["STOCK_NO_TO"];
				}
				update = "REC_REVISOR,REC_ERASE_TIME,STOCK_NO,HALL_NO_FR,STOCK_NO_FROM,STOCK_PLACE_NO_FROM,YARD_LAYER_FROM,STOCK_NO_TO,STOCK_PLACE_NO_TO,STOCK_PLACE_NO_FIN,STOCK_OPER_ORDER,STOCK_OPER_ORDER_FIN,MOVE_TYPE,UNIT_CODE,VEHICLE_NO,YARD_LAYER_TO,CRANE_CMDGRPNO,HALL_NO_TO,STOCK_NO_FIN,HALL_NO_FIN";
				twma7.Update(update, "MAT_NO");
			}


			doFlag = f_wm00_pileinfocal(stock_no_old, stock_place_no_old, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (twma7["STOCK_PLACE_NO_TO"].ToString().Trim() != "")
			{
				doFlag = f_wm00_pileinfocal(twma7["STOCK_NO"].ToString(), twma7["STOCK_PLACE_NO_TO"].ToString(), bcls_ret, conn);
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
