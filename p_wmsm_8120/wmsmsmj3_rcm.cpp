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

BM2F_ENTERACE(wmsmsmj3_rcm)

int f_wmsmsmj3_rcm(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	/*打印程序起止日志*/
	CTracer log(__FUNCTION__);
	Log::Debug("", __FUNCTION__, "wmsmsmj3_rcm--------开始");
	/*定义公v_mat_no用变量*/
	int doFlag = 0;
	CString  sqlstr = "";
	CString  sql = "";
	CString v_mat_no,v_crane_no = "";
	CString  dateNow14 = CDateTime::Now().ToString("yyyyMMddHHmmss");  //14位日期变量

	//定义实体类变量
	CModel tmmsm01 = CModel("TMMSM01");
	CModel twm04 = CModel("TWM04");

	CDbCommand cmd_inq(conn);
	CDbCommand cmd(conn);

	//EIClass bcls_temp;

	try
	{
		v_mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim();
		v_crane_no = bcls_rec->Tables[0].Rows[0]["CRANE_NO"].ToString().Trim();
		
		tmmsm01.Reset();
		tmmsm01["MAT_NO"] = v_mat_no;
		if (!tmmsm01.Query("MAT_NO")){
			sprintf(s.msg, "材料" + v_mat_no + "不存在。");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (tmmsm01["STOCK_PLACE_NO"].ToString().Trim() == ""){
			sprintf(s.msg, "材料[%s]垛位查询失败。", (const char*)v_mat_no);
			throw CApplicationException(-1, s.msg, log.Location);
		}
	
		twm04["STOCK_PLACE_NO"] = tmmsm01["STOCK_PLACE_NO"];
		if (!twm04.Query("STOCK_PLACE_NO")){
			sprintf(s.msg, "垛位[%s]没有再垛位属性表中维护。", (const char*)tmmsm01["STOCK_PLACE_NO"].ToString());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (twm04["HALL_NO"].ToString().Trim() == ""){
			sprintf(s.msg, "板坯[%s]垛位[%s]跨号查询失败.", v_mat_no, tmmsm01["STOCK_PLACE_NO"]);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		sqlstr = "select crane_no from twm06 where hall_no = decode('" + twm04["HALL_NO"].ToString().Trim() + "', 'S00', hall_no, '" + twm04["HALL_NO"].ToString().Trim() + "') and crane_no = decode('" + v_crane_no + "', '', crane_no, null, crane_no, '" + v_crane_no + "') order by crane_no";
		cmd_inq.Close();
		cmd_inq.SetCommandText(sqlstr);
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		int count = cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		bcls_ret->Tables[0].Rows.Add();
		bcls_ret->Tables.Add();
		bcls_ret->Tables[1].Columns.Add(DT_STRING, "STOCK_PLACE_NO");
		bcls_ret->Tables[1].Columns.Add(DT_STRING, "LAYERNO");
		bcls_ret->Tables[1].Columns.Add(DT_STRING, "STOCK_PLACE_NO_TO");
		bcls_ret->Tables[1].Columns.Add(DT_STRING, "CRANE_NO");
		if (2== bcls_ret->Tables[0].Rows.get_Count())//只有一行
		{
			bcls_ret->Tables[1].Rows.Add();
			bcls_ret->Tables[1].Rows[0]["STOCK_PLACE_NO"] = tmmsm01["STOCK_PLACE_NO"];
			bcls_ret->Tables[1].Rows[0]["CRANE_NO"] = bcls_ret->Tables[0].Rows[0]["CRANE_NO"];
			bcls_ret->Tables[1].Rows[0]["STOCK_PLACE_NO_TO"] = "A101";
			bcls_ret->Tables[1].Rows[0]["LAYERNO"] = " ";
		}

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
