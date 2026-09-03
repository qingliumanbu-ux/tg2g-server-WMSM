/* **************************************************************************
*	Copyright (c) Baosight Corporation 2008 . All Rights Reserved.
*  	BM2PES 宝信生产执行系统
*****************************************************************************
*  程序名称			: f_wm_cmd_follow
*  程序描述			: 指令跟踪函数
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
BM2_FUNCTION_IMPORT    //发送电文函数
//int f_wm_send(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
//垛位最大高度、重量修正
int f_wm00_pileinfocal(CString stock_no, CString stock_place_no, EIClass * bcls_ret, CDbConnection * conn);
BM2_FUNCTION_EXPORT
int f_wmsmsm_cmd_follow(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	/* ***** 程序变量 ***** */
	int doFlag = 0;
	CString v_datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString v_mat_no = "";

	/* ***** 数据库SQL操作字符串 ***** */
	CString	sqlstr = "";

	CModel twma7 = CModel("TWMA7");
	CModel twm04 = CModel("TWM04");
	CModel twm04_now = CModel("TWM04");
	CModel twm04_from = CModel("TWM04");
	CModel twma7_old = CModel("TWMA7");
	CModel hwm00a7 = CModel("HWM00A7");

	CString tc_to = ""; //电文号
	EIClass bcls_send;
	if (!bcls_send.Tables.Contains("SEND"))
	{
		bcls_send.Tables[0].set_TableName("SEND");
		bcls_send.Tables[0].Columns.Add(DT_STRING, "CT_NO");
		bcls_send.Tables[0].Columns.Add(DT_STRING, "COIL_NO");
		bcls_send.Tables[0].Columns.Add(DT_STRING, "LOCATION_FROM");
		bcls_send.Tables[0].Columns.Add(DT_STRING, "LOCATION_TO");
		bcls_send.Tables[0].Columns.Add(DT_STRING, "CY_JOB_KIND");
		bcls_send.Tables[0].Columns.Add(DT_STRING, "MESSAGE_ID");
		bcls_send.Tables["SEND"].Rows.Add();
	}
	EIClass bcls_rec_send;//
	bcls_rec_send.Tables[0].set_TableName("SEND");
	bcls_rec_send.Tables[0].Columns.Add(DT_STRING, "TASK_NO");
	bcls_rec_send.Tables[0].Columns.Add(DT_STRING, "OPERATE_FLAG");
	bcls_rec_send.Tables[0].Columns.Add(DT_STRING, "OLD_STOCK_PLACE_NO");
	bcls_rec_send.Tables[0].Columns.Add(DT_STRING, "MOVE_TYPE");
	bcls_rec_send.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
	bcls_rec_send.Tables[0].Columns.Add(DT_STRING, "TO_STOCK_PLACE_NO");
	bcls_rec_send.Tables[0].Columns.Add(DT_STRING, "SEND_FLAG");
	bcls_rec_send.Tables[0].Columns.Add(DT_STRING, "CT_NO");
	bcls_rec_send.Tables["SEND"].Rows.Add();

	EIClass bcls_send_make;//新增热轧
	if (!bcls_send_make.Tables.Contains("SEND"))
	{
		bcls_send_make.Tables[0].set_TableName("SEND");
		bcls_send_make.Tables[0].Columns.Add(DT_STRING, "SLAB_NO");
		bcls_send_make.Tables[0].Columns.Add(DT_STRING, "TABLE_NO");
		bcls_send_make.Tables[0].Columns.Add(DT_STRING, "PILE_NO_SY");
		bcls_send_make.Tables[0].Columns.Add(DT_STRING, "CRANE_NO");
		bcls_send_make.Tables[0].Columns.Add(DT_STRING, "TABLE_NO_SY");
		bcls_send_make.Tables[0].Columns.Add(DT_STRING, "SPARE01");
		bcls_send_make.Tables[0].Columns.Add(DT_STRING, "COIL_NO");
		bcls_send_make.Tables[0].Columns.Add(DT_STRING, "SPARE");
		bcls_send_make.Tables[0].Columns.Add(DT_STRING, "LOCATION_FROM");
		bcls_send_make.Tables[0].Columns.Add(DT_STRING, "LOCATION_TO");
		bcls_send_make.Tables[0].Columns.Add(DT_STRING, "CY_JOB_KIND");
		bcls_send_make.Tables[0].Columns.Add(DT_STRING, "PRO_ID");
		bcls_send_make.Tables[0].Columns.Add(DT_STRING, "DIRECT");
		bcls_send_make.Tables[0].Columns.Add(DT_STRING, "PLAN_NO");
		bcls_send_make.Tables[0].Columns.Add(DT_STRING, "MESSAGE_ID");
		bcls_send_make.Tables[0].Columns.Add(DT_STRING, "X_AXIS_PILE");
		bcls_send_make.Tables[0].Columns.Add(DT_STRING, "Y_AXIS_PILE");
		bcls_send_make.Tables[0].Columns.Add(DT_STRING, "X1_AXIS_ACT");
		bcls_send_make.Tables[0].Columns.Add(DT_STRING, "Y1_AXIS_ACT");
		bcls_send_make.Tables[0].Columns.Add(DT_STRING, "Z1_AXIS_ACT");
		bcls_send_make.Tables[0].Columns.Add(DT_STRING, "X2_AXIS_ACT");
		bcls_send_make.Tables[0].Columns.Add(DT_STRING, "Y2_AXIS_ACT");
		bcls_send_make.Tables[0].Columns.Add(DT_STRING, "Z2_AXIS_ACT");
		bcls_send_make.Tables[0].Columns.Add(DT_STRING, "CT_NO");
		bcls_send_make.Tables[0].Columns.Add(DT_STRING, "TC_NO");
		bcls_send_make.Tables[0].Columns.Add(DT_STRING, "TC");
		bcls_send_make.Tables[0].Columns.Add(DT_STRING, "ROWS");		
		bcls_send_make.Tables["SEND"].Rows.Add();
	}
	/* ***** 应用程序开始处理 ***** */
	try
	{
		/*if (!bcls_rec->Tables.Contains("WM_CMD")){
			sprintf(s.msg, "函数f_wm_cmd_follow中找不到接收块名[WM_CMD]");
			throw CApplicationException(-1, s.msg, log.Location);
		}*/
		Log::Trace("", __FUNCTION__, "函数中传入数据为空ccccccc");
		if (bcls_rec->Tables[0].Rows.get_Count() == 0){
			Log::Trace("", __FUNCTION__, "函数f_wm_cmd_follow中传入数据为空");
		
		}
		CString sevename = s.svc_name;
		Log::Trace("", __FUNCTION__, "sevename\t[{0}]", sevename);
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{

			v_mat_no = bcls_rec->Tables[0].Rows[i]["MAT_NO"].ToString();
			if (v_mat_no.Trim() == "")
			{
				sprintf(s.msg, "函数f_wm_cmd_follow中传入值为空");
				continue;
			} 
		
			twma7_old.Reset();
			twma7_old["MAT_NO"] = v_mat_no;
			
		   
		   
		       hwm00a7.CopyFrom(twma7_old);
		       hwm00a7["REC_ERASE_TIME"] = v_datetime;
		       hwm00a7["REC_ERASOR"] = s.userid;
		       hwm00a7["SVC_NAME"] = s.svc_name; 
		   	   hwm00a7["REMARK"] = "f_wm_cmd_follow";
		       hwm00a7.Insert();
		       twma7.Reset();
		       twma7["MAT_NO"] = v_mat_no;
		       twma7.Delete("MAT_NO");
		   }
		   doFlag = f_wm00_pileinfocal(twma7_old["STOCK_NO"].ToString(), twma7_old["STOCK_PLACE_NO_TO"].ToString(), bcls_ret, conn);
		   if (doFlag < 0)
		   {
		   	throw CApplicationException(-1, s.msg, log.Location);
		   }
			if (v_mat_no.Trim() != "")
			{
				if (twma7_old["STOCK_NO"].ToString().Trim() != "")
				{
					doFlag = f_wm00_pileinfocal(twma7_old["STOCK_NO"].ToString(), twma7_old["STOCK_PLACE_NO_TO"].ToString(), bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
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

