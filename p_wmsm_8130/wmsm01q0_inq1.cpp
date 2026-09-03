/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         LIZHEN
Version:		1.0
Date:			2023-11-07
Description:	板坯数据查询
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件



//函数申明

/*<remark>=========================================================

===========================================================</remark>*/

BM2F_ENTERACE(wmsm01q0_inq1);

int f_wmsm01q0_inq1(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;


	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */
	//CTWM06 twm06(conn);
	CModel tmmsm01("TMMSM01");

	/* 数据库SQL操作字符串 */
	CString sql = "";
	CString sqlstr = "";
	CString sqlwhere = "";
	CString sqlstr_count;
	CString sqlstr_temp;

	/* 业务变量 */
	CString	datetime("");
	CString	cs_ladle_no("");
	CString	s_heat_no("");
	CString	s_st_no("");
	CDecimal cd_count = 0;
	int	record_count_per_page = 0; /* 每页记录数 */
	int	current_page_no = 0; /* 需查询的页号,从0开始计数 */
	int	start_row = 0; /* 将要压入outBlock的起始行 */


	/* 全局变量 */
	CString col_name = "";
	CString stock_place_no_from = "";
	CString stock_place_no_to = "";
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	//系统的分页类信息。
	CPageInfo pageInfo;
	EIClass temp;
	EIClass temp1;

	try
	{
		tmmsm01["SLAB_NO"] = bcls_rec->Tables[0].Rows[0]["SLAB_NO"].ToString();
		if (tmmsm01.QueryCount("SLAB_NO")>0)
		{
			sqlstr = " select T1.*,T3.*,(SELECT ORDER_THICK from tqmom01 where  ORDER_NO = T1.ORDER_NO) ORDER_THICK,ROUND((SELECT sum(DEVO_WT)\
				FROM TMMSMGY08\
				WHERE HEAT_NO = T1.HEAT_NO\
				AND MAT_CODE IN(select MAT_CODE from tmmsm50 t WHERE CS_FLAG IN('1', '3'))) / (SELECT sum(DEVO_WT)\
					FROM TMMSMGY08\
					WHERE HEAT_NO = T1.HEAT_NO\
					AND MAT_CODE IN\
					(select MAT_CODE from tmmsm50 t where MAT_TYPE in('1', '2', '4'))) *\
				100, 1)                                                  RC_RATE_S,\
				ROUND((SELECT sum(DEVO_WT)\
					FROM TMMSMGY08\
					WHERE HEAT_NO = T1.HEAT_NO\
					AND MAT_CODE IN(select MAT_CODE from tmmsm50 t WHERE CS_FLAG IN('2', '3'))) / (SELECT sum(DEVO_WT)\
						FROM TMMSMGY08\
						WHERE HEAT_NO = T1.HEAT_NO\
						AND MAT_CODE IN\
						(select MAT_CODE from tmmsm50 t where MAT_TYPE in('1', '2', '4'))) *\
					100, 1)            RCS_RATE_S, (SELECT ATTRI_NUM_ATFLV||'-'||ATTRI_NUM_ATFLB  FROM TQMOM02 WHERE ATTRI_ITEM ='A000' AND ORDER_NO =t1.ORDER_NO)  AS RC_RATE,\
(SELECT ATTRI_NUM_ATFLV||'-'||ATTRI_NUM_ATFLB  FROM TQMOM02 WHERE ATTRI_ITEM ='A001' AND ORDER_NO =t1.ORDER_NO) RCS_RATE \
 from Tmmsm01 T1 LEFT JOIN TPSSM03 T3 ON T1.PONO_SLAB=T3.SLAB_NO where T1.SLAB_NO='" + bcls_rec->Tables[0].Rows[0]["SLAB_NO"].ToString() + "' AND T1.MAT_NO NOT IN (SELECT TMMSM35.IN_MAT_NO FROM TMMSM35) ";
		}
		else
		{
			sqlstr = " select T1.*,T3.*,(SELECT ORDER_THICK from tqmom01 where  ORDER_NO = T1.ORDER_NO) ORDER_THICK,ROUND((SELECT sum(DEVO_WT)\
				FROM TMMSMGY08\
				WHERE HEAT_NO = T1.HEAT_NO\
				AND MAT_CODE IN(select MAT_CODE from tmmsm50 t WHERE CS_FLAG IN('1', '3'))) / (SELECT sum(DEVO_WT)\
					FROM TMMSMGY08\
					WHERE HEAT_NO = T1.HEAT_NO\
					AND MAT_CODE IN\
					(select MAT_CODE from tmmsm50 t where MAT_TYPE in('1', '2', '4'))) *\
				100, 1)                                                  RC_RATE_S,\
				ROUND((SELECT sum(DEVO_WT)\
					FROM TMMSMGY08\
					WHERE HEAT_NO = T1.HEAT_NO\
					AND MAT_CODE IN(select MAT_CODE from tmmsm50 t WHERE CS_FLAG IN('2', '3'))) / (SELECT sum(DEVO_WT)\
						FROM TMMSMGY08\
						WHERE HEAT_NO = T1.HEAT_NO\
						AND MAT_CODE IN\
						(select MAT_CODE from tmmsm50 t where MAT_TYPE in('1', '2', '4'))) *\
					100, 1)               RCS_RATE_S, (SELECT ATTRI_NUM_ATFLV||'-'||ATTRI_NUM_ATFLB  FROM TQMOM02 WHERE ATTRI_ITEM ='A000' AND ORDER_NO =t1.ORDER_NO)  AS RC_RATE,\
(SELECT ATTRI_NUM_ATFLV||'-'||ATTRI_NUM_ATFLB  FROM TQMOM02 WHERE ATTRI_ITEM ='A001' AND ORDER_NO =t1.ORDER_NO) RCS_RATE \
 from Hmmsm01 T1 LEFT JOIN TPSSM03 T3 ON T1.PONO_SLAB=T3.SLAB_NO where T1.SLAB_NO='" + bcls_rec->Tables[0].Rows[0]["SLAB_NO"].ToString() + "' AND T1.MAT_NO NOT IN (SELECT TMMSM35.IN_MAT_NO FROM TMMSM35) ";
		}
		Log::Trace("", "", "SQL", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();

		bcls_ret->Tables[0].set_TableName("TMMSM01");
		bcls_ret->Tables.Add();
		bcls_ret->Tables.Add();
		if (bcls_ret->Tables[0].Rows.get_Count() > 0) {
			s_heat_no = bcls_ret->Tables[0].Rows[0]["HEAT_NO"].ToString();
			s_st_no = bcls_ret->Tables[0].Rows[0]["ST_NO"].ToString();
			sqlstr = " select T29.ELM_NAME,T02.SPE_MAX MAX,T02.SPE_MIN MIN,T29.ELM_ACT AIM, "
				" CASE WHEN T29.ELM_ACT < T02.SPE_MIN OR T29.ELM_ACT > T02.SPE_MAX THEN '1' ELSE '0' END OK "
				" from tqmts29 t29\
				LEFT JOIN TQMTS0X T0X ON T0X.ST_NO = '"+s_st_no+"'\
				LEFT JOIN TQMTS02 t02 ON T02.IDX_NO = T0X.ELM_STD_IDX_A  AND T02.ELM_CODE = T29.ELM_CODE\
				WHERE\
				T29.HEAT_NO = '"+s_heat_no+"' AND t29.ELM_CODE NOT IN ('103') ";
			Log::Trace("","", "[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteQuery(temp.Tables[0]);
			cmd_inq.Close();
			if (temp.Tables[0].Rows.get_Count() == 0)
			{
				sqlstr = " select T02.ELM_NAME,\
					T02.SPE_MAX MAX,\
					T02.SPE_MIN MIN,\
					' '          AIM,\
					' '          OK\
					from TQMTS02 t02,\
					TQMTS0X T0X\
					WHERE T02.IDX_NO(+) = T0X.ELM_STD_IDX_A\
					AND T0x.ST_NO = '" + s_st_no + "' ";
				Log::Trace("",  "", "[{0}]", sqlstr);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(temp.Tables[0]);
				cmd_inq.Close();
			}
			if (temp.Tables[0].Rows.get_Count()>0)
			{
				bcls_ret->Tables[1].Columns.Add(DT_STRING, "HEAT_NO");
				bcls_ret->Tables[1].Rows.Add();
				bcls_ret->Tables[1].Rows[0]["HEAT_NO"] = s_heat_no;
				//Log::Trace("", "", "", "[{0}][{1}]", temp.Tables[0].Rows.get_Count(), temp.Tables[0].Columns.get_Count());
				for (int i = 0; i < temp.Tables[0].Rows.get_Count(); i++) {
					for (int j = 1; j < temp.Tables[0].Columns.get_Count(); j++) {
						col_name = temp.Tables[0].Rows[i]["ELM_NAME"].ToString().ToUpper() + "_" + temp.Tables[0].Columns[j].get_ColumnName();
						//Log::Trace("", "", "", "[{0}][{1}]", temp.Tables[0].Rows[i]["ELM_NAME"].ToString().ToUpper(), temp.Tables[0].Columns[j].get_ColumnName());
						if (bcls_ret->Tables[1].Columns.Contains(col_name))
						{
							sprintf(s.msg, "该钢种[%s]规程[%s]元素重复，请联系技术室处理，如需看元素，可在修改成分画面查看.", (const char*)s_st_no, (const char*)temp.Tables[0].Rows[i]["ELM_NAME"]);
							throw CApplicationException(-1, s.msg, log.Location);
						}
						bcls_ret->Tables[1].Columns.Add(DT_STRING, col_name);
						bcls_ret->Tables[1].Rows[0][col_name] = temp.Tables[0][i][j];
					}

				}
			}

			sqlstr = " SELECT ELM_NAME,ELM_ACT FROM TQMTS29 WHERE HEAT_NO = '" + s_heat_no + "' AND ELM_CODE NOT IN ('103')  ";
			Log::Trace("", "", "[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteQuery(temp1.Tables[0]);
			cmd_inq.Close();
			
			if (temp1.Tables[0].Rows.get_Count() > 0)
			{
				bcls_ret->Tables[2].Rows.Add();
				//Log::Trace("", "", "", "[{0}][{1}]", temp1.Tables[0].Rows.get_Count(), temp1.Tables[0].Columns.get_Count());
				for (int i = 0; i < temp1.Tables[0].Rows.get_Count(); i++) {
					
						col_name = temp1.Tables[0].Rows[i]["ELM_NAME"].ToString().ToUpper() ;
						//Log::Trace("", "", "", "[{0}][{1}]", temp1.Tables[0].Rows[i]["ELM_NAME"].ToString().ToUpper(), temp1.Tables[0].Rows[i]["ELM_ACT"].ToDecimal());
						bcls_ret->Tables[2].Columns.Add(DT_DECIMAL, col_name);
						bcls_ret->Tables[2].Rows[0][col_name] = temp1.Tables[0].Rows[i]["ELM_ACT"].ToDecimal();
					

				}
			}
		}
		
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