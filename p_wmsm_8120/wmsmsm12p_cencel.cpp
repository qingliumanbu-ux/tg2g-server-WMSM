/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         lizhen
Version:		1.0
Date:			2024-1-4
Description:	撤销装车
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
int f_wmsm_21a009_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wmsm_load_d_proc(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);


BM2F_ENTERACE(wmsmsm12p_cencel);

int f_wmsmsm12p_cencel(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */

	CModel twmsm12 = CModel("TWMSM12");
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
	CModel twmsm61 = CModel("TWMSM61");


	//系统的分页类信息。
	
	/*装车任务号*/
	CString load_scheme_no("");
	EIClass bcls_load;
	bcls_load.Tables[0].set_TableName("21A009");
	bcls_load.Tables[0].Columns.Add(twmsm61);
	bcls_load.Tables[0].Rows.Clear();
	//调用物料事件
	EIClass mm0099;
	mm0099.Tables[0].set_TableName("MM0099");
	mm0099.Tables[0].Columns.Add(tmmsm96);
	mm0099.Tables[0].Rows.Clear();
	try 
	{
		load_scheme_no = bcls_rec->Tables[0].Rows[0]["LOAD_SCHEME_NO"].ToString();
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			twmsm12.Reset();
			twmsm12["LOAD_SCHEME_NO"] = load_scheme_no;
			twmsm12.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			if (!twmsm12.Query("LOAD_SCHEME_NO,MAT_NO")) {
				sprintf(s.msg, "该材料不能撤销预装车！");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			/*tmmsm01["MAT_NO"] = twmsm12["MAT_NO"];
			tmmsm01.Query("MAT_NO");*/
		
			twmsm12.Delete("LOAD_SCHEME_NO,MAT_NO");

			if (twmsm12["ARCHIVE_FLAG"].ToString() == "1")
			{
				hmmsm01.CopyFrom(twmsm12);
				hmmsm01.Query("MAT_NO");
				CString practice_no = hmmsm01["PRACTICE_NO"].ToString();
				hmmsm01["LOGISTICS_STATUS"] = "0";//1--装车
				hmmsm01["PRE_LOAD_FLAG"] = "0";//1--装车
				hmmsm01["FACTORY_TO"] = " ";
				hmmsm01["DST_STOCK_CODE"] = " ";
				hmmsm01["LOAD_SCHEME_NO"] = " ";
				hmmsm01["PRACTICE_NO"] = " ";
				CString	upd_str = " FACTORY_TO,DST_STOCK_CODE,UNLOAD_CODE,LOGISTICS_STATUS,PRE_LOAD_FLAG,LOAD_SCHEME_NO,PRACTICE_NO ";
				hmmsm01.Update(upd_str, "MAT_NO");

				if (twmsm12["UNLOAD_CODE_FACTORY"].ToString() == "WXK1")
				{
					twmsm61["PRACTICE_NO"] = practice_no;
					twmsm61.Query("PRACTICE_NO,MAT_NO");
					twmsm61["DEAL_FLAG"] = "D";

					/*twmsm61.MergeFrom(bcls_rec->Tables[0].Rows[i]);*/

					twmsm61.MergeTo(bcls_load.Tables["21A009"], false);
					twmsm61.Delete("PRACTICE_NO,MAT_NO");
				}
			}
			else
			{
				tmmsm96.Reset();
				tmmsm96.CopyFrom(twmsm12);
				tmmsm96["LOGISTICS_STATUS"] = "0";//1--未装
				tmmsm96["PRE_LOAD_FLAG"] = "0";//1--未装
				tmmsm96["FACTORY_TO"] = " ";
				tmmsm96["DST_STOCK_CODE"] = " ";
				tmmsm96["LOAD_SCHEME_NO"] = " ";
				tmmsm96["LOAD_UP_TIME"] = " ";
				tmmsm96["EVENT_ID"] = "MM78";
				tmmsm96["SYSTEM_ID"] = "MMSM";
				tmmsm96["EVENT_LINE_TYPE"] = "00";
				tmmsm96["FUNC_ID"] = s.svc_name;
				tmmsm96.MergeTo(mm0099.Tables["MM0099"], false);
			}
			
		}
		if (mm0099.Tables[0].Rows.get_Count() > 0) {
			doFlag = f_mmsm99(&mm0099, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}
		if (bcls_load.Tables[0].Rows.get_Count() > 0)
		{
			doFlag = f_wmsm_21a009_snd(&bcls_load, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
			doFlag = f_wmsm_load_d_proc(&bcls_load, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
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