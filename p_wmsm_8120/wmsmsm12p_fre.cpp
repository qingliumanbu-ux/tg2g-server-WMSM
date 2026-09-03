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

BM2F_ENTERACE(wmsmsm12p_fre)


int f_wmsmsm12p_fre(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
	CDbCommand cmd(conn);
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
		CString arr = "";
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			arr += bcls_rec->Tables[0].Rows[i]["LOAD_SCHEME_NO"].ToString() + "','";
		}
		if (arr.GetLength() > 3) {
			arr = arr.Substring(0, arr.GetLength() - 3);
		}

		EIClass tmp;
		sql = " select distinct a.TRUCK_NO,a.PLAN_NO\
			from twmsm12 a,\
			tmmsm01 b\
		where a.MAT_NO = b.MAT_NO\
		and a.LOAD_SCHEME_NO in('" + arr + "') ";
		cmd.SetCommandText(sql);
		cmd.ExecuteQuery(tmp.Tables[0]);
		cmd.Close();
		if (tmp.Tables[0].Rows.get_Count() > 1)
		{
			sprintf(s.msg, "所选预装车单不是同一计划号或车号，请重新选择");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		sqlstr = " select b.MAT_NO,\
			B.ST_NO,			   \
			B.MAT_LINE_TYPE,	   \
			B.HOT_SEND_FLAG,	   \
			B.HEAT_NO,			   \
			B.MAT_ACT_THICK,	   \
			B.MAT_ACT_WIDTH,	   \
			B.MAT_ACT_LEN,		   \
			B.MAT_ACT_WT,		   \
			B.LAYERNO,			   \
			B.MAT_STATUS,		   \
			B.GUIDE_DEST,		   \
			B.SURF_QUALITY,		   \
			B.PONO,				   \
			A.PLAN_NO,			   \
			A.LOAD_CODE_FACTORY,   \
			A.LOAD_CODE_AREA,	   \
			A.LOAD_CODE,		   \
			A.UNLOAD_CODE_FACTORY, \
			A.UNLOAD_CODE_AREA,	   \
			A.UNLOAD_CODE,\
			B.USAGE_DECISION,\
			A.OUT_STOCK_TIME,	   \
			A.TRNP_MODE_CODE,	   \
			A.TRUCK_NO,A.LOAD_SCHEME_NO,B.C_DELIVERYID,b.order_no	   \
			from twmsm12 a,\
			tmmsm01 b\
		where a.MAT_NO = b.MAT_NO\
		and a.LOAD_SCHEME_NO  in ('" + arr + "')";

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
