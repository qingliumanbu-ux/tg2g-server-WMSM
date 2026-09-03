/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         JHZHAO
Version:		1.0
Date:			2016-02-01
Description:	直供出坯 材料临时下线功能
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件


//业务头文件


//函数申明
BM2_FUNCTION_IMPORT
int f_wmsmsm_stock_in(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
/*<remark>=========================================================
///<summary>
///直供出坯确认材料出库开始功能
///<para>
///</para>
///<para>数据库表：TMMSM01坯料主档表
///<returns>更新主档材料信息</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wmsmsm13_downline1);

int f_wmsmsm13_downline1(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	int count = 0;

	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString stock_place_no = "";


	/* 数据库SQL操作字符串 */
	CString sqlstr = "";

	/* 数据库操作类定义 */
	CModel twma0 = CModel("TWMA0");
	CModel twma1 = CModel("TMMSM01");
	CModel twma2 = CModel("TWMA2");
	CModel twm04 = CModel("TWM04");

	try
	{

		//调用仓库入库主函数
		EIClass bcls_stock_in;
		bcls_stock_in.Tables[0].set_TableName("WM_STOCK");
		bcls_stock_in.Tables[0].Columns.Add(twma0);
		bcls_stock_in.Tables[0].Columns.Add(twma2);
		bcls_stock_in.Tables[0].Rows.Clear();

		count = bcls_rec->Tables[0].Rows.get_Count();

		if (bcls_rec->Tables[0].Columns.Contains("STOCK_PLACE_NO") == true)
		{
			stock_place_no = bcls_rec->Tables[0].Rows[0]["STOCK_PLACE_NO"].ToString().Trim();
		}

		Log::Trace("", __FUNCTION__, "记录数 count：\t[{0}]", count);
		Log::Trace("", __FUNCTION__, "目标库位 stock_place_no：\t[{0}]", stock_place_no);

		if (stock_place_no == "")
		{
			strcpy(s.msg, "目标库位不能为空。");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		twm04["STOCK_PLACE_NO"] = stock_place_no;
		if (!twm04.Query("STOCK_PLACE_NO"))
		{
			strcpy(s.msg, "目标库位在twm04表没有维护。");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (twm04["STOCK_PLACE_TYPE"].ToString().Trim() != "A")
		{
			strcpy(s.msg, "临时下线只能下到临时堆垛区。");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		for (int i = 0; i < count; i++)
		{
			twma1.Reset();

			twma1["MAT_NO"] = bcls_rec->Tables[0].Rows[i]["MAT_NO"].ToString().Trim();

			if (!twma1.Query())
			{
				strcpy(s.msg, "查询材料" + twma1["MAT_NO"].ToString() + "信息出错。");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (twma1["IN_FLAG"].ToString().Trim() == "1")
			{
				strcpy(s.msg, "材料" + twma1["MAT_NO"].ToString() + "已入库，请刷新画面后再操作。");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (twma1["IN_FLAG"].ToString().Trim() == "2")
			{
				strcpy(s.msg, "材料" + twma1["MAT_NO"].ToString() + "已出库，请刷新画面后再操作。");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			
			//调用仓库入库主函数
			bcls_stock_in.Tables["WM_STOCK"].Rows.Clear();
			bcls_stock_in.Tables["WM_STOCK"].Rows.Add();
			bcls_stock_in.Tables["WM_STOCK"].Rows[0]["MAT_NO"] = twma1["MAT_NO"].ToString();
			bcls_stock_in.Tables["WM_STOCK"].Rows[0]["STOCK_OPER_ORDER"] = "1R";
			bcls_stock_in.Tables["WM_STOCK"].Rows[0]["STOCK_OPER_ORDER_DIV"] = "C";
			bcls_stock_in.Tables["WM_STOCK"].Rows[0]["STOCK_NO"] = twm04["STOCK_NO"].ToString();
			bcls_stock_in.Tables["WM_STOCK"].Rows[0]["STOCK_PLACE_NO"] = twm04["STOCK_PLACE_NO"].ToString();
			bcls_stock_in.Tables["WM_STOCK"].Rows[0]["ROWNO"] = " ";
			bcls_stock_in.Tables["WM_STOCK"].Rows[0]["COLUMN_NO"] = " ";
			bcls_stock_in.Tables["WM_STOCK"].Rows[0]["LAYERNO"] = 0;
			bcls_stock_in.Tables["WM_STOCK"].Rows[0]["STOCK_PLACE_POSITION"] = " ";

			doFlag = f_wmsmsm_stock_in(&bcls_stock_in, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}

		}


	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
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
	return doFlag;

}