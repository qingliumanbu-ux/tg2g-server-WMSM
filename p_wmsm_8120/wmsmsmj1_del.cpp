/*************************************************
版权: Baosight Software LTD.co Copyright (c) 2012

功能: 
修改历史:
日期:________;修改人:________; 需求提出人:________
变更内容:
**************************************************/

/*框架头文件*/
#include "stdafx.h" 
#include "epex.h"

BM2F_ENTERACE(wmsmsmj1_del)

int f_wmsmsmj1_del(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	/*打印程序起止日志*/
	CTracer log(__FUNCTION__);
	Log::Debug("", __FUNCTION__, "wmsmsmj1_del--------开始");
	/*定义公用变量*/
	int doFlag = 0;
	CString  sqlstr = "";
	CString  sql = "";
	CString  dateNow14 = CDateTime::Now().ToString("yyyyMMddHHmmss");  //14位日期变量

	//定义实体类变量
	CModel twmj1 = CModel("TWMJ1");

	CDbCommand cmd_inq(conn);
	CDbCommand cmd(conn);

	

	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			twmj1.Reset();
			twmj1.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			twmj1.Delete("MAT_NO");

		}

		Log::Debug("", __FUNCTION__, "wmsmsmj1_del--------结束");
	}

	catch (CDbException& ex)         //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006"), arguments, 1); /*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;                  //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)//捕获应用错误
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
