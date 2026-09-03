/*************************************************twma0
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:
Version:		1.0
Date:			2016-03-05
Description:	板坯入库功能
**************************************************/

//框架头文件
#include "stdafx.h"

//程序用头文件





//函数申明 
BM2_FUNCTION_IMPORT
int f_wmsmsm_stock_back(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);


/*<remark>=========================================================
业务类型：1A-来料，1Q-退料
删除入库队列

根据业务类型，返回电文
///<returns>执行预材料预入库功能</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wmsmsm11_instock);
int f_wmsmsm11_instock(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	int ii = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CDecimal rowCount = 0;

	/*程序用变量*/
	CString stock_no = "";
	CString stock_place_no = "";
	CString vehicle_no = "";
	CString stock_oper_order_div = "";//业务类型内区分
	CString stock_place_position = "";//位置信息
	CString store_keeper = " ";//现场管料
	CString crane_resp = " ";//行车工
	CString crane_no = " ";//吊车号

	CString layerno = "";
	CDecimal layerno1 = 0;


	/* 实体类定义 */
	//CTWMA0 twma0(conn);
	//CTWMA1 twma1(conn);
	//CTWMA2 twma2(conn);
	Log::Trace("", __FUNCTION__, "11111111111111");
	CModel twma0 = CModel("TWMA0");
	Log::Trace("", __FUNCTION__, "222222222222");
	CModel twma1 = CModel("TMMSM01");
	CModel twm01 = CModel("TWM01");
	CModel hwma1 = CModel("HMMSM01");
	CModel twmsm62 = CModel("TWMSM62");
	CModel twm41dj = CModel("TWM41DJ");



	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";
	CString sqlStr = "";

	CString MEASURE_WT_FLAG = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);


	//调用仓库入库主函数
	EIClass bcls_stock_in;
	bcls_stock_in.Tables[0].set_TableName("WM_STOCK");
	bcls_stock_in.Tables[0].Columns.Add(twma0);
	bcls_stock_in.Tables[0].Columns.Add(twmsm62);
	bcls_stock_in.Tables[0].Columns.Add(twm41dj);
	bcls_stock_in.Tables[0].Rows.Clear();
	try
	{

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			twma0.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			

			

			if (bcls_rec->Tables[1].Columns.Contains("STOCK_NO"))
			{
				stock_no = bcls_rec->Tables[1].Rows[0]["STOCK_NO"].ToString();
				stock_place_no = bcls_rec->Tables[1].Rows[0]["STOCK_PLACE_NO"].ToString();
			}
			else
			{
				stock_no = "SYA";
				stock_place_no = "SYA";
			}



		



			sqlstr =
				" SELECT COUNT(1) FROM TWMA0"
				" WHERE MAT_NO = @mat_no"
				" AND STOCK_OPER_ORDER = @stock_oper_order";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("mat_no", twma0["MAT_NO"].ToString());
			cmd_inq.Parameters.Set("stock_oper_order", twma0["STOCK_OPER_ORDER"].ToString());
			rowCount = cmd_inq.ExecuteScalar();

			if (rowCount != 1)
			{
				sprintf(s.msg, "队列已变化，请刷新画面。");
				throw CApplicationException(-1, s.msg, log.Location);
			}


			if (i != 0)
			{
				if (bcls_rec->Tables[0].Rows[i]["C_SENDDEPT"].ToString() != bcls_rec->Tables[0].Rows[i - 1]["C_SENDDEPT"].ToString())
				{
					sprintf(s.msg, "不同去向的退料，不可以批量操作。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			
			



			//查询库区定义表
			twm01["STOCK_NO"] = stock_no;
			if (!twm01.Query("STOCK_NO"))
			{
				sprintf(s.msg, "库区号不存在");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			

			




			//更新队列表
			twma0["PROC_STATUS"] = "9";//倒垛处理状态 0-未倒垛 9-预倒垛
			twma0["STOCK_OPER_ORDER_DIV"] = stock_oper_order_div;
			twma0["STOCK_PLACE_POSITION"] = stock_place_position;
			twma0["STORE_KEEPER"] = store_keeper;
			twma0["CRANE_RESP"] = crane_resp;
			twma0["CRANE_NO"] = crane_no;
			twma0["REC_REVISOR"] = CString(s.userid);
			twma0["REC_REVISE_TIME"] = datetime;
			twma0.TrimOrBlank();
			twma0.Update(
				"REC_REVISOR,"
				"REC_REVISE_TIME,"
				"PROC_STATUS,"
				"STOCK_OPER_ORDER_DIV,"
				"STOCK_PLACE_POSITION,"
				"STORE_KEEPER,"
				"CRANE_RESP,"
				"CRANE_NO"
				,
				"MAT_NO,"
				"STOCK_OPER_ORDER");


			


			//调用仓库入库主函数
			bcls_stock_in.Tables["WM_STOCK"].Rows.Add();
			if (!bcls_stock_in.Tables["WM_STOCK"].Columns.Contains("IF_LOGI"))
				bcls_stock_in.Tables["WM_STOCK"].Columns.Add(DT_STRING, "IF_LOGI");
			bcls_stock_in.Tables["WM_STOCK"].Rows[i].Merge(bcls_rec->Tables[0].Rows[i]);
			bcls_stock_in.Tables["WM_STOCK"].Rows[i]["STOCK_NO"] = stock_no;
			bcls_stock_in.Tables["WM_STOCK"].Rows[i]["STOCK_PLACE_NO"] = stock_place_no;
		}

		doFlag = f_wmsmsm_stock_back(&bcls_stock_in, bcls_ret, conn);
		if (doFlag != 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
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

