/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         JINQUAN
Version:		1.0
Date:			2016-07-27
Description:	吊车命令卸下
**************************************************/

//框架头文件
#include "stdafx.h"

//程序用头文件

//函数申明
BM2_FUNCTION_IMPORT
int f_wmsmsm_crane_up(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

/*<remark>=========================================================
///<summary>
///吊车命令卸下
///<para>
///2.排序方式：
///</para>
///<para>
===========================================================</remark>*/

BM2F_ENTERACE(wmsmsma7_up);

int f_wmsmsma7_up(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	/* 程序内部变量 */
	int	doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString sqlstr = "";
	CString in_flag = "";     //等于1为在库内
	CDecimal  cou = 0;


	//定义表实体对象
	//CTWMA0 twma0(conn);
	//CHWMA0 hwma0(conn);
	//CTWMA1 twma1(conn);
	//CTWMA2 twma2(conn);
	//CTWMA7 twma7(conn);
	//CTWMA7 twma7_hmi(conn);
	//CTWM04 twm04(conn);
	//CHWM00A7 hwm00a7(conn);

	CModel twma7 = CModel("TWMA7");
	CModel twma7_hmi = CModel("TWMA7");

	CDbCommand cmd_inq(conn);

	EIClass bcls_up;
	bcls_up.Tables[0].set_TableName("WM00_UP");
	bcls_up.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
	bcls_up.Tables["WM00_UP"].Rows.Add();

	try
	{
		//传入参数检核
		if (bcls_rec->Tables[0].Rows.get_Count() == 0)
		{
			sprintf(s.msg, "No incoming data.");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{		
			
			twma7_hmi.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			if (twma7_hmi["STOCK_PLACE_NO_TO"].ToString().Trim() == "")
			{
				sprintf(s.msg, "Target position cannot be empty!");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			twma7["MAT_NO"] = twma7_hmi["MAT_NO"];

			if (!twma7.QueryCount("MAT_NO"))
			{
				sprintf(s.msg, "[%s] instruction doesn't exist!", (const char*)twma7_hmi["MAT_NO"]);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			
			bcls_up.Tables["WM00_UP"].Rows[0]["MAT_NO"] = twma7_hmi["MAT_NO"];

			doFlag = f_wmsmsm_crane_up(&bcls_up, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
			
			/*设置系统返回参数*/
			Log::Trace("", __FUNCTION__, "end");
		}
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "Database processing error，sqlcode = [{0}]."/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);

		//strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
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