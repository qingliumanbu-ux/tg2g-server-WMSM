/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         LIZHEN
Version:		1.0
Date:			2023-11-07
Description:	辊道退料302
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"
//程序用头文件



//函数申明

/*<remark>=========================================================
//1、删除退料队列
//2、向制造发送调拨申请
===========================================================</remark>*/

BM2F_ENTERACE(wmsmgdtl_f4);

int f_wmsmgdtl_f4(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	int ret = 0;

	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */
	//CTWM06 twm06(conn);
	CModel twmsma0("TWMSMA0");
	CModel tmmsm01("TMMSM01");
	CModel tmmsm96("TMMSM96");
	CModel twmsm13("TWMSM13");
	EPEX epex;

	/* 数据库SQL操作字符串 */
	CString sql = "";
	CString sqlstr = "";
	CString sqlwhere = "";
	CString sqlstr_count;
	CString sqlstr_temp;
	CString v_datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString v_shift_no(" ");
	CString v_shift_group(" ");

	/* 业务变量 */
	
	/* 全局变量 */

	CDbCommand cmd_inq(conn);

	//系统的分页类信息。
	EIClass inblock;
	inblock.Tables[0].Columns.Add(tmmsm01);
	inblock.Tables[0].Rows.Clear();


	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			twmsma0.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			twmsma0.Delete("MAT_NO");
			
		}
		
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