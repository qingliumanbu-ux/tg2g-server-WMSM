/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         JHZHAO
Version:		1.0
Date:			2016-03-05
Description:	行车属性配置信息查询
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件
//#include "twm06.h"


//函数申明

/*<remark>=========================================================
///<summary>
///库区定义信息查询
///<para>
///2.排序方式：CRANE_NO
///</para>
///<para>数据库表：TWM06 行车属性配置信息表；
///<returns>返回符合查询条件的行车属性配置信息</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wmsm02_show_inq);

int f_wmsm02_show_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	

	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */
	//CTWM06 twm06(conn);
	CModel twm06("TWM06");
	CModel twmm1("TWMM1");

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
	CDecimal cd_count	= 0;								
	int	record_count_per_page	= 0; /* 每页记录数 */
	int	current_page_no			= 0; /* 需查询的页号,从0开始计数 */
	int	start_row				= 0; /* 将要压入outBlock的起始行 */
    

	/* 全局变量 */
	CString mat_no = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq2(conn);
	

	//系统的分页类信息。
	CPageInfo pageInfo;

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim();	//材料号

		
		// Table0   =   全局点击查询 gridViewShow
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库

			
			sqlstr = "select * from tmmsm01 where mat_no = '" + mat_no + "' and mat_position in('1', '2')";

		

			break;
		}


		Log::Trace("", __FUNCTION__, "sqlstr				= [{0}]", (const char*)sqlstr);

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		
		cmd_inq.Close();
	

		bcls_ret->Tables.Add("TABLE_2");
		Log::Trace("", "", "222222");
		
		sql = "SELECT  a.mat_no from TWMM1 a left outer join(select * from tmmsm01 where mat_position in('1', '2')) b on a.mat_no = b.mat_no  "
			  "where REST_ROLLER_NO = (select  REST_ROLLER_NO from TWMM1 where mat_no = '" + mat_no + "'  )   ";
		Log::Info("", __FUNCTION__, "===sql==== [{0}]", sql);
		 
		
		Log::Trace("", "", "222222");
		
		cmd_inq2.SetCommandText(sql);
		cmd_inq2.ExecuteQuery(bcls_ret->Tables[1]);
		cmd_inq2.ExecuteReader();
		cmd_inq2.Close();

		Log::Trace("", "", "222222");
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