/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         lq
Version:		1.0
Date:			2023/3/18
Description:	棒线坯料库图后台查询位置查询(B10库)
**************************************************/

//框架头文件
#include "stdafx.h"

BM2F_ENTERACE(wmsmsm1_inq);

int f_wmsmsm1_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString FUNC_ID = "";
	CString STOCK_NO = "";
	CString STOCK_NO1 = "";
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	//系统的分页类信息。
	CPageInfo pageInfo;

	try
	{

		if (bcls_rec->Tables[0].Columns.Contains("FUNC_ID"))
			FUNC_ID = bcls_rec->Tables[0].Rows[0]["FUNC_ID"].ToString().Trim();
		if (FUNC_ID == "")
		{
			sprintf(s.msg, "功能不能为空");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//将FUNC_ID转换成库

		CString sqlstr = "select code from TEP0002 where code_class='WM100' and code_desc_1_content= '" + FUNC_ID + "'";

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();

		if (cmd_inq.Read())
		{
			STOCK_NO1 = cmd_inq.GetString(1);
		}
		CString s1 = "('";
		CString s2 = "')";
		STOCK_NO = s1 + STOCK_NO1 + s2;
		cmd_inq.Close();
		//STOCK_NO = "('B10')";
		Log::Trace("", "", "000000");
		Log::Trace("", "", "FUNC_ID= {0}", FUNC_ID);
		//  --根据总库容和总库存可以计算百分比
		//LOGIC_STOCK_NO:区域号  STOCK_NUM：总库位数量   KC_WT：库存重量  KC_WT：库存数量

		//CString sql = "SELECT  C.LOGIC_STOCK_NO ,D.STOCK_NUM  , SUM(MAT_ACT_WT) AS KC_WT,SUM(1) AS KC_NUM "
		//	" FROM TWMA2 A, TMMBW01 B, TWM04  C															  "
		//	" LEFT JOIN(SELECT LOGIC_STOCK_NO, SUM(1) AS STOCK_NUM FROM TWM04 GROUP BY LOGIC_STOCK_NO) D  "
		//	" ON C.LOGIC_STOCK_NO = D.LOGIC_STOCK_NO													  "
		//	" WHERE A.MAT_NO = B.MAT_NO AND A.STOCK_PLACE_NO = C.STOCK_PLACE_NO	AND C.STOCK_NO IN " + STOCK_NO +
		//	" GROUP BY C.LOGIC_STOCK_NO, D.STOCK_NUM ";

		//CString sql = "SELECT  C.LOGIC_STOCK_NO ,D.STOCK_NUM ,E.MAX_WT , SUM(MAT_ACT_WT) AS KC_WT,SUM(1) AS KC_NUM "
		//	" FROM TWMA2 A, TMMBW01 B, TWM04  C,( SELECT LOGIC_STOCK_NO,sum(MAX_WT ) AS MAX_WT FROM twm04 GROUP BY LOGIC_STOCK_NO)	E  "
		//	" LEFT JOIN(SELECT LOGIC_STOCK_NO, SUM(1) AS STOCK_NUM FROM TWM04 GROUP BY LOGIC_STOCK_NO) D  "
		//	" ON C.LOGIC_STOCK_NO = D.LOGIC_STOCK_NO													  "
		//	" WHERE A.MAT_NO = B.MAT_NO AND A.STOCK_PLACE_NO = C.STOCK_PLACE_NO	AND C.STOCK_NO IN "+ STOCK_NO +
		//	" AND E.LOGIC_STOCK_NO=C.LOGIC_STOCK_NO GROUP BY C.LOGIC_STOCK_NO, D.STOCK_NUM ,E.MAX_WT ";

		bcls_ret->Tables.Add("TABLE_1");

		CString sql = "SELECT  C.LOGIC_STOCK_NO ,E.STOCK_NUM ,E.MAX_WT , SUM(MAT_ACT_WT) AS KC_WT,SUM(1) AS KC_NUM "
			" FROM TWMA2 A, TMMSM01 B, TWM04  C,( SELECT LOGIC_STOCK_NO ,sum(MAX_WT ) AS MAX_WT, SUM(1) AS STOCK_NUM FROM twm04 WHERE LOGIC_STOCK_NO!=' ' "
			" GROUP BY LOGIC_STOCK_NO) E "
			" WHERE A.MAT_NO = B.MAT_NO AND A.STOCK_PLACE_NO = C.STOCK_PLACE_NO	  "
			" AND C.STOCK_NO IN " + STOCK_NO +
			" AND E.LOGIC_STOCK_NO=C.LOGIC_STOCK_NO	  "
			" GROUP BY C.LOGIC_STOCK_NO, E.STOCK_NUM ,E.MAX_WT ";

		Db::QueryTable(sql, bcls_ret->Tables["TABLE_1"]);
		Log::Trace("", "", "111111");
		//--待判量(综判)		
		//NOCONFIM_WT ：待判量
		bcls_ret->Tables.Add("TABLE_2");
		sql = "SELECT   SUM(MAT_ACT_WT) AS NOCONFIM_WT FROM TMMSM01 WHERE  COMPLEX_DECIDE_CODE = '0' AND STOCK_NO IN " + STOCK_NO;
		Db::QueryTable(sql, bcls_ret->Tables["TABLE_2"]);
		Log::Trace("", "", "222222");
		//--可发量 CONFIM_WT
		bcls_ret->Tables.Add("TABLE_3");
		sql = "SELECT   SUM(MAT_ACT_WT) AS CONFIM_WT FROM TMMSM01 WHERE  COMPLEX_DECIDE_CODE = '1' AND STOCK_NO IN " + STOCK_NO;
		Db::QueryTable(sql, bcls_ret->Tables["TABLE_3"]);
		Log::Trace("", "", "33333");
		////--库位明细
		////LOGIC_STOCK_NO：区域号  STOCK_PLACE_NO 库位号  HEAT_NO：炉号   STOCK_NUM：材料数量
		//bcls_ret->Tables.Add("TABLE_4");
		//sql = "SELECT E.*,F.HEAT_NO FROM (																												"
		//	" SELECT  C.LOGIC_STOCK_NO, C.STOCK_PLACE_NO, SUM(1) AS  STOCK_NUM																			"
		//	" FROM TWMA2 A, TMMBW01 B, TWM04  C																											"
		//	" WHERE A.MAT_NO = B.MAT_NO AND A.STOCK_PLACE_NO = C.STOCK_PLACE_NO	AND C.STOCK_NO IN "+ STOCK_NO +
		//	" GROUP BY C.LOGIC_STOCK_NO, C.STOCK_PLACE_NO) E LEFT JOIN(SELECT DISTINCT STOCK_PLACE_NO, HEAT_NO FROM TMMBW01 WHERE STOCK_PLACE_NO IN(	"
		//	" SELECT STOCK_PLACE_NO FROM(																												"
		//	" 	SELECT DISTINCT  a.STOCK_PLACE_NO, B.HEAT_NO  FROM TWMA2 A, TMMBW01 B  WHERE A.MAT_NO = B.MAT_NO)GROUP BY STOCK_PLACE_NO				"
		//	" 	HAVING COUNT(STOCK_PLACE_NO) = 1)) F ON E.STOCK_PLACE_NO = F.STOCK_PLACE_NO";
		//Db::QueryTable(sql, bcls_ret->Tables["TABLE_4"]);
		//
		// 
		// 
		// --库位明细
		//LOGIC_STOCK_NO：区域号  STOCK_PLACE_NO 库位号  HEAT_NO：炉号   STOCK_NUM：材料数量  STOCK_PLACE_TYPE库位类型 STOCK_STATUS 库位状态 REMARK 库位说明备注

		bcls_ret->Tables.Add("TABLE_4");

		sql = "SELECT G.*, E.*,H.LOGIC_STOCK_NO,H.STOCK_PLACE_TYPE, H.STOCK_STATUS, H.REMARK FROM TWMG10 G LEFT JOIN ( "
			" SELECT C.STOCK_PLACE_NO,B.HEAT_NO ,SUM(1) AS STOCK_NUM FROM TWMA2 A, TMMSM01 B, TWM04 C	"
			"  WHERE A.MAT_NO = B.MAT_NO AND A.STOCK_PLACE_NO = C.STOCK_PLACE_NO	AND C.STOCK_NO IN " + STOCK_NO +
			"  GROUP BY C.STOCK_PLACE_NO,B.HEAT_NO) E ON G.ITEM_CODE = E.STOCK_PLACE_NO	"
			" LEFT JOIN TWM04 H ON G.ITEM_CODE = H.STOCK_PLACE_NO WHERE G.FUNC_ID ='" + FUNC_ID + "'";

		Db::QueryTable(sql, bcls_ret->Tables["TABLE_4"]);
		Log::Trace("", "", "444444");



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

