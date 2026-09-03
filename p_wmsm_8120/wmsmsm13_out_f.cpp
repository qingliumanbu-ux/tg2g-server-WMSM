/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         JHZHAO
Version:		1.0
Date:			2016-02-01
Description:	直供出坯确认材料出库结束功能
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件


//业务头文件


//函数申明
BM2_FUNCTION_IMPORT

int f_wmsmsm13_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
/*<remark>=========================================================
///<summary>
///直供出坯确认材料出库结束功能
///<para>
///</para>
///<para>数据库表：TMMSM01坯料主档表 
///<returns>更新主档材料信息</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wmsmsm13_out_f);

int f_wmsmsm13_out_f(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;


	/* 数据库SQL操作字符串 */
	CString sqlstr = "";

	try
	{

		if (!bcls_rec->Tables[0].Columns.Contains("PROC_DIV"))
		{
			bcls_rec->Tables[0].Columns.Add(DT_STRING, "PROC_DIV");
		}
		bcls_rec->Tables[0].Rows[0]["PROC_DIV"] = "END";
		doFlag = f_wmsmsm13_proc(bcls_rec, bcls_ret, conn);
		if (doFlag != 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
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