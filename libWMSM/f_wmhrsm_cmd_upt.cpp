/* **************************************************************************
*	Copyright (c) Baosight Corporation 2008 . All Rights Reserved.
*  	BM2PES 宝信生产执行系统
*****************************************************************************
*  程序名称			: f_wm_cmd_update
*  程序描述			: 指令修改函数
*  备注说明			:
*  修改历史			:
*  		2022-09-19 仓库产品化			(ADD)程序建立
*			... ...
* **************************************************************************** */
/* ***************************传入参数********************************
传入块名：WM_CMD
MAT_NO                     材料号                  非空
* **************************************************************************** */
/* C/C++ 的标准头文件部分 */
#include "stdafx.h"		// 框架头，不可删除 

BM2_FUNCTION_EXPORT
int f_wmsmsm_cmd_upt(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	/* ***** 程序变量 ***** */
	int doFlag = 0;
	CString v_datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString v_mat_no = "";
	CString stock_oper_order = "";
	CString cmd_method = "";
	/* ***** 数据库SQL操作字符串 ***** */
	CString	sqlstr = "";

	CModel twma7 = CModel("TWMA7");
	CModel twm04_from = CModel("TWM04");
	CModel twm04_to = CModel("TWM04");
	CModel twma2 = CModel("TWMA2");
	/* ***** 应用程序开始处理 ***** */
	try
	{

		if (!bcls_rec->Tables.Contains("WM_CMD")){
			sprintf(s.msg, "函数f_wm_cmd_update中找不到接收块名[WM_CMD]");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (bcls_rec->Tables["WM_CMD"].Rows.get_Count() == 0){
			Log::Trace("", __FUNCTION__, "函数f_wm_cmd_make中传入数据为空");
			return doFlag;
		}

		for (int i = 0; i < bcls_rec->Tables["WM_CMD"].Rows.get_Count(); ++i)
		{
			twma7.Reset();
			twma7["MAT_NO"] = bcls_rec->Tables["WM_CMD"].Rows[i]["MAT_NO"].ToString();
			Log::Trace("", __FUNCTION__, "MAT_NO = [{0}]", twma7["MAT_NO"].ToString());
			if (twma7.Query("MAT_NO"))
			{
				twma7["REC_REVISE_TIME"] = v_datetime;
				twma7["STOCK_PLACE_NO_TO"] = bcls_rec->Tables["WM_CMD"].Rows[i]["STOCK_PLACE_NO_TO"].ToString();
				twma7["STOCK_PLACE_NO_FROM"] = bcls_rec->Tables["WM_CMD"].Rows[i]["STOCK_PLACE_NO_FROM"].ToString();
				twma7["CMD_METHOD"] = bcls_rec->Tables["WM_CMD"].Rows[i]["STOCK_OPER_ORDER"].ToString();
				/*if (bcls_rec->Tables["WM_CMD"].Columns.Contains("PRO_ID") && bcls_rec->Tables["WM_CMD"].Rows[i]["PRO_ID"].ToString().Trim() != "")
				{
					twma7["CRANE_INST_STATUS"] = bcls_rec->Tables["WM_CMD"].Rows[i]["PRO_ID"].ToString().Trim();
				}*/
				Log::Trace("", __FUNCTION__, "STOCK_PLACE_NO_TO = [{0}]", twma7["STOCK_PLACE_NO_TO"].ToString());
				Log::Trace("", __FUNCTION__, "STOCK_PLACE_NO_FROM = [{0}]", twma7["STOCK_PLACE_NO_FROM"].ToString());
				Log::Trace("", __FUNCTION__, "CMD_METHOD = [{0}]", twma7["CMD_METHOD"].ToString());
				//Log::Trace("", __FUNCTION__, "cy_job_kind = [{0}]", twma7["STOCK_PLACE_NO_TO"]);
				//twma7["REC_REVISE_TIME"] = v_datetime;
				twm04_from.Reset();
				twm04_from["STOCK_PLACE_NO"] = twma7["STOCK_PLACE_NO_TO"];
				twm04_to.Reset();
				twm04_to["STOCK_PLACE_NO"] = twma7["STOCK_PLACE_NO_TO"];
				stock_oper_order = twma7["STOCK_OPER_ORDER"];
				if (!twm04_to.Query("STOCK_PLACE_NO"))
				{
					sprintf(s.msg, "[%s]twm04_to位置不存在。", (const char*)twm04_to["STOCK_PLACE_NO"].ToString());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (!twm04_from.Query("STOCK_PLACE_NO"))
				{
					sprintf(s.msg, "[%s]twm04_from位置不存在。", (const char*)twm04_from["STOCK_PLACE_NO"].ToString());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				twma7["STOCK_PLACE_POSITION_FR"] = twm04_from["STOCK_PLACE_NO_1"].ToString();
				twma7["STOCK_PLACE_POSITION_TO"] = twm04_to["STOCK_PLACE_NO_1"].ToString();
				twma7.Update("REC_REVISE_TIME,STOCK_PLACE_NO_TO,STOCK_PLACE_NO_FROM,CMD_METHOD,STOCK_PLACE_POSITION_FR,STOCK_PLACE_POSITION_TO,CRANE_INST_STATUS","MAT_NO");
			}
			else
			{
				sprintf(s.msg, "请刷新页面，该命令已删除或执行完毕"); //只能修改倒跺或入库的目标库位
				throw CApplicationException(-1, s.msg, log.Location);
			}

		}
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CString str = ex.GetMsg() + "\r\n" + sqlstr;
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), 399);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), 399);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}

