/*
*  程序名称			: cm_a02108_rcv
*  程序描述			: 汽运、铁路装车实绩
*
*  	2023-11-9 	李振			(ADD)程序建立
*			... ...
* **************************************************************************** */
/*<remark>=========================================================
 <summary>
 装车实绩
 新增TWMSM62和TWMA0


 
 <para>数据库表：TWMSM61(装车实绩表)         </para>
 </summary>
 <returns>电文处理成功与否</returns>
===========================================================</remark>*/

/* C/C++ 的标准头文件部分 */

#include "stdafx.h"
#include "epex.h"
int f_mmsm99(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wm00_queue(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);


BM2F_ENTERACE_TELE(cm_a02108_rcv)
int f_cm_a02108_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);
	/* ***** 静态变量定义 ***** */
	int doFlag = 0;
	int fetchRowCount = 0;
	int row_count = 0, i = 0, ret = 0;
	int blkNum = 0;
	int wm00que_count = 0;
	int mm0099_count = 0;


	CString lpsz_user_id, c_datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString	lpsz_out_div;
	EIClass sm_bcls_rec;

	CModel tmmsm96("TMMSM96");
	CModel twmsm62("TWMSM62");
	CModel twmsm41("TWMSM41");
	CModel twmsm42("TWMSM42");
	CModel tmmsm01("TMMSM01");
	CModel hmmsm01("HMMSM01");
	CModel twma0("TWMA0");

	/* ***** 电文变量定义 ***** */
	CString    c_mat_no;
	CString    c_ready_bill_no;
	CString    c_order_no;
	CString    c_red_cause_desc;
	CString    c_rec_revisor;
	CString    c_rec_revise_time;
	CString    c_red_flag;


	/* ***** 程序变量 ***** */
	CString c_user = " ", c_tc_no = " ";

	/* ***** 数据库SQL操作字符串 ***** */
	CString	sqlstr(""), sqlstr1(""), sqlstr2(""), sqlstr3(""), sqlstr4(""), sqlstr5("");

	/* ***** 数据库操作类定义 ***** */
	CDbCommand execute_sql(conn);

	/* ***** 应用程序开始处理 ***** */
	if (!bcls_rec->Tables.Contains("MM0099")) {
		bcls_rec->Tables.Add("MM0099");
		bcls_rec->Tables["MM0099"].Columns.Add(tmmsm96);
	}
	bcls_rec->Tables["MM0099"].Rows.Clear();
	blkNum = bcls_rec->Tables.IndexOf("WM00QUE");
	if (blkNum < 0)
	{
		bcls_rec->Tables.Add("WM00QUE");
		bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");
		bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "MAT_NO");
		bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "MAT_NUM");
		bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "UNIT_CODE");
		bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "OPER_FLAG");
		bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "STOCK_NO");
	}
	bcls_rec->Tables["WM00QUE"].Rows.Clear();

	try
	{
		if (!bcls_rec->Tables.IndexOf("A02108_1")) {
			sprintf(s.msg, "电文不包含A02108_1表。");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (bcls_rec->Tables["A02108"].Rows[0]["DEAL_FLAG"].ToString() == "I")
		{
			for (int i = 0; i < bcls_rec->Tables["A02108_1"].Rows.get_Count(); i++) {
				twmsm62.Reset();
				twmsm62.MergeFrom(bcls_rec->Tables[0].Rows[0]);
				twmsm62.MergeFrom(bcls_rec->Tables["A02108_1"].Rows[i]);
				twmsm62["UNLOAD_FLAG"] = "0";
				twmsm62["AFFIRM_FLAG"] = "0";
				twmsm62.Insert();
				bool return_file = false;

				tmmsm01["MAT_NO"] = twmsm62["MAT_NO"];
				hmmsm01["MAT_NO"] = twmsm62["MAT_NO"];
				twma0["MAT_NO"] = twmsm62["MAT_NO"];
				if (!tmmsm01.Query("MAT_NO")) {
					if (hmmsm01.Query("MAT_NO")) {
						return_file = true;
					}
				}
				else {
					return_file = true;
				}


				if (return_file)//退料
				{
					twma0["STOCK_OPER_ORDER"] = "1Q";
				}
				else {
					twma0["STOCK_OPER_ORDER"] = "1A";
				}
				if (!twma0.Query("MAT_NO,STOCK_OPER_ORDER") 
					&& bcls_rec->Tables["A02108"].Rows[0]["PLAN_NO"].ToString().SubstringNE(0, 4) != "21PS"
					&& bcls_rec->Tables["A02108"].Rows[0]["PLAN_NO"].ToString().SubstringNE(0, 2) != "AB" 
					&& bcls_rec->Tables["A02108"].Rows[0]["PLAN_NO"].ToString().SubstringNE(0, 2) != "C0")
				{
					bcls_rec->Tables["WM00QUE"].Rows.Add();
					bcls_rec->Tables["WM00QUE"].Rows[wm00que_count]["MAT_NO"] = tmmsm01["MAT_NO"];
					bcls_rec->Tables["WM00QUE"].Rows[wm00que_count]["STOCK_OPER_ORDER"] = twma0["STOCK_OPER_ORDER"];
					bcls_rec->Tables["WM00QUE"].Rows[wm00que_count]["MAT_NUM"] = "1";
					bcls_rec->Tables["WM00QUE"].Rows[wm00que_count]["UNIT_CODE"] = tmmsm01["UNIT_CODE"];
					bcls_rec->Tables["WM00QUE"].Rows[wm00que_count]["OPER_FLAG"] = "I";
					bcls_rec->Tables["WM00QUE"].Rows[wm00que_count]["STOCK_NO"] = "SYA";
					wm00que_count++;
				}





			}
		}
		else if (bcls_rec->Tables["A02108"].Rows[0]["DEAL_FLAG"].ToString() == "D")
		{
			for (int i = 0; i < bcls_rec->Tables["A02108_1"].Rows.get_Count(); i++) {
				twmsm62.Reset();
				twmsm62.MergeFrom(bcls_rec->Tables[0].Rows[0]);
				twmsm62.MergeFrom(bcls_rec->Tables["A02108_1"].Rows[i]);
				twmsm62.Query("PRACTICE_NO,MAT_NO");
				if (twmsm62["UNLOAD_FLAG"].ToString() == "1")
				{
					sprintf(s.msg, "该材料已卸车，不可撤销。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				
				twmsm62["DEAL_FLAG"] = bcls_rec->Tables["A02108"].Rows[0]["DEAL_FLAG"].ToString();//卸车标记
				twmsm62["AFFIRM_FLAG"] = "9";//确认标记
				twmsm62.Update("DEAL_FLAG,AFFIRM_FLAG", "PRACTICE_NO,MAT_NO");


				twma0["MAT_NO"] = twmsm62["MAT_NO"];
				if (twma0.QueryCount("MAT_NO")>0)
				{
					twma0.Delete("MAT_NO");
				}



			}
		}
		if (bcls_rec->Tables["WM00QUE"].Rows.get_Count() > 0) {
			Log::Trace("", "", "xxx");
			doFlag = f_wm00_queue(bcls_rec, bcls_ret, conn);  
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		


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
	catch (const CApplicationException& ex)
	{
		//	strncpy(s.msg, (const char*)ex.GetMsg(), 399); //返回前台，与EI.EITuxedo.CallService()方法返回的EI.EIInfo对象的sys_info.msg参数对应
		s.flag = ex.GetCode();       //返回前台，与EI.EITuxedo.CallService()方法返回的EI.EIInfo对象的sys_info.flag参数对应
		Log::Error("", __FUNCTION__, "error=[{0}]", s.msg);
		doFlag = -1;
	}

	catch (const CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), 399); //返回前台，与EI.EITuxedo.CallService()方法返回的EI.EIInfo对象的sys_info.msg参数对应
		s.flag = ex.GetCode();       //返回前台，与EI.EITuxedo.CallService()方法返回的EI.EIInfo对象的sys_info.flag参数对应
		doFlag = -1;
	}

	return doFlag;
}
