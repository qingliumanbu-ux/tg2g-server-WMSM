/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         JHZHAO
Version:		1.0
Date:			2016-03-05
Description:	称重请求
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
///库图板坯信息查询
///<para>
===========================================================</remark>*/

BM2F_ENTERACE(wmsm01_wt_snd);

int f_wmsm01_wt_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDecimal d_thick_fr = 0;
	CDecimal d_thick_to = 0;
	CString v_transfer_flag = "";
	int fetchRowCount = 0;
	CDecimal rowCount = 0;

	/* 实体类定义 */
	CModel twma1_q = CModel("TMMSM01");
	CModel twma1 = CModel("TMMSM01");

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


	EIClass bcls_rec_f;
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "SEND_POINT");
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "TOTAL");
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "MAT_STATUS");

	try
	{

		// 获取前台传入参数

		//获取库区授权
		CString stock_no_auth = "' '";
		EIClass *bcls_auth = new EIClass;

		delete bcls_auth;


		//分页信息
		CDataTable& table = bcls_ret->Tables.Add("PAGEINFO");
		table.Columns.Add(DT_DECIMAL, "recordsum");

		//2)获取分页信息
		if (bcls_rec->Tables.Contains("PageInfo"))
		{
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		else
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = -1;  //每页记录数量
		}

		Log::Trace("", __FUNCTION__, "pageInfo.RecordFrom[{0}]pageInfo.PageSize[{1}]", pageInfo.RecordFrom, pageInfo.PageSize);


		twma1_q.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		twma1_q.TrimOrBlank();

		/*if (bcls_rec->Tables[0].Columns.Contains("MAT_THICK_FR"))
		d_thick_fr = bcls_rec->Tables[0].Rows[0]["MAT_THICK_FR"].ToDecimal();*/


		Log::Trace("", __FUNCTION__, "twma1_q.MAT_NO=[{0}]", twma1_q["MAT_NO"].ToString());

		twma1["MAT_NO"] = twma1_q["MAT_NO"];
		if (!twma1.Query("MAT_NO"))
		{
			sprintf(s.msg, "材料号不存在！");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (twma1["SURFACE_DECIDE_CODE"].ToString().Trim() != "1")
		{
			sprintf(s.msg, "板坯表面判定不合格或者封锁的！");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (twma1["IN_FLAG"].ToString().Trim() != "1")
		{
			sprintf(s.msg, "板坯已经送出，请确认！");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (twma1["MAT_STATUS"].ToString().Trim() == "24")
		{
			sprintf(s.msg, "材料在转库计划中，请先取消计划或倒出板坯！");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (twma1["SLAB_HEAD_WIDTH"].ToDecimal() > 1320 || twma1["SLAB_TAIL_WIDTH"].ToDecimal() > 1320)
		{
			sprintf(s.msg, "超宽板坯，请先倒出板坯！");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		/*if (tmmsm01.slab_head_width > 1320 || tmmsm01.slab_tail_width > 1320)
		{
			EDLog(1, 1, "垛位中有超宽板坯，请先倒出板坯");
			sprintf(s.msg, "%s", "垛位中有超宽板坯，请先倒出板坯");
			s.flag = -1;
			goto l_return;
		}*/
		if (twma1["MAT_ACT_LEN"].ToDecimal() > 9560)
		{
			sprintf(s.msg, "超长板坯，请先倒出板坯！");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (twma1["MAT_ACT_LEN"].ToDecimal() > 4600 && twma1["MAT_ACT_LEN"].ToDecimal() < 7000)
		{
			sprintf(s.msg, "11超长板坯，请先倒出板坯！");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (twma1["SLAB_HEAD_WIDTH"].ToDecimal() - twma1["SLAB_TAIL_WIDTH"].ToDecimal() > 50 || twma1["SLAB_TAIL_WIDTH"].ToDecimal() - twma1["SLAB_HEAD_WIDTH"].ToDecimal() > 50)
		{
			sprintf(s.msg, "头尾宽差超宽板坯，请先倒出板坯！");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		
	
		//发电文
		//板坯送热轧
		//doFlag = f_ymsm30_rz_snd(&bcls_rec_f, bcls_ret);
		if (doFlag != 0)
		{
			EDLog(1, 1, "f_ymsm30_rz_snd发送失败.");
			sprintf(s.msg, "头尾宽差超宽板坯，请先倒出板坯！");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		


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

