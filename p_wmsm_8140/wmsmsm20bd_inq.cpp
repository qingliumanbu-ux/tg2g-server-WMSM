/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         JHZHAO
Version:		1.0
Date:			2016-03-05
Description:	待入库材料信息查询
**************************************************/

//框架头文件
#include "stdafx.h"
//#include "smhs.h"
//程序用头文件

//函数申明

/*<remark>=========================================================
///<summary>
///待入库材料信息查询
///<para>
///2.排序方式：队列写入时间
///</para>
///<para>数据库表：TWMA0 倒躲队列；TWMA1 物料主档表
///<returns>返回符合查询条件的队列信息</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wmsmsm20bd_inq)
int f_wmsmsm20bd_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString v_report_date = "";
	CString v_factory_div = "";
	CString v_stock_no = "";
	CString s_seq_no = "";
	CString v_col_name = "";

	int fetchRowCount = 0;

	/* 实体类定义 */
	//CTWM20BD twm20bd(conn);
	//CTWM20A1 twm20a1(conn);
	//CTWM20A2 twm20a2(conn);
	//CTWM20A3 twm20a3(conn);
	CModel twm20bd = CModel("TWM20BD");
	CModel twm20a1 = CModel("TWM20A1");
	CModel twm20a2 = CModel("TWM20A2");
	CModel twm20a3 = CModel("TWM20A3");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";

	/* 数据库操作类定义 */
	CDbCommand comm(conn);

	//系统的分页类信息。
	CPageInfo pageInfo;

	try
	{

		// 获取前台传入参数 

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

		v_report_date = bcls_rec->Tables[0].Rows[0]["REPORT_DATE"].ToString().TrimOrBlank();
		v_factory_div = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().TrimOrBlank();
		v_stock_no = bcls_rec->Tables[0].Rows[0]["STOCK_NO"].ToString().TrimOrBlank();

		Log::Trace("", __FUNCTION__, "v_report_date\t[{0}]", v_report_date);
		Log::Trace("", __FUNCTION__, "v_factory_div\t[{0}]", v_factory_div);
		Log::Trace("", __FUNCTION__, "v_stock_no\t[{0}]", v_stock_no);


		if (v_report_date.Trim() == "")
		{
			sprintf(s.msg, "日期不能都为空。");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (v_factory_div.Trim() == "" &&
			v_stock_no.Trim() == "")
		{
			sprintf(s.msg, "厂别/库区不能都为空。");
			throw CApplicationException(-1, s.msg, log.Location);
		}



#pragma region 设置返回块列名
		sqlstr =
			" SELECT * FROM TWM20A1 T1"
			" WHERE FACTORY_DIV = @factory_div"
			" AND STOCK_NO = @stock_no"
			" AND EXISTS (SELECT NULL FROM TWM01 T2"
			" WHERE T1.STOCK_NO = T2.STOCK_NO"
			" AND T2.MAT_LINE_TYPE = 'SM'"
			" AND T2.MAT_KIND = 'SM')";
		comm.SetCommandText(sqlstr);
		comm.Parameters.Set("factory_div", v_factory_div);
		comm.Parameters.Set("stock_no", v_stock_no);
		comm.ExecuteReader();
		while (comm.Read())
		{
			comm.Fetch(twm20a1);

			s_seq_no = twm20a1["SEQ_NO"].ToString();
			if (s_seq_no.GetLength() == 1)
			{
				s_seq_no = "0" + s_seq_no;
			}
			v_col_name = "CONDITION_" + s_seq_no;

			Log::Trace("", __FUNCTION__, "v_col_name\t[{0}]", v_col_name);
			bcls_ret->Tables[0].Columns.Add(DT_STRING, v_col_name, twm20a1["ITEM_CNAME"].ToString());
		}
		comm.Close();

		bcls_ret->Tables[0].Columns.Add(DT_STRING, "INIT_WT", "期初重量");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "INIT_NUM", "期初数量");



		sqlstr =
			" SELECT * FROM TWM20A3 T1"
			" WHERE FACTORY_DIV = @factory_div"
			" AND STOCK_NO = @stock_no"
			" AND EXISTS (SELECT NULL FROM TWM01 T2"
			" WHERE T1.STOCK_NO = T2.STOCK_NO"
			" AND T2.MAT_LINE_TYPE = 'SM'"
			" AND T2.MAT_KIND = 'SM')"
			" ORDER BY IN_OUT_DIV, SEQ_NO";
		comm.SetCommandText(sqlstr);
		comm.Parameters.Set("factory_div", v_factory_div);
		comm.Parameters.Set("stock_no", v_stock_no);
		comm.ExecuteReader();
		while (comm.Read())
		{
			comm.Fetch(twm20a3);


			s_seq_no = twm20a3["SEQ_NO"].ToString();
			if (s_seq_no.GetLength() == 1)
			{
				s_seq_no = "0" + s_seq_no;
			}
			if (twm20a3["IN_OUT_DIV"].ToString().Trim() == "1")
			{
				v_col_name = "INSTOCK_" + s_seq_no;
			}
			else if (twm20a3["IN_OUT_DIV"].ToString().Trim() == "2")
			{
				v_col_name = "OUTSTOCK_" + s_seq_no;
			}

			bcls_ret->Tables[0].Columns.Add(DT_STRING, v_col_name, twm20a3["ITEM_CNAME"].ToString());

			if (twm20a3["IN_OUT_DIV"].ToString().Trim() == "1")
			{
				v_col_name = "INSTOCK_NUM_" + s_seq_no;
			}
			else if (twm20a3["IN_OUT_DIV"].ToString().Trim() == "2")
			{
				v_col_name = "OUTSTOCK_NUM_" + s_seq_no;
			}
			bcls_ret->Tables[0].Columns.Add(DT_STRING, v_col_name, twm20a3["ITEM_CNAME"].ToString() + "数量");
		}
		comm.Close();


		bcls_ret->Tables[0].Columns.Add(DT_STRING, "ADJ_WT", "重量调整量");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "ADJ_NUM", "数量调整量");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "END_WT", "期末重量");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "END_NUM", "期末数量");

#pragma endregion

		sqlstr =
			" SELECT * FROM TWM20BD T1"
			" WHERE REPORT_DATE = @report_date"
			" AND EXISTS (SELECT NULL FROM TWM01 T2"
			" WHERE T1.STOCK_NO = T2.STOCK_NO"
			" AND T2.MAT_LINE_TYPE = 'SM'"
			" AND T2.MAT_KIND = 'SM')";

		if (v_factory_div.Trim() != "")
		{
			sqlstr += " AND FACTORY_DIV = @factory_div";
		}
		if (v_stock_no.Trim() != "")
		{
			sqlstr += " AND STOCK_NO = @stock_no";
		}

		comm.SetCommandText(sqlstr);
		comm.Parameters.Set("report_date", v_report_date);
		comm.Parameters.Set("factory_div", v_factory_div);
		comm.Parameters.Set("stock_no", v_stock_no);
		comm.ExecuteReader();
		while (comm.Read())
		{
			comm.Fetch(twm20bd);

			twm20bd.MergeTo(bcls_ret->Tables[0], true);
		}
		comm.Close();
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

