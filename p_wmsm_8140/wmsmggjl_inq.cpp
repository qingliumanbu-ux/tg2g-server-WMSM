/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   lz
Version:    1.0
Date:     2024-04-01 17:13:56
Description:
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"





// service入口
BM2F_ENTERACE(wmsmggjl_inq)

int f_wmsmggjl_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString v_mat_no = "";
	CString v_heat_no = "";
	CString v_st_no = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = " ";
	int		TotalRecordCount = 0;
	CString v_start_time = "";
	CString v_end_time = "";


	//系统的分页类信息。
	CPageInfo pageInfo;



	CDbCommand cmd_inq(conn);

	try
	{
		sqlstr = " SELECT *\
			FROM(\
				SELECT row_number() over(partition by MAT_NO order by REC_CREATE_TIME DESC) ROW_ID, M.*\
				FROM(\
					select A.MAT_NO,\
					A.BATCH,\
					A.HEAT_NO,\
					A.SLAB_CUT_TIME,\
					A.PREC_ST_NO,\
					A.ST_NO,\
					A.MAT_ACT_WT,\
					A.SG_SIGN,\
					A.SG_GRADE_1,\
					B.REC_CREATE_TIME\
					from Vmmsm01 A\
					LEFT JOIN VMMSM96 B ON A.MAT_NO = B.MAT_NO AND B.EVENT_ID IN('QM73', 'MM03', 'MM2N') AND\
					A.ST_NO = B.ST_NO\
		where A.PREC_ST_NO != A.ST_NO\
		AND A.PREC_ST_NO != ' '\
			order by A.SLAB_CUT_TIME desc) M)\
			WHERE ROW_ID = 1  ";
		for (int i = 0; i < bcls_rec->Tables[0].Columns.get_Count(); i++)
		{
			

			if (bcls_rec->Tables[0].Columns[i].get_DataType() == DT_STRING
				&& bcls_rec->Tables[0].Rows[0][i].ToString().Trim().IsEmpty())
			{
				continue;
			}

			if (bcls_rec->Tables[0].Columns[i].get_DataType() == DT_DECIMAL
				&& bcls_rec->Tables[0].Rows[0][i].ToDecimal() == 0)
			{
				continue;
			}

			if (bcls_rec->Tables[0].Columns[i].get_DataType() == DT_DATETIME
				&& bcls_rec->Tables[0].Rows[0][i].ToString().Trim().IsEmpty())
			{
				continue;
			}
			sqlstr += " AND " + bcls_rec->Tables[0].Columns[i].get_ColumnName() + " LIKE @" + bcls_rec->Tables[0].Columns[i].get_ColumnName() + "||'%' ";
			cmd_inq.Parameters.Set(bcls_rec->Tables[0].Columns[i].get_ColumnName(), bcls_rec->Tables[0].Rows[0][i].ToString().Trim());
		}
		sqlstr += " order by SLAB_CUT_TIME desc ";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
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
