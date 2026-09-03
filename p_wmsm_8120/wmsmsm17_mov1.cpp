/*************************************************************************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:
Version:
Date:
Description:
**************************************************************************************************************/
//框架公用头文件，勿删
#include "stdafx.h"
//#include "smhs.h"
//程序用头文件
//#include "twma0.h"
//#include "twma1.h"
//#include "twma2.h"
//#include "twm01.h"
//#include "twm04.h"

/*<remark>=========================================================
/// <summary>
/// 1.
/// 2.
/// <para>
/// </para>
/// <para>数据库表：TMMHP01(厚板物料主表)          </para>
/// <para>主调用函数： 前台YMHP012画面F5 倒垛      </para>
/// </summary>
/// <param name="param1">参数1  </param>
/// <param name="param2">参数2  </param>
/// <returns>返回参数：0（成功）；-1（失败）  </returns>
===========================================================</remark>*/

//调用外部函数
BM2_FUNCTION_IMPORT
int f_mmsm99(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);


// service入口
BM2F_ENTERACE(wmsmsm17_mov1)
//-EP_SYSTEM_HEAD_END
int f_wmsmsm17_mov1(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);  // 系统日志

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	int i, rows, j = 0;


	/* 业务变量 */
	CString c_heat_no = "";
	CString c_mat_no = "";
	CString c_stock_place_no_to = "";
	CDecimal c_layerno_to = 0;
	CString c_stock_place_no = "";
	CDecimal c_layerno = 0;
	CString mat_line_type = "";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString sqlstr;
	CString stock_no = "";
	CDecimal layerno = 0;
	CString mat_kind = "";


	/* 实体类定义 */


	CModel twma1 = CModel("TMMSM01");
	CModel tmmsm96 = CModel("TMMSM96");
	CModel twm04 = CModel("TWM04");
	CModel twm04_to = CModel("TWM04");


	/* 数据库操作类定义 */

	EIClass bc_99;


	try
	{
		//获取前台传入参数
		rows = bcls_rec->Tables[0].Rows.get_Count();
		Log::Trace("", __FUNCTION__, "传入记录数 = [{0}]", rows);

		/*c_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		c_mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim();
		c_stock_place_no_to = bcls_rec->Tables[0].Rows[0]["STOCK_PLACE_NO_TO"].ToString().Trim();
		c_layerno_to = bcls_rec->Tables[0].Rows[0]["LAYERNO_TO"].ToDecimal();
		c_stock_place_no = bcls_rec->Tables[0].Rows[0]["STOCK_PLACE_NO"].ToString().Trim();
		c_layerno = bcls_rec->Tables[0].Rows[0]["LAYERNO"].ToDecimal();
		


		Log::Trace("", __FUNCTION__, "传入参数c_heat_no		= [{0}]", c_heat_no);
		Log::Trace("", __FUNCTION__, "传入参数c_mat_no	= [{0}]", c_mat_no);
		Log::Trace("", __FUNCTION__, "传入参数c_stock_place_no_to		= [{0}]", c_stock_place_no_to);
		Log::Trace("", __FUNCTION__, "传入参数c_layerno_to			= [{0}]", c_layerno_to);
		Log::Trace("", __FUNCTION__, "传入参数c_stock_place_no	= [{0}]", c_stock_place_no);
		Log::Trace("", __FUNCTION__, "传入参数c_layerno			= [{0}]", c_layerno);*/
		bc_99.Tables[0].set_TableName("MM0099");
		bc_99.Tables[0].Columns.Add(tmmsm96);
		bc_99.Tables[0].Rows.Clear();

		for (i = 0; i < rows; i++)
		{
			tmmsm96.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			twma1.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			if (tmmsm96["MAT_NO"].ToString().Trim()=="")
			{
				sprintf(s.msg, "材料号不能为空！");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			twma1.Query("MAT_NO");
			tmmsm96.CopyFrom(twma1);
			tmmsm96["EVENT_ID"] = "WM03";
			tmmsm96["EVENT_LINE_TYPE"] = "SM";
			tmmsm96["FUNC_ID"] = "wmsmsm17_mov1";
			tmmsm96["SYSTEM_ID"] = "MMSM";
			tmmsm96["EVENT_DESC"] = "材料倒垛";
			tmmsm96["STOCK_PLACE_NO"]= bcls_rec->Tables[0].Rows[i]["STOCK_PLACE_NO_TO"].ToString().Trim();
			tmmsm96["LAYERNO"] = bcls_rec->Tables[0].Rows[i]["LAYERNO_TO"].ToDecimal();

			tmmsm96.MergeTo(bc_99.Tables[0]);

		}


		doFlag = f_mmsm99(&bc_99, bcls_ret, conn);
		if (doFlag != 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。" /* _RES("GCRSS0000006")*//*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
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

	return(doFlag);
}



