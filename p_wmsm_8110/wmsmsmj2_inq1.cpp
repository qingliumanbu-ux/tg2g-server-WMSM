/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         
Version:		1.0
Date:			
Description:	
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件




BM2F_ENTERACE(wmsmsmj2_inq1);

int f_wmsmsmj2_inq1(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */


	/* 业务变量 */
	CString MAT_NO = "";
	CString HALL_NO = "";
	CString STOCK_PLACE_NOC = "";
	CString STOCK_PLACE_NOD = "";


	/* 数据库SQL操作字符串 */
	CString sqlstr = "";



	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	//系统的分页类信息。
	CPageInfo pageInfo;

	try
	{


		//查询条件获取
		if (bcls_rec->Tables[0].Columns.Contains("MAT_NO")){
			MAT_NO = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString();
			Log::Trace("", __FUNCTION__, "MAT_NO = [{0}]", MAT_NO);
		}
		if (bcls_rec->Tables[0].Columns.Contains("HALL_NO")){
			HALL_NO = bcls_rec->Tables[0].Rows[0]["HALL_NO"].ToString();
			Log::Trace("", __FUNCTION__, "HALL_NO = [{0}]", HALL_NO);
		}
		if (bcls_rec->Tables[0].Columns.Contains("STOCK_PLACE_NOC")){
			STOCK_PLACE_NOC = bcls_rec->Tables[0].Rows[0]["STOCK_PLACE_NOC"].ToString();
			Log::Trace("", __FUNCTION__, "STOCK_PLACE_NOC = [{0}]", STOCK_PLACE_NOC);
		}
		if (bcls_rec->Tables[0].Columns.Contains("STOCK_PLACE_NOD")){
			STOCK_PLACE_NOD = bcls_rec->Tables[0].Rows[0]["STOCK_PLACE_NOD"].ToString();
			Log::Trace("", __FUNCTION__, "STOCK_PLACE_NOD = [{0}]", STOCK_PLACE_NOD);
		}
		
		//sql
		sqlstr = "SELECT A.* FROM TWM04 A  WHERE 1=1 ";
		if (MAT_NO.Trim() != "")
		{
			sqlstr += " AND A.STOCK_PLACE_NO IN(SELECT STOCK_PLACE_NO FROM TMMSM01 WHERE MAT_NO LIKE @MAT_NO||'%') ";
		}
		if (HALL_NO.Trim() != "")
		{
			sqlstr += " AND A.HALL_NO LIKE @HALL_NO||'#' ";
		}
		
		if (STOCK_PLACE_NOC.Trim() != "")
		{
			sqlstr += " AND A.STOCK_PLACE_NO>=@STOCK_PLACE_NOC ";
		}
		if (STOCK_PLACE_NOD.Trim() != "")
		{
			sqlstr += " AND A.STOCK_PLACE_NO<=@STOCK_PLACE_NOD ";
		}
	

		Log::Debug("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("MAT_NO", MAT_NO);
		cmd_inq.Parameters.Set("STOCK_PLACE_NOD", STOCK_PLACE_NOD);
		cmd_inq.Parameters.Set("STOCK_PLACE_NOC", STOCK_PLACE_NOC);
		cmd_inq.Parameters.Set("HALL_NO", HALL_NO);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);

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

