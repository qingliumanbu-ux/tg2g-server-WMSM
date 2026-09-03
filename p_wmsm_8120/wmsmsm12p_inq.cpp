/*************************************************
版权: Baosight Software LTD.co Copyright (c) 2012
创建者：lizhen
功能:
修改历史:
日期:________;修改人:________; 需求提出人:________
变更内容:
**************************************************/

/*框架头文件*/
#include "stdafx.h" 
#include "epex.h"

BM2F_ENTERACE(wmsmsm12p_inq)


int f_wmsmsm12p_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	/*打印程序起止日志*/
	CTracer log(__FUNCTION__);
	Log::Debug("", __FUNCTION__, "wmsmsmj3_rcm--------开始");
	/*定义公v_mat_no用变量*/
	int doFlag = 0;
	CString  sqlstr = "";
	CString  sql = "";
	CString v_mat_no, v_crane_no = "";
	CString  dateNow14 = CDateTime::Now().ToString("yyyyMMddHHmmss");  //14位日期变量

	//定义实体类
	CModel twmsm12("TWMSM12");

	CDbCommand cmd_inq(conn);

	/*出库时间*/
	CString out_stock_time("");
	/*运输方式*/
	CString trnp_mode_code("");
	/*卡车号*/
	CString truck_no("");
	/*装车任务号*/
	CString load_scheme_no("");

	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	//EIClass bcls_temp;

	try
	{
		sqlstr = " select distinct LOAD_SCHEME_NO,\
			PLAN_NO,\
			LOAD_CODE_FACTORY,\
			LOAD_CODE_AREA,\
			LOAD_CODE,\
			UNLOAD_CODE_AREA,\
			UNLOAD_CODE,\
			UNLOAD_CODE_FACTORY,\
			OUT_STOCK_TIME,\
			TRNP_MODE_CODE,\
			SHIFT_GROUP,OPERATOR,\
			TRUCK_NO, FN_R_REFCUR(LOAD_SCHEME_NO) COMPANY_CODE \
			from TWMSM12 where 1=1  ";
		for (int i = 0; i < bcls_rec->Tables[0].Columns.get_Count(); i++)
		{
			if (bcls_rec->Tables[0].Columns[i].get_DataType() == DT_STRING
				&& bcls_rec->Tables[0].Rows[0][i].ToString().Trim().IsEmpty())
			{
				continue;
			}
			if (bcls_rec->Tables[0].Columns[i].get_DataType() == DT_DECIMAL
				&& bcls_rec->Tables[0].Rows[0][i].ToDecimal() == 0)
			{
				continue;
			}
			if (bcls_rec->Tables[0].Columns[i].get_ColumnName()=="LOAD_UP_TIME_F"
				|| bcls_rec->Tables[0].Columns[i].get_ColumnName() == "LOAD_UP_TIME_T")
			{
				continue;
			}
			sql += " AND " + bcls_rec->Tables[0].Columns[i].get_ColumnName() + " LIKE @" + bcls_rec->Tables[0].Columns[i].get_ColumnName() + "||'%' ";
			cmd_inq.Parameters.Set(bcls_rec->Tables[0].Columns[i].get_ColumnName(), bcls_rec->Tables[0].Rows[0][i].ToString().Trim());
			Log::Trace("", __FUNCTION__, "b		= [{0}][{1}]", bcls_rec->Tables[0].Columns[i].get_ColumnName(),bcls_rec->Tables[0].Rows[0][i].ToString().Trim());
		}
		if (bcls_rec->Tables[0].Columns.Contains("LOAD_UP_TIME_F")&&bcls_rec->Tables[0].Rows[0]["LOAD_UP_TIME_F"].ToString().Trim()!="")
		{
			sql += " AND OUT_STOCK_TIME >= '" + bcls_rec->Tables[0].Rows[0]["LOAD_UP_TIME_F"].ToString() + "' ";
		}
		if (bcls_rec->Tables[0].Columns.Contains("LOAD_UP_TIME_T") && bcls_rec->Tables[0].Rows[0]["LOAD_UP_TIME_T"].ToString().Trim() != "")
		{
			sql += " AND OUT_STOCK_TIME <= '" + bcls_rec->Tables[0].Rows[0]["LOAD_UP_TIME_T"].ToString() + "' ";
		}
		sql += " ORDER BY out_stock_time DESC ";
		sqlstr += sql;
		Log::Trace("", __FUNCTION__, "sqlcount[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();

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
