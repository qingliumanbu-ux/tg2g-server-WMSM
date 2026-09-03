/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         lizhen
Version:		1.0
Date:			2024-4-1
Description:	碳钢热送率
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件

//函数申明

/*<remark>=========================================================
///<summary>
///卸车计划查询
///<para>
///2.排序方式：
///</para>
///<para>数据库表：TWMSM62 倒运计划表；
///<returns>返回符合查询条件的计划信息</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wmsmsc_inq1);

int f_wmsmsc_inq1(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */


	CString sqlstr = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_con(conn);


	//系统的分页类信息。
	CPageInfo pageInfo;

	/* 业务变量 */


	try {
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "STRAND_NO1");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "MAT_NO1");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "MEND_FLAG1");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "MAT_LEN1");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "MAT_WT1");

		bcls_ret->Tables[0].Columns.Add(DT_STRING, "STRAND_NO2");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "MAT_NO2");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "MEND_FLAG2");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "MAT_LEN2");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "MAT_WT2");

		bcls_ret->Tables[0].Columns.Add(DT_STRING, "STRAND_NO3");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "MAT_NO3");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "MEND_FLAG3");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "MAT_LEN3");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "MAT_WT3");

		EIClass temp;
		CString mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Replace(",", "','");
		sqlstr = " SELECT STRAND_NO, case when MAT_NO like '1A%' then SUBSTR2(MAT_NO, 10) else SUBSTR2(MAT_NO, 9) end MAT_NO, decode(MEND_FLAG,'1','X',' ')MEND_FLAG, round(MAT_LEN/1000,3) MAT_LEN, MAT_WT\
			FROM VMMSM01\
			WHERE MAT_NO IN('"+ mat_no +"')\
			ORDER BY STRAND_NO,MAT_NO ";
		Log::Trace("", __FUNCTION__, "sqlstr				= [{0}]", (const char*)sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(temp.Tables[0]);
		cmd_inq.Close();
		int max_row = 0;
		int col_cou = 1;
		Log::Trace("", __FUNCTION__, "temp.Tables[0].Rows.get_Count()				= [{0}]", temp.Tables[0].Rows.get_Count());
		for (int i = 0; i < temp.Tables[0].Rows.get_Count(); i++)
		{
			Log::Trace("", __FUNCTION__, "i			= [{0}]", i);
			Log::Trace("", __FUNCTION__, "temp.Tables[0].Rows[i][MAT_NO]				= [{0}]", temp.Tables[0].Rows[i]["MAT_NO"].ToString());
			if (i==0)
			{
				bcls_ret->Tables[0].Rows.Add();
				bcls_ret->Tables[0].Rows[0]["STRAND_NO"+ CConvert::ToString(col_cou)] = temp.Tables[0].Rows[i]["STRAND_NO"];
				bcls_ret->Tables[0].Rows[0]["MAT_NO" + CConvert::ToString(col_cou)] = temp.Tables[0].Rows[i]["MAT_NO"].ToString();
				bcls_ret->Tables[0].Rows[0]["MEND_FLAG" + CConvert::ToString(col_cou)] = temp.Tables[0].Rows[i]["MEND_FLAG"];
				bcls_ret->Tables[0].Rows[0]["MAT_LEN" + CConvert::ToString(col_cou)] = temp.Tables[0].Rows[i]["MAT_LEN"];
				bcls_ret->Tables[0].Rows[0]["MAT_WT" + CConvert::ToString(col_cou)] = temp.Tables[0].Rows[i]["MAT_WT"];
				max_row ++;
			}
			else 
			{
				if (temp.Tables[0].Rows[i]["STRAND_NO"].ToString() == temp.Tables[0].Rows[i - 1]["STRAND_NO"].ToString())
				{
					if (max_row>= bcls_ret->Tables[0].Rows.get_Count())
					{
						bcls_ret->Tables[0].Rows.Add();
					}
				
					bcls_ret->Tables[0].Rows[max_row]["STRAND_NO" + CConvert::ToString(col_cou)] = temp.Tables[0].Rows[i]["STRAND_NO"];
					bcls_ret->Tables[0].Rows[max_row]["MAT_NO" + CConvert::ToString(col_cou)] = temp.Tables[0].Rows[i]["MAT_NO"].ToString();
					bcls_ret->Tables[0].Rows[max_row]["MEND_FLAG" + CConvert::ToString(col_cou)] = temp.Tables[0].Rows[i]["MEND_FLAG"];
					bcls_ret->Tables[0].Rows[max_row]["MAT_LEN" + CConvert::ToString(col_cou)] = temp.Tables[0].Rows[i]["MAT_LEN"];
					bcls_ret->Tables[0].Rows[max_row]["MAT_WT" + CConvert::ToString(col_cou)] = temp.Tables[0].Rows[i]["MAT_WT"];
					max_row++;
				}
				else
				{
					col_cou++;
					max_row = 0;
					
					bcls_ret->Tables[0].Rows[max_row]["STRAND_NO" + CConvert::ToString(col_cou)] = temp.Tables[0].Rows[i]["STRAND_NO"];
					bcls_ret->Tables[0].Rows[max_row]["MAT_NO" + CConvert::ToString(col_cou)] = temp.Tables[0].Rows[i]["MAT_NO"].ToString();
					bcls_ret->Tables[0].Rows[max_row]["MEND_FLAG" + CConvert::ToString(col_cou)] = temp.Tables[0].Rows[i]["MEND_FLAG"];
					bcls_ret->Tables[0].Rows[max_row]["MAT_LEN" + CConvert::ToString(col_cou)] = temp.Tables[0].Rows[i]["MAT_LEN"];
					bcls_ret->Tables[0].Rows[max_row]["MAT_WT" + CConvert::ToString(col_cou)] = temp.Tables[0].Rows[i]["MAT_WT"];
					max_row++;
				}
			}
			Log::Trace("", __FUNCTION__, "		bcls_ret		= [{0}]", bcls_ret->Tables[0].Rows.get_Count());
		}
	
		for (int i = 0; i < bcls_ret->Tables[0].Rows.get_Count(); i++)
		{
			if (i > 0)
			{
				if (bcls_ret->Tables[0].Rows[i]["STRAND_NO1"].ToString() != bcls_ret->Tables[0].Rows[i - 1]["STRAND_NO1"].ToString())
				{
					bcls_ret->Tables[0].Rows[i]["STRAND_NO1"] = bcls_ret->Tables[0].Rows[i - 1]["STRAND_NO1"].ToString();
				}
				if (bcls_ret->Tables[0].Rows[i]["STRAND_NO2"].ToString() != bcls_ret->Tables[0].Rows[i - 1]["STRAND_NO2"].ToString())
				{
					bcls_ret->Tables[0].Rows[i]["STRAND_NO2"] = bcls_ret->Tables[0].Rows[i - 1]["STRAND_NO2"].ToString();
				}
				if (bcls_ret->Tables[0].Rows[i]["STRAND_NO3"].ToString() != bcls_ret->Tables[0].Rows[i - 1]["STRAND_NO3"].ToString())
				{
					bcls_ret->Tables[0].Rows[i]["STRAND_NO3"] = bcls_ret->Tables[0].Rows[i - 1]["STRAND_NO3"].ToString();
				}
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