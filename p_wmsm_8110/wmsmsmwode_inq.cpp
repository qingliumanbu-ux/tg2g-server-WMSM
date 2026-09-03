/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         JHZHAO
Version:		1.0
Date:			2016-03-05
Description:	xxx
**************************************************/

//框架头文件
#include "stdafx.h"

BM2F_ENTERACE(wmsmsmwode_inq);
int f_wmsmsmwode_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	

	/* 实体类定义 */
	CModel twmhq02 = CModel("TWMHQ02");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	//系统的分页类信息。
	CPageInfo pageInfo;

	try
	{
		//分页信息
		/*CDataTable& table = bcls_ret->Tables.Add("PAGEINFO");
		table.Columns.Add(DT_DECIMAL, "recordsum");*/
		
		//方法1：用CModel
		twmhq02.Reset();
		twmhq02["STOCK_NO"] = bcls_rec->Tables[0].Rows[0]["STOCK_NO"].ToString().Trim();
		twmhq02["STOCK_NO_CLASS"] = bcls_rec->Tables[0].Rows[0]["STOCK_NO_CLASS"].ToString().Trim();
		twmhq02["FACTORY_DIV"] = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().Trim();
		
		Log::Trace("", __FUNCTION__, "第[{0}]行，开始打印", __LINE__);
		Log::Trace("", __FUNCTION__, "STOCK_NO[{0}]", twmhq02["STOCK_NO"].ToString().Trim());
		Log::Trace("", __FUNCTION__, "STOCK_NO_CLASS[{0}]", twmhq02["STOCK_NO_CLASS"].ToString().Trim());

		if (twmhq02["STOCK_NO"].ToString().Trim() != "")
		{
			sqlwhere += "and stock_no = '" + twmhq02["STOCK_NO"].ToString().Trim() + "'";
		}
		if (twmhq02["STOCK_NO_CLASS"].ToString().Trim() != "")
		{
			sqlwhere += "and stock_no_class = '" + twmhq02["STOCK_NO_CLASS"].ToString().Trim() + "'";
		}
		if (twmhq02["FACTORY_DIV"].ToString().Trim() != "")
		{
			sqlwhere += "and factory_div = '" + twmhq02["FACTORY_DIV"].ToString().Trim() + "'";
		}

		sqlstr = "SELECT * FROM TWMHQ02 WHERE 1 = 1 ";

		sqlstr += sqlwhere;
		Log::Trace("", __FUNCTION__, "sqlwhere[{0}]", sqlwhere);
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

