/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:    KE2111
Version:    1.0
Date:     2023-11-14
Description: 字典查询
**************************************************/
//框架公用头文件，勿删
#include "stdafx.h"


// service入口
BM2F_ENTERACE(wmsmzd_inq)

int f_wmsmzd_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString picture_no = "";//画面号
	CString fn_no = "";//功能键号
	CDbCommand cmd(conn);
	CDbCommand cmd_inq(conn);
	CString sql = "";
	CString v_code;
	CString c_code;
	/* ***** 静态变量定义 ***** */
	int doFlag = 0;
	int fetchRowCount = 0;
	int RowCount = 0;
	int recordFrom = 0;
	int pageSize = -1;		// 查询所有记录
	int totalCount = 0;

	try
	{
		sql = "SELECT CODE_DESC_2_CONTENT FROM TWMSMZD02 WHERE  CODE_CLASS like '%YH' and CODE_DESC_1_CONTENT=@userid ";
		Log::Trace("", "", "sql", sql, s.userid);
		cmd.SetCommandText(sql);
		cmd.Parameters.Set("userid", s.userid);
		cmd.ExecuteReader();
		while (cmd.Read())
		{
			c_code = cmd.GetString(1).Trim();
			v_code = v_code + "," + "'" + c_code + "'";
		}
		cmd.Close();
		
		Log::Trace("", __FUNCTION__, "scode = [{0}]", v_code);
		if (c_code.GetLength() > 0)
		{
			v_code = v_code.Substring(1, v_code.GetLength() - 1);
			sqlstr = " select * from TWMSMZD01 where 1=1  and CODE_CLASS in (" + v_code + ") ";
			Log::Trace("", __FUNCTION__, "sqlstr= [{0}]", sqlstr);
		}
		else
		{
			sqlstr = " select * from TWMSMZD01 where 1=1 ";
			if (bcls_rec->Tables[0].Rows[0]["CODE_CLASS"].ToString().Trim() != "") {
				sqlstr += " and CODE_CLASS LIKE '" + bcls_rec->Tables[0].Rows[0]["CODE_CLASS"].ToString() + "%' ";
			}
			if (bcls_rec->Tables[0].Rows[0]["CODE_DESC"].ToString().Trim() != "") {
				sqlstr += " and CODE_DESC LIKE '%" + bcls_rec->Tables[0].Rows[0]["CODE_DESC"].ToString() + "%' ";
			}
		}
		sqlstr += " order by CODE_CLASS   ";
		cmd_inq.SetCommandText(sqlstr);
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

	cmd_inq.Close();

	return doFlag;

}