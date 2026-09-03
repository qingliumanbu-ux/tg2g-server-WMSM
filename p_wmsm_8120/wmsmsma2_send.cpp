/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:
Version:		1.0
Date:			2016-03-05
Description:	来料拒收
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件
//#include "twm01.h"
//#include "twma0.h"
//#include "twma1.h"
//#include "twma2.h"

BM2_FUNCTION_IMPORT
int f_mmsm99(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);


/*<remark>=========================================================
///<summary>
///板坯入库功能
///<para>
///2.排序方式：
///</para>
///<para>数据库表：TWMA0 倒躲队列；TWMA1 物料主档表
///<returns>执行预材料预入库功能</returns> 
===========================================================</remark>*/

BM2F_ENTERACE(wmsmsma2_send);

int f_wmsmsma2_send(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CDecimal rowCount = 0;

	/*程序用变量*/
	CString mat_no = "";
	CString event_id = "";
	CString slab_place_code = "";
	CString slat_unlade_cause = "";
	CString surface_decide_code = "";
	/* 实体类定义 */
	//CTWM01 twm01(conn);
	//CTWMA0 twma0(conn);
	//CTWMA1 twma1(conn);
	//CTWMA2 twma2(conn);
	CModel twm01 = CModel("TWM01");
	CModel twma0 = CModel("TWMA0");
	CModel twma1 = CModel("TMMSM01");
	CModel twma2 = CModel("TWMA2");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);



	//调用物料函数
	

	EIClass bcls_rec_wmmm99;
	bcls_rec_wmmm99.Tables[0].set_TableName("MM0099");
	bcls_rec_wmmm99.Tables[0].Columns.Add(DT_STRING, "EVENT_ID");
	bcls_rec_wmmm99.Tables[0].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");
	bcls_rec_wmmm99.Tables[0].Columns.Add(DT_STRING, "SYSTEM_ID");
	bcls_rec_wmmm99.Tables[0].Columns.Add(DT_STRING, "FUNC_ID");
	bcls_rec_wmmm99.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
	bcls_rec_wmmm99.Tables[0].Columns.Add(DT_STRING, "SLAB_PLACE_CODE");
	bcls_rec_wmmm99.Tables[0].Columns.Add(DT_STRING, "SLAT_UNLADE_CAUSE");
	bcls_rec_wmmm99.Tables[0].Columns.Add(DT_STRING, "SURFACE_DECIDE_CODE");

	EIClass bcls_rec_QM02;//材料表面判定
	bcls_rec_QM02.Tables[0].set_TableName("MM0099");
	bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "EVENT_ID");
	bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");
	bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "SYSTEM_ID");
	bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "FUNC_ID");
	bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
	bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "SURFACE_DECIDE_CODE");
	bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "SURFACE_DECIDE_MAKER");
	bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "SURFACE_DECIDE_TIME");
	bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "DEFECT_CODE");
	bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "DEFECT_CLASS");

	EIClass bcls_rec_QM17;//材料质量封锁
	bcls_rec_QM17.Tables[0].set_TableName("MM0099");
	bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "EVENT_ID");
	bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");
	bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "SYSTEM_ID");
	bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "FUNC_ID");
	bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
	//bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "HOLD_REMARK");
	bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "REL_REMARK");
	bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "REL_MAKER");
	bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "REL_TIME");
	bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "HOLD_CAUSE_CODE");
	bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "DEFECT_CLASS");

	EIClass bcls_rec_QM18;//材料质量释放
	bcls_rec_QM18.Tables[0].set_TableName("MM0099");
	bcls_rec_QM18.Tables[0].Columns.Add(DT_STRING, "EVENT_ID");
	bcls_rec_QM18.Tables[0].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");
	bcls_rec_QM18.Tables[0].Columns.Add(DT_STRING, "SYSTEM_ID");
	bcls_rec_QM18.Tables[0].Columns.Add(DT_STRING, "FUNC_ID");
	bcls_rec_QM18.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
	//bcls_rec_QM18.Tables[0].Columns.Add(DT_STRING, "HOLD_REMARK");
	bcls_rec_QM18.Tables[0].Columns.Add(DT_STRING, "REL_REMARK");
	bcls_rec_QM18.Tables[0].Columns.Add(DT_STRING, "REL_MAKER");
	bcls_rec_QM18.Tables[0].Columns.Add(DT_STRING, "REL_TIME");
	bcls_rec_QM18.Tables[0].Columns.Add(DT_STRING, "DEFECT_CODE");
	bcls_rec_QM18.Tables[0].Columns.Add(DT_STRING, "DEFECT_CLASS");



	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
	
			mat_no = bcls_rec->Tables[0].Rows[i]["mat_no"].ToString();
			event_id = bcls_rec->Tables[0].Rows[i]["event_id"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("SLAB_PLACE_CODE"))
			{
				slab_place_code = bcls_rec->Tables[0].Rows[i]["SLAB_PLACE_CODE"].ToString();
			}
			if (bcls_rec->Tables[0].Columns.Contains("SLAT_UNLADE_CAUSE"))
			{
				slat_unlade_cause = bcls_rec->Tables[0].Rows[i]["SLAT_UNLADE_CAUSE"].ToString();
			}
			if (bcls_rec->Tables[0].Columns.Contains("SURFACE_DECIDE_CODE"))
			{
				surface_decide_code = bcls_rec->Tables[0].Rows[i]["SURFACE_DECIDE_CODE"].ToString();
			}
			
			
			Log::Trace("", __FUNCTION__, "参数赋值mat_no：\t[{0}]", mat_no);
			Log::Trace("", __FUNCTION__, "参数赋值event_id：\t[{0}]", event_id);
			Log::Trace("", __FUNCTION__, "参数赋值slab_place_code：\t[{0}]", slab_place_code);
			Log::Trace("", __FUNCTION__, "参数赋值slat_unlade_cause：\t[{0}]", slat_unlade_cause);
			Log::Trace("", __FUNCTION__, "参数赋值surface_decide_code：\t[{0}]", surface_decide_code);
			
			if (event_id != "WMBP")
			{
				//调用物料跟踪
				bcls_rec_wmmm99.Tables[0].Rows.Clear();
				bcls_rec_wmmm99.Tables[0].Rows.Add();
				bcls_rec_wmmm99.Tables[0].Rows[0]["EVENT_ID"] = event_id;
				bcls_rec_wmmm99.Tables[0].Rows[0]["EVENT_LINE_TYPE"] = "SM";
				bcls_rec_wmmm99.Tables[0].Rows[0]["SYSTEM_ID"] = "MMSM";
				bcls_rec_wmmm99.Tables[0].Rows[0]["FUNC_ID"] = "wmsmsma2_send";
				bcls_rec_wmmm99.Tables[0].Rows[0]["MAT_NO"] = mat_no;
				bcls_rec_wmmm99.Tables[0].Rows[0]["SLAB_PLACE_CODE"] = slab_place_code;
				bcls_rec_wmmm99.Tables[0].Rows[0]["SLAT_UNLADE_CAUSE"] = slat_unlade_cause;
				bcls_rec_wmmm99.Tables[0].Rows[0]["SURFACE_DECIDE_CODE"] = surface_decide_code;

				doFlag = f_mmsm99(&bcls_rec_wmmm99, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			else if (event_id == "WMBP"&& surface_decide_code == "1")
			{
				bcls_rec_wmmm99.Tables[0].Rows.Clear();
				bcls_rec_wmmm99.Tables[0].Rows.Add();
				bcls_rec_wmmm99.Tables[0].Rows[0]["EVENT_ID"] = event_id;
				bcls_rec_wmmm99.Tables[0].Rows[0]["EVENT_LINE_TYPE"] = "SM";
				bcls_rec_wmmm99.Tables[0].Rows[0]["SYSTEM_ID"] = "MMSM";
				bcls_rec_wmmm99.Tables[0].Rows[0]["FUNC_ID"] = "wmsmsma2_send";
				bcls_rec_wmmm99.Tables[0].Rows[0]["MAT_NO"] = mat_no;
			
				doFlag = f_mmsm99(&bcls_rec_wmmm99, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}

				bcls_rec_QM02.Tables[0].Rows.Clear();
				bcls_rec_QM02.Tables[0].Rows.Add();
				bcls_rec_QM02.Tables[0].Rows[0]["EVENT_ID"] = "QM02";
				bcls_rec_QM02.Tables[0].Rows[0]["EVENT_LINE_TYPE"] = "00";
				bcls_rec_QM02.Tables[0].Rows[0]["SYSTEM_ID"] = "MMSM";
				bcls_rec_QM02.Tables[0].Rows[0]["FUNC_ID"] = "wmsmsma2_send";
				bcls_rec_QM02.Tables[0].Rows[0]["MAT_NO"] = mat_no;;
				bcls_rec_QM02.Tables[0].Rows[0]["SURFACE_DECIDE_CODE"] = "1";// 1:合格
				bcls_rec_QM02.Tables[0].Rows[0]["SURFACE_DECIDE_MAKER"] = s.userid;
				bcls_rec_QM02.Tables[0].Rows[0]["SURFACE_DECIDE_TIME"] = datetime;
				bcls_rec_QM02.Tables[0].Rows[0]["DEFECT_CODE"] = " ";
				bcls_rec_QM02.Tables[0].Rows[0]["DEFECT_CLASS"] = " ";

				doFlag = f_mmsm99(&bcls_rec_QM02, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}

				bcls_rec_QM18.Tables[0].Rows.Clear();
				bcls_rec_QM18.Tables[0].Rows.Add();
				bcls_rec_QM18.Tables[0].Rows[0]["EVENT_ID"] = "QM18";
				bcls_rec_QM18.Tables[0].Rows[0]["EVENT_LINE_TYPE"] = "00";
				bcls_rec_QM18.Tables[0].Rows[0]["SYSTEM_ID"] = "MMSM";
				bcls_rec_QM18.Tables[0].Rows[0]["FUNC_ID"] = "wmsmsma2_send";
				bcls_rec_QM18.Tables[0].Rows[0]["MAT_NO"] = mat_no;
				bcls_rec_QM18.Tables[0].Rows[0]["REL_REMARK"] = "表判合格解封锁";
				bcls_rec_QM18.Tables[0].Rows[0]["REL_MAKER"] = s.userid;
				bcls_rec_QM18.Tables[0].Rows[0]["REL_TIME"] = datetime;
				bcls_rec_QM18.Tables[0].Rows[0]["DEFECT_CODE"] = " ";
				bcls_rec_QM18.Tables[0].Rows[0]["DEFECT_CLASS"] = " ";

				doFlag = f_mmsm99(&bcls_rec_QM18, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			else if (event_id == "WMBP" && surface_decide_code == "2")
			{
				bcls_rec_wmmm99.Tables[0].Rows.Clear();
				bcls_rec_wmmm99.Tables[0].Rows.Add();
				bcls_rec_wmmm99.Tables[0].Rows[0]["EVENT_ID"] = event_id;
				bcls_rec_wmmm99.Tables[0].Rows[0]["EVENT_LINE_TYPE"] = "SM";
				bcls_rec_wmmm99.Tables[0].Rows[0]["SYSTEM_ID"] = "MMSM";
				bcls_rec_wmmm99.Tables[0].Rows[0]["FUNC_ID"] = "wmsmsma2_send";
				bcls_rec_wmmm99.Tables[0].Rows[0]["MAT_NO"] = mat_no;

				doFlag = f_mmsm99(&bcls_rec_wmmm99, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}

				bcls_rec_QM02.Tables[0].Rows.Clear();
				bcls_rec_QM02.Tables[0].Rows.Add();
				bcls_rec_QM02.Tables[0].Rows[0]["EVENT_ID"] = "QM02";
				bcls_rec_QM02.Tables[0].Rows[0]["EVENT_LINE_TYPE"] = "00";
				bcls_rec_QM02.Tables[0].Rows[0]["SYSTEM_ID"] = "MMSM";
				bcls_rec_QM02.Tables[0].Rows[0]["FUNC_ID"] = "wmsmsma2_send";
				bcls_rec_QM02.Tables[0].Rows[0]["MAT_NO"] = mat_no;;
				bcls_rec_QM02.Tables[0].Rows[0]["SURFACE_DECIDE_CODE"] = "2";
				bcls_rec_QM02.Tables[0].Rows[0]["SURFACE_DECIDE_MAKER"] = s.userid;
				bcls_rec_QM02.Tables[0].Rows[0]["SURFACE_DECIDE_TIME"] = datetime;
				bcls_rec_QM02.Tables[0].Rows[0]["DEFECT_CODE"] = " ";
				bcls_rec_QM02.Tables[0].Rows[0]["DEFECT_CLASS"] = " ";

				doFlag = f_mmsm99(&bcls_rec_QM02, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}

				bcls_rec_QM17.Tables[0].Rows.Clear();
				bcls_rec_QM17.Tables[0].Rows.Add();
				bcls_rec_QM17.Tables[0].Rows[0]["EVENT_ID"] = "QM17";
				bcls_rec_QM17.Tables[0].Rows[0]["EVENT_LINE_TYPE"] = "00";
				bcls_rec_QM17.Tables[0].Rows[0]["SYSTEM_ID"] = "MMSM";
				bcls_rec_QM17.Tables[0].Rows[0]["FUNC_ID"] = "wmsmsma2_send";
				bcls_rec_QM17.Tables[0].Rows[0]["MAT_NO"] = mat_no;
				bcls_rec_QM17.Tables[0].Rows[0]["REL_REMARK"] = "表判不合封锁";
				bcls_rec_QM17.Tables[0].Rows[0]["REL_MAKER"] = s.userid;
				bcls_rec_QM17.Tables[0].Rows[0]["REL_TIME"] = datetime;
				bcls_rec_QM17.Tables[0].Rows[0]["HOLD_CAUSE_CODE"] = "WMBP";
				bcls_rec_QM17.Tables[0].Rows[0]["DEFECT_CLASS"] = " ";

				doFlag = f_mmsm99(&bcls_rec_QM17, bcls_ret, conn);
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
