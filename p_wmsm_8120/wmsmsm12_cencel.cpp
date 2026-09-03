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
int f_mmsm99(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wmsm_21a009_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wmsm_load_d_proc(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);


/*<remark>=========================================================
///<summary>
///装车实绩查询
///<para>
///2.排序方式：
///</para>
///<para>数据库表：TWMSM61 装车实绩表；
///<returns>返回符合查询条件的实绩信息</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wmsmsm12_cencel);

int f_wmsmsm12_cencel(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */
	CModel tmmsm01 = CModel("TMMSM01");
	CModel tmmsm96 = CModel("TMMSM96");
	CModel twmsm61 = CModel("TWMSM61");
	CModel twma0 = CModel("TWMA0");

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


	//系统的分页类信息。
	CPageInfo pageInfo;

	/* 业务变量 */
	CString practice_no("");
	//发送电文
	EIClass bcls_load;
	bcls_load.Tables[0].set_TableName("21A009");
	bcls_load.Tables[0].Columns.Add(twmsm61);
	bcls_load.Tables[0].Rows.Clear();

	//调用物料事件
	EIClass mm0099;
	mm0099.Tables[0].set_TableName("MM0099");
	mm0099.Tables[0].Columns.Add(tmmsm96);
	mm0099.Tables[0].Rows.Clear();
	EIClass mm00991;
	mm00991.Tables[0].set_TableName("MM0099");
	mm00991.Tables[0].Columns.Add(tmmsm96);
	mm00991.Tables[0].Rows.Clear();

	
	try {
		practice_no = bcls_rec->Tables[0].Rows[0]["PRACTICE_NO"].ToString();
		Log::Trace("", __FUNCTION__, "row=[{0}]", bcls_rec->Tables[0].Rows.get_Count());
		for (int i=0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			if (tmmsm01["MAT_NO"].ToString().Trim() == "")
			{
				sprintf(s.sysmsg, "材料号不能为空");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (tmmsm01.Query("MAT_NO"))
			{
				if (tmmsm01["LOGISTICS_STATUS"].ToString() != "2")
				{
					sprintf(s.sysmsg, "材料物流状态错误，只有装车确认状态才可以撤销！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				//发装车实绩
				twmsm61.CopyFrom(tmmsm01);
				twmsm61["PRACTICE_NO"] = practice_no;
				twmsm61.Query("PRACTICE_NO,MAT_NO");
				twmsm61["DEAL_FLAG"] = "D";

				/*twmsm61.MergeFrom(bcls_rec->Tables[0].Rows[i]);*/

				twmsm61.MergeTo(bcls_load.Tables["21A009"], false);
				twmsm61.Delete("PRACTICE_NO,MAT_NO");
				Log::Trace("", __FUNCTION__, "LINKE=[{0}]", __LINE__);
				//15、调物流事件
				tmmsm96.Reset();
				tmmsm96.CopyFrom(tmmsm01);
				tmmsm96["LOGISTICS_STATUS"] = "0";//0--未装车
				tmmsm96["FACTORY_TO"] = " ";
				tmmsm96["DST_STOCK_CODE"] = " ";
				tmmsm96["UNLOAD_CODE"] = " ";
				tmmsm96["OUT_STOCK_TIME"] = " ";
				tmmsm96["PRACTICE_NO"] = " ";
				tmmsm96["EVENT_ID"] = "MM77";
				tmmsm96["SYSTEM_ID"] = "MMSM";
				tmmsm96["EVENT_LINE_TYPE"] = "00";
				tmmsm96["FUNC_ID"] = s.svc_name;
				tmmsm96.MergeTo(mm0099.Tables["MM0099"], false);

				//15、调预装事件
				tmmsm96.Reset();
				tmmsm96.CopyFrom(tmmsm01);
				tmmsm96["LOGISTICS_STATUS"] = "0";//1--未装
				tmmsm96["PRE_LOAD_FLAG"] = "0";//1--未装
				tmmsm96["FACTORY_TO"] = " ";
				tmmsm96["DST_STOCK_CODE"] = " ";
				tmmsm96["LOAD_SCHEME_NO"] = " ";
				tmmsm96["EVENT_ID"] = "MM78";
				tmmsm96["SYSTEM_ID"] = "MMSM";
				tmmsm96["EVENT_LINE_TYPE"] = "00";
				tmmsm96["FUNC_ID"] = s.svc_name;
				tmmsm96.MergeTo(mm00991.Tables["MM0099"], false);

				tmmsm01["IN_FLAG"] = "1";
				tmmsm01.Update("IN_FLAG", "MAT_NO");

				twma0.Reset();
				twma0["STOCK_OPER_ORDER"] = "1L";
				twma0["MAT_NO"] = tmmsm01["MAT_NO"];
				if (twma0.QueryCount("MAT_NO,STOCK_OPER_ORDER")>0)
				{
					twma0.Delete("MAT_NO,STOCK_OPER_ORDER");
				}
			}
			else
			{
			/*	sprintf(s.sysmsg, "材料号不存在");
				throw CApplicationException(-1, s.msg, s.svc_name);*/
				strcpy(s.msg, "材料号不存在或已归档！");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}
		//调用物料事件
		if (mm0099.Tables["MM0099"].Rows.get_Count() > 0)
		{
			doFlag = f_mmsm99(&mm0099, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		if (mm00991.Tables["MM0099"].Rows.get_Count() > 0)
		{
			doFlag = f_mmsm99(&mm00991, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		//发装车实绩电文
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