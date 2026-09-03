/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         LIZHEN
Version:		1.0
Date:			2023-11-07
Description:	临钢坯302
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件



//函数申明

/*<remark>=========================================================

===========================================================</remark>*/

BM2F_ENTERACE(wmsmlgpb_inq);

int f_wmsmlgpb_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;


	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */

	CModel twmsma0("TWMSMA0");

	/* 数据库SQL操作字符串 */
	CString sql = "";
	CString sqlstr = "";
	CString sqlwhere = "";
	CString sqlstr_count;
	CString sqlstr_temp;

	/* 业务变量 */
	CString	datetime("");
	CString	cs_ladle_no("");
	CString	cs_sm_unit_no("");
	CDecimal cd_count = 0;
	int	record_count_per_page = 0; /* 每页记录数 */
	int	current_page_no = 0; /* 需查询的页号,从0开始计数 */
	int	start_row = 0; /* 将要压入outBlock的起始行 */


	/* 全局变量 */
	CString crane_no = "";
	CString stock_place_no_from = "";
	CString stock_place_no_to = "";
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	//系统的分页类信息。
	CPageInfo pageInfo;

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		sqlstr = " SELECT * FROM HMMSM01 T01 LEFT JOIN TWMSMZD02 T02 ON T01.GUIDE_DEST=T02.CODE WHERE T02.CODE_DESC_3_CONTENT LIKE '%4%' AND CODE_CLASS='WM02' AND C_DELIVERY_FAC='6380' ";
		/*获取查询条件传入列数*/
		int count_row = bcls_rec->Tables[0].Columns.get_Count();

		for (int i = 0; i < count_row; i++)
		{

			/*如果查询条件的值为空则跳出*/
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
			
			Log::Trace("", "", "条件查询sql {0}++[{1}],", i, bcls_rec->Tables[0].Columns[i].get_ColumnName() + ":" + bcls_rec->Tables[0].Rows[0][i].ToString().Trim());

			sqlwhere += " AND " + bcls_rec->Tables[0].Columns[i].get_ColumnName() + " LIKE @" + bcls_rec->Tables[0].Columns[i].get_ColumnName() + "||'%'";
			cmd_inq.Parameters.Set(bcls_rec->Tables[0].Columns[i].get_ColumnName(), bcls_rec->Tables[0].Rows[0][i].ToString().Trim());

		}
		/*连接sql语句*/
		sqlstr += sqlwhere;
		Log::Trace("", __FUNCTION__, "sqlstr[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();

		
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。" /* _RES("GCRSS0000006")*//*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
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