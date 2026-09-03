/*
*  程序名称			: cm_m7t801_rcv
*  程序描述			: 1549调拨信息接收
*
*  	2023-11-9 	李振			(ADD)程序建立
*			... ...
* **************************************************************************** */
/*<remark>=========================================================
 <summary>
 1549调拨信息接收
 <para>数据库表：TWM41DJ(转库单据表)         </para>
 </summary>
 <returns>电文处理成功与否</returns>
===========================================================</remark>*/

/* C/C++ 的标准头文件部分 */

#include "stdafx.h"
#include "epex.h"

BM2_FUNCTION_IMPORT
int f_mmsm99(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wmsm_t80ry0_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wm00_queue(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);

BM2F_ENTERACE_TELE(cm_6021yx_rcv)
int f_cm_6021yx_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);
	/* ***** 静态变量定义 ***** */
	int doFlag = 0;
	int blkNum = 0;
	int wm00que_count = 0;
	int fetchRowCount = 0;
	int row_count = 0, i = 0, ret = 0;
	CString	record_name = "sm00_record";

	CString lpsz_user_id, c_datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString	lpsz_out_div;
	EIClass sm_bcls_rec;

	CModel twm41dj("TWM41DJ");
	CModel twma0("TWMA0");
	CModel tmmsm01("TMMSM01");
	CModel hmmsm01("HMMSM01");
	CModel tmmsm96("TMMSM96");

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
	EIClass mm0099;
	mm0099.Tables[0].set_TableName("MM0099");
	mm0099.Tables[0].Columns.Add(tmmsm96);
	mm0099.Tables[0].Rows.Clear();
	try
	{
		c_tc_no = s.svc_name;
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


		/* ***** 解析电文 ***** */
		twm41dj.Reset();
		
		twm41dj.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		
		twm41dj.TrimOrBlank();
		if (twm41dj["C_STATESIGN"].ToString() == "2"
			|| twm41dj["C_STATESIGN"].ToString() == "3")//正向调拨反馈，调拨状态（1-未确认，2-接收，3-驳回）
		{
			if (twm41dj.QueryCount("C_DELIVERYID") == 0) {
				sprintf(s.msg, "不存在调拨单号[%s]。", (const char*)twm41dj["C_DELIVERYID"]);
				throw CApplicationException(-1, s.msg, log.Location);
			}
		
			
			tmmsm01["MAT_NO"] = twm41dj["C_BATCHUNIT"];
			tmmsm01.Query("MAT_NO");
			Log::Trace("", "", "line", __LINE__);
			if ((CDateTime::Now() - CDateTime::Parse(tmmsm01["SLAB_CUT_TIME"].ToString())).TotalHours() <= 6 && tmmsm01["C_DIV"].ToString().SubstringNE(0, 1) == "1")//不锈
			{
				tmmsm96["C_ISHOTSEND"] = "1";
			}
			if ((CDateTime::Now() - CDateTime::Parse(tmmsm01["SLAB_CUT_TIME"].ToString())).TotalHours() <= 4 && tmmsm01["C_DIV"].ToString().SubstringNE(0, 1) == "2")//碳
			{
				tmmsm96["C_ISHOTSEND"] = "1";
			}
			twm41dj["C_ISHOTSEND"] = tmmsm96["C_ISHOTSEND"];
			twm41dj.Update("C_STATESIGN,C_ISHOTSEND", "C_DELIVERYID");
			Log::Trace("", "", "line", __LINE__);
			//16、调调拨事件
			tmmsm96.Reset();
			tmmsm96.CopyFrom(tmmsm01);
			Log::Trace("", "", "line", __LINE__);
			if ((CDateTime::Now() - CDateTime::Parse(tmmsm01["SLAB_CUT_TIME"].ToString())).TotalHours() <= 6 && tmmsm01["C_DIV"].ToString().SubstringNE(0, 1) == "1")//不锈
			{
				tmmsm96["C_ISHOTSEND"] = "1";
			}
			if ((CDateTime::Now() - CDateTime::Parse(tmmsm01["SLAB_CUT_TIME"].ToString())).TotalHours() <= 4 && tmmsm01["C_DIV"].ToString().SubstringNE(0, 1) == "2")//碳
			{
				tmmsm96["C_ISHOTSEND"] = "1";
			}
			if (twm41dj["C_STATESIGN"].ToString() == "2")
			{
				tmmsm96["C_STATESIGN"] = "3";//1--正向调拨出库，3--正向调拨完成
				tmmsm96["C_DELIVERYID"] = twm41dj["C_DELIVERYID"];
				tmmsm96["C_DELIVERY_FAC"] = twm41dj["C_ACCEPTDEPT"];
				tmmsm96["C_DELIVERY_STOCK"] = twm41dj["C_ACCEPTSTOCK"];
				tmmsm96["TRAN_END_TIME"] = c_datetime;
			}
			else
			{
				tmmsm96["C_STATESIGN"] = "0";//1--正向调拨出库，3--正向调拨完成
				
				tmmsm96["C_DELIVERY_FAC"] = " ";
				tmmsm96["C_DELIVERY_STOCK"] = " ";
				tmmsm96["TRAN_TIME"] = " ";
				tmmsm96["TRAN_END_TIME"] = " ";
			}
			Log::Trace("", "", "line", __LINE__);
			CString v_shift_no = " ";
			CString v_shift_group = " ";
			f_epep_get_shift_group("SMCP", c_datetime, v_shift_no, v_shift_group, conn);
			tmmsm96["HAND_OVER_GROUP"] = v_shift_group;
			tmmsm96["EVENT_ID"] = "MM76";
			tmmsm96["SYSTEM_ID"] = "MMSM";
			tmmsm96["EVENT_LINE_TYPE"] = "00";
			tmmsm96["FUNC_ID"] = s.svc_name;
			tmmsm96.MergeTo(mm0099.Tables["MM0099"], false);
			Log::Trace("", "", "line", __LINE__);
			if (mm0099.Tables["MM0099"].Rows.get_Count() > 0)
			{
				doFlag = f_mmsm99(&mm0099, bcls_ret, conn);
				if (doFlag != 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			Log::Trace("", "", "line", __LINE__);
			if (twm41dj["C_STATESIGN"].ToString() == "2")
			{
				EIClass dbsq;
				dbsq.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
				dbsq.Tables[0].Columns.Add(DT_STRING, "C_DELIVERYID");
				dbsq.Tables[0].Rows.Add();
				dbsq.Tables[0].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"];
				dbsq.Tables[0].Rows[0]["C_DELIVERYID"] = twm41dj["C_DELIVERYID"];
				doFlag = f_wmsm_t80ry0_snd(&dbsq, bcls_ret, conn);
				if (doFlag != 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
		}
		else if (twm41dj["C_STATESIGN"].ToString() == "1")//正向调拨
		{
			twm41dj["REC_CREATOR"] = s.userid;
			twm41dj["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			vector<CString> date_l = { "D_BILLDATE", "D_OPERATIONDATE", "T_ACCEPTTIME", "C_CLOSEGATETIME", "T_UPLOADTIME", "T_OUTSTOCKTIME",
					"T_INSTOCKTIME","T_SALESCOMFIRMTIME","T_OVERRULETIME","D_REQUIREDATE" };
			CString CODE_NAME = "";
			CString C_V = "";
			for (int i = 0; i < date_l.size(); i++)
			{
				Log::Trace("", "", "line", __LINE__);
				Log::Trace("", __FUNCTION__, "twm41djToString()=[{0}][{1}]", twm41dj[date_l[i]].ToString(), (const char*)date_l[i]);
				Log::Trace("", "", "line", __LINE__);
				if (twm41dj[date_l[i]].ToString().GetLength() == 16)
				{
					Log::Trace("", __FUNCTION__, "ghgtf=[{0}]", twm41dj[date_l[i]].ToString());
					Log::Trace("", __FUNCTION__, "ghgtf=[{0}]", twm41dj[date_l[i]].ToString().Substring(0, 10));
					Log::Trace("", __FUNCTION__, "ghgtf=[{0}]", twm41dj[date_l[i]].ToString().Substring(11, 2));
					Log::Trace("", __FUNCTION__, "ghgtf=[{0}]", twm41dj[date_l[i]].ToString().Substring(14, 2));
					CODE_NAME = date_l[i];
					twm41dj[date_l[i]] = twm41dj[date_l[i]].ToString().Substring(0, 10)
						+ twm41dj[date_l[i]].ToString().Substring(11, 2) + twm41dj[date_l[i]].ToString().Substring(14, 2);
					Log::Trace("", __FUNCTION__, "CODE_NAME=[{0}]C_V=[{1}]", CODE_NAME, C_V);

				}
				Log::Trace("", "", "line", __LINE__);
				if (twm41dj[date_l[i]].ToString().GetLength() == 0)
				{
					twm41dj[date_l[i]] = c_datetime;
				}
			}
			twm41dj.Insert();

			hmmsm01["MAT_NO"] = twm41dj["C_BATCHUNIT"];
			twma0["MAT_NO"] = twm41dj["C_BATCHUNIT"];
			CString stock_oper_order = "";
			if (hmmsm01.Query("MAT_NO"))//退料
			{
				twma0["STOCK_OPER_ORDER"] = "1Q";
			}
			else//来料
			{
				twma0["STOCK_OPER_ORDER"] = "1A";
			}
			if (!twma0.Query("MAT_NO,STOCK_OPER_ORDER"))
			{
				bcls_rec->Tables["WM00QUE"].Rows.Add();
				bcls_rec->Tables["WM00QUE"].Rows[0]["STOCK_OPER_ORDER"] = twma0["STOCK_OPER_ORDER"];
				bcls_rec->Tables["WM00QUE"].Rows[0]["MAT_NO"] = hmmsm01["MAT_NO"];
				bcls_rec->Tables["WM00QUE"].Rows[0]["MAT_NUM"] = "1";
				bcls_rec->Tables["WM00QUE"].Rows[0]["UNIT_CODE"] = hmmsm01["UNIT_CODE"];
				bcls_rec->Tables["WM00QUE"].Rows[0]["OPER_FLAG"] = "I";
				bcls_rec->Tables["WM00QUE"].Rows[0]["STOCK_NO"] = "SYA";
			}
			if (bcls_rec->Tables["WM00QUE"].Rows.get_Count() > 0) {
				
				doFlag = f_wm00_queue(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
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
