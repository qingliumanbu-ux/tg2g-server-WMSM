/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         QL
Version:		1.0
Date:			2016-08-05
Description:	吊车命令信息删除
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件

//函数申明

/*<remark>=========================================================
///<summary>
///吊车命令信息删除
///<para>
///2.排序方式：MAT_NO
///</para>
///<para>数据库表：TWMA7 行车命令表；
///<returns>删除吊车命令信息</returns>
===========================================================</remark>*/


//调用外部函数
BM2_FUNCTION_IMPORT

//int f_wm_jolbdy_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);


BM2F_ENTERACE(wmsmsma7_send);

int f_wmsmsma7_send(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDecimal Count = 0;

	/* 实体类定义 */
	//CTWMA7 twma7(conn);
	//CTWMA2 twma2(conn);
	//CTWM04 twm04(conn);

	CModel twma7 = CModel("TWMA7");
	CModel hwma7 = CModel("HWMA7");
	CModel twma1 = CModel("TMMSM01");
	CModel twm04 = CModel("TWM04");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";
	CString dateTime = " ";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	//取系统时间
	dateTime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	//吊车命令传入块


	EIClass bcls_rec_hc;
	bcls_rec_hc.Tables[0].set_TableName("HC");
	bcls_rec_hc.Tables[0].Columns.Add(DT_DECIMAL, "COUNT");
	bcls_rec_hc.Tables[0].Columns.Add(DT_STRING, "CRANE_NO");
	bcls_rec_hc.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
	bcls_rec_hc.Tables[0].Columns.Add(DT_STRING, "HALL_NO");
	bcls_rec_hc.Tables[0].Columns.Add(DT_STRING, "ORDER_TYPE");
	bcls_rec_hc.Tables[0].Columns.Add(DT_DECIMAL, "ORDER_NO");
	bcls_rec_hc.Tables[0].Columns.Add(DT_STRING, "FROM_STOCK");
	bcls_rec_hc.Tables[0].Columns.Add(DT_STRING, "FROM_STOCK_LAYER");
	bcls_rec_hc.Tables[0].Columns.Add(DT_STRING, "TO_STOCK_NO");
	bcls_rec_hc.Tables[0].Columns.Add(DT_STRING, "TO_STOCK_LAYER");
	bcls_rec_hc.Tables[0].Columns.Add(DT_DECIMAL, "MAT_ACT_LEN");
	bcls_rec_hc.Tables[0].Columns.Add(DT_DECIMAL, "MAT_ACT_WIDTH");
	bcls_rec_hc.Tables[0].Columns.Add(DT_DECIMAL, "MAT_ACT_THICK");
	bcls_rec_hc.Tables[0].Columns.Add(DT_DECIMAL, "MAT_ACT_WT");
	bcls_rec_hc.Tables[0].Columns.Add(DT_STRING, "OP_FLAG");
	bcls_rec_hc.Tables[0].Columns.Add(DT_STRING, "ORDER_STATUS");
	bcls_rec_hc.Tables[0].Columns.Add(DT_STRING, "DAO_FLAG");
	bcls_rec_hc.Tables[0].Columns.Add(DT_STRING, "GROUP_NO");
	bcls_rec_hc.Tables[0].Columns.Add(DT_STRING, "CREATE_TIME");
	bcls_rec_hc.Tables[0].Rows.Add();

	try
	{
		Log::Trace("", __FUNCTION__, "传入记录数 \t[{0}]", bcls_rec->Tables[0].Rows.get_Count());
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{

			twma7.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			twma1.Reset();

			twma7.Query();

			twma7.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			twma1["MAT_NO"] = twma7["MAT_NO"];
			Log::Trace("", __FUNCTION__, "MAT_NO={0}", twma7["MAT_NO"].ToString());

			if (!twma1.Query("MAT_NO"))
			{
				Log::Trace("", __FUNCTION__, "主档表无此材料号{0}", twma7["MAT_NO"].ToString());
				continue;
			}

			if (twma7["MAT_NO"].ToString().Trim() == "")
			{
				sprintf(s.msg, "Mat No. cannot be empty.");
				throw CApplicationException(-1, s.msg, log.Location);
			}
		

			Log::Trace("", __FUNCTION__, "twma7.MAIN_MAT_NO[{0}]", twma7["MAIN_MAT_NO"].ToString());

			bcls_rec_hc.Tables[0].Rows.Clear();
			bcls_rec_hc.Tables[0].Rows.Add();
			bcls_rec_hc.Tables[0].Rows[0]["COUNT"] = 1;
			bcls_rec_hc.Tables[0].Rows[0]["MAT_NO"] = twma7["MAT_NO"];
			bcls_rec_hc.Tables[0].Rows[0]["HALL_NO"] = twma7["HALL_NO_FR"];

			bcls_rec_hc.Tables[0].Rows[0]["ORDER_TYPE"] = twma7["STOCK_OPER_ORDER"].ToString().SubstringNE(0,1);
			Log::Trace("", __FUNCTION__, "AAA ");
			bcls_rec_hc.Tables[0].Rows[0]["ORDER_NO"] = twma7["CMD_SEQ"].ToString();
			Log::Trace("", __FUNCTION__, "BBB ");
			bcls_rec_hc.Tables[0].Rows[0]["FROM_STOCK"] = twma7["STOCK_NO_FROM"];
			bcls_rec_hc.Tables[0].Rows[0]["FROM_STOCK_LAYER"] = "";
			
			bcls_rec_hc.Tables[0].Rows[0]["TO_STOCK_NO"] = twma7["STOCK_NO_FIN"];
			bcls_rec_hc.Tables[0].Rows[0]["TO_STOCK_LAYER"] = "";
			bcls_rec_hc.Tables[0].Rows[0]["MAT_ACT_LEN"] = twma1["MAT_ACT_LEN"];
			bcls_rec_hc.Tables[0].Rows[0]["MAT_ACT_WIDTH"] = twma1["MAT_ACT_WIDTH"];
			bcls_rec_hc.Tables[0].Rows[0]["MAT_ACT_THICK"] = twma1["MAT_ACT_THICK"];
			bcls_rec_hc.Tables[0].Rows[0]["MAT_ACT_WT"] = twma1["MAT_ACT_WT"];
			bcls_rec_hc.Tables[0].Rows[0]["OP_FLAG"] = "1";
			Log::Trace("", __FUNCTION__, "CCC ");
			bcls_rec_hc.Tables[0].Rows[0]["ORDER_STATUS"] = "0";
			bcls_rec_hc.Tables[0].Rows[0]["DAO_FLAG"] = "1";
			bcls_rec_hc.Tables[0].Rows[0]["GROUP_NO"] = "000";
			bcls_rec_hc.Tables[0].Rows[0]["CREATE_TIME"] = dateTime;

			//doFlag = f_wm_jolbdy_snd(&bcls_rec_hc, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
			
		}


	
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "Database processing error，sqlcode = [{0}]." /* _RES("GCRSS0000006")*//*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
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