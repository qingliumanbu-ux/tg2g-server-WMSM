/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         JHZHAO
Version:		1.0
Date:			2016-03-05
Description:	待入库材料信息查询
**************************************************/

//框架头文件
#include "stdafx.h"
//#include "smhs.h"
//程序用头文件
//#include "twma1.h"
//#include "twma0.h"



/*<remark>=========================================================
///<summary>
///待入库材料信息查询
///<para>
///2.排序方式：队列写入时间
///</para>
///<para>数据库表：TWMA0 倒躲队列；TWMA1 物料主档表
///<returns>返回符合查询条件的队列信息</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wmsmsm12zklog_inq);

int f_wmsmsm12zklog_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString s_userid("");
	CString TRANSFER_BILL_NO, TRANSFER_PLAN_NO, SEND_TIMEC, SEND_TIMED, MAT_NO = "";
	CDecimal rowCount = 0;
	int fetchRowCount = 0;
	int recordFrom = 0;	//起始页
	int pageSize = 0;	//每页记录数
	/* 实体类定义 */
	//CTWMA1 twma1_q(conn);
	//CTWMA1 twma1(conn);
	//CTWMA1 twma1_1(conn);
	//CTWMA0 twma0(conn);
	CModel twma1_q = CModel("TMMSM01");
	CModel twma1 = CModel("TMMSM01");
	CModel twma1_1 = CModel("TMMSM01");
	CModel twma0 = CModel("TWMA0");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	//系统的分页类信息。
	CPageInfo pageInfo;

	try
	{
		//获取分页参数
		if (!bcls_rec->Tables.Contains("PageInfo"))
		{
			recordFrom = 0;
			pageSize = 1;
		}
		else
		{
			if (bcls_rec->Tables["PageInfo"].Columns.Contains("RecordFrom"))
			{
				recordFrom = (int)bcls_rec->Tables["PageInfo"].Rows[0]["RecordFrom"];
				recordFrom = recordFrom-1;
				Log::Debug("", __FUNCTION__, "传入参数recordFrom = [{0}]", recordFrom);
			}
			if (bcls_rec->Tables["PageInfo"].Columns.Contains("PageSize"))
			{
				pageSize = (int)bcls_rec->Tables["PageInfo"].Rows[0]["PageSize"];
				Log::Debug("", __FUNCTION__, "传入参数pageSize = [{0}]", pageSize);
			}
		}
		//查询条件获取
		if (bcls_rec->Tables[0].Columns.Contains("TRANSFER_BILL_NO")){
			TRANSFER_BILL_NO = bcls_rec->Tables[0].Rows[0]["TRANSFER_BILL_NO"].ToString();
			Log::Trace("", __FUNCTION__, "TRANSFER_BILL_NO = [{0}]", TRANSFER_BILL_NO);
		}
		if (bcls_rec->Tables[0].Columns.Contains("TRANSFER_PLAN_NO")){
			TRANSFER_PLAN_NO = bcls_rec->Tables[0].Rows[0]["TRANSFER_PLAN_NO"].ToString();
			Log::Trace("", __FUNCTION__, "TRANSFER_PLAN_NO = [{0}]", TRANSFER_PLAN_NO);
		}
		if (bcls_rec->Tables[0].Columns.Contains("SEND_TIMEC")){
			SEND_TIMEC = bcls_rec->Tables[0].Rows[0]["SEND_TIMEC"].ToString();
			Log::Trace("", __FUNCTION__, "SEND_TIMEC = [{0}]", SEND_TIMEC);
		}
		if (bcls_rec->Tables[0].Columns.Contains("SEND_TIMED")){
			SEND_TIMED = bcls_rec->Tables[0].Rows[0]["SEND_TIMED"].ToString();
			Log::Trace("", __FUNCTION__, "SEND_TIMED = [{0}]", SEND_TIMED);
		}
		if (bcls_rec->Tables[0].Columns.Contains("MAT_NO")){
			MAT_NO = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString();
			Log::Trace("", __FUNCTION__, "MAT_NO = [{0}]", MAT_NO);
		}
		//sql
		sqlstr = "SELECT * FROM TWM42 A WHERE AFFIRM_MARK='9'";
		if (MAT_NO.Trim() != "")
		{
			sqlstr += " AND A.MAT_NO like @MAT_NO||'%'";
		}
		if (TRANSFER_BILL_NO.Trim() != "")
		{
			sqlstr += " AND A.TRANSFER_BILL_NO LIKE @TRANSFER_BILL_NO||'%' ";
		}
		if (TRANSFER_PLAN_NO.Trim() != "")
		{
			sqlstr += " AND A.TRANSFER_PLAN_NO LIKE @TRANSFER_PLAN_NO||'%' ";
		}
		if (SEND_TIMEC.Trim() != "")
		{
			sqlstr += " AND A.SEND_TIME >=@SEND_TIMEC ";
		}
		if (SEND_TIMED.Trim() != "")
		{
			sqlstr += " AND A.SEND_TIME <=@SEND_TIMED ";
		}
		Log::Debug("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("MAT_NO", MAT_NO);
		cmd_inq.Parameters.Set("SEND_TIMEC", SEND_TIMEC);
		cmd_inq.Parameters.Set("TRANSFER_PLAN_NO", TRANSFER_PLAN_NO);
		cmd_inq.Parameters.Set("SEND_TIMED", SEND_TIMED);
		cmd_inq.Parameters.Set("TRANSFER_BILL_NO", TRANSFER_BILL_NO);
		Log::Debug("", __FUNCTION__, "recordFrom = [{0}]", recordFrom);
		Log::Debug("", __FUNCTION__, "pageSize = [{0}]", pageSize);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], recordFrom, pageSize);

		sqlstr = "SELECT COUNT(*) FROM (" + sqlstr + ")";
		cmd_inq.SetCommandText(sqlstr);
		int rowCount = cmd_inq.ExecuteScalar().ToInt32();
		Log::Debug("", __FUNCTION__, "rowCount = [{0}]", rowCount);
		cmd_inq.Close();
		//返回记录总数
		//bcls_ret->Tables.Add();
		//bcls_ret->Tables[1].set_TableName("PAGEINFO");
		CDataTable& table = bcls_ret->Tables.Add("PAGEINFO");
		table.Columns.Add(DT_DECIMAL, "recordsum");
		CDataRow& row1 = bcls_ret->Tables["PAGEINFO"].Rows.Add();
		row1["recordsum"] = rowCount;

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

