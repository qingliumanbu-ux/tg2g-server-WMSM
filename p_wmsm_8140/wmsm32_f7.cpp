/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2024
Author:      nyy
Version:     1.0
Date:        2024-01-9 13:10:05
Description: 非销售出厂装车取消，并发送物流系统
**************************************************/

#include "stdafx.h"
//函数申明
BM2_FUNCTION_IMPORT
int f_wmsm_21a009_snd1(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm99(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
BM2F_ENTERACE(wmsm32_f7)

int f_wmsm32_f7(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString plan_no = " ";
	CString v_datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	int affectRow = 0;
	EIClass  output;
	output.Tables.Add();
	//实体类定义
	CModel twmsm30 = CModel("TWMSM30");
	CModel twmsm32 = CModel("TWMSM32");
	CModel twmsm32m = CModel("TWMSM32M");
	CModel twmsm30m = CModel("TWMSM30M");
	CModel twmsm32_bak = CModel("TWMSM32");
	CModel tmmsm96 = CModel("TMMSM96");
	CModel tmmsm01 = CModel("TMMSM01");
	EIClass mm0099;
	mm0099.Tables[0].set_TableName("MM0099");
	mm0099.Tables[0].Columns.Add(tmmsm96);
	mm0099.Tables[0].Rows.Clear();

	try
	{
		if (!bcls_rec->Tables.Contains("21A009"))
		{
			bcls_rec->Tables.Add("21A009");
			bcls_rec->Tables["21A009"].Columns.Add(DT_STRING, "PLAN_NO");
			bcls_rec->Tables["21A009"].Columns.Add(DT_STRING, "MISSION_NO");
			bcls_rec->Tables["21A009"].Columns.Add(DT_STRING, "DEAL_FLAG");
			bcls_rec->Tables["21A009"].Rows.Add();
		}
		int count = bcls_rec->Tables[0].Rows.get_Count();
		Log::Trace("", "", "count=[{0}]", count);


		for (int i = 0; i < count; i++)
		{
			//重置表结构变量
			twmsm32.Reset();
			twmsm32m.Reset();
			twmsm30.Reset();
			twmsm32_bak.Reset();
			// 取得单行传入信息 
			twmsm32.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			twmsm32_bak["MISSION_NO"] = twmsm32["MISSION_NO"].ToString();
			twmsm32_bak["PLAN_NO"] = twmsm32["PLAN_NO"].ToString();
			twmsm32_bak.Query("MISSION_NO,PLAN_NO");
			if (twmsm32_bak["STATUS"].ToString() != "3" && twmsm32_bak["STATUS"].ToString() != "6")
			{
				strcpy(s.msg, _S("此任务号的状态不为已装车或已完成,装车回退失败!"));
				s.flag = -1;
				doFlag = -1;
				return doFlag;
			}

			if (twmsm32_bak["METERAGE_TYPE"].ToString().Trim() != "1" && twmsm32_bak["REAL_QUANTITY"].ToString() > 0)
			{
				strcpy(s.msg, _S("此任务号的计量实绩已经完成,装车回退失败!"));
				s.flag = -1;
				doFlag = -1;
				return doFlag;
			}
			//1、将用车实绩置为已进厂未装车
			twmsm32["STATUS"] = "2";
			twmsm32["UNLOAD_FINISH_TIME"] = " ";
			
			twmsm32.Update("STATUS,UNLOAD_FINISH_TIME", "MISSION_NO");

			//发送汽运进出厂装卸车实绩取消电文
			bcls_rec->Tables["21A009"].Rows[0]["DEAL_FLAG"] = "D";
			bcls_rec->Tables["21A009"].Rows[0]["PLAN_NO"] = twmsm32["PLAN_NO"].ToString();
			bcls_rec->Tables["21A009"].Rows[0]["MISSION_NO"] = twmsm32["MISSION_NO"].ToString();
			doFlag = f_wmsm_21a009_snd1(bcls_rec, bcls_ret, conn);
			Log::Trace("", "", "fffffff");
			Log::Trace("", "", "发送信息  = [{0}] ", doFlag);
			if (doFlag < 0)
			{
				strcpy(s.msg, _S("后台程序错误：调用发送电文21A009错误"));
				s.flag = -1;
				doFlag = -1;
				return doFlag;
			}
			Log::Trace("", "", "affectRow  = [{0}] ", affectRow);

			//如果是不计量的车，将实际完成量置为0
			if (twmsm32["METERAGE_TYPE"].ToString().Trim() == "1")
			{
				twmsm32["REAL_QUANTITY"] = 0;
				twmsm32.Update("REAL_QUANTITY", "MISSION_NO");
				//恢复计划材料状态
				CDbCommand cmd_update(" update    twmsm30m t  set t.Status=' ' where t.Mat_No in(select distinct g.MAT_NO  from twmsm32m g where g.Mission_No='" + twmsm32["MISSION_NO"].ToString() + "')  and t.Plan_No='" + twmsm32["PLAN_NO"].ToString() + "'   ", conn);
				cmd_update.ExecuteNonQuery();
				cmd_update.Close();
				twmsm32m["MISSION_NO"] = twmsm32["MISSION_NO"].ToString();
				twmsm32m.Delete("MISSION_NO");
			}
			//判断对应计划的状态
			twmsm30m["PLAN_NO"] = twmsm32["PLAN_NO"].ToString();
			twmsm30m["STATUS"] = " ";//未装车的
			if (twmsm30m.QueryCount("PLAN_NO,STATUS") > 0)
			{
				twmsm30["PLAN_NO"] = twmsm32["PLAN_NO"].ToString();
				twmsm30.Query("PLAN_NO");

				twmsm30["STATUS"] = "3";//审核
				twmsm30.Update("STATUS", "PLAN_NO");
				twmsm30m.Update("STATUS", "PLAN_NO");
			}
			for (int i = 0; i < bcls_rec->Tables[1].Rows.get_Count(); i++)
			{
				tmmsm96.Reset();
				tmmsm01["MAT_NO"] = bcls_rec->Tables[1].Rows[i]["MAT_NO"].ToString().Trim();
				tmmsm01.Query("MAT_NO");
				tmmsm96.CopyFrom(tmmsm01);
				tmmsm96["LOGISTICS_STATUS"] = "0";//2--装车确认
				tmmsm96["FACTORY_TO"] = " ";
				tmmsm96["DST_STOCK_CODE"] = " ";
				tmmsm96["UNLOAD_CODE"] = " ";
				tmmsm96["OUT_STOCK_TIME"] = " ";
				tmmsm96["EVENT_ID"] = "MM77";
				tmmsm96["SYSTEM_ID"] = "MMSM";
				tmmsm96["EVENT_LINE_TYPE"] = "00";
				tmmsm96["FUNC_ID"] = s.svc_name;
				tmmsm96.MergeTo(mm0099.Tables["MM0099"], false);
				Log::Info("", __FUNCTION__, "f_mmsm10_trace_linke   =[{0}]", __LINE__);
			}
			
		}
		doFlag = f_mmsm99(&mm0099, bcls_ret, conn);
		if (doFlag != 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
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


