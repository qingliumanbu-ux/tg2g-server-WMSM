/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         BHY
Version:		1.0
Date:			2025-02-21
Description:
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"
//程序用头文件


//函数申明

/*<remark>=========================================================

===========================================================</remark>*/

BM2F_ENTERACE(wmsmyry_inq);

int f_wmsmyry_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	int ret = 0;

	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */

	EPEX epex;

	/* 数据库SQL操作字符串 */
	CString sql = "";
	CString sqlstr = "";
	CString sqlwhere = "";
	CString sqlstr_count;
	CString sqlstr_temp;
	CString datetime = " ";

	CString heat_no_a0_a = " ";
	CDecimal heat_no_a0_b = 0;
	CString factory_2_a0 = " ";

	CString heat_no_a1_a = " ";
	CDecimal heat_no_a1_b = 0;
	CString factory_2_a1 = " ";

	CString heat_no_a2_a = " ";
	CDecimal heat_no_a2_b = 0;
	CString factory_2_a2 = " ";

	CString heat_no_f1_a = " ";
	CDecimal heat_no_f1_b = 0;
	CString factory_2_f1 = " ";

	CString heat_no_f2_a = " ";
	CDecimal heat_no_f2_b = 0;
	CString factory_2_f2 = " ";

	CString heat_no_f3_a = " ";
	CDecimal heat_no_f3_b = 0;
	CString factory_2_f3 = " ";

	CString heat_no_f4_a = " ";
	CDecimal heat_no_f4_b = 0;
	CString factory_2_f4 = " ";

	CString heat_no_e1_a = " ";
	CDecimal heat_no_e1_b = 0;
	CString factory_2_e1 = " ";

	CString heat_no_e2_a = " ";
	CDecimal heat_no_e2_b = 0;
	CString factory_2_e2 = " ";


	/* 业务变量 */
	CString v_st_no = " ";
	/* 全局变量 */

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_sql(conn);
	CModel twmsmyry("TWMSMYRY");
	CModel twmsmyry_1("TWMSMYRY");

	//系统的分页类信息。

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		//AO\A1\A2
		//F1\F2\F3\F4
		//E1\E2
		//计划炉号每次查询后插入修改表中
		sql = "select substr(heat_no, 0, 4) as HEAT_NO_A,to_number(substr(heat_no, 5, 4)) AS HEAT_NO_B, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn\
			from TPSSMSS t1 where t1.dev_code = 'A0' and t1.event_id = '3')\
			where rn = '1'";
		Log::Trace("", "", "sql_a0=[{0}]", sql);
		cmd_sql.SetCommandText(sql);
		cmd_sql.ExecuteReader();
		if (cmd_sql.Read())
		{
			heat_no_a0_a = cmd_sql.GetString(1);
			heat_no_a0_b = cmd_sql.GetDecimal(2);
			factory_2_a0 = cmd_sql.GetString(3);
		}
		cmd_sql.Close();
		
		for (int i = 0; i < 6; i++)
		{
			Log::Trace("", __FUNCTION__, "a0", "");
			
			twmsmyry.Reset();
			if ((heat_no_a0_b + i).ToString().GetLength() == 4)
			{
				twmsmyry["HEAT_NO_OLD"] = heat_no_a0_a + (heat_no_a0_b + i).ToString();
				twmsmyry["HEAT_NO"] = heat_no_a0_a + (heat_no_a0_b + i).ToString();
			}
			if ((heat_no_a0_b + i).ToString().GetLength() == 3)
			{
				twmsmyry["HEAT_NO_OLD"] = heat_no_a0_a + "0" + (heat_no_a0_b + i).ToString();
				twmsmyry["HEAT_NO"] = heat_no_a0_a + "0" + (heat_no_a0_b + i).ToString();
			}
			if ((heat_no_a0_b + i).ToString().GetLength() == 2)
			{
				twmsmyry["HEAT_NO_OLD"] = heat_no_a0_a + "00" + (heat_no_a0_b + i).ToString();
				twmsmyry["HEAT_NO"] = heat_no_a0_a + "00" + (heat_no_a0_b + i).ToString();
			}
			/*twmsmyry["HEAT_NO_OLD"] = heat_no_a0_a + (heat_no_a0_b + i).ToString();
			twmsmyry["HEAT_NO"] = heat_no_a0_a + (heat_no_a0_b + i).ToString();*/
			twmsmyry["FACTORY_2"] = factory_2_a0;
			if (heat_no_a0_a.Trim().GetLength() > 0 && !twmsmyry.Query("HEAT_NO_OLD,FACTORY_2"))
			{
				twmsmyry["REC_CREATE_TIME"] = datetime;
				twmsmyry["REC_CREATOR"] = s.userid;
				twmsmyry.Insert();
				Log::Trace("", __FUNCTION__, "插入A0记录[{0}]", i);
			}
		}

		sql = "select substr(heat_no, 0, 4) as HEAT_NO_A,to_number(substr(heat_no, 5, 4)) AS HEAT_NO_B, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn\
			from TPSSMSS t1 where t1.dev_code = 'A1' and t1.event_id = '3')\
			where rn = '1'";
		Log::Trace("", "", "sql_a1=[{0}]", sql);
		cmd_sql.SetCommandText(sql);
		cmd_sql.ExecuteReader();
		if (cmd_sql.Read())
		{
			heat_no_a1_a = cmd_sql.GetString(1);
			heat_no_a1_b = cmd_sql.GetDecimal(2);
			factory_2_a1 = cmd_sql.GetString(3);
		}
		cmd_sql.Close();

		for (int i = 0; i < 6; i++)
		{
			Log::Trace("", __FUNCTION__, "a1", "");
			
			twmsmyry.Reset();
			if ((heat_no_a1_b + i).ToString().GetLength() == 4)
			{
				twmsmyry["HEAT_NO_OLD"] = heat_no_a1_a + (heat_no_a1_b + i).ToString();
				twmsmyry["HEAT_NO"] = heat_no_a1_a + (heat_no_a1_b + i).ToString();
			}
			if ((heat_no_a1_b + i).ToString().GetLength() == 3)
			{
				twmsmyry["HEAT_NO_OLD"] = heat_no_a1_a + "0" + (heat_no_a1_b + i).ToString();
				twmsmyry["HEAT_NO"] = heat_no_a1_a + "0" + (heat_no_a1_b + i).ToString();
			}
			if ((heat_no_a1_b + i).ToString().GetLength() == 2)
			{
				twmsmyry["HEAT_NO_OLD"] = heat_no_a1_a + "00" + (heat_no_a1_b + i).ToString();
				twmsmyry["HEAT_NO"] = heat_no_a1_a + "00" + (heat_no_a1_b + i).ToString();
			}
			/*twmsmyry["HEAT_NO_OLD"] = heat_no_a1_a + (heat_no_a1_b + i).ToString();
			twmsmyry["HEAT_NO"] = heat_no_a1_a + (heat_no_a1_b + i).ToString();*/
			twmsmyry["FACTORY_2"] = factory_2_a1;
			if (heat_no_a1_a.Trim().GetLength() > 0 && !twmsmyry.Query("HEAT_NO_OLD,FACTORY_2"))
			{
				twmsmyry["REC_CREATE_TIME"] = datetime;
				twmsmyry["REC_CREATOR"] = s.userid;
				twmsmyry.Insert();
				Log::Trace("", __FUNCTION__, "插入A1记录[{0}]", i);
			}
		}

		sql = "select substr(heat_no, 0, 4) as HEAT_NO_A,to_number(substr(heat_no, 5, 4)) AS HEAT_NO_B, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn\
			from TPSSMSS t1 where t1.dev_code = 'A2' and t1.event_id = '3')\
			where rn = '1'";
		Log::Trace("", "", "sql_a2=[{0}]", sql);
		cmd_sql.SetCommandText(sql);
		cmd_sql.ExecuteReader();
		if (cmd_sql.Read())
		{
			heat_no_a2_a = cmd_sql.GetString(1);
			heat_no_a2_b = cmd_sql.GetDecimal(2);
			factory_2_a2 = cmd_sql.GetString(3);
		}
		cmd_sql.Close();

		for (int i = 0; i < 6; i++)
		{
			Log::Trace("", __FUNCTION__, "a2", "");
			
			twmsmyry.Reset();
			if ((heat_no_a2_b + i).ToString().GetLength() == 4)
			{
				twmsmyry["HEAT_NO_OLD"] = heat_no_a2_a + (heat_no_a2_b + i).ToString();
				twmsmyry["HEAT_NO"] = heat_no_a2_a + (heat_no_a2_b + i).ToString();
			}
			if ((heat_no_a2_b + i).ToString().GetLength() == 3)
			{
				twmsmyry["HEAT_NO_OLD"] = heat_no_a2_a + "0" + (heat_no_a2_b + i).ToString();
				twmsmyry["HEAT_NO"] = heat_no_a2_a + "0" + (heat_no_a2_b + i).ToString();
			}
			if ((heat_no_a2_b + i).ToString().GetLength() == 2)
			{
				twmsmyry["HEAT_NO_OLD"] = heat_no_a2_a + "00" + (heat_no_a2_b + i).ToString();
				twmsmyry["HEAT_NO"] = heat_no_a2_a + "00" + (heat_no_a2_b + i).ToString();
			}
			/*twmsmyry["HEAT_NO_OLD"] = heat_no_a2_a + (heat_no_a2_b + i).ToString();
			twmsmyry["HEAT_NO"] = heat_no_a2_a + (heat_no_a2_b + i).ToString();*/
			twmsmyry["FACTORY_2"] = factory_2_a2;
			if (heat_no_a2_a.Trim().GetLength() > 0 && !twmsmyry.Query("HEAT_NO_OLD,FACTORY_2"))
			{
				twmsmyry["REC_CREATE_TIME"] = datetime;
				twmsmyry["REC_CREATOR"] = s.userid;
				twmsmyry.Insert();
				Log::Trace("", __FUNCTION__, "插入A2记录[{0}]", i);
			}
		}

		sql = "select substr(heat_no, 0, 4) as HEAT_NO_A,to_number(substr(heat_no, 5, 4)) AS HEAT_NO_B, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn\
			from TPSSMSS t1 where t1.dev_code = 'Z1' and t1.event_id = '3')\
			where rn = '1'";
		Log::Trace("", "", "sql_f1=[{0}]", sql);
		cmd_sql.SetCommandText(sql);
		cmd_sql.ExecuteReader();
		if (cmd_sql.Read())
		{
			heat_no_f1_a = cmd_sql.GetString(1);
			heat_no_f1_b = cmd_sql.GetDecimal(2);
			factory_2_f1 = cmd_sql.GetString(3);
		}
		cmd_sql.Close();

		Log::Trace("", __FUNCTION__, "heat_no_f1_a[{0}]", heat_no_f1_a);
		for (int i = 0; i < 5; i++)
		{
			Log::Trace("", __FUNCTION__, "f1", "");
			
			twmsmyry.Reset();
			if ((heat_no_f1_b + i).ToString().GetLength() == 4)
			{
				twmsmyry["HEAT_NO_OLD"] = heat_no_f1_a + (heat_no_f1_b + i).ToString();
				twmsmyry["HEAT_NO"] = heat_no_f1_a + (heat_no_f1_b + i).ToString();
			}
			if ((heat_no_f1_b + i).ToString().GetLength() == 3)
			{
				twmsmyry["HEAT_NO_OLD"] = heat_no_f1_a + "0" + (heat_no_f1_b + i).ToString();
				twmsmyry["HEAT_NO"] = heat_no_f1_a + "0" + (heat_no_f1_b + i).ToString();
			}
			if ((heat_no_f1_b + i).ToString().GetLength() == 2)
			{
				twmsmyry["HEAT_NO_OLD"] = heat_no_f1_a + "00" + (heat_no_f1_b + i).ToString();
				twmsmyry["HEAT_NO"] = heat_no_f1_a + "00" + (heat_no_f1_b + i).ToString();
			}
			/*twmsmyry["HEAT_NO_OLD"] = heat_no_f1_a + (heat_no_f1_b + i).ToString();
			twmsmyry["HEAT_NO"] = heat_no_f1_a + (heat_no_f1_b + i).ToString();*/
			twmsmyry["FACTORY_2"] = factory_2_f1;
			if (heat_no_f1_a.Trim().GetLength() > 0 && !twmsmyry.Query("HEAT_NO_OLD,FACTORY_2"))
			{
				twmsmyry["REC_CREATE_TIME"] = datetime;
				twmsmyry["REC_CREATOR"] = s.userid;
				twmsmyry.Insert();
				Log::Trace("", __FUNCTION__, "插入F1记录[{0}]", i);
			}
		}

		sql = "select substr(heat_no, 0, 4) as HEAT_NO_A,to_number(substr(heat_no, 5, 4)) AS HEAT_NO_B, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn\
			from TPSSMSS t1 where t1.dev_code = 'Z2' and t1.event_id = '3')\
			where rn = '1'";
		Log::Trace("", "", "sql_f2=[{0}]", sql);
		cmd_sql.SetCommandText(sql);
		cmd_sql.ExecuteReader();
		if (cmd_sql.Read())
		{
			heat_no_f2_a = cmd_sql.GetString(1);
			heat_no_f2_b = cmd_sql.GetDecimal(2);
			factory_2_f2 = cmd_sql.GetString(3);
		}
		cmd_sql.Close();

		for (int i = 0; i < 5; i++)
		{
			Log::Trace("", __FUNCTION__, "f2", "");

			twmsmyry.Reset();
			if ((heat_no_f2_b + i).ToString().GetLength() == 4)
			{
				twmsmyry["HEAT_NO_OLD"] = heat_no_f2_a + (heat_no_f2_b + i).ToString();
				twmsmyry["HEAT_NO"] = heat_no_f2_a + (heat_no_f2_b + i).ToString();
			}
			if ((heat_no_f2_b + i).ToString().GetLength() == 3)
			{
				twmsmyry["HEAT_NO_OLD"] = heat_no_f2_a + "0" + (heat_no_f2_b + i).ToString();
				twmsmyry["HEAT_NO"] = heat_no_f2_a + "0" + (heat_no_f2_b + i).ToString();
			}
			if ((heat_no_f2_b + i).ToString().GetLength() == 2)
			{
				twmsmyry["HEAT_NO_OLD"] = heat_no_f2_a + "00" + (heat_no_f2_b + i).ToString();
				twmsmyry["HEAT_NO"] = heat_no_f2_a + "00" + (heat_no_f2_b + i).ToString();
			}
			//twmsmyry["HEAT_NO_OLD"] = heat_no_f2_a + (heat_no_f2_b + i).ToString();
			//twmsmyry["HEAT_NO"] = heat_no_f2_a + (heat_no_f2_b + i).ToString();
			twmsmyry["FACTORY_2"] = factory_2_f2;
			if (heat_no_f2_a.Trim().GetLength() > 0 && !twmsmyry.Query("HEAT_NO_OLD,FACTORY_2"))
			{
				twmsmyry["REC_CREATE_TIME"] = datetime;
				twmsmyry["REC_CREATOR"] = s.userid;
				twmsmyry.Insert();
				Log::Trace("", __FUNCTION__, "插入F2记录[{0}]", i);
			}
		}

		sql = "select substr(heat_no, 0, 4) as HEAT_NO_A,to_number(substr(heat_no, 5, 4)) AS HEAT_NO_B, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn\
			from TPSSMSS t1 where t1.dev_code = 'Z3' and t1.event_id = '3')\
			where rn = '1'";
		Log::Trace("", "", "sql_f3=[{0}]", sql);
		cmd_sql.SetCommandText(sql);
		cmd_sql.ExecuteReader();
		if (cmd_sql.Read())
		{
			heat_no_f3_a = cmd_sql.GetString(1);
			heat_no_f3_b = cmd_sql.GetDecimal(2);
			factory_2_f3 = cmd_sql.GetString(3);
		}
		cmd_sql.Close();

		for (int i = 0; i < 5; i++)
		{
			Log::Trace("", __FUNCTION__, "f3", "");

			twmsmyry.Reset();
			if ((heat_no_f3_b + i).ToString().GetLength() == 4)
			{
				twmsmyry["HEAT_NO_OLD"] = heat_no_f3_a + (heat_no_f3_b + i).ToString();
				twmsmyry["HEAT_NO"] = heat_no_f3_a + (heat_no_f3_b + i).ToString();
			}
			if ((heat_no_f3_b + i).ToString().GetLength() == 3)
			{
				twmsmyry["HEAT_NO_OLD"] = heat_no_f3_a + "0" + (heat_no_f3_b + i).ToString();
				twmsmyry["HEAT_NO"] = heat_no_f3_a + "0" + (heat_no_f3_b + i).ToString();
			}
			if ((heat_no_f3_b + i).ToString().GetLength() == 2)
			{
				twmsmyry["HEAT_NO_OLD"] = heat_no_f3_a + "00" + (heat_no_f3_b + i).ToString();
				twmsmyry["HEAT_NO"] = heat_no_f3_a + "00" + (heat_no_f3_b + i).ToString();
			}
			/*twmsmyry["HEAT_NO_OLD"] = heat_no_f3_a + (heat_no_f3_b + i).ToString();
			twmsmyry["HEAT_NO"] = heat_no_f3_a + (heat_no_f3_b + i).ToString();*/
			twmsmyry["FACTORY_2"] = factory_2_f3;
			if (heat_no_f3_a.Trim().GetLength() > 0 && !twmsmyry.Query("HEAT_NO_OLD,FACTORY_2"))
			{
				twmsmyry["REC_CREATE_TIME"] = datetime;
				twmsmyry["REC_CREATOR"] = s.userid;
				twmsmyry.Insert();
				Log::Trace("", __FUNCTION__, "插入F3记录[{0}]", i);
			}
		}

		sql = "select substr(heat_no, 0, 4) as HEAT_NO_A,to_number(substr(heat_no, 5, 4)) AS HEAT_NO_B, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn\
			from TPSSMSS t1 where t1.dev_code = 'Z4' and t1.event_id = '3')\
			where rn = '1'";
		Log::Trace("", "", "sql_f4=[{0}]", sql);
		cmd_sql.SetCommandText(sql);
		cmd_sql.ExecuteReader();
		if (cmd_sql.Read())
		{
			heat_no_f4_a = cmd_sql.GetString(1);
			heat_no_f4_b = cmd_sql.GetDecimal(2);
			factory_2_f4 = cmd_sql.GetString(3);
		}
		cmd_sql.Close();

		for (int i = 0; i < 5; i++)
		{
			Log::Trace("", __FUNCTION__, "f4", "");

			twmsmyry.Reset();
			if ((heat_no_f4_b + i).ToString().GetLength() == 4)
			{
				twmsmyry["HEAT_NO_OLD"] = heat_no_f4_a + (heat_no_f4_b + i).ToString();
				twmsmyry["HEAT_NO"] = heat_no_f4_a + (heat_no_f4_b + i).ToString();
			}
			if ((heat_no_f4_b + i).ToString().GetLength() == 3)
			{
				twmsmyry["HEAT_NO_OLD"] = heat_no_f4_a + "0" + (heat_no_f4_b + i).ToString();
				twmsmyry["HEAT_NO"] = heat_no_f4_a + "0" + (heat_no_f4_b + i).ToString();
			}
			if ((heat_no_f4_b + i).ToString().GetLength() == 2)
			{
				twmsmyry["HEAT_NO_OLD"] = heat_no_f4_a + "00" + (heat_no_f4_b + i).ToString();
				twmsmyry["HEAT_NO"] = heat_no_f4_a + "00" + (heat_no_f4_b + i).ToString();
			}
			/*twmsmyry["HEAT_NO_OLD"] = heat_no_f4_a + (heat_no_f4_b + i).ToString();
			twmsmyry["HEAT_NO"] = heat_no_f4_a + (heat_no_f4_b + i).ToString();*/
			twmsmyry["FACTORY_2"] = factory_2_f4;
			if (heat_no_f4_a.Trim().GetLength() > 0 && !twmsmyry.Query("HEAT_NO_OLD,FACTORY_2"))
			{
				twmsmyry["REC_CREATE_TIME"] = datetime;
				twmsmyry["REC_CREATOR"] = s.userid;
				twmsmyry.Insert();
				Log::Trace("", __FUNCTION__, "插入F4记录[{0}]", i);
			}
		}

		sql = "select substr(heat_no, 0, 4) as HEAT_NO_A,to_number(substr(heat_no, 5, 4)) AS HEAT_NO_B, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn\
			from TPSSMSS t1 where t1.dev_code = 'E1' and t1.event_id = '3')\
			where rn = '1'";
		Log::Trace("", "", "sql_e1=[{0}]", sql);
		cmd_sql.SetCommandText(sql);
		cmd_sql.ExecuteReader();
		if (cmd_sql.Read())
		{
			heat_no_e1_a = cmd_sql.GetString(1);
			heat_no_e1_b = cmd_sql.GetDecimal(2);
			factory_2_e1 = cmd_sql.GetString(3);
		}
		cmd_sql.Close();

		for (int i = 0; i < 5; i++)
		{
			Log::Trace("", __FUNCTION__, "e1", "");

			twmsmyry.Reset();
			if ((heat_no_e1_b + i).ToString().GetLength() == 4)
			{
				twmsmyry["HEAT_NO_OLD"] = heat_no_e1_a + (heat_no_e1_b + i).ToString();
				twmsmyry["HEAT_NO"] = heat_no_e1_a + (heat_no_e1_b + i).ToString();
			}
			if ((heat_no_e1_b + i).ToString().GetLength() == 3)
			{
				twmsmyry["HEAT_NO_OLD"] = heat_no_e1_a + "0" + (heat_no_e1_b + i).ToString();
				twmsmyry["HEAT_NO"] = heat_no_e1_a + "0" + (heat_no_e1_b + i).ToString();
			}
			if ((heat_no_e1_b + i).ToString().GetLength() == 2)
			{
				twmsmyry["HEAT_NO_OLD"] = heat_no_e1_a + "00" + (heat_no_e1_b + i).ToString();
				twmsmyry["HEAT_NO"] = heat_no_e1_a + "00" + (heat_no_e1_b + i).ToString();
			}
			/*twmsmyry["HEAT_NO_OLD"] = heat_no_e1_a + (heat_no_e1_b + i).ToString();
			twmsmyry["HEAT_NO"] = heat_no_e1_a + (heat_no_e1_b + i).ToString();*/
			twmsmyry["FACTORY_2"] = factory_2_e1;
			if (heat_no_e1_a.Trim().GetLength() > 0 && !twmsmyry.Query("HEAT_NO_OLD,FACTORY_2"))
			{
				twmsmyry["REC_CREATE_TIME"] = datetime;
				twmsmyry["REC_CREATOR"] = s.userid;
				twmsmyry.Insert();
				Log::Trace("", __FUNCTION__, "插入E1记录[{0}]", i);
			}
		}

		sql = "select substr(heat_no, 0, 4) as HEAT_NO_A,to_number(substr(heat_no, 5, 4)) AS HEAT_NO_B, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn\
			from TPSSMSS t1 where t1.dev_code = 'E2' and t1.event_id = '3')\
			where rn = '1'";
		Log::Trace("", "", "sql_e2=[{0}]", sql);
		cmd_sql.SetCommandText(sql);
		cmd_sql.ExecuteReader();
		if (cmd_sql.Read())
		{
			heat_no_e2_a = cmd_sql.GetString(1);
			heat_no_e2_b = cmd_sql.GetDecimal(2);
			factory_2_e2 = cmd_sql.GetString(3);
		}
		cmd_sql.Close();

		for (int i = 0; i < 5; i++)
		{
			Log::Trace("", __FUNCTION__, "e2", "");
			
			twmsmyry.Reset();
			if ((heat_no_e2_b + i).ToString().GetLength() == 4)
			{
				twmsmyry["HEAT_NO_OLD"] = heat_no_e2_a + (heat_no_e2_b + i).ToString();
				twmsmyry["HEAT_NO"] = heat_no_e2_a + (heat_no_e2_b + i).ToString();
			}
			if ((heat_no_e2_b + i).ToString().GetLength() == 3)
			{
				twmsmyry["HEAT_NO_OLD"] = heat_no_e2_a + "0" + (heat_no_e2_b + i).ToString();
				twmsmyry["HEAT_NO"] = heat_no_e2_a + "0" + (heat_no_e2_b + i).ToString();
			}
			if ((heat_no_e2_b + i).ToString().GetLength() == 2)
			{
				twmsmyry["HEAT_NO_OLD"] = heat_no_e2_a + "00" + (heat_no_e2_b + i).ToString();
				twmsmyry["HEAT_NO"] = heat_no_e2_a + "00" + (heat_no_e2_b + i).ToString();
			}
			/*twmsmyry["HEAT_NO_OLD"] = heat_no_e2_a + (heat_no_e2_b + i).ToString();
			twmsmyry["HEAT_NO"] = heat_no_e2_a + (heat_no_e2_b + i).ToString();*/
			twmsmyry["FACTORY_2"] = factory_2_e2;
			if (heat_no_e2_a.Trim().GetLength() > 0 && !twmsmyry.Query("HEAT_NO_OLD,FACTORY_2"))
			{
				twmsmyry["REC_CREATE_TIME"] = datetime;
				twmsmyry["REC_CREATOR"] = s.userid;
				twmsmyry.Insert();
				Log::Trace("", __FUNCTION__, "插入E2记录[{0}]", i);
			}
		}
		

		//AOD
		bcls_ret->Tables.Add();
		sql = "SELECT DECODE(T2.HEAT_NO,T2.HEAT_NO_OLD,T2.HEAT_NO_OLD,T2.HEAT_NO) AS HEAT_NO,T2.HEAT_NO_OLD,T.FACTORY_2,T2.ST_NO,T2.SAP_ERP_PRCSPATH,T2.OUT_STEEL_TIME,T2.IDCARD FROM (\
			select heat_no, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'A0' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2\
			UNION ALL\
			SELECT DECODE(T2.HEAT_NO, T2.HEAT_NO_OLD, T2.HEAT_NO_OLD, T2.HEAT_NO) AS HEAT_NO, T2.HEAT_NO_OLD, T.FACTORY_2, T2.ST_NO, T2.SAP_ERP_PRCSPATH, T2.OUT_STEEL_TIME, T2.IDCARD FROM(\
			select concat(substr(heat_no, 0, 4), lpad(to_char(to_number(substr(heat_no, 5, 4)) + 1), 4, '0')) AS HEAT_NO, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'A0' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2\
			UNION ALL\
			SELECT DECODE(T2.HEAT_NO, T2.HEAT_NO_OLD, T2.HEAT_NO_OLD, T2.HEAT_NO) AS HEAT_NO, T2.HEAT_NO_OLD, T.FACTORY_2, T2.ST_NO, T2.SAP_ERP_PRCSPATH, T2.OUT_STEEL_TIME, T2.IDCARD FROM(\
			select concat(substr(heat_no, 0, 4), lpad(to_char(to_number(substr(heat_no, 5, 4)) + 2), 4, '0')) AS HEAT_NO, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'A0' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2\
			UNION ALL\
			SELECT DECODE(T2.HEAT_NO, T2.HEAT_NO_OLD, T2.HEAT_NO_OLD, T2.HEAT_NO) AS HEAT_NO, T2.HEAT_NO_OLD, T.FACTORY_2, T2.ST_NO, T2.SAP_ERP_PRCSPATH, T2.OUT_STEEL_TIME, T2.IDCARD FROM(\
			select concat(substr(heat_no, 0, 4), lpad(to_char(to_number(substr(heat_no, 5, 4)) + 3), 4, '0')) AS HEAT_NO, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'A0' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2\
			UNION ALL\
			SELECT DECODE(T2.HEAT_NO, T2.HEAT_NO_OLD, T2.HEAT_NO_OLD, T2.HEAT_NO) AS HEAT_NO, T2.HEAT_NO_OLD, T.FACTORY_2, T2.ST_NO, T2.SAP_ERP_PRCSPATH, T2.OUT_STEEL_TIME, T2.IDCARD FROM(\
			select concat(substr(heat_no, 0, 4), lpad(to_char(to_number(substr(heat_no, 5, 4)) + 4), 4, '0')) AS HEAT_NO, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'A0' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2\
			UNION ALL\
			SELECT DECODE(T2.HEAT_NO, T2.HEAT_NO_OLD, T2.HEAT_NO_OLD, T2.HEAT_NO) AS HEAT_NO, T2.HEAT_NO_OLD, T.FACTORY_2, T2.ST_NO, T2.SAP_ERP_PRCSPATH, T2.OUT_STEEL_TIME, T2.IDCARD FROM(\
			select concat(substr(heat_no, 0, 4), lpad(to_char(to_number(substr(heat_no, 5, 4)) + 5), 4, '0')) AS HEAT_NO, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'A0' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2";
		Log::Trace("", "", "sql3=[{0}]", sql);
		cmd_inq.SetCommandText(sql);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();
		/*if (bcls_ret->Tables[0].Rows.get_Count() == 0)
		{
			bcls_ret->Tables[0].Rows.Add();
		}*/


		bcls_ret->Tables.Add();
		sql = "SELECT DECODE(T2.HEAT_NO,T2.HEAT_NO_OLD,T2.HEAT_NO_OLD,T2.HEAT_NO) AS HEAT_NO,T2.HEAT_NO_OLD,T.FACTORY_2,T2.ST_NO,T2.SAP_ERP_PRCSPATH,T2.OUT_STEEL_TIME,T2.IDCARD FROM (\
			select heat_no, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'A1' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2\
			UNION ALL\
			SELECT DECODE(T2.HEAT_NO, T2.HEAT_NO_OLD, T2.HEAT_NO_OLD, T2.HEAT_NO) AS HEAT_NO, T2.HEAT_NO_OLD, T.FACTORY_2, T2.ST_NO, T2.SAP_ERP_PRCSPATH, T2.OUT_STEEL_TIME, T2.IDCARD FROM(\
			select concat(substr(heat_no, 0, 4), lpad(to_char(to_number(substr(heat_no, 5, 4)) + 1), 4, '0')) AS HEAT_NO, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'A1' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2\
			UNION ALL\
			SELECT DECODE(T2.HEAT_NO, T2.HEAT_NO_OLD, T2.HEAT_NO_OLD, T2.HEAT_NO) AS HEAT_NO, T2.HEAT_NO_OLD, T.FACTORY_2, T2.ST_NO, T2.SAP_ERP_PRCSPATH, T2.OUT_STEEL_TIME, T2.IDCARD FROM(\
			select concat(substr(heat_no, 0, 4), lpad(to_char(to_number(substr(heat_no, 5, 4)) + 2), 4, '0')) AS HEAT_NO, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'A1' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2\
			UNION ALL\
			SELECT DECODE(T2.HEAT_NO, T2.HEAT_NO_OLD, T2.HEAT_NO_OLD, T2.HEAT_NO) AS HEAT_NO, T2.HEAT_NO_OLD, T.FACTORY_2, T2.ST_NO, T2.SAP_ERP_PRCSPATH, T2.OUT_STEEL_TIME, T2.IDCARD FROM(\
			select concat(substr(heat_no, 0, 4), lpad(to_char(to_number(substr(heat_no, 5, 4)) + 3), 4, '0')) AS HEAT_NO, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'A1' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2\
			UNION ALL\
			SELECT DECODE(T2.HEAT_NO, T2.HEAT_NO_OLD, T2.HEAT_NO_OLD, T2.HEAT_NO) AS HEAT_NO, T2.HEAT_NO_OLD, T.FACTORY_2, T2.ST_NO, T2.SAP_ERP_PRCSPATH, T2.OUT_STEEL_TIME, T2.IDCARD FROM(\
			select concat(substr(heat_no, 0, 4), lpad(to_char(to_number(substr(heat_no, 5, 4)) + 4), 4, '0')) AS HEAT_NO, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'A1' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2\
			UNION ALL\
			SELECT DECODE(T2.HEAT_NO, T2.HEAT_NO_OLD, T2.HEAT_NO_OLD, T2.HEAT_NO) AS HEAT_NO, T2.HEAT_NO_OLD, T.FACTORY_2, T2.ST_NO, T2.SAP_ERP_PRCSPATH, T2.OUT_STEEL_TIME, T2.IDCARD FROM(\
			select concat(substr(heat_no, 0, 4), lpad(to_char(to_number(substr(heat_no, 5, 4)) + 5), 4, '0')) AS HEAT_NO, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'A1' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2";
		Log::Trace("", "", "sql4=[{0}]", sql);
		cmd_inq.SetCommandText(sql);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[1]);
		cmd_inq.Close();
		

		bcls_ret->Tables.Add();
		sql = "SELECT DECODE(T2.HEAT_NO,T2.HEAT_NO_OLD,T2.HEAT_NO_OLD,T2.HEAT_NO) AS HEAT_NO,T2.HEAT_NO_OLD,T.FACTORY_2,T2.ST_NO,T2.SAP_ERP_PRCSPATH,T2.OUT_STEEL_TIME,T2.IDCARD FROM (\
			select heat_no, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'A2' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2\
			UNION ALL\
			SELECT DECODE(T2.HEAT_NO, T2.HEAT_NO_OLD, T2.HEAT_NO_OLD, T2.HEAT_NO) AS HEAT_NO, T2.HEAT_NO_OLD, T.FACTORY_2, T2.ST_NO, T2.SAP_ERP_PRCSPATH, T2.OUT_STEEL_TIME, T2.IDCARD FROM(\
			select concat(substr(heat_no, 0, 4), lpad(to_char(to_number(substr(heat_no, 5, 4)) + 1), 4, '0')) AS HEAT_NO, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'A2' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2\
			UNION ALL\
			SELECT DECODE(T2.HEAT_NO, T2.HEAT_NO_OLD, T2.HEAT_NO_OLD, T2.HEAT_NO) AS HEAT_NO, T2.HEAT_NO_OLD, T.FACTORY_2, T2.ST_NO, T2.SAP_ERP_PRCSPATH, T2.OUT_STEEL_TIME, T2.IDCARD FROM(\
			select concat(substr(heat_no, 0, 4), lpad(to_char(to_number(substr(heat_no, 5, 4)) + 2), 4, '0')) AS HEAT_NO, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'A2' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2\
			UNION ALL\
			SELECT DECODE(T2.HEAT_NO, T2.HEAT_NO_OLD, T2.HEAT_NO_OLD, T2.HEAT_NO) AS HEAT_NO, T2.HEAT_NO_OLD, T.FACTORY_2, T2.ST_NO, T2.SAP_ERP_PRCSPATH, T2.OUT_STEEL_TIME, T2.IDCARD FROM(\
			select concat(substr(heat_no, 0, 4), lpad(to_char(to_number(substr(heat_no, 5, 4)) + 3), 4, '0')) AS HEAT_NO, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'A2' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2\
			UNION ALL\
			SELECT DECODE(T2.HEAT_NO, T2.HEAT_NO_OLD, T2.HEAT_NO_OLD, T2.HEAT_NO) AS HEAT_NO, T2.HEAT_NO_OLD, T.FACTORY_2, T2.ST_NO, T2.SAP_ERP_PRCSPATH, T2.OUT_STEEL_TIME, T2.IDCARD FROM(\
			select concat(substr(heat_no, 0, 4), lpad(to_char(to_number(substr(heat_no, 5, 4)) + 4), 4, '0')) AS HEAT_NO, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'A2' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2\
			UNION ALL\
			SELECT DECODE(T2.HEAT_NO, T2.HEAT_NO_OLD, T2.HEAT_NO_OLD, T2.HEAT_NO) AS HEAT_NO, T2.HEAT_NO_OLD, T.FACTORY_2, T2.ST_NO, T2.SAP_ERP_PRCSPATH, T2.OUT_STEEL_TIME, T2.IDCARD FROM(\
			select concat(substr(heat_no, 0, 4), lpad(to_char(to_number(substr(heat_no, 5, 4)) + 5), 4, '0')) AS HEAT_NO, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'A2' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2";
		Log::Trace("", "", "sql5", sql);
		cmd_inq.SetCommandText(sql);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[2]);
		cmd_inq.Close();
		

		//AMF
		bcls_ret->Tables.Add();
		sql = "SELECT * FROM (SELECT DECODE(T2.HEAT_NO,T2.HEAT_NO_OLD,T2.HEAT_NO_OLD,T2.HEAT_NO) AS HEAT_NO,T2.HEAT_NO_OLD,T.FACTORY_2,T2.ST_NO,T2.SAP_ERP_PRCSPATH,T2.OUT_STEEL_TIME,T2.IDCARD FROM (\
			select heat_no, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'Z1' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2)\
			WHERE OUT_STEEL_TIME = ' ' OR OUT_STEEL_TIME>to_char(sysdate, 'yyyyMMddHH24miss')\
			UNION ALL\
			SELECT * FROM(SELECT DECODE(T2.HEAT_NO, T2.HEAT_NO_OLD, T2.HEAT_NO_OLD, T2.HEAT_NO) AS HEAT_NO, T2.HEAT_NO_OLD, T.FACTORY_2, T2.ST_NO, T2.SAP_ERP_PRCSPATH, T2.OUT_STEEL_TIME, T2.IDCARD FROM(\
			select concat(substr(heat_no, 0, 4), lpad(to_char(to_number(substr(heat_no, 5, 4)) + 1), 4, '0')) AS HEAT_NO, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'Z1' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2)\
			WHERE OUT_STEEL_TIME = ' ' OR OUT_STEEL_TIME>to_char(sysdate, 'yyyyMMddHH24miss')\
			UNION ALL\
			SELECT * FROM(SELECT DECODE(T2.HEAT_NO, T2.HEAT_NO_OLD, T2.HEAT_NO_OLD, T2.HEAT_NO) AS HEAT_NO, T2.HEAT_NO_OLD, T.FACTORY_2, T2.ST_NO, T2.SAP_ERP_PRCSPATH, T2.OUT_STEEL_TIME, T2.IDCARD FROM(\
			select concat(substr(heat_no, 0, 4), lpad(to_char(to_number(substr(heat_no, 5, 4)) + 2), 4, '0')) AS HEAT_NO, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'Z1' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2)\
			WHERE OUT_STEEL_TIME = ' ' OR OUT_STEEL_TIME>to_char(sysdate, 'yyyyMMddHH24miss')\
			UNION ALL\
			SELECT * FROM(SELECT DECODE(T2.HEAT_NO, T2.HEAT_NO_OLD, T2.HEAT_NO_OLD, T2.HEAT_NO) AS HEAT_NO, T2.HEAT_NO_OLD, T.FACTORY_2, T2.ST_NO, T2.SAP_ERP_PRCSPATH, T2.OUT_STEEL_TIME, T2.IDCARD FROM(\
			select concat(substr(heat_no, 0, 4), lpad(to_char(to_number(substr(heat_no, 5, 4)) + 3), 4, '0')) AS HEAT_NO, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'Z1' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2)\
			WHERE OUT_STEEL_TIME = ' ' OR OUT_STEEL_TIME>to_char(sysdate, 'yyyyMMddHH24miss')\
			UNION ALL\
			SELECT * FROM(SELECT DECODE(T2.HEAT_NO, T2.HEAT_NO_OLD, T2.HEAT_NO_OLD, T2.HEAT_NO) AS HEAT_NO, T2.HEAT_NO_OLD, T.FACTORY_2, T2.ST_NO, T2.SAP_ERP_PRCSPATH, T2.OUT_STEEL_TIME, T2.IDCARD FROM(\
			select concat(substr(heat_no, 0, 4), lpad(to_char(to_number(substr(heat_no, 5, 4)) + 4), 4, '0')) AS HEAT_NO, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'Z1' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2)\
			WHERE OUT_STEEL_TIME = ' ' OR OUT_STEEL_TIME>to_char(sysdate, 'yyyyMMddHH24miss')";
		Log::Trace("", "", "sql7", sql);
		cmd_inq.SetCommandText(sql);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[3]);
		cmd_inq.Close();
		

		bcls_ret->Tables.Add();
		sql = "SELECT * FROM (SELECT DECODE(T2.HEAT_NO,T2.HEAT_NO_OLD,T2.HEAT_NO_OLD,T2.HEAT_NO) AS HEAT_NO,T2.HEAT_NO_OLD,T.FACTORY_2,T2.ST_NO,T2.SAP_ERP_PRCSPATH,T2.OUT_STEEL_TIME,T2.IDCARD FROM (\
			select heat_no, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'Z2' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2)\
			WHERE OUT_STEEL_TIME = ' ' OR OUT_STEEL_TIME>to_char(sysdate, 'yyyyMMddHH24miss')\
			UNION ALL\
			SELECT * FROM(SELECT DECODE(T2.HEAT_NO, T2.HEAT_NO_OLD, T2.HEAT_NO_OLD, T2.HEAT_NO) AS HEAT_NO, T2.HEAT_NO_OLD, T.FACTORY_2, T2.ST_NO, T2.SAP_ERP_PRCSPATH, T2.OUT_STEEL_TIME, T2.IDCARD FROM(\
			select concat(substr(heat_no, 0, 4), lpad(to_char(to_number(substr(heat_no, 5, 4)) + 1), 4, '0')) AS HEAT_NO, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'Z2' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2)\
			WHERE OUT_STEEL_TIME = ' ' OR OUT_STEEL_TIME>to_char(sysdate, 'yyyyMMddHH24miss')\
			UNION ALL\
			SELECT * FROM(SELECT DECODE(T2.HEAT_NO, T2.HEAT_NO_OLD, T2.HEAT_NO_OLD, T2.HEAT_NO) AS HEAT_NO, T2.HEAT_NO_OLD, T.FACTORY_2, T2.ST_NO, T2.SAP_ERP_PRCSPATH, T2.OUT_STEEL_TIME, T2.IDCARD FROM(\
			select concat(substr(heat_no, 0, 4), lpad(to_char(to_number(substr(heat_no, 5, 4)) + 2), 4, '0')) AS HEAT_NO, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'Z2' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2)\
			WHERE OUT_STEEL_TIME = ' ' OR OUT_STEEL_TIME>to_char(sysdate, 'yyyyMMddHH24miss')\
			UNION ALL\
			SELECT * FROM(SELECT DECODE(T2.HEAT_NO, T2.HEAT_NO_OLD, T2.HEAT_NO_OLD, T2.HEAT_NO) AS HEAT_NO, T2.HEAT_NO_OLD, T.FACTORY_2, T2.ST_NO, T2.SAP_ERP_PRCSPATH, T2.OUT_STEEL_TIME, T2.IDCARD FROM(\
			select concat(substr(heat_no, 0, 4), lpad(to_char(to_number(substr(heat_no, 5, 4)) + 3), 4, '0')) AS HEAT_NO, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'Z2' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2)\
			WHERE OUT_STEEL_TIME = ' ' OR OUT_STEEL_TIME>to_char(sysdate, 'yyyyMMddHH24miss')\
			UNION ALL\
			SELECT * FROM(SELECT DECODE(T2.HEAT_NO, T2.HEAT_NO_OLD, T2.HEAT_NO_OLD, T2.HEAT_NO) AS HEAT_NO, T2.HEAT_NO_OLD, T.FACTORY_2, T2.ST_NO, T2.SAP_ERP_PRCSPATH, T2.OUT_STEEL_TIME, T2.IDCARD FROM(\
			select concat(substr(heat_no, 0, 4), lpad(to_char(to_number(substr(heat_no, 5, 4)) + 4), 4, '0')) AS HEAT_NO, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'Z2' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2)\
			WHERE OUT_STEEL_TIME = ' ' OR OUT_STEEL_TIME>to_char(sysdate, 'yyyyMMddHH24miss')";
		Log::Trace("", "", "sql8", sql);
		cmd_inq.SetCommandText(sql);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[4]);
		cmd_inq.Close();
		

		bcls_ret->Tables.Add();
		sql = "SELECT * FROM (SELECT DECODE(T2.HEAT_NO,T2.HEAT_NO_OLD,T2.HEAT_NO_OLD,T2.HEAT_NO) AS HEAT_NO,T2.HEAT_NO_OLD,T.FACTORY_2,T2.ST_NO,T2.SAP_ERP_PRCSPATH,T2.OUT_STEEL_TIME,T2.IDCARD FROM (\
			select heat_no, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'Z3' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2)\
			WHERE OUT_STEEL_TIME = ' ' OR OUT_STEEL_TIME>to_char(sysdate, 'yyyyMMddHH24miss')\
			UNION ALL\
			SELECT * FROM(SELECT DECODE(T2.HEAT_NO, T2.HEAT_NO_OLD, T2.HEAT_NO_OLD, T2.HEAT_NO) AS HEAT_NO, T2.HEAT_NO_OLD, T.FACTORY_2, T2.ST_NO, T2.SAP_ERP_PRCSPATH, T2.OUT_STEEL_TIME, T2.IDCARD FROM(\
			select concat(substr(heat_no, 0, 4), lpad(to_char(to_number(substr(heat_no, 5, 4)) + 1), 4, '0')) AS HEAT_NO, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'Z3' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2)\
			WHERE OUT_STEEL_TIME = ' ' OR OUT_STEEL_TIME>to_char(sysdate, 'yyyyMMddHH24miss')\
			UNION ALL\
			SELECT * FROM(SELECT DECODE(T2.HEAT_NO, T2.HEAT_NO_OLD, T2.HEAT_NO_OLD, T2.HEAT_NO) AS HEAT_NO, T2.HEAT_NO_OLD, T.FACTORY_2, T2.ST_NO, T2.SAP_ERP_PRCSPATH, T2.OUT_STEEL_TIME, T2.IDCARD FROM(\
			select concat(substr(heat_no, 0, 4), lpad(to_char(to_number(substr(heat_no, 5, 4)) + 2), 4, '0')) AS HEAT_NO, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'Z3' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2)\
			WHERE OUT_STEEL_TIME = ' ' OR OUT_STEEL_TIME>to_char(sysdate, 'yyyyMMddHH24miss')\
			UNION ALL\
			SELECT * FROM(SELECT DECODE(T2.HEAT_NO, T2.HEAT_NO_OLD, T2.HEAT_NO_OLD, T2.HEAT_NO) AS HEAT_NO, T2.HEAT_NO_OLD, T.FACTORY_2, T2.ST_NO, T2.SAP_ERP_PRCSPATH, T2.OUT_STEEL_TIME, T2.IDCARD FROM(\
			select concat(substr(heat_no, 0, 4), lpad(to_char(to_number(substr(heat_no, 5, 4)) + 3), 4, '0')) AS HEAT_NO, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'Z3' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2)\
			WHERE OUT_STEEL_TIME = ' ' OR OUT_STEEL_TIME>to_char(sysdate, 'yyyyMMddHH24miss')\
			UNION ALL\
			SELECT * FROM(SELECT DECODE(T2.HEAT_NO, T2.HEAT_NO_OLD, T2.HEAT_NO_OLD, T2.HEAT_NO) AS HEAT_NO, T2.HEAT_NO_OLD, T.FACTORY_2, T2.ST_NO, T2.SAP_ERP_PRCSPATH, T2.OUT_STEEL_TIME, T2.IDCARD FROM(\
			select concat(substr(heat_no, 0, 4), lpad(to_char(to_number(substr(heat_no, 5, 4)) + 4), 4, '0')) AS HEAT_NO, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'Z3' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2)\
			WHERE OUT_STEEL_TIME = ' ' OR OUT_STEEL_TIME>to_char(sysdate, 'yyyyMMddHH24miss')";
		Log::Trace("", "", "sql9", sql);
		cmd_inq.SetCommandText(sql);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[5]);
		cmd_inq.Close();
		

		bcls_ret->Tables.Add();
		sql = "SELECT * FROM (SELECT DECODE(T2.HEAT_NO,T2.HEAT_NO_OLD,T2.HEAT_NO_OLD,T2.HEAT_NO) AS HEAT_NO,T2.HEAT_NO_OLD,T.FACTORY_2,T2.ST_NO,T2.SAP_ERP_PRCSPATH,T2.OUT_STEEL_TIME,T2.IDCARD FROM (\
			select heat_no, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'Z4' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2)\
			WHERE OUT_STEEL_TIME = ' ' OR OUT_STEEL_TIME>to_char(sysdate, 'yyyyMMddHH24miss')\
			UNION ALL\
			SELECT * FROM(SELECT DECODE(T2.HEAT_NO, T2.HEAT_NO_OLD, T2.HEAT_NO_OLD, T2.HEAT_NO) AS HEAT_NO, T2.HEAT_NO_OLD, T.FACTORY_2, T2.ST_NO, T2.SAP_ERP_PRCSPATH, T2.OUT_STEEL_TIME, T2.IDCARD FROM(\
			select concat(substr(heat_no, 0, 4), lpad(to_char(to_number(substr(heat_no, 5, 4)) + 1), 4, '0')) AS HEAT_NO, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'Z4' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2)\
			WHERE OUT_STEEL_TIME = ' ' OR OUT_STEEL_TIME>to_char(sysdate, 'yyyyMMddHH24miss')\
			UNION ALL\
			SELECT * FROM(SELECT DECODE(T2.HEAT_NO, T2.HEAT_NO_OLD, T2.HEAT_NO_OLD, T2.HEAT_NO) AS HEAT_NO, T2.HEAT_NO_OLD, T.FACTORY_2, T2.ST_NO, T2.SAP_ERP_PRCSPATH, T2.OUT_STEEL_TIME, T2.IDCARD FROM(\
			select concat(substr(heat_no, 0, 4), lpad(to_char(to_number(substr(heat_no, 5, 4)) + 2), 4, '0')) AS HEAT_NO, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'Z4' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2)\
			WHERE OUT_STEEL_TIME = ' ' OR OUT_STEEL_TIME>to_char(sysdate, 'yyyyMMddHH24miss')\
			UNION ALL\
			SELECT * FROM(SELECT DECODE(T2.HEAT_NO, T2.HEAT_NO_OLD, T2.HEAT_NO_OLD, T2.HEAT_NO) AS HEAT_NO, T2.HEAT_NO_OLD, T.FACTORY_2, T2.ST_NO, T2.SAP_ERP_PRCSPATH, T2.OUT_STEEL_TIME, T2.IDCARD FROM(\
			select concat(substr(heat_no, 0, 4), lpad(to_char(to_number(substr(heat_no, 5, 4)) + 3), 4, '0')) AS HEAT_NO, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'Z4' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2)\
			WHERE OUT_STEEL_TIME = ' ' OR OUT_STEEL_TIME>to_char(sysdate, 'yyyyMMddHH24miss')\
			UNION ALL\
			SELECT * FROM(SELECT DECODE(T2.HEAT_NO, T2.HEAT_NO_OLD, T2.HEAT_NO_OLD, T2.HEAT_NO) AS HEAT_NO, T2.HEAT_NO_OLD, T.FACTORY_2, T2.ST_NO, T2.SAP_ERP_PRCSPATH, T2.OUT_STEEL_TIME, T2.IDCARD FROM(\
			select concat(substr(heat_no, 0, 4), lpad(to_char(to_number(substr(heat_no, 5, 4)) + 4), 4, '0')) AS HEAT_NO, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'Z4' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2)\
			WHERE OUT_STEEL_TIME = ' ' OR OUT_STEEL_TIME>to_char(sysdate, 'yyyyMMddHH24miss')";
		Log::Trace("", "", "sql10", sql);
		cmd_inq.SetCommandText(sql);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[6]);
		cmd_inq.Close();
		

		//EAF
		bcls_ret->Tables.Add();
		sql = "SELECT * FROM (SELECT DECODE(T2.HEAT_NO,T2.HEAT_NO_OLD,T2.HEAT_NO_OLD,T2.HEAT_NO) AS HEAT_NO,T2.HEAT_NO_OLD,T.FACTORY_2,T2.ST_NO,T2.SAP_ERP_PRCSPATH,T2.OUT_STEEL_TIME,T2.IDCARD FROM (\
			select heat_no, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'E1' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2)\
			WHERE OUT_STEEL_TIME = ' ' OR OUT_STEEL_TIME>to_char(sysdate, 'yyyyMMddHH24miss')\
			UNION ALL\
			SELECT * FROM(SELECT DECODE(T2.HEAT_NO, T2.HEAT_NO_OLD, T2.HEAT_NO_OLD, T2.HEAT_NO) AS HEAT_NO, T2.HEAT_NO_OLD, T.FACTORY_2, T2.ST_NO, T2.SAP_ERP_PRCSPATH, T2.OUT_STEEL_TIME, T2.IDCARD FROM(\
			select concat(substr(heat_no, 0, 4), lpad(to_char(to_number(substr(heat_no, 5, 4)) + 1), 4, '0')) AS HEAT_NO, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'E1' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2)\
			WHERE OUT_STEEL_TIME = ' ' OR OUT_STEEL_TIME>to_char(sysdate, 'yyyyMMddHH24miss')\
			UNION ALL\
			SELECT * FROM(SELECT DECODE(T2.HEAT_NO, T2.HEAT_NO_OLD, T2.HEAT_NO_OLD, T2.HEAT_NO) AS HEAT_NO, T2.HEAT_NO_OLD, T.FACTORY_2, T2.ST_NO, T2.SAP_ERP_PRCSPATH, T2.OUT_STEEL_TIME, T2.IDCARD FROM(\
			select concat(substr(heat_no, 0, 4), lpad(to_char(to_number(substr(heat_no, 5, 4)) + 2), 4, '0')) AS HEAT_NO, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'E1' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2)\
			WHERE OUT_STEEL_TIME = ' ' OR OUT_STEEL_TIME>to_char(sysdate, 'yyyyMMddHH24miss')\
			UNION ALL\
			SELECT * FROM(SELECT DECODE(T2.HEAT_NO, T2.HEAT_NO_OLD, T2.HEAT_NO_OLD, T2.HEAT_NO) AS HEAT_NO, T2.HEAT_NO_OLD, T.FACTORY_2, T2.ST_NO, T2.SAP_ERP_PRCSPATH, T2.OUT_STEEL_TIME, T2.IDCARD FROM(\
			select concat(substr(heat_no, 0, 4), lpad(to_char(to_number(substr(heat_no, 5, 4)) + 3), 4, '0')) AS HEAT_NO, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'E1' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2)\
			WHERE OUT_STEEL_TIME = ' ' OR OUT_STEEL_TIME>to_char(sysdate, 'yyyyMMddHH24miss')\
			UNION ALL\
			SELECT * FROM(SELECT DECODE(T2.HEAT_NO, T2.HEAT_NO_OLD, T2.HEAT_NO_OLD, T2.HEAT_NO) AS HEAT_NO, T2.HEAT_NO_OLD, T.FACTORY_2, T2.ST_NO, T2.SAP_ERP_PRCSPATH, T2.OUT_STEEL_TIME, T2.IDCARD FROM(\
			select concat(substr(heat_no, 0, 4), lpad(to_char(to_number(substr(heat_no, 5, 4)) + 4), 4, '0')) AS HEAT_NO, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'E1' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2)\
			WHERE OUT_STEEL_TIME = ' ' OR OUT_STEEL_TIME>to_char(sysdate, 'yyyyMMddHH24miss')";
		Log::Trace("", "", "sql12", sql);
		cmd_inq.SetCommandText(sql);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[7]);
		cmd_inq.Close();
		

		bcls_ret->Tables.Add();
		sql = "SELECT * FROM (SELECT DECODE(T2.HEAT_NO,T2.HEAT_NO_OLD,T2.HEAT_NO_OLD,T2.HEAT_NO) AS HEAT_NO,T2.HEAT_NO_OLD,T.FACTORY_2,T2.ST_NO,T2.SAP_ERP_PRCSPATH,T2.OUT_STEEL_TIME,T2.IDCARD FROM (\
			select heat_no, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'E2' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2)\
			WHERE OUT_STEEL_TIME = ' ' OR OUT_STEEL_TIME>to_char(sysdate, 'yyyyMMddHH24miss')\
			UNION ALL\
			SELECT * FROM(SELECT DECODE(T2.HEAT_NO, T2.HEAT_NO_OLD, T2.HEAT_NO_OLD, T2.HEAT_NO) AS HEAT_NO, T2.HEAT_NO_OLD, T.FACTORY_2, T2.ST_NO, T2.SAP_ERP_PRCSPATH, T2.OUT_STEEL_TIME, T2.IDCARD FROM(\
			select concat(substr(heat_no, 0, 4), lpad(to_char(to_number(substr(heat_no, 5, 4)) + 1), 4, '0')) AS HEAT_NO, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'E2' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2)\
			WHERE OUT_STEEL_TIME = ' ' OR OUT_STEEL_TIME>to_char(sysdate, 'yyyyMMddHH24miss')\
			UNION ALL\
			SELECT * FROM(SELECT DECODE(T2.HEAT_NO, T2.HEAT_NO_OLD, T2.HEAT_NO_OLD, T2.HEAT_NO) AS HEAT_NO, T2.HEAT_NO_OLD, T.FACTORY_2, T2.ST_NO, T2.SAP_ERP_PRCSPATH, T2.OUT_STEEL_TIME, T2.IDCARD FROM(\
			select concat(substr(heat_no, 0, 4), lpad(to_char(to_number(substr(heat_no, 5, 4)) + 2), 4, '0')) AS HEAT_NO, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'E2' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2)\
			WHERE OUT_STEEL_TIME = ' ' OR OUT_STEEL_TIME>to_char(sysdate, 'yyyyMMddHH24miss')\
			UNION ALL\
			SELECT * FROM(SELECT DECODE(T2.HEAT_NO, T2.HEAT_NO_OLD, T2.HEAT_NO_OLD, T2.HEAT_NO) AS HEAT_NO, T2.HEAT_NO_OLD, T.FACTORY_2, T2.ST_NO, T2.SAP_ERP_PRCSPATH, T2.OUT_STEEL_TIME, T2.IDCARD FROM(\
			select concat(substr(heat_no, 0, 4), lpad(to_char(to_number(substr(heat_no, 5, 4)) + 3), 4, '0')) AS HEAT_NO, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'E2' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2)\
			WHERE OUT_STEEL_TIME = ' ' OR OUT_STEEL_TIME>to_char(sysdate, 'yyyyMMddHH24miss')\
			UNION ALL\
			SELECT * FROM(SELECT DECODE(T2.HEAT_NO, T2.HEAT_NO_OLD, T2.HEAT_NO_OLD, T2.HEAT_NO) AS HEAT_NO, T2.HEAT_NO_OLD, T.FACTORY_2, T2.ST_NO, T2.SAP_ERP_PRCSPATH, T2.OUT_STEEL_TIME, T2.IDCARD FROM(\
			select concat(substr(heat_no, 0, 4), lpad(to_char(to_number(substr(heat_no, 5, 4)) + 4), 4, '0')) AS HEAT_NO, DEV_CODE AS FACTORY_2 from(\
			select t1.*, row_number() over(order by t1.event_time desc) as rn from TPSSMSS t1 where t1.dev_code = 'E2' and t1.event_id = '3')\
		where rn = '1'\
			)T LEFT JOIN TWMSMYRY T2\
			ON T.HEAT_NO = T2.HEAT_NO_OLD AND T.FACTORY_2 = T2.FACTORY_2)\
			WHERE OUT_STEEL_TIME = ' ' OR OUT_STEEL_TIME>to_char(sysdate, 'yyyyMMddHH24miss')";
		Log::Trace("", "", "sql13", sql);
		cmd_inq.SetCommandText(sql);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[8]);
		cmd_inq.Close();
		
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
	return doFlag;

}