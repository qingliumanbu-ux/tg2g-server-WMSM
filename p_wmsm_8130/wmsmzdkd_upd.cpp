/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         KE2111
Version:		1.0
Date:			2023-12-1
Description:	指导宽度修改
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件

//函数申明



BM2F_ENTERACE(wmsmzdkd_upd);

int f_wmsmzdkd_upd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */

	CModel twmsmpz = CModel("TWMSMPZ");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlstr1 = "";
	CString sqlwhere = "";
	CString s_userid("");
	CString sqlstr_count;
	CString sqlstr_temp;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_con(conn);



	/* 业务变量 */



	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++) {
			twmsmpz.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			twmsmpz["CODE_CLASS"] = "ZDLK";
			twmsmpz.Update("BACK_N_1", "CODE_CLASS,CODE_DESC_1_CONTENT");
			
		}
		

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "Database processing error，sqlcode = [{0}]." /* _RES("GCRSS0000006")*//*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
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