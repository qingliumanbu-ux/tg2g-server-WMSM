/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         LIZHEN
Version:		1.0
Date:			2023-11-07
Description:	查询2250板坯数据
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件



//函数申明

/*<remark>=========================================================

===========================================================</remark>*/

BM2F_ENTERACE(wmsm01gg_inq1);

int f_wmsm01gg_inq1(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;


	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */

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
	CString	cs_sm_unit_no("");
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
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		sqlstr = " select * from Vmmsm01 where 1=1 and slab_no='" + bcls_rec->Tables[0].Rows[0]["SLAB_NO"].ToString() + "'";
		Log::Trace("", __FUNCTION__, "sqlstr[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();
		tmmsm01.MergeFrom(bcls_ret->Tables[0].Rows[0]);

		bcls_ret->Tables.Add();
		sqlstr = " select T03.CAST_LOT_NO,\
			V1.ST_NO,\
			(SELECT MAT_NO FROM VMMSM01 WHERE LSLAB_NO = T03.LSLAB_NO AND PONO = V1.PONO) MAT_NO,\
			T03.*,\
			'0'                                                                           ifkepei,\
			T33.LEN_MIN,\
			T33.LEN_MAX,\
			T33.MINWIDTH,\
			T33.MAXWIDTH,\
			T33.THICK_MIN,\
			T33.THICK_MAX\
			from VPSSM11 V1\
			left join(select LSLAB_NO,\
				listagg(SLAB_NO, '/'),\
				max(PONO)                                                                        pono,\
				max(SLAB_PROD_FLAG)                                                              SLAB_PROD_FLAG,\
				decode(max(LSLAB_NO_LENGTH), 0, max(SLAB_LEN), max(LSLAB_NO_LENGTH))             SLAB_LEN,\
				decode(max(LSLAB_NO_LENGTH_MAX), 0, max(SLAB_MAX_LEN),\
					max(LSLAB_NO_LENGTH_MAX))                                                 SLAB_MAX_LEN,\
				decode(max(LSLAB_NO_LENGTH_MIN), 0, max(SLAB_MIN_LEN),\
					max(LSLAB_NO_LENGTH_MIN))                                                 SLAB_MIN_LEN,\
				max(SLAB_WIDTH)                                                                  SLAB_WIDTH,\
				max(SLAB_THICK)                                                                  SLAB_THICK,\
				max(SLAB_DEST)                                                                   SLAB_DEST,\
				max(apn)                                                                         apn,\
				max(ORDER_NO)                                                                    ORDER_NO,\
				max(SG_SIGN)                                                                     SG_SIGN,\
				max(BILLET_TYPE)                                                                 BILLET_TYPE,\
				max(MATIRAL_CODE)                                                                MATIRAL_CODE,\
				max(CAST_LOT_NO)                                                                 CAST_LOT_NO,\
				 max(FACTORY_NEXT)                                                                FACTORY_NEXT\
				from tpssm03\
		where PONO = '" + bcls_ret->Tables[0].Rows[0]["PONO"].ToString() + "'\
			group by LSLAB_NO) T03 ON V1.PONO = T03.PONO\
			left join Vpssm10 T10 ON V1.PONO = T10.PONO\
			LEFT JOIN TMMSM33BPGG T33 ON DECODE(T10.SLAB_DEST, '41', '6', T10.C_DIV) = T33.ST_NO AND\
			DECODE(SUBSTR2(T03.MATIRAL_CODE, 2, 2), 'AA', 'J', 'AB', 'Z') = T33.SLAB_TYPE\
			WHERE 1 = 1\
			and V1.PONO = '" + bcls_ret->Tables[0].Rows[0]["PONO"].ToString() + "'";


		Log::Trace("", __FUNCTION__, "sqlstr[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[1]);
		cmd_inq.Close();

		bcls_ret->Tables.Add();
		sqlstr = " select  T03.CAST_LOT_NO, V1.ST_NO,(SELECT MAT_NO FROM VMMSM01 WHERE LSLAB_NO=T03.LSLAB_NO AND PONO IN (SELECT PONO\
			FROM TPSSM01\
			WHERE CAST_LOT_NO = '" + bcls_ret->Tables[1].Rows[0]["CAST_LOT_NO"].ToString() + "')) MAT_NO, T03.*,'0'ifkepei,T33.LEN_MIN,T33.LEN_MAX,T33.MINWIDTH,T33.MAXWIDTH,T33.THICK_MIN,T33.THICK_MAX\
			from VPSSM11 V1\
			left join (select LSLAB_NO,\
			listagg(SLAB_NO, '/'),\
			max(PONO)                                                            pono,\
			max(SLAB_PROD_FLAG)                                                  SLAB_PROD_FLAG,\
			decode(max(LSLAB_NO_LENGTH), 0, max(SLAB_LEN), max(LSLAB_NO_LENGTH)) SLAB_LEN,\
			decode(max(LSLAB_NO_LENGTH_MAX), 0, max(SLAB_MAX_LEN),\
				max(LSLAB_NO_LENGTH_MAX))                                     SLAB_MAX_LEN,\
			decode(max(LSLAB_NO_LENGTH_MIN), 0, max(SLAB_MIN_LEN),\
				max(LSLAB_NO_LENGTH_MIN))                                     SLAB_MIN_LEN,\
			max(SLAB_WIDTH)                                                      SLAB_WIDTH,\
			max(SLAB_THICK)                                                      SLAB_THICK,\
			max(SLAB_DEST)                                                       SLAB_DEST,\
			max(apn)                                                             apn,\
			max(ORDER_NO)                                                        ORDER_NO,\
			max(SG_SIGN)                                                         SG_SIGN,\
			max(BILLET_TYPE)                                                     BILLET_TYPE,\
			max(MATIRAL_CODE)                                                    MATIRAL_CODE,\
			max(CAST_LOT_NO)                                                     CAST_LOT_NO,\
			 max(FACTORY_NEXT)                                                    FACTORY_NEXT\
			from tpssm03\
		where CAST_LOT_NO = '" + bcls_ret->Tables[1].Rows[0]["CAST_LOT_NO"].ToString() + "'\
			group by LSLAB_NO) T03 ON V1.PONO = T03.PONO\
			left join Vpssm10 T10 ON V1.PONO = T10.PONO\
			LEFT JOIN TMMSM33BPGG T33 ON DECODE(T10.SLAB_DEST, '41', '6', T10.C_DIV) = T33.ST_NO AND\
			DECODE(SUBSTR2(T03.MATIRAL_CODE, 2, 2), 'AA', 'J', 'AB', 'Z') = T33.SLAB_TYPE\
			WHERE 1 = 1 and V1.PONO IN (SELECT PONO\
			FROM TPSSM01\
			WHERE CAST_LOT_NO = '" + bcls_ret->Tables[1].Rows[0]["CAST_LOT_NO"].ToString() + "') ";


		Log::Trace("", __FUNCTION__, "sqlstr[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[2]);
		cmd_inq.Close();

		bcls_ret->Tables.Add();
		if (bcls_rec->Tables[0].Rows[0]["PONO_SLAB"].ToString().Trim() != "")
		{
			
			sqlstr = " select decode(LSLAB_NO_LENGTH, 0, SLAB_LEN, LSLAB_NO_LENGTH)             SLAB_LEN,\
				decode(LSLAB_NO_LENGTH_MAX, 0, SLAB_MAX_LEN, LSLAB_NO_LENGTH_MAX) SLAB_MAX_LEN,\
				decode(LSLAB_NO_LENGTH_MIN, 0, SLAB_MIN_LEN, LSLAB_NO_LENGTH_MIN) SLAB_MIN_LEN,\
				SLAB_WIDTH, SLAB_THICK, SLAB_DEST, apn, ORDER_NO, SG_SIGN, BILLET_TYPE, MATIRAL_CODE, CAST_LOT_NO, FACTORY_NEXT from tpssm03 where 1=1 and slab_no='" + bcls_rec->Tables[0].Rows[0]["PONO_SLAB"].ToString() + "'";

			Log::Trace("", __FUNCTION__, "sqlstr[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[3]);
			cmd_inq.Close();
		}
		else
		{
			for (int i = 0; i < bcls_ret->Tables[1].Rows.get_Count(); i++)
			{
				if (bcls_ret->Tables[1].Rows[i]["SLAB_PROD_FLAG"].ToString() == "0"
					&&tmmsm01["MAT_LEN"].ToDecimal()>= bcls_ret->Tables[1].Rows[i]["SLAB_MIN_LEN"].ToDecimal()
					&& tmmsm01["MAT_LEN"].ToDecimal() <= bcls_ret->Tables[1].Rows[i]["SLAB_MAX_LEN"].ToDecimal())
				{
					bcls_ret->Tables[1].Rows[i]["IFKEPEI"] = "1";
				}
			}
			for (int i = 0; i < bcls_ret->Tables[2].Rows.get_Count(); i++)
			{
				if (bcls_ret->Tables[2].Rows[i]["SLAB_PROD_FLAG"].ToString() == "0"
					&& tmmsm01["MAT_LEN"].ToDecimal() >= bcls_ret->Tables[2].Rows[i]["SLAB_MIN_LEN"].ToDecimal()
					&& tmmsm01["MAT_LEN"].ToDecimal() <= bcls_ret->Tables[2].Rows[i]["SLAB_MAX_LEN"].ToDecimal()
					&& bcls_ret->Tables[2].Rows[i]["FACTORY_NEXT"].ToString() != "6391")
				{
					bcls_ret->Tables[2].Rows[i]["IFKEPEI"] = "1";
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