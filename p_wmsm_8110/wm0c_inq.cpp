/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         ljnie
Version:		1.0
Date:			2016/7/6 15:34:36
Description:	用车申请查询函数
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件
//#include "twm04.h"


//函数申明
int f_epes_get_auth_other(const char *iuser, int irestype, EIClass *bcls_ret, CDbConnection * conn);

/*<remark>=========================================================
///<summary>
///库位材料倒垛查询
///<para>
///2.排序方式：STOCK_NO,HALL_NO
///</para>
///<para>数据库表：TWM04 仓库跨号信息查询；TWMA2
///<returns>返回符合查询条件的仓库跨号信息</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wm0c_inq);

int f_wm0c_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */
	//CTWM04 twm04(conn);
	//CTWMA1 twma1_q(conn);
	CModel twm0c = CModel("TWM0C");
	CModel twma1_q = CModel("TMMSM01");

	/* 业务变量 */
	CString stock_no("");
	CString loading_plan_no("");
	CString app_date_fr("");
	CString app_date_to("");
	CString mat_no("");
	CString order_no = "";
	CString sg_sign = "";
	CDecimal d_thick_fr = 0;
	CDecimal d_thick_to = 0;

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlstr1 = "";
	CString sqlwhere = "";
	CString sqlorderby = "";
	CString s_userid("");


	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	//系统的分页类信息。
	CPageInfo pageInfo;

	try
	{

		// 获取前台传入参数
		s_userid = s.userid;
		//获取库区授权
		/*CString stock_no_auth = "' '";
		EIClass *bcls_auth = new EIClass;
		if (f_epes_get_auth_other(s.userid, 5, bcls_auth, conn) != 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
		for (int fetchRowCount = 0; fetchRowCount < bcls_auth->Tables[0].Rows.get_Count(); fetchRowCount++)
		{
			stock_no_auth += ", '" + bcls_auth->Tables[0].Rows[fetchRowCount]["name"].ToString() + "' ";
		}
		delete bcls_auth;*/



		if (bcls_rec->Tables[0].Columns.Contains("STOCK_NO"))
			stock_no = bcls_rec->Tables[0].Rows[0]["STOCK_NO"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("LOADING_PLAN_NO"))
			loading_plan_no = bcls_rec->Tables[0].Rows[0]["LOADING_PLAN_NO"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("APP_DATE_FR"))
			app_date_fr = bcls_rec->Tables[0].Rows[0]["APP_DATE_FR"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("APP_DATE_TO"))
			app_date_to = bcls_rec->Tables[0].Rows[0]["APP_DATE_TO"].ToString();

		/* ***** 打印输入参数 ***** */
		Log::Trace("", __FUNCTION__, "stock_no\t[{0}]", stock_no);
		Log::Trace("", __FUNCTION__, "loading_plan_no\t[{0}]", loading_plan_no);
		Log::Trace("", __FUNCTION__, "app_date_fr\t[{0}]", app_date_fr);
		Log::Trace("", __FUNCTION__, "app_date_to\t[{0}]", app_date_to);

		if (stock_no.Trim() == "")
		{
			sprintf(s.msg, "请选择库区。");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (stock_no.Trim() != "")
		{
			sqlwhere += " AND A.stock_no = @stock_no";
		}
		if (loading_plan_no.Trim() != "")
		{
			sqlwhere += " AND A.loading_plan_no like @loading_plan_no ||'%' ";
		}
		if (app_date_fr.Trim() != "")
		{
			sqlwhere += " AND A.app_date >= substr(@app_date_fr,8)";
		}
		if (app_date_to.Trim() != "")
		{
			sqlwhere += " AND A.app_date <= substr(@app_date_to,8)";
			Log::Debug("", __FUNCTION__, "sqlwhere = [{0}]", sqlwhere);
		}

	/*	sqlwhere +=
			" AND A.STOCK_NO IN (" + stock_no_auth + ")";*/

		sqlstr1 = " SELECT A.*, B.VEHICLE_NUM AS VEHICLE_NUM1,B.TRUCK_NO,B.FLEET_NAME "
			" FROM TWM0C A  LEFT OUTER JOIN TWM0D B  ON  A.LOADING_PLAN_NO = B.LOADING_PLAN_NO AND B.ARCHIVE_FLAG != '1' "
			" WHERE A.PRO_FLAG != 'D' ";
		sqlorderby =
			" ORDER BY A.LOADING_PLAN_NO DESC ";

		sqlstr = sqlstr1 + sqlwhere + sqlorderby;
		Log::Debug("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stock_no", stock_no);
		cmd_inq.Parameters.Set("loading_plan_no", loading_plan_no);
		cmd_inq.Parameters.Set("app_date_fr", app_date_fr);
		cmd_inq.Parameters.Set("app_date_to", app_date_to);
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

