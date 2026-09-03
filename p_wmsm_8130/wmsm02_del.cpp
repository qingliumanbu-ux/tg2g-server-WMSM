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
int f_wm1900d8_snd(char* in_mat_no, char* in_cause, char* in_user);

BM2F_ENTERACE(wmsm02_del)

int f_wmsm02_del(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	/*打印程序起止日志*/
	CTracer log(__FUNCTION__);
	Log::Debug("", __FUNCTION__, "wmsm02_del--------开始");
	/*定义公用变量*/
	int doFlag = 0;
	CString  sqlstr = "";
	CString  sql = "";
	CString  dateNow14 = CDateTime::Now().ToString("yyyyMMddHHmmss");  //14位日期变量

	CString mat_no = "";
	CString cause = "";
	//定义实体类变量
	CModel twmm1("TWMM1");
	CModel tmmsm01("TMMSM01");
	CDbCommand cmd_inq(conn);
	CDbCommand cmd(conn);

	if (bcls_rec->Tables[0].Columns.Contains("MAT_NO"))
		mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim();
	
		cause = bcls_rec->Tables[0].Rows[0]["CAUSE"].ToString().Trim();
	Log::Trace("", __FUNCTION__, "twmm1.CAUSE[{0}]", cause);

	//doFlag = f_wm1900d8_snd(tmmsm01.mat_no, cause, v_userid);

	/*if (doFlag != 0)
	{
		EDLog(1, 1, "1900d8发送失败.");
		doFlag = -1;
		goto l_return;
	}*/

	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			twmm1.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			if (bcls_rec->Tables[0].Columns.Contains("CAUSE"))
				cause = bcls_rec->Tables[0].Rows[0]["CAUSE"].ToString().Trim();

			Log::Trace("", __FUNCTION__, "i[{0}]", i);
			Log::Trace("", __FUNCTION__, "twmm1.MAT_NO[{0}]", twmm1["MAT_NO"].ToString());
			Log::Trace("", __FUNCTION__, "twmm1.CAUSE[{0}]", cause);
			twmm1.Reset();
			twmm1.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			twmm1.Delete("MAT_NO");
		}
		//	switch (conn->DatabaseKind)
		//	{
		//	case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		//	case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//	case DB_KIND_MSSQL:	        // MS SQL Server数据库
		//	case DB_KIND_ORACLE:	        // Oracle 数据库
		//		sqlstr = "SELECT COUNT(1) FROM TWM000C WHERE STOCK_NO =@stock_no AND STOCK_OPER_ORDER = @stock_oper_order AND UNIT_CODE = @unit_code AND SEQ_NO = @seq_no ";
		//		break;
		//	}
		//	sqlstr = sqlstr + sqlwhere;
		//	cmd_inq.SetCommandText(sqlstr);
		//	cmd_inq.Parameters.Set("stock_no", twmm1["STOCK_NO"].ToString());
		//	cmd_inq.Parameters.Set("stock_oper_order", twmm1["STOCK_OPER_ORDER"].ToString());
		//	cmd_inq.Parameters.Set("unit_code", twmm1["UNIT_CODE"].ToString());
		//	cmd_inq.Parameters.Set("seq_no", twmm1["SEQ_NO"].ToDecimal());
		//	Count = cmd_inq.ExecuteScalar();

		//	if (Count < 1)
		//	{
		//		sprintf(s.msg, "业务步骤配置信息不存在，无需删除。");
		//		throw CApplicationException(-1, s.msg, log.Location);
		//	}

		//	twmm1.Delete("MAT_NO,STOCK_OPER_ORDER,UNIT_CODE,SEQ_NO");
		//}
		Log::Debug("", __FUNCTION__, "wmsm02_del--------结束");
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
