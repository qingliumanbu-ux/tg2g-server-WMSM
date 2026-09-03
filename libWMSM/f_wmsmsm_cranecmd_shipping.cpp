/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:      JQ
Version:     1.1.1
Date:         2017-3-16
Description: 发货
**************************************************/

/* C/C++ 的标准头文件部分 */
#include "stdafx.h"		// 框架头，不可删除 

int f_wmsmsm_cranecmd_make(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

BM2_FUNCTION_EXPORT
int f_wmsmsm_cranecmd_shipping(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{

	//程序内部变量
	int doFlag = 0;
	int seq_no = 0;
	CString sqlstr = " ";

	//业务变量
	CString mat_no = "";                         //材料
	CString stock_place_no_to = "";              //目标垛位
	CString vehicle_no = "";                     //卡车号
	CString stock_oper_order = "";               //发货类型

	//数据块
	CDataTable plan_mat;                         //存放计划材料

	EIClass bcls_rec_make;
	bcls_rec_make.Tables[0].set_TableName("CMD_MAKE");
	bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
	bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");
	bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO_TO");
	bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "STOCK_OPER_ORDER_FIN");
	bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO_FIN");
	bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "VEHICLE_NO");
	bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "HALL_NO_TO");
	bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "STOCK_NO_TO");

	EIClass bcls_ret_tr;

	//定义表实体对象
	CModel twma2 = CModel("TWMA2");
	CModel twm04 = CModel("TWM04");
	try
	{
		//项目自定义日志
		CTracer log(__FUNCTION__);     

		//检验传入数据块
		if (!bcls_rec->Tables.Contains("CMD_SHIPPING"))
		{
			sprintf(s.msg, "函数f_wmsmsm_cranecmd_shipping中找不到接收块名[CMD_SHIPPING]");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (!bcls_rec->Tables.Contains("CMD_SHIP_MAT"))
		{
			sprintf(s.msg, "函数f_wmsmsm_cranecmd_shipping中找不到接收块名[CMD_SHIP_MAT]");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//获取发货位置
		stock_place_no_to = bcls_rec->Tables["CMD_SHIPPING"].Rows[0]["STOCK_PLACE_NO_TO"].ToString();
		vehicle_no = bcls_rec->Tables["CMD_SHIPPING"].Rows[0]["VEHICLE_NO"].ToString();
		stock_oper_order = bcls_rec->Tables["CMD_SHIPPING"].Rows[0]["STOCK_OPER_ORDER"].ToString();
		twm04["STOCK_PLACE_NO"] = stock_place_no_to;
		twm04.Query("STOCK_PLACE_NO");

		if (stock_place_no_to.Trim()=="")
		{
			sprintf(s.msg, "传入STOCK_PLACE_NO_TO为空");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (vehicle_no.Trim() == "")
		{
			sprintf(s.msg, "传入VEHICLE_NO为空");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (stock_oper_order.Trim() == "")
		{
			sprintf(s.msg, "传入STOCK_OPER_ORDER为空");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		Log::Trace("", __FUNCTION__, "stock_place_no_to={0}", stock_place_no_to);
		Log::Trace("", __FUNCTION__, "vehicle_no={0}", vehicle_no);
		Log::Trace("", __FUNCTION__, "stock_oper_order={0}", stock_oper_order);


		for (int i = 0; i < bcls_rec->Tables["CMD_SHIP_MAT"].Rows.get_Count(); i++)
		{
			//获取前台参数
			mat_no = bcls_rec->Tables["CMD_SHIP_MAT"].Rows[i]["MAT_NO"].ToString();
			Log::Trace("", __FUNCTION__, "mat_no={0}", mat_no);
			twma2.Reset();
			twma2["MAT_NO"] = mat_no;
			if (!twma2.Query("MAT_NO"))
			{
				sprintf(s.msg, "材料不在库内");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (twma2["HALL_NO"].ToString() == twm04["HALL_NO"].ToString())
			{
				//在目标跨
				bcls_rec_make.Tables[0].Rows.Add();
				bcls_rec_make.Tables[0].Rows[seq_no]["MAT_NO"] = mat_no;
				bcls_rec_make.Tables[0].Rows[seq_no]["STOCK_OPER_ORDER"] = stock_oper_order;
				bcls_rec_make.Tables[0].Rows[seq_no]["STOCK_PLACE_NO_TO"] = stock_place_no_to;
				bcls_rec_make.Tables[0].Rows[seq_no]["VEHICLE_NO"] = vehicle_no;
				bcls_rec_make.Tables[0].Rows[seq_no]["HALL_NO_TO"] = twma2["HALL_NO"].ToString();
				bcls_rec_make.Tables[0].Rows[seq_no]["STOCK_NO_TO"] = twma2["STOCK_NO"].ToString();
				seq_no++;


			}
			else
			{
				//不在目标跨
				bcls_rec_make.Tables[0].Rows.Add();
				bcls_rec_make.Tables[0].Rows[seq_no]["MAT_NO"] = mat_no;
				bcls_rec_make.Tables[0].Rows[seq_no]["STOCK_OPER_ORDER"] = "32";
				bcls_rec_make.Tables[0].Rows[seq_no]["STOCK_PLACE_NO_TO"] = " ";
				bcls_rec_make.Tables[0].Rows[seq_no]["VEHICLE_NO"] = vehicle_no;
				bcls_rec_make.Tables[0].Rows[seq_no]["STOCK_OPER_ORDER_FIN"] = stock_oper_order;
				bcls_rec_make.Tables[0].Rows[seq_no]["STOCK_PLACE_NO_FIN"] = stock_place_no_to;
				bcls_rec_make.Tables[0].Rows[seq_no]["HALL_NO_TO"] = twm04["HALL_NO"].ToString();
				bcls_rec_make.Tables[0].Rows[seq_no]["STOCK_NO_TO"] = twm04["STOCK_NO"].ToString();
				seq_no++;
			}
		}

		if (bcls_rec_make.Tables[0].Rows.get_Count() > 0)
		{
			doFlag = f_wmsmsm_cranecmd_make(&bcls_rec_make, bcls_ret, conn);
			if (doFlag != 0)
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