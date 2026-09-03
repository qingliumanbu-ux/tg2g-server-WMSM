/*<remark>=========================================================
/// <summary>
///  库区号查询
/// <para>查询库区号、库区描述。
/// </para>
/// <para>数据库表：(库号定义表)</para>
/// </summary>
/// <param name=""> </param>
/// <returns>返回参数：库区号、库区描述</returns>
===========================================================</remark>*/
/***** C/C++ 的标准头文件部分 *****/
#include "stdafx.h"

//函数申明
int f_epes_get_auth_other(const char *iuser, int irestype, EIClass *bcls_ret, CDbConnection * conn);

//BM2_FUNCTION_IMPORT
//int ts0006_trunk_no_type_inq(const char *iuser, int irestype, EIClass *bcls_ret, CDbConnection * conn);


// Service 入口
BM2F_ENTERACE(wm00_truckNo1)
int f_wm00_truckNo1(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

/* 程序内部变量 */
int doFlag = 0;

/* 业务变量 */
CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
CString v_out_stock_time = "";
CString bill_of_lading_no = "";
CString c_delivery_mode = "";
CString c_delivy_plan_type = "";
CString c_delivy_status = "";//发货页面标志符:2为发货页面;空为装车
CString v_stock_no = "";

/* 数据库SQL操作字符串 */
CString sqlstr(" "), sqlstr1(" "), sqlstr2(" ");

/* 数据库操作类定义 */
CDbCommand cmd_inq(conn);

try
{
	// 获取前台传入参数
	if (bcls_rec->Tables[0].Columns.Contains("OUT_STOCK_TIME"))
	{
		v_out_stock_time = bcls_rec->Tables[0].Rows[0]["OUT_STOCK_TIME"].ToString().Trim();
	}
	if (v_out_stock_time == ""){
		v_out_stock_time = datetime;
	}
	if (bcls_rec->Tables[0].Columns.Contains("BILL_OF_LADING_NO"))
	{
		bill_of_lading_no = bcls_rec->Tables[0].Rows[0]["BILL_OF_LADING_NO"].ToString().Trim();
	}
	if (bcls_rec->Tables[0].Columns.Contains("DELIVY_STATUS"))
	{
		c_delivy_status = bcls_rec->Tables[0].Rows[0]["DELIVY_STATUS"].ToString().Trim();
	}
	if (bcls_rec->Tables[0].Columns.Contains("STOCK_NO"))
	{
		v_stock_no = bcls_rec->Tables[0].Rows[0]["STOCK_NO"].ToString().Trim();
	}

	//bcls_ret->Tables[0].set_TableName("WM_DYN_SQL");
	//bcls_ret->Tables["WM_DYN_SQL"].Columns.Add(DT_STRING, "CODE");
	//bcls_ret->Tables["WM_DYN_SQL"].Columns.Add(DT_STRING, "CODE_DESC_1_CONTENT");

	/* ***** 打印输入参数 ***** */
	Log::Debug("", __FUNCTION__, "传入参数 v_out_stock_time = [{0}]", v_out_stock_time.Substring(0, 8));
	Log::Debug("", __FUNCTION__, "传入参数 bill_of_lading_no = [{0}]", bill_of_lading_no);
	Log::Debug("", __FUNCTION__, "传入参数 c_delivy_status = [{0}]", c_delivy_status);
	Log::Debug("", __FUNCTION__, "传入参数 v_stock_no = [{0}]", v_stock_no);



	switch (conn->DatabaseKind)
	{
	case DB_KIND_DB2:			// DB2 数据库（未开Oracle兼容）
	case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
	case DB_KIND_MSSQL:			// MS SQL Server数据库
	default:					// 所有数据库适用，通用SQL语句
		//sqlstr =
		//          "SELECT DISTINCT D.TRUCK_NO FROM TWM0D D ,TWM0C C "
		//          "WHERE D.LOADING_PLAN_NO = C.LOADING_PLAN_NO AND C.PRO_FLAG = 'I' "
		//	"AND C.STOCK_NO = @stock_no AND C.AIM_STOCK_NO = @aim_stock_no AND D.EXPIRY_DATE >= @out_stock_time AND D.DATE_TIME <= @out_stock_time"
		//	;
		sqlstr1 = "select DELIVERY_MODE,DELIVY_PLAN_TYPE,"
			"(select sum(stacking_wt) from tsmpe11 where stacking_status = '4' and bill_of_lading_no = a.bill_of_lading_no and order_no = a.order_no) TRUCK_WT"
			" from tsmsm10 a     where 1 = 1     AND DELIVY_PLAN_STATUS <= '4'   and stock_place_no != 'Y' and bill_of_lading_no = @bill_of_lading_no "
			"order by a.PRG_SEND_TIME desc";
		sqlstr = "SELECT VEHICLE_NO,LOADING_WT FROM TSMSM14 WHERE  ARCHIVE_FLAG != '1' and  BILL_OF_LADING_NO = @bill_of_lading_no ";//装车下拉框查询
		sqlstr2 = "SELECT DISTINCT VEHICLE_NO FROM tsmpe02 t WHERE VEHICLE_NO <> '' AND STOCK_NO = @stock_no";//发货车辆下拉框查询
		break;
	}

	if (c_delivy_status != ""){
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr2);

		cmd_inq.SetCommandText(sqlstr2);

		//cmd_inq.Parameters.Set("out_stock_time", v_out_stock_time.Substring(0, 8));
		//cmd_inq.Parameters.Set("aim_stock_no", v_aim_stock_no);
		cmd_inq.Parameters.Set("stock_no", v_stock_no);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], 0, -1);  //0,-1：非翻页查询
		cmd_inq.Close();
	}
	else{
		/*if (bill_of_lading_no.Trim() = ""){
			cmd_inq.SetCommandText(sqlstr1);
			cmd_inq.Parameters.Set("bill_of_lading_no", bill_of_lading_no);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read()){
				c_delivery_mode = cmd_inq.GetString(1);
				c_delivy_plan_type = cmd_inq.GetString(2);

			}
		}
		cmd_inq.Close();

		Log::Debug("", __FUNCTION__, "传入参数 c_order_no = [{0}]", c_delivery_mode);
		Log::Debug("", __FUNCTION__, "传入参数 c_delivy_plan_type = [{0}]", c_delivy_plan_type);

		if (c_delivy_plan_type == "1" || c_delivery_mode.Trim() == "Y"){


		}
		else{*/
			Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr2);

			cmd_inq.SetCommandText(sqlstr2);
			cmd_inq.Parameters.Set("bill_of_lading_no", bill_of_lading_no);
			//cmd_inq.Parameters.Set("out_stock_time", v_out_stock_time.Substring(0, 8));
			//cmd_inq.Parameters.Set("aim_stock_no", v_aim_stock_no);
			//cmd_inq.Parameters.Set("stock_no", v_stock_no);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0], 0, -1);  //0,-1：非翻页查询
			cmd_inq.Close();
		//}
	}

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
