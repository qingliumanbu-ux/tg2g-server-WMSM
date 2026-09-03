/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2013
Author:     ljnie
Version:    1.0
Date:       2016/7/7 16:28:52
Description: 库存信息查询
**************************************************/
//框架头文件
#include "stdafx.h" 
//#include "twma1.h"

/*<remark>=========================================================
/// <summary>
/// 库存信息查询
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件

//外部函数声明
int f_epes_get_auth_other(const char *iuser, int irestype, EIClass *bcls_ret, CDbConnection * conn);

BM2F_ENTERACE(wmsmsma2_inq)

int f_wmsmsma2_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;

	/* 业务变量 */
	CString	datetime("");
	CDecimal totalCount = 0;
	int	record_count_per_page = 0; /* 每页记录数 */
	int	current_page_no = 0; /* 需查询的页号,从0开始计数 */
	int	start_row = 0; /* 将要压入outBlock的起始行 */

	CDecimal d_thick_fr = 0;
	CDecimal d_thick_to = 0;
	CString v_time_fr = "";
	CString v_time_to = "";
	CString roll_plan_no = "";
	CString ingot_code = "";
	CString flag = "";
	CString raw_origin = "";
	CString mat_line_type = "";
	CString mat_kind = "";

	/* 实体类定义 */
	//CTWMA1 twma1(conn);
	CModel twma1 = CModel("TMMSM01");

	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CString sqlstr_count;
	CString s_userid("");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	try
	{
		// 获取前台传入参数
		s_userid = s.userid;
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
        


		/* 获取输入参数 */
		twma1.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		twma1.TrimOrBlank();

		CString stock_no = twma1["STOCK_NO"].ToString().Trim();
		CString mat_no = twma1["MAT_NO"].ToString().Trim();			//材料号
		CString order_no = twma1["ORDER_NO"].ToString().Trim();		//合同号
		CString heat_no = twma1["HEAT_NO"].ToString().Trim();			//炉号
		CString pono = twma1["PONO"].ToString().Trim();			//制造命令号
		CString sg_sign = twma1["SG_SIGN"].ToString().Trim();
		CString st_no = twma1["ST_NO"].ToString().Trim();	//内部钢种
		raw_origin = twma1["RAW_ORIGIN"].ToString().Trim();	//原料来源

		if (bcls_rec->Tables[0].Columns.Contains("MAT_THICK_FR"))
			d_thick_fr = bcls_rec->Tables[0].Rows[0]["MAT_THICK_FR"].ToDecimal();
		if (bcls_rec->Tables[0].Columns.Contains("MAT_THICK_TO"))
			d_thick_to = bcls_rec->Tables[0].Rows[0]["MAT_THICK_TO"].ToDecimal();

		if (bcls_rec->Tables[0].Columns.Contains("TIME_FR"))
			v_time_fr = bcls_rec->Tables[0].Rows[0]["TIME_FR"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("TIME_TO"))
			v_time_to = bcls_rec->Tables[0].Rows[0]["TIME_TO"].ToString();


		if (bcls_rec->Tables[0].Columns.Contains("ROLL_PLAN_NO") == true)
		{
			roll_plan_no = bcls_rec->Tables[0].Rows[0]["ROLL_PLAN_NO"].ToString().Trim();	//轧制计划号
		}
		if (bcls_rec->Tables[0].Columns.Contains("INGOT_CODE") == true)
		{
			ingot_code = bcls_rec->Tables[0].Rows[0]["INGOT_CODE"].ToString().Trim();	//锭坯型
		}
		if (bcls_rec->Tables[0].Columns.Contains("FLAG") == true)
		{
			flag = bcls_rec->Tables[0].Rows[0]["FLAG"].ToString().Trim();
			Log::Trace("", __FUNCTION__, "flag					= [{0}]", flag);
		}

		record_count_per_page = bcls_rec->Tables[0].Rows[0]["RECORD_COUNT_PER_PAGE"]; //每页记录数
		current_page_no = bcls_rec->Tables[0].Rows[0]["CURRENT_PAGE_NO"];       //需查询的页号


		if (bcls_rec->Tables[0].Columns.Contains("MAT_LINE_TYPE"))
			mat_line_type = bcls_rec->Tables[0].Rows[0]["MAT_LINE_TYPE"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("MAT_KIND"))
			mat_kind = bcls_rec->Tables[0].Rows[0]["MAT_KIND"].ToString();


		Log::Trace("", __FUNCTION__, "传入参数 in_stock_time_from					= [{0}]", v_time_fr);
		Log::Trace("", __FUNCTION__, "传入参数 in_stock_time_to					= [{0}]", v_time_to);
		Log::Trace("", __FUNCTION__, "传入参数 mat_act_thick_from				= [{0}]", d_thick_fr);
		Log::Trace("", __FUNCTION__, "传入参数 mat_act_thick_to			= [{0}]", d_thick_to);
		Log::Trace("", __FUNCTION__, "传入参数 sg_sign			= [{0}]", sg_sign);
		Log::Trace("", __FUNCTION__, "传入参数 stock_no			= [{0}]", stock_no);
		Log::Trace("", __FUNCTION__, "传入参数 mat_no			= [{0}]", mat_no);
		Log::Trace("", __FUNCTION__, "传入参数 order_no			= [{0}]", order_no);
		Log::Trace("", __FUNCTION__, "传入参数 heat_no			= [{0}]", heat_no);
		Log::Trace("", __FUNCTION__, "传入参数 pono			= [{0}]", pono);
		Log::Trace("", __FUNCTION__, "传入参数 st_no			= [{0}]", st_no);
		Log::Trace("", __FUNCTION__, "传入参数 roll_plan_no			= [{0}]", roll_plan_no);
		Log::Trace("", __FUNCTION__, "传入参数 ingot_code			= [{0}]", ingot_code);
		Log::Trace("", __FUNCTION__, "传入参数 raw_origin			= [{0}]", raw_origin);

		Log::Trace("", __FUNCTION__, "传入参数 record_count_per_page	= [{0}]", record_count_per_page);
		Log::Trace("", __FUNCTION__, "传入参数 current_page_no		= [{0}]", current_page_no);
		Log::Trace("", __FUNCTION__, "传入参数 mat_kind		= [{0}]", mat_kind);
		Log::Trace("", __FUNCTION__, "传入参数 mat_line_type		= [{0}]", mat_line_type);




		/* 检查输入参数合法性 */


		if (d_thick_fr > 9999)
		{
			d_thick_fr = 9999;
		}
		if (d_thick_to == 0 ||
			d_thick_to > 9999)
		{
			d_thick_to = 9999;
		}


		sqlstr = " SELECT * FROM TMMSM01 t1, TWMA2 T2 WHERE T1.MAT_NO = T2.MAT_NO ";

		if (stock_no != "")
		{
			sqlstr += " AND t1.STOCK_NO = @stock_no ";
		}
		if (mat_no != "")
		{
			sqlstr += " AND t1.MAT_NO IN ('";
			sqlstr += mat_no;
			sqlstr += "' )";
		}
		if (order_no != "")
		{
			sqlstr += " AND t1.ORDER_NO = @order_no ";
		}
		if (heat_no != "")
		{
			sqlstr += " AND t1.HEAT_NO = @heat_no ";
		}
		if (pono != "")
		{
			sqlstr += " AND t1.PONO = @pono ";
		}
		if (sg_sign != "")
		{
			sqlstr += " AND t1.SG_SIGN = @sg_sign ";
		}
		if (st_no != "")
		{
			sqlstr += " AND t1.ST_NO = @st_no ";
		}
		if (raw_origin != "")
		{
			sqlstr += " AND t1.RAW_ORIGIN = @raw_origin ";
		}
		if (d_thick_fr.ToString().Trim() != "" && d_thick_fr.ToString().Trim() != "0")
		{
			sqlstr += " AND t1.MAT_ACT_THICK >= @mat_act_thick_from ";
		}
		if (d_thick_to != 0)
		{
			sqlstr += " AND t1.MAT_ACT_THICK <= @mat_act_thick_to ";
		}
		if (v_time_fr != "")
		{
			v_time_fr += "000000";
			sqlstr += " AND t1.IN_STOCK_TIME >= @in_stock_time_from ";
		}
		if (v_time_to != "")
		{
			v_time_to += "235959";
			sqlstr += " AND t1.IN_STOCK_TIME <= @in_stock_time_to ";
		}
		if (roll_plan_no != "")
		{
			sqlstr += " AND t1.ROLL_PLAN_NO = @roll_plan_no ";
		}
		if (ingot_code != "")
		{
			sqlstr += " AND t1.INGOT_CODE = @ingot_code ";
		}

		if (flag == "0")
		{
			sqlstr += " AND t1.MAT_DESTION != '20' ";
		}
		else if (flag == "1")
		{
			sqlstr += " AND t1.MAT_DESTION = '20' ";
		}

		if (mat_line_type.Trim() != "")
		{
			sqlstr +=
				" AND EXISTS(SELECT NULL FROM TWM01"
				" WHERE TWM01.STOCK_NO = T1.STOCK_NO"
				" AND TWM01.MAT_LINE_TYPE = @mat_line_type)";
		}
		if (mat_kind.Trim() != "")
		{
			sqlstr +=
				" AND EXISTS(SELECT NULL FROM TWM01"
				" WHERE TWM01.STOCK_NO = T1.STOCK_NO"
				" AND TWM01.MAT_KIND = @mat_kind)";
		}

		//sqlstr +=
		//	" AND T2.STOCK_NO IN"
		//	"( select STOCK_NO"
		//	" from   twm0a "
		//	" where   groupid in ( "
		//	" select groupid from TESGROUPMEMBER "
		//	"  where memberid in ( "
		//	" select id from tesuserinfo where ename = @userid "
		//	" ) "
		//	"  ) "
		//	"  ) ";

		/*sqlstr +=
			" AND T2.STOCK_NO IN (" + stock_no_auth + ")";*/


		cmd_inq.Parameters.Set("userid", s_userid);
		cmd_inq.Parameters.Set("stock_no", stock_no);
		cmd_inq.Parameters.Set("mat_no", mat_no);
		cmd_inq.Parameters.Set("order_no", order_no);
		cmd_inq.Parameters.Set("heat_no", heat_no);
		cmd_inq.Parameters.Set("pono", pono);
		cmd_inq.Parameters.Set("sg_sign", sg_sign);
		cmd_inq.Parameters.Set("st_no", st_no);
		cmd_inq.Parameters.Set("raw_origin", raw_origin);
		cmd_inq.Parameters.Set("mat_act_thick_from", d_thick_fr);
		cmd_inq.Parameters.Set("mat_act_thick_to", d_thick_to);
		cmd_inq.Parameters.Set("in_stock_time_from", v_time_fr);
		cmd_inq.Parameters.Set("in_stock_time_to", v_time_to);
		cmd_inq.Parameters.Set("roll_plan_no", roll_plan_no);
		cmd_inq.Parameters.Set("ingot_code", ingot_code);
		cmd_inq.Parameters.Set("mat_line_type", mat_line_type);
		cmd_inq.Parameters.Set("mat_kind", mat_kind);

		sqlstr_count = "SELECT COUNT(1) FROM (" + sqlstr + ") T ";
		Log::Trace("", __FUNCTION__, "sqlstr_count = [{0}]", sqlstr_count);
		cmd_inq.SetCommandText(sqlstr_count);
		totalCount = cmd_inq.ExecuteScalar();
		Log::Trace("", __FUNCTION__, "totalCount = [{0}]", totalCount);
		cmd_inq.Close();

		start_row = record_count_per_page * (current_page_no - 1);
		if (start_row > totalCount.ToDouble())
		{
			start_row = 0;
		}
		sqlstr += " ORDER BY T1.MAT_NO ASC ";
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], start_row, record_count_per_page);
		cmd_inq.Close();

		for (int j = 1; j <= 4; j++)
		{
			if (!bcls_ret->Tables[0].Columns.Contains("DEFECT_CODE_" + CConvert::ToString(j)))
			{
				bcls_ret->Tables[0].Columns.Add(DT_STRING, "DEFECT_CODE_" + CConvert::ToString(j));
			}
			if (!bcls_ret->Tables[0].Columns.Contains("DEFECT_DEGREE_F_" + CConvert::ToString(j)))
			{
				bcls_ret->Tables[0].Columns.Add(DT_STRING, "DEFECT_DEGREE_F_" + CConvert::ToString(j));
			}
			if (!bcls_ret->Tables[0].Columns.Contains("DEFECT_POSITION_F_" + CConvert::ToString(j)))
			{
				bcls_ret->Tables[0].Columns.Add(DT_STRING, "DEFECT_POSITION_F_" + CConvert::ToString(j));
			}
		}
		

		for (int i = 0; i < bcls_ret->Tables[0].Rows.get_Count(); i++)
		{
			sqlstr =
				" select "
				" listagg(DEFECT_CODE1),"
				" listagg(DEFECT_CODE2),"
				" listagg(DEFECT_CODE3),"
				" listagg(DEFECT_CODE4),"
				" listagg(DEFECT_SURF1),"
				" listagg(DEFECT_SURF2),"
				" listagg(DEFECT_SURF3),"
				" listagg(DEFECT_SURF4),"
				" sum(DEFECT_START1),"
				" sum(DEFECT_START2),"
				" sum(DEFECT_START3),"
				" sum(DEFECT_START4) "
				" from( "
				" SELECT "
				" case when substr(defect_id,17,1) ='1' then DEFECT_CODE else ' ' end DEFECT_CODE1,"
				" case when substr(defect_id, 17, 1) = '2' then DEFECT_CODE else ' ' end DEFECT_CODE2,"
				" case when substr(defect_id, 17, 1) = '3' then DEFECT_CODE else ' ' end DEFECT_CODE3,"
				" case when substr(defect_id, 17, 1) = '4' then DEFECT_CODE else ' ' end DEFECT_CODE4,"
				" case when substr(defect_id, 17, 1) = '1' then DEFECT_SURF else ' ' end DEFECT_SURF1,"
				" case when substr(defect_id, 17, 1) = '2' then DEFECT_SURF else ' ' end DEFECT_SURF2,"
				" case when substr(defect_id, 17, 1) = '3' then DEFECT_SURF else ' ' end DEFECT_SURF3,"
				" case when substr(defect_id, 17, 1) = '4' then DEFECT_SURF else ' ' end DEFECT_SURF4,"
				" case when substr(defect_id, 17, 1) = '1' then DEFECT_START else 0 end DEFECT_START1,"
				" case when substr(defect_id, 17, 1) = '2' then DEFECT_START else 0 end DEFECT_START2,"
				" case when substr(defect_id, 17, 1) = '3' then DEFECT_START else 0 end DEFECT_START3,"
				" case when substr(defect_id, 17, 1) = '4' then DEFECT_START else 0 end DEFECT_START4  "
				" FROM tmmsm06 WHERE mat_no = @mat_no)  "
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("mat_no", bcls_ret->Tables[0].Rows[i]["MAT_NO"].ToString());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				
				bcls_ret->Tables[0].Rows[i]["DEFECT_CODE_1"] = cmd_inq.GetString(1);
				bcls_ret->Tables[0].Rows[i]["DEFECT_CODE_2"] = cmd_inq.GetString(2);
				bcls_ret->Tables[0].Rows[i]["DEFECT_CODE_3"] = cmd_inq.GetString(3);
				bcls_ret->Tables[0].Rows[i]["DEFECT_CODE_4"] = cmd_inq.GetString(4);
				bcls_ret->Tables[0].Rows[i]["DEFECT_DEGREE_F_1"] = cmd_inq.GetString(5);
				bcls_ret->Tables[0].Rows[i]["DEFECT_DEGREE_F_2"] = cmd_inq.GetString(6);
				bcls_ret->Tables[0].Rows[i]["DEFECT_DEGREE_F_3"] = cmd_inq.GetString(7);
				bcls_ret->Tables[0].Rows[i]["DEFECT_DEGREE_F_4"] = cmd_inq.GetString(8);
				bcls_ret->Tables[0].Rows[i]["DEFECT_POSITION_F_1"] = cmd_inq.GetDecimal(9);
				bcls_ret->Tables[0].Rows[i]["DEFECT_POSITION_F_2"] = cmd_inq.GetDecimal(10);
				bcls_ret->Tables[0].Rows[i]["DEFECT_POSITION_F_3"] = cmd_inq.GetDecimal(11);
				bcls_ret->Tables[0].Rows[i]["DEFECT_POSITION_F_4"] = cmd_inq.GetDecimal(12);

			}
			cmd_inq.Close();
			

		}
		//返回分页信息 
		bcls_ret->Tables.Add("PAGEINFO");	//增加块
		bcls_ret->Tables["PAGEINFO"].Columns.Add(DT_DECIMAL, "TOTAL_RECORD");						//总记录数
		bcls_ret->Tables["PAGEINFO"].Rows.Add();
		bcls_ret->Tables["PAGEINFO"].Rows[0][0] = totalCount.ToInt32();
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
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;

}
