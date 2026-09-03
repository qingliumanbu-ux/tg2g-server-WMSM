/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         lizhen
Version:		1.0
Date:			2024-1-4
Description:	装车确认
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件

//函数申明

/*<remark>=========================================================
///<summary>
///装车实绩查询
///<para>
///2.排序方式：
///</para>
///<para>数据库表：TWMSM61 装车实绩表；
///<returns>返回符合查询条件的实绩信息</returns>
===========================================================</remark>*/
int f_mmsm99(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);


BM2F_ENTERACE(wmsmsm12p_conf);

int f_wmsmsm12p_conf(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */

	CModel twmsm12 = CModel("TWMSM12");
	CModel hwmsm12 = CModel("HWMSM12");
	CModel hmmsm01("HMMSM01");

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
	CModel tmmsm96 = CModel("TMMSM96");
	CModel tmmsm01 = CModel("TMMSM01");
	//CModel hmmsm01 = CModel("HMMSM01");

	//系统的分页类信息。

	/*装车任务号*/
	CString load_scheme_no("");
	//调用物料事件
	EIClass mm0099;
	mm0099.Tables[0].set_TableName("MM0099");
	mm0099.Tables[0].Columns.Add(tmmsm96);
	mm0099.Tables[0].Rows.Clear();
	try
	{
		load_scheme_no = bcls_rec->Tables[0].Rows[0]["LOAD_SCHEME_NO"].ToString();
		sqlstr = " select * from twmsm12 where LOAD_SCHEME_NO='"+ load_scheme_no +"' ";
		Log::Trace("", __FUNCTION__, "sqlstr				= [{0}]", (const char*)sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			cmd_inq.Fetch(twmsm12);
			hmmsm01.Reset();
			hmmsm01.CopyFrom(twmsm12);
			if (hmmsm01.Query("MAT_NO"))
			{
				hwmsm12.CopyFrom(twmsm12);
				hwmsm12.Insert();
				twmsm12.Delete("LOAD_SCHEME_NO,MAT_NO");
				hmmsm01["LOGISTICS_STATUS"] = "3";
				hmmsm01.Update("LOGISTICS_STATUS", "MAT_NO");
			}
			
		}
		cmd_inq.Close();
		
		if (mm0099.Tables[0].Rows.get_Count() > 0) {
			doFlag = f_mmsm99(&mm0099, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
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