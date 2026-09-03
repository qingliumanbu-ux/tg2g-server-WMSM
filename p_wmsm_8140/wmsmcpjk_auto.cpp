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

BM2F_ENTERACE(wmsmcpjk_auto)
int f_wmsmcpjk_auto(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
    CString v_shift_no = " ";
    CString v_shift_group = " ";
    f_epep_get_shift_group("SMCP", CDateTime::Now().ToString("yyyyMMddHHmmss"), v_shift_no, v_shift_group, conn);


    try
    {
        sqlstr = " insert into DA_MAT_ZPJK\
            select C_DIV,\
            去向                           location,\
            to_char(sysdate, 'yyyyMMdd') DATA_DAY,\
            to_char(sysdate, 'hh24')     DATA_HOUR,\
            count(1)                     sum_cou,\
            sum(MAT_WT)                  sum_wt,\
            '"+ v_shift_group +"' PROD_SHIFT_GROUP\
            from(\
                SELECT C_DIV,\
                CASE\
                WHEN T2.CODE_DESC_1_CONTENT IN('2250轧机', '1549轧机', '放现场', '三轧', '定襄自提', '型材厂','五轧新线')\
                THEN CODE_DESC_1_CONTENT\
                WHEN T2.CODE_DESC_1_CONTENT LIKE '%临钢%' then '临钢'\
                    else '外销坯' end 去向,\
                    MAT_WT\
                    FROM HMMSM01 T\
                    LEFT JOIN TWMSMZD02 T2 ON T2.CODE_CLASS = 'WM02' AND T.GUIDE_DEST = T2.CODE\
                    WHERE USAGE_DECISION != '3005'\
                    and MAT_NO not in(select IN_MAT_NO from tmmsm35)\
                    AND TRAN_END_TIME >= to_char(sysdate - 1 / 2, 'yyyyMMddHH24miss'))\
            group by C_DIV, 去向  ";

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

