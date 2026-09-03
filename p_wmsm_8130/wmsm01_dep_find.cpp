/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         JHZHAO
Version:		1.0
Date:			2016-03-05
Description:	库图板坯信息查询
**************************************************/

//框架头文件
#include "stdafx.h"
//#include "smhs.h"
//程序用头文件
//#include "twma0.h"
//#include "twma1.h"
//#include "twm00.h"


/*<remark>=========================================================
///<summary>
///库图板坯信息查询
///<para>
===========================================================</remark>*/

BM2F_ENTERACE(wmsm01_dep_find);

int f_wmsm01_dep_find(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDecimal d_thick_fr = 0;
	CDecimal d_thick_to = 0;
	CString v_transfer_flag = "";
	int fetchRowCount = 0;
	CDecimal rowCount = 0;

	/* 实体类定义 */
	CModel twma1_q = CModel("TMMSM01");
	CModel twma1 = CModel("TMMSM01");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";
	CString sqlgroup = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	//系统的分页类信息。
	CPageInfo pageInfo;

	//返回数据信息
	bcls_ret->Tables[0].Columns.Add(twma1);

	try
	{

		// 获取前台传入参数

		//获取库区授权
		CString stock_no_auth = "' '";
		EIClass *bcls_auth = new EIClass;
	
		delete bcls_auth;


		//分页信息
		CDataTable& table = bcls_ret->Tables.Add("PAGEINFO");
		table.Columns.Add(DT_DECIMAL, "recordsum");

		//2)获取分页信息
		if (bcls_rec->Tables.Contains("PageInfo"))
		{
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		else
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = -1;  //每页记录数量
		}

		Log::Trace("", __FUNCTION__, "pageInfo.RecordFrom[{0}]pageInfo.PageSize[{1}]", pageInfo.RecordFrom, pageInfo.PageSize);


		twma1_q.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		twma1_q.TrimOrBlank();

		/*if (bcls_rec->Tables[0].Columns.Contains("MAT_THICK_FR"))
			d_thick_fr = bcls_rec->Tables[0].Rows[0]["MAT_THICK_FR"].ToDecimal();*/


		Log::Trace("", __FUNCTION__, "twma1_q.MAT_NO=[{0}]", twma1_q["MAT_NO"].ToString());



		if (twma1_q["MAT_NO"].ToString().Trim() != "")
		{
			sqlwhere += " AND T2.MAT_NO like @mat_no ";
		}

		sqlstr = " SELECT COUNT(MAT_NO) FROM TMMSM01 WHERE IN_FLAG = '1' ";

		sqlstr = sqlstr + sqlwhere;
		Log::Trace("", __FUNCTION__, "sqlcount[{0}]", sqlstr);

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("MAT_NO", twma1_q["MAT_NO"].ToString());

		rowCount = cmd_inq.ExecuteScalar();
		cmd_inq.Close();

		Log::Trace("", __FUNCTION__, "SELECT COUNT rowCount=[{0}]", rowCount);


		sqlstr = " SELECT * FROM TMMSM01 WHERE IN_FLAG = '1' ";
		sqlstr = sqlstr + sqlwhere;
		Log::Trace("", __FUNCTION__, "sql[{0}]", sqlstr);

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("MAT_NO", twma1_q["MAT_NO"].ToString());
		cmd_inq.ExecuteReader();
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();


		//返回记录总数
		CDataRow& row1 = bcls_ret->Tables["PAGEINFO"].Rows.Add();
		row1["recordsum"] = rowCount.ToInt32();
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

