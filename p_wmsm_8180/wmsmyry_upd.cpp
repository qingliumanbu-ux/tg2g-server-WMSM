/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      BHY
Version:     1.0
Date:        2025-05-09
Description: 北区不锈钢预溶液编辑
**************************************************/
//框架头文件
#include "stdafx.h"

#include "CUtils.h"



BM2F_ENTERACE(wmsmyry_upd)

int f_wmsmyry_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString sqlstr_inq = " ";
	CString datetime = " ";
	CString msgstr = "提示信息:";	//提示信息
	CString v_operate = "";
	CDbCommand cmd(conn);
	int proc_sum = 0;				//操作总数
	CModel twmsmyry("TWMSMYRY");
	CModel twmsmyry_1("TWMSMYRY");
	CModel twmsmyryts("TWMSMYRYTS");

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		
		
		/************************新增*******************************************/
		if (bcls_rec->Tables.Contains("ADD"))
		{
			if (bcls_rec->Tables["PRO_DIV"].Columns.Contains("PRO_DIV"))
			{
				v_operate = bcls_rec->Tables["PRO_DIV"].Rows[0]["PRO_DIV"].ToString().Trim();
				Log::Trace("", "", "v_operate=[{0}],", v_operate);
			}

			Log::Trace("", "", "新增开始,count=[{0}]", bcls_rec->Tables["ADD"].Rows.get_Count());
			Log::Trace("", "", "--LINE,count=[{0}]", __LINE__);
			Log::Trace("", "", "Tables_ADD=[{0}]", __LINE__);

			if (v_operate == "TS")
			{
				for (int i = 0; i < bcls_rec->Tables["ADD"].Rows.get_Count(); i++)
				{
					CString v_factory_2 = bcls_rec->Tables["ADD"].Rows[i]["FACTORY_2"].ToString();
					CString v_memo_detail = bcls_rec->Tables["ADD"].Rows[i]["MEMO_DETAIL"].ToString();
					twmsmyryts.Reset();
					twmsmyryts["REC_CREATE_TIME"] = datetime;
					twmsmyryts["REC_CREATOR"] = s.userid;
					twmsmyryts["IDCARD"] = datetime.SubstringNE(0, 8) + Db::QueryCString("SELECT TWMSMYRYTS_SEQ.NEXTVAL FROM DUAL");
					twmsmyryts["FACTORY_2"] = v_factory_2;
					twmsmyryts["MEMO_DETAIL"] = v_memo_detail;
					proc_sum += twmsmyryts.Insert();
				}
			}
			
		}

		/************************修改*******************************************/
		if (bcls_rec->Tables.Contains("UPD"))
		{
			if (bcls_rec->Tables["PRO_DIV"].Columns.Contains("PRO_DIV"))
			{
				v_operate = bcls_rec->Tables["PRO_DIV"].Rows[0]["PRO_DIV"].ToString().Trim();
				Log::Trace("", "", "v_operate=[{0}],", v_operate);
			}

			Log::Trace("", "", "修改开始,count=[{0}]", bcls_rec->Tables["UPD"].Rows.get_Count());
			if (v_operate == "TS")
			{
				for (int i = 0; i < bcls_rec->Tables["UPD"].Rows.get_Count(); i++)
				{
					twmsmyryts.Reset();
					twmsmyryts.MergeFrom(bcls_rec->Tables["UPD"].Rows[i]);
					Log::Trace("", __FUNCTION__, "这里1", "");
					twmsmyryts["REC_REVISOR"] = s.userid;			//记录修改责任者
					twmsmyryts["REC_REVISE_TIME"] = datetime;		//记录修改时刻
					Log::Trace("", __FUNCTION__, "这里2", "");
					proc_sum += twmsmyryts.Update("REC_REVISOR,REC_REVISE_TIME,FACTORY_2,MEMO_DETAIL", "IDCARD");
					Log::Trace("", __FUNCTION__, "这里3", "");

				}
			}
			//if (v_operate == "A0" || v_operate == "A1" || v_operate == "A2" || v_operate == "E1" || v_operate == "E2")
			else
			{
				for (int i = 0; i < bcls_rec->Tables["UPD"].Rows.get_Count(); i++)
				{
					twmsmyry.Reset();
					twmsmyry.MergeFrom(bcls_rec->Tables["UPD"].Rows[i]);
					Log::Trace("", __FUNCTION__, "这里1", "");

					if (v_operate == "A0" || v_operate == "A1" || v_operate == "A2" || v_operate == "E1" || v_operate == "E2"){
						if (twmsmyry.Query("HEAT_NO_OLD,FACTORY_2")){
							twmsmyry["REC_REVISOR"] = s.userid;			//记录修改责任者
							twmsmyry["REC_REVISE_TIME"] = datetime;		//记录修改时刻
							if (bcls_rec->Tables["UPD"].Rows[i]["HEAT_NO"].ToString().GetLength()>0)
							{
								twmsmyry["HEAT_NO"] = bcls_rec->Tables["UPD"].Rows[i]["HEAT_NO"].ToString();
								Log::Trace("", "", "HEAT_NO=[{0}]", twmsmyry["HEAT_NO"].ToString());
							}
							else
							{
								twmsmyry["HEAT_NO"] = " ";
							}
							if (bcls_rec->Tables["UPD"].Rows[i]["ST_NO"].ToString().GetLength()>0)
							{
								twmsmyry["ST_NO"] = bcls_rec->Tables["UPD"].Rows[i]["ST_NO"].ToString();
							}
							else
							{
								twmsmyry["ST_NO"] = " ";
							}
							if (bcls_rec->Tables["UPD"].Rows[i]["SAP_ERP_PRCSPATH"].ToString().GetLength()>0)
							{
								twmsmyry["SAP_ERP_PRCSPATH"] = bcls_rec->Tables["UPD"].Rows[i]["SAP_ERP_PRCSPATH"].ToString();
							}
							else
							{
								twmsmyry["SAP_ERP_PRCSPATH"] = " ";
							}
							if (bcls_rec->Tables["UPD"].Rows[i]["OUT_STEEL_TIME"].ToString().GetLength()>0)
							{
								twmsmyry["OUT_STEEL_TIME"] = bcls_rec->Tables["UPD"].Rows[i]["OUT_STEEL_TIME"].ToString();
							}
							else
							{
								twmsmyry["OUT_STEEL_TIME"] = " ";
							}
							proc_sum += twmsmyry.Update("REC_REVISOR,REC_REVISE_TIME,HEAT_NO,ST_NO,SAP_ERP_PRCSPATH,OUT_STEEL_TIME", "HEAT_NO_OLD,FACTORY_2");
							Log::Trace("", __FUNCTION__, "更新1", "");
						}
						else
						{
							//twmsmyry["IDCARD"] = datetime.SubstringNE(0, 8) + Db::QueryCString("SELECT TWMSMYRY_SEQ.NEXTVAL FROM DUAL");
							twmsmyry["REC_CREATE_TIME"] = datetime;
							twmsmyry["REC_CREATOR"] = s.userid;
							twmsmyry["HEAT_NO_OLD"] = twmsmyry["HEAT_NO"];
							proc_sum += twmsmyry.Insert();
							Log::Trace("", __FUNCTION__, "插入1", "");
						}
					}
					else
					{
						if (twmsmyry.Query("HEAT_NO_OLD,FACTORY_2")){
							twmsmyry["REC_REVISOR"] = s.userid;			//记录修改责任者
							twmsmyry["REC_REVISE_TIME"] = datetime;		//记录修改时刻
							if (bcls_rec->Tables["UPD"].Rows[i]["HEAT_NO"].ToString().GetLength()>0)
							{
								twmsmyry["HEAT_NO"] = bcls_rec->Tables["UPD"].Rows[i]["HEAT_NO"].ToString();
							}
							else
							{
								twmsmyry["HEAT_NO"] = " ";
							}
							if (bcls_rec->Tables["UPD"].Rows[i]["ST_NO"].ToString().GetLength()>0)
							{
								twmsmyry["ST_NO"] = bcls_rec->Tables["UPD"].Rows[i]["ST_NO"].ToString();
							}
							else
							{
								twmsmyry["ST_NO"] = " ";
							}
							if (bcls_rec->Tables["UPD"].Rows[i]["OUT_STEEL_TIME"].ToString().GetLength()>0)
							{
								twmsmyry["OUT_STEEL_TIME"] = bcls_rec->Tables["UPD"].Rows[i]["OUT_STEEL_TIME"].ToString();
							}
							else
							{
								twmsmyry["OUT_STEEL_TIME"] = " ";
							}
							proc_sum += twmsmyry.Update("REC_REVISOR,REC_REVISE_TIME,HEAT_NO,ST_NO,OUT_STEEL_TIME", "HEAT_NO_OLD,FACTORY_2");
							Log::Trace("", __FUNCTION__, "更新2", "");
						}
						else
						{
							//twmsmyry["IDCARD"] = datetime.SubstringNE(0, 8) + Db::QueryCString("SELECT TWMSMYRY_SEQ.NEXTVAL FROM DUAL");
							twmsmyry["REC_CREATE_TIME"] = datetime;
							twmsmyry["REC_CREATOR"] = s.userid;
							twmsmyry["SAP_ERP_PRCSPATH"] = " ";
							twmsmyry["HEAT_NO_OLD"] = twmsmyry["HEAT_NO"];
							proc_sum += twmsmyry.Insert();
							Log::Trace("", __FUNCTION__, "插入2", "");
						}
					}
					Log::Trace("", __FUNCTION__, "这里3", "");

				}
			}
		}

		/************************删除*******************************************/
		if (bcls_rec->Tables.Contains("DEL"))
		{
			if (bcls_rec->Tables["PRO_DIV"].Columns.Contains("PRO_DIV"))
			{
				v_operate = bcls_rec->Tables["PRO_DIV"].Rows[0]["PRO_DIV"].ToString().Trim();
				Log::Trace("", "", "v_operate=[{0}],", v_operate);
			}

			Log::Trace("", "", "删除开始,count=[{0}]", bcls_rec->Tables["DEL"].Rows.get_Count());
			if (v_operate == "TS")
			{
				for (int i = 0; i < bcls_rec->Tables["DEL"].Rows.get_Count(); i++)
				{
					twmsmyryts.Reset();
					twmsmyryts.MergeFrom(bcls_rec->Tables["DEL"].Rows[i]);
					proc_sum += twmsmyryts.Delete("IDCARD");
				}
			}
			else
			{
				for (int i = 0; i < bcls_rec->Tables["DEL"].Rows.get_Count(); i++)
				{
					Log::Trace("", "", "v_operatedel=[{0}],", v_operate);
					twmsmyry.Reset();
					twmsmyry.MergeFrom(bcls_rec->Tables["DEL"].Rows[i]);
					proc_sum += twmsmyry.Delete("HEAT_NO_OLD,FACTORY_2");
				}
			}
		}
		msgstr += msgstr.Format("%d条记录操作成功。", proc_sum);
		strncpy(s.msg, (const char*)msgstr, sizeof(s.msg) - 1);
	}


	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}


