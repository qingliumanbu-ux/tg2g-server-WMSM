/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         ljnie
Version:		1.0
Date:			2016/7/6 15:34:36
Description:	用车申请申请函数
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"
//程序用头文件


BM2_FUNCTION_IMPORT
//函数申明
int f_epes_get_auth_other(const char *iuser, int irestype, EIClass *bcls_ret, CDbConnection * conn);

//int f_cm_8a8t01_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//发物流发送计划关闭电文 

/*<remark>=========================================================

===========================================================</remark>*/

BM2F_ENTERACE(wm0c_planCancel);

int f_wm0c_planCancel(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	EPEX epex;

	/* 程序内部变量 */
	int doFlag = 0;
	int ret = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString datetime_next = CDateTime::Now().AddDays(+1).ToString("yyyyMMddHHmmss");

	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */
	//CTWM04 twm04(conn);
	//CTWMA1 twma1_q(conn);
	CModel twm0c = CModel("TWM0C");
	CModel twm0d = CModel("TWM0D");
	CModel twm01 = CModel("TWM01");
	CModel tsi0021_to = CModel("TSI0021");
	CModel tmmsm01 = CModel("CTMMSM01");


	/* 业务变量 */
	CString oper_type("");
	CString stock_no("");
	CString aim_stock_no("");
	CString loading_plan_no("");
	CString v_loading_plan_no("");
	CString app_remark("");
	CString prod_seq_no("");
	CString truck_model("");
	CString pond_flag("");
	CString prod_code("");
	CString prod_cname("");
	CString prod_shift("");
	CString prod_group("");
	CString data("");
	CString transfer_plan_no("");
	CString balance_code("");
	CString expiry_date("");

	CDecimal vehicle_num = 0;
	CDecimal weight_app = 0;
	CDecimal stock_num = 0;

	//CString s_tc_no = "JOJL01";

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlstr1 = "";
	CString sqlwhere = "";
	CString sqlorderby = "";
	CString s_userid("");


	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{

		// 获取前台传入参数
		s_userid = s.userid;
		//获取库区授权
		CString stock_no_auth = "' '";
		EIClass *bcls_auth = new EIClass;
		if (f_epes_get_auth_other(s.userid, 5, bcls_auth, conn) != 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
		for (int fetchRowCount = 0; fetchRowCount < bcls_auth->Tables[0].Rows.get_Count(); fetchRowCount++)
		{
			stock_no_auth += ", '" + bcls_auth->Tables[0].Rows[fetchRowCount]["name"].ToString() + "' ";
		}
		delete bcls_auth;

		twm0c.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		twm0d.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		if (twm0c["PRO_FLAG"].ToString().Trim() == "C"){
			sprintf(s.msg, "该计划已被关闭！！");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//产品化 暂时注掉
		/*tmmsm01["JL_PLAN_NO"] = twm0c["LOADING_PLAN_NO"].ToString();
		tmmsm01{"IN_FLAG"] = "2";
		if (tmmsm01.QueryCount("JL_PLAN_NO,IN_FLAG") > 0)
		{
			sprintf(s.msg, "该计划还未卸车完成，不能关闭11 ！");
			throw CApplicationException(-1, s.msg, log.Location);
		}*/


		/* ***** 打印输入参数 ***** */
		Log::Trace("", __FUNCTION__, "oper_type\ttwm0c[{0}],twm0d[{1}]", twm0c["PRO_FLAG"].ToString(), twm0d["PRO_FLAG"].ToString());

		Log::Trace("", __FUNCTION__, "LOADING_PLAN_NO\t[{0}],", twm0c["LOADING_PLAN_NO"].ToString());

		twm0c["PRO_FLAG"] = "C";
		twm0d["PRO_FLAG"] = "C";

		twm0c.Update("PRO_FLAG", "LOADING_PLAN_NO");
		twm0d.Update("PRO_FLAG", "LOADING_PLAN_NO");

		//和物流的接口产品化 暂时注掉

		//CString tc_no = "8A8T01";
		////初始化
		//doFlag = epex.Initialize(tc_no);
		//if (doFlag < 0)
		//{
		//	Log::Trace("", __FUNCTION__, "epex.Initialize [{0}] code= [{1}]", tc_no, doFlag);
		//	CFormattable arguments[] = { tc_no }; // 定义参数列表的数组
		//	CMessageFormat::Format(s.msg, _RES("YM00S0000514")/*初始化电文[{0}]失败。*/, arguments, 1);
		//	throw CApplicationException(-1, s.msg, s.svc_name);
		//}
		////拼电文数据
		////操作标记
		//if (epex.SetValue("DEAL_FLAG", 0, twm0c["PRO_FLAG"].ToString().Trim()) < 0)
		//{
		//	strcpy(s.msg, _RES("GCRSS0000032")/*电文发送失败。*/);
		//	strcpy(s.sysmsg, "压入电文f_cm_8c8t01_snd的数据项DEAL_FLAG出错");
		//	Log::Trace("", __FUNCTION__, "压入电文数据DEAL_FLAG出错：[{0}]", epex.GetMsg());
		//	throw CApplicationException(-1, s.msg, s.svc_name);
		//}
		////计划号
		//if (epex.SetValue("PLAN_NO", 0, twm0c["LOADING_PLAN_NO"].ToString().Trim()) < 0)
		//{
		//	strcpy(s.msg, _RES("GCRSS0000032")/*电文发送失败。*/);
		//	strcpy(s.sysmsg, "压入电文f_cm_8c8t01_snd的数据项PLAN_NO出错");
		//	Log::Trace("", __FUNCTION__, "压入电文数据PLAN_NO出错：[{0}]", epex.GetMsg());
		//	throw CApplicationException(-1, s.msg, s.svc_name);
		//}
		////业务类型
		//if (epex.SetValue("BUSI_TYPE", 0, "1") < 0)
		//{
		//	strcpy(s.msg, _RES("GCRSS0000032")/*电文发送失败。*/);
		//	strcpy(s.sysmsg, "压入电文f_cm_8c8t01_snd的数据项BUSI_TYPE出错");
		//	Log::Trace("", __FUNCTION__, "压入电文数据BUSI_TYPE出错：[{0}]", epex.GetMsg());
		//	throw CApplicationException(-1, s.msg, s.svc_name);
		//}
		////发送电文
		//if (epex.SendTele() < 0)
		//{
		//	{
		//		CFormattable arguments[] = { epex.GetMsg() };// 定义参数列表的数组
		//		CMessageFormat::Format(s.msg, _RES("SM00S0000055")/*发送电文时失败! 原因描述： [{0}]*/, arguments, 1);//格式化字符串
		//	}
		//	//sprintf(s.msg,"发送电文时失败! 原因描述: %s", epex.GetMsg());//转换前
		//	throw	CApplicationException(-1, s.msg, log.Location);
		//}
		//epex.Uninitialize();

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
	return doFlag;

}

