/*************************************************************************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:
Version:
Date:
Description:
**************************************************************************************************************/
//框架公用头文件，勿删
#include "stdafx.h"
//#include "smhs.h"
//程序用头文件
//#include "twma0.h"
//#include "twma1.h"
//#include "twma2.h"
//#include "twm01.h"
//#include "twm04.h"

/*<remark>=========================================================
/// <summary>
/// 1.
/// 2.
/// <para>
/// </para>
/// <para>数据库表：TMMHP01(厚板物料主表)          </para>
/// <para>主调用函数： 前台YMHP012画面F5 倒垛      </para>
/// </summary>
/// <param name="param1">参数1  </param>
/// <param name="param2">参数2  </param>
/// <returns>返回参数：0（成功）；-1（失败）  </returns>
===========================================================</remark>*/

//调用外部函数
BM2_FUNCTION_IMPORT
int f_wmsmsm_stock_move(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);

/*
BM2_FUNCTION_IMPORT
int f_wm00_b12106_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
BM2_FUNCTION_IMPORT
int f_wm00_b22105_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
*/

// service入口
BM2F_ENTERACE(wmsmsm17_mov1)
//-EP_SYSTEM_HEAD_END
int f_wmsmsm17_mov1(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);  // 系统日志

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	int i, rows, j = 0;


	/* 业务变量 */
	CString stock_oper_order = "";
	CString stock_place_no_from = "";
	CString stock_place_no_to = "";
	CString yard_layer_to = "";
	CString stock_place_position = "";
	CString mat_no = "";
	CString mat_line_type = "";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString sqlstr;
	CString stock_no = "";
	CDecimal layerno = 0;
	CString mat_kind = "";


	/* 实体类定义 */
	//CTWMA0 twma0(conn);
	//CTWMA1 twma1(conn);
	//CTWMA2 twma2(conn);
	//CTWM01 twm01(conn);
	//CTWM01 twm01_to(conn);
	//CTWM04 twm04(conn);
	//CTWM04 twm04_to(conn);
	CModel twma0 = CModel("TWMA0");
	CModel twma1 = CModel("TMMSM01");
	CModel twma2 = CModel("TWMA2");
	CModel twm01 = CModel("TWM01");
	CModel twm01_to = CModel("TWM01");
	CModel twm04 = CModel("TWM04");
	CModel twm04_to = CModel("TWM04");


	/* 数据库操作类定义 */




	//调用仓库入库主函数
	EIClass bcls_stock_move;
	bcls_stock_move.Tables[0].set_TableName("WM_STOCK");
	bcls_stock_move.Tables[0].Columns.Add(twma0);
	bcls_stock_move.Tables[0].Columns.Add(twma2);
	bcls_stock_move.Tables[0].Rows.Clear();


	try
	{
		//获取前台传入参数
		rows = bcls_rec->Tables[0].Rows.get_Count();
		Log::Trace("", __FUNCTION__, "传入记录数 = [{0}]", rows);

		stock_oper_order = bcls_rec->Tables[0].Rows[0]["STOCK_OPER_ORDER"].ToString().Trim();
		stock_place_no_from = bcls_rec->Tables[0].Rows[0]["STOCK_PLACE_NO_FROM"].ToString().Trim();
		stock_place_no_to = bcls_rec->Tables[0].Rows[0]["STOCK_PLACE_NO_TO"].ToString().Trim();
		//yard_layer_to = bcls_rec->Tables[0].Rows[0]["YARD_LAYER_TO"].ToString().Trim();
		stock_place_position = bcls_rec->Tables[0].Rows[0]["STOCK_PLACE_POSITION"].ToString().Trim();
		mat_line_type = bcls_rec->Tables[0].Rows[0]["MAT_LINE_TYPE"].ToString().Trim();
		stock_no = bcls_rec->Tables[0].Rows[0]["STOCK_NO"].ToString().Trim();


		Log::Trace("", __FUNCTION__, "传入参数STOCK_OPER_ORDER		= [{0}]", stock_oper_order);
		Log::Trace("", __FUNCTION__, "传入参数STOCK_PLACE_NO_FROM	= [{0}]", stock_place_no_from);
		Log::Trace("", __FUNCTION__, "传入参数STOCK_PLACE_NO_TO		= [{0}]", stock_place_no_to);
		Log::Trace("", __FUNCTION__, "传入参数YARD_LAYER_TO			= [{0}]", yard_layer_to);
		Log::Trace("", __FUNCTION__, "传入参数STOCK_PLACE_POSITION	= [{0}]", stock_place_position);
		Log::Trace("", __FUNCTION__, "传入参数MAT_LINE_TYPE			= [{0}]", mat_line_type);
		Log::Trace("", __FUNCTION__, "传入参数STOCK_NO				= [{0}]", stock_no);

		for (i = 0; i < rows; i++)
		{
			// 获取前台传入参数
			mat_no = bcls_rec->Tables[0].Rows[i]["MAT_NO"].ToString().Trim();
			Log::Trace("", __FUNCTION__, "传入参数MAT_NO = [{0}]", mat_no);

			twma1.Reset();
			twma1["MAT_NO"] = mat_no;
			Log::Trace("", __FUNCTION__, "twma1.MAT_NO = [{0}]", twma1["MAT_NO"].ToString());
			if (!twma1.Query("MAT_NO"))
			{
				sprintf(s.msg, "主档表表中无材料信息！！！");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			Log::Trace("", __FUNCTION__, "111wwww111");
			twma2.Reset();
			twma2["MAT_NO"] = mat_no;
			if (!twma2.Query("MAT_NO"))
			{
				sprintf(s.msg, "材料没在垛位上，不允许进行倒垛！！！！");
				throw CApplicationException(-1, s.msg, log.Location);
			}


			if (twma2["STOCK_PLACE_NO"].ToString() == stock_place_no_to)
			{
				strcpy(s.msg, "同垛位内的层号调整请用 '垛内调整'功能！ ");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			twm04["STOCK_PLACE_NO"] = twma2["STOCK_PLACE_NO"];
			twm04_to["STOCK_PLACE_NO"] = stock_place_no_to;

			twm04.Query("STOCK_PLACE_NO");
			twm04_to.Query("STOCK_PLACE_NO");


			twm01["STOCK_NO"] = twm04["STOCK_NO"];
			twm01_to["STOCK_NO"] = twm04_to["STOCK_NO"];

			twm01.Query("STOCK_NO");
			twm01_to.Query("STOCK_NO");


			if (twm01["FACTORY_DIV"].ToString().Trim() != twm01_to["FACTORY_DIV"].ToString().Trim())
			{
				strcpy(s.msg, "不同厂别之间不能倒垛！ ");
				throw CApplicationException(-1, s.msg, log.Location);
			}





			//调用仓库倒垛主函数
			bcls_stock_move.Tables["WM_STOCK"].Rows.Add();
			bcls_stock_move.Tables["WM_STOCK"].Rows[i]["MAT_NO"] = mat_no;
			bcls_stock_move.Tables["WM_STOCK"].Rows[i]["STOCK_OPER_ORDER"] = stock_oper_order;
			bcls_stock_move.Tables["WM_STOCK"].Rows[i]["STOCK_NO"] = stock_no;
			bcls_stock_move.Tables["WM_STOCK"].Rows[i]["STOCK_PLACE_NO"] = stock_place_no_to;
			bcls_stock_move.Tables["WM_STOCK"].Rows[i]["ROWNO"] = " ";
			bcls_stock_move.Tables["WM_STOCK"].Rows[i]["COLUMN_NO"] = " ";
			bcls_stock_move.Tables["WM_STOCK"].Rows[i]["LAYERNO"] = 0;
			bcls_stock_move.Tables["WM_STOCK"].Rows[i]["STOCK_PLACE_POSITION"] = stock_place_position;

		}


		doFlag = f_wmsmsm_stock_move(&bcls_stock_move, bcls_ret, conn);
		if (doFlag != 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。" /* _RES("GCRSS0000006")*//*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
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

	return(doFlag);
}



