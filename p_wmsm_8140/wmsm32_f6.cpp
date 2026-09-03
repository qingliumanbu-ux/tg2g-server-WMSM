/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2024
Author:      nyy
Version:     1.0
Date:        2024-01-9 13:10:05
Description: 新增非销售出厂用车实绩材料明细
**************************************************/

#include "stdafx.h"
//函数申明
BM2_FUNCTION_IMPORT
int f_wmsm_21a009_snd1(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm99(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
BM2F_ENTERACE(wmsm32_f6)

int f_wmsm32_f6(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString v_datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	//实体类定义
	CModel twmsm32 = CModel("TWMSM32");
	CModel twmsm30 = CModel("TWMSM30");
	CModel twmsm30m = CModel("TWMSM30M");
	CModel twmsm30m_bak = CModel("TWMSM30M");
	CModel twmsm32m = CModel("TWMSM32M");
	CModel tmmsm01 = CModel("TMMSM01");
	CModel tmmsm96 = CModel("TMMSM96");
	CModel twmsm12 = CModel("TWMSM12");
	CModel hwmsm12 = CModel("HWMSM12");

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
		twmsm32.Reset();
		twmsm32.MergeFrom(bcls_rec->Tables[1].Rows[0]);
		twmsm32.TrimOrBlank();
		twmsm32.Query("PLAN_NO,MISSION_NO");
		if (twmsm32["STATUS"].ToString().Trim() != "1"&&twmsm32["STATUS"].ToString().Trim() != "2")
		{
			strcpy(s.msg, _S("此任务号【" + twmsm32["MISSION_NO"].ToString() + "】的状态不为已进厂,装车确认失败!"));
			s.flag = -1;
			doFlag = -1;
			return doFlag;
		}
		if (twmsm32["FACTORY_DIV1"].ToString().Trim() == "")
		{
			strcpy(s.msg, _S("此任务号【" + twmsm32["MISSION_NO"].ToString() + "】的厂别为空,装车确认失败!"));
			s.flag = -1;
			doFlag = -1;
			return doFlag;
		}
		bcls_rec->Tables["21A009"].Rows[0]["DEAL_FLAG"] = "I";
		bcls_rec->Tables["21A009"].Rows[0]["PLAN_NO"] = twmsm32["PLAN_NO"].ToString();
		bcls_rec->Tables["21A009"].Rows[0]["MISSION_NO"] = twmsm32["MISSION_NO"].ToString();
		// 传入块中第一个表的行数
		int rowCount1 = bcls_rec->Tables[0].Rows.get_Count();
		Log::Trace("", "", "rowCount1=[{0}]", rowCount1);
		//新增
		if (rowCount1 >0)
		{
			for (int i = 0; i < rowCount1; i++)
			{
				//将对象字段重置为默认值
				twmsm32m.Reset();

				// 获取前台传入参数
				twmsm32m.MergeFrom(bcls_rec->Tables[0].Rows[i]);
				tmmsm01["MAT_NO"] = twmsm32m["MAT_NO"].ToString().Trim();
				Log::Trace("", "", "111111111111111");
				if (!tmmsm01.Query("MAT_NO"))
				{
					strcpy(s.msg, _S("【" + tmmsm01["MAT_NO"].ToString() + "】此材料不存在，不能装车!"));
					s.flag = -1;
					doFlag = -1;
					return doFlag;
				}
				if (tmmsm01["PRODUCT_FLAG"].ToString() == "1")
				{
					strcpy(s.msg, _S("【" + tmmsm01["MAT_NO"].ToString() + "】此材料不是在制品，不能装车!"));
					s.flag = -1;
					doFlag = -1;
					return doFlag;
				}
				//设置记录者信息
				twmsm32m["REC_CREATOR"] = s.userid;
				twmsm32m["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				CString v_prod_shift_no = "";
				CString v_prod_shift_group = "";
				f_epep_get_shift_group("SMCP", twmsm32m["REC_CREATE_TIME"].ToString(), v_prod_shift_no, v_prod_shift_group, conn);
				twmsm32m["MISSION_NO"] = twmsm32["MISSION_NO"].ToString();
				twmsm32m["PLAN_NO"] = twmsm32["PLAN_NO"].ToString();
				twmsm32m["STATUS"] = " ";
				//twmsm32m["PRACTICE_NO"] = twmsm32["PRACTICE_NO"].ToString();
				twmsm32m["MODEL_SIZE"] = " ";
				twmsm32m["MATERIAL_NAME"] = " ";
				twmsm32m["FACTORY_DIV1"] = twmsm32["FACTORY_DIV1"].ToString();
				twmsm32m["AREA_CODE"] = twmsm32["LOAD_CODE_AREA"].ToString();
				twmsm32m["LOAD_CODE"] = twmsm32["LOAD_CODE"].ToString();
				twmsm32m["PROD_ORDER_NO"] = tmmsm01["ORDER_NO"].ToString();
				twmsm32m["REMARK"] = " ";
				twmsm32m["REMARK1"] = v_prod_shift_group;
				twmsm32m["REMARK2"] = " ";
				twmsm32m["MATERIAL_CODE1"] = " ";
				twmsm32m["STORE_PLACE"] = tmmsm01["STOCK_PLACE_NO"].ToString();
				//验证在表中是否存在
				if (twmsm32m.QueryCount("MAT_NO,MISSION_NO,PLAN_NO") > 0)
				{
					strcpy(s.msg, _S("此计划下的材料号【" + twmsm32m["MAT_NO"].ToString() + "】在表中已经存在, 新增失败!"));
					s.flag = -1;
					doFlag = -1;
					return doFlag;
				}

				//新增记录
				twmsm32m.Insert();
				twmsm30m["PLAN_NO"] = twmsm32m["PLAN_NO"].ToString();
				twmsm30m["MAT_NO"] = twmsm32m["MAT_NO"].ToString();
				twmsm30m["STATUS"] = "3";
				twmsm30m.Update("STATUS", "PLAN_NO,MAT_NO");
				//判断计划车数与实际车数大小
				twmsm30m_bak["PLAN_NO"] = twmsm32m["PLAN_NO"].ToString();
				twmsm30m_bak["STATUS"] = " ";//未装车的
				if (twmsm30m_bak.QueryCount("PLAN_NO,STATUS") <= 0)
				{
					Log::Trace("", "", "SSSSSSS");
					twmsm30["PLAN_NO"] = twmsm30m_bak["PLAN_NO"].ToString();
					twmsm30["STATUS"] = "4";//已完成
					twmsm30.Update("STATUS", "PLAN_NO");
													
				}
				//查询预装车记录，如有则将预装记录归档
				twmsm12["MAT_NO"] = twmsm32m["MAT_NO"].ToString();
				if (twmsm12.QueryCount("MAT_NO")>0) {
					twmsm12.Query("MAT_NO");
					hwmsm12.CopyFrom(twmsm12);
					hwmsm12.Insert();
					twmsm12.Delete("MAT_NO");
				}				
				tmmsm96.Reset();
				tmmsm96.CopyFrom(tmmsm01);
				tmmsm96["LOGISTICS_STATUS"] = "2";//2--装车确认
				tmmsm96["FACTORY_TO"] = "6380";
				tmmsm96["DST_STOCK_CODE"] = "638001";
				tmmsm96["UNLOAD_CODE"] = "638001001";
				tmmsm96["OUT_STOCK_TIME"] = v_datetime;
				tmmsm96["EVENT_ID"] = "MM77";
				tmmsm96["SYSTEM_ID"] = "MMSM";
				tmmsm96["EVENT_LINE_TYPE"] = "00";
				tmmsm96["FUNC_ID"] = s.svc_name;
				tmmsm96.MergeTo(mm0099.Tables["MM0099"], false);
				Log::Info("", __FUNCTION__, "f_mmsm10_trace_linke   =[{0}]", __LINE__);
			}
			twmsm32["REC_REVISOR"] = s.userid;
			twmsm32["REC_REVISE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			twmsm32["STATUS"] = "6";
			twmsm32["UNLOAD_FINISH_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			if (twmsm32["METERAGE_TYPE"].ToString() == "1")//不计量
			{
				//将材料明细的重量累加到主表的实际运量中
				CDbCommand tatpm26(" update twmsm32  t set t.Real_Quantity = (select sum(g.MAT_WT)  from twmsm32m g where g.Mission_No='" + twmsm32["MISSION_NO"].ToString() + "')  where t.Mission_No = '" + twmsm32["MISSION_NO"].ToString() + "'  ", conn);
				tatpm26.ExecuteNonQuery();
				tatpm26.Close();
			}
			twmsm32.Update("REC_REVISE_TIME,REC_REVISOR,STATUS,UNLOAD_FINISH_TIME", "MISSION_NO");

			doFlag = f_wmsm_21a009_snd1(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
			
			doFlag = f_mmsm99(&mm0099, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
			
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


