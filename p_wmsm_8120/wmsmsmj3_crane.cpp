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

int f_create_crane_no(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
BM2F_ENTERACE(wmsmsmj3_crane)

int f_wmsmsmj3_crane(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	/*打印程序起止日志*/
	CTracer log(__FUNCTION__);
	Log::Debug("", __FUNCTION__, "wmsmsmj3_crane--------开始");
	/*定义公用变量*/
	int doFlag = 0;
	CString  sqlstr = "";
	CString  sql = "";
	CString  dateNow14 = CDateTime::Now().ToString("yyyyMMddHHmmss");  //14位日期变量


	CDbCommand cmd_inq(conn);
	CDbCommand cmd(conn);

	EIClass bcls_crane;
	bcls_crane.Tables[0].Columns.Add(DT_STRING, "MAT_NO");//材料号
	bcls_crane.Tables[0].Columns.Add(DT_STRING, "CRANE_NO");//行车号
	bcls_crane.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO_FROM");//源垛位
	bcls_crane.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO_TO");//目标垛位
	bcls_crane.Tables[0].Columns.Add(DT_STRING, "MOVE_TYPE");//移动类型-I入库O出库 D倒垛
	bcls_crane.Tables[0].Rows.Add();

	try
	{
		Log::Debug("", __FUNCTION__, "f_create_crane_no------------------------------开始");
		bcls_crane.Tables[0].Rows[0]["MAT_NO"] = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim();
		bcls_crane.Tables[0].Rows[0]["CRANE_NO"] = bcls_rec->Tables[1].Rows[0]["CRANE_NO"].ToString().Trim();
		bcls_crane.Tables[0].Rows[0]["STOCK_PLACE_NO_FROM"] = bcls_rec->Tables[1].Rows[0]["STOCK_PLACE_NO_FROM"].ToString().Trim();
		bcls_crane.Tables[0].Rows[0]["STOCK_PLACE_NO_TO"] = bcls_rec->Tables[1].Rows[0]["STOCK_PLACE_NO_TO"].ToString().Trim();
		bcls_crane.Tables[0].Rows[0]["MOVE_TYPE"] = bcls_rec->Tables[1].Rows[0]["MOVE_TYPE"].ToString().Trim();

		Log::Trace("", __FUNCTION__, "MAT_NO = [{0}]", bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim());
		Log::Trace("", __FUNCTION__, "CRANE_NO = [{0}]", bcls_rec->Tables[1].Rows[0]["CRANE_NO"].ToString().Trim());
		Log::Trace("", __FUNCTION__, "STOCK_PLACE_NO = [{0}]", bcls_rec->Tables[0].Rows[0]["STOCK_PLACE_NO"].ToString().Trim());
		Log::Trace("", __FUNCTION__, "STOCK_PLACE_NO_TO = [{0}]", bcls_rec->Tables[0].Rows[0]["STOCK_PLACE_NO_TO"].ToString().Trim());
		Log::Trace("", __FUNCTION__, "MOVE_TYPE = [{0}]", bcls_rec->Tables[1].Rows[0]["MOVE_TYPE"].ToString().Trim());
		doFlag = f_create_crane_no(&bcls_crane, bcls_ret, conn);
		if (doFlag != 0)
		{
			s.flag = -1;
			doFlag = -1;
			throw CApplicationException(doFlag, s.msg, s.svc_name);
		}
		Log::Debug("", __FUNCTION__, "f_create_crane_no------------------------------结束");
		
		Log::Debug("", __FUNCTION__, "wmsmsmj3_crane--------结束");
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
