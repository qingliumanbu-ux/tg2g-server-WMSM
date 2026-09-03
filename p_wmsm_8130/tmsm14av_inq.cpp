/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      gongnn
Version:     1.0
Date:        2024-03-01
Description: 保护渣抽检记录查询
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(tmsm14av_inq)


int f_tmsm14av_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString date_c = CDateTime::Now().ToString("yyyyMMdd");

	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */


	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlstr1 = "";
	CString sqlwhere = "";
	CString s_userid("");

	
	CString c_orderid("");
	CString sap_erp_matnr("");
	CString sampl_entr_no("");
	CModel ttmsm14("TTMSM14");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_con(conn);

	//系统的分页类信息。
	CPageInfo pageInfo;

	try
	{
		// 获取前台传入参数
		s_userid = s.userid;

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


		ttmsm14.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		
		/*
		if (bcls_rec->Tables[0].Columns.Contains("DATE_C"))
			date_c = bcls_rec->Tables[0].Rows[0]["DATE_C"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("C_ORDERID"))
			c_orderid = bcls_rec->Tables[0].Rows[0]["C_ORDERID"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("SAP_ERP_MATNR"))
			sap_erp_matnr = bcls_rec->Tables[0].Rows[0]["SAP_ERP_MATNR"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("SAMPLE_ENTR_NO"))
			sampl_entr_no = bcls_rec->Tables[0].Rows[0]["SAMPLE_ENTR_NO"].ToString();
		*/
		Log::Trace("", __FUNCTION__, "ttmsm14.DATE_C	= [{0}]", (const char*)ttmsm14["DATE_C"].ToString());
		Log::Trace("", __FUNCTION__, "ttmsm14.C_ORDERID	= [{0}]", (const char*)ttmsm14["C_ORDERID"].ToString());
		Log::Trace("", __FUNCTION__, "ttmsm14.SAP_ERP_MATNR	= [{0}]", (const char*)ttmsm14["SAP_ERP_MATNR"].ToString());
		Log::Trace("", __FUNCTION__, "ttmsm14.SAP_ERP_MATNR	= [{0}]", (const char*)ttmsm14["SAMPLE_ENTR_NO"].ToString());
		
		if (ttmsm14["DATE_C"].ToString().Trim() != "")
		{
			sqlwhere += "  AND DATE_C = @date_c ";
		}
		if (ttmsm14["C_ORDERID"].ToString().Trim() != "")
		{
			sqlwhere += " AND C_ORDERID LIKE  '%' || @c_orderid || '%' ";
		}
		if (ttmsm14["SAP_ERP_MATNR"].ToString().Trim() != "")
		{
			sqlwhere += " AND SAP_ERP_MATNR  LIKE  '%' || @sap_erp_matnr || '%' ";
		}
		if (ttmsm14["SAMPLE_ENTR_NO"].ToString().Trim() != "")
		{
			sqlwhere += " AND SAMPLE_ENTR_NO LIKE  '%' || @sampl_entr_no || '%' ";
		}
		

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
			//sqlstr = "SELECT * FROM TTMSM95 WHERE 1=1 ";

			sqlstr = " select * from ttmsm14 WHERE 1 = 1";  //查询ttmsm14
			break;
		}
		
		
		sqlstr = sqlstr + sqlwhere;
		sqlwhere = " ORDER BY DATE_C ";
		sqlstr = sqlstr + sqlwhere;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("date_c", ttmsm14["DATE_C"].ToString().SubstringNE(0,8));
		cmd_inq.Parameters.Set("c_orderid", ttmsm14["C_ORDERID"].ToString());
		cmd_inq.Parameters.Set("sap_erp_matnr", ttmsm14["SAP_ERP_MATNR"].ToString());
		cmd_inq.Parameters.Set("sampl_entr_no", ttmsm14["SAMPLE_ENTR_NO"].ToString());
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			fetchRowCount++;

			if (fetchRowCount >    (pageInfo.RecordFrom + pageInfo.PageSize))
			{
				Log::Trace("", __FUNCTION__, "超页面上限值，break");
				break;
			}
			if (!((fetchRowCount > pageInfo.RecordFrom) && (fetchRowCount <= (pageInfo.RecordFrom + pageInfo.PageSize))))
			{
				continue;
			}



		}

		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();
		Log::Trace(" ", " ", sqlstr);

		//返回记录总数
		CDataRow& row1 = bcls_ret->Tables["PAGEINFO"].Rows.Add();
		row1["recordsum"] = rowCount.ToInt32();
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "Database processing error，sqlcode = [{0}]." /* _RES("GCRSS0000006")*//*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
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

	return doFlag;
}


