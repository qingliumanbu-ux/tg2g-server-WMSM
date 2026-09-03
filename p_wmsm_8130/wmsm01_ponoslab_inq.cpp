/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         LIZHEN
Version:		1.0
Date:			2023-11-07
Description:	使用pono查询未使用的slab_no
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件



//函数申明

/*<remark>=========================================================

===========================================================</remark>*/

BM2F_ENTERACE(wmsm01_ponoslab_inq);

int f_wmsm01_ponoslab_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;


	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */
	//CTWM06 twm06(conn);
	CModel twm06("TWM06");

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
	CDecimal cd_count = 0;
	int	record_count_per_page = 0; /* 每页记录数 */
	int	current_page_no = 0; /* 需查询的页号,从0开始计数 */
	int	start_row = 0; /* 将要压入outBlock的起始行 */


	/* 全局变量 */
	CString crane_no = "";
	CString stock_place_no_from = "";
	CString stock_place_no_to = "";
	/* 数据库操作类定义 */
	
	CDbCommand cmd_inq(conn);

	//系统的分页类信息。
	CPageInfo pageInfo;

	try
	{
		sqlstr = " WITH MM1 AS (select *																													   \
			from(SELECT max(LSLAB_NO)                                                      SLAB_NO,															   \
				max(PONO)                                                          PONO,																	   \
				max(STRAND_NO)                                                     STRAND_NO,																   \
				max(SLAB_WIDTH)                                                    SLAB_WIDTH,																   \
				max(SLAB_THICK)                                                    SLAB_THICK,																   \
				max(LSLAB_NO_LENGTH)                                               SLAB_LEN,																   \
				max(ORDER_NO)                                                      ORDER_NO,																   \
				max(LSLAB_NO_LENGTH_MAX)                                           SLAB_MAX_LEN,															   \
				max(LSLAB_NO_LENGTH_MIN)                                           SLAB_MIN_LEN,															   \
				max(SLAB_DEST)                                                     SLAB_DEST,																   \
				(LISTAGG(DISTINCT(SLAB_NO), '/') WITHIN GROUP(ORDER BY SLAB_NO)) SLAB_NO1																	   \
				FROM TPSSM03																																   \
				WHERE LSLAB_NO != SLAB_NO																													   \
				and SLAB_PROD_FLAG = '0'																													   \
				and substr2(PONO, 0, 1) != '9'																													\
				and PONO = '" + bcls_rec->Tables[0].Rows[0]["PONO"].ToString() + "'																			   \
				group by LSLAB_NO																															   \
				UNION																																		   \
				SELECT DISTINCT SLAB_NO,																													   \
				PONO,																																		   \
				STRAND_NO,																																	   \
				SLAB_WIDTH,																																	   \
				SLAB_THICK,																																	   \
				SLAB_LEN,																																	   \
				ORDER_NO,																																	   \
				SLAB_MAX_LEN,																																   \
				SLAB_MIN_LEN,																																   \
				SLAB_DEST,																																	   \
				SLAB_NO SLAB_NO1																															   \
				FROM TPSSM03																																   \
				WHERE LSLAB_NO = SLAB_NO																													   \
				and SLAB_PROD_FLAG = '0'																													   \
	            and substr2(PONO, 0, 1) != '9'																													\
				 and PONO in (select pono                                                                                                                      \
			from tpssm01                                                                                                                                       \
		where TPSSM01.CAST_LOT_NO = (select CAST_LOT_NO from tpssm01 where PONO = '" + bcls_rec->Tables[0].Rows[0]["PONO"].ToString() + "') and ST_NO = (select ST_NO from tpssm01 where PONO = '" + bcls_rec->Tables[0].Rows[0]["PONO"].ToString() + "')                    \
			and PONO_STATUS != '91'))					                                                                                                       \
			where 1 = 1)																																	   \
			SELECT a.*, b.ORDER_CUST_CNAME,B.TRNP_MODE_CODE																													   \
			FROM MM1 a																																		   \
			left join tqmom01 b on a.ORDER_NO = b.ORDER_NO																									   \
			where 1 = 1 ";
		Log::Trace("", __FUNCTION__, "sqlstr{0}", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
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