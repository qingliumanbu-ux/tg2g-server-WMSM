/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:
Version:		1.0
Date:			2016-03-05
Description:	来料拒收
**************************************************/

//框架头文件
#include "stdafx.h"

BM2_FUNCTION_IMPORT

/*<remark>=========================================================
///<summary>
///板坯入库功能
///<para>
///2.排序方式：
///</para>
///<para>数据库表：TWMA0 倒躲队列；TWMA1 物料主档表
///<returns>执行预材料预入库功能</returns> 
===========================================================</remark>*/

BM2F_ENTERACE(wmsmsma2mg_f6);

int f_wmsmsma2mg_f6(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CDecimal rowCount = 0;

	/*程序用变量*/
	CString mat_no = "";
	CString event_id = "";
	CString slab_place_code = "";
	CString slat_unlade_cause = "";
	CString surface_decide_code = "";
	/* 实体类定义 */
	//CTWM01 twm01(conn);
	//CTWMA0 twma0(conn);
	//CTWMA1 twma1(conn);
	//CTWMA2 twma2(conn);
	CModel twmj1 = CModel("TWMJ1");
	

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
	
			twmj1["MAT_NO"] = bcls_rec->Tables[0].Rows[i]["mat_no"].ToString();
			if (twmj1.QueryCount("MAT_NO") > 0)
			{
				twmj1.Delete("MAT_NO");
			}

			twmj1["EVENT_MAKER"] = s.userid;
			//twmj1["EVENT_MARK"] = "3";
			twmj1["PRI_GRADE"] = "2";
			twmj1["SHIFT_GROUP"] = bcls_rec->Tables[1].Rows[0]["SHIFT_GROUP"].ToString();
			twmj1["END_TIME_EVENT"] = bcls_rec->Tables[1].Rows[0]["END_TIME_EVENT"].ToString();
			//twmj1["EVENT_MAKER"] = bcls_rec->Tables[1].Rows[0]["EVENT_SORT_FLAG"].ToString();
			twmj1["REMARK"] = bcls_rec->Tables[1].Rows[0]["REMARK"].ToString();

			Log::Trace("", __FUNCTION__, "参数赋值mat_no：\t[{0}]", twmj1["MAT_NO"].ToString());
		
			
			twmj1.Insert();
			
			

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
