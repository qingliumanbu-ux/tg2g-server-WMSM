/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   KE2111
Version:
Date:     2024/11/15
Description: 查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/

// service入口  
BM2F_ENTERACE(wmsmshow_inq)

int f_wmsmshow_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
    CTracer log(__FUNCTION__);

    /* ***** 自定义变量 ***** */
    int doFlag = 0;

    CString sqlstr = "";

    CDbCommand cmd_inq(conn);
    CString table_name = "";

    try
    {
        EIClass inBlock_tbl;
        inBlock_tbl.Tables[0].Columns.Add(DT_STRING, "TABLE");
        for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
        {
            inBlock_tbl.Tables[0].Rows.Add();
            inBlock_tbl.Tables[0].Rows[i]["TABLE"] = bcls_rec->Tables[0].Rows[i]["TABLE"].ToString();
        }

        // SQL查询语句
        EIClass inBlock;


        if (true)
        {
            sqlstr = " select * from twmsmformcon where SQL_CONTEXT!=' ' ";
            cmd_inq.SetCommandText(sqlstr);

            cmd_inq.ExecuteQuery(inBlock.Tables[0]);
            cmd_inq.Close();
        }
        // 执行查询
        if (true)
        {

            for (int i = 0; i < inBlock_tbl.Tables[0].Rows.get_Count(); i++)
            {
                for (int j = 0; j < inBlock.Tables[0].Rows.get_Count(); j++)
                {
                    if (inBlock.Tables[0].Rows[j]["EQUIP_CODE"].ToString() == inBlock_tbl.Tables[0].Rows[i]["TABLE"].ToString())
                    {
                        table_name = inBlock.Tables[0].Rows[j]["EQUIP_CODE"].ToString();
                        if (!bcls_ret->Tables.Contains(table_name))
                        {
                            bcls_ret->Tables.Add(table_name);
                        }
                        switch (conn->DatabaseKind)
                        {

                        case DB_KIND_DB2:
                            sqlstr = inBlock.Tables[0].Rows[j]["SQL_CONTEXT"].ToString();
                            break;
                        default:
                            sqlstr = inBlock.Tables[0].Rows[j]["SQL_CONTEXT"].ToString();
                            break;
                        }
                        // sqlstr = inBlock.Tables[0].Rows[j]["SQL"].ToString();
                        cmd_inq.SetCommandText(sqlstr);
                        cmd_inq.ExecuteQuery(bcls_ret->Tables[table_name]);
                        break;
                    }
                }
            }

            for (int i = 0; i < bcls_ret->Tables.get_Count(); i++)
            {
                EIClass tmp;
                tmp.Tables[0].Copy(bcls_ret->Tables[i]);
                bcls_ret->Tables[i].Clear();
             
                for (int j = 0; j < tmp.Tables[0].Rows.get_Count(); j++)
                {
                   
                    bcls_ret->Tables[i].Columns.Add(DT_DECIMAL, tmp.Tables[0].Rows[j][0].ToString());
                }


                for (int k = 0; k < tmp.Tables[0].Columns.get_Count() - 1; k++)
                {

                    bcls_ret->Tables[i].Rows.Add();
                    for (int j = 0; j < tmp.Tables[0].Rows.get_Count(); j++)
                    {
                       
                        bcls_ret->Tables[i].Rows[k][j] = tmp.Tables[0].Rows[j][k + 1];
                    }
                }

            }

            cmd_inq.Close();
        }
    }
    catch (CDbException& ex) // 捕获数据库操作异常
    {
        CFormattable arguments[] = { ex.GetCode() };
        CMessageFormat::Format(s.msg, _RES("GCRSS0000006") /*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
        CString str = sqlstr + "\r\n" + ex.GetMsg();
        strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
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

    return doFlag;
}
