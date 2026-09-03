/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:      jinquan
Version:     1.1.1
Date:        2016-12-20
Description: 更新命令流水号
**************************************************/

/* C/C++ 的标准头文件部分 */
#include "WM_Utility.h"	// 框架头，不可删除 
//#include "twma7.h"

BM2_FUNCTION_EXPORT
int f_wmsm_craneCmd_seq_update(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	/*数据库操作类定义*/
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_1(conn);
	CDbCommand cmd_inq_2(conn);

	/*定义表实体对象*/
	//CTWMA7   twma7(conn);
	CModel twma7 = CModel("TWMA7");

	/*程序内部变量*/
	int doFlag = 0;
	CString sqlstr = " ";
	CString down_flag = " ";
	CString dateTime = " ";

	CDataTable cmd_update;

	try
	{
		//项目自定义日志
		CTracer log(__FUNCTION__);

		//取系统时间
		dateTime = CDateTime::Now().ToString("yyyyMMddHHmmss");


		sqlstr = "DROP SEQUENCE seqTest";
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq_1.SetCommandText(sqlstr);
		cmd_inq_1.ExecuteNonQuery();
		cmd_inq_1.Close();

		sqlstr = "CREATE SEQUENCE seqTest INCREMENT BY 1 START WITH 1 NOMAXvalue NOCYCLE CACHE 10";
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq_2.SetCommandText(sqlstr);
		cmd_inq_2.ExecuteNonQuery();
		cmd_inq_2.Close();

		sqlstr = "select cmd_seq,mat_no from twma7 order by cmd_seq,yard_layer_from";
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(cmd_update);
		cmd_inq.Close();

		for (int i = 0; i < cmd_update.Rows.get_Count(); i++)
		{
			twma7.Reset();
			twma7["MAT_NO"] = cmd_update.Rows[i]["MAT_NO"];
			twma7["CMD_SEQ"] = atoi(WM_Utility::GetSeqence("seqTest", conn));
			twma7.Update("CMD_SEQ", "MAT_NO");
		}
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };

		/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/
		CMessageFormat::Format(s.msg, "Database processing error. sqlcode=[{0}].", arguments, 1);
		CString str = ex.GetMsg() + "\r\n" + sqlstr;

		/*返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应*/
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);

		/*数据库异常时返回-1，事务将被回滚*/
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

