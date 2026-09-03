/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:
Version:		1.0
Date:			2016-03-05
Description:	来料拒收
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件
//#include "twm01.h"
//#include "twma0.h"
//#include "twma1.h"
//#include "twma2.h"



BM2_FUNCTION_IMPORT
int f_wmsmsm_mm0099(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn); 



BM2_FUNCTION_IMPORT
int f_wmsm_21a007_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wmsmsm_allot_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);



/*<remark>=========================================================
///<summary>
///板坯入库功能
///<para>
///2.排序方式：
///</para>
///<para>数据库表：TWMA0 倒躲队列；TWMA1 物料主档表
///<returns>执行预材料预入库功能</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wmsmsm11_in_reject);

int f_wmsmsm11_in_reject(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CDecimal rowCount = 0;

	/*程序用变量*/
	CString stock_no = "";
	CString stock_place_no = "";
	CString vehicle_no = "";
	CString stock_oper_order_div = "";
	CString layerno = "";
	CString come_reject_cause = " ";


	/* 实体类定义 */
	//CTWM01 twm01(conn);
	//CTWMA0 twma0(conn);
	//CTWMA1 twma1(conn);
	//CTWMA2 twma2(conn);
	CModel twm01 = CModel("TWM01");
	CModel twma0 = CModel("TWMA0");
	CModel twma1 = CModel("TMMSM01");
	CModel twma2 = CModel("TWMA2");
	CModel twmsm62 = CModel("TWMSM62");
	CModel twm41dj("TWM41DJ");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);



	


	//发送电文
	EIClass bcls_js;
	bcls_js.Tables[0].set_TableName("21A007");
	bcls_js.Tables["21A007"].Columns.Add(twmsm62);
	bcls_js.Tables["21A007"].Rows.Clear();
	//发送电文
	EIClass bcls_allot;
	//bcls_load.Tables[0].set_TableName("21A009");
	bcls_allot.Tables[0].Columns.Add(twm41dj);
	bcls_allot.Tables[0].Rows.Clear();

	


	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			twma0.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			
			//come_reject_cause = bcls_rec->Tables[1].Rows[0]["COME_REJECT_CAUSE"].ToString();

			
			Log::Trace("", __FUNCTION__, "line[{0}]", __LINE__);


			if (twma0["STOCK_OPER_ORDER"].ToString().Trim() != "1Q" &&
				twma0["STOCK_OPER_ORDER"].ToString().Trim() != "1A")
			{
				sprintf(s.msg, "只有上产线来料入库、下产线回退入库可以拒收。");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			Log::Trace("", __FUNCTION__, "line[{0}]", __LINE__);

			

			twma1["MAT_NO"] = twma0["MAT_NO"];
			twma1.Query("MAT_NO");
			Log::Trace("", __FUNCTION__, "line[{0}]", __LINE__);
			


			Log::Trace("", __FUNCTION__, "line[{0}]", __LINE__);

			//删除队列
			twma0.Query("MAT_NO");

			twma0.Delete("MAT_NO");
			Log::Trace("", __FUNCTION__, "line[{0}]", __LINE__);
			Log::Trace("", __FUNCTION__, "来源库区 twma0.FROM_STOCK_NO[{0}]", twma0["FROM_STOCK_NO"].ToString());
			if (bcls_rec->Tables[0].Rows[i]["IF_LOGI"].ToString()=="1")
			{
				twmsm62.CopyFrom(twma1);
				twmsm62["PRACTICE_NO"] = bcls_rec->Tables[0][i]["PRACTICE_NO"].ToString();
				twmsm62.Query("PRACTICE_NO,MAT_NO");
				twmsm62.MergeTo(bcls_js.Tables["21A007"], false);

				twmsm62["UNLOAD_FLAG"] = "2";//卸车标记
				twmsm62["AFFIRM_FLAG"] = "9";//确认标记
				twmsm62.Update("UNLOAD_FLAG,AFFIRM_FLAG", "PRACTICE_NO,MAT_NO");

				sqlstr = " select * from TWM41DJ where C_BATCHUNIT='" + twma0["MAT_NO"].ToString() + "' and C_STATESIGN='1' ";
				Log::Trace("", __FUNCTION__, "查询到有未显示的调拨单号[{0}]", sqlstr);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					twm41dj.Reset();
					cmd_inq.Fetch(twm41dj);
					twm41dj["C_STATESIGN"] = "3";
					twm41dj.Update("C_STATESIGN", "C_DELIVERYID");
					twm41dj.MergeTo(bcls_allot.Tables[0], false);
				}
				cmd_inq.Close();
			}
			twm41dj.Reset();
			if (bcls_rec->Tables[0].Rows[i]["C_DELIVERYID"].ToString().Trim() != "")
			{
				twm41dj["C_DELIVERYID"] = bcls_rec->Tables[0].Rows[i]["C_DELIVERYID"].ToString();
				twm41dj.Query("C_DELIVERYID");
				twm41dj["C_STATESIGN"] = "3";
				twm41dj.Update("C_STATESIGN", "C_DELIVERYID");
				twm41dj.MergeTo(bcls_allot.Tables[0], false);
			}
			Log::Trace("", __FUNCTION__, "line[{0}]", __LINE__);
			
		}


		
		if (bcls_js.Tables[0].Rows.get_Count() > 0)
		{
			//发送电文
			doFlag = f_wmsm_21a007_snd(&bcls_js, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		
		if (bcls_allot.Tables[0].Rows.get_Count() > 0) {
			doFlag = f_wmsmsm_allot_snd(&bcls_allot, bcls_ret, conn);
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
