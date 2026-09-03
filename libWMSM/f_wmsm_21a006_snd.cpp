/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      05501061
Version:     1.0
Date:        2023-11-22 10:45:20
Description: 倒运计划发送
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2_FUNCTION_EXPORT


int f_wmsm_21a006_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
CTracer log(__FUNCTION__);
 int doFlag = 0;
 int blkNum = 0;
 CString sqlstr = " ";
 CString deal_flag = " ";
 //电文号
 CString cs_tc_no("");
 //电文变量
 EPEX epex(&s, conn);
 /*实体类定义*/
 CModel twmsm60("TWMSM60");
 try
 {
	 blkNum = bcls_rec->Tables.IndexOf("TWMSM60");	 
	 if (blkNum < 0)
	 {
		 strcpy(s.msg, "传入数据块TWMSM60不存在。");
		 throw CApplicationException(-1, s.msg, s.svc_name);
	 }
	
	 cs_tc_no = "21A006";
	 if (epex.Initialize(cs_tc_no) < 0)
	 {
		 sprintf(s.msg, "电文初始化失败[%s]", epex.GetMsg());
		 throw CApplicationException(-1, s.msg, s.svc_name);
	 }
	 for (int i = 0; i < bcls_rec->Tables["TWMSM60"].Rows.get_Count(); i++)
	 {
		 
		 twmsm60.MergeFrom(bcls_rec->Tables["TWMSM60"].Rows[i]);
		 twmsm60.Query("PLAN_NO");
		
		 if (epex.SetValue("ZCHO_GPDYJH",0, twmsm60) < 0)
		 {
			 sprintf(s.msg, "发送电文失败，原因[%s]", epex.GetMsg());
			 throw CApplicationException(-1, s.msg, s.svc_name);
		 }
		 
		 if (epex.SendTele() < 0)
		 {
			 sprintf(s.msg, "电文发送失败");
			 throw CApplicationException(-1, s.msg, s.svc_name);
		 }
		
	 }
	 epex.Uninitialize();
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


