/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         LIZHEN
Version:		1.0
Date:			2023-11-07
Description:	获取板坯数据
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

BM2F_ENTERACE(wmsmbp_f3);

int f_wmsmbp_f3(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
	CModel twmsma1("TWMSMA1");
	EPEX epex;

	/* 数据库SQL操作字符串 */
	CString sql = "";
	CString sqlstr = "";
	CString sqlwhere = "";
	CString sqlstr_count;
	CString sqlstr_temp;

	/* 业务变量 */
	CString s_tc_no = "T8P305";
	/* 全局变量 */

	CDbCommand cmd_inq(conn);

	//系统的分页类信息。


	try
	{
		sql = " DELETE FROM TWMSMA1 WHERE 1=1 ";
		cmd_inq.SetCommandText(sql);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//初始化
		ret = epex.Initialize(s_tc_no);
		if (ret < 0)
		{
			CFormattable arguments[] = { s_tc_no }; // 定义参数列表的数组
			CMessageFormat::Format(s.msg, _RES("YM00S0000514")/*初始化电文[{0}]失败。*/, arguments, 1);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		//拼电文数据
		if (epex.SetValue("MES_MM_GM_RFC", "SlabID", 0, "1") < 0)
		{
			strcpy(s.msg, _RES("GCRSS0000015"));//系统出现异常，电文拼接出错，请联系系统维护人员。
			throw CApplicationException(-1, s.msg, s.svc_name);
		}





		if (epex.SendTele() < 0)
		{
			strcpy(s.msg, _RES("GCRSS0000032")/*电文发送失败。*/);
			sprintf(s.sysmsg, "[%s]发送失败", (const char*)s_tc_no);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}


		epex.Uninitialize();


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