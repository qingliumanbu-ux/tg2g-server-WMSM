/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         KE2111
Version:		1.0
Date:			2023-12-1
Description:	临钢坯调拨反馈
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件

//函数申明
int f_mmsm99(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wmsm_t80ry0_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);


BM2F_ENTERACE(wmsmsm12_db_feedback);

int f_wmsmsm12_db_feedback(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */

	CModel twm41dj("TWM41DJ");
	CModel tmmsm01("TMMSM01");
	CModel tmmsm96("TMMSM96");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlstr1 = "";
	CString sqlwhere = "";
	CString s_userid("");
	CString sqlstr_count;
	CString sqlstr_temp;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_con(conn);



	/* 业务变量 */
	EIClass mm0099;
	mm0099.Tables[0].set_TableName("MM0099");
	mm0099.Tables[0].Columns.Add(tmmsm96);
	mm0099.Tables[0].Rows.Clear();


	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++) {
			twm41dj.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			if (twm41dj["C_DELIVERYID"].ToString().Trim() == "")
			{
				sprintf(s.msg, "调拨单号不能为空");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			twm41dj["C_STATESIGN"] = "2";
			twm41dj.Update("C_STATESIGN", "C_DELIVERYID");
			
			tmmsm01.Query("MAT_NO");
			//16、调调拨事件
			tmmsm96.Reset();
			tmmsm96.CopyFrom(tmmsm01);
			
			tmmsm96["C_STATESIGN"] = "3";//1--正向调拨出库，3--正向调拨完成
			
			tmmsm96["C_DELIVERYID"] = twm41dj["C_DELIVERYID"];
		
			tmmsm96["EVENT_ID"] = "MM76";
			tmmsm96["SYSTEM_ID"] = "MMSM";
			tmmsm96["EVENT_LINE_TYPE"] = "00";
			tmmsm96["FUNC_ID"] = s.svc_name;
			tmmsm96.MergeTo(mm0099.Tables["MM0099"], false);

			if (mm0099.Tables["MM0099"].Rows.get_Count() > 0)
			{
				doFlag = f_mmsm99(&mm0099, bcls_ret, conn);
				if (doFlag != 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			if (twm41dj["C_STATESIGN"].ToString() == "2")
			{
				EIClass dbsq;
				dbsq.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
				dbsq.Tables[0].Columns.Add(DT_STRING, "C_DELIVERYID");
				dbsq.Tables[0].Rows.Add();
				dbsq.Tables[0].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"];
				dbsq.Tables[0].Rows[0]["C_DELIVERYID"] = twm41dj["C_DELIVERYID"];
				doFlag = f_wmsm_t80ry0_snd(&dbsq, bcls_ret, conn);
				if (doFlag != 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}

		}


	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "Database processing error，sqlcode = [{0}]." /* _RES("GCRSS0000006")*//*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
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