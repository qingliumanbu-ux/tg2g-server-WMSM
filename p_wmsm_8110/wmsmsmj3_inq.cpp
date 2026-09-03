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




BM2F_ENTERACE(wmsmsmj3_inq);

int f_wmsmsmj3_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
	CString ROLL_PLAN_NO = "";
	CString LOT_NO = "";



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
		if (bcls_rec->Tables[0].Columns.Contains("ROLL_PLAN_NO")){
			ROLL_PLAN_NO = bcls_rec->Tables[0].Rows[0]["ROLL_PLAN_NO"].ToString();
			Log::Trace("", __FUNCTION__, "ROLL_PLAN_NO = [{0}]", ROLL_PLAN_NO);
		}
		if (bcls_rec->Tables[0].Columns.Contains("LOT_NO")){
			LOT_NO = bcls_rec->Tables[0].Rows[0]["LOT_NO"].ToString();
			Log::Trace("", __FUNCTION__, "LOT_NO = [{0}]", LOT_NO);
		}

		
		//sql
		sqlstr = "SELECT A.*,B.* "
			//"(select count(mat_no) from tmmsm01 ta where ta.stock_place_no = B.stock_place_no and to_number(decode(ta.layerno,'',0,' ',0,null,0,ta.layerno)) > to_number(B.layerno)) over_mat_num,"
			//"(select count(mat_no) from tmmsm01 ta where ta.stock_place_no = B.stock_place_no and not exists(select mat_no from tymsm59 tb where tb.mat_no = ta.mat_no and tb.plan_status in('0', '1', '2'))) plan_mat_num, "
			" FROM TPSSMHRA2 A  left join tmmsm01 b on a.mat_no=b.mat_no WHERE 1=1 ";
		if (MAT_NO.Trim() != "")
		{
			sqlstr += " AND A.MAT_NO like @MAT_NO||'%'";
		}
		if (LOT_NO.Trim() != "")
		{
			sqlstr += " AND A.LOT_NO LIKE @LOT_NO||'#' ";
		}
		if (ROLL_PLAN_NO.Trim() != "")
		{
			sqlstr += " AND A.ROLL_PLAN_NO LIKE @ROLL_PLAN_NO||'#' ";
		}
		
	

		Log::Debug("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("MAT_NO", MAT_NO);
		cmd_inq.Parameters.Set("ROLL_PLAN_NO", ROLL_PLAN_NO);
		cmd_inq.Parameters.Set("LOT_NO", LOT_NO);
		int rowCount =cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);

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

