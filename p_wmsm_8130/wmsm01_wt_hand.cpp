/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         JHZHAO
Version:		1.0
Date:			2016-03-05
Description:	称重后备
**************************************************/

//框架头文件
#include "stdafx.h"
#include <math.h>
//#include "smhs.h"
//程序用头文件
//#include "twma0.h"
//#include "twma1.h"
//#include "twm00.h"



/*<remark>=========================================================
///<summary>
///库图板坯称重后备
///<para>
===========================================================</remark>*/

BM2F_ENTERACE(wmsm01_wt_hand);

int f_wmsm01_wt_hand(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	int count = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	//double d_weight = 0;
	CDecimal tol_count = 0;
	CDecimal tol_volunm = 0;
	CDecimal tol_theory_wt = 0;
	CDecimal mat_volunm = 0;
	CDecimal mat_theory_wt = 0;
	CDecimal v_weight = 0;
	CDecimal t_weight = 0;
	CDecimal d_weight = 0;
	CString mat_no = "";
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



	try
	{
		Log::Trace("", __FUNCTION__, "bcls_rec->Tables[0] = [{0}]", bcls_rec->Tables.get_Count());
		//if (bcls_rec->Tables[0].Columns.Contains("MAT_NO"))
		//
		//	mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim();

		//if (bcls_rec->Tables[0].Columns.Contains("WEIGHT"))
		//	d_weight = bcls_rec->Tables[1].Rows[0]["WEIGHT"].ToDouble() * 1000;  //单位 kg
		//Log::Trace("", __FUNCTION__, "d_weight = [{0}]", d_weight);
		//Log::Trace("", __FUNCTION__, "mat_no = [{0}]", mat_no);

		if (bcls_rec->Tables[0].Columns.Contains("WEIGHT"))
			//Log::Trace("", __FUNCTION__, "bcls_rec->Tables[0] = [{0}]", bcls_rec->Tables[0].Rows[0]["WEIGHT"].ToDecimal());
			//d_weight = bcls_rec->Tables[0].Rows[0]["WEIGHT"].ToDouble() * 1000;  //单位 kg
			d_weight = bcls_rec->Tables[0].Rows[0]["WEIGHT"].ToDecimal() * 1000;  //单位 kg
		if (bcls_rec->Tables[0].Columns.Contains("MAT_NO"))
			mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim();
		 //获取前台传入参数

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

	

		Log::Trace("", __FUNCTION__, "d_weight = [{0}]", d_weight);
		Log::Trace("", __FUNCTION__, "mat_no = [{0}]", mat_no);
		Log::Trace("", __FUNCTION__, "twma1_q.MAT_NO=[{0}]", twma1_q["MAT_NO"].ToString());

		/*twma1["MAT_NO"] = twma1_q["MAT_NO"];
		if (!twma1.Query("MAT_NO"))
		{
			sprintf(s.msg, "材料号不存在！");
			throw CApplicationException(-1, s.msg, log.Location);
		}*/

		sqlstr = "select sum(mat_act_len*mat_act_width*mat_act_thick), count(mat_no), sum(mat_theory_wt*1000) "
			"from tmmsm01 where stock_place_no = '701' "  
			//" and mat_position = '2' "   //梅钢原来逻辑有 
			;

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			tol_volunm = cmd_inq.GetDouble(1);
			tol_count = cmd_inq.GetInt32(2);
			tol_theory_wt = cmd_inq.GetDouble(3);
		}
		cmd_inq.Close();

		Log::Trace("", __FUNCTION__, "总体积 tol_volunm =[{0}]", tol_volunm);
		Log::Trace("", __FUNCTION__, "总个数 tol_count =[{0}]", tol_count);
		Log::Trace("", __FUNCTION__, "总理重 tol_theory_wt =[{0}]", tol_theory_wt);
		
		//if (abs(tol_theory_wt - d_weight) / d_weight * 100 > 2.0)
		if ((tol_theory_wt - d_weight).Abs() / d_weight * 100 > 2.0)
		{
			sprintf(s.msg, "板坯理论重量与实际重量大于2% ！");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		sqlstr = "select mat_act_len*mat_act_width*mat_act_thick, mat_no, mat_theory_wt*1000 "
			"from tmmsm01 where stock_place_no = '701' "
			//" and mat_position = '2' "   //梅钢原来逻辑有 
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			count++;
			mat_volunm = cmd_inq.GetDouble(1);
			mat_no = cmd_inq.GetString(2);
			mat_theory_wt = cmd_inq.GetDouble(3);

			Log::Trace("", __FUNCTION__, " mat_volunm =[{0}]", mat_volunm);
			Log::Trace("", __FUNCTION__, " mat_no =[{0}]", mat_no);
			Log::Trace("", __FUNCTION__, " mat_theory_wt =[{0}]", mat_theory_wt);
			//Ceiling() 向上取整函数
			v_weight = (mat_volunm / tol_volunm * d_weight).Ceiling();
			Log::Trace("", __FUNCTION__, " v_weight =[{0}]", v_weight);

			if (count == tol_count) // 最后一次分摊重量 总重量 - 已分摊的重量
			{
				v_weight = d_weight - v_weight;
				Log::Trace("", __FUNCTION__, "最后一次分摊重量 v_weight =[{0}]", v_weight);
			}
			else
			{
				t_weight = t_weight + v_weight;
				Log::Trace("", __FUNCTION__, "已分摊 t_weight =[{0}]", t_weight);
			}

			twma1.Reset();
			twma1["MAT_NO"] = mat_no;
			twma1["MAT_ACT_WT"] = v_weight/1000;   //后台按kg分摊重量，数据库重量是吨
			twma1["MEASURE_WT_FLAG"] = "1";  // 0 未称重  1 已称重
			int k = twma1.Update("MAT_ACT_WT,MEASURE_WT_FLAG", "MAT_NO");
			Log::Trace("", __FUNCTION__, "fro [{0}] twma1.Update =[{1}]", count, k);
		}
		cmd_inq.Close();

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

