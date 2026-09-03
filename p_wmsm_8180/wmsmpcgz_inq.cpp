/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     XXX
Version:    1.0
Date:       2025-02-24
Description: 成品炼钢助手排产钢种查询
**************************************************/
//框架头文件
#include "stdafx.h"


// service入口
BM2F_ENTERACE(wmsmpcgz_inq)

int f_wmsmpcgz_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int		TotalRecordCount = 0;
	//排产钢种查当前时间前6h--后24h
	//CString v_start_time = CDateTime::Now().AddHours(-6).ToString("yyyyMMddHHmmss");
	//CString v_end_time = CDateTime::Now().AddDays(1).ToString("yyyyMMddHHmmss");
	CString v_station_id = "";

	//系统的分页类信息。
	CPageInfo pageInfo;


	CDbCommand cmd_inq(conn);

	try
	{
		try
		{//获取前台DEV控件传入的分页信息
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		catch (CException& ce)
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = 1000;
		}


		//--------------------------------
		//获取传入参数
		if (bcls_rec->Tables[0].Columns.Contains("STATION_ID"))
			v_station_id = bcls_rec->Tables[0].Rows[0]["STATION_ID"].ToString();
		/*if (bcls_rec->Tables[0].Columns.Contains("END_TIME"))
			v_end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString();*/


		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			//123
			if (v_station_id == "CP")//成品
			{
				Log::Trace("", __FUNCTION__, "STATION_ID[{0}]  ", v_station_id);

				sqlstr = " SELECT DISTINCT(ST_NO) FROM("
					" SELECT  a.*, B.CC_SEQ, B.SG_SIGN, B.CAST_LOT_NO, B.CAST_LOT_DIV_NO, B.CAST_LOT_SUM, B.REFINE_DIV, B.PLAN_POUR_WT, B.NEW_TEST_NO, B.PLAN_DATE"
					" , B.CC_PREP_TIME, B.POUR_TIME, B.CC_REQ_TIME_FLAG, B.HOT_SEND_FLAG, B.HOT_CHARGE_FLAG, B.CARRY_DIV, B.SLAB_DEST, B.BILLET_TYPE, B.SLAB_THICK"
					" , B.SLAB_WIDTH, B.SLAB_LEN, B.SHIFT_NO, B.SHIFT_GROUP, B.C_DIV, B.CC_REQ_TIMEL4, B.CAST_LOT_NO2, B.CAST_LOT_DIV_NO2"
					" , B.CAST_LOT_SUM2, C.START_TIME_REAL FROM TPSSM11 A, TPSSM10 B, TPSSM12 C WHERE a.FACTORY_DIV = 'LG1'"
					" and a.factory_div = b.factory_div and a.pono = b.pono AND a.SM_PLAN_NO = c.SM_PLAN_NO AND c.AREA_ID = 3"
					" AND((c.START_TIME_REAL >= TO_CHAR(SYSDATE - INTERVAL '6' HOUR, 'YYYYMMDDHH24MISS')"
					" AND c.START_TIME_REAL <= TO_CHAR(SYSDATE + 1, 'YYYYMMDDHH24MISS')"
					" OR c.START_TIME_REAL = ' ')"
					" OR a.RUN_STATUS < '53')  order by  c.START_TIME_REAL)";

				//if (v_start_time.Trim() != "")
				//{
				//	sqlstr_temp += " AND REC_CREATE_TIME			>= '202502240000'";
				//	//sqlstr_temp += " AND REC_CREATE_TIME			>= '" + v_start_time + "'";
				//}
				//if (v_end_time.Trim() != "")
				//{
				//	//sqlstr_temp += " AND REC_CREATE_TIME			<= '" + v_end_time + "'";
				//}
			}
			else//原料
			{
				Log::Trace("", __FUNCTION__, "STATION_ID[{0}]  ", v_station_id);

				sqlstr = " SELECT T.ST_NO,T2.GRADE_TYPE1,T2.F_ROUTE1,T2.COMPOSE_LIST_NO2 FROM ("
					" SELECT DISTINCT(ST_NO) FROM("
					" SELECT  a.*, B.CC_SEQ, B.SG_SIGN, B.CAST_LOT_NO, B.CAST_LOT_DIV_NO, B.CAST_LOT_SUM, B.REFINE_DIV, B.PLAN_POUR_WT, B.NEW_TEST_NO, B.PLAN_DATE"
					" , B.CC_PREP_TIME, B.POUR_TIME, B.CC_REQ_TIME_FLAG, B.HOT_SEND_FLAG, B.HOT_CHARGE_FLAG, B.CARRY_DIV, B.SLAB_DEST, B.BILLET_TYPE, B.SLAB_THICK"
					" , B.SLAB_WIDTH, B.SLAB_LEN, B.SHIFT_NO, B.SHIFT_GROUP, B.C_DIV, B.CC_REQ_TIMEL4, B.CAST_LOT_NO2, B.CAST_LOT_DIV_NO2"
					" , B.CAST_LOT_SUM2, C.START_TIME_REAL FROM TPSSM11 A, TPSSM10 B, TPSSM12 C WHERE a.FACTORY_DIV = 'LG1'"
					" and a.factory_div = b.factory_div and a.pono = b.pono AND a.SM_PLAN_NO = c.SM_PLAN_NO AND c.AREA_ID = 3"
					" AND((c.START_TIME_REAL >= TO_CHAR(SYSDATE, 'YYYYMMDDHH24MISS')"
					" AND c.START_TIME_REAL <= TO_CHAR(SYSDATE + 1, 'YYYYMMDDHH24MISS')"
					" OR c.START_TIME_REAL = ' ')"
					" OR a.RUN_STATUS < '53')  order by  c.START_TIME_REAL))T"
					" LEFT JOIN TQMTSCB09_DR T1 ON T.ST_NO = T1.STEEL_GRADE"
					" LEFT JOIN TQMTSCB11D_DR T2 ON T1.STEEL_TYPE = T2.GRADE_TYPE1";

				//if (v_start_time.Trim() != "")
				//{
				//	sqlstr_temp += " AND REC_CREATE_TIME			>= '202502240000'";
				//	//sqlstr_temp += " AND REC_CREATE_TIME			>= '" + v_start_time + "'";
				//}
				//if (v_end_time.Trim() != "")
				//{
				//	//sqlstr_temp += " AND REC_CREATE_TIME			<= '" + v_end_time + "'";
				//}
			}
			

			//sqlstr_temp += " ORDER BY  REC_CREATE_TIME)";
			sqlstr = sqlstr + sqlstr_temp;
			break;
		}

		Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);

		//分页获取
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], pageInfo.RecordFrom, pageInfo.PageSize);
		cmd_inq.Close();

		//返回分页总数量信息
		bcls_ret->Tables.Add("PageInfo");
		bcls_ret->Tables["PageInfo"].Columns.Add(DT_DECIMAL, "TotalRecordCount");
		bcls_ret->Tables["PageInfo"].Rows.Add();
		bcls_ret->Tables["PageInfo"].Rows[0]["TotalRecordCount"] = TotalRecordCount;

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
