/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2013
Author:      ljnie
Version:     3.1.1
Date:        2016/7/6 16:08:19
Description: 库位材料信息查询WM17
**************************************************/
/***** C/C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** 头文件部分 *****/
//#include "twm04.h"
//#include "twma1.h"

/*<remark>=========================================================
/// <summary>
///  库位材料信息查询
/// <para>根据查询条件查询库位材料信息
/// </para>
/// <para>数据库表：</para>
/// </summary>
/// <param name="STOCK_PLACE_NO">库位号  </param>
/// <returns>返回参数：材料数据</returns>
===========================================================</remark>*/
// Service 入口
BM2F_ENTERACE(wmsmsm17_inq_mat1)

//函数申明
int f_epes_get_auth_other(const char *iuser, int irestype, EIClass *bcls_ret, CDbConnection * conn);

int f_wmsmsm17_inq_mat1(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;

	/* 业务变量 */
	CString mat_no("");
	CDecimal cmd_flag = 0;
	CString s_userid("");

	int NOW_NUM = 0;
	int RETURN_NUM = -1;////每页记录数量
	int INDEX_FROM = 0;//页数

	/* 数据库SQL操作字符串 */
	CString sqlstr("");
	CString sql("");
	CString sqlwhere("");
	CString sqlorderby("");
	//CString sqlstr1 ("");

	/* 实体类定义 */
	//CTWM04 twm04(conn);
	//CTWMA1 twma1(conn);
	CModel twm04 = CModel("TWM04");
	CModel twma1 = CModel("TMMSM01");
	//CModel twm34 = CModel("TMMSM34");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);

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
		//sqlstr1 = "SELECT REC_CREATE_TIME FROM TMMSM34 WHERE MAT_NO"
		// 获取前台传入参数
		mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim();
		twm04["STOCK_PLACE_NO"] = bcls_rec->Tables[0].Rows[0]["STOCK_PLACE_NO"].ToString().Trim();
		//twm04["ROWNO"] = bcls_rec->Tables[0].Rows[0]["ROWNO"].ToString().Trim();
		//twm04["COLUMN_NO"] = bcls_rec->Tables[0].Rows[0]["COLUMN_NO"].ToString().Trim();
		//twma1["MAT_LINE_TYPE"] = bcls_rec->Tables[0].Rows[0]["MAT_LINE_TYPE"].ToString().Trim();
		//twm04["HALL_NO"] = bcls_rec->Tables[0].Rows[0]["HALL_NO"].ToString().Trim();
		twm04["STOCK_NO"] = bcls_rec->Tables[0].Rows[0]["STOCK_NO"].ToString().Trim();
		//twma1["HSF_END_TIME"] = twm34["REC_CREATE_TIME"];

		/* ***** 打印输入参数 ***** */
		Log::Trace("", __FUNCTION__, "传入参数STOCK_PLACE_NO = [{0}]", twm04["STOCK_PLACE_NO"].ToString());
		Log::Trace("", __FUNCTION__, "传入参数ROWNO = [{0}]", twm04["ROWNO"].ToString());
		Log::Trace("", __FUNCTION__, "传入参数COLUMN_NO = [{0}]", twm04["COLUMN_NO"].ToString());
		Log::Trace("", __FUNCTION__, "传入参数MAT_LINE_TYPE = [{0}]", twma1["MAT_LINE_TYPE"].ToString());
		//Log::Trace("", __FUNCTION__, "传入参数HALL_NO = [{0}]", twm04["HALL_NO"].ToString());
		Log::Trace("", __FUNCTION__, "传入参数STOCK_NO = [{0}]", twm04["STOCK_NO"].ToString());
		stock_no_auth = twm04["STOCK_NO"].ToString();
		//		if (!twm04.Query())
		//		{
		//			//CFormattable arguments[] = { twm04.STOCK_PLACE_NO }; // 定义参数列表的数组
		//			//CMessageFormat::Format(s.msg, _RES("YM00S0000551")/*库位[{0}]不存在。*/, arguments, 1);
		//			sprintf(s.msg, "库位[%s]不存在。", (const char*)twm04.STOCK_PLACE_NO);
		//			throw CApplicationException(-1, s.sysmsg, log.Location);
		//		}
		Log::Trace("", __FUNCTION__, "11111111");

		if (twm04["ROWNO"].ToString().Trim() != ""){
			sqlwhere = " AND ROWNO = @rowno";
		}
		if (twm04["STOCK_NO"].ToString().Trim() != ""){
			sqlwhere = " AND A.STOCK_NO = @stock_no ";
		}
		if (twm04["COLUMN_NO"].ToString().Trim() != ""){
			sqlwhere = " AND COLUMN_NO = @column_no";
		}
		if (mat_no != ""){
			sqlwhere = " AND B.MAT_NO LIKE '%'||@mat_no||'%'";
		}
		if (twm04["STOCK_PLACE_NO"].ToString().Trim() != ""){
			sqlwhere = " AND B.STOCK_PLACE_NO LIKE '%'||@stock_place_no||'%'";
		}
		sqlwhere +=
			" AND A.STOCK_NO IN ('" + stock_no_auth + "')";

		switch (conn->DatabaseKind)
		{
		case DB_KIND_MSSQL:			// MS SQL Server数据库
			sql =
				" SELECT (SELECT CODE_DESC_1_CONTENT FROM TEP0002 WHERE CODE_CLASS = 'M005' AND CODE = A.MAT_STATUS) MAT_STATUS ,* FROM TMMSM01 A LEFT JOIN TWMA2 B ON A.MAT_NO=B.MAT_NO "
				//" WHERE B.STOCK_PLACE_NO = @stock_place_no";
				//" AND B.STOCK_NO = @stock_no ";
				"WHERE 1 = 1 ";
			sqlorderby = " ORDER BY	CAST(ISNULL(ltrim(rtrim(b.LAYERNO)), '0') AS INT) DESC, a.REC_CREATE_TIME ASC ";
			break;
		case DB_KIND_DB2:			// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
		case DB_KIND_ORACLE:		// Oracle 数据库
			sql =
				" SELECT (SELECT CODE_DESC_1_CONTENT FROM TEP0002 WHERE CODE_CLASS = 'M005' AND CODE = A.MAT_STATUS) MAT_STATUS ,*"
				" FROM TMMSM01 A, TWMA2 B"
				" WHERE A.MAT_NO = B.MAT_NO"
				//" AND B.STOCK_PLACE_NO = @stock_place_no"
				//" AND B.STOCK_NO = @stock_no"
				;

			sqlorderby = " ORDER BY B.LAYERNO DESC";
			break;
		}
		Log::Trace("", __FUNCTION__, "22222222");
		sqlstr = sql + sqlwhere + sqlorderby;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stock_place_no", twm04["STOCK_PLACE_NO"].ToString());
		//cmd_inq.Parameters.Set("mat_line_type", twma1.MAT_LINE_TYPE);
		cmd_inq.Parameters.Set("rowno", twm04["ROWNO"].ToString());
		cmd_inq.Parameters.Set("column_no", twm04["COLUMN_NO"].ToString());
		cmd_inq.Parameters.Set("mat_no", mat_no);
		//cmd_inq.Parameters.Set("hall_no", twm04["HALL_NO"].ToString());
		cmd_inq.Parameters.Set("stock_no", twm04["STOCK_NO"].ToString());

		NOW_NUM = INDEX_FROM*RETURN_NUM;//当前个数

		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], NOW_NUM, RETURN_NUM );  //0,-1：非翻页查询
		cmd_inq.Close();
	}
	catch (CDbException& ex)					//捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000021")/*信息读取失败。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1; //数据库异常时返回-1，事务将被回滚
	}
	catch (const CApplicationException& ex)	//捕获应用错误
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