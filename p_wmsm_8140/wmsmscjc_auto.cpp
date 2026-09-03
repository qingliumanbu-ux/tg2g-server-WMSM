/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         nieyuanyuan
Version:		1.0
Date:			2023-11-14
Description:	轮询2250装车报错
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

BM2F_ENTERACE(wmsmscjc_auto)
int f_wmsmscjc_auto(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	int count = 0;
	int cs = 0;
	int blkNum = 0;
	CString date_time = CDateTime::Now().ToString("yyyyMMdd");
    CString date_time_o = CDateTime::Now().AddDays(-1).ToString("yyyyMMdd");

	
	CString sqlstr = "";
	
	CDbCommand comm(conn);
	CDbCommand cmd_inq(conn);
	

	try
	{
        sqlstr = " insert into TMMSMSCJC  WITH MM1 AS (select C_DIV,\
                    MAT_NO,\
                    MAT_WT,\
                    GUIDE_DEST,\
                    CASE\
                        WHEN CODE_DESC_1_CONTENT in ('2250轧机', '1549轧机', '三轧', '五轧新线', '型材厂')\
                            then CODE_DESC_1_CONTENT\
                        else '外销坯' end 计划流向,\
                    TRAN_TIME,\
                    TRAN_END_TIME,\
                    ARCHIVE_TIME,\
                    SLAB_CUT_TIME\
             from VMMSM01 v1\
                      left join twmsmzd02 t2 on v1.GUIDE_DEST = t2.CODE and t2.CODE_CLASS = 'WM02'\
             where SLAB_CUT_TIME >= '"+ date_time_o +"' || '080000'\
               and SLAB_CUT_TIME <= '"+ date_time +"' || '080000'),\
     MM2 AS (select C_DIV,\
                    MAT_NO,\
                    MAT_WT,\
                    GUIDE_DEST,\
                    COMPLEX_DECIDE_CODE,\
                    CASE\
                        WHEN CODE_DESC_1_CONTENT in ('2250轧机', '1549轧机', '三轧', '五轧新线', '型材厂')\
                            then CODE_DESC_1_CONTENT\
                        else '外销坯' end 计划流向,\
                    TRAN_TIME,\
                    TRAN_END_TIME,\
                    ARCHIVE_TIME,\
                    SLAB_CUT_TIME\
             from hMMSM01 v1\
                      left join twmsmzd02 t2 on v1.GUIDE_DEST = t2.CODE and t2.CODE_CLASS = 'WM02'\
             where COMPLEX_DECIDE_CODE != '9'\
               and MAT_NO not in (select IN_MAT_NO from tmmsm35)\
               and TRAN_END_TIME >= '"+ date_time_o +"' || '080000'\
               and TRAN_END_TIME <= '"+ date_time +"' || '080000'),\
     mm3 as (select C_DIV,\
                    计划流向,\
                    sum(MM1.MAT_WT)   sc_mat_wt,\
                    count(mm1.MAT_NO) sc_count\
             from mm1\
             GROUP BY mm1.C_DIV, mm1.计划流向),\
     mm4 as (select C_DIV,\
                    计划流向,\
                    sum(MM2.MAT_WT)   jc_mat_wt,\
                    count(mm2.MAT_NO) jc_count\
             from mm2\
             GROUP BY mm2.C_DIV, mm2.计划流向)\
        SELECT T2.CODE_DESC_2_CONTENT c_div,\
        T2.CODE_DESC_1_CONTENT guide_dest,\
        nvl(mm3.sc_mat_wt, 0)  sc_mat_wt,\
        nvl(mm3.sc_count, 0)   sc_count,\
        nvl(mm4.jc_mat_wt, 0)  jc_mat_wt,\
        nvl(mm4.jc_count, 0)   jc_count,\
         '"+ CDateTime::Now().ToString("yyyyMMddHHmmss") +"'                              REC_CREATE_TIME \
        FROM TWMSMZD02 T2\
        LEFT JOIN MM3 ON T2.CODE_DESC_1_CONTENT = MM3.计划流向 AND T2.CODE_DESC_2_CONTENT = MM3.C_DIV\
        LEFT join mm4 on mm3.C_DIV = mm4.C_DIV and mm3.计划流向 = mm4.计划流向\
        WHERE T2.CODE_CLASS = 'MMSMSCJC'\
        order by mm3.C_DIV, mm3.计划流向\ ";

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

