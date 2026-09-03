/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         JINQUAN
Version:		1.0
Date:			2016-07-27
Description:	吊车命令卸下
**************************************************/

//框架头文件
#include "stdafx.h"


//程序用头文件

//函数申明
BM2_FUNCTION_IMPORT


/*<remark>=========================================================
///<summary>
///吊车命令卸下
///<para>
///2.排序方式：
///</para>
///<para>
===========================================================</remark>*/

BM2F_ENTERACE(wmsmsma7_dwon);

int f_wmsmsma7_dwon(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	/* 程序内部变量 */
	int	doFlag = 0;
	int ret = 0;

	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString sqlstr = "";
	//实体类
	CModel twma7("TWMA7");
	CModel hwm00a7("HWM00A7");
	CDbCommand cmd(conn);
	try
	{


		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			twma7.Reset();
			twma7.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			//更新板坯命令标记
			//if (0 == strcmp(tymsm10.comd_div, "2"))//针对板坯中转作业的行车命令
			//{
			//	EXEC SQL update tymsm30 set cmd_flag = '0' where end_flag = '0' and mat_no = :tymsm10.mat_no;//行车命令标志，1：命令中，0：无命令    
			//}
			//else
			//{
			sqlstr = "update twm04 set pre_mat_num = pre_mat_num - 1 where stock_place_no = @stock_place_no";//更新目标垛位板坯预约块数
			cmd.Close();
			cmd.SetCommandText(sqlstr);
			Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			cmd.Parameters.Set("stock_place_no", twma7["STOCK_PLACE_NO_TO"].ToString());
			cmd.ExecuteNonQuery();

			sqlstr = "update tmmsm01 set cmd_flag = '0' where mat_no = @mat_no";;//行车命令标志，1：命令中，0：无命令
			cmd.Close();
			cmd.SetCommandText(sqlstr);
			Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			cmd.Parameters.Set("mat_no", twma7["MAT_NO"].ToString());
			cmd.ExecuteNonQuery();

			//}
			//更新吊车命令
			twma7["CRANE_INST_STATUS"] = "2";
			twma7.Update("CRANE_INST_STATUS","MAT_NO,CRANE_INST_CODE");

			//更新源垛位状态
			sqlstr = "update twm04 set STOCK_STATUS='0' where stock_place_no = @stock_place_no"
				" AND (SELECT COUNT(*) FROM TWMA7 WHERE STOCK_PLACE_NO_FROM=@stock_place_no AND CRANE_INST_STATUS='0')=0";//更新目标垛位板坯预约块数
			cmd.Close();
			cmd.SetCommandText(sqlstr);
			Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			cmd.Parameters.Set("stock_place_no", twma7["STOCK_PLACE_NO_FROM"].ToString());
			cmd.ExecuteNonQuery();
		
			//归档
			hwm00a7.Reset();
			hwm00a7.CopyFrom(twma7);
			hwm00a7["SVC_NAME"] = s.svc_name;
			hwm00a7["CLIENT_IP"] = s.fore_ip;
			hwm00a7["REC_ERASOR"] = s.userid;
			hwm00a7["REC_ERASE_TIME"] = datetime;
			hwm00a7.Insert();

			twma7.Delete("MAT_NO,CRANE_INST_CODE");
				;
		}
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "Database processing error，sqlcode = [{0}]." /*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
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
