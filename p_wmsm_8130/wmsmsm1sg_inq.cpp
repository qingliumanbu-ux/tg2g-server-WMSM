/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         lq
Version:		1.0
Date:			2023/3/18
Description:	库图后台查询位置查询
**************************************************/

//框架头文件
#include "stdafx.h"

BM2F_ENTERACE(wmsmsm1sg_inq);

int f_wmsmsm1sg_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString FUNC_ID = "";
	CString stock_place_no = "";
	CString STOCK_NO = "";
	CString STOCK_NO1 = "";
	CString STOCK_NUM = "";
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	//系统的分页类信息。
	CPageInfo pageInfo;

	try
	{
		if (bcls_rec->Tables[0].Columns.Contains("STOCK_PLACE_NO"))
			stock_place_no = bcls_rec->Tables[0].Rows[0]["STOCK_PLACE_NO"].ToString().Trim();


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
		Log::Trace("", "", "STOCK_NO= {0}", STOCK_NO);
		//  --根据总库容和总库存可以计算百分比
		//LOGIC_STOCK_NO:区域号  STOCK_NUM：总库位数量   KC_WT：库存重量  KC_WT：库存数量

		//空垛位：
		bcls_ret->Tables.Add("TABLE_1");
		CString sql = " SELECT COUNT(1) AS EMPTY_RESULT" 
			" FROM TWM04  WHERE NOT EXISTS( "
			" SELECT DISTINCT STOCK_PLACE_NO FROM TWMA2 WHERE TWMA2.STOCK_PLACE_NO = TWM04.STOCK_PLACE_NO) AND TWM04.STOCK_NO='" + STOCK_NO1 + "'"; 
		Db::QueryTable(sql, bcls_ret->Tables["TABLE_1"]);
		Log::Trace("", "", "111111");
		//总块数,实际总重量
		bcls_ret->Tables.Add("TABLE_2");
		 sql = "SELECT  E.STOCK_NUM ,SUM(MAT_ACT_WT) AS KC_WT "
			" FROM TWMA2 A, TMMSM01 B, TWM04  C,( SELECT LOGIC_STOCK_NO ,sum(MAX_WT ) AS MAX_WT, SUM(1) AS STOCK_NUM FROM twm04 WHERE LOGIC_STOCK_NO!=' ' "
			" GROUP BY LOGIC_STOCK_NO) E "
			" WHERE A.MAT_NO = B.MAT_NO AND A.STOCK_PLACE_NO = C.STOCK_PLACE_NO	  "
			" AND C.STOCK_NO IN " + STOCK_NO +
			" AND E.LOGIC_STOCK_NO=C.LOGIC_STOCK_NO	  "
			" GROUP BY C.LOGIC_STOCK_NO, E.STOCK_NUM ,E.MAX_WT ";

		cmd_inq.SetCommandText(sql);
		cmd_inq.ExecuteReader();

		if (cmd_inq.Read())
		{
			STOCK_NUM = cmd_inq.GetString(1);
		}
		else
		{
			if (!bcls_ret->Tables["TABLE_2"].Columns.Contains("STOCK_NUM"))
			{
				bcls_ret->Tables["TABLE_2"].Columns.Add(DT_DECIMAL, "STOCK_NUM");
			}
			if (!bcls_ret->Tables["TABLE_2"].Columns.Contains("KC_WT"))
			{
				bcls_ret->Tables["TABLE_2"].Columns.Add(DT_DECIMAL, "KC_WT");
			}
			bcls_ret->Tables["TABLE_2"].Rows.Add();
			bcls_ret->Tables["TABLE_2"].Rows[0]["STOCK_NUM"] = 0;
			bcls_ret->Tables["TABLE_2"].Rows[0]["KC_WT"] = 0;
			STOCK_NUM = '0';
		}
		Db::QueryTable(sql, bcls_ret->Tables["TABLE_2"]);
		Log::Trace("", "", "2222222");
		cmd_inq.Close();
		//库容比：  
		bcls_ret->Tables.Add("TABLE_3");
		Log::Trace("", "", "3333333");
		 sql = "SELECT " + STOCK_NUM + " /SUM(MAX_LAYER_COUNT) AS S_RESULT FROM TWM04  WHERE STOCK_PLACE_TYPE !='9' AND STOCK_NO='" + STOCK_NO1 + "'";
		 Log::Info("", __FUNCTION__, "===sql==== [{0}]", sql);
		 Db::QueryTable(sql, bcls_ret->Tables["TABLE_3"]);
		 Log::Trace("", "", "3333333");

		cmd_inq.SetCommandText(sql);
		cmd_inq.ExecuteReader();
		cmd_inq.Close();

		Log::Trace("", "", "3333333");

		//库位信息 取值  04表：    
		bcls_ret->Tables.Add("TABLE_6");
		Log::Trace("", "", "6666666");

		sql = "SELECT '主垛位' || (SELECT code_desc_1_content FROM TEP0002 WHERE CODE_CLASS = 'YM01' and code = a.PILE_FIELDNO_NO) || ', "
			"允许块数:' || (SELECT code_desc_1_content FROM TEP0002 WHERE CODE_CLASS = 'YM01' and code = a.MAX_HEIGHT)  || ', "
			"允许最大宽差 : ' ||  (SELECT code_desc_1_content FROM TEP0002 WHERE CODE_CLASS = 'YM01' and code = a.MAX_WIDTH) || ',  "
			"允许冷却时间差 : ' || (SELECT code_desc_1_content FROM TEP0002 WHERE CODE_CLASS = 'YM01' and code = a.INTER_OP_TIME_HH) || ',"
			"扩展属性 : ' || (SELECT code_desc_1_content FROM TEP0002 WHERE CODE_CLASS = 'YM01' and code = a.PILE_FIELDNO_NO2) AS S_TEST    "
			"FROM TWM04 a where stock_place_no = '" + stock_place_no + "'   ";
		Log::Info("", __FUNCTION__, "===sql==== [{0}]", sql);
		Db::QueryTable(sql, bcls_ret->Tables["TABLE_6"]);
		Log::Trace("", "", "666666");

		cmd_inq.SetCommandText(sql);
		cmd_inq.ExecuteReader();
		cmd_inq.Close();

		Log::Trace("", "", "666666");





		// --库位明细
		//LOGIC_STOCK_NO：区域号  STOCK_PLACE_NO 库位号  HEAT_NO：炉号   STOCK_NUM：材料数量  STOCK_PLACE_TYPE库位类型 STOCK_STATUS 库位状态 REMARK 库位说明备注

		bcls_ret->Tables.Add("TABLE_4");
		// G twmg10    E 一堆   H twm04  C TWM04    B  TMMBW01
		//sql = "SELECT G.*, E.*,H.LOGIC_STOCK_NO,H.STOCK_PLACE_TYPE, H.STOCK_STATUS, H.REMARK FROM TWMG10 G LEFT JOIN ( "
		//	" SELECT C.STOCK_PLACE_NO,B.HEAT_NO ,SUM(1) AS STOCK_NUM FROM TWMA2 A, TMMBW01 B, TWM04  C	"
		//	"  WHERE A.MAT_NO = B.MAT_NO AND A.STOCK_PLACE_NO = C.STOCK_PLACE_NO	AND C.STOCK_NO IN " + STOCK_NO +
		//	"  GROUP BY C.STOCK_PLACE_NO,B.HEAT_NO) E ON G.ITEM_CODE = E.STOCK_PLACE_NO	"
		//	" LEFT JOIN TWM04 H ON G.ITEM_CODE = H.STOCK_PLACE_NO WHERE G.FUNC_ID ='" + FUNC_ID + "'";

		 sql = "SELECT G.*,Z.*,H.LOGIC_STOCK_NO,H.STOCK_PLACE_TYPE, H.STOCK_STATUS AS STOCK_STATUS ,Z.HOLD_FLAG AS HOLD_FLAG, H.REMARK,E.STOCK_NUM FROM TWMG10 G "
			" LEFT JOIN (SELECT COUNT(1) AS STOCK_NUM,A.STOCK_PLACE_NO FROM TWMA2 A,TWM04 H WHERE A.STOCK_PLACE_NO =H.STOCK_PLACE_NO GROUP BY A.STOCK_PLACE_NO ) E ON G.ITEM_CODE =E.STOCK_PLACE_NO "
			" LEFT JOIN (SELECT B.* FROM TWM04 A,TMMSM01 B WHERE A.STOCK_PLACE_NO=B.STOCK_PLACE_NO ) Z ON G.ITEM_CODE =Z.STOCK_PLACE_NO "
			 " LEFT JOIN TWM04 H    "

			 "ON G.ITEM_CODE = H.STOCK_PLACE_NO WHERE G.FUNC_ID = '" + FUNC_ID + "'    " ;
		Db::QueryTable(sql, bcls_ret->Tables["TABLE_4"]);
		Log::Trace("", "", "444444");

		//材料明细
		bcls_ret->Tables.Add("TABLE_5");
		Log::Trace("", "", "3333333");
		sql = "SELECT * FROM TMMSM01  WHERE STOCK_NO='" + STOCK_NO1 + "' ORDER BY STOCK_PLACE_NO,LAYERNO DESC";
		Log::Info("", __FUNCTION__, "===sql==== [{0}]", sql);
		Db::QueryTable(sql, bcls_ret->Tables["TABLE_5"]);
		Log::Trace("", "", "3333333");


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

