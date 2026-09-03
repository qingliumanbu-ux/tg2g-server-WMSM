/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         LIZHEN
Version:		1.0
Date:			2023-11-07
Description:	板坯修改指导去向
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件

int f_mmsm99(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
//int f_wmsm_t8p301_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
//int f_wmsm_t8p302_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wmsm_t8p303_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);

//函数申明

/*<remark>=========================================================

===========================================================</remark>*/

BM2F_ENTERACE(wmsm01q0_qxupd);

int f_wmsm01q0_qxupd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
	CModel tmmsm01("TMMSM01");
	CModel hmmsm01("HMMSM01");
	CModel tmmsm96("TMMSM96");
	CModel tpssm03("TPSSM03");
	CModel tqmom01("TQMOM01");
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
		if (!bcls_rec->Tables.Contains("MM0099"))
		{
			bcls_rec->Tables.Add("MM0099");
			bcls_rec->Tables["MM0099"].Columns.Add(tmmsm96);
		}
		
		if (bcls_rec->Tables[1].Rows[0]["STOCK_L2"].ToString().Trim() == "")
		{
			sprintf(s.msg, "二级库存地不能为空！。");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		
		
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++) {
			tmmsm01.Reset();
			hmmsm01.Reset();
			tmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			hmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			if (tmmsm01.Query("MAT_NO"))
			{

				CString c_quxiang = Db::QueryCString(" select CODE_DESC_1_CONTENT from TWMSMZD02 where CODE_CLASS = 'WM02' and CODE ='" + bcls_rec->Tables[1].Rows[0]["GUIDE_DEST"].ToString() + "' ");
				if (c_quxiang.Find("2250") >= 0&&tmmsm01["STOCK_L2"].ToString()!= bcls_rec->Tables[1].Rows[0]["STOCK_L2"].ToString())
				{
					/*strcpy(s.msg, "只有非2250去向，才允许修改二级库存地!");
					throw CApplicationException(-1, s.msg, s.svc_name);*/
				}
				Log::Trace("", "", "STOCK_L2={0}", bcls_rec->Tables[1].Rows[0]["STOCK_L2"].ToString(), tmmsm01["STOCK_L2"].ToString());
				if (tmmsm01["STOCK_L2"].ToString() != bcls_rec->Tables[1].Rows[0]["STOCK_L2"].ToString() && bcls_rec->Tables[1].Rows[0]["STOCK_L2"].ToString() == "SYA")
				{
					Log::Trace("", "", "STOCK_L2={0}", "进来");
					tmmsm01["STOCK_NO"] = "SYA";
				}
				tmmsm01["GUIDE_DEST"] = bcls_rec->Tables[1].Rows[0]["GUIDE_DEST"];
				tmmsm01["STOCK_L2"] = bcls_rec->Tables[1].Rows[0]["STOCK_L2"];
				
				
				tmmsm01.MergeTo(inblock.Tables[0], false);
				tmmsm96.CopyFrom(tmmsm01);
				tmmsm96["EVENT_ID"] = "MM75";
				tmmsm96["EVENT_LINE_TYPE"] = "00";
				tmmsm96["SYSTEM_ID"] = "WMSM";
				tmmsm96["FUNC_ID"] = s.svc_name;
				tmmsm96["EVENT_DESC"] = "板坯修改指导去向";
				tmmsm96["FORM_NAME"] = s.formname;
				tmmsm96.MergeTo(bcls_rec->Tables["MM0099"], false);
			}
			else if (hmmsm01.Query("MAT_NO"))
			{
				hmmsm01["GUIDE_DEST"] = bcls_rec->Tables[1].Rows[0]["GUIDE_DEST"];
				hmmsm01.Update("GUIDE_DEST", "MAT_NO");
			}
			else
			{
				sprintf(s.msg, "该未在二钢发现该材料。");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			//CString old_if_hr = Db::QueryCString(" select decode(CODE_DESC_3_CONTENT,'1','1','0') from TWMSMZD02 where CODE_CLASS = 'WM02' and CODE ='" + tmmsm01["GUIDE_DEST"].ToString() + "' ");
			
			/*CString new_if_hr = Db::QueryCString(" select decode(CODE_DESC_3_CONTENT,'1','1','0') from TWMSMZD02 where CODE_CLASS = 'WM02' and CODE ='" + tmmsm01["GUIDE_DEST"].ToString() + "' ");
			if (old_if_hr.Find("1") >= 0 && new_if_hr.Find("1") < 0)
			{
				tmmsm01.MergeTo(inblock.Tables[0], false);
			}
			if (old_if_hr.Find("1") < 0 && new_if_hr.Find("1") >= 0)
			{
				tmmsm01.MergeTo(inblock1.Tables[0], false);
			}*/
			
			
		}
		if (bcls_rec->Tables["MM0099"].Rows.get_Count() > 0) {

			doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		if (inblock.Tables[0].Rows.get_Count() > 0)
		{
			doFlag = f_wmsm_t8p303_snd(&inblock, bcls_ret, conn);
			if (doFlag < 0) {
				throw CApplicationException(-1, s.msg, s.svc_name);
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