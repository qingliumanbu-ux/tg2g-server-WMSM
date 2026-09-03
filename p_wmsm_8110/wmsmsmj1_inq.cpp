/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         
Version:		1.0
Date:			
Description:	在库板坯计划查询
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件



/*<remark>=========================================================
///<summary>
///库位材料倒垛查询
///<para>
///2.排序方式：STOCK_NO,HALL_NO
///</para>
///<para>数据库表：TWM04 仓库跨号信息查询；TWMA2
///<returns>返回符合查询条件的仓库跨号信息</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wmsmsmj1_inq);

int f_wmsmsmj1_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CDecimal rowCount = 0;
	int fetchRowCount = 0;
	int recordFrom = 0;	//起始页
	int pageSize = 0;	//每页记录数
	/* 实体类定义 */
	//CTWM04 twm04(conn);
	//CTWMA1 twma1_q(conn);
	CModel twm0c = CModel("TWM0C");
	CModel twma1_q = CModel("TMMSM01");

	/* 业务变量 */
	CString MAT_NO = "";
	CString SURFACE_DECIDE_CODE = "";
	CString PRI_GRADE = "";
	CString END_TIME_EVENTC = "";
	CString END_TIME_EVENTD = "";
	CString FIN_CONFM_FLAG = "";

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";



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
		if (bcls_rec->Tables[0].Columns.Contains("MAT_NO")){
			MAT_NO = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString();
			Log::Trace("", __FUNCTION__, "MAT_NO = [{0}]", MAT_NO);
		}
		if (bcls_rec->Tables[0].Columns.Contains("SURFACE_DECIDE_CODE")){
			SURFACE_DECIDE_CODE = bcls_rec->Tables[0].Rows[0]["SURFACE_DECIDE_CODE"].ToString();
			Log::Trace("", __FUNCTION__, "SURFACE_DECIDE_CODE = [{0}]", SURFACE_DECIDE_CODE);
		}
		if (bcls_rec->Tables[0].Columns.Contains("PRI_GRADE")){
			PRI_GRADE = bcls_rec->Tables[0].Rows[0]["PRI_GRADE"].ToString();
			Log::Trace("", __FUNCTION__, "PRI_GRADE = [{0}]", PRI_GRADE);
		}
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME_EVENTC")){
			END_TIME_EVENTC = bcls_rec->Tables[0].Rows[0]["END_TIME_EVENTC"].ToString();
			Log::Trace("", __FUNCTION__, "END_TIME_EVENTC = [{0}]", END_TIME_EVENTC);
		}
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME_EVENTD")){
			END_TIME_EVENTD = bcls_rec->Tables[0].Rows[0]["END_TIME_EVENTD"].ToString();
			Log::Trace("", __FUNCTION__, "END_TIME_EVENTD = [{0}]", END_TIME_EVENTD);
		}
		if (bcls_rec->Tables[0].Columns.Contains("FIN_CONFM_FLAG")){
			FIN_CONFM_FLAG = bcls_rec->Tables[0].Rows[0]["FIN_CONFM_FLAG"].ToString();
			Log::Trace("", __FUNCTION__, "FIN_CONFM_FLAG = [{0}]", FIN_CONFM_FLAG);
		}
		//sql
		sqlstr = "SELECT A.*,B.* FROM TWMJ1 A LEFT JOIN TMMSM01 B ON A.MAT_NO=B.MAT_NO WHERE 1=1 ";
		if (MAT_NO.Trim() != "")
		{
			sqlstr += " AND A.MAT_NO like @MAT_NO ||'%' ";
		}
		if (SURFACE_DECIDE_CODE.Trim() != "")
		{
			sqlstr += " AND A.SURFACE_DECIDE_CODE =@SURFACE_DECIDE_CODE ";
		}
		if (PRI_GRADE.Trim() != "")
		{
			sqlstr += " AND A.PRI_GRADE =@PRI_GRADE ";
		}

		if (END_TIME_EVENTC.Trim() != "")
		{
			sqlstr += " AND A.END_TIME_EVENT>=@END_TIME_EVENTC ";
		}
		if (END_TIME_EVENTD.Trim() != "")
		{
			sqlstr += " AND A.END_TIME_EVENTD<=@END_TIME_EVENTC ";
		}
		if (FIN_CONFM_FLAG.Trim() != "")
		{
			sqlstr += " AND A.FIN_CONFM_FLAG =@FIN_CONFM_FLAG ";
		}

		Log::Debug("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("MAT_NO", MAT_NO);
		cmd_inq.Parameters.Set("SURFACE_DECIDE_CODE", SURFACE_DECIDE_CODE);
		cmd_inq.Parameters.Set("PRI_GRADE", PRI_GRADE);
		cmd_inq.Parameters.Set("END_TIME_EVENTC", END_TIME_EVENTC);
		cmd_inq.Parameters.Set("END_TIME_EVENTD", END_TIME_EVENTD);
		cmd_inq.Parameters.Set("FIN_CONFM_FLAG", FIN_CONFM_FLAG);
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
		cmd_inq.Close();
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

