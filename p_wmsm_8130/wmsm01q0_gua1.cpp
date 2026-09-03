/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         LIZHEN
Version:		1.0
Date:			2023-11-07
Description:	板坯人工匹配
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件

int f_mmsm99(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_t8z_23m_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//发送专家系统数据

//函数申明

/*<remark>=========================================================

===========================================================</remark>*/

BM2F_ENTERACE(wmsm01q0_gua1);

int f_wmsm01q0_gua1(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
	CString s_no_slab_cause = " ";
	/* 数据库操作类定义 */
	CModel tmmsm01("TMMSM01");
	CModel tmmsm96("TMMSM96");
	CModel tpssm03("TPSSM03");
	CModel tqmom01("TQMOM01");
	CDbCommand cmd_inq(conn);

	//系统的分页类信息。
	CPageInfo pageInfo;

	EIClass in_23m;
	in_23m.Tables[0].Columns.Add(DT_STRING, "TC_NO");
	in_23m.Tables[0].Rows.Add();
	in_23m.Tables[0].Rows[0]["TC_NO"] = "T82322";
	in_23m.Tables.Add();
	in_23m.Tables[1].Columns.Add(tmmsm01);

	try
	{
		if (!bcls_rec->Tables.Contains("MM0099"))
		{
			bcls_rec->Tables.Add("MM0099");
			bcls_rec->Tables["MM0099"].Columns.Add(tmmsm96);
		}
		//判断该PONO下还有足够的slab_no
		tpssm03.MergeFrom(bcls_rec->Tables[1].Rows[0]);
		tpssm03["SLAB_PROD_FLAG"] = "0";
		int slab_c = tpssm03.QueryCount("PONO,SLAB_PROD_FLAG");
		Log::Trace("", __FUNCTION__, "slab_c【{0}】", slab_c);
		if (slab_c < bcls_rec->Tables[0].Rows.get_Count())
		{
			/*sprintf(s.msg, "该PONO下只有[%d]条未使用的预定铸坯号，不足以进行自动匹配。", (const char*)slab_c);
			throw CApplicationException(-1, s.msg, log.Location);*/
		}

		EIClass temp;
		sqlstr = " select * from tpssm01 where PONO='" + tpssm03["PONO"].ToString() + "'  ";
		Log::Trace("", __FUNCTION__, "sqlstr{0}", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(temp.Tables[0]);
		cmd_inq.Close();

		EIClass MM99;
		MM99.Tables[0].Columns.Add(tmmsm96);
		MM99.Tables[0].set_TableName("MM0099");
		MM99.Tables[0].Rows.Clear();
		Log::Trace("", __FUNCTION__, "bcls_rec【{0}】", bcls_rec->Tables[0].Rows.get_Count(),bcls_rec->Tables[2].Rows.get_Count(), temp.Tables[0].Rows[0]["ST_NO"]);
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++) {
			tmmsm01.Reset();//不能去掉，否则有bug by 王建征 20240516
			tmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			if (!tmmsm01.Query("MAT_NO"))
			{
				sprintf(s.msg, "当前材料[%s]已归档不可操作。", (const char*)tmmsm01["MAT_NO"]);
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			else if (tmmsm01["RCV_MAT_FLAG"].ToString() == "S")
			{
				sprintf(s.msg, "当前材料[%s]已收货不可操作。", (const char*)tmmsm01["MAT_NO"]);
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (tmmsm01["LSLAB_NO"].ToString().Trim() != "")
			{
				tpssm03["LSLAB_NO"] = tmmsm01["LSLAB_NO"];
				tpssm03["SLAB_PROD_FLAG"] = "0";
				tpssm03.Update("SLAB_PROD_FLAG", "LSLAB_NO");
				tmmsm96.CopyFrom(tmmsm01);
				tmmsm96["EVENT_ID"] = "MM73";
				tmmsm96["EVENT_LINE_TYPE"] = "00";
				tmmsm96["SYSTEM_ID"] = "WMSM";
				tmmsm96["FUNC_ID"] = s.svc_name;
				tmmsm96["EVENT_DESC"] = "收货后取消匹配";
				tmmsm96["FORM_NAME"] = s.formname;
				tmmsm96.MergeTo(MM99.Tables["MM0099"], false);
			}
			tmmsm01.Query("MAT_NO");
			tmmsm01["PONO_SLAB_1"] = " ";
			tmmsm01["PONO_SLAB_2"] = " ";
			tmmsm01["PONO_SLAB_3"] = " ";
			tmmsm01["PONO_SLAB_4"] = " ";
			tmmsm01["PONO_SLAB_5"] = " ";
			tmmsm01["PONO_SLAB_6"] = " ";
			tmmsm01["PONO_SLAB_7"] = " ";
			tmmsm01["PONO_SLAB_8"] = " ";
			tmmsm01["PONO_SLAB_9"] = " ";
			tmmsm01["PONO_SLAB_10"] = " ";
			tmmsm01["PONO_SLAB_11"] = " ";
			tmmsm01["PONO_SLAB_12"] = " ";
			
			for (int j = 0; j < bcls_rec->Tables[2].Rows.get_Count(); j++)
			{
				s_no_slab_cause = " ";
				
				if (tmmsm01["MAT_WIDTH"].ToDecimal() != bcls_rec->Tables[2].Rows[j]["SLAB_WIDTH"])
				{
					s_no_slab_cause += "铸坯宽度不符";
					continue;
				}
				if (tmmsm01["MAT_THICK"].ToDecimal() != bcls_rec->Tables[2].Rows[j]["SLAB_THICK"])
				{
					s_no_slab_cause += "铸坯厚度不符";
					continue;
				}
				if (tmmsm01["ST_NO"].ToString() != temp.Tables[0].Rows[0]["ST_NO"].ToString().Trim())
				{
					s_no_slab_cause += "钢种不符";
					continue;
				}
			
				tmmsm01["LSLAB_NO"] = bcls_rec->Tables[2].Rows[j]["SLAB_NO"];
				Log::Trace("", __FUNCTION__, "LSLAB_NO{0}", tmmsm01["LSLAB_NO"].ToString());
				/*int pos = 0;
				CString token;
				std::string delimiter = "/";
				string LSLAB_NO = string(bcls_rec->Tables[2].Rows[j]["SLAB_NO"]);
				int j = 0;
				while ((pos = LSLAB_NO.find(delimiter)) != std::string::npos) {
					token = LSLAB_NO.substr(0, pos);
					Log::Trace("", __FUNCTION__, "fddfgtyuijhbhjkolp【{0}】", token);
					++j;
					tmmsm01["PONO_SLAB_" + CConvert::ToString(j)] = token;
				}*/
				
				int slab_no_count = 0;
				sqlstr = " select * from tpssm03 where  LSLAB_NO='" + tmmsm01["LSLAB_NO"].ToString() + "' AND SLAB_PROD_FLAG <> '1' ";
				Log::Trace("", __FUNCTION__, "sqlstr{0}", sqlstr);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				while (cmd_inq.Read())
				{
					++slab_no_count;
					Log::Trace("", __FUNCTION__, "slab_no_count{0}", slab_no_count);
					cmd_inq.Fetch(tpssm03);
					tmmsm01["ORDER_NO"] = tpssm03["ORDER_NO"];
					tmmsm01["WHOLE_BACKLOG"] = tpssm03["WHOLE_BACKLOG"];
					tmmsm01["WHOLE_BACKLOG_NO"] = tpssm03["WHOLE_BACKLOG_NO"];
					tmmsm01["WHOLE_BACKLOG_CODE"] = tpssm03["WHOLE_BACKLOG_CODE"];
					tmmsm01["WHOLE_BACKLOG_SEQ"] = tpssm03["WHOLE_BACKLOG_SEQ"];
					tmmsm01["NEXT_WHOLE_BACKLOG_SEQ"] = tpssm03["WHOLE_BACKLOG_SEQ"].ToDecimal() + 1;
					tmmsm01["MSC"] = tpssm03["MSC"];
					tmmsm01["APN"] = tpssm03["APN"];
					tmmsm01["PSC"] = tpssm03["PSC"];
					tmmsm01["PONO_SLAB_" + CConvert::ToString(slab_no_count)] = tpssm03["SLAB_NO"];
					tmmsm01["MAT_DESTION"] = tpssm03["SLAB_DEST"];
					//if (tpssm03["SLAB_DEST"].ToString() == "50")//外卖
					//{
					//	tmmsm01["PRODUCT_FLAG"] = "1";
					//}
					//else
					//{
					//	tmmsm01["PRODUCT_FLAG"] = "0";
					//}
					if (tpssm03["MATIRAL_CODE"].ToString().SubstringNE(0, 1) == "F")//外卖
					{
						tmmsm01["PRODUCT_FLAG"] = "1";
					}
					else
					{
						tmmsm01["PRODUCT_FLAG"] = "0";
					}
				}
				tmmsm01["PREC_SLAB_NO"] = tmmsm01["PONO_SLAB_1"];
				tmmsm01["PONO_SLAB"] = tmmsm01["PONO_SLAB_1"];
				tpssm03["LSLAB_NO"] = tmmsm01["LSLAB_NO"];
				tpssm03["SLAB_PROD_FLAG"] = "1";
				tpssm03.Update("SLAB_PROD_FLAG", "LSLAB_NO");
				cmd_inq.Close();
				
				bcls_rec->Tables[2].Rows.Remove(j);
				
			}
			tmmsm01["NO_SLAB_CAUSE"] = s_no_slab_cause;
			if (s_no_slab_cause.Trim()!="") {
				sprintf(s.msg, "匹配失败，原因[%s]。", (const char*)s_no_slab_cause);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			
			if (tmmsm01["ORDER_NO"].ToString().Trim() != "") {
				tqmom01["ORDER_NO"] = tmmsm01["ORDER_NO"];
				if (tqmom01.Query("ORDER_NO")) {
					tmmsm01["PROD_CODE"] = tqmom01["PROD_CODE"];
					tmmsm01["FIN_CUST_CODE"] = tqmom01["FIN_CUST_CODE"];
					tmmsm01["SG_SIGN"] = tqmom01["SG_SIGN"];
					tmmsm01["SG_STD"] = tqmom01["SG_STD"];
					tmmsm01["PROD_CLASS_CODE"] = tqmom01["PROD_CLASS_CODE"];
				}
			}
			if (tmmsm01["LSLAB_NO"].ToString() != "")
			{
				if (tmmsm01["RCV_MAT_FLAG"].ToString() == "S") //已收货；使用MM72
				{
					/*tmmsm96.CopyFrom(tmmsm01);
					tmmsm96["EVENT_ID"] = "MM72";
					tmmsm96["EVENT_LINE_TYPE"] = "00";
					tmmsm96["SYSTEM_ID"] = "WMSM";
					tmmsm96["FUNC_ID"] = s.svc_name;
					tmmsm96["EVENT_DESC"] = "收货后取消匹配";
					tmmsm96["FORM_NAME"] = s.formname;

					tmmsm96.MergeTo(bcls_rec->Tables["MM0099"], false);*/
				}
				else//已收货；使用MM71
				{
					tmmsm01.Print();
					tmmsm96.CopyFrom(tmmsm01);
					tmmsm96["EVENT_ID"] = "MM71";
					tmmsm96["EVENT_LINE_TYPE"] = "00";
					tmmsm96["SYSTEM_ID"] = "WMSM";
					tmmsm96["FUNC_ID"] = s.svc_name;
					tmmsm96["EVENT_DESC"] = "收货前匹配";
					tmmsm96["FORM_NAME"] = s.formname;
					tmmsm96.MergeTo(bcls_rec->Tables["MM0099"], false);
				}
			}
			tmmsm01.MergeTo(in_23m.Tables[1]);
		}
		if (MM99.Tables["MM0099"].Rows.get_Count() > 0) {

			doFlag = f_mmsm99(&MM99, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		if (bcls_rec->Tables["MM0099"].Rows.get_Count() > 0) {

			doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}

#pragma region 调用函数，发送智慧质量电文
		if (in_23m.Tables[1].Rows.get_Count() > 0)
		{
			doFlag = f_t8z_23m_snd(&in_23m, bcls_ret, conn);
		}
#pragma endregion
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