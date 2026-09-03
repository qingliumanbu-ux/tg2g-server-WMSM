/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     lizhen
Version:    1.0
Date:       2025-7-16
Description: 成品大炉号按钢种新增消耗
**************************************************/
//框架头文件
#include "stdafx.h"


int f_mm0011(CString SeqName, CDecimal SeqLen, CString& SeqNo, CDbConnection* conn);	//获取流水号
int f_mmsm_gyins2(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//原料函数


// service入口
BM2F_ENTERACE(wmsm33c_f8)

int f_wmsm33c_f8(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";


	CString v_station_id = "";
	CString v_resume_seq_no = "";//序号
	CDecimal v_mat_wt = 0;//物料重量

	
	CModel tmmsm33c("TMMSM33C");
	CModel tmmsm33c_dgdh("TMMSM33C_DGDH");
	CModel tmmsm33c_ggl("TMMSM33C_GGL");
	CModel tmmsm2a_yl("TMMSM2A_YL");
	CModel tmmsm50("TMMSM50");

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_sql(conn);
	CString	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{
		tmmsm33c.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsm2a_yl["HEAT_NO"] = tmmsm33c["HEAT_NO"];
		if (tmmsm2a_yl.QueryCount("HEAT_NO")>0)
		{
			sprintf(s.msg, "该炉号[%s]已有消耗，不可重复新增!", (const char*)tmmsm2a_yl["HEAT_NO"].ToString());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}


		tmmsm33c["HEAT_USE_FLAG"] = "1";
		tmmsm33c["STEEL_AMOUNT_FLAG"] = "1";
		tmmsm33c["REC_REVISOR"] = s.userid;
		tmmsm33c["REC_REVISE_TIME"] = datetime;
		tmmsm33c.Update("HEAT_USE_FLAG,STEEL_AMOUNT_FLAG,REC_REVISOR,REC_REVISE_TIME", "HEAT_NO");
		sqlstr = " SELECT SUM(MAT_WT) FROM VMMSM01 WHERE HEAT_NO='" + tmmsm33c["HEAT_NO"].ToString() + "' ";
		Log::Trace("", __FUNCTION__, "HEAT_NO=[{0}]", tmmsm33c["HEAT_NO"].ToString());
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			v_mat_wt = cmd_inq.GetDecimal(1);
		}
		cmd_inq.Close();

		tmmsm33c_dgdh["ST_NO"] = tmmsm33c["ST_NO"];
		tmmsm33c_ggl["ST_NO"] = tmmsm33c["ST_NO"];
		if (tmmsm33c_dgdh.QueryCount("ST_NO") == 0)
		{
			sprintf(s.msg, "该钢种[%s]没有吨钢单耗的基础数据，请维护!", (const char*)tmmsm33c["ST_NO"].ToString());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (tmmsm33c_ggl.QueryCount("ST_NO") == 0)
		{
			sprintf(s.msg, "该钢种[%s]没有过钢量的基础数据，请维护!", (const char*)tmmsm33c["ST_NO"].ToString());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		CString v_unit_code = Db::QueryCString("select UNIT_CODE from TMMSM33C_GGL where ST_NO='" + tmmsm33c["ST_NO"].ToString() + "' and rownum=1");
		int PROC_COUNT = 0;

		sqlstr = " select * from TMMSM33C_DGDH where ST_NO='" + tmmsm33c["ST_NO"].ToString() + "' ";
		Log::Trace("", __FUNCTION__, "sqlstr=[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			PROC_COUNT++;
			cmd_inq.Fetch(tmmsm33c_dgdh);

			tmmsm50["MAT_CODE"] = tmmsm33c_dgdh["MAT_CODE"];
			tmmsm50.Query("MAT_CODE");

			tmmsm2a_yl.Reset();
			tmmsm2a_yl["REC_CREATE_TIME"] = datetime;
			tmmsm2a_yl["REC_CREATOR"] = s.userid;
			tmmsm2a_yl["PRACT_COLL_MODE"] = "0";
			tmmsm2a_yl["HEAT_NO"] = tmmsm33c["HEAT_NO"].ToString();
			tmmsm2a_yl["PROC_NO"] = tmmsm33c["HEAT_NO"].ToString();
			tmmsm2a_yl["STATION_ID"] = v_unit_code.SubstringNE(0, 1);
			tmmsm2a_yl["STATION_NO"] = v_unit_code.SubstringNE(1, 1);
			tmmsm2a_yl["PROC_COUNT"] = PROC_COUNT;
			tmmsm2a_yl["PONO"] = tmmsm33c["HEAT_NO"].ToString();
			tmmsm2a_yl["PROD_DATE"] = datetime.SubstringNE(0,8);
			tmmsm2a_yl["SAME_PROC_NUM"] = 1;
			tmmsm2a_yl["DEVO_TIME"] = datetime;
			tmmsm2a_yl["MAT_CODE"] = tmmsm33c_dgdh["MAT_CODE"];
			tmmsm2a_yl["MAT_NAME"] = tmmsm33c_dgdh["MAT_CODE_NAME"];
			tmmsm2a_yl["DEVO_WT"] = v_mat_wt* tmmsm33c_dgdh["WT_UNIT"].ToDecimal();
			tmmsm2a_yl["HANDWORK_MARK"] = "0";
			tmmsm2a_yl["SM_PLAN_NO"] = tmmsm33c["HEAT_NO"].ToString();
			tmmsm2a_yl["DEV_CODE"] = v_unit_code;
			tmmsm2a_yl["MAT_AMOUNT1"] = v_mat_wt * tmmsm33c_dgdh["WT_UNIT"].ToDecimal();
			tmmsm2a_yl["MATERIAL_CODE"] = tmmsm50["MATERIAL_CODE"];
		
			tmmsm2a_yl["L2_PROC_NO"] = tmmsm33c["HEAT_NO"].ToString();
			tmmsm2a_yl["STK_NO"] = "M" + v_unit_code.SubstringNE(0, 1);
			tmmsm2a_yl["SM_PLAN_NOL2"] = tmmsm33c["HEAT_NO"].ToString();
			doFlag = f_mm0011("TMMSM2A_SEQ", 8, v_resume_seq_no, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
			tmmsm2a_yl["PROD_SEQ_NO"] = datetime+v_resume_seq_no;
		
			tmmsm2a_yl["ID_2A"] = "C"+ datetime.SubstringNE(2,8);
			CString SeqNo = Db::QueryCString("SELECT LPAD(TO_CHAR(MMLC_2AYL.NEXTVAL),5 ,'0') FROM DUAL");

			tmmsm2a_yl["SEQ_NO_2A"] = datetime.SubstringNE(0,8)+ SeqNo;

			if (tmmsm50["QUALITY_FLAS"].ToString() == "1" )
			{
				sqlstr = " select QUALITY_BATCH_NO,LOT_NO,WEIGH_NO from tmmsm81 "
					" where 1=1"
					" and mat_rcv_time != ' '  and quality_batch_no != ' '"
					" and LOT_NO!=' '"
					" and mat_code = @mat_code"
					" order by MAT_RCV_TIME desc "
					;
				cmd_sql.SetCommandText(sqlstr);
				cmd_sql.Parameters.Set("mat_code", tmmsm33c_dgdh["MAT_CODE"].ToString());
				cmd_sql.ExecuteReader();
				if (cmd_sql.Read())
				{
					tmmsm2a_yl["QUALITY_BATCH_NO"] = cmd_sql.GetString(1);
					tmmsm2a_yl["LOT_NO"] = cmd_sql.GetString(2);
					tmmsm2a_yl["WEIGH_NO"] = cmd_sql.GetString(3);

					Log::Trace("", __FUNCTION__, "1-tmmsm81:HEAT_NO=[{0}],mat_code = [{1}],QUALITY_BATCH_NO = [{2}],LOT_NO = [{3}],devo_wt=[{4}]", tmmsm2a_yl["HEAT_NO"].ToString(), tmmsm2a_yl["MAT_CODE"].ToString(), tmmsm2a_yl["QUALITY_BATCH_NO"].ToString(), tmmsm2a_yl["LOT_NO"].ToDecimal());

				}
				cmd_sql.Close();
			}
			else 
			{
				sqlstr = " select QUALITY_BATCH_NO,LOT_NO,WEIGH_NO from tmmsm81 "
					" where 1=1"
					" and mat_rcv_time != ' '  and quality_batch_no != ' '"
					" and mat_code = @mat_code"
					" order by MAT_RCV_TIME desc "
					;
				cmd_sql.SetCommandText(sqlstr);
				cmd_sql.Parameters.Set("mat_code", tmmsm33c_dgdh["MAT_CODE"].ToString());
				cmd_sql.ExecuteReader();
				if (cmd_sql.Read())
				{
					tmmsm2a_yl["QUALITY_BATCH_NO"] = cmd_sql.GetString(1);
					tmmsm2a_yl["LOT_NO"] = cmd_sql.GetString(2);
					tmmsm2a_yl["WEIGH_NO"] = cmd_sql.GetString(3);
					Log::Trace("", __FUNCTION__, "2-tmmsm81:HEAT_NO=[{0}],mat_code = [{1}],QUALITY_BATCH_NO = [{2}],LOT_NO = [{3}],devo_wt=[{4}]", tmmsm2a_yl["HEAT_NO"].ToString(), tmmsm2a_yl["MAT_CODE"].ToString(), tmmsm2a_yl["QUALITY_BATCH_NO"].ToString(), tmmsm2a_yl["LOT_NO"].ToDecimal());

				}
				cmd_sql.Close();
			}

			tmmsm2a_yl["BATCH_NUMBER"] = tmmsm33c_dgdh["MAT_CODE"];
			tmmsm2a_yl.Insert();
		}
		cmd_inq.Close();
		CString v_prod_shift_no("");
		CString v_prod_shift_group("");
		f_epep_get_shift_group("SMDD", datetime, v_prod_shift_no, v_prod_shift_group, conn);

		CModel tmmsm31("TMMSM31");
		tmmsm31["HEAT_NO"] = tmmsm33c["HEAT_NO"].ToString();
		if (tmmsm31.QueryCount("HEAT_NO") == 0)
		{
			sqlstr = "INSERT INTO TMMSM31 (REC_CREATE_TIME, REC_CREATOR, HEAT_NO, PROC_NO, PONO,SM_PLAN_NOL2, DEV_CODE,L2_PROC_NO,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP,ST_NO,STATION_ID,STATION_NO) "
				" VALUES ('" + datetime + "','" + s.userid + "', '" + tmmsm33c["HEAT_NO"].ToString() + "', '" + tmmsm33c["HEAT_NO"].ToString() + "', '" + tmmsm33c["HEAT_NO"].ToString() + "','" + tmmsm33c["HEAT_NO"].ToString() + "',"
				"'" + tmmsm33c["UNIT_CODE"].ToString() + "',"
				"'" + tmmsm33c["HEAT_NO"].ToString() + "','" + datetime.Substring(0, 8) + "','" + v_prod_shift_no + "','" + v_prod_shift_group + "','" + tmmsm33c["ST_NO"].ToString() + "', "
				"'" + tmmsm33c["UNIT_CODE"].ToString().SubstringNE(0, 1) + "','" + tmmsm33c["UNIT_CODE"].ToString().SubstringNE(1, 1).TrimOrBlank() + "')";
			Log::Trace("", __FUNCTION__, "sqlstr=[{0}]", sqlstr);


			cmd_sql.SetCommandText(sqlstr);
			cmd_sql.ExecuteNonQuery();

			cmd_sql.Close();
		}

		

		sqlstr = " select * from TMMSM33C_GGL where ST_NO='" + tmmsm33c["ST_NO"].ToString() + "' ";
		Log::Trace("", __FUNCTION__, "sqlstr=[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read()) 
		{
			cmd_inq.Fetch(tmmsm33c_ggl);
			CString v_table_name("");
			CString v_cols_names("");
			if (tmmsm33c_ggl["UNIT_CODE"].ToString().SubstringNE(0, 1) == "B")
			{
				v_table_name = "TMMSM21";
				v_cols_names = "OXYGEN_FINAL,TOTAL_N2_CONS,AR_SUM_COMSUME";
			}
			if (tmmsm33c_ggl["UNIT_CODE"].ToString().SubstringNE(0, 1) == "A")
			{
				v_table_name = "TMMSM27";
				v_cols_names = "OXYGEN_FINAL,NITROGEN_TOT,TOTAL_AR_CONS";
			}	
			if (tmmsm33c_ggl["UNIT_CODE"].ToString().SubstringNE(0, 1) == "R")
			{
				v_table_name = "TMMSM23";
				v_cols_names = "O2_SUM_COMSUME,N_SUM_COMSUME,AR_SUM_COMSUME";
			}
			if (tmmsm33c_ggl["UNIT_CODE"].ToString().SubstringNE(0, 1) == "F")
			{
				v_table_name = "TMMSM24";
				v_cols_names = "POWER_CONSUME,AR_SUM_COMSUME,REMARK";
			}
			if (tmmsm33c_ggl["UNIT_CODE"].ToString().SubstringNE(0, 1) == "V")
			{
				v_table_name = "TMMSM25";
				v_cols_names = "OXYGEN_FINAL,AR_SUM_COMSUME,REMARK";
			}
			CModel tmmsmxx(v_table_name);
			tmmsmxx["HEAT_NO"] = tmmsm33c["HEAT_NO"].ToString();
			if (v_table_name.Trim()!=""&&tmmsmxx.QueryCount("HEAT_NO")==0)
			{
				sqlstr = "INSERT INTO " + v_table_name + " (REC_CREATE_TIME, REC_CREATOR, HEAT_NO, PROC_NO, PONO,SM_PLAN_NOL2, START_TIME, END_TIME,DEV_CODE,L2_PROC_NO,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP,ST_NO,STATION_ID,"+v_cols_names+") "
					" VALUES ('"+ datetime +"','"+ s.userid +"', '"+ tmmsm33c["HEAT_NO"].ToString() +"', '" + tmmsm33c["HEAT_NO"].ToString() + "', '" + tmmsm33c["HEAT_NO"].ToString() + "','" + tmmsm33c["HEAT_NO"].ToString() + "',"
					"'"+ CDateTime::Now().AddMinutes(-tmmsm33c_ggl["ELAPSED_TIME"].ToDouble()).ToString("yyyyMMddHHmmss") +"', '"+ datetime +"','"+ tmmsm33c_ggl["UNIT_CODE"].ToString() +"',"
					"'"+ tmmsm33c["HEAT_NO"].ToString() +"','" + datetime.Substring(0,8)+ "','" + v_prod_shift_group + "','" + v_prod_shift_group + "','" + tmmsm33c["ST_NO"].ToString() + "','" + tmmsm33c_ggl["UNIT_CODE"].ToString().SubstringNE(0, 1) + "'"
					" ,"+ tmmsm33c_ggl["REMARK_1"].ToString() +"," + tmmsm33c_ggl["REMARK_2"].ToString() + ","+ tmmsm33c_ggl["REMARK_3"].ToString() +") ";
				Log::Trace("", __FUNCTION__, "sqlstr=[{0}]", sqlstr);
			

				cmd_sql.SetCommandText(sqlstr);
				cmd_sql.ExecuteNonQuery();
				
				cmd_sql.Close();
			}
			
			EIClass mmsmgy06;
			mmsmgy06.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
			mmsmgy06.Tables[0].Columns.Add(DT_STRING, "SM_PLAN_NOL2");
			mmsmgy06.Tables[0].Rows.Add();
			mmsmgy06.Tables[0].Rows[0]["HEAT_NO"] = tmmsm33c["HEAT_NO"];
			mmsmgy06.Tables[0].Rows[0]["SM_PLAN_NOL2"] = tmmsm33c["HEAT_NO"];
			doFlag = f_mmsm_gyins2(&mmsmgy06, bcls_ret, conn);

		}
		cmd_inq.Close();
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
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
