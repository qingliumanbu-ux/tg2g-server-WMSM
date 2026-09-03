/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      nyy
Version:     1.0
Date:        2024-01-13 13:07:54
Description: 非销售出厂用车实绩查询（用车实绩是接收临钢坯配车反馈时自动生成的）
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(wmsm32_inq)


int f_wmsm32_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	int affectRow = 0;
	try
	{
		// -----Begin IPLAT4C::IPLAT4CServiceCompositeStatementObj()----- //
		// -----End IPLAT4C::IPLAT4CServiceCompositeStatementObj()----- //
/*
		int INDEX_FROM = bcls_rec->Tables[0].Rows[0]["INDEX_FROM"].ToDecimal().ToInt32();
		int RETURN_NUM = bcls_rec->Tables[0].Rows[0]["RETURN_NUM"].ToDecimal().ToInt32();
		int RECORD_TOTAL = 0;*/

		//获取输入参数
		CString PLAN_NO = "";
		CString MISSION_NO = "";
		CString TRUCK_NO = "";
		CString LOAD_CODE_FACTORY = "";
		CString STATUS = "";
		CString START_TIME = "";
		CString END_TIME = "";
		CString LOAD_CODE_AREA = "";
		CString LOAD_CODE = "";
		CString FACTORY_DIV = "";
		CString MAT_NO = "";
		if (bcls_rec->Tables[0].Columns.Contains("MISSION_NO"))
			MISSION_NO = bcls_rec->Tables[0].Rows[0]["MISSION_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("PLAN_NO"))
			PLAN_NO = bcls_rec->Tables[0].Rows[0]["PLAN_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("TRUCK_NO"))
			TRUCK_NO = bcls_rec->Tables[0].Rows[0]["TRUCK_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("LOAD_CODE_FACTORY"))
			FACTORY_DIV = bcls_rec->Tables[0].Rows[0]["LOAD_CODE_FACTORY"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("STATUS"))
			STATUS = bcls_rec->Tables[0].Rows[0]["STATUS"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("START_TIME"))
			START_TIME = bcls_rec->Tables[0].Rows[0]["START_TIME"].ToString().Trim();

		if (bcls_rec->Tables[0].Columns.Contains("END_TIME"))
			END_TIME = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("LOAD_CODE_AREA"))
			LOAD_CODE_AREA = bcls_rec->Tables[0].Rows[0]["LOAD_CODE_AREA"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("LOAD_CODE"))
			LOAD_CODE = bcls_rec->Tables[0].Rows[0]["LOAD_CODE"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("FACTORY_DIV"))//厂别
			LOAD_CODE_FACTORY = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("MAT_NO"))
			MAT_NO = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim();
		CDbCommand comm1(conn);

		CString sql = "select t.*,t1.WAGON_NUM,t1.mat_wt  from twmsm32 t left join (select count(*) WAGON_NUM,sum(a.mat_wt) mat_wt,a.mission_no from TWMSM32M a group by a.mission_no) t1 on  t.mission_no=t1.mission_no where  t.BUSY_TYPE='1'  ";

		if (!START_TIME.Trim().IsEmpty())
		{
			sql += " and t.CREATION_TIME  >=@START_TIME  ";
			comm1.Parameters.Set("START_TIME", START_TIME.Substring(0, 8));
		}

		if (!END_TIME.Trim().IsEmpty())
		{
			sql += " and t.CREATION_TIME <=@END_TIME  ";
			comm1.Parameters.Set("END_TIME", END_TIME.Substring(0, 8));

		}
		Log::Trace("", "", "MISSION_NO=[{0}]", MISSION_NO);

		if (!MISSION_NO.Trim().IsEmpty())
		{
			sql += " and t.MISSION_NO like  '%' ||@MISSION_NO|| '%'  ";
			comm1.Parameters.Set("MISSION_NO", MISSION_NO);

		}
		if (!TRUCK_NO.Trim().IsEmpty())
		{
			sql += " and t.TRUCK_NO like  '%' ||@TRUCK_NO|| '%' ";
			comm1.Parameters.Set("TRUCK_NO", TRUCK_NO);
		}
		if (!PLAN_NO.Trim().IsEmpty())
		{
			sql += " and t.PLAN_NO like  '%' ||@PLAN_NO|| '%' ";
			comm1.Parameters.Set("PLAN_NO", PLAN_NO);
		}
		if (!LOAD_CODE_FACTORY.Trim().IsEmpty())
		{
			sql += " and t.LOAD_CODE_FACTORY =@LOAD_CODE_FACTORY ";
			comm1.Parameters.Set("LOAD_CODE_FACTORY", LOAD_CODE_FACTORY);
		}
		if (!STATUS.Trim().IsEmpty())
		{
			sql += " and t.STATUS =@STATUS ";
			comm1.Parameters.Set("STATUS", STATUS);
		}
		if (!LOAD_CODE_AREA.Trim().IsEmpty())
		{
			sql += " and t.AREA_CODE =@LOAD_CODE_AREA ";
			comm1.Parameters.Set("LOAD_CODE_AREA", LOAD_CODE_AREA);
		}
		if (!LOAD_CODE.Trim().IsEmpty())
		{
			sql += " and t.ULPLACE =@LOAD_CODE ";
			comm1.Parameters.Set("LOAD_CODE", LOAD_CODE);
		}
		if (!FACTORY_DIV.Trim().IsEmpty())
		{
			sql += " and t.FACTORY_DIV =@FACTORY_DIV ";
			comm1.Parameters.Set("FACTORY_DIV", FACTORY_DIV);
		}
		if (!MAT_NO.Trim().IsEmpty())
		{
			sql += " and t.MISSION_NO in (select distinct t1.MISSION_NO  from  twmsm32m t1 where t1.mat_no like '%' ||@MAT_NO || '%') ";
			comm1.Parameters.Set("MAT_NO", MAT_NO);
		}
		sql += "  order by t.CREATION_TIME  ";
		Log::Trace("", "", "sql=[{0}]", sql);
		comm1.SetCommandText(sql);
		affectRow = comm1.ExecuteQuery(bcls_ret->Tables[0]);
		//分页备用 返回影响的行数，返回结果将写入bcls_ret->Tables[0]
		//affectRow = comm1.ExecuteQuery(bcls_ret->Tables[0], INDEX_FROM, RETURN_NUM);
		////获取总行数
		//Log::Trace("", "", "affectRowsrrrr");
		//bcls_ret->Tables.Add();
		//CDbCommand comm2(conn);
		//Log::Trace("", "", "ggggggg");
		//if (!START_TIME.Trim().IsEmpty())
		//{

		//	comm2.Parameters.Set("START_TIME", START_TIME.Substring(0, 8));
		//}

		//if (!END_TIME.Trim().IsEmpty())
		//{

		//	comm2.Parameters.Set("END_TIME", END_TIME.Substring(0, 8));

		//}
		//Log::Trace("", "", "MISSION_NO=[{0}]", MISSION_NO);

		//if (!MISSION_NO.Trim().IsEmpty())
		//{

		//	comm2.Parameters.Set("MISSION_NO", MISSION_NO);

		//}
		//if (!TRUCK_NO.Trim().IsEmpty())
		//{

		//	comm2.Parameters.Set("TRUCK_NO", TRUCK_NO);
		//}
		//if (!PLAN_NO.Trim().IsEmpty())
		//{

		//	comm2.Parameters.Set("PLAN_NO", PLAN_NO);
		//}
		//if (!FACTORY_DIV.Trim().IsEmpty())
		//{

		//	comm2.Parameters.Set("FACTORY_DIV", FACTORY_DIV);
		//}
		//if (!STATUS.Trim().IsEmpty())
		//{

		//	comm2.Parameters.Set("STATUS", STATUS);
		//}

		//if (!LOAD_CODE_AREA.Trim().IsEmpty())
		//{
		//	comm2.Parameters.Set("LOAD_CODE_AREA", LOAD_CODE_AREA);
		//}
		//if (!LOAD_CODE.Trim().IsEmpty())
		//{
		//	comm2.Parameters.Set("LOAD_CODE", LOAD_CODE);
		//}
		//if (!LOAD_CODE_FACTORY.Trim().IsEmpty())
		//{

		//	comm2.Parameters.Set("LOAD_CODE_FACTORY", LOAD_CODE_FACTORY);
		//}
		//if (!MAT_NO.Trim().IsEmpty())
		//{

		//	comm2.Parameters.Set("MAT_NO", MAT_NO);
		//}
		//comm2.SetCommandText(sql);
		//Log::Trace("", "", "hhhhh");
		//int affectRows = comm2.ExecuteQuery(bcls_ret->Tables[1]);
		//Log::Trace("", "", "affectRows  = [{0}] ", affectRows);
		//comm2.Close();

		//定义表名
		/*bcls_ret->Tables[0].set_TableName("MATPM45");*/
	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}


