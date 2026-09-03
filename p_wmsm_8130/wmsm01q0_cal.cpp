/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         LIZHEN
Version:		1.0
Date:			2024-4-20
Description:	计算复合元素
**************************************************/

//框架头文件
#include "stdafx.h"
#include<regex>
//程序用头文件



//函数申明

/*<remark>=========================================================

===========================================================</remark>*/

BM2F_ENTERACE(wmsm01q0_cal);

int f_wmsm01q0_cal(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;


	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */
	//CTWM06 twm06(conn);
	CModel twm06("TWM06");

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

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_dual(conn);

	//系统的分页类信息。
	

	try
	{
		EIClass tmp;
		EIClass tmp_rec;
		for (int i = 0; i < bcls_rec->Tables[0].Columns.get_Count(); i++)
		{
			Log::Trace("", __FUNCTION__, "i{0}", i, bcls_rec->Tables[0].Columns[i].get_ColumnName());
			if (i % 2 != 0 && i != 0)
			{
				
			}
			if (bcls_rec->Tables[0].Columns[i].get_ColumnName().GetLength()==3)
			{
				tmp_rec.Tables[0].Columns.Add(DT_DECIMAL, bcls_rec->Tables[0].Columns[i].get_ColumnName());
				if (tmp_rec.Tables[0].Rows.get_Count() != 1)
				{
					tmp_rec.Tables[0].Rows.Add();
				}
				
				tmp_rec.Tables[0].Rows[0][bcls_rec->Tables[0].Columns[i].get_ColumnName()] = bcls_rec->Tables[0].Rows[0][i].ToDecimal();
				
			}
			else
			{
				bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, bcls_rec->Tables[0].Columns[i].get_ColumnName());
			}
		}
		sql = " select ELM_NAME,ELM_ACT from tqmts29  t29 left join TEP0002 t2 on t2.CODE_CLASS='QMYS2N' AND T29.ELM_CODE=T2.CODE  where HEAT_NO='"+bcls_rec->Tables[1].Rows[0]["HEAT_NO"].ToString() + "'  AND T2.CODE_DESC_4_CONTENT=' ' ";
		Log::Trace("", __FUNCTION__, "sqlstr{0}", sql);
		cmd_inq.SetCommandText(sql);
		cmd_inq.ExecuteQuery(tmp.Tables[0]);
		cmd_inq.Close();

		bcls_ret->Tables[0].Rows.Add();
		Log::Trace("", __FUNCTION__, "sqlstr{0}", bcls_ret->Tables[0].Rows.get_Count(), bcls_ret->Tables[0].Columns.get_Count());
		Log::Trace("", __FUNCTION__, "sqlstr{0}", tmp.Tables[0].Rows.get_Count(), tmp.Tables[0].Columns.get_Count());
		Log::Trace("", __FUNCTION__, "sqlstr{0}", tmp_rec.Tables[0].Rows.get_Count(), tmp_rec.Tables[0].Columns.get_Count());
		for (int i = 0; i < bcls_ret->Tables[0].Columns.get_Count(); i++)
		{
			CString elm_complex = bcls_ret->Tables[0].Columns[i].get_ColumnName();
			Log::Trace("", __FUNCTION__, "elm_complex{0}", elm_complex);
			for (int j = 0; j < tmp.Tables[0].Rows.get_Count(); j++)
			{
				elm_complex = elm_complex.Replace(tmp.Tables[0].Rows[j]["ELM_NAME"].ToString().ToUpper(), tmp.Tables[0].Rows[j]["ELM_ACT"].ToString());
			}
			Log::Trace("", __FUNCTION__, "elm_complex{0}", elm_complex);
			for (int j = 0; j < tmp_rec.Tables[0].Columns.get_Count(); j++)
			{
				elm_complex = elm_complex.Replace(tmp_rec.Tables[0].Columns[j].get_ColumnName(), tmp_rec.Tables[0].Rows[0][j].ToString());
			}
			Log::Trace("", __FUNCTION__, "elm_complex{0}", elm_complex);
			string dest = string((const char*)elm_complex);
			regex pattern("[a-zA-z]");
			bool is_match = regex_search(dest, pattern);
			if (!is_match)//没找到
			{
				regex pattern0("[\\/][0][^\\.]");
				bool is_match0 = regex_search(dest, pattern0);
				if (!is_match0)//未找到，说明不存在/0的格式
				{
					regex pattern0_end("[\\/][0]$");
					bool is_match0_end = regex_search(dest, pattern0_end);
					if (!is_match0_end)//末尾不为/0
					{
						sqlstr = "SELECT " + dest + " FROM DUAL";
						cmd_dual.SetCommandText(sqlstr);
						cmd_dual.ExecuteReader();
						if (cmd_dual.Read())
						{
							bcls_ret->Tables[0].Rows[0][i] = cmd_dual.GetDecimal(1).Round(5);
						}
					}
				}
				else
				{
					Log::Trace("", "", "line = {0},有错误算法不计算", __LINE__);
					Log::Trace("", "", "elm_complex = {0}", elm_complex);
					continue;
				}
			}
			else
			{
				Log::Trace("", "", "line = {0},缺少元素不计算", __LINE__);
				Log::Trace("", "", "elm_complex = {0}", elm_complex);
				continue;
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