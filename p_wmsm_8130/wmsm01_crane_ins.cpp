/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         JHZHAO
Version:		1.0
Date:			2016-03-05
Description:	库图行车命令生成
**************************************************/

//框架头文件
#include "stdafx.h"
//#include "smhs.h"
//程序用头文件
//#include "twma0.h"
//#include "twma1.h"
//#include "twm00.h"


/*<remark>=========================================================
///<summary>
///库图行车命令生成
///<para>
===========================================================</remark>*/

//函数申明
BM2_FUNCTION_IMPORT
int f_wmsmsm_cranecmd_make(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);

BM2F_ENTERACE(wmsm01_crane_ins);

int f_wmsm01_crane_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDecimal d_thick_fr = 0;
	CDecimal d_thick_to = 0;
	CString prod_seq_no = "";
	CString v_transfer_flag = "";
	int fetchRowCount = 0;
	CDecimal rowCount = 0;
	CString crane_no("");
	CString	stock_place_no_from("");
	CString	stock_place_no_to("");
	CString stock_no_to("");
	/* 实体类定义 */
	CModel twma7 = CModel("TWMA7");
	CModel twma1 = CModel("TMMSM01");
	CModel twm04 = CModel("TWM04");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";
	CString sqlgroup = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	//系统的分页类信息。
	CPageInfo pageInfo;

	//返回数据信息
	bcls_ret->Tables[0].Columns.Add(twma1);

	try
	{
		//吊运命令生成
		EIClass bcls_rec_make;
		bcls_rec_make.Tables[0].set_TableName("CMD_MAKE");
		bcls_rec_make.Tables["CMD_MAKE"].Columns.Add(DT_STRING, "CRANE_NO");
		bcls_rec_make.Tables["CMD_MAKE"].Columns.Add(DT_STRING, "STOCK_PLACE_NO_TO");
		bcls_rec_make.Tables["CMD_MAKE"].Columns.Add(DT_STRING, "STOCK_PLACE_NO_FIN");
		bcls_rec_make.Tables["CMD_MAKE"].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");
		bcls_rec_make.Tables["CMD_MAKE"].Columns.Add(DT_STRING, "MAT_NO");
		bcls_rec_make.Tables["CMD_MAKE"].Columns.Add(DT_STRING, "HALL_NO_TO");
		bcls_rec_make.Tables["CMD_MAKE"].Columns.Add(DT_STRING, "STOCK_NO_TO");
		bcls_rec_make.Tables["CMD_MAKE"].Columns.Add(DT_STRING, "STOCK_PLACE_NO_FROM");
		bcls_rec_make.Tables["CMD_MAKE"].Columns.Add(DT_STRING, "CMD_METHOD");  //1  一吊一块 0  一吊多块
		bcls_rec_make.Tables["CMD_MAKE"].Columns.Add(DT_STRING, "MAT_STATUS");
		bcls_rec_make.Tables["CMD_MAKE"].Columns.Add(DT_STRING, "MAT_DESTION");
		bcls_rec_make.Tables["CMD_MAKE"].Columns.Add(DT_STRING, "MAT_THEORY_WT");
		bcls_rec_make.Tables["CMD_MAKE"].Columns.Add(DT_STRING, "HEAD_TAIL_WITH_DIFF"); //头尾宽度差
		bcls_rec_make.Tables["CMD_MAKE"].Rows.Clear(); 

		//获取库区授权
		CString stock_no_auth = "' '";
		EIClass *bcls_auth = new EIClass;

		delete bcls_auth;

		
		rowCount = bcls_rec->Tables[0].Rows.get_Count();
		Log::Trace("", __FUNCTION__, "rowCount [{0}]", rowCount);

	
		if (bcls_rec->Tables[1].Columns.Contains("CRANE_NO"))
			crane_no = bcls_rec->Tables[1].Rows[0]["CRANE_NO"].ToString().Trim();
		if (bcls_rec->Tables[1].Columns.Contains("STOCK_PLACE_NO_FROM"))
			stock_place_no_from = bcls_rec->Tables[1].Rows[0]["STOCK_PLACE_NO_FROM"].ToString().Trim();
		if (bcls_rec->Tables[1].Columns.Contains("STOCK_PLACE_NO_TO"))
			stock_place_no_to = bcls_rec->Tables[1].Rows[0]["STOCK_PLACE_NO_TO"].ToString().Trim();

	/*	if (bcls_rec->Tables[1].Columns.Contains("STOCK_NO_TO"))
			stock_no_to = bcls_rec->Tables[1].Rows[0]["STOCK_NO_TO"] = "SA1".ToString().Trim();*/


		Log::Info("", __FUNCTION__, "crane_no		= [{0}]", crane_no);
		Log::Info("", __FUNCTION__, "mat_no	= [{0}]", stock_place_no_from);
		Log::Trace("", __FUNCTION__, "stock_no = [{0}]", stock_place_no_to);

		for (int i = 0; i < rowCount; i++)
		{

			// 获取前台传入参数
			twma7.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			twma7.MergeFrom(bcls_rec->Tables[1].Rows[i]);

			
			
			Log::Trace("", __FUNCTION__, "twma7.STOCK_PLACE_NO_TOrrrrrrr=[{0}]", bcls_rec->Tables[1].Rows[i]["STOCK_PLACE_NO_TO"].ToString());

			Log::Trace("", __FUNCTION__, "twma7.MAT_NO=[{0}]", twma7["MAT_NO"].ToString());
			Log::Trace("", __FUNCTION__, "twma7.CRANE_NO=[{0}]", twma7["CRANE_NO"].ToString());  //吊车号
			Log::Trace("", __FUNCTION__, "twma7.STOCK_NO_TO=[{0}]", twma7["STOCK_NO_TO"].ToString());
			Log::Trace("", __FUNCTION__, "twma7.STOCK_PLACE_NO_TO=[{0}]", twma7["STOCK_PLACE_NO_TO"].ToString());
			Log::Trace("", __FUNCTION__, "twma7.STOCK_OPER_ORDER=[{0}]", twma7["STOCK_OPER_ORDER"].ToString());

			if (twma7["MAT_NO"].ToString().Trim() == "")
			{
				sprintf(s.msg, "材料不能为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			//查询材料主档、库位跟踪表
			twma1["MAT_NO"] = twma7["MAT_NO"];
			if (!twma1.Query("MAT_NO"))
			{
				sprintf(s.msg, "物料档该材料不存在");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			twm04.Reset();
			twm04["STOCK_PLACE_NO"] = twma7["STOCK_PLACE_NO_TO"];
			twm04.Query("STOCK_PLACE_NO");

			bcls_rec_make.Tables["CMD_MAKE"].Rows.Add();
			bcls_rec_make.Tables["CMD_MAKE"].Rows[i]["CRANE_NO"] = twma7["CRANE_NO"];
			bcls_rec_make.Tables["CMD_MAKE"].Rows[i]["STOCK_PLACE_NO_TO"] = twma7["STOCK_PLACE_NO_TO"];
			bcls_rec_make.Tables["CMD_MAKE"].Rows[i]["STOCK_PLACE_NO_FIN"] = " ";
			bcls_rec_make.Tables["CMD_MAKE"].Rows[i]["STOCK_OPER_ORDER"] = twma7["STOCK_OPER_ORDER"];
			bcls_rec_make.Tables["CMD_MAKE"].Rows[i]["MAT_NO"] = twma7["MAT_NO"];
			bcls_rec_make.Tables["CMD_MAKE"].Rows[i]["HALL_NO_TO"] = twm04["HALL_NO"];
			bcls_rec_make.Tables["CMD_MAKE"].Rows[i]["STOCK_NO_TO"] = twma7["STOCK_NO_TO"];
			bcls_rec_make.Tables["CMD_MAKE"].Rows[i]["STOCK_PLACE_NO_FROM"] = twma7["STOCK_PLACE_NO_FROM"];
			bcls_rec_make.Tables["CMD_MAKE"].Rows[i]["CMD_METHOD"] = "1";
			bcls_rec_make.Tables["CMD_MAKE"].Rows[i]["MAT_STATUS"] = twma1["MAT_STATUS"];
			bcls_rec_make.Tables["CMD_MAKE"].Rows[i]["MAT_DESTION"] = twma1["MAT_DESTION"];
			bcls_rec_make.Tables["CMD_MAKE"].Rows[i]["MAT_THEORY_WT"] = twma1["MAT_THEORY_WT"];
			bcls_rec_make.Tables["CMD_MAKE"].Rows[i]["HEAD_TAIL_WITH_DIFF"] = twma1["SLAB_HEAD_WIDTH"].ToDecimal() - twma1["SLAB_TAIL_WIDTH"].ToDecimal();
			Log::Trace("", __FUNCTION__, "bcls_rec_make", bcls_rec_make.Tables["CMD_MAKE"].Rows[i]["CRANE_NO"].ToString());
			//Log::Trace("", __FUNCTION__, "twma7=[{0}]", twma7);
			Log::Trace("", __FUNCTION__, "bcls_rec_make", bcls_rec_make.Tables["CMD_MAKE"].Rows[i]["STOCK_PLACE_NO_TO"].ToString());
			
			doFlag = f_wmsmsm_cranecmd_make(&bcls_rec_make, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}

			//twma7["REC_CREATOR"] = s.userid;
			//twma7["REC_CREATE_TIME"] = datetime;
			//twma7["STOCK_NO"] = twma1["STOCK_NO"];
			//twma7["MAT_KIND"] = twma1["MAT_KIND"];
			//prod_seq_no = EPGetNextSeq("WM_CMD_SEQ", conn);
			//if (doFlag < 0 || prod_seq_no.Trim() == "")
			//{
			//	sprintf(s.msg, "获取流水号失败，请查看EPED21【WM_CMD_SEQ】是否正常!");
			//	throw CApplicationException(-1, s.msg, s.svc_name);
			//}
			//twma7["CMD_SEQ"] = datetime.SubstringNE(2, 6) + prod_seq_no; // 10位 
			//twma7["CMD_SEQ"] = twma1["MAT_KIND"];

		}//end for

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };

		/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006"), arguments, 1);
		CString str = ex.GetMsg() + "\r\n" + sqlstr;

		/*返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应*/
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);

		/*数据库异常时返回-1，事务将被回滚*/
		s.flag = -1;
		doFlag = -1;
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

