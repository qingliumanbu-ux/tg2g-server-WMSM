/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2020
Author:      nyy
Version:     1.0
Date:        2024-01-10 15:43:15
Description: 查询
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(wmsm30c_inq)


int f_wmsm30c_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	int  affectRow = 0;
	try
	{
		// -----Begin IPLAT4C::IPLAT4CServiceCompositeStatementObj()----- //
		// -----End IPLAT4C::IPLAT4CServiceCompositeStatementObj()----- //
		/*int INDEX_FROM = bcls_rec->Tables[0].Rows[0]["INDEX_FROM"].ToDecimal().ToInt32();
		int RETURN_NUM = bcls_rec->Tables[0].Rows[0]["RETURN_NUM"].ToDecimal().ToInt32();*/
		int RECORD_TOTAL = 0;

		//获取输入参数
		CString PLAN_NO = "";
		CString START_TIME = "";
		CString END_TIME = "";
		CString CONSIGN_NAME = "";
		CString YCDW = "";
		CString BUSY_TYPE = "";
		CString STATUS = "";
		CString LOAD_CODE_FACTORY = "";
		CString LOAD_CODE_AREA = "";
		CString LOAD_CODE = "";
		CString FACTORY_DIV = "";
		CString MAT_NO = "";
		if (bcls_rec->Tables[0].Columns.Contains("START_TIME"))
			START_TIME = bcls_rec->Tables[0].Rows[0]["START_TIME"].ToString();

		if (bcls_rec->Tables[0].Columns.Contains("END_TIME"))
			END_TIME = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().Trim();
		
		if (bcls_rec->Tables[0].Columns.Contains("PLAN_NO"))
			PLAN_NO = bcls_rec->Tables[0].Rows[0]["PLAN_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("YCDW"))
			YCDW = bcls_rec->Tables[0].Rows[0]["YCDW"].ToString().Trim();

		if (bcls_rec->Tables[0].Columns.Contains("STATUS"))
			STATUS = bcls_rec->Tables[0].Rows[0]["STATUS"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("LOAD_CODE_FACTORY"))//厂别
			FACTORY_DIV = bcls_rec->Tables[0].Rows[0]["LOAD_CODE_FACTORY"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("LOAD_CODE_AREA"))
			LOAD_CODE_AREA = bcls_rec->Tables[0].Rows[0]["LOAD_CODE_AREA"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("LOAD_CODE"))
			LOAD_CODE = bcls_rec->Tables[0].Rows[0]["LOAD_CODE"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("FACTORY_DIV"))//装点工厂
			LOAD_CODE_FACTORY = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("MAT_NO"))
			MAT_NO = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim();
		Log::Trace("", "", "START_TIME=[{0}]", START_TIME);
		Log::Trace("", "", "END_TIME=[{0}]", END_TIME);
		//根据计划号依据任务实绩更改对应计划的车次与已完成量
		CModel twmsm30("TWMSM30");
		CModel twmsm30m("TWMSM30M");
		twmsm30.Reset();
		twmsm30m.Reset();
		
		//下面逻辑暂时保留
		CDbCommand tatpm26(" update twmsm30 g set g.RUN_TRAINID= (select count(*)  from  twmsm32 t where t.Plan_No=g.plan_no and  t.Status='6') where g.BUSY_TYPE='1'", conn);
		tatpm26.ExecuteNonQuery();
		tatpm26.Close();

		////不计量完成量
		CDbCommand tatpm26_1(" update twmsm30 g set g.FINISH_WEIGHT= (select sum(t.MAT_WT)  from  twmsm30m t where t.Plan_No=g.plan_no  and t.Status<>' ' )  where  "
			" g.Plan_No in(select distinct t.Plan_No  from  twmsm30m t  left outer join twmsm30 k on k.Plan_No = t.Plan_No  where  t.Status<>' ' and  k.Meterage_Type = '1')  ", conn);
		tatpm26_1.ExecuteNonQuery();
		tatpm26_1.Close();
		//////计量的完成量
		//CDbCommand tatpm26_2(" update twmsm30 g set g.FINISH_WEIGHT= (select  sum(t.Real_Quantity)   from  twmsm32 t where t.Plan_No=g.Plan_No  )  where  g.Plan_No in(select distinct h.Plan_No  from  twmsm30 h  where h.Busy_Type='1' and h.Meterage_Type<>'1' and  h.Status='6')  ", conn);
		//tatpm26_2.ExecuteNonQuery();
		//tatpm26_2.Close();

		CDbCommand comm1(conn);
		CString sql = " select t.* from TWMSM30  t where  1=1 and BUSY_TYPE='1' ";

		if (!START_TIME.Trim().IsEmpty())
		{
			sql += " and t.DATE_C  >=@START_TIME  ";
			comm1.Parameters.Set("START_TIME", START_TIME.Substring(0, 8));
		}

		if (!END_TIME.Trim().IsEmpty())
		{
			sql += " and t.DATE_C <=@END_TIME  ";
			comm1.Parameters.Set("END_TIME", END_TIME.Substring(0, 8));

		}
		if (!CONSIGN_NAME.Trim().IsEmpty())
		{
			sql += " and t.CONSIGN_NAME like '%' ||@CONSIGN_NAME||'%' ";
			comm1.Parameters.Set("CONSIGN_NAME", CONSIGN_NAME);
		}
		if (!PLAN_NO.Trim().IsEmpty())
		{
			sql += " and t.PLAN_NO like '%' ||@PLAN_NO||'%' ";
			comm1.Parameters.Set("PLAN_NO", PLAN_NO);
		}
		if (!YCDW.Trim().IsEmpty())
		{
			sql += " and t.YCDW like '%' ||@YCDW||'%'";
			comm1.Parameters.Set("YCDW", YCDW);
		}

		if (!STATUS.Trim().IsEmpty())
		{
			sql += " and t.STATUS =@STATUS ";
			comm1.Parameters.Set("STATUS", STATUS);
		}
		if (!LOAD_CODE_FACTORY.Trim().IsEmpty())
		{
			sql += " and t.LOAD_CODE_FACTORY =@LOAD_CODE_FACTORY ";
			comm1.Parameters.Set("LOAD_CODE_FACTORY", LOAD_CODE_FACTORY);
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
			sql += " and t.PLAN_NO in (select distinct t41.PLAN_NO  from  tatpm41 t41 where t41.mat_no like '%' ||@MAT_NO || '%') ";
			comm1.Parameters.Set("MAT_NO", MAT_NO);
		}
			
		sql += "  order by t.DATE_C  ";
		Log::Trace("", "", "sql=[{0}]", sql);
		comm1.SetCommandText(sql);

		//返回影响的行数，返回结果将写入bcls_ret->Tables[0]
		affectRow = comm1.ExecuteQuery(bcls_ret->Tables[0]);
		//获取总行数
		Log::Trace("", "", "affectRowsrrrr");
		bcls_ret->Tables.Add();
		CDbCommand comm2(conn);
		Log::Trace("", "", "ggggggg");
		if (!START_TIME.Trim().IsEmpty())
		{

			comm2.Parameters.Set("START_TIME", START_TIME.Substring(0, 8));
		}

		if (!END_TIME.Trim().IsEmpty())
		{

			comm2.Parameters.Set("END_TIME", END_TIME.Substring(0, 8));

		}
		if (!CONSIGN_NAME.Trim().IsEmpty())
		{

			comm2.Parameters.Set("CONSIGN_NAME", CONSIGN_NAME);
		}
		if (!PLAN_NO.Trim().IsEmpty())
		{

			comm2.Parameters.Set("PLAN_NO", PLAN_NO);
		}
		if (!YCDW.Trim().IsEmpty())
		{

			comm2.Parameters.Set("YCDW", YCDW);
		}

		if (!STATUS.Trim().IsEmpty())
		{

			comm2.Parameters.Set("STATUS", STATUS);
		}

		if (!LOAD_CODE_FACTORY.Trim().IsEmpty())
		{
			comm2.Parameters.Set("LOAD_CODE_FACTORY", LOAD_CODE_FACTORY);
		}
		if (!LOAD_CODE_AREA.Trim().IsEmpty())
		{
			comm2.Parameters.Set("LOAD_CODE_AREA", LOAD_CODE_AREA);
		}
		if (!LOAD_CODE.Trim().IsEmpty())
		{
			comm2.Parameters.Set("LOAD_CODE", LOAD_CODE);
		}
		if (!FACTORY_DIV.Trim().IsEmpty())
		{

			comm2.Parameters.Set("FACTORY_DIV", FACTORY_DIV);
		}
		if (!MAT_NO.Trim().IsEmpty())
		{

			comm2.Parameters.Set("MAT_NO", MAT_NO);
		}

		comm2.SetCommandText(sql);
		Log::Trace("", "", "hhhhh");
		int affectRows = comm2.ExecuteQuery(bcls_ret->Tables[1]);
		Log::Trace("", "", "affectRows  = [{0}] ", affectRows);
		comm2.Close();

		//定义表名
		
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


