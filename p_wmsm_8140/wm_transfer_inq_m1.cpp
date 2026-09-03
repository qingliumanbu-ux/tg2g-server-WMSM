/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   吴新
Version:
Date:     2016-04-13
Description: 查询转库计划材料明细
**************************************************/

/*<remark>=========================================================
/// <summary>
/// 查询转库计划材料明细
/// <para>
/// 根据传入的转库计划号查询转库计划材料明细。
/// </para>
/// <para>数据库表：TWM42(转库计划明细表)         </para>
/// </summary>
/// <param name="TRANSFER_PLAN_NO">转库计划号    </param>
/// <returns>转库计划材料明细</returns>
===========================================================</remark>*/

#include "stdafx.h" //框架头

//函数申明
int f_epes_get_auth_other(const char *iuser, int irestype, EIClass *bcls_ret, CDbConnection * conn);

// service入口
BM2F_ENTERACE(wm_transfer_inq_m1)

int f_wm_transfer_inq_m1(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;

	/* 业务变量 */
	CString  transfer_plan_no = "";
	CString  mat_no = "";
	CString  mat_kind = "";
	CString  stock_no = "";
	CString  stock_place_no = "";
	CString  layerno = "";
	CString  s_userid = "";
	CDecimal mat_act_wt = 0;

	CDecimal rowCount = 0;
	CDecimal rowSum = 0;
	int fetchRowCount = 0;

	int NOW_NUM = 0;
	int RETURN_NUM = -1;////每页记录数量
	int INDEX_FROM = 0;//页数

	/* 实体类定义 */
	CString sqlstr("");
	CString sqlcount("");
	CString  sql("");              // 数据库SQL操作字符串
	CString sqlwhere("");
	CString sqlorderby("");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		// 获取前台传入参数
		s_userid = s.userid;
		//获取库区授权
		CString stock_no_auth = "' '";
		//EIClass *bcls_auth = new EIClass;
		//if (f_epes_get_auth_other(s.userid, 5, bcls_auth, conn) != 0)
		//{
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}
		//for (int fetchRowCount = 0; fetchRowCount < bcls_auth->Tables[0].Rows.get_Count(); fetchRowCount++)
		//{
		//	stock_no_auth += ", '" + bcls_auth->Tables[0].Rows[fetchRowCount]["name"].ToString() + "' ";
		//}
		//delete bcls_auth;

		CDataTable& table = bcls_ret->Tables.Add("PAGEINFO");
		table.Columns.Add(DT_DECIMAL, "TOTAL_RECORD");
		table.Columns.Add(DT_DECIMAL, "WEIGHT");

		/* ***** 获取输入参数 ***** */
		transfer_plan_no = bcls_rec->Tables[0].Rows[0]["TRANSFER_PLAN_NO"];
		mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"];

		//分页
		if (bcls_rec->Tables[0].Columns.Contains("INDEX_FROM"))
		{
			INDEX_FROM = bcls_rec->Tables[0].Rows[0]["INDEX_FROM"];

		}
		if (bcls_rec->Tables[0].Columns.Contains("RETURN_NUM"))
		{
			RETURN_NUM = bcls_rec->Tables[0].Rows[0]["RETURN_NUM"];
		}

		Log::Trace("", __FUNCTION__, "INDEX_FROM[{0}];RETURN_NUM[{1}]", INDEX_FROM, RETURN_NUM);

		/* ***** 打印输入参数 ***** */
		Log::Trace("", __FUNCTION__, "传入参数 TRANSFER_PLAN_NO	= [{0}]", transfer_plan_no);
		Log::Trace("", __FUNCTION__, "传入参数 MAT_NO	= [{0}]", mat_no);

		if (mat_no.Trim() != "")
		{
			sqlwhere += " AND A.MAT_NO LIKE '%'||@mat_no||'%' ";
		}
		if (transfer_plan_no.Trim() != "")
		{
			sqlwhere += " AND A.TRANSFER_PLAN_NO = @transfer_plan_no ";
		}
		sqlwhere +=
			" AND A.STOCK_NO IN (" + stock_no_auth + ") ";

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default: // 所有数据库适用，通用SQL语句
			sql = CString(
				" SELECT A.*, B.STOCK_PLACE_NO, B.LAYERNO "
				" FROM TWM42 A LEFT JOIN TWMA2 B on A.MAT_NO = B.MAT_NO "
				" WHERE 1 = 1 "
				" AND NOT EXISTS(SELECT NULL FROM TWMA0 C WHERE A.MAT_NO = C.MAT_NO AND C.STOCK_OPER_ORDER = '2G') AND transfer_plan_no <> '' AND AFFIRM_MARK = '2' "
				);
			break;
		}
		sqlorderby = " ORDER BY A.MAT_NO ";
		sqlstr = sql + sqlwhere + sqlorderby;
		Log::Debug("", __FUNCTION__, "sqlstr= [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("transfer_plan_no", transfer_plan_no);
		cmd_inq.Parameters.Set("mat_no", mat_no);


		NOW_NUM = INDEX_FROM*RETURN_NUM;//当前个数                         
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], NOW_NUM, RETURN_NUM);
		cmd_inq.Close();

		sqlcount = "SELECT COUNT(1),SUM(BILL_MAT_WT) FROM (" + sqlstr + ") T";
		Log::Debug("", __FUNCTION__, "sqlstr= [{0}]", sqlcount);
		cmd_inq.SetCommandText(sqlcount);
		cmd_inq.Parameters.Set("transfer_plan_no", transfer_plan_no);
		cmd_inq.Parameters.Set("mat_no", mat_no);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read()){
			rowCount = cmd_inq.GetDecimal(1);
			rowSum = cmd_inq.GetDecimal(2);
		}
		cmd_inq.Close();

		CDataRow& row1 = bcls_ret->Tables["PAGEINFO"].Rows.Add();
		row1["TOTAL_RECORD"] = rowCount.ToInt32();
		row1["WEIGHT"] = rowSum.ToInt32();

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。" /* _RES("GCRSS0000006")*//*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sql + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		////EDLog(1,1, "[%s]", s.sysmsg);
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

