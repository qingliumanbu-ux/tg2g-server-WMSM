/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         LIZHEN
Version:		1.0
Date:			2023-11-07
Description:	指标展板自动采集数据
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"
//程序用头文件


//函数申明

/*<remark>=========================================================
//1、删除退料队列
//2、向制造发送调拨申请
===========================================================</remark>*/

BM2F_ENTERACE(wmsmfosm_auto);

int f_wmsmfosm_auto(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	int ret = 0;

	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */

	EPEX epex;

	/* 数据库SQL操作字符串 */
	CString sql = "";
	CString sqlstr = "";
	CString sqlwhere = "";
	CString sqlstr_count;
	CString sqlstr_ins;

	/* 业务变量 */
	CString s_tc_no = " ";
	/* 全局变量 */

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_cou(conn);
	CDbCommand cmd_ins(conn);

	//系统的分页类信息。


	CString time_lastmonth = CDateTime::Now().AddMonths(-1).ToString("yyyyMMddHHmmss");
	CString time_lastday = CDateTime::Now().AddDays(-1).ToString("yyyyMMddHHmmss");
	CString datetime_n = CDateTime::Now().ToString("yyyyMMddHHmmss");
	try
	{
		sqlstr = " delete from twmsmfosm where 1=1 ";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();
		//==================================================================================不锈钢====================================================================================================
		//AOD当班冶炼炉数
		sqlstr = " insert into TWMSMFOSM\
			select '1' c_div, '01' work_code, 'AOD当班冶炼炉数' work_name, count(1) work_value,  case when PROD_SHIFT_GROUP='A' THEN '甲' when PROD_SHIFT_GROUP='B' THEN '乙' when PROD_SHIFT_GROUP='C' THEN '丙' when PROD_SHIFT_GROUP='D' THEN '丁' end PROD_SHIFT_GROUP, ' ', ' ' \
			from tmmsm27 \
		where PROD_DATE = '" + datetime_n.SubstringNE(0, 8) + "' \
			and PROD_SHIFT_GROUP = ( \
				select * \
				from(select PROD_SHIFT_GROUP from tmmsm27 where PROD_DATE = '" + datetime_n.SubstringNE(0, 8) + "' order by REC_CREATE_TIME desc) \
		where ROWNUM = 1) \
			group by PROD_SHIFT_GROUP ";
		Log::Trace("", __FUNCTION__, "sqlstr				= [{0}]", (const char*)sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//AOD当月各班平均冶炼炉数
		sqlstr = "insert into TWMSMFOSM \
			select '1' c_div, '02' work_code, 'AOD各班当月平均炉数' work_name, T.count,  case when PROD_SHIFT_GROUP='A' THEN '甲' when PROD_SHIFT_GROUP='B' THEN '乙' when PROD_SHIFT_GROUP='C' THEN '丙' when PROD_SHIFT_GROUP='D' THEN '丁' end PROD_SHIFT_GROUP, ROWNUM, ' '\
			from(\
				select PROD_SHIFT_GROUP, round(avg(COU), 2) count\
				from(\
					select PROD_SHIFT_GROUP, PROD_DATE, count(1) cou\
					from tmmsm27\
		where PROD_DATE like '"+ datetime_n.SubstringNE(0, 6) +"%'\
			group by PROD_DATE, PROD_SHIFT_GROUP)\
		where cou >= 3\
			group by PROD_SHIFT_GROUP order by count desc) T ";
		Log::Trace("", __FUNCTION__, "sqlstr				= [{0}]", (const char*)sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//合金熔化炉当班冶炼炉数
		sqlstr = " insert into TWMSMFOSM\
			select '1' c_div, '03' work_code, '合金熔化炉当班冶炼炉数' work_name, count(1) work_value,  case when PROD_SHIFT_GROUP='A' THEN '甲' when PROD_SHIFT_GROUP='B' THEN '乙' when PROD_SHIFT_GROUP='C' THEN '丙' when PROD_SHIFT_GROUP='D' THEN '丁' end PROD_SHIFT_GROUP, ' ', ' ' \
			from tmmsm19 \
		where PROD_DATE = '" + datetime_n.SubstringNE(0, 8) + "' \
			and PROD_SHIFT_GROUP = ( \
				select * \
				from(select PROD_SHIFT_GROUP from tmmsm19 where PROD_DATE = '" + datetime_n.SubstringNE(0, 8) + "' order by REC_CREATE_TIME desc) \
		where ROWNUM = 1) \
			group by PROD_SHIFT_GROUP ";
		Log::Trace("", __FUNCTION__, "sqlstr				= [{0}]", (const char*)sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//合金熔化炉当月各班平均冶炼炉数
		sqlstr = "insert into TWMSMFOSM \
			select '1' c_div, '04' work_code, '合金熔化炉当月各班平均冶炼炉数' work_name, T.count,  case when PROD_SHIFT_GROUP='A' THEN '甲' when PROD_SHIFT_GROUP='B' THEN '乙' when PROD_SHIFT_GROUP='C' THEN '丙' when PROD_SHIFT_GROUP='D' THEN '丁' end PROD_SHIFT_GROUP, ROWNUM, ' '\
			from(\
				select PROD_SHIFT_GROUP, round(avg(COU), 2) count\
				from(\
					select PROD_SHIFT_GROUP, PROD_DATE, count(1) cou\
					from tmmsm19\
		where PROD_DATE like '" + datetime_n.SubstringNE(0, 6) + "%'\
			group by PROD_DATE, PROD_SHIFT_GROUP)\
		where cou >= 3\
			group by PROD_SHIFT_GROUP order by count desc) T ";
		Log::Trace("", __FUNCTION__, "sqlstr				= [{0}]", (const char*)sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();


		//==================================================================================全厂====================================================================================================
		//班组板坯交库量
		sqlstr = " insert into TWMSMFOSM\
			select '0', '01', '班组板坯交库量', t.sum_w,  case when HAND_OVER_GROUP='A' THEN '甲' when HAND_OVER_GROUP='B' THEN '乙' when HAND_OVER_GROUP='C' THEN '丙' when HAND_OVER_GROUP='D' THEN '丁' end HAND_OVER_GROUP, ROWNUM, ' '\
			from(select *\
				from(select HAND_OVER_GROUP, round(sum(MAT_WT), 0) sum_w\
					from(SELECT HAND_OVER_GROUP, MAT_WT FROM VMMSM01 WHERE TRAN_END_TIME LIKE '"+ datetime_n.SubstringNE(0, 6) +"%')\
					group by HAND_OVER_GROUP)\
				order by sum_w desc) t "; 
		Log::Trace("", __FUNCTION__, "sqlstr				= [{0}]", (const char*)sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();


		//班组修磨量
		sqlstr = " insert into TWMSMFOSM\
			select '0', '02', '班组修磨量', t.sum_w,  case when MEND_SHIFT='A' THEN '甲' when MEND_SHIFT='B' THEN '乙' when MEND_SHIFT='C' THEN '丙' when MEND_SHIFT='D' THEN '丁' end MEND_SHIFT, ROWNUM, ' '\
			from(select *\
				from(select MEND_SHIFT, round(sum(MEND_BEFORE_WEIGHT), 0) sum_w\
					from(select MEND_SHIFT, MEND_BEFORE_WEIGHT from tmmsm34 where START_TIME LIKE '" + datetime_n.SubstringNE(0, 6) + "%')\
					group by MEND_SHIFT)\
				order by sum_w desc) t ";
		Log::Trace("", __FUNCTION__, "sqlstr				= [{0}]", (const char*)sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//班组产量
		sqlstr = " insert into TWMSMFOSM\
			select '0', '03', '班组产量', t.sum_w,  case when PROD_SHIFT_GROUP='A' THEN '甲' when PROD_SHIFT_GROUP='B' THEN '乙' when PROD_SHIFT_GROUP='C' THEN '丙' when PROD_SHIFT_GROUP='D' THEN '丁' end PROD_SHIFT_GROUP, ROWNUM, ' '\
			from(select *\
				from(select PROD_SHIFT_GROUP, round(sum(MAT_WT), 0) sum_w\
					from(select PROD_SHIFT_GROUP, MAT_WT from vmmsm01 where PROD_TIME LIKE '" + datetime_n.SubstringNE(0, 6) + "%' and PROD_SHIFT_GROUP!=' ')\
					group by PROD_SHIFT_GROUP)\
				order by sum_w desc) t ";
		Log::Trace("", __FUNCTION__, "sqlstr				= [{0}]", (const char*)sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();



		//==================================================================================碳钢====================================================================================================
		
			//当月班组BOF总炉数
		sqlstr = " INSERT INTO TWMSMFOSM\
			SELECT '2', '01', '当月班组BOF总炉数', COUNT,  case when PROD_SHIFT_GROUP='A' THEN '甲' when PROD_SHIFT_GROUP='B' THEN '乙' when PROD_SHIFT_GROUP='C' THEN '丙' when PROD_SHIFT_GROUP='D' THEN '丁' end PROD_SHIFT_GROUP, ROWNUM, ' ' FROM(\
				SELECT PROD_SHIFT_GROUP, COUNT(HEAT_NO) COUNT FROM TMMSM21 WHERE PROD_DATE LIKE '"+ datetime_n.SubstringNE(0, 6) +"%'\
				AND PROD_SHIFT_GROUP != ' ' GROUP BY PROD_SHIFT_GROUP ORDER BY COUNT DESC) ";
		Log::Trace("", __FUNCTION__, "sqlstr				= [{0}]", (const char*)sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();
		
		//当月班组BOF总产量
		sqlstr = " INSERT INTO TWMSMFOSM\
			SELECT '2', '02', '当月班组BOF总产量', sum_wt,  case when PROD_SHIFT_GROUP='A' THEN '甲' when PROD_SHIFT_GROUP='B' THEN '乙' when PROD_SHIFT_GROUP='C' THEN '丙' when PROD_SHIFT_GROUP='D' THEN '丁' end PROD_SHIFT_GROUP, ROWNUM, ' ' FROM(\
				SELECT PROD_SHIFT_GROUP, round(sum(ACTRESULT),0) sum_wt FROM TMMSM21 WHERE PROD_DATE LIKE '"+ datetime_n.SubstringNE(0, 6) +"%'\
				AND PROD_SHIFT_GROUP != ' ' GROUP BY PROD_SHIFT_GROUP ORDER BY sum_wt DESC) ";
		Log::Trace("", __FUNCTION__, "sqlstr				= [{0}]", (const char*)sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//当月班组BOF总产量
		sqlstr = " insert into TWMSMFOSM\
			select '2', '03', '当月班组BOF冶炼周期', TIME_SPAN,  case when PROD_SHIFT_GROUP='A' THEN '甲' when PROD_SHIFT_GROUP='B' THEN '乙' when PROD_SHIFT_GROUP='C' THEN '丙' when PROD_SHIFT_GROUP='D' THEN '丁' end PROD_SHIFT_GROUP, ROWNUM, ' ' from(\
				select PROD_SHIFT_GROUP, round(avg(TIME_SPAN), 1) TIME_SPAN\
				from(select HEAT_NO,\
					START_TIME,\
					END_TIME,\
					CEIL((TO_DATE(END_TIME, 'yyyy-mm-dd hh24-mi-ss') -\
						TO_DATE(START_TIME, 'yyyy-mm-dd hh24-mi-ss')) * 24 * 60) TIME_SPAN,\
					PROD_SHIFT_GROUP\
					from tmmsm21\
		where PROD_DATE like '" + datetime_n.SubstringNE(0, 6) + "%'\
			and START_TIME != ' '\
			and START_TIME != '19000101000000'\
			and END_TIME != ' '\
			and END_TIME != '19000101000000' and PROD_SHIFT_GROUP != ' ') group by PROD_SHIFT_GROUP order by TIME_SPAN) ";
		Log::Trace("", __FUNCTION__, "sqlstr				= [{0}]", (const char*)sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();
		
		
		//溅渣结束到兑铁
		sqlstr = " insert into TWMSMFOSM\
			SELECT '2', '04', '溅渣结束到兑铁', TIME_SPAN,  case when PROD_SHIFT_GROUP='A' THEN '甲' when PROD_SHIFT_GROUP='B' THEN '乙' when PROD_SHIFT_GROUP='C' THEN '丙' when PROD_SHIFT_GROUP='D' THEN '丁' end PROD_SHIFT_GROUP, ROWNUM, ' '\
			FROM(SELECT PROD_SHIFT_GROUP, ROUND(AVG(TIME_SPAN), 1) TIME_SPAN\
				FROM(SELECT A.*, case\
					when START_TIME != ' ' and SLAG_END_TIME is not null and SLAG_END_TIME != ' ' then CEIL(\
						(TO_DATE(START_TIME, 'yyyy-mm-dd hh24-mi-ss') -\
							TO_DATE(SLAG_END_TIME, 'yyyy-mm-dd hh24-mi-ss')) * 24 * 60) end TIME_SPAN\
					FROM(SELECT PROD_DATE, HEAT_NO, START_TIME, PROD_SHIFT_GROUP, SLAG_END_TIME\
						FROM TMMSM21\
						WHERE SLAG_END_TIME != '19000101000000'\
						and PROD_DATE like '"+ datetime_n.SubstringNE(0, 6) +"%') A)\
				GROUP BY PROD_SHIFT_GROUP\
				ORDER BY TIME_SPAN) ";
		Log::Trace("", __FUNCTION__, "sqlstr				= [{0}]", (const char*)sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//当月班组LF精炼时间
		sqlstr = " INSERT INTO TWMSMFOSM\
			select '2', '05', '当月班组LF精炼时间', TIME_SPAN,  case when PROD_SHIFT_GROUP='A' THEN '甲' when PROD_SHIFT_GROUP='B' THEN '乙' when PROD_SHIFT_GROUP='C' THEN '丙' when PROD_SHIFT_GROUP='D' THEN '丁' end PROD_SHIFT_GROUP, ROWNUM, ' '\
			from(select PROD_SHIFT_GROUP, round(avg(TIME_SPAN), 1) TIME_SPAN\
				from(select HEAT_NO,START_TIME,END_TIME,CEIL((TO_DATE(END_TIME, 'yyyy-mm-dd hh24-mi-ss') -\
						TO_DATE(START_TIME, 'yyyy-mm-dd hh24-mi-ss')) * 24 * 60) TIME_SPAN,\
					case\
					when PROD_SHIFT_GROUP = 'a' then 'A'\
					when PROD_SHIFT_GROUP = 'b' then 'B'\
					when PROD_SHIFT_GROUP = 'c' then 'C'\
					when PROD_SHIFT_GROUP = 'd' then 'D'\
					ELSE PROD_SHIFT_GROUP END                                  PROD_SHIFT_GROUP\
					from tmmsm24\
		where PROD_DATE like '" + datetime_n.SubstringNE(0, 6) + "%'\
			and HEAT_NO like 'B%'\
			and START_TIME != ' '\
			and START_TIME != '19000101000000'\
			and END_TIME != ' '\
			and END_TIME != '19000101000000'\
			and PROD_SHIFT_GROUP != ' ')\
				group by PROD_SHIFT_GROUP\
			order by TIME_SPAN) ";
		Log::Trace("", __FUNCTION__, "sqlstr				= [{0}]", (const char*)sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();



		

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。" /* _RES("GCRSS0000006")*//*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
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