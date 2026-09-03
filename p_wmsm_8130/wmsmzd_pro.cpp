/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:    KE2111
Version:    1.0
Date:     2023-08-08
Description: 字典类型维护_综合
**************************************************/
//框架公用头文件，勿删
#include "stdafx.h"


// service入口
BM2F_ENTERACE(wmsmzd_pro)

int f_wmsmzd_pro(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义	

	/* ***** 静态变量定义 ***** */

	int fetchRowCount = 0;
	int i;
	int ret;
	int RowCount = 0;
	CDecimal i_count = 0;
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

	CModel twmsmzd01("TWMSMZD01");
	CModel twmsmzd02("TWMSMZD02");


	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{

		
		if (bcls_rec->Tables[0].Columns.Contains("TABLE_TYPE"))
			v_table_name = bcls_rec->Tables[0].Rows[0]["TABLE_TYPE"].ToString();

		Log::Info("", __FUNCTION__, "v_table_name   =[{0}]", v_table_name);
		/* 获得传入参数 */
		/* 维护事件表 */
		// 新增事件

#pragma region   字典目录维护

		if (v_table_name == "LB")
		{
			if (bcls_rec->Tables.IndexOf("WMSM_INS") >= 0)
			{
				for (int i = 0; i < bcls_rec->Tables["WMSM_INS"].Rows.get_Count(); i++)
				{
					twmsmzd01.Reset();
					twmsmzd01.MergeFrom(bcls_rec->Tables["WMSM_INS"].Rows[i]);
					twmsmzd01.TrimOrBlank();

					if (twmsmzd01["CODE_CLASS"].ToString().Trim() == "")
					{
						sprintf(s.msg, "代码编号不可为空。");
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
					//twmsmzd01.Print();


					/* 新增事件信息 */
					twmsmzd01["REC_CREATOR"] = s.userid;   //记录创建责任者
					twmsmzd01["REC_CREATE_TIME"] = datetimeNow;   //记录创建时刻
					
					twmsmzd01.TrimOrBlank();



					if (twmsmzd01.QueryCount("CODE_CLASS") > 0)
					{
						sprintf(s.msg, "代码编号[%s]已存在，不可重复新增。", (const char*)twmsmzd01["CODE_CLASS"]);
						throw CApplicationException(-1, s.msg, s.svc_name);
					}

					twmsmzd01.Insert();

				}

			}

			// 修改事件
			if (bcls_rec->Tables.IndexOf("WMSM_UPD") >= 0)
			{
				for (int i = 0; i < bcls_rec->Tables["WMSM_UPD"].Rows.get_Count(); i++)
				{
					twmsmzd01.Reset();

					twmsmzd01.MergeFrom(bcls_rec->Tables["WMSM_UPD"].Rows[i]);
					twmsmzd01.TrimOrBlank();


					/* 修改事件信息 */
					twmsmzd01["REC_REVISOR"] = s.userid;
					twmsmzd01["REC_REVISE_TIME"] = datetimeNow;
					twmsmzd01.TrimOrBlank();

					twmsmzd01.Delete("CODE_CLASS");
					twmsmzd01.Insert();

				}
			}

			// 删除事件
			if (bcls_rec->Tables.IndexOf("WMSM_DEL") >= 0)
			{
				for (int i = 0; i < bcls_rec->Tables["WMSM_DEL"].Rows.get_Count(); i++)
				{
					twmsmzd01.Reset();
					twmsmzd01.MergeFrom(bcls_rec->Tables["WMSM_DEL"].Rows[i]);
					twmsmzd01.TrimOrBlank();


					/* 删除事件信息 */
					twmsmzd01.Delete("CODE_CLASS");


				}
			}
		}


#pragma endregion
#pragma region   字典内容维护
		if (v_table_name == "MX")
		{
			if (bcls_rec->Tables.IndexOf("WMSM_INS") >= 0)
			{
				for (int i = 0; i < bcls_rec->Tables["WMSM_INS"].Rows.get_Count(); i++)
				{
					twmsmzd02.Reset();
					twmsmzd02.MergeFrom(bcls_rec->Tables["WMSM_INS"].Rows[i]);
					twmsmzd02.TrimOrBlank();

					//twmsmzd02.Print();
					if (twmsmzd02["CODE"].ToString().Trim() == "")
					{
						sprintf(s.msg, "代码不可为空。");
						throw CApplicationException(-1, s.msg, s.svc_name);
					}


					/* 新增事件信息 */
					twmsmzd02["REC_CREATOR"] = s.userid;   //记录创建责任者
					twmsmzd02["REC_CREATE_TIME"] = datetimeNow;   //记录创建时刻
					twmsmzd02.TrimOrBlank();



					if (twmsmzd02.QueryCount("CODE_CLASS,CODE") > 0)
					{
						sprintf(s.msg, "代码[%s]已存在，不可重复新增。", (const char*)twmsmzd02["CODE"]);
						throw CApplicationException(-1, s.msg, s.svc_name);
					}

					twmsmzd02.Insert();

				}

			}

			// 修改事件
			if (bcls_rec->Tables.IndexOf("WMSM_UPD") >= 0)
			{
				for (int i = 0; i < bcls_rec->Tables["WMSM_UPD"].Rows.get_Count(); i++)
				{
					twmsmzd02.Reset();

					twmsmzd02.MergeFrom(bcls_rec->Tables["WMSM_UPD"].Rows[i]);
					twmsmzd02.TrimOrBlank();


					/* 修改事件信息 */
					twmsmzd02["REC_CREATOR"] = s.userid;   //记录创建责任者
					twmsmzd02["REC_CREATE_TIME"] = datetimeNow;   //记录创建时刻
					twmsmzd02["REC_REVISOR"] = s.userid;
					twmsmzd02["REC_REVISE_TIME"] = datetimeNow;
					twmsmzd02.TrimOrBlank();

					//2026.03.25 指导去向限制内容长度
					if (twmsmzd02["CODE_CLASS"].ToString().Trim() == "WM02" && twmsmzd02["CODE_DESC_1_CONTENT"].ToString().GetLength() > 20)
					{
						strcpy(s.msg, "维护内容超长!");
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
					twmsmzd02.Delete("CODE_CLASS,CODE");
					twmsmzd02.Insert();

				}
			}

			// 删除事件
			if (bcls_rec->Tables.IndexOf("WMSM_DEL") >= 0)
			{
				for (int i = 0; i < bcls_rec->Tables["WMSM_DEL"].Rows.get_Count(); i++)
				{
					twmsmzd02.Reset();
					twmsmzd02.MergeFrom(bcls_rec->Tables["WMSM_DEL"].Rows[i]);
					twmsmzd02.TrimOrBlank();


					/* 删除事件信息 */
					twmsmzd02.Delete("CODE_CLASS,CODE");


				}
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
