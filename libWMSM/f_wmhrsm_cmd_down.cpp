/* **************************************************************************
*	Copyright (c) Baosight Corporation 2008 . All Rights Reserved.
*  	BM2PES 宝信生产执行系统
*****************************************************************************
*  程序名称			: f_wm_cmd_down
*  程序描述			: 指令卸下函数
*  备注说明			:
*  修改历史			:
*  		2022-09-11 仓库产品化			(ADD)程序建立
*			... ...
* **************************************************************************** */
/* ***************************传入参数********************************
传入块名：WM_CMD
MAT_NO                     材料号                  非空
* **************************************************************************** */
/* C/C++ 的标准头文件部分 */
#include "stdafx.h"		// 框架头，不可删除 

BM2_FUNCTION_EXPORT
int f_wmsmsm_stock_in(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wmsmsm_stock_move(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wmsmsm_stock_out(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wmsmsm_cmd_follow(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);
int f_wmsmsm_cmd_down(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	/* ***** 程序变量 ***** */
	int doFlag = 0;
	CString v_datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString v_mat_no = "";
	CString stock_oper_order = "";
	CString stock_place_no = "";
	CString stock_no = "";
	CString to_stock_no = "";
	CString cmd_method = "";
	CString location_from = "";
	CString location_to = "";
	/* ***** 数据库SQL操作字符串 ***** */
	CString	sqlstr = "";

	CModel twma7 = CModel("TWMA7");
	CModel twma7_old = CModel("TWMA7");
	CModel hwm00a7 = CModel("HWM00A7");
	CModel twma2 = CModel("TWMA2");
	CModel twm04_from = CModel("TWM04");
	CModel twm04_to = CModel("TWM04");
	CModel tpshra4 = CModel("TPSHRA4");
	/* ***** 应用程序开始处理 ***** */
	//调用仓库主函数
	EIClass bcls_stock;
	bcls_stock.Tables[0].set_TableName("WM_STOCK");
	bcls_stock.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
	bcls_stock.Tables[0].Columns.Add(DT_STRING, "STOCK_NO");
	bcls_stock.Tables[0].Columns.Add(DT_STRING, "TO_STOCK_NO");
	bcls_stock.Tables[0].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");
	bcls_stock.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO");

	try
	{
		if (!bcls_rec->Tables.Contains("WM_CMD")){
			sprintf(s.msg, "函数f_wm_cmd_cranedown中找不到接收块名[WM_CMD]");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (bcls_rec->Tables["WM_CMD"].Rows.get_Count() == 0){
			Log::Trace("", __FUNCTION__, "函数f_wm_cmd_cranedown中传入数据为空");
			return doFlag;
		}
		for (int i = 0; i < bcls_rec->Tables["WM_CMD"].Rows.get_Count(); i++)//一般不会有循环
		{
			v_mat_no = bcls_rec->Tables["WM_CMD"].Rows[i]["MAT_NO"].ToString();
			stock_oper_order = bcls_rec->Tables["WM_CMD"].Rows[i]["STOCK_OPER_ORDER"].ToString();
			stock_place_no = bcls_rec->Tables["WM_CMD"].Rows[i]["STOCK_PLACE_NO"].ToString();
			stock_no = bcls_rec->Tables["WM_CMD"].Rows[i]["STOCK_NO"].ToString();
			//to_stock_no = bcls_rec->Tables["WM_CMD"].Rows[i]["TO_STOCK_NO"].ToString();
			Log::Trace("", __FUNCTION__, "v_mat_no = [{0}]", v_mat_no);
			Log::Trace("", __FUNCTION__, "stock_no = [{0}]", stock_no);
			Log::Trace("", __FUNCTION__, "stock_oper_order = [{0}]", stock_oper_order);
			Log::Trace("", __FUNCTION__, "stock_place_no = [{0}]", stock_place_no);
			Log::Trace("", __FUNCTION__, "to_stock_no = [{0}]", to_stock_no);
			//一般来说，行车卸下不自动出库，转为倒垛到约定位置
			if (stock_oper_order[0] == '2')
			{
				stock_place_no = stock_place_no.SubstringNE(0, 3) + "XN01011";
				stock_oper_order = "30";
			}
			if (i > 0&& stock_oper_order!= bcls_stock.Tables["WM_STOCK"].Rows[i-1]["STOCK_OPER_ORDER"].ToString().Trim())
			{
				sprintf(s.msg, "函数f_wm_cmd_cranedown中,不同类型的命令不可一起卸下");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			bcls_stock.Tables["WM_STOCK"].Rows.Add();
			bcls_stock.Tables["WM_STOCK"].Rows[i]["MAT_NO"] = v_mat_no;
			bcls_stock.Tables["WM_STOCK"].Rows[i]["STOCK_OPER_ORDER"] = stock_oper_order;
			bcls_stock.Tables["WM_STOCK"].Rows[i]["STOCK_PLACE_NO"] = stock_place_no;
			bcls_stock.Tables["WM_STOCK"].Rows[i]["STOCK_NO"] = stock_no;
			bcls_stock.Tables["WM_STOCK"].Rows[i]["TO_STOCK_NO"] = to_stock_no;
			twma7_old.Reset();
			twma7_old["MAT_NO"] = v_mat_no;
			twma7_old.Query("MAT_NO");
			twm04_to.Reset();
			twm04_to["STOCK_PLACE_NO"] = stock_place_no;
			twm04_to.Query("STOCK_PLACE_NO");	
			doFlag = f_wmsmsm_cmd_follow(bcls_rec, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		if (stock_oper_order.SubstringNE(0, 1) == "1"&& bcls_stock.Tables[0].Rows.get_Count())//入库
		{
			doFlag = f_wmsmsm_stock_in(&bcls_stock, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		if (stock_oper_order.SubstringNE(0, 1) == "2" && bcls_stock.Tables[0].Rows.get_Count())//出库,应该进不去
		{
			doFlag = f_wmsmsm_stock_out(&bcls_stock, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		if (stock_oper_order.SubstringNE(0, 1) == "3" && bcls_stock.Tables[0].Rows.get_Count())//倒垛
		{
			doFlag = f_wmsmsm_stock_move(&bcls_stock, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CString str = ex.GetMsg() + "\r\n" + sqlstr;
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
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

