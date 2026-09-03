/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:
Version:		1.0
Date:			2016-03-05
Description:	出库功能
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"

//程序用头文件
//#include "twma0.h"
//#include "twma1.h"
//#include "twma2.h"
//#include "twm01.h"



//函数申明
int f_mmsm99(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);

/*<remark>=========================================================
///<summary>
///板坯出库功能
///<para>
///2.排序方式：
///</para>
///<para>数据库表：TWMA0 倒躲队列；TWMA1 物料主档表
///<returns>执行预材料预入库功能</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wmsm302)
int f_wmsm302(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	EPEX epex;

	int doFlag = 0;
	int ret = 0;
	int blkNum = 0;
	int row_plan = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	int ii = 0;

	CDecimal rowCount = 0;
	CString aim_stock_no = "";
	CString out_stock_time = "";
	CString trnp_mode_code = "";
	CString truck_no = "";
	CString if_wt = "";
	CString dest_fin = "";
	CString aim_store = "";
	CString mat_line_type = "";
	CString remark0 = "";
	CString remark1 = "";
	CString store_keeper = "";
	CString crane_resp = "";
	CString crane_no = "";

	CString	update_data = "";
	CString	condition_data = "";
	CDecimal layerno1 = 0;

	CString SEQ_ID = "";                 //顺序号 件数
	CString LAYERNO = "";                 //层号
	CString STOCK_PLACE_POSITION = "";  //车内顺序号
	CString	load_scheme_no = "";//预装方案
	/* 实体类定义 */
	//CTWMA0 twma0(conn);
	//CTWMA1 twma1(conn);
	//CTWMA2 twma2(conn);
	//CTWM01 twm01(conn);
	CModel twma0 = CModel("TWMA0");
	CModel twma1 = CModel("TMMSM01");
	CModel tmmsm01 = CModel("TMMSM01");
	CModel hmmsm01 = CModel("HMMSM01");
	CModel twma2 = CModel("TWMA2");
	CModel twm01 = CModel("TWM01");
	CModel twm41dj = CModel("TWM41DJ");
	CModel tmmsm96 = CModel("TMMSM96");
	CModel twmsm12 = CModel("TWMSM12");
	CModel hwmsm12 = CModel("HWMSM12");
	CString v_datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CString s_tc_no = "T80RY0";


	//调用仓库入库主函数


	EIClass mm0099;
	mm0099.Tables[0].set_TableName("MM0099");
	mm0099.Tables[0].Columns.Add(tmmsm96);
	mm0099.Tables[0].Rows.Clear();



	try
	{
		//初始化
		ret = epex.Initialize(s_tc_no);
		if (ret < 0)
		{
			CFormattable arguments[] = { s_tc_no }; // 定义参数列表的数组
			CMessageFormat::Format(s.msg, _RES("YM00S0000514")/*初始化电文[{0}]失败。*/, arguments, 1);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			hmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			if (!hmmsm01.Query("MAT_NO"))
			{
				sprintf(s.msg, "材料号不存在", epex.GetMsg());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}


			

			//拼电文数据
			if (epex.SetValue("MAT_NO", 0, hmmsm01["MAT_NO"].ToString()) < 0
				|| epex.SetValue("DEAL_FLAG", 0, "I") < 0
				|| epex.SetValue("PLANT", 0, bcls_rec->Tables[0].Rows[i]["C_DELIVERY_FAC"].ToString()) < 0
				|| epex.SetValue("STGE_LOC", 0, bcls_rec->Tables[0].Rows[i]["C_DELIVERY_STOCK"].ToString()) < 0
				|| epex.SetValue("MOVE_PLANT", 0, "6240") < 0
				|| epex.SetValue("MOVE_STLOC", 0, "6242") < 0
				|| epex.SetValue("HEAD_TEXT", 0, " ") < 0)
			{
				strcpy(s.msg, _RES("GCRSS0000015"));//系统出现异常，电文拼接出错，请联系系统维护人员。
				throw CApplicationException(-1, s.msg, s.svc_name);
			}





			if (epex.SendTele() < 0)
			{
				strcpy(s.msg, _RES("GCRSS0000032")/*电文发送失败。*/);
				sprintf(s.sysmsg, "[%s]发送失败", (const char*)s_tc_no);
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}


		

		epex.Uninitialize();




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

