/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:
Version:     1.0
Date:
Description:  
**************************************************/
/***** C/C++ 的标准头文件部分 *****/
#include "stdafx.h"
//#include "AppFunc.h"

//函数申明
int f_epes_get_auth_other(const char *iuser, int irestype, EIClass *bcls_ret, CDbConnection * conn);

/*<remark>=========================================================
/// <summary>
///  库区号查询
/// <para>查询计划描述。
/// </para>
/// <para>数据库表：TSMHR09</para>
/// </summary>
/// <param name=""> </param>
/// <returns>返回参数：计划号</returns>
===========================================================</remark>*/
// Service 入口
BM2F_ENTERACE(sm00_BillOfLadingNO)
int f_sm00_BillOfLadingNO(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;

	/* 业务变量 */
	CString s_userid("");
	CString v_mat_line_type = "";
	CString v_mat_kind = "";
	CString v_factory_div = "";
	CString v_stock_type_code = "";
	CString v_stock_no = "";

	CString bill_of_lading_no = "";
	CString c_delivy_status = ""; //1:查码单，2:查计划号;否则为装车时查询计划号
	CString v_vehicle_no = "";

	/* 数据库SQL操作字符串 */
	CString sqlstr(" "), sqlstr1(" "), sqlstr2(" ");
	CString sqlwhere("");

	/* 实体类定义 */

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	//AppFunc XYZ(bcls_rec, bcls_ret, conn);

	try
	{
		Log::Trace("", __FUNCTION__, "====== 接收块开始 =======  ");
		//XYZ.Prt(bcls_rec);
		Log::Trace("", __FUNCTION__, "====== 接收块结束 =======  ");

		// 获取前台传入参数
		s_userid = s.userid;
		//获取库区授权
		CString stock_no_auth = "' '";
		EIClass *bcls_auth = new EIClass;
		//if (f_epes_get_auth_other(s.userid, 5, bcls_auth, conn) != 0)   //获得细部授权库区代码
		//{
		//	throw CApplicationException(-1, s.msg, log.Location);
		//	Log::Trace("", __FUNCTION__, "===========1");
		//}
		//for (int fetchRowCount = 0; fetchRowCount < bcls_auth->Tables[0].Rows.get_Count(); fetchRowCount++)
		//{
		//	stock_no_auth += ", '" + bcls_auth->Tables[0].Rows[fetchRowCount]["name"].ToString() + "' ";
		//}
		//delete bcls_auth;
		
		if (bcls_rec->Tables[0].Columns.Contains("BILL_OF_LADING_NO"))
		{
			bill_of_lading_no = bcls_rec->Tables[0].Rows[0]["BILL_OF_LADING_NO"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("DELIVY_STATUS"))
		{
			c_delivy_status = bcls_rec->Tables[0].Rows[0]["DELIVY_STATUS"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("DELIVY_STATUS"))
		{
			c_delivy_status = bcls_rec->Tables[0].Rows[0]["DELIVY_STATUS"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("STOCK_NO"))
		{
			v_stock_no = bcls_rec->Tables[0].Rows[0]["STOCK_NO"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("VEHICLE_NO"))
		{
			v_vehicle_no = bcls_rec->Tables[0].Rows[0]["VEHICLE_NO"].ToString().Trim();
		}

		Log::Trace("", __FUNCTION__, "===== stock_no_auth=[{0}]", stock_no_auth);
		Log::Trace("", __FUNCTION__, "===== delivy_status=[{0}]", c_delivy_status);


		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:			// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:			// MS SQL Server数据库
		case DB_KIND_ORACLE:		// Oracle 数据库
		default:					// 所有数据库适用，通用SQL语句

			sqlstr =
//				" SELECT ' ' AS STOCK_NO, ' ' AS STOCK_DESC FROM sysibm.sysdummy1 UNION ALL"
				" SELECT DISTINCT T2.BILL_OF_LADING_NO"
				" FROM TSMSM10 T2 WHERE DELIVY_PLAN_STATUS NOT IN('5','X','F')";
				//" SELECT T2.STOCK_NO, T2.STOCK_DESC "
				//" FROM TWM0A T1, TWM01 T2 "
				//" WHERE T1.STOCK_NO = T2.STOCK_NO "
				//" AND T1.GROUPID IN ( "
				//" SELECT GROUPID FROM TESGROUPMEMBER "
				//" WHERE MEMBERID IN ( "
				//" SELECT ID FROM TESUSERINFO WHERE ENAME = @userid "
				//" ) "
				//" ) ";
			//" union "
			//" select STOCK_NO, STOCK_DESC from twm01 where 'admin' = @userid "

			sqlstr1 = "SELECT DISTINCT BILL_OF_LADING_NO  FROM tsmpe02 t WHERE 1 = 1";
			sqlstr2 = "SELECT DISTINCT STACKING_NO FROM tsmpe02 t WHERE 1 = 1 ";

			break;
		}

		if (v_stock_no != ""){
			sqlwhere += " and STOCK_NO = @stock_no ";
		}
		if (bill_of_lading_no != ""){
			sqlwhere += " and BILL_OF_LADING_NO = @bill_of_lading_no ";
		}
		if (v_vehicle_no != ""){
			sqlwhere += " and VEHICLE_NO = @vehicle_no ";
		}


		if (c_delivy_status == "1"){
			sqlstr = sqlstr2 + sqlwhere;
		}
		if (c_delivy_status == "2"){
			sqlstr = sqlstr1 + sqlwhere;
		}
		
		Log::Trace("", __FUNCTION__, "===== sqlstr=[{0}]", sqlstr);


		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stock_no", v_stock_no);
		cmd_inq.Parameters.Set("vehicle_no", v_vehicle_no);
		cmd_inq.Parameters.Set("bill_of_lading_no", bill_of_lading_no);

		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], 0, -1);  //0,-1：非翻页查询
		cmd_inq.Close();
	}
	catch (CDbException& ex)					//捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000021")/*信息读取失败。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1; //数据库异常时返回-1，事务将被回滚
	}
	catch (const CApplicationException& ex)	//捕获应用错误
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

	return(doFlag);
}
