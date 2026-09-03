/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         JHZHAO
Version:		1.0
Date:			2016-03-05
Description:	xxx
**************************************************/

//框架头文件
#include "stdafx.h"

BM2F_ENTERACE(wmsmsmwode_inq2);
int f_wmsmsmwode_inq2(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	

	/* 实体类定义 */
	CModel twmhq01 = CModel("TWMHQ01");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	//系统的分页类信息。
	CPageInfo pageInfo;

	try
	{
		//1)获取分页信息
		//if (bcls_rec->Tables.Contains("PageInfo"))
		//{
		//	pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		//}
		//else
		//{
		//	pageInfo.RecordFrom = 0;
		//	pageInfo.PageSize = -1;  //每页记录数量
		//}

		//Log::Trace("", __FUNCTION__, "pageInfo.RecordFrom[{0}]pageInfo.PageSize[{1}]", pageInfo.RecordFrom, pageInfo.PageSize);


		//方法1：
		//CString stock_no = "";
		//if (bcls_rec->Tables[0].Columns.Contains("STOCK_NO")) //bcls_rec传入块的第0张表 是否 包含第0张表的STOCK_NO
		//{
		//	stock_no = bcls_rec->Tables[0].Rows[0]["STOCK_NO"].ToString().Trim();	// 从传入块里面找第0张表的第“STOCK_NO”列的第1行
		//}
		//
		//Log::Trace("", __FUNCTION__, "[{1}]  stock_no[{0}] ", stock_no,__LINE__);
		//if (stock_no != "")
		//{
		//	sqlwhere += " and stock_no = '" + stock_no + "'";
		//}

		//CString stock_no_class = "";
		//if (bcls_rec->Tables[0].Columns.Contains("STOCK_NO_CLASS"))
		//{
		//	stock_no_class = bcls_rec->Tables[0].Rows[0]["STOCK_NO_CLASS"].ToString().Trim();
		//}

		//Log::Trace("", __FUNCTION__, "[{1}] stock_no_class[{0}] ", stock_no_class,__LINE__);
		//if (stock_no_class != "")
		//{
		//	sqlwhere += " and stock_no_class = '" + stock_no_class + "'";
		//}

		//sqlstr = "SELECT * FROM TWMHQ01 WHERE 1=1";
		//sqlstr = sqlstr + sqlwhere;

		//Log::Trace("", __FUNCTION__, "sqlstr[{0}]", sqlstr);
		//cmd_inq.SetCommandText(sqlstr);
		//cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		////cmd_inq.ExecuteQuery(bcls_ret->Tables[0], pageInfo.RecordFrom, pageInfo.PageSize);
		//cmd_inq.Close();

		//方法2
		twmhq01.Reset();  //重置只会把数据清除掉，列名还在
		//twmhq01.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		twmhq01["STOCK_NO"] = bcls_rec->Tables[0].Rows[0]["STOCK_NO"].ToString().Trim();
		twmhq01["STOCK_DESC"] = bcls_rec->Tables[0].Rows[0]["STOCK_DESC"].ToString().Trim();
		
		twmhq01.Print();
		if (twmhq01["STOCK_NO"].ToString().Trim() != "")
		{
			sqlwhere += "and stock_no = '" + twmhq01["STOCK_NO"].ToString().Trim() + "'";
		}
		
		
		sqlstr = "SELECT T.* FROM TWMHQ01 T WHERE 1=1 ";

		sqlstr += sqlwhere;
		Log::Trace("", __FUNCTION__, "sqlstr[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();





		//给第二个GRID查数据
		sqlstr = "";
		sqlwhere = "";
		bcls_ret->Tables.Add("XX");
		if (twmhq01["STOCK_NO"].ToString().Trim() != "")
		{
			sqlwhere += "and STOCK_NO = '" + twmhq01["STOCK_NO"].ToString().Trim() + "'";
		}
		if (twmhq01["STOCK_DESC"].ToString().Trim() != "")
		{
			sqlwhere += "and STOCK_DESC = '" + twmhq01["STOCK_DESC"].ToString().Trim() + "'";
		}
		sqlstr = "SELECT T.* FROM TWMHQ02 T WHERE 1=1 ";

		sqlstr += sqlwhere;
		Log::Trace("", __FUNCTION__, "sqlstr[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[1]);
		cmd_inq.Close();



		//cmd_inq.ExecuteReader();
		//bcls_ret->Tables[0].Columns.Add(twmhq01);
		//bcls_ret->Tables[0].Columns.Add(twmhq01);
		
		//bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "recordsum");
		/*while (cmd_inq.Read())
		{
			twmhq01.Reset();
			cmd_inq.Fetch(twmhq01);

			twmhq01.Print();
			CDataRow& row = bcls_ret->Tables[0].Rows.Add();

			row.Merge(twmhq01);
			Log::Trace("", __FUNCTION__, "开始打印，第[{0}]",__LINE__ );
			Log::Trace("", __FUNCTION__, "打印行[{0}]的值", row["STOCK_NO"]);

		}*/
		//Log::Trace("", __FUNCTION__, "结束打印返回块[{0}]的值",);
		
		

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

