/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2024
Author:      lz
Version:     1.0
Date:        2024-01-9 13:10:05
Description: 熔炼号转变
**************************************************/

#include "stdafx.h"
//函数申明
BM2_FUNCTION_IMPORT
int f_mmsm_gyins2(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//原料函数
int f_210010_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//成分电文

BM2F_ENTERACE(wmsm_heatno_turn)


int f_wmsm_heatno_turn(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CDbCommand cmd_inq(conn);
	//实体类定义
	CModel tmmsm01("TMMSM01");
	CModel hmmsm01("HMMSM01");
	CModel twmsmturn("TWMSMTURN");
	try
	{
		CString heat_no_old = bcls_rec->Tables[0].Rows[0]["HEAT_NO_OLD"].ToString();
		CString heat_no_new = bcls_rec->Tables[0].Rows[0]["HEAT_NO_NEW"].ToString();

		tmmsm01["HEAT_NO"] = heat_no_old;
		tmmsm01["RCV_MAT_FLAG"] = "N";
		hmmsm01["HEAT_NO"] = heat_no_old;
		if (tmmsm01.QueryCount("HEAT_NO") <= 0) {
			sprintf(s.msg, "没有当前炉号的材料，请重新确认！");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (tmmsm01.QueryCount("HEAT_NO") != tmmsm01.QueryCount("HEAT_NO,RCV_MAT_FLAG"))
		{
			sprintf(s.msg, "当前炉号须全部取消收货，请重新确认！");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (hmmsm01.QueryCount("HEAT_NO") > 0) {
			sprintf(s.msg, "当前炉号的材料有在历史档，请重新确认！");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		tmmsm01["HEAT_NO"] = heat_no_new;
		if (tmmsm01.QueryCount("HEAT_NO") > 0) {
			sprintf(s.msg, "目标炉号已有材料，请重新确认！");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		sqlstr = " UPDATE TMMSM19 SET HEAT_NO = '" + heat_no_new + "' WHERE HEAT_NO = '" + heat_no_old + "' ";//中频炉
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " UPDATE TMMSM20 SET HEAT_NO = '" + heat_no_new + "' WHERE HEAT_NO = '" + heat_no_old + "' ";//电炉
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " UPDATE TMMSM23 SET HEAT_NO = '" + heat_no_new + "',L2_PROC_NO='"+ heat_no_new +"'  WHERE HEAT_NO = '" + heat_no_old + "' ";//RH
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " UPDATE TMMSM24 SET HEAT_NO = '" + heat_no_new + "',L2_PROC_NO='" + heat_no_new + "'  WHERE HEAT_NO = '" + heat_no_old + "' ";//LF
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " UPDATE TMMSM25 SET HEAT_NO = '" + heat_no_new + "',L2_PROC_NO='" + heat_no_new + "'  WHERE HEAT_NO = '" + heat_no_old + "' ";//VOD
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " UPDATE TMMSM26 SET HEAT_NO = '" + heat_no_new + "',L2_PROC_NO='" + heat_no_new + "'  WHERE HEAT_NO = '" + heat_no_old + "' ";//LTS
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " UPDATE TMMSM31 SET HEAT_NO = '" + heat_no_new + "',L2_PROC_NO='" + heat_no_new + "'  WHERE HEAT_NO = '" + heat_no_old + "' ";//连铸
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " UPDATE TMMSM2A SET HEAT_NO = '" + heat_no_new + "',L2_PROC_NO='" + heat_no_new + "'  WHERE HEAT_NO = '" + heat_no_old + "' ";//加料
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " UPDATE TMMSM2A_YL SET HEAT_NO = '" + heat_no_new + "',L2_PROC_NO='" + heat_no_new + "'  WHERE HEAT_NO = '" + heat_no_old + "' ";//原料加料
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " UPDATE TMMSM2B SET HEAT_NO = '" + heat_no_new + "',L2_PROC_NO='" + heat_no_new + "'  WHERE HEAT_NO = '" + heat_no_old + "' ";//测温
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();
		//计划
		sqlstr = " UPDATE TPSSM11 SET HEAT_NO = '" + heat_no_new + "' WHERE HEAT_NO = '" + heat_no_old + "' ";//计划编制
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " UPDATE TPSSM41 SET HEAT_NO = '" + heat_no_new + "' WHERE HEAT_NO = '" + heat_no_old + "' ";//计划编制
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " UPDATE TPSSM12 SET HEAT_NO = '" + heat_no_new + "' WHERE HEAT_NO = '" + heat_no_old + "' ";//计划工序
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " UPDATE TPSSM42 SET HEAT_NO = '" + heat_no_new + "' WHERE HEAT_NO = '" + heat_no_old + "' ";//计划编制
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " UPDATE TPSSM99 SET HEAT_NO = '" + heat_no_new + "' WHERE HEAT_NO = '" + heat_no_old + "' ";//计划编制
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//质量
		sqlstr = " UPDATE TQMTS24 SET HEAT_NO = '" + heat_no_new + "' WHERE HEAT_NO = '" + heat_no_old + "' AND SUBSTR2(ST_SAMPLE_NO, 10, 1) IN ('C', 'F', 'R', 'S', 'V') ";//成分主档
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " UPDATE TQMTS25 SET HEAT_NO = '" + heat_no_new + "' WHERE HEAT_NO = '" + heat_no_old + "' AND SUBSTR2(ST_SAMPLE_NO, 10, 1) IN ('C', 'F', 'R', 'S', 'V') ";//成分明细
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " UPDATE TQMTS29 SET HEAT_NO = '" + heat_no_new + "' WHERE HEAT_NO = '" + heat_no_old + "' ";//判定成分
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//成品
		sqlstr = " UPDATE TMMSM33 SET MAT_NO = '" + heat_no_new + "' || SUBSTR2(MAT_NO, 9), BATCH = '" + heat_no_new + "' || SUBSTR2(BATCH, 9), HEAT_NO = '" + heat_no_new + "',  "
			" SLAB_NO = '" + heat_no_new + "' || SUBSTR2(SLAB_NO, 9),PRINT_NO = '" + heat_no_new + "' || SUBSTR2(PRINT_NO, 9) WHERE HEAT_NO = '" + heat_no_old + "' ";//切割
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " UPDATE TMMSM01 SET MAT_NO = '" + heat_no_new + "' || SUBSTR2(MAT_NO, 9), BATCH = '" + heat_no_new + "' || SUBSTR2(BATCH, 9), HEAT_NO = '" + heat_no_new + "',  "
			" SLAB_NO = '" + heat_no_new + "' || SUBSTR2(SLAB_NO, 9),PRINT_NO = '" + heat_no_new + "' || SUBSTR2(PRINT_NO, 9) WHERE HEAT_NO = '" + heat_no_old + "' ";//材料主档
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr =  " UPDATE TMMSM96 SET MAT_NO = '" + heat_no_new + "' || SUBSTR2(MAT_NO, 9), BATCH = '" + heat_no_new + "' || SUBSTR2(BATCH, 9), HEAT_NO = '" + heat_no_new + "', "
			" SLAB_NO = '" + heat_no_new + "' || SUBSTR2(SLAB_NO, 9),PRINT_NO = '" + heat_no_new + "' || SUBSTR2(PRINT_NO, 9) WHERE HEAT_NO = '" + heat_no_old + "' ";//材料履历
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//原料
		EIClass gyins;
		gyins.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
		gyins.Tables[0].Rows.Add();
		gyins.Tables[0].Rows[0]["HEAT_NO"] = heat_no_new;
		doFlag = f_mmsm_gyins2(&gyins, bcls_ret, conn);
		if (doFlag < 0)
		{
			Log::Trace("", "", "f_mmsm_gyins2() msg = [{0}]", s.msg);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		
		doFlag = f_210010_snd(&gyins, bcls_ret, conn);
		if (doFlag != 0) {
			Log::Trace("", "", "f_210010_snd() msg = [{0}]", s.msg);
			
			throw CApplicationException(-1, s.msg, log.Location);
		}

		twmsmturn["HEAT_NO"] = heat_no_new;
		twmsmturn["HEAT_NO_OLD"] = heat_no_old;
		twmsmturn["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
		twmsmturn["REC_CREATOR"] = s.userid;
		twmsmturn.Insert();
	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}


