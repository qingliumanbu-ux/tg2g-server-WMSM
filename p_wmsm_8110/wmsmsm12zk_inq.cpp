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

//函数申明
int f_epes_get_auth_other(const char *iuser, int irestype, EIClass *bcls_ret, CDbConnection * conn);

/*<remark>=========================================================
///<summary>
///待入库材料信息查询
///<para>
///2.排序方式：队列写入时间
///</para>
///<para>数据库表：TWMA0 倒躲队列；TWMA1 物料主档表
///<returns>返回符合查询条件的队列信息</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wmsmsm12zk_inq);

int f_wmsmsm12zk_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString s_userid("");
	CString ORDER_NO, TRANSFER_PLAN_NO, HEAT_NO, PONO, SG_SIGN, ST_NO, MAT_NO = "";
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
				recordFrom = recordFrom - 1;
				Log::Debug("", __FUNCTION__, "传入参数recordFrom = [{0}]", recordFrom);
			}
			if (bcls_rec->Tables["PageInfo"].Columns.Contains("PageSize"))
			{
				pageSize = (int)bcls_rec->Tables["PageInfo"].Rows[0]["PageSize"];
				Log::Debug("", __FUNCTION__, "传入参数pageSize = [{0}]", pageSize);
			}
		}
		//查询条件获取
		if (bcls_rec->Tables[0].Columns.Contains("ORDER_NO")){
			ORDER_NO = bcls_rec->Tables[0].Rows[0]["ORDER_NO"].ToString();
			Log::Trace("", __FUNCTION__, "ORDER_NO = [{0}]", ORDER_NO);
		}
		if (bcls_rec->Tables[0].Columns.Contains("TRANSFER_PLAN_NO")){
			TRANSFER_PLAN_NO = bcls_rec->Tables[0].Rows[0]["TRANSFER_PLAN_NO"].ToString();
			Log::Trace("", __FUNCTION__, "TRANSFER_PLAN_NO = [{0}]", TRANSFER_PLAN_NO);
		}
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO")){
			HEAT_NO = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString();
			Log::Trace("", __FUNCTION__, "HEAT_NO = [{0}]", HEAT_NO);
		}
		if (bcls_rec->Tables[0].Columns.Contains("PONO")){
			PONO = bcls_rec->Tables[0].Rows[0]["PONO"].ToString();
			Log::Trace("", __FUNCTION__, "PONO = [{0}]", PONO);
		}
		if (bcls_rec->Tables[0].Columns.Contains("SG_SIGN")){
			SG_SIGN = bcls_rec->Tables[0].Rows[0]["SG_SIGN"].ToString();
			Log::Trace("", __FUNCTION__, "SG_SIGN = [{0}]", SG_SIGN);
		}
		if (bcls_rec->Tables[0].Columns.Contains("ST_NO")){
			ST_NO = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString();
			Log::Trace("", __FUNCTION__, "ST_NO = [{0}]", ST_NO);
		}
		if (bcls_rec->Tables[0].Columns.Contains("MAT_NO")){
			MAT_NO = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString();
			Log::Trace("", __FUNCTION__, "MAT_NO = [{0}]", MAT_NO);
		}
		//sql
		sqlstr = "SELECT a.*,b.TRANSFER_PLAN_NO,b.TRANSFER_BILL_NO,c.* from twma0 a left join twm42 b on a.mat_no=b.mat_no and b.AFFIRM_MARK='8'  "
			" left join tmmsm01 c on a.mat_no=c.mat_no "
			" WHERE 1=1 AND A.PROC_STATUS!='9' and a.STOCK_OPER_ORDER='2G'";
		if (MAT_NO.Trim() != "")
		{
			sqlstr += " AND A.MAT_NO like @MAT_NO||'%'";
		}
		if (ORDER_NO.Trim() != "")
		{
			sqlstr += " AND B.ORDER_NO LIKE @ORDER_NO||'%' ";
		}
		if (TRANSFER_PLAN_NO.Trim() != "")
		{
			sqlstr += " AND B.TRANSFER_PLAN_NO LIKE @TRANSFER_PLAN_NO||'%' ";
		}
		if (HEAT_NO.Trim() != "")
		{
			sqlstr += " AND C.HEAT_NO LIKE @HEAT_NO||'%' ";
		}
		if (PONO.Trim() != "")
		{
			sqlstr += " AND C.PONO LIKE @PONO||'%' ";
		}
		if (SG_SIGN.Trim() != "")
		{
			sqlstr += " AND C.SG_SIGN LIKE @SG_SIGN||'%' ";
		}
		if (ST_NO.Trim() != "")
		{
			sqlstr += " AND C.ST_NO LIKE @ST_NO||'%' ";
		}
		if (ST_NO.Trim() != "")
		{
			sqlstr += " AND C.ST_NO LIKE @ST_NO||'%' ";
		}
		Log::Debug("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("MAT_NO", MAT_NO);
		cmd_inq.Parameters.Set("ORDER_NO", ORDER_NO);
		cmd_inq.Parameters.Set("TRANSFER_PLAN_NO", TRANSFER_PLAN_NO);
		cmd_inq.Parameters.Set("HEAT_NO", HEAT_NO);
		cmd_inq.Parameters.Set("PONO", PONO);
		cmd_inq.Parameters.Set("SG_SIGN", SG_SIGN);
		cmd_inq.Parameters.Set("ST_NO", ST_NO);
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

