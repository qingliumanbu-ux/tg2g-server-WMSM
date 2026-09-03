/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2024
Author:      nyy
Version:     1.0
Date:        2024-01-12 13:44:44
Description: 单表通用保存-信融专用后台
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(wmsm30c_save)


int f_wmsm30c_save(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;

	CString sqlstr = " ";
	CString table_name = " ";
	CString msgstr = "提示信息:";	//提示信息。
	int proc_sum = 0;				//操作总数

	CDbCommand cmd_sql(conn);
	CDataTable temp_table;
	try
	{
		//获取传入参数
		CString  nowTime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		if (!bcls_rec->Tables.Contains("PARA"))
		{
			strcpy(s.msg, "传入数据块PARA不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (!bcls_rec->Tables.Contains("ADD"))
		{
			strcpy(s.msg, "传入数据块ADD不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		table_name = bcls_rec->Tables["PARA"].Rows[0]["TABLE_NAME"].ToString();

		Log::Trace("", "", "获取传入参数...");
		Log::Trace("", "", "传入表名：table_name=[{0}]", table_name);
		Log::Trace("", "", "当前时间：nowTime=[{0}]", nowTime);

		CModel model = CModel(table_name);
		CModel model_bak = CModel(table_name);
		CModel twmsm30m = CModel("TWMSM30M");
		
		///************************新增*******************************************/
		if (bcls_rec->Tables.Contains("ADD"))
		{
			Log::Trace("", "", "新增开始,count=[{0}]", bcls_rec->Tables["ADD"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["ADD"].Rows.get_Count(); i++)
			{
				model.Reset();
				model.MergeFrom(bcls_rec->Tables["ADD"].Rows[i]);
				model["REC_CREATOR"] = s.userid;
				model["REC_CREATE_TIME"] = s.datetime;
				model["PLAN_NO"] = "ATCC6240" + CDateTime::Now().ToString("yyyyMMdd").Substring(2, 6) + EPGetNextSeq("ATCC_SEQNO", conn);
				model["DATE_C"] = CDateTime::Now().ToString("yyyyMMdd");
				model.TrimOrBlank();
				if (model["SUPPLIER_CODE"].ToString().Trim() == "")
				{
					strcpy(s.msg, _S("此计划号【" + model["PLAN_NO"].ToString() + "】的供应商代码不能为空, 新增失败!"));
					s.flag = -1;
					doFlag = -1;
					return doFlag;
				}
				if (model["CARRIER_CODE"].ToString().Trim() == "")
				{
					strcpy(s.msg, _S("此计划号【" + model["PLAN_NO"].ToString() + "】的承运商代码不能为空, 新增失败!"));
					s.flag = -1;
					doFlag = -1;
					return doFlag;
				}
				if (model["LOAD_CODE_FACTORY"].ToString().Trim() == "")
				{
					strcpy(s.msg, _S("此计划号【" + model["PLAN_NO"].ToString() + "】的装点工厂不能为空, 新增失败!"));
					s.flag = -1;
					doFlag = -1;
					return doFlag;
				}
				if (model["LOAD_CODE_AREA"].ToString().Trim() == "")
				{
					strcpy(s.msg, _S("此计划号【" + model["PLAN_NO"].ToString() + "】的装点区域代码不能为空, 新增失败!"));
					s.flag = -1;
					doFlag = -1;
					return doFlag;
				}
				if (model["LOAD_CODE"].ToString().Trim() == "")
				{
					strcpy(s.msg, _S("此计划号【" + model["PLAN_NO"].ToString() + "】的装点代码不能为空, 新增失败!"));
					s.flag = -1;
					doFlag = -1;
					return doFlag;
				}
				if (model["WAGON_NUM"].ToString().Trim() == "")
				{
					strcpy(s.msg, _S("此计划号【" + model["PLAN_NO"].ToString() + "】的计划车数不能为空, 新增失败!"));
					s.flag = -1;
					doFlag = -1;
					return doFlag;
				}
				if (model["QUANTITY"].ToString().Trim() == "")
				{
					strcpy(s.msg, _S("此计划号【" + model["PLAN_NO"].ToString() + "】的计划运量不能为空, 新增失败!"));
					s.flag = -1;
					doFlag = -1;
					return doFlag;
				}
				// 验证在表中是否存在
				if (model.QueryCount("PLAN_NO") > 0)
				{
					strcpy(s.msg, _S("此计划号【" + model["PLAN_NO"].ToString() + "】在表中已经存在, 新增失败!"));
					s.flag = -1;
					doFlag = -1;
					return doFlag;
				}

				model["BUSY_TYPE"] = "1";
				model["STATUS"] = "2";
				model["PRIORITY"] = " "; 
					model["CITY_CODE"] = "000264";
				/*if (model["METERAGE_TYPE"].ToString().Trim() == "3")
				{
				strcpy(s.msg, _S("此计划号【" + model["PLAN_NO"].ToString() + "】的业务类型为委外加工, 计量方式不能为定皮, 新增失败!"));
				s.flag = -1;
				doFlag = -1;
				return doFlag;
				}*/
					sqlstr = "INSERT INTO " + table_name;
					model.Print();
					model.Insert();
					Log::Trace("", "", "新增成功");
			}

			
		}

	
		/************************修改*******************************************/
		if (bcls_rec->Tables.Contains("UPD"))
		{
			Log::Trace("", "", "修改开始,count=[{0}]", bcls_rec->Tables["UPD"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["UPD"].Rows.get_Count(); i++)
			{
				model.Reset();
				model.MergeFrom(bcls_rec->Tables["UPD"].Rows[i]);
				model.TrimOrBlank();
				model.Print();
				model_bak.Reset();
				model_bak.MergeFrom(bcls_rec->Tables["UPD"].Rows[i]);
				model_bak.TrimOrBlank();
				if (!model_bak.Query()){
					////Log::Debug("", __FUNCTION__, "未找到第{0}条记录，无法修改。(是否新增？)", i + 1);
					msgstr += msgstr.Format("未找到第%d条记录，无法修改。", i + 1);
					continue;
				}
				else
				{
					if (model_bak["STATUS"].ToString().Trim() != "2" && model_bak["STATUS"].ToString().Trim() != "1")
					{
						strcpy(s.msg, _S("此计划号的状态不为未审核或开启, 修改失败!"));
						s.flag = -1;
						doFlag = -1;
						return doFlag;
					}
					model_bak.Delete();				
					model["REC_REVISOR"] = s.userid;
					model["REC_REVISE_TIME"] = s.datetime;
					model.TrimOrBlank();
					model.Insert();
				}


			}
		}


		/************************删除*******************************************/
		if (bcls_rec->Tables.Contains("DEL"))
		{
			Log::Trace("", "", "删除开始,count=[{0}]", bcls_rec->Tables["DEL"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["DEL"].Rows.get_Count(); i++)
			{
				model.Reset();
				model.MergeFrom(bcls_rec->Tables["DEL"].Rows[i]);

				//if (model.QueryCount(condition) != 1){
				if (!model.Query("PLAN_NO")){
					////Log::Debug("", __FUNCTION__, "未找到第{0}条记录，无法删除。", i + 1);
					msgstr += msgstr.Format("未找到第%d条记录，无法删除。", i + 1);
					continue;
				}
				else
				{
					if (model["STATUS"].ToString().Trim() != "2" && model["STATUS"].ToString().Trim() != "1")
					{
						strcpy(s.msg, _S("此计划号的状态不为未审核或开启, 删除失败!"));
						s.flag = -1;
						doFlag = -1;
						return doFlag;
					}
					twmsm30m["PLAN_NO"] = model["PLAN_NO"].ToString();
					
					model.Delete("PLAN_NO");
					Log::Trace("", __FUNCTION__, "model删除成功");
					twmsm30m.Delete("PLAN_NO");
					
				}
				

			}
		}

		msgstr += msgstr.Format("%d条记录操作成功。", proc_sum);
		strncpy(s.msg, (const char*)msgstr, sizeof(s.msg) - 1);

	}
	catch (CDbException& ex)  //捕获数据库操作异常 
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。", arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
		////Log::Warn("", __FUNCTION__, "CDbException: {0}", s.msg);
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
		////Log::Error("", __FUNCTION__, "CApplicationException: {0}", ex.GetMsg());
	}
	catch (CException& ex)
	{
		strncpy(s.sysmsg, (const char*)ex.GetMsg(), sizeof(s.sysmsg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
		////Log::Fatal("", __FUNCTION__, "CException: {0}", ex.GetMsg());
	}
	return doFlag;
}


