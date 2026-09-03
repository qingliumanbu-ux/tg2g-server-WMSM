/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:    KE2111
Version:    1.0
Date:     2024-11-15
Description:
**************************************************/
// 框架公用头文件，勿删
#include "stdafx.h"

// service入口
BM2F_ENTERACE(wmsm_form_save)

int f_wmsm_form_save(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
    CTracer log(__FUNCTION__); // 系统日志，必须在代码段开始处定义
    int doFlag = 0;
    /* ***** 静态变量定义 ***** */
    CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
    CModel mr00xx("TWMSMFORM");
    CString sqlstr;
    CDbCommand cmd(conn);
    CString OP_TYPE("I");


    try
    {
        if (bcls_rec->Tables.Contains("TPYE"))
            OP_TYPE = bcls_rec->Tables["TPYE"].Rows[0]["OP_TYPE"].ToString();
        if (OP_TYPE == "I")
        {
            for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
            {
                mr00xx.Reset();

                mr00xx.MergeFrom(bcls_rec->Tables[0].Rows[i]);

                mr00xx.TrimOrBlank();
                mr00xx.Delete("IDX_REQ");
                /* 修改事件信息 */

                sqlstr = "  insert into  TWMSMFORM (REC_REVISOR,REC_REVISE_TIME,FORM_DESC,CONDITIONS,VIEW_FLAG,IDX_REQ) "
                    " values (:REC_REVISOR,:REC_REVISE_TIME,:FORM_DESC,:CONDITIONS,:VIEW_FLAG,:IDX_REQ) ";
                cmd.SetCommandText(sqlstr);
                cmd.Parameters.Set("REC_REVISOR", s.userid);
                cmd.Parameters.Set("REC_REVISE_TIME", datetimeNow);
                cmd.Parameters.Set("FORM_DESC", mr00xx["FORM_DESC"].ToString());
                cmd.Parameters.Set("VIEW_FLAG", mr00xx["VIEW_FLAG"].ToString());
                cmd.Parameters.Set("CONDITIONS", bcls_rec->Tables[0].Rows[i]["CONDITIONS"].ToString());
                cmd.Parameters.Set("IDX_REQ", mr00xx["IDX_REQ"].ToDecimal());
                cmd.ExecuteNonQuery();
                cmd.Close();
            }
        }
        if (OP_TYPE == "U")
        {
            mr00xx.Reset();

            mr00xx.MergeFrom(bcls_rec->Tables[0].Rows[0]);

            mr00xx.TrimOrBlank();
            mr00xx.Update("VIEW_FLAG", "IDX_REQ");
        }
        if (OP_TYPE == "D")
        {
            for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
            {
                mr00xx.Reset();

                mr00xx.MergeFrom(bcls_rec->Tables[0].Rows[i]);

                mr00xx.TrimOrBlank();
                mr00xx.Delete("IDX_REQ");
            }
        }
    }
    catch (CDbException& ex) // 捕获数据库操作异常
    {
        CFormattable arguments[] = { ex.GetCode() };
        CMessageFormat::Format(s.msg, _RES("GCRSS0000006") /*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
        CString str = sqlstr + "\r\n" + ex.GetMsg();
        strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1); // 返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
        s.flag = -1;
        doFlag = -1; // 数据库异常时返回-1，事务将被回滚
    }
    catch (CApplicationException& ex) // 捕获应用错误
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

    cmd.Close();

    return doFlag;
}
