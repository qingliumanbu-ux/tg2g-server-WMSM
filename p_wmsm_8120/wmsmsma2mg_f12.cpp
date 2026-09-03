/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:
Version:		1.0
Date:			2016-03-05
Description:	来料拒收
**************************************************/

//框架头文件
#include "stdafx.h"
#include "math.h"
#include  "stdlib.h"



BM2_FUNCTION_IMPORT
int f_mmsm99(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wmsm_slab_weight(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
//int f_ym1900c9_snd(CString* in_mat_no, CString* in_qf_flag);
int f_wmsm_slab_check(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
//int f_mm1900cb_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
//int f_mm1900da_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_mmsm3401_proc(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//修改板坯主档信息
/*<remark>=========================================================
///<summary>
///板坯入库功能
///<para>
///2.排序方式：
///</para>
///<para>数据库表：TWMA0 倒躲队列；TWMA1 物料主档表
///<returns>执行预材料预入库功能</returns> 
===========================================================</remark>*/

BM2F_ENTERACE(wmsmsma2mg_f12);

int f_wmsmsma2mg_f12(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	
	/* 程序内部变量 */
	
	int blkNum = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CDecimal rowCount = 0;

	/*程序用变量*/
	/*程序用变量*/
	int doFlag = 0;
	int blockCount = 0;
	EIClass blck_mat_wt;
	EIClass blck_mat_new;
	EIClass blck_mat_upd;
	EIClass blck_qt_ck;
	int isLock = 0;
	int ret = 0;
	int isChgFlag = 0;

	int v_mat_count = 0;
	int v_hsf_len = 0;
	int v_waste_len = 0;
	CString v_prod_shift_no = "";
	CString v_prod_shift_group = "";
	int v_hsf_weight = 0;
	int v_slab_fix_s_min = 0;
    int v_slab_len_max = 0;
	int v_slab_min_width = 0;
	int v_slab_max_width = 0;
	CString v_strand_no = "";
	CString v_hold_flag = "";
	CString v_hold_remark = " ";
	CString PROD_SHIFT_NO = " ";
	CString PROD_SHIFT_GROUP = " ";
	int v_isfinish = 0;
	CDecimal v_slab_density = 0.0;		//板坯密度
	CDecimal v_slab_wtadj_factor = 0.0;	//修正系数
	int snd_flag = 0;

	CString v_defect_code_f_1 = "";
	CString v_defect_code_f_2 = "";
	CString v_defect_code_f_3 = "";
	CString v_defect_code_f_4 = "";
	int v_defect_position_f_1 = 0;
	int v_defect_position_f_2 = 0;
	int v_defect_position_f_3 = 0;
	int v_defect_position_f_4 = 0;
	CString v_defect_degree_f_1 = "";
	CString v_defect_degree_f_2 = "";
	CString v_defect_degree_f_3 = "";
	CString v_defect_degree_f_4 = "";

	CString v_hold_cause = "";
	int v_width = 0;	//宽度
	//CModel tmmsm34 = CModel("TMMSM34");
	CModel tmmsm01("TMMSM01");
	CModel tmmsm34("TMMSM34");
	CModel tmmsm06("TMMSM06");
	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
	
		EIClass bcls_rec_QM17;//材料质量封锁
		bcls_rec_QM17.Tables[0].set_TableName("MM0099");
		bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "EVENT_ID");
		bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");
		bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "SYSTEM_ID");
		bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "FUNC_ID");
		bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
		bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "REL_REMARK");
		bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "REL_MAKER");
		bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "REL_TIME");
		bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "HOLD_CAUSE_CODE");
		bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "DEFECT_CLASS");

		tmmsm01["MAT_NO"] = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString();
		tmmsm34["MAT_ACT_WIDTH"] = bcls_rec->Tables[0].Rows[0]["WIDTH_SLAB_ACT"];
		tmmsm34["MAT_ACT_LEN"] = bcls_rec->Tables[0].Rows[0]["LENGTH_SLAB_ACT"];
		tmmsm34["WTDTH_HEAD_SLAB2"] = bcls_rec->Tables[0].Rows[0]["WTDTH_HEAD_SLAB2"];
		tmmsm34["WTDTH_BOT_SLAB2"] = bcls_rec->Tables[0].Rows[0]["WTDTH_BOT_SLAB2"];
		tmmsm34["FINISH_FLAG"] = bcls_rec->Tables[0].Rows[0]["FINISH_FLAG"].ToString();
		v_hsf_len = bcls_rec->Tables[0].Rows[0]["HSF_LEN"];
		v_waste_len = bcls_rec->Tables[0].Rows[0]["WASTE_LEN"];
		v_hold_remark = bcls_rec->Tables[0].Rows[0]["HF_REMARK"].ToString();
		v_hold_flag = bcls_rec->Tables[0].Rows[0]["SLAB_JUD"].ToString();
		tmmsm34["HF_REMARK"] = bcls_rec->Tables[0].Rows[0]["REL_REMARK"].ToString();
		v_defect_code_f_1 = bcls_rec->Tables[0].Rows[0]["DEFECT_CODE_F_1"].ToString();
		v_defect_code_f_2 = bcls_rec->Tables[0].Rows[0]["DEFECT_CODE_F_2"].ToString();
		v_defect_code_f_3 = bcls_rec->Tables[0].Rows[0]["DEFECT_CODE_F_3"].ToString();
		v_defect_code_f_4 = bcls_rec->Tables[0].Rows[0]["DEFECT_CODE_F_4"].ToString();
		v_defect_position_f_1 = bcls_rec->Tables[0].Rows[0]["DEFECT_POSITION_F_1"];
		v_defect_position_f_2 = bcls_rec->Tables[0].Rows[0]["DEFECT_POSITION_F_2"];
		v_defect_position_f_3 = bcls_rec->Tables[0].Rows[0]["DEFECT_POSITION_F_3"];
		v_defect_position_f_4 = bcls_rec->Tables[0].Rows[0]["DEFECT_POSITION_F_4"];
		v_defect_degree_f_1 = bcls_rec->Tables[0].Rows[0]["DEFECT_DEGREE_F_1"].ToString();
		v_defect_degree_f_2 = bcls_rec->Tables[0].Rows[0]["DEFECT_DEGREE_F_2"].ToString();
		v_defect_degree_f_3 = bcls_rec->Tables[0].Rows[0]["DEFECT_DEGREE_F_3"].ToString();
		v_defect_degree_f_4 = bcls_rec->Tables[0].Rows[0]["DEFECT_DEGREE_F_4"].ToString();
		tmmsm34["SLAB_HDSCARF_MODE"] = bcls_rec->Tables[0].Rows[0]["FINISH_TYPE"].ToString();
		v_hold_cause = bcls_rec->Tables[0].Rows[0]["HOLD_CAUSE"].ToString();
		tmmsm34["SURFACE_DECIDE_CODE"] = bcls_rec->Tables[0].Rows[0]["SURFACE_DECIDE_CODE"].ToString();
		tmmsm34["STOCK_PLACE_NO"] = bcls_rec->Tables[0].Rows[0]["STOCK_PLACE_NO"].ToString();
		tmmsm34["FINISH_MODE"] = bcls_rec->Tables[0].Rows[0]["FINISH_MODE"].ToString();
		tmmsm34["HDSCARF_MODE"] = bcls_rec->Tables[0].Rows[0]["HDSCARF_MODE"].ToString();

		Log::Trace("", "", "v_hold_flag=[{0}]", v_hold_flag);
		Log::Trace("", "", "defect_code_f_1=[{0}]", v_defect_code_f_1);
		EDLog(1, 1, "defect_position_f_1 = [%d]", v_defect_position_f_1);
		EDLog(1, 1, "hsf_len = [%d], waste_len = [%d]", v_hsf_len, v_waste_len);

		tmmsm01.Query("MAT_NO");

		v_strand_no = tmmsm01["STRAND_NO"];

		Log::Trace("", __FUNCTION__, "精整板坯流号：\t[{0}]", v_strand_no);

		sqlstr =
			" SELECT SLAB_FIX_S_MIN, SLAB_LEN_MAX, SLAB_MIN_WIDTH, SLAB_MAX_WIDTH FROM TQMTS9CC"
			" WHERE STRAND_NO = @v_strand_no"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("v_strand_no", v_strand_no);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			v_slab_fix_s_min = cmd_inq.GetInt32(1);
			v_slab_len_max = cmd_inq.GetInt32(2);
			v_slab_min_width = cmd_inq.GetInt32(3);
			v_slab_max_width = cmd_inq.GetInt32(4);
		}		
		cmd_inq.Close();

		Log::Trace("", __FUNCTION__, "v_slab_fix_s_min[{0}]", v_slab_fix_s_min);
		Log::Trace("", __FUNCTION__, "v_slab_len_max[{0}]", v_slab_len_max);
		Log::Trace("", __FUNCTION__, "v_slab_min_width[{0}]", v_slab_min_width);
		Log::Trace("", __FUNCTION__, "v_slab_max_width[{0}]", v_slab_max_width);

		
		if (v_slab_fix_s_min > tmmsm34["MAT_ACT_LEN"].ToDecimal() || v_slab_len_max < tmmsm34["MAT_ACT_LEN"].ToDecimal())
		{
			sprintf(s.msg, "板坯精整后 长度 超出限定范围！( %d <= 长度 <=%d )", v_slab_fix_s_min, v_slab_len_max);
			throw CApplicationException(-1, s.msg, log.Location);
			
		}
		if (v_slab_min_width > tmmsm34["MAT_ACT_WIDTH"].ToDecimal() || v_slab_max_width < tmmsm34["MAT_ACT_WIDTH"].ToDecimal())
		{
			sprintf(s.msg, "板坯精整后 宽度 超出限定范围！( %d <= 宽度 <= %d )", v_slab_min_width, v_slab_max_width);
			throw CApplicationException(-1, s.msg, log.Location);
			
		}
		EDLog(1, 1, "--1-- 精整废长度 = [%d], 废弃品长度 = [%d]", v_hsf_len, v_waste_len);
		
		tmmsm34["REC_CREATE_TIME"] = datetime;
		tmmsm34["REC_CREATOR"] = s.userid;
		tmmsm34["HSF_END_TIME"] = datetime;

        tmmsm34["PROD_TIME"] = datetime;
		

		if (tmmsm34["PROD_SHIFT_NO"].ToString().Trim() == "" || tmmsm34["PROD_SHIFT_GROUP"].ToString().Trim() == "")
		{
			f_epep_get_shift_group("SM", tmmsm34["PROD_TIME"].ToString(), PROD_SHIFT_NO, PROD_SHIFT_GROUP , conn);
			tmmsm34["PROD_SHIFT_NO"] = PROD_SHIFT_NO;
			tmmsm34["PROD_SHIFT_GROUP"] = PROD_SHIFT_GROUP;
			Log::Trace("", "", "tmmsm34[PROD_TIME] =[{0}]", tmmsm34["PROD_TIME"].ToString());
			Log::Trace("", "", "tmmsm34[PROD_SHIFT_NO] =[{0}]", tmmsm34["PROD_SHIFT_NO"].ToString());
			Log::Trace("", "", "tmmsm34[PROD_SHIFT_GROUP] =[{0}]", tmmsm34["PROD_SHIFT_GROUP"].ToString());
		}
		//精整前
		tmmsm34["MAT_ACT_THICK"] = tmmsm01["MAT_ACT_THICK"];
		tmmsm34["SL_SLBWD"] = tmmsm01["MAT_ACT_WIDTH"];
		tmmsm34["SL_SLBLEN"] = tmmsm01["MAT_ACT_LEN"];
		tmmsm34["SL_SLBTHK"] = tmmsm01["MAT_ACT_THICK"];
		tmmsm34["WTDTH_BOT_SLAB1"] = tmmsm01["SLAB_TAIL_WIDTH"];
		tmmsm34["WTDTH_HEAD_SLAB1"] = tmmsm01["SLAB_HEAD_WIDTH"];
		tmmsm34["MAT_ACT_WT"] = tmmsm01["MAT_ACT_WT"];
		tmmsm34["MAT_THEORY_WT"] = tmmsm01["MAT_THEORY_WT"];
		tmmsm34["MAT_NO"]= tmmsm01["MAT_NO"];
		tmmsm34["HEAT_NO"]= tmmsm01["HEAT_NO"];
		tmmsm34["ST_NO"]= tmmsm01["ST_NO"];
		tmmsm34["PONO"]= tmmsm01["PONO"];
		tmmsm34["FINISH_MEND_CODE"]= "2";//1:板坯精整，2：板坯清理
		tmmsm34["HOLD_CAUSE_CODE"]= v_hold_cause;
		tmmsm34["HOLD_REMARK"]= v_hold_remark;//封锁注释

		//增加表面判定人和判定时间
		tmmsm01["SURFACE_DECIDE_TIME"] = datetime;
		tmmsm01["SURFACE_DECIDE_MAKER"] = s.userid;

		EIClass blck_mat_wt;//精整实绩电文参数
		blck_mat_wt.Tables[0].set_TableName("CSWT");
		blck_mat_wt.Tables[0].Columns.Add(DT_DECIMAL, "MAT_ACT_WIDTH");
		blck_mat_wt.Tables[0].Columns.Add(DT_DECIMAL, "MAT_ACT_THICK");
		blck_mat_wt.Tables[0].Columns.Add(DT_DECIMAL, "MAT_ACT_LEN");
		blck_mat_wt.Tables[0].Columns.Add(DT_STRING, "STRAND_NO");
		blck_mat_wt.Tables[0].Columns.Add(DT_STRING, "ST_NO");
		blck_mat_wt.Tables[0].Rows.Add();
		//计算板坯精整重量
		blck_mat_wt.Tables[0].Rows[0]["MAT_ACT_THICK"] = tmmsm01["MAT_ACT_THICK"];
		blck_mat_wt.Tables[0].Rows[0]["STRAND_NO"] = tmmsm01["STRAND_NO"];
	

		//获取修正系数和板坯密度
		sqlstr =
			" SELECT SLAB_DENSITY, SLAB_WTADJ_FACTOR FROM TQMTS9M WHERE STRAND_NO = @v_strand_no"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("v_strand_no", v_strand_no);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			v_slab_density = cmd_inq.GetDecimal(1);
			v_slab_wtadj_factor = cmd_inq.GetDecimal(2);
		}
		cmd_inq.Close();	

		//计算精整废量
		tmmsm34["HSF_FRITTER"] = (v_hsf_len * 230 * tmmsm01["MAT_ACT_WIDTH"].ToDecimal() * v_slab_density * v_slab_wtadj_factor + 0.5).ToInt32() / 1000000;

		//v_width = ;

		//废弃品量
		tmmsm34["OTHERS_FRITTER"] = ((v_waste_len * 230 * tmmsm01["MAT_ACT_WIDTH"].ToDecimal() + tmmsm34["MAT_ACT_LEN"].ToDecimal() * 230 * (tmmsm01["MAT_ACT_WIDTH"].ToDecimal() - tmmsm34["MAT_ACT_WIDTH"].ToDecimal())) * v_slab_density * v_slab_wtadj_factor + 0.5).ToInt32() / 1000000;
		if (tmmsm34["OTHERS_FRITTER"].ToDecimal() < 0)
		{
			tmmsm34["OTHERS_FRITTER"] =  -tmmsm34["OTHERS_FRITTER"].ToDecimal();
		}
	
		//计算精整后板坯理论重量
		Log::Trace("", __FUNCTION__, "精整前mat_act_width[{0}],mat_act_len[{1}]", tmmsm01["MAT_ACT_WIDTH"].ToDecimal(), tmmsm01["MAT_ACT_LEN"].ToDecimal());
	
		tmmsm01["MAT_ACT_WIDTH"] = tmmsm34["MAT_ACT_WIDTH"];
		tmmsm01["MAT_ACT_LEN"] = tmmsm34["MAT_ACT_LEN"];

		Log::Trace("", __FUNCTION__, "精整后mat_act_width[{0}],mat_act_len[{1}]", tmmsm01["MAT_ACT_WIDTH"].ToDecimal(), tmmsm01["MAT_ACT_LEN"].ToDecimal());
		//EDLog(1, 1, "精整后 mat_act_width = [%d], mat_act_len = [%d]", tmmsm01["MAT_ACT_WIDTH"].ToDecimal(), tmmsm01["MAT_ACT_LEN"].ToDecimal());
		
		blck_mat_wt.Tables[0].Rows[0]["MAT_ACT_WIDTH"] = tmmsm01["MAT_ACT_WIDTH"];
		blck_mat_wt.Tables[0].Rows[0]["MAT_ACT_LEN"] = tmmsm01["MAT_ACT_LEN"];
		blck_mat_wt.Tables[0].Rows[0]["ST_NO"] = tmmsm01["ST_NO"];
	
		doFlag = f_wmsm_slab_weight(&blck_mat_wt, bcls_ret, conn);
		if (0 != doFlag)
		{
			EDLog(1, 1, "--3--计算板坯理论重量失败", s.msg);
			strcpy(s.msg, "计算板坯理论重量失败");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		tmmsm01["MAT_THEORY_WT"] = bcls_ret->Tables["CSWT"].Rows[0]["THEORY_WT"];
		tmmsm01["MAT_ACT_WT"] = bcls_ret->Tables["CSWT"].Rows[0]["THEORY_WT_ACT"];
		
		Log::Trace("", "", "板坯精整后的重量=[{0}]", tmmsm01["MAT_ACT_WT"].ToDecimal());
	
		tmmsm34["MAT_THEORY_WT"] = tmmsm01["MAT_THEORY_WT"];
		tmmsm34["MAT_ACT_WT"] = tmmsm01["MAT_ACT_WT"];

		//修改TMMSM01中板坯数据
		int i = 0;
		for (i = 1; i <= 4; i++)
		{
			tmmsm06.Reset();
			tmmsm06["REC_CREATE_TIME"] = datetime;
			tmmsm06["REC_CREATOR"] = s.userid;
			if (v_defect_code_f_1 != "" && i == 1)
			{
				tmmsm06["DEFECT_ID"] = datetime + "001";
				tmmsm06["MAT_NO"] = tmmsm01["MAT_NO"];
				tmmsm06["DEFECT_CODE"] = v_defect_code_f_1;
				tmmsm06["DEFECT_START"] = v_defect_position_f_1;
				tmmsm06["DEFECT_SURF"] = v_defect_degree_f_1;
				tmmsm06.Delete();
				tmmsm06.Insert();
			}
			else if (v_defect_code_f_2 != "" && i== 2)
			{
				tmmsm06["DEFECT_ID"] = datetime + "002";
				tmmsm06["MAT_NO"] = tmmsm01["MAT_NO"];
				tmmsm06["DEFECT_CODE"] = v_defect_code_f_2;
				tmmsm06["DEFECT_START"] = v_defect_position_f_2;
				tmmsm06["DEFECT_SURF"] = v_defect_degree_f_2;
				tmmsm06.Delete();
				tmmsm06.Insert();
			}
			else if (v_defect_code_f_3 != "" && i == 3)
			{
				tmmsm06["DEFECT_ID"] = datetime + "003";
				tmmsm06["MAT_NO"] = tmmsm01["MAT_NO"];
				tmmsm06["DEFECT_CODE"] = v_defect_code_f_3;
				tmmsm06["DEFECT_START"] = v_defect_position_f_3;
				tmmsm06["DEFECT_SURF"] = v_defect_degree_f_3;
				tmmsm06.Delete();
				tmmsm06.Insert();
			}
			else if (v_defect_code_f_4 != "" && i == 4)
			{
				tmmsm06["DEFECT_ID"] = datetime + "004";
				tmmsm06["MAT_NO"] = tmmsm01["MAT_NO"];
				tmmsm06["DEFECT_CODE"] = v_defect_code_f_4;
				tmmsm06["DEFECT_START"] = v_defect_position_f_4;
				tmmsm06["DEFECT_SURF"] = v_defect_degree_f_4;
				tmmsm06.Delete();
				tmmsm06.Insert();
			}
			
		}
		tmmsm01["HOLD_REMARK"]= v_hold_remark;
		

		tmmsm01["FINISH_FLAG"]= tmmsm34["FINISH_FLAG"];
		tmmsm01["SLAB_TAIL_WIDTH"] = tmmsm34["WTDTH_BOT_SLAB2"];
		tmmsm01["SLAB_HEAD_WIDTH"] = tmmsm34["WTDTH_HEAD_SLAB2"];
		//tmmsm01.SLAB_HEAD_TAIL_WIDTH_DIFF = tmmsm01["SLAB_HEAD_WIDTH"].ToDecimal() - tmmsm01["SLAB_TAIL_WIDTH"];

		//20160119 LWB 应郑雷需求维护01表新增字段值
		tmmsm01["SPARE_ITEM_N2"] = tmmsm01["SPARE_ITEM_N2"].ToDecimal() + tmmsm34["HSF_FRITTER"].ToDecimal(); //精整废
		tmmsm01["SPARE_ITEM_N1"] = tmmsm01["SPARE_ITEM_N1"].ToDecimal() + tmmsm34["OTHERS_FRITTER"].ToDecimal(); //废弃量 
		//if (strcmp(tmmsm34["SLAB_HDSCARF_MODE"].ToString(), tmmsm01.DEFECT_DEGREE_L_2) > 0)//实际清理方法
		//{
		//	//tmmsm01.DEFECT_DEGREE_L_2= tmmsm34["SLAB_HDSCARF_MODE"];
		//}

		//if ((tmmsm01.SLAB_LENGTH_MIN_NOM - 80) > tmmsm01["MAT_ACT_LEN"].ToDecimal() || (tmmsm01.SLAB_LENGTH_MAX_NOM + 80) < tmmsm01["MAT_ACT_LEN"].ToDecimal())
		//{
		//	if (0 == strcmp("1", tmmsm01["FIX_FLAG"].ToString()) || 0 == strcmp("3", tmmsm01["FIX_FLAG"].ToString()))//1:精整前 定尺
		//	{
		//		strcpy(tmmsm01["FIX_FLAG"].ToString(), "2");//2:二次切割造成的 非定尺
		//	}
		//}
		//else
		//{
		//	if (0 == strcmp("0", tmmsm01["FIX_FLAG"].ToString()) || 0 == strcmp("2", tmmsm01["FIX_FLAG"].ToString()))//0:精整前 非定尺
		//	{
		//		strcpy(tmmsm01["FIX_FLAG"].ToString(), "3");//3:二次切割造成的 定尺
		//	}
		//}
		//当选择合格的时候进行校验
		//if ( v_hold_flag == "1")
		//{
		//	EIClass blck_qt_ck;//精整实绩电文参数
		//	blck_qt_ck.Tables[0].set_TableName("SQCK");
		//	blck_qt_ck.Tables[0].Columns.Add(DT_DECIMAL, "L");
		//	blck_qt_ck.Tables[0].Columns.Add(DT_DECIMAL, "W");
		//	blck_qt_ck.Tables[0].Columns.Add(DT_DECIMAL, "Wb");
		//	blck_qt_ck.Tables[0].Columns.Add(DT_DECIMAL, "Wt");
		//	blck_qt_ck.Tables[0].Columns.Add(DT_STRING, "A");
		//	blck_qt_ck.Tables[0].Columns.Add(DT_STRING, "B");
		//	blck_qt_ck.Tables[0].Columns.Add(DT_STRING, "C");
		//	blck_qt_ck.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
		//	blck_qt_ck.Tables[0].Rows.Add();
		//	//板坯质量判定校验流程
		//	blck_qt_ck.Tables[0].Rows[0]["L"] = tmmsm34["MAT_ACT_LEN"];
		//	blck_qt_ck.Tables[0].Rows[0]["W"] = tmmsm34["MAT_ACT_WIDTH"];
		//	blck_qt_ck.Tables[0].Rows[0]["Wb"] = tmmsm34["WTDTH_HEAD_SLAB2"];
		//	blck_qt_ck.Tables[0].Rows[0]["Wt"] = tmmsm34["MAT_ACT_LEN"];
		//	blck_qt_ck.Tables[0].Rows[0]["A"] = tmmsm34["FINISH_FLAG"];
		//	//blck_qt_ck.Tables[0].Rows[0]["B"] = tmmsm01["DEFECT_CODE_L_2"];
		//	//blck_qt_ck.Tables[0].Rows[0]["C"] = tmmsm01["CHG_ST_NO_TYPE"];
		//	blck_qt_ck.Tables[0].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"];

		//	Log::Trace("", "", "111=[{0}]", blck_qt_ck.Tables[0].Rows[0]["L"]);


		//	doFlag = f_wmsm_slab_check(&blck_qt_ck, bcls_ret, conn);
		//	if (0 != doFlag)
		//	{
		//		EDLog(1, 1, "f_wmsm_slab_check板坯质量判定校验失败", s.msg);
		//		strcpy(s.msg, "f_wmsm_slab_check板坯质量判定校验失败");
		//		throw CApplicationException(-1, s.msg, s.svc_name);
		//	}
		//
		//	isLock = bcls_ret->Tables["SQCK"].Rows[0]["isLock"];
		//	if (isLock == 1)
		//	{
		//		tmmsm01["SURFACE_DECIDE_CODE"] = "1";
		//		tmmsm01["HOLD_FLAG"] = "0";
		//		//精整实绩表其他信息维护
		//		tmmsm34["JUDGE_MAKER"] = s.userid;
		//		tmmsm34["JUDGE_TIME"] = datetime;
		//		tmmsm01["FINISH_FLAG"] = "1";//已精整完		
		//	}
		//	else
		//	{
		//		strcpy(s.msg, "请重新确认");
		//		throw CApplicationException(-1, s.msg, s.svc_name);
		//		
		//	}

		//	//对满足花纹板规格要求的改钢板坯将出钢记号修改为GR3180F2、GR4180F2
		//	/*if (0 == strcmp(tmmsm01.prec_slab_no, " ") && 0 == strcmp(tmmsm01.order_no, " "))
		//	{
		//		if (0 == strcmp(tmmsm01.fin_st_no, "GR3160F1") || 0 == strcmp(tmmsm01.fin_st_no, "GR3160F4") || 0 == strcmp(tmmsm01.fin_st_no, "GR4160F4") || 0 == strcmp(tmmsm01.fin_st_no, "GR4160F1") || 0 == strcmp(tmmsm01.fin_st_no, "AP1860C1"))
		//		{
		//			if ((tmmsm01.mat_act_len >= 7000 && tmmsm01.mat_act_len <= 8500) && ((tmmsm01.mat_act_width >= 1010 && tmmsm01.mat_act_width <= 1100) || (tmmsm01.mat_act_width >= 1240 && tmmsm01.mat_act_width <= 1300)))
		//			{
		//				if (0 == strcmp(tmmsm01.fin_st_no, "GR3160F1") || 0 == strcmp(tmmsm01.fin_st_no, "GR3160F4") || 0 == strcmp(tmmsm01.fin_st_no, "AP1860C1"))
		//				{
		//					strcpy(tmmsm01.fin_st_no, "GR3180F2");
		//				}
		//				else if (0 == strcmp(tmmsm01.fin_st_no, "GR4160F1") || 0 == strcmp(tmmsm01.fin_st_no, "GR4160F4"))
		//				{
		//					strcpy(tmmsm01.fin_st_no, "GR4180F2");
		//				}
		//				strcpy(tmmsm01.st_no, tmmsm01.fin_st_no);
		//				strcpy(tmmsm01.mat_destion, "00");
		//				isChgFlag = 1;
		//			}
		//		}
		//		else if (0 == strcmp(tmmsm01.st_no, "GR3180F2") || 0 == strcmp(tmmsm01.st_no, "GR4180F2"))
		//		{
		//			if (tmmsm01.mat_act_len < 7000 || tmmsm01.mat_act_len > 8500 || tmmsm01.mat_act_width < 1010 || tmmsm01.mat_act_width > 1300 || (tmmsm01.mat_act_width < 1240 && tmmsm01.mat_act_width > 1100))
		//			{
		//				if (0 == strcmp(tmmsm01.st_no, "GR3180F2"))
		//				{
		//					strcpy(tmmsm01.fin_st_no, "GR3160F1");
		//				}
		//				else if (0 == strcmp(tmmsm01.st_no, "GR4180F2"))
		//				{
		//					strcpy(tmmsm01.fin_st_no, "GR4160F1");
		//				}
		//				strcpy(tmmsm01.st_no, tmmsm01.fin_st_no);
		//				strcpy(tmmsm01.mat_destion, "01");
		//				if (0 == strcmp(tmmsm01.chg_st_no_type, " "))
		//					strcpy(tmmsm01.chg_st_no_type, "9");
		//				isChgFlag = 1;
		//			}
		//		}
		//	}*/
		//}	


		sqlstr =
			" SELECT COUNT(*)  FROM TWMJ1 WHERE MAT_NO =@mat_no"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("mat_no", tmmsm01["MAT_NO"].ToString());
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			snd_flag = cmd_inq.GetInt32(1);			
		}
		cmd_inq.Close();
	
		//判断是否发送切坯信息至L4
		if (snd_flag > 0)
		{
			/*bcls_rec_f.AddColName(1, "mat_no");
			bcls_rec_f.AddColName(1, "surface_decide_code");
			bcls_rec_f.AddColName(1, "deal_flag");
			bcls_rec_f.SetColVal(1, 1, "mat_no", tmmsm01["MAT_NO"].ToString());
			bcls_rec_f.SetColVal(1, 1, "surface_decide_code", tmmsm34["SURFACE_DECIDE_CODE"].ToString());
			bcls_rec_f.SetColVal(1, 1, "deal_flag", "0");*/
			/*ret = f_mm1900cb_snd(&bcls_rec_f, &bcls_ret_f);
			if (ret != 0)
			{
				bcls_ret_f.GetSYS(&s);
				EDLog(1, 1, "发送切割坯实绩f_mm1900cb_snd()失败 msg = [%s]", s.msg);
				strcpy(s.msg, "发送切割坯实绩f_mm1900cb_snd()失败");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}*/
		}

		//判定通过发送精整电文
		if (isLock == 1)
		{
			//doFlag = f_ym1900c9_snd(tmmsm01["MAT_NO"].ToString(), "1");
			//if (doFlag < 0)
			//{
			//	//bcls_ret.GetSYS(&s);
			//	EDLog(1, 1, "发送产销L4板坯精整电文f_ym1900c9_snd()失败 msg = [%s]", s.msg);
			//	strcpy(s.msg, "发送产销L4板坯精整电文f_ym1900c9_snd()失败");
			//	throw CApplicationException(-1, s.msg, s.svc_name);
			//}
		}
		if (tmmsm34["HSF_FRITTER"].ToDecimal() < 0)
		{
			tmmsm34["HSF_FRITTER"] = 0;
		}
		if (tmmsm34["OTHERS_FRITTER"].ToDecimal() < 0)
		{
			tmmsm34["OTHERS_FRITTER"] = 0;
		}

		tmmsm34["PRACT_COLL_MODE"] = "0";
		tmmsm34["PROD_SEQ_NO"] = tmmsm34["REC_CREATE_TIME"];
		tmmsm34["IN_MAT_NO"] = tmmsm34["MAT_NO"];
		//记录板坯精整信息
		//tmmsm34.Insert();

		if (v_hold_flag == "4")
		{
			bcls_rec_QM17.Tables[0].Rows.Add();
			bcls_rec_QM17.Tables[0].Rows[0]["EVENT_ID"] = "QM17";
			bcls_rec_QM17.Tables[0].Rows[0]["EVENT_LINE_TYPE"] = "00";
			bcls_rec_QM17.Tables[0].Rows[0]["SYSTEM_ID"] = "MMSM";
			bcls_rec_QM17.Tables[0].Rows[0]["FUNC_ID"] = "wmsmsma2mg_f12";
			bcls_rec_QM17.Tables[0].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"];
			bcls_rec_QM17.Tables[0].Rows[0]["REL_REMARK"] = "不合格封锁";
			bcls_rec_QM17.Tables[0].Rows[0]["REL_MAKER"] = s.userid; 
			bcls_rec_QM17.Tables[0].Rows[0]["REL_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			bcls_rec_QM17.Tables[0].Rows[0]["HOLD_CAUSE_CODE"] = v_hold_cause;
			bcls_rec_QM17.Tables[0].Rows[0]["DEFECT_CLASS"] = " ";
			doFlag = f_mmsm99(&bcls_rec_QM17, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}

		if (!bcls_rec->Tables.Contains("MMSM34"))
		{
			bcls_rec->Tables.Add("MMSM34");
		}
		if (!bcls_rec->Tables["MMSM34"].Columns.Contains("PROC_DIV"))
		{
			bcls_rec->Tables["MMSM34"].Columns.Add(DT_STRING, "PROC_DIV");
		}

		tmmsm34.MergeTo(bcls_rec->Tables["MMSM34"], false);
		bcls_rec->Tables["MMSM34"].Rows[0]["PROC_DIV"] = "I";/*I:新增 U:修改 D:删除*/

		bcls_rec->Tables.Add("PARA");
		bcls_rec->Tables["PARA"].Columns.Add(DT_STRING, "PROC_DIV");
		bcls_rec->Tables["PARA"].Columns.Add(DT_STRING, "FACTORY_DIV");	
		bcls_rec->Tables["PARA"].Columns.Add(DT_STRING, "STATION_ID");
		bcls_rec->Tables["PARA"].Rows.Add();
		bcls_rec->Tables["PARA"].Rows[0]["PROC_DIV"] = "I";
		bcls_rec->Tables["PARA"].Rows[0]["FACTORY_DIV"] = "A10";
		bcls_rec->Tables["PARA"].Rows[0]["STATION_ID"] = "C";

		Log::Trace("", __FUNCTION__, "------------发送电文[GEGM34]--------------");
		doFlag = f_mmsm3401_proc(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
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
