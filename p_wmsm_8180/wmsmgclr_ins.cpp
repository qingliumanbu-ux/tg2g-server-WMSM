/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         LIZHEN
Version:		1.0
Date:			2023-11-07
Description:
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"
//程序用头文件


//函数申明

/*<remark>=========================================================

===========================================================</remark>*/

BM2F_ENTERACE(wmsmgclr_ins);

int f_wmsmgclr_ins(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	int ret = 0;

	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */

	EPEX epex;

	/* 数据库SQL操作字符串 */
	CString sql = "";
	CString sqlstr = "";
	CString sqlwhere = "";
	CString sqlstr_count;
	CString sqlstr_temp;

	/* 业务变量 */
	CString s_tc_no = " ";
	/* 全局变量 */

	CDbCommand cmd_inq(conn);


	//系统的分页类信息。
	CModel twmsmczts("TWMSMCZTS");

	
	try
	{
		twmsmczts.Reset();
		twmsmczts["GRADE_TYPE2"] = bcls_rec->Tables[0].Rows[0]["GRADE_TYPE2"].ToString();
		twmsmczts["C_DIV"] = bcls_rec->Tables[0].Rows[0]["C_DIV"].ToString();
		twmsmczts["FACTORY_2"] = bcls_rec->Tables[0].Rows[0]["FACTORY_2"].ToString();
		twmsmczts["ST_NO"] = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString();
		if (twmsmczts.QueryCount("GRADE_TYPE2,C_DIV,FACTORY_2,ST_NO")==0)
		{
			sprintf(s.msg, "该钢种[%s]不在配置信息中", (const char*)twmsmczts["GRADE_TYPE2"]);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		twmsmczts.Query("GRADE_TYPE2,C_DIV,FACTORY_2,ST_NO");
		twmsmczts.Delete("GRADE_TYPE2,C_DIV,FACTORY_2,ST_NO");

		sql = " insert into TWMSMCZTS  (GUICHENG,GRADE_TYPE2,C_DIV,FACTORY_2,ST_NO,MEMO_DETAIL,MEMO_DETAIL1,MEMO_DETAIL2,OP_FLAG,REC_CREATOR,REC_CREATE_TIME,REC_REVISOR,REC_REVISE_TIME) "
			" values  ( :GUICHENG,:GRADE_TYPE2,:C_DIV,:FACTORY_2,:ST_NO,:MEMO_DETAIL,:MEMO_DETAIL1,:MEMO_DETAIL2,:OP_FLAG,:REC_CREATOR,:REC_CREATE_TIME,:REC_REVISOR,:REC_REVISE_TIME ) ";
		Log::Trace("", __FUNCTION__, "LINKE=[{0}]", sql);
		cmd_inq.SetCommandText(sql);
		cmd_inq.Parameters.Set("GUICHENG", bcls_rec->Tables[0].Rows[0]["GUICHENG"].ToString());
		cmd_inq.Parameters.Set("GRADE_TYPE2", bcls_rec->Tables[0].Rows[0]["GRADE_TYPE2"].ToString());
		cmd_inq.Parameters.Set("C_DIV", bcls_rec->Tables[0].Rows[0]["C_DIV"].ToString());
		cmd_inq.Parameters.Set("FACTORY_2", bcls_rec->Tables[0].Rows[0]["FACTORY_2"].ToString());
		cmd_inq.Parameters.Set("ST_NO", bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString());
		cmd_inq.Parameters.Set("MEMO_DETAIL", twmsmczts["MEMO_DETAIL"].ToString());
		cmd_inq.Parameters.Set("MEMO_DETAIL1", twmsmczts["MEMO_DETAIL1"].ToString());
		cmd_inq.Parameters.Set("MEMO_DETAIL2", twmsmczts["MEMO_DETAIL2"].ToString());
		cmd_inq.Parameters.Set("OP_FLAG", twmsmczts["OP_FLAG"].ToString());
		cmd_inq.Parameters.Set("REC_CREATOR", twmsmczts["REC_CREATOR"].ToString());
		cmd_inq.Parameters.Set("REC_CREATE_TIME", twmsmczts["REC_CREATE_TIME"].ToString());
		cmd_inq.Parameters.Set("REC_REVISOR", twmsmczts["REC_REVISOR"].ToString());
		cmd_inq.Parameters.Set("REC_REVISE_TIME", twmsmczts["REC_REVISE_TIME"].ToString());
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();


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