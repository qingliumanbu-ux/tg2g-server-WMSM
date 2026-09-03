/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:    KE2111
Version:    1.0
Date:     2023-08-08
Description: 字典类型维护_综合
**************************************************/
//框架公用头文件，勿删
#include "stdafx.h"

int f_qmts_grind_desc(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
// service入口
BM2F_ENTERACE(wmsmqm11_pro)

int f_wmsmqm11_pro(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义	

	/* ***** 静态变量定义 ***** */

	int fetchRowCount = 0;
	int i;
	int ret;
	int RowCount = 0;
	CString i_count = "";
	int doFlag = 0;
	int blkNum = 0;

	CString	datetime("");
	CString	v_table_name = "";

	CDbCommand cmd(conn);
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_del(conn);
	CString picture_no = "";//画面号
	CString fn_no = "";//功能键号
	CString  sqlstr = "";

	CModel tqmts11a("TQMTS11A");



	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	EIClass v_q11;
	v_q11.Tables[0].Columns.Add(DT_STRING, "STEEL_GRADE");
	v_q11.Tables[0].Columns.Add(DT_STRING, "SG_GRADE_1");
	v_q11.Tables[0].Rows.Clear();
	
	try
	{


#pragma region   字典目录维护

		
			if (bcls_rec->Tables.IndexOf("WMSM_INS") >= 0)
			{
				for (int i = 0; i < bcls_rec->Tables["WMSM_INS"].Rows.get_Count(); i++)
				{
					tqmts11a.Reset();
					tqmts11a.MergeFrom(bcls_rec->Tables["WMSM_INS"].Rows[i]);
					tqmts11a.TrimOrBlank();
					tqmts11a.Print();
					if (tqmts11a["STEEL_GRADE"].ToString().Trim() == "")
					{
						sprintf(s.msg, "内部钢种不可为空。");
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
					if (tqmts11a.QueryCount("STEEL_GRADE")>0)
					{
						sprintf(s.msg, "该内部钢种已有维护，不可重复维护。");
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
					

					/* 新增事件信息 */
					tqmts11a["REC_CREATOR"] = s.userid;   //记录创建责任者
					tqmts11a["REC_REVISE_TIME"] = datetimeNow;   //记录创建时刻

					tqmts11a.TrimOrBlank();




					tqmts11a.Insert();
					tqmts11a.MergeTo(v_q11.Tables[0]);
				}

			}

			// 修改事件
			if (bcls_rec->Tables.IndexOf("WMSM_UPD") >= 0)
			{
				for (int i = 0; i < bcls_rec->Tables["WMSM_UPD"].Rows.get_Count(); i++)
				{
					tqmts11a.Reset();

					tqmts11a.MergeFrom(bcls_rec->Tables["WMSM_UPD"].Rows[i]);
					tqmts11a.TrimOrBlank();
					if (tqmts11a["STEEL_GRADE"].ToString().Trim() == "")
					{
						sprintf(s.msg, "内部钢种不可为空。");
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
					

					/* 修改事件信息 */
					tqmts11a["REC_ERASOR"] = s.userid;
					tqmts11a["REC_ERASE_TIME"] = datetimeNow;
					tqmts11a.TrimOrBlank();

					tqmts11a.Delete("STEEL_GRADE");
					tqmts11a.Insert();
					tqmts11a.MergeTo(v_q11.Tables[0]);

				}
			}

			if (v_q11.Tables[0].Rows.get_Count() > 0) {
				doFlag = f_qmts_grind_desc(&v_q11, bcls_ret, conn);
				if (doFlag < 0)
				{
					strcpy(s.msg, "调用函数报错!");
					throw CApplicationException(-1, s.msg, log.Location);
				}
		}


#pragma endregion

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

	cmd_inq.Close();

	return doFlag;

}
