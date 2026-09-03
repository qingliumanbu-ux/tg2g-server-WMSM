/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         nyy
Version:		1.0
Date:			2024-01-05
Description:	临钢坯材料查询
**************************************************/

//框架头文件
#include "stdafx.h"
//#include "smhs.h"
//程序用头文件
//#include "twma1.h"
//#include "twma0.h"

//函数申明


BM2F_ENTERACE(wmsm30c_mat_inq);

int f_wmsm30c_mat_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString stock_oper_order = "";
	CString stock_no = "";
	CString heat_no = "";
	CString guide_dest = "";
	CString mat_no = "";
	CString transfer_plan_no = "";//add by ljnie 2016/6/17 13:55:31 增加转库计划号
	CString mat_no_array = "";
	CString s_userid("");
	CDecimal d_thick_fr = 0;
	CDecimal d_thick_to = 0;

	CDecimal rowCount = 0;
	CDecimal rowSum = 0;
	int fetchRowCount = 0;

	int NOW_NUM = 0;
	int RETURN_NUM = -1;////每页记录数量
	int INDEX_FROM = 0;//页数

	/* 实体类定义 */
	
	CModel tmmsm01 = CModel("TMMSM01");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	//系统的分页类信息。
	

	try
	{

		// 获取前台传入参数 
		s_userid = s.userid;

		tmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsm01.TrimOrBlank();


		if (bcls_rec->Tables[0].Columns.Contains("MAT_NO"))
			mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString();
		guide_dest = bcls_rec->Tables[0].Rows[0]["GUIDE_DEST"].ToString();
		/* ***** 打印输入参数 ***** */
		
		Log::Trace("", __FUNCTION__, "mat_no=[{0}]", mat_no);	

		sqlstr = "select * from tmmsm01 t where  (t.in_flag = '1' or t.UNLOAD_CODE like 'WXK1%') and  t.GUIDE_DEST IN (select CODE from TWMSMZD02 where CODE_CLASS = 'WM02' AND CODE_DESC_3_CONTENT LIKE '%4%')";
		if (tmmsm01["MAT_NO"].ToString().Trim() != "")
		{			
			sqlstr += " AND t.MAT_NO LIKE @mat_no";
		}
		if (tmmsm01["GUIDE_DEST"].ToString().Trim() != "")
		{
			sqlstr += " AND t.GUIDE_DEST = @guide_dest";
		}
		if (tmmsm01["HEAT_NO"].ToString().Trim() != "")
		{
			sqlstr += " AND t.HEAT_NO LIKE '%" + tmmsm01["HEAT_NO"].ToString() + "%'";
		}
		sqlstr += " and not exists( select 2  from twmsm30m t2 where  exists (select 1  from twmsm30 t3 where "; 
		sqlwhere +=	" t2.mat_no = t.mat_no and t3.plan_no = t2.plan_no and t3.status < '4')) order by REC_CREATE_TIME";

		
		
			//"select t.* from tmmsm01 t where t.in_flag=1 and t.guide_dest like '%临钢%' ";
			
		sqlstr = sqlstr + sqlwhere;
		Log::Trace("", __FUNCTION__, "sqlstr[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);		
		cmd_inq.Parameters.Set("mat_no", "%" + mat_no + "%");
		cmd_inq.Parameters.Set("guide_dest", guide_dest);
		
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();
		//返回记录总数
	
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
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

