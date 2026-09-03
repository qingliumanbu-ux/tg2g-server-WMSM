/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         nieyuanyuan
Version:		1.0
Date:			2023-11-14
Description:
**************************************************/

//框架头文件
#include "stdafx.h"

//程序用头文件

/*<remark>=========================================================
///<summary>
///
///<para>
///
///</para>
///<para>数据库表：TWMSM60 倒运计划表；
///<returns>倒运计划生成，发送物流系统</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wmsmzhiliu_auto)
int f_wmsmzhiliu_auto(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	int count = 0;
	int cs = 0;
	int blkNum = 0;
	CString date_time = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CString v_factory_div = "";
	CString v_stock_no = "";
	CString v_plan_no = "";
	CString s_date_from = "";
	CString s_date_to = "";
	CString s_factory_div = "";
	CString s_stock_no = "";
	CString s_seq_no = "";

	CDateTime dt_date;


	CString plan_time_from = "";
	CString plan_time_to = "";

	/* 实体类定义 */


	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CDbCommand cmd_inq(conn);

	CString v_shift_no = " ";
	CString v_shift_group = " ";
	f_epep_get_shift_group("SMCP", CDateTime::Now().ToString("yyyyMMddHHmmss"), v_shift_no, v_shift_group, conn);


	try
	{
		sqlstr = " insert into DA_MAT_DAY_ZHILIU\
			select\
			t.钢种分类1,\
			t.plandirectition1,\
			to_char(sysdate, 'yyyyMMdd'),\
			sum(t.more_30days) as more_30days,\
			sum(t.more_60days) as more_60days,\
			sum(t.more_90days) as more_90days\
			from(\
				select t.*,\
				case when days_span > 30 then 1 else 0 end as more_30days,\
				case when days_span > 60 then 1 else 0 end as more_60days,\
				case when days_span > 90 then 1 else 0 end as more_90days,\
				case\
				when plandirectition1 = '2250轧机' then '2250'\
				when plandirectition1 = '1549轧机' then '1549'\
				when plandirectition1 = '五轧新线' then '4300'\
				when plandirectition1 = '临钢' then '临钢'\
					else t.plandirectition1 end            as plandirectition\
					from(select decode(C_DIV, '1', '不锈钢', '碳钢') as                           钢种分类1,\
					MAT_NO,\
					MAT_WT                          as                           收货重量,\
					CASE\
					WHEN CODE_DESC_1_CONTENT in('2250轧机', '1549轧机', '三轧', '五轧新线', '临钢') then CODE_DESC_1_CONTENT\
						  else '外销' end                                            plandirectition1,\
						  SLAB_CUT_TIME,\
						  CEIL(\
						  ((sysdate -\
						  TO_DATE(SLAB_CUT_TIME, 'yyyy-mm-dd hh24-mi-ss')))) days_span\
						  from tMMSM01 v1\
						  left join twmsmzd02 t2 on v1.GUIDE_DEST = t2.CODE and t2.CODE_CLASS = 'WM02'\
						  where 1 = 1) t\
						  ) t\
			group by t.钢种分类1, t.plandirectition1  ";
		Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();
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
		Log::Trace("", __FUNCTION__, "s.flag[{0}]", s.flag);
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

