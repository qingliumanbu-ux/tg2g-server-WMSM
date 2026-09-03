/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         LIZHEN
Version:		1.0
Date:			2023-11-07
Description:	板坯修改铸坯存放类型
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件

//int f_mmsm99(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
//int f_wmsm_t8p301_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
//int f_wmsm_t8p302_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
//int f_wmsm_t8p303_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);

//函数申明

/*<remark>=========================================================

===========================================================</remark>*/

BM2F_ENTERACE(wmsm01q0_cfupd);

int f_wmsm01q0_cfupd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;


	CDecimal rowCount = 0;
	int fetchRowCount = 0;


	/* 数据库SQL操作字符串 */
	CString sql = "";
	CString sqlstr = "";
	CString sqlwhere = "";
	CString sqlstr_count;
	CString sqlstr_temp;

	/* 业务变量 */
	CString	datetime("");
	CString	cs_ladle_no("");
	CString	s_heat_no("");
	CDecimal cd_count = 0;
	int	record_count_per_page = 0; /* 每页记录数 */
	int	current_page_no = 0; /* 需查询的页号,从0开始计数 */
	int	start_row = 0; /* 将要压入outBlock的起始行 */


	/* 全局变量 */
	CString crane_no = "";
	CString stock_place_no_from = "";
	CString stock_place_no_to = "";
	/* 数据库操作类定义 */
	CModel tmmsm01("TMMSM01");
	CModel hmmsm01("HMMSM01");
	CDbCommand cmd_inq(conn);

	//系统的分页类信息。
	CPageInfo pageInfo;

	EIClass inblock;
	inblock.Tables[0].Columns.Add(tmmsm01);
	inblock.Tables[0].Rows.Clear();
	EIClass inblock1;
	inblock1.Tables[0].Columns.Add(tmmsm01);
	inblock1.Tables[0].Rows.Clear();

	try
	{
		Log::Trace("", "", "111SLAB_STORAGE_TYPE={0}", bcls_rec->Tables[1].Rows[0]["SLAB_STORAGE_TYPE"].ToString());

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++) {
			tmmsm01.Reset();
			hmmsm01.Reset();
			tmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			hmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			Log::Trace("", "", "222SLAB_STORAGE_TYPE={0}", bcls_rec->Tables[1].Rows[0]["SLAB_STORAGE_TYPE"].ToString());

			if (tmmsm01.QueryCount("MAT_NO") > 0)
			{
				Log::Trace("", "", "333SLAB_STORAGE_TYPE={0}", bcls_rec->Tables[1].Rows[0]["SLAB_STORAGE_TYPE"].ToString());
				Log::Trace("", "", "tMAT_NO={0}", bcls_rec->Tables[0].Rows[i]["MAT_NO"].ToString());
				tmmsm01["SLAB_STORAGE_TYPE"] = bcls_rec->Tables[1].Rows[0]["SLAB_STORAGE_TYPE"];
				tmmsm01.Update("SLAB_STORAGE_TYPE", "MAT_NO");
			}
			else
			{
				hmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[i]);
				if (hmmsm01.QueryCount("MAT_NO") > 0)
				{
					Log::Trace("", "", "444SLAB_STORAGE_TYPE={0}", bcls_rec->Tables[1].Rows[0]["SLAB_STORAGE_TYPE"].ToString());
					Log::Trace("", "", "hMAT_NO={0}", bcls_rec->Tables[0].Rows[i]["MAT_NO"].ToString());

					hmmsm01["SLAB_STORAGE_TYPE"] = bcls_rec->Tables[1].Rows[0]["SLAB_STORAGE_TYPE"];
					hmmsm01.Update("SLAB_STORAGE_TYPE", "MAT_NO");
				}
				else
				{
					strcpy(s.msg, "在库和历史表中都没有该数据！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}


		}


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