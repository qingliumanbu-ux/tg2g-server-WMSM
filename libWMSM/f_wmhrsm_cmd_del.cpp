/* **************************************************************************
*	Copyright (c) Baosight Corporation 2008 . All Rights Reserved.
*  	BM2PES 宝信生产执行系统
*****************************************************************************
*  程序名称			: f_wm_cmd_delete
*  程序描述			: 指令删除函数
*  备注说明			:
*  修改历史			:
*  		2020-09-19 仓库产品化			(ADD)程序建立
*			... ...
* **************************************************************************** */
/* ***************************传入参数********************************
传入块名：WM_CMD
MAT_NO                     材料号                  非空
* **************************************************************************** */
/* C/C++ 的标准头文件部分 */
#include "stdafx.h"		// 框架头，不可删除 
//垛位最大高度、重量修正
int f_wm00_pileinfocal(CString stock_no, CString stock_place_no, EIClass * bcls_ret, CDbConnection * conn);

BM2_FUNCTION_EXPORT
int f_wmsmsm_cmd_del(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	/* ***** 程序变量 ***** */
	int doFlag = 0;
	CString v_datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString v_mat_no = "";
	CString s_shift_group = " ", s_shift_no = " ", s_operate_time = " ";
	CDataTable table_mat;
	/* ***** 数据库SQL操作字符串 ***** */
	CString	sqlstr = "";
	CModel tpshra7 = CModel("TPSHRA7");
	CModel twma7 = CModel("TWMA7");
	CModel twma7_old = CModel("TWMA7");
	CModel hwm00a7 = CModel("HWM00A7");
	CModel twma2 = CModel("TWMA2");
	CModel twma7up = CModel("TWMA7");

	
	/* ***** 应用程序开始处理 ***** */
	try
	{
		if (!bcls_rec->Tables.Contains("WM_CMD")){
			sprintf(s.msg, "函数f_wm_cmd_make中找不到接收块名[WM_CMD]");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (bcls_rec->Tables["WM_CMD"].Rows.get_Count() == 0){
			Log::Trace("", __FUNCTION__, "函数f_wm_cmd_make中传入数据为空");
			return doFlag;
		}
		//炼钢按块删除
		if (bcls_rec->Tables["WM_CMD"].Columns.Contains("MAT_NUM") && bcls_rec->Tables["WM_CMD"].Rows[0]["MAT_NUM"].ToString() != "0" && bcls_rec->Tables["WM_CMD"].Rows[0]["MAT_NUM"].ToString().Trim() != "")
		{
			if (!bcls_rec->Tables["WM_CMD"].Columns.Contains("MAIN_MAT_NO"))
			{
				sprintf(s.msg, "按块删除请传入组吊号");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			sqlstr = "select * from twma7  where MAIN_MAT_NO='"+ bcls_rec->Tables["WM_CMD"].Rows[0]["MAIN_MAT_NO"].ToString() +"' ORDER BY CMD_SEQ ";
			Db::QueryTable(sqlstr, table_mat);
			for (int j = 0; j < bcls_rec->Tables["WM_CMD"].Rows[0]["MAT_NUM"].ToDouble(); j++)
			{
				twma7_old.Reset();
				twma7_old["MAT_NO"] = v_mat_no;
				twma7_old.Query("MAT_NO");
				hwm00a7.CopyFrom(twma7_old);
				hwm00a7["REC_ERASE_TIME"] = v_datetime;
				hwm00a7["REC_ERASOR"] = s.userid;
				hwm00a7["SVC_NAME"] = s.svc_name;
				hwm00a7["REMARK"] = "f_wm_cmd_delete";
				if (twma7_old["COMPANY_CODE"].ToString().Trim() != "")
				{
					f_epep_get_shift_group("DEFAULT", twma7_old["COMPANY_CODE"].ToString().Trim(), s_shift_no, s_shift_group, conn);
				}
				hwm00a7["SHIFT_NO"] = s_shift_no;
				hwm00a7["SHIFT_GROUP"] = s_shift_group;
				hwm00a7.Insert();
				/*tpshra7.Reset();
				tpshra7["CMD_STATUS"] = "0";
				tpshra7["IN_MAT_NO"] = twma7["MAT_NO"].ToString();
				tpshra7.Update("CMD_STATUS", "IN_MAT_NO");*/
				twma7_old.Delete("MAT_NO");
				doFlag = f_wm00_pileinfocal(twma7_old["MAT_NO"].ToString(), twma7_old["STOCK_PLACE_NO_TO"].ToString(), bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
		}
		else
		{
			for (int i = 0; i < bcls_rec->Tables["WM_CMD"].Rows.get_Count(); i++)
			{
				v_mat_no = bcls_rec->Tables["WM_CMD"].Rows[i]["MAT_NO"].ToString();
				twma7_old.Reset();
				twma7_old["MAT_NO"] = v_mat_no;
				twma7_old.Query("MAT_NO");
				hwm00a7.CopyFrom(twma7_old);
				hwm00a7["REC_ERASE_TIME"] = v_datetime;
				hwm00a7["REC_ERASOR"] = s.userid;
				hwm00a7["SVC_NAME"] = s.svc_name;
				hwm00a7["REMARK"] = "f_wm_cmd_delete";

				if (v_mat_no.Trim() == "")
				{
					sprintf(s.msg, "函数f_wm_cmd_make中传入值为空");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (twma7_old["COMPANY_CODE"].ToString().Trim() != "")
				{
					f_epep_get_shift_group("DEFAULT", twma7_old["COMPANY_CODE"].ToString().Trim(), s_shift_no, s_shift_group, conn);
				}
				hwm00a7["SHIFT_NO"] = s_shift_no;
				hwm00a7["SHIFT_GROUP"] = s_shift_group;
				hwm00a7.Insert();
				tpshra7.Reset();
				tpshra7["CMD_STATUS"] = "0";
				tpshra7["IN_MAT_NO"] = twma7["MAT_NO"].ToString();
				tpshra7.Update("CMD_STATUS", "IN_MAT_NO");
				twma7.Reset();
				twma7["MAT_NO"] = v_mat_no;
				twma7.Delete("MAT_NO");
				if (twma7_old["STOCK_NO"].ToString().Trim() == "H10")
				{
					sqlstr = "SELECT STOCK_PLACE_POSITION_FR,STOCK_PLACE_POSITION_TO,MAT_NO,CMD_METHOD,STOCK_PLACE_NO_TO FROM TWMA7 WHERE  MAIN_MAT_NO='" + v_mat_no + "' AND MAT_NO !='" + v_mat_no + "'";
					Log::Trace("", __FUNCTION__, "sqlstr[{0}]", sqlstr);
					Db::QueryTable(sqlstr, table_mat);
					for (int j = 0; j < table_mat.Rows.get_Count(); j++)
					{

						hwm00a7.CopyFrom(twma7up);
						hwm00a7["REC_ERASE_TIME"] = v_datetime;
						hwm00a7["REC_ERASOR"] = s.userid;
						hwm00a7["SVC_NAME"] = s.svc_name;
						hwm00a7.Insert();
						twma7up.Reset();
						twma7up["MAT_NO"] = table_mat.Rows[j]["MAT_NO"].ToString();
						twma7up.Delete("MAT_NO");
						doFlag = f_wm00_pileinfocal("H10", table_mat.Rows[j]["STOCK_PLACE_NO_TO"].ToString(), bcls_ret, conn);
						if (doFlag < 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}

					}
				}
				doFlag = f_wm00_pileinfocal(twma7_old["STOCK_NO"].ToString(), twma7_old["STOCK_PLACE_NO_TO"].ToString(), bcls_ret, conn);
				Log::Trace("", __FUNCTION__, "STOCK_NO = [{0}]", twma7_old["STOCK_NO"].ToString());
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
				Log::Trace("", __FUNCTION__, "STOCK_NO = [{0}]", twma7_old["STOCK_PLACE_NO_TO"].ToString());
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

