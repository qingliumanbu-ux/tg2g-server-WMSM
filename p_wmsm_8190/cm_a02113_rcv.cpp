/*
*  程序名称			: cm_a02111_rcv
*  程序描述			: 铁路临钢坯配车反馈
*
*  	2024-1-25 	nyy			(ADD)程序建立
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


BM2F_ENTERACE_TELE(cm_a02113_rcv)
int f_cm_a02113_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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

	try
	{

		CString	  DEAL_FLAG = bcls_rec->Tables["A02113"].Rows[0]["DEAL_FLAG"].ToString().Trim();
		CString	  PLAN_NO = bcls_rec->Tables["A02113"].Rows[0]["PLAN_NO"].ToString().Trim();
		CString	  JCC_FLAG = bcls_rec->Tables["A02113"].Rows[0]["JCC_FLAG"].ToString().Trim();
		if (PLAN_NO.Trim() == "")
		{
			sprintf(s.msg, "计划号不能为空，电文接收失败");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		
		//判断计划号在汽运非销售出厂计划表中是否存在
		twmsm30["PLAN_NO"] = PLAN_NO;
		if (twmsm30.QueryCount("PLAN_NO") > 0)
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
			int count = bcls_rec->Tables[1].Rows.get_Count();
			if (DEAL_FLAG == "I")
			{
				Log::Trace("", "__FUNCTION__", "insert count  = [{0}]", count);
				/*practiceID = "LG6240" + CDateTime::Now().ToString("yyyyMMdd") + EPGetNextSeq("ATCC_SEQNO", conn);
				Log::Trace("", "", "practiceID=[{0}]", practiceID);*/
				for (int i = 0; i < count; i++)
				{
					Log::Trace("", "__FUNCTION__", "循环i = [{0}] ", i);
					
					twmsm32.MergeFrom(bcls_rec->Tables[0].Rows[0]);
					twmsm32m.MergeFrom(bcls_rec->Tables[1].Rows[i]);
					PLAN_NO = bcls_rec->Tables[0].Rows[i]["PLAN_NO"].ToString().Trim();
					MISSION_NO = bcls_rec->Tables[0].Rows[0]["MISSION_NO"].ToString().Trim();
					JCC_FLAG = bcls_rec->Tables[0].Rows[0]["JCC_FLAG"].ToString().Trim();
					MEMO1 = bcls_rec->Tables[0].Rows[0]["MEMO1"].ToString().Trim();
					MEMO2 = bcls_rec->Tables[0].Rows[0]["MEMO2"].ToString().Trim();
					TRUCK_NO = bcls_rec->Tables[0].Rows[0]["TRUCK_NO"].ToString().Trim();
					TRUCK_MODEL = bcls_rec->Tables[0].Rows[0]["TRUCK_MODEL"].ToString().Trim();//车型和承运商需要物流在第三方基础表中查到后转发
					//CARRIER_DES = bcls_rec->Tables[0].Rows[0]["CARRIER_DES"].ToString().Trim();
					//通过车号去第三方基础表中寻找车型代码和承运商
					if (TRUCK_MODEL == "")
					{
						TRUCK_MODEL = " ";
					}
					//判断计划号在汽运非销售出厂计划表中是否存在

					if (twmsm32m["MAT_NO"].ToString().TrimOrBlank() != "")
					{
						twmsm30m["PLAN_NO"] = PLAN_NO;
						twmsm30m["MAT_NO"] = twmsm32m["MAT_NO"].ToString().TrimOrBlank();
						if (twmsm30m.Query("PLAN_NO,MAT_NO") == true)
						{
							twmsm32m["FACTORY_DIV1"] = twmsm30m["FACTORY_DIV"].ToString().TrimOrBlank();
							twmsm32m["AREA_CODE"] = twmsm30m["AREA_CODE"].ToString().TrimOrBlank();
							twmsm32m["LOAD_CODE"] = twmsm30m["LOAD_CODE"].ToString().TrimOrBlank();
							twmsm32m["STORE_PLACE"] = twmsm30m["STORE_PLACE"].ToString().TrimOrBlank();
							twmsm32m["PROD_ORDER_NO"] = twmsm30m["PROD_ORDER_NO"].ToString().TrimOrBlank();
							twmsm32m["REMARK1"] = twmsm30m["REMARK1"].ToString().TrimOrBlank();
							twmsm32m["REMARK2"] = twmsm30m["REMARK2"].ToString().TrimOrBlank();
							/*twmsm32m["PRACTICE_NO"] = practiceID;*/
							if (twmsm32m["FACTORY_DIV1"].ToString().Trim() == "")
							{
								twmsm32m["FACTORY_DIV1"] = " ";
							}
							if (twmsm32m["AREA_CODE"].ToString().Trim() == "")
							{
								twmsm32m["AREA_CODE"] = " ";
							}
							if (twmsm32m["LOAD_CODE"].ToString().Trim() == "")
							{
								twmsm32m["LOAD_CODE"] = " ";
							}
							if (twmsm32m["STORE_PLACE"].ToString().Trim() == "")
							{
								twmsm32m["STORE_PLACE"] = " ";
							}
							if (twmsm32m["PROD_ORDER_NO"].ToString().Trim() == "")
							{
								twmsm32m["PROD_ORDER_NO"] = " ";

							}
							if (twmsm32m["REMARK1"].ToString().Trim() == "")
							{
								twmsm32m["REMARK1"] = " ";
							}
							if (twmsm32m["REMARK2"].ToString().Trim() == "")
							{
								twmsm32m["REMARK2"] = " ";
							}
						}

						twmsm32m["REC_CREATOR"] = "A02113";
						twmsm32m["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");

						twmsm32m["MISSION_NO"] = MISSION_NO;
						twmsm32m.Insert();
					}
					
					twmsm32["PLAN_NO"] = PLAN_NO;
					
					Log::Trace("", "", "MISSION_NO=[{0}]", MISSION_NO);
					Log::Trace("", "", "PLAN_NO=[{0}]", PLAN_NO);
					if (twmsm32.QueryCount("MISSION_NO,PLAN_NO") <= 0)
					{
						//新增主信息用车实绩
						Log::Trace("", "", "新增主信息用车实绩");
						twmsm30.Query("PLAN_NO");
						//设置记录者信息
						twmsm32.CopyFrom(twmsm30);
						//twmsm32["PRACTICE_NO"] = practiceID;
						twmsm32["MISSION_NO"] = MISSION_NO;
						twmsm32["REC_CREATOR"] = "J0A006";
						twmsm32["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
						twmsm32["CREATION_TIME"] = CDateTime::Now().ToString("yyyyMMdd");	
						twmsm32["CARRIER_DES"] = CARRIER_DES;
						twmsm32["TRUCK_MODEL"] = TRUCK_MODEL;
						if (MEMO1 != "")
						{
							twmsm32["DRIVER_NAME"] = MEMO1;
						}
						if (MEMO2 != "")
						{
							twmsm32["TELPHONE"] = MEMO2;
						}

						twmsm32["STATUS"] = "1";
						twmsm32.Insert();
					}
				}
			}
			if (DEAL_FLAG == "D")
			{
				twmsm32.MergeFrom(bcls_rec->Tables[0].Rows[0]);
				twmsm32["STATUS"] = "10";
				twmsm32.Update("STATUS", "MISSION_NO");
			}

			Log::Trace("", "", "结束汽运非销售出厂");
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
