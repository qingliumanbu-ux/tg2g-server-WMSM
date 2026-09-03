/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         JHZHAO
Version:		1.0
Date:			2016-03-05
Description:	板坯入库队列信息查询
**************************************************/

//框架头文件
#include "stdafx.h"
//#include "smhs.h"
//程序用头文件
//#include "twma0.h"
//#include "twma1.h"
//#include "twm00.h"


//函数申明
int f_epes_get_auth_other(const char *iuser, int irestype, EIClass *bcls_ret, CDbConnection * conn);

/*<remark>=========================================================
///<summary>
///板坯入库队列信息查询
///<para>
///2.排序方式：队列写入时间
///</para>
///<para>数据库表：TWMA0 倒躲队列；TWMA1 物料主档表
///<returns>返回符合查询条件的队列信息</returns>
===========================================================</remark>*/
BM2F_ENTERACE(wmsmsm11_inq);
int f_wmsmsm11_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString s_userid("");
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString stock_no = "";
	CString stock_oper_order = "";
	CString heat_no = "";
	CString pono = "";
	CString order_no = "";
	CString mat_no = "";
	CString mat_line_type = " ";
	CString mat_kind = " ";
	CString old_stock_no = "";
	CString st_no = "";
	CDecimal d_from_len = 0, d_to_len = 99999, d_from_width = 0, d_to_width = 99999;
	CDecimal d_thick_fr = 0;
	CDecimal d_thick_to = 0;
	CString event_timec, event_timed = "";


	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */
	CModel twma0 = CModel("TWMA0");
	CModel twma1_q = CModel("TMMSM01");
	CModel twma1 = CModel("TMMSM01");
	CModel twma1_1 = CModel("TMMSM01");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);

	//系统的分页类信息。
	CPageInfo pageInfo;

	//返回数据信息
	bcls_ret->Tables[0].Columns.Add(twma0);
	bcls_ret->Tables[0].Columns.Add(twma1);

	try
	{

		

		twma1.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		twma1.TrimOrBlank();


		
		d_thick_fr = bcls_rec->Tables[0].Rows[0]["MAT_THICK_FR"].ToDecimal();
		d_thick_to = bcls_rec->Tables[0].Rows[0]["MAT_THICK_TO"].ToDecimal();
		if (d_thick_fr > 9999)
		{
			d_thick_fr = 9999;
		}
		if (d_thick_to == 0 ||
			d_thick_to > 9999)
		{
			d_thick_to = 9999;
		}
		if (twma1["STOCK_NO"].ToString().Trim() != "")
		{
			sqlwhere += " AND TWMA0.STOCK_NO = '"+ twma1["STOCK_NO"].ToString() +"' ";
		}
		if (twma1["STOCK_OPER_ORDER"].ToString().Trim() != "")
		{
			sqlwhere += " AND TWMA0.STOCK_OPER_ORDER = '"+ twma1["STOCK_OPER_ORDER"].ToString() +"' ";
		}
		if (twma1["ORDER_NO"].ToString().Trim() != "")
		{
			sqlwhere += " AND TWMA1.ORDER_NO LIKE '"+ twma1["ORDER_NO"].ToString() +"%' ";
		}
		if (twma1["HEAT_NO"].ToString().Trim() != "")
		{
			sqlwhere += " AND TWMA1.HEAT_NO LIKE '"+ twma1["HEAT_NO"].ToString() +"' ";
		}
		if (twma1["PONO"].ToString().Trim() != "")
		{
			sqlwhere += " AND TWMA1.PONO LIKE '"+ twma1["PONO"].ToString() +"' ";
		}
		if (twma1["SG_SIGN"].ToString().Trim() != "")
		{
			sqlwhere += " AND TWMA1.SG_SIGN LIKE '"+ twma1["SG_SIGN"].ToString() +"' ";
		}
		if (twma1["ST_NO"].ToString().Trim() != "")
		{
			sqlwhere += " AND TWMA1.ST_NO LIKE '"+ twma1["ST_NO"].ToString() +"' ";
		}
		Log::Trace("", __FUNCTION__, "MAT_NO11111=[{0}]", twma1_q["MAT_NO"].ToString());
		if (twma1["MAT_NO"].ToString().Trim() != "")
		{
			sqlwhere += " AND TWMA0.MAT_NO IN ( ";
			sqlwhere += twma1["MAT_NO"].ToString();
			sqlwhere += " )";
		}
		
	

			sqlstr =
				" select twma0.MAT_NO,\
				TWMA0.STOCK_OPER_ORDER               SERV_TYPE,\
				TWMA0.STOCK_OPER_ORDER,\
				twma1.LGORT,\
       twm41.C_BATCHID BATCH,\
				twm41.C_DELIVERYID,\
				twm41.C_SENDDEPT,\
				twm41.C_SENDSTOCK,\
				TWM41.C_ACCEPTSTOCK,\
				decode(twm62.MAT_NO, null, ' ', '1') IF_LOGI,\
				twm62.PRACTICE_NO,\
				twm62.TRUCK_NO,\
				 nvl(TWM41.I_RESERVECOL3, twma1.MAT_LEN)        MAT_LEN,\
				nvl(TWM41.DELIVERY_WIDTH, twma1.MAT_WIDTH)     MAT_WIDTH,\
				nvl(TWM41.DELIVERY_THICKNESS, twma1.MAT_THICK) MAT_THICK,\
				nvl(TWM41.N_SENDAMOUNT, twma1.MAT_WT)          MAT_WT\
				from twma0\
				left join vmmsm01 twma1 ON TWMA0.MAT_NO = TWMA1.MAT_NO\
				left join twmSM62 TWM62 ON TWMA0.MAT_NO = TWM62.MAT_NO AND TWM62.UNLOAD_FLAG = '0' AND TWM62.DEAL_FLAG='I'\
				left join twm41dj TWM41 ON TWMA0.MAT_NO = TWM41.C_BATCHUNIT AND TWM41.C_STATESIGN = '1'\
				WHERE TWMA0.PROC_STATUS = '0' AND TWMA0.MAT_NO != ' '\
				AND TWMA0.STOCK_OPER_ORDER LIKE '1%'\
				AND TWMA0.STOCK_OPER_ORDER NOT IN('1B') ";
			
		



		sqlstr = sqlstr + sqlwhere;
		Log::Trace("", __FUNCTION__, "sqlst[{0}]", sqlstr);

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();
		

	
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };

		/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006"), arguments, 1);
		CString str = ex.GetMsg() + "\r\n" + sqlstr;

		/*返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应*/
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);

		/*数据库异常时返回-1，事务将被回滚*/
		s.flag = -1;
		doFlag = -1;
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

