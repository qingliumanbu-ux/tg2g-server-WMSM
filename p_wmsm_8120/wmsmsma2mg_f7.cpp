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
int f_mmsm99(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
/*<remark>=========================================================
///<summary>
///板坯入库功能
///<para>
///2.排序方式：
///</para>
///<para>数据库表：TWMA0 倒躲队列；TWMA1 物料主档表
///<returns>执行预材料预入库功能</returns> 
===========================================================</remark>*/

BM2F_ENTERACE(wmsmsma2mg_f7);

int f_wmsmsma2mg_f7(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
		EIClass bcls_rec_QM17;//材料质量封锁
		bcls_rec_QM17.Tables[0].set_TableName("MM0099");
		bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "EVENT_ID");
		bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");
		bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "SYSTEM_ID");
		bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "FUNC_ID");
		bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
		//bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "HOLD_REMARK");
		bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "REL_REMARK");
		bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "REL_MAKER");
		bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "REL_TIME");
		bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "HOLD_CAUSE_CODE");
		bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "SCRAP_CAUSE_CODE");
		

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{

			
			bcls_rec_QM17.Tables[0].Rows.Clear();
			bcls_rec_QM17.Tables[0].Rows.Add();
			bcls_rec_QM17.Tables[0].Rows[0]["EVENT_ID"] = "QM05";
			bcls_rec_QM17.Tables[0].Rows[0]["EVENT_LINE_TYPE"] = "00";
			bcls_rec_QM17.Tables[0].Rows[0]["SYSTEM_ID"] = "MMSM";
			bcls_rec_QM17.Tables[0].Rows[0]["FUNC_ID"] = "wmsmsma2mg_f7";
			bcls_rec_QM17.Tables[0].Rows[0]["MAT_NO"] = bcls_rec->Tables[0].Rows[i]["mat_no"].ToString();;
			bcls_rec_QM17.Tables[0].Rows[0]["REL_REMARK"] = "板坯库场判废";
			bcls_rec_QM17.Tables[0].Rows[0]["REL_MAKER"] = s.userid;
			bcls_rec_QM17.Tables[0].Rows[0]["REL_TIME"] = datetime;
			bcls_rec_QM17.Tables[0].Rows[0]["HOLD_CAUSE_CODE"] = " ";
			bcls_rec_QM17.Tables[0].Rows[0]["SCRAP_CAUSE_CODE"] = "aaaa";

			doFlag = f_mmsm99(&bcls_rec_QM17, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
			
			

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
