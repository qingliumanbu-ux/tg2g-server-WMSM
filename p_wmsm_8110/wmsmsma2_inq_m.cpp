/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         JHZHAO
Version:		1.0
Date:			2016-03-05
Description:	板坯入库队列信息查询
**************************************************/

//框架头文件
#include "stdafx.h"

//程序用头文件
//#include "twma0.h"
//#include "twma1.h"
//#include "twm00.h"


//函数申明

/*<remark>=========================================================
///<summary>
///板坯入库队列信息查询
///<para>
///2.排序方式：队列写入时间
///</para>
///<para>数据库表：TWMA0 倒躲队列；TWMA1 物料主档表
///<returns>返回符合查询条件的队列信息</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wmsmsma2_inq_m);

int f_wmsmsma2_inq_m(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString s_userid("");
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString stock_no = "";
	CString stock_oper_order = "";
	CString heat_no = "";
	CString pono = "";
	CString order_no = "";
	CString mat_no = "";
	CString mat_line_type = " ";
	CString mat_kind = " ";
	CString old_stock_no = "";
	CString st_no = "";
	CDecimal d_from_len = 0, d_to_len = 99999, d_from_width = 0, d_to_width = 99999;
	CDecimal d_thick_fr = 0;
	CDecimal d_thick_to = 0;

	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */
	//CTWMA0 twma0(conn);
	//CTWMA1 twma1_q(conn);
	//CTWMA1 twma1(conn);
	//CTWMA1 twma1_1(conn);
	CModel twma0 = CModel("TWMA0");
	CModel twma1 = CModel("TMMSM01");


	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";
	CString sqlgroup = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);

	//系统的分页类信息。
	CPageInfo pageInfo;

	//返回数据信息
	bcls_ret->Tables[0].Columns.Add(twma0);
	bcls_ret->Tables[0].Columns.Add(twma1);

	try
	{
		

		sqlstr =
			" SELECT T2.*,CASE "
			" WHEN T2.SLAB_CUT_TIME != ' ' THEN CEIL( "
			"	(SYSDATE - TO_DATE(T2.SLAB_CUT_TIME, 'yyyy-mm-dd hh24-mi-ss')) * 24) end TIME_SPAN_HOUR, "
			" CASE "
			" WHEN (T2.SLAB_CUT_TIME) != ' ' AND CEIL( "
			" 	(SYSDATE - TO_DATE((T2.SLAB_CUT_TIME), 'yyyy-mm-dd hh24-mi-ss')) * "
			" 	24) > 72 THEN 1 "
			" ELSE 0 end                                                                                ZHILIU_FLAG "
			" FROM TMMSM01 T2 WHERE 1=1"
			" AND EXISTS(SELECT NULL FROM TWMA2 T1"
			" WHERE T1.MAT_NO = T2.MAT_NO)"
			;
		for (int i = 0; i < bcls_rec->Tables[0].Columns.get_Count(); i++)
		{
			if (bcls_rec->Tables[0].Columns[i].get_DataType() == DT_STRING
				&& bcls_rec->Tables[0].Rows[0][i].ToString().Trim().IsEmpty())
			{
				continue;
			}
			if (bcls_rec->Tables[0].Columns[i].get_DataType() == DT_DECIMAL
				&& bcls_rec->Tables[0].Rows[0][i].ToDecimal() == 0)
			{
				continue;
			}
			if (bcls_rec->Tables[0].Columns[i].get_ColumnName() == "SLAB_CUT_TIME"
				|| bcls_rec->Tables[0].Columns[i].get_ColumnName() == "TIME_SPAN_HOUR"
				|| bcls_rec->Tables[0].Columns[i].get_ColumnName() == "MAT_WT"
				|| bcls_rec->Tables[0].Columns[i].get_ColumnName() == "MAT_NUM")
			{
				continue;
			}
			sqlstr += " AND " + bcls_rec->Tables[0].Columns[i].get_ColumnName() + " LIKE @" + bcls_rec->Tables[0].Columns[i].get_ColumnName() + "||'%' ";
			cmd_inq.Parameters.Set(bcls_rec->Tables[0].Columns[i].get_ColumnName(), bcls_rec->Tables[0].Rows[0][i].ToString().Trim());
			Log::Trace("", __FUNCTION__, "b		= [{0}][{1}]", bcls_rec->Tables[0].Columns[i].get_ColumnName(), bcls_rec->Tables[0].Rows[0][i].ToString().Trim());
		}
		Log::Trace("", __FUNCTION__, "sqlstr				= [{0}]", (const char*)sqlstr);
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

