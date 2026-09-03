/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         lizhen
Version:		1.0
Date:			2024-4-1
Description:	碳钢热送率
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件

//函数申明

/*<remark>=========================================================
///<summary>
///卸车计划查询
///<para>
///2.排序方式：
///</para>
///<para>数据库表：TWMSM62 倒运计划表；
///<returns>返回符合查询条件的计划信息</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wmsmsc_inq);

int f_wmsmsc_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */


	CString sqlstr = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_con(conn);


	//系统的分页类信息。
	CPageInfo pageInfo;

	/* 业务变量 */


	try {
		for (int i = 0; i < bcls_rec->Tables[0].Columns.get_Count(); i++)
		{
			Log::Trace("", __FUNCTION__, "Columns				= [{0}]", i,bcls_rec->Tables[0].Columns[i].get_ColumnName());
		}
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		CString mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Replace(",", "','");
		sqlstr = " with mm1 as (select t1.HEAT_NO,\
			t3.SG_GRADE_1,\
			MAX(t1.MAT_WIDTH)            MAT_WIDTH,\
			MAX(t1.MAT_THICK)            MAT_THICK,\
			t1.ST_NO,\
			SUM(T1.MAT_WT)               MAT_WT,\
			SUM(T1.MAT_LEN)              MAT_LEN,\
			MAX(T1.TRUCK_NO)             TRUCK_NO,\
			MAX(SUBSTR2(T1.SLAB_NO, 18)) SLAB_NO\
			from vmmsm01 t1\
			left join tqmts0x t3 on t1.ST_NO = t3.ST_NO\
		where t1.MAT_NO IN('" + mat_no + "')\
			GROUP BY T1.HEAT_NO, T3.SG_GRADE_1, t1.ST_NO),\
			mm2 as(select *\
				from(select HEAT_NO, ELM_NAME, ELM_ACT\
					from tqmts29\
		where HEAT_NO = '" + bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString() + "') PIVOT(SUM(ELM_ACT) FOR ELM_NAME IN('C' C, 'Si' Si, 'Mn' Mn, 'P' P, 'S' S, 'Cr' Cr, 'Ni' Ni, 'Cu' Cu, 'Al' Al, 'Mo' Mo, 'V' V, 'Ti' Ti, 'Nb' Nb, 'N' N, 'As' AA, 'Sn' Sn, 'W' W, 'H' H, 'Creq/Nieq' CRNI)))\
			select mm1.SG_GRADE_1,\
			mm1.MAT_WIDTH || 'x' || mm1.MAT_THICK MAT_THICK,\
			mm1.ST_NO,\
			MM1.MAT_WT,\
			round(MM1.MAT_LEN / 1000, 3)          MAT_LEN,\
			MM1.SLAB_NO,\
			MM1.HEAT_NO,\
			C, SI, MN, P, S, CR, NI, CU, AL, MO, V, TI, NB, N, AA, SN, W, H, CRNI\
			from mm1\
			left join mm2 on mm1.HEAT_NO = mm2.HEAT_NO ";
		
		Log::Trace("", __FUNCTION__, "sqlstr				= [{0}]", (const char*)sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();

		
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "KP_TIME");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "KP_MARKER");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "GUIDE_DEST");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "SHIFT_GROUP");
		bcls_ret->Tables[0].Rows[0]["KP_TIME"] = datetime.Substring(0, 8);
		bcls_ret->Tables[0].Rows[0]["KP_MARKER"] = bcls_rec->Tables[0].Rows[0]["MAKER"].ToString();
		bcls_ret->Tables[0].Rows[0]["GUIDE_DEST"] = bcls_rec->Tables[0].Rows[0]["GUIDE_DEST"].ToString()+ bcls_rec->Tables[0].Rows[0]["TRUCK_NO"].ToString();
		CString shift_group = " ";
		if (bcls_rec->Tables[0].Rows[0]["SHIFT_GROUP"].ToString()=="A")
		{
			shift_group = "甲";
		}
		else if (bcls_rec->Tables[0].Rows[0]["SHIFT_GROUP"].ToString() == "B")
		{
			shift_group = "乙";
		}
		else if (bcls_rec->Tables[0].Rows[0]["SHIFT_GROUP"].ToString() == "C")
		{
			shift_group = "丙";
		}
		else if (bcls_rec->Tables[0].Rows[0]["SHIFT_GROUP"].ToString() == "D")
		{
			shift_group = "丁";
		}

		bcls_ret->Tables[0].Rows[0]["SHIFT_GROUP"] = shift_group;
		Log::Trace("", __FUNCTION__, "bcls_ret->Tables[0].Rows[0][KP_MARKER]				= [{0}]", bcls_ret->Tables[0].Rows[0]["KP_MARKER"].ToString());

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
	return doFlag;

}