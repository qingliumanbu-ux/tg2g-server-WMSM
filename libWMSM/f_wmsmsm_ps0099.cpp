/* **************************************************************************
*	Copyright (c) Baosight Corporation 2008 . All Rights Reserved.
*  	BM2PES 宝信生产执行系统
*****************************************************************************
*  程序名称			: f_wm00_pm0099
*  程序描述			: PM生产同步电文处理
*  备注说明			:
*  修改历史			:
*  		henno 2016-09-28			(ADD)程序建立
*			... ...
* **************************************************************************** */
/* C/C++ 的标准头文件部分 */
#include "stdafx.h"		// 框架头，不可删除 
//#include "twm04.h" 
//#include "twma0.h"
//#include "twma1.h"
//#include "twma2.h"
//#include "twm01.h"




//外部函数声明

#if defined (_SYS_MES) || defined (_SYS_PES)

#if defined _LINE_HP && defined _LINE_SM
int f_pshp_dhcr_del_sm(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);
#endif

#else

#endif

BM2_FUNCTION_EXPORT
int f_wmsmsm_ps0099(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	/* ***** 程序变量 ***** */
	int doFlag = 0;
	CString v_datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	int i = 0;

	/* ***** 数据库SQL操作字符串 ***** */
	CString	sqlstr = "";

	/* ***** 数据库操作类定义 ***** */
	CDbCommand comm(conn);
	CDbCommand comm1(conn);

	/* ***** 定义表实体对象 ***** */
	CModel twma0 = CModel("TWMA0");


	EIClass inBlock;
	EIClass outBlock;
	inBlock.Tables[0].set_TableName("PSHP");
	inBlock.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
	inBlock.Tables[0].Rows.Clear();
	inBlock.Tables[0].Rows.Add();


	/* ***** 应用程序开始处理 ***** */
	try
	{
		if (!bcls_rec->Tables.Contains("WMPS99"))
		{
			sprintf(s.msg, "函数f_wm00_ps0099中找不到接收块名[WMPS99]");
			throw CApplicationException(-1, s.msg, log.Location);
		}

#if defined (_SYS_MES) || defined (_SYS_PES)
		for (int iRow = 0; iRow < bcls_rec->Tables["WMPS99"].Rows.get_Count(); iRow++)
		{
			twma0.Reset();
			twma0.MergeFrom(bcls_rec->Tables["WMPS99"].Rows[iRow]);

			Log::Trace("", __FUNCTION__, "twma0.MAT_NO\t[{0}]", twma0["MAT_NO"].ToString());
			Log::Trace("", __FUNCTION__, "twma0.STOCK_OPER_ORDER\t[{0}]", twma0["STOCK_OPER_ORDER"].ToString());
			Log::Trace("", __FUNCTION__, "twma0.STOCK_OPER_ORDER_DIV\t[{0}]", twma0["STOCK_OPER_ORDER_DIV"].ToString());


#if defined _LINE_HP && defined _LINE_SM

			if (twma0["STOCK_OPER_ORDER"].ToString().SubstringNE(0, 1) == "1" &&
				twma0["STOCK_OPER_ORDER_DIV"].ToString() == "1")
			{
				inBlock.Tables["PSHP"].Rows[0]["MAT_NO"] = twma0["MAT_NO"].ToString();
				doFlag = f_pshp_dhcr_del_sm(&inBlock, &outBlock, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
#endif

		}
#else


#endif

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };

		/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006"), arguments, 1);
		CString str = ex.GetMsg() + "\r\n" + sqlstr;

		/*返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应*/
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);

		/*数据库异常时返回-1，事务将被回滚*/
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), 399);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), 399);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}


