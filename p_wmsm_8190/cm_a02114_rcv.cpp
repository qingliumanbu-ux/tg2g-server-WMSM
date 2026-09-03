/*
*  程序名称			: cm_a02114_rcv
*  程序描述			: 铁路临钢坯配车反馈
*
*  	2024-1-25 	lizhen			(ADD)程序建立
*			... ...
* **************************************************************************** */
/*<remark>=========================================================
<summary>
铁路临钢坯装车实绩
1、记录装车实绩
2、材料归档
3、通知制造
<para>数据库表：TWMSM61LG(装车实绩表(临钢))         </para>
</summary>
<returns>电文处理成功与否</returns>
===========================================================</remark>*/

/* C/C++ 的标准头文件部分 */


#include "stdafx.h"
#include "epex.h"

int f_mmsm99(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wmsm_t80ry0_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);



BM2F_ENTERACE_TELE(cm_a02114_rcv)
int f_cm_a02114_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);
	/* ***** 静态变量定义 ***** */
	int doFlag = 0;
	int fetchRowCount = 0;
	int row_count = 0, i = 0, ret = 0;


	CString lpsz_user_id, c_datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString	lpsz_out_div;
	CString	practiceID;
	EIClass sm_bcls_rec;

	CModel twmsm32("TWMSM32");

	CModel twmsm32m("TWMSM32M");
	CModel twmsm30("TWMSM30");
	CModel twmsm30m("TWMSM30M");

	CModel twm41dj("TWM41DJ");
	CModel tmmsm01("TMMSM01");
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
	CDbCommand cmd_inq(conn);

	EIClass mm0099;
	mm0099.Tables[0].set_TableName("MM0099");
	mm0099.Tables[0].Columns.Add(tmmsm96);
	mm0099.Tables[0].Rows.Clear();

	try
	{

		CString	  DEAL_FLAG = bcls_rec->Tables[0].Rows[0]["DEAL_FLAG"].ToString().Trim();
		CString	  MISSION_NO = bcls_rec->Tables[0].Rows[0]["MISSION_NO"].ToString().Trim();
		CString	  JCC_FLAG = bcls_rec->Tables[0].Rows[0]["JCC_FLAG"].ToString().Trim();
		if (MISSION_NO.Trim() == "")
		{
			sprintf(s.msg, "计划号不能为空，电文接收失败");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//判断计划号在汽运非销售出厂计划表中是否存在
		twmsm32["MISSION_NO"] = MISSION_NO;
		if (twmsm32.QueryCount("MISSION_NO") > 0)
		{
			Log::Trace("", "", "进入汽运非销售出厂");

			CString    MISSION_NO = "";
			CString    MATERIAL_CODE = "";
			CString    MATERIAL_NAME = "";

			CString    MEMO1 = "";
			CString    MEMO2 = "";
			CString	  TRUCK_MODEL = " ";
			CString	  CARRIER_DES = " ";
			CString	  TRUCK_NO = " ";
			CString sql = "";
			//int count = bcls_rec->Tables[1].Rows.get_Count();
			if (DEAL_FLAG == "I")
			{
				//Log::Trace("", "__FUNCTION__", "insert count  = [{0}]", count);
				/*practiceID = "LG6240" + CDateTime::Now().ToString("yyyyMMdd") + EPGetNextSeq("ATCC_SEQNO", conn);
				Log::Trace("", "", "practiceID=[{0}]", practiceID);*/
				
				
				Log::Trace("", "__FUNCTION__", "循环i = [{0}] ", i);

				twmsm32.MergeFrom(bcls_rec->Tables[0].Rows[0]);
				twmsm32m.MergeFrom(bcls_rec->Tables[0].Rows[0]);
				twmsm32m["STATUS"] = "9";
				twmsm32m.Update("STATUS", "MISSION_NO");

				sql = " select * from twmsm32m where MISSION_NO='" + twmsm32m["MISSION_NO"].ToString() + "' and mat_no!=' ' ";
				Log::Trace("", "__FUNCTION__", "sql = [{0}] ", sql);
				cmd_inq.SetCommandText(sql);
				cmd_inq.ExecuteReader();
				while (cmd_inq.Read())
				{
					cmd_inq.Fetch(tmmsm01);
					tmmsm01.Query("MAT_NO");
					tmmsm96.Reset();
					tmmsm96.CopyFrom(tmmsm01);
					tmmsm96["LOGISTICS_STATUS"] = "3";//2--装车确认
				
					tmmsm96["EVENT_ID"] = "MM77";
					tmmsm96["SYSTEM_ID"] = "MMSM";
					tmmsm96["EVENT_LINE_TYPE"] = "00";
					tmmsm96["FUNC_ID"] = s.svc_name;
					tmmsm96.MergeTo(mm0099.Tables["MM0099"], false);
					Log::Info("", __FUNCTION__, "f_mmsm10_trace_linke   =[{0}]", __LINE__);
				}
				cmd_inq.Close();

				twm41dj.Reset();
				
				twm41dj["REC_CREATOR"] = s.userid;
				twm41dj["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				twm41dj["C_DELIVERYID"] = "6240" + c_datetime.SubstringNE(0, 8) + Db::QueryCString("SELECT LPAD(TO_CHAR(DB_NO.NEXTVAL), 4, '0') FROM DUAl");
				twm41dj["C_QULITYTRACEID"] = tmmsm01["HEAT_NO"];//炉号
				twm41dj["C_BATCHID"] = tmmsm01["BATCH"];//批次号
				twm41dj["C_BATCHUNIT"] = tmmsm01["MAT_NO"];//
				twm41dj["C_SENDDEPT"] = "6240";//发送工厂
				twm41dj["C_ACCEPTDEPT"] = "6380";//接受工厂1
				
				twm41dj["C_SENDSTOCK"] = tmmsm01["LGORT"];//发送库房
				twm41dj["C_ACCEPTSTOCK"] = "6381";//接受库房
				twm41dj["DELIVERY_THICKNESS"] = tmmsm01["MAT_THICK"];//厚度
				twm41dj["DELIVERY_WIDTH"] = tmmsm01["MAT_WIDTH"];//宽度1
				twm41dj["STEELGRADE"] = tmmsm01["ST_NO"];//钢牌号
				twm41dj["N_SENDAMOUNT"] = tmmsm01["MAT_ACT_WT"];//发送重量
				twm41dj["C_SENDUNIT"] = "TON";//发送单位
				twm41dj["N_ACCEPTAMOUNT"] = tmmsm01["MAT_WT"];//接收重量
				twm41dj["C_ACCEPTUNIT"] = "TON";//接收单位
				twm41dj["C_STATESIGN"] = "1";//调拨状态（1-未确认，2-接收，3-驳回）
				twm41dj["D_OPERATIONDATE"] = c_datetime;
				twm41dj["D_BILLDATE"] = c_datetime;
				Log::Info("", __FUNCTION__, "f_mmsm10_trace_linke   =[{0}]", __LINE__);
				twm41dj["T_OUTSTOCKTIME"] = c_datetime;
				
				{
					twm41dj["T_ACCEPTTIME"] = c_datetime;
					twm41dj["C_CLOSEGATETIME"] = c_datetime;
					twm41dj["T_UPLOADTIME"] = c_datetime;
					twm41dj["T_INSTOCKTIME"] = c_datetime;
					twm41dj["T_SALESCOMFIRMTIME"] = c_datetime;
					twm41dj["T_OVERRULETIME"] = c_datetime;
					twm41dj["D_REQUIREDATE"] = c_datetime;
				}
				twm41dj["I_STOCKMODE"] = "件次";
				twm41dj["C_REMARK"] = tmmsm01["SG_GRADE_1"];
				twm41dj["I_RESERVECOL4"] = "0";//调拨类型（0-正常调拨，1-回退调拨）
				twm41dj["C_INSTOCKSIGN"] = "3";
				twm41dj["C_ISFREEZE"] = "FREE";//库存类型-
				Log::Info("", __FUNCTION__, "f_mmsm10_trace_linke   =[{0}]", __LINE__);
				twm41dj["C_STOCKSPEC"] = "FREE";//特殊库存标识
				twm41dj["C_ORDERID"] = tmmsm01["ORDER_NO"];//合同号
				twm41dj["I_RESERVECOL3"] = tmmsm01["MAT_LEN"];
				twm41dj["C_ACHIEVEID"] = "1";
				twm41dj["C_TRUCKNUM"] = bcls_rec->Tables[0].Rows[0]["TRUCK_NO"].ToString();
				Log::Info("", __FUNCTION__, "f_mmsm10_trace_linke   =[{0}]", __LINE__);
				if (tmmsm01["PRODUCT_FLAG"].ToString() == "1")
				{
					
						twm41dj["C_PRODUCTID"] = "FAA000000000000000";
						twm41dj["C_PRODUCTNAME"] = "连铸卷板坯";
					
				}
				else
				{
					
						twm41dj["C_PRODUCTID"] = "HAA000000000000000";
						twm41dj["C_PRODUCTNAME"] = "连铸卷板坯";
					
				}
				Log::Info("", __FUNCTION__, "f_mmsm10_trace_linke   =[{0}]", __LINE__);
				
				twm41dj.Insert();
				sql = " select * from twm41dj where C_BATCHID='" + tmmsm01["MAT_NO"].ToString() + "' and C_STATESIGN='1' ";
				Log::Trace("", "__FUNCTION__", "sql = [{0}] ", sql);
				cmd_inq.SetCommandText(sql);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					cmd_inq.Fetch(twm41dj);
					if ((CDateTime::Now() - CDateTime::Parse(tmmsm01["SLAB_CUT_TIME"].ToString())).TotalHours() <= 6 && tmmsm01["ST_NO"].ToString().SubstringNE(0, 1) == "1")//不锈
					{
						tmmsm96["C_ISHOTSEND"] = "1";
					}
					if ((CDateTime::Now() - CDateTime::Parse(tmmsm01["SLAB_CUT_TIME"].ToString())).TotalHours() <= 4 && tmmsm01["ST_NO"].ToString().SubstringNE(0, 1) == "2")//碳
					{
						tmmsm96["C_ISHOTSEND"] = "1";
					}
					twm41dj["C_STATESIGN"] = "3";
					twm41dj["C_ISHOTSEND"] = tmmsm96["C_ISHOTSEND"];
					twm41dj.Update("C_STATESIGN,C_ISHOTSEND", "C_DELIVERYID");

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
				cmd_inq.Close();
			}
			if (DEAL_FLAG == "D")
			{
				twmsm32m.MergeFrom(bcls_rec->Tables[0].Rows[0]);
				twmsm32m["STATUS"] = "10";
				twmsm32m.Update("STATUS", "MISSION_NO");

				sql = " select * from twmsm32m where MISSION_NO='" + twmsm32m["MISSION_NO"].ToString() + "' ";
				cmd_inq.SetCommandText(sql);
				cmd_inq.ExecuteReader();
				while (cmd_inq.Read())
				{
					cmd_inq.Fetch(tmmsm01);
					tmmsm01.Query("MAT_NO");
					tmmsm96.Reset();
					tmmsm96.CopyFrom(tmmsm01);
					tmmsm96["LOGISTICS_STATUS"] = "4";//2--装车确认

					tmmsm96["EVENT_ID"] = "MM77";
					tmmsm96["SYSTEM_ID"] = "MMSM";
					tmmsm96["EVENT_LINE_TYPE"] = "00";
					tmmsm96["FUNC_ID"] = s.svc_name;
					tmmsm96.MergeTo(mm0099.Tables["MM0099"], false);
					Log::Info("", __FUNCTION__, "f_mmsm10_trace_linke   =[{0}]", __LINE__);
				}
				cmd_inq.Close();
			}

			Log::Trace("", "", "结束汽运非销售出厂");
		}
		if (mm0099.Tables["MM0099"].Rows.get_Count() > 0)
		{
			doFlag = f_mmsm99(&mm0099, bcls_ret, conn);
			if (doFlag != 0)
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
