/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      lizhen
Version:     1.0
Date:        2023-11-08
Description:
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"

/*<remark>=========================================================
/// <summary>
/// 物料数据同步
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件




BM2F_ENTERACE_TELE(cm_002162_rcv)

int f_cm_002162_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	int blkNum_pmol02 = 0;
	/* 业务变量 */
	CString	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	/* 业务变量 */

	/* 实体类定义 */
	CModel tmm0097("TMM0097");
	CModel tmmsm96("TMMSM96");
	CModel tmmsm01("TMMSM01");
	CModel hmmsm01("HMMSM01");
	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CDbCommand cmd_sql(conn);
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_code(conn);




	try
	{
		tmmsm01.MergeFrom(bcls_rec->Tables["STAR"].Rows[0]);
		hmmsm01.MergeFrom(bcls_rec->Tables["STAR"].Rows[0]);
		tmmsm01["TRANSFER_FLAG"] = bcls_rec->Tables["STAR"].Rows[0]["STATE"].ToString();
		hmmsm01["TRANSFER_FLAG"] = bcls_rec->Tables["STAR"].Rows[0]["STATE"].ToString();
		
		tmmsm01.Update("TRANSFER_FLAG", "MAT_NO");
		hmmsm01.Update("TRANSFER_FLAG", "MAT_NO");
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		//返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		//数据库异常时返回-1，事务将被回滚
		doFlag = -1;
	}
	//捕获应用错误
	catch (CApplicationException& ex)
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
	//返回-1时事务将回滚，返回为0是事务将提交	return doFlag;
}


