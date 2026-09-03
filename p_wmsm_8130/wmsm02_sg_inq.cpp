/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         lq
Version:		1.0
Date:			2023/3/18
Description:	库图后台查询位置查询
**************************************************/

//框架头文件
#include "stdafx.h"

BM2F_ENTERACE(wmsm02_sg_inq);

int f_wmsm02_sg_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString FUNC_ID = "";
	CString stock_place_no = "";

	CString v_condition = "";
	
	CString STOCK_NO = "";
	CString STOCK_NO1 = "";
	CString STOCK_NUM = "";
	//跟踪图 变量：
	CString location = "";
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDataTable dt;
	CDataTable dt2;
	//系统的分页类信息。
	CPageInfo pageInfo;

	try
	{                                                                                                              
		if (bcls_rec->Tables[0].Columns.Contains("REST_ROLLER_NO"))
			location = bcls_rec->Tables[0].Rows[0]["REST_ROLLER_NO"].ToString().Trim();




		//  1  grid   ：    
		bcls_ret->Tables.Add("TABLE_6");
		Log::Trace("", "", "6666666");

		CString sql = "SELECT  a.mat_no,value(hold_flag,' '),a.rest_roller_no as location,nvl(b.slab_cut_time,' ') as slab_cut_time,nvl(a.SLAB_DEST,' ') AS A,   "
			  "nvl(a.slab_print_mode,' ')AS B ,nvl(a.slab_burr_type,' ')AS C,nvl(b.prec_slab_no,  ' ')AS PREC_SLAB_NO ,nvl(a.fix_flag,' '),nvl(a.qual_id,' ')as D,nvl(a.auto_wt_flag,' '),nvl(a.hot_send_flag,' ')  "
			  "from TWMM1 a left outer join(select * from tmmsm01 where mat_position in('1', '2')) b on a.mat_no = b.mat_no     "
			  "where REST_ROLLER_NO in('5')    ";
		Log::Info("", __FUNCTION__, "===sql==== [{0}]", sql);
		Db::QueryTable(sql, bcls_ret->Tables["TABLE_6"]);
		Log::Trace("", "", "666666");

		cmd_inq.SetCommandText(sql);
		cmd_inq.ExecuteReader();
		cmd_inq.Close();

		Log::Trace("", "", "666666");



		//  2  grid     ：    
		bcls_ret->Tables.Add("TABLE_7");
		Log::Trace("", "", "77777");

		sql = "SELECT  a.mat_no,value(hold_flag,' '),a.rest_roller_no as location,nvl(b.slab_cut_time,' ') as slab_cut_time,nvl(a.SLAB_DEST,' ') AS A,   "
			  "nvl(a.slab_print_mode,' ')AS B ,nvl(a.slab_burr_type,' ')AS C,nvl(b.prec_slab_no,  ' '),nvl(a.fix_flag,' '),nvl(a.qual_id,' ')as D,nvl(a.auto_wt_flag,' '),nvl(a.hot_send_flag,' ')  "
			  "from TWMM1 a left outer join(select * from tmmsm01 where mat_position in('1', '2')) b on a.mat_no = b.mat_no     "
			  "where REST_ROLLER_NO in('6')    ";
		Log::Info("", __FUNCTION__, "===sql==== [{0}]", sql);
		Db::QueryTable(sql, bcls_ret->Tables["TABLE_7"]);
		Log::Trace("", "", "77777");

		cmd_inq.SetCommandText(sql);
		cmd_inq.ExecuteReader();
		cmd_inq.Close();

		Log::Trace("", "", "77777");


		//  3  grid     ：    
		bcls_ret->Tables.Add("TABLE_8");
		Log::Trace("", "", "88888");

		sql = "SELECT  a.mat_no,value(hold_flag,' '),a.rest_roller_no as location,nvl(b.slab_cut_time,' ') as slab_cut_time,nvl(a.SLAB_DEST,' ') AS A,   "
			  "nvl(a.slab_print_mode,' ')AS B ,nvl(a.slab_burr_type,' ')AS C,nvl(b.prec_slab_no,  ' '),nvl(a.fix_flag,' '),nvl(a.qual_id,' ')as D,nvl(a.auto_wt_flag,' '),nvl(a.hot_send_flag,' ')  "
			  "from TWMM1 a left outer join(select * from tmmsm01 where mat_position in('1', '2')) b on a.mat_no = b.mat_no     "
			  "where REST_ROLLER_NO in('7')    ";
		Log::Info("", __FUNCTION__, "===sql==== [{0}]", sql);
		Db::QueryTable(sql, bcls_ret->Tables["TABLE_8"]);
		Log::Trace("", "", "88888");

		cmd_inq.SetCommandText(sql);
		cmd_inq.ExecuteReader();
		cmd_inq.Close();

		Log::Trace("", "", "88888");

		//  4  grid     ：    
		bcls_ret->Tables.Add("TABLE_9");
		Log::Trace("", "", "999999");

		sql = "SELECT  a.mat_no,value(hold_flag,' '),a.rest_roller_no as location,nvl(b.slab_cut_time,' ') as slab_cut_time,nvl(a.SLAB_DEST,' ') AS A,   "
			  "nvl(a.slab_print_mode,' ')AS B ,nvl(a.slab_burr_type,' ')AS C,nvl(b.prec_slab_no,  ' '),nvl(a.fix_flag,' '),nvl(a.qual_id,' ')as D,nvl(a.auto_wt_flag,' '),nvl(a.hot_send_flag,' ')  "
			  "from TWMM1 a left outer join(select * from tmmsm01 where mat_position in('1', '2')) b on a.mat_no = b.mat_no     "
			  "where REST_ROLLER_NO in('8')    ";
		Log::Info("", __FUNCTION__, "===sql==== [{0}]", sql);
		Db::QueryTable(sql, bcls_ret->Tables["TABLE_9"]);
		Log::Trace("", "", "999999");

		cmd_inq.SetCommandText(sql);
		cmd_inq.ExecuteReader();
		cmd_inq.Close();

		Log::Trace("", "", "999999");


		
		//  5 grid 
		bcls_ret->Tables.Add("TABLE_10");
		Log::Trace("", "", "1010101010");

		sql = "SELECT  a.mat_no,value(hold_flag,' '),a.rest_roller_no as location,nvl(b.slab_cut_time,' ') as slab_cut_time,nvl(a.SLAB_DEST,' ') AS A,   "
			  "nvl(a.slab_print_mode,' ')AS B ,nvl(a.slab_burr_type,' ')AS C,nvl(b.prec_slab_no,  ' '),nvl(a.fix_flag,' '),nvl(a.qual_id,' ')as D,nvl(a.auto_wt_flag,' '),nvl(a.hot_send_flag,' ')  "
			  "from TWMM1 a left outer join(select * from tmmsm01 where mat_position in('1', '2')) b on a.mat_no = b.mat_no  "
			  "where REST_ROLLER_NO like 'A%'   ";
		Log::Info("", __FUNCTION__, "===sql==== [{0}]", sql);
		
		Log::Trace("", "", "1010101010");

		cmd_inq.SetCommandText(sql);
		cmd_inq.ExecuteQuery(dt2);
		cmd_inq.Close();
		bcls_ret->Tables["TABLE_10"].Columns.Add(DT_STRING, "MAT_NO", "材料号");
		bcls_ret->Tables["TABLE_10"].Columns.Add(DT_STRING, "A", "A");
		bcls_ret->Tables["TABLE_10"].Columns.Add(DT_STRING, "B", "B");
		bcls_ret->Tables["TABLE_10"].Columns.Add(DT_STRING, "C", "C");
		bcls_ret->Tables["TABLE_10"].Columns.Add(DT_STRING, "D", "D");
		bcls_ret->Tables["TABLE_10"].Rows.Add();
		bcls_ret->Tables["TABLE_10"].Rows.Add();
		bcls_ret->Tables["TABLE_10"].Rows.Add();

		for (int i = 0; i < dt.Rows.get_Count();i++) {
			if (dt2.Rows[i]["location"].ToString() != "A1" && dt2.Rows[i]["location"].ToString() != "A2" && dt.Rows[i]["location"].ToString() != "A3") {

			}
			else
			{
				if (dt.Rows[i]["location"].ToString() == "A1") {
					bcls_ret->Tables["TABLE_10"].Rows[0]["MAT_NO"] = dt.Rows[i]["MAT_NO"].ToString();
					
				}
				if (dt.Rows[i]["location"].ToString() == "A2") {
					bcls_ret->Tables["TABLE_10"].Rows[1]["MAT_NO"] = dt.Rows[i]["MAT_NO"].ToString();

				}
				if (dt.Rows[i]["location"].ToString() == "A3") {
					bcls_ret->Tables["TABLE_10"].Rows[2]["MAT_NO"] = dt.Rows[i]["MAT_NO"].ToString();

				}
			}
		}
		Log::Trace("", "", "1010101010");
				


		//  6 grid 
		bcls_ret->Tables.Add("TABLE_11");
		Log::Trace("", "", "B1111111111");

		sql = "SELECT  a.mat_no,value(hold_flag,' '),a.rest_roller_no as location,nvl(b.slab_cut_time,' ') as slab_cut_time,nvl(a.SLAB_DEST,' ') AS A,   "
			  "nvl(a.slab_print_mode,' ')AS B ,nvl(a.slab_burr_type,' ')AS C,nvl(b.prec_slab_no,  ' '),nvl(a.fix_flag,' '),nvl(a.qual_id,' ')as D,nvl(a.auto_wt_flag,' '),nvl(a.hot_send_flag,' ')  "
			  "from TWMM1 a left outer join(select * from tmmsm01 where mat_position in('1', '2')) b on a.mat_no = b.mat_no  "
			  "where  REST_ROLLER_NO like 'B%'  ";
		Log::Info("", __FUNCTION__, "===sql==== [{0}]", sql);

		Log::Trace("", "", "B1111111111");

		cmd_inq.SetCommandText(sql);
		
		cmd_inq.ExecuteQuery(dt);
		Log::Trace("", __FUNCTION__, "SQL", sql);
		Log::Trace("", __FUNCTION__, "dt", dt.Rows.get_Count());
		cmd_inq.ExecuteReader();

		cmd_inq.Close();
		         bcls_ret->Tables["TABLE_11"].Columns.Add(DT_STRING, "MAT_NO", "材料号");
		         bcls_ret->Tables["TABLE_11"].Columns.Add(DT_STRING, "A", "A");
		         bcls_ret->Tables["TABLE_11"].Columns.Add(DT_STRING, "B", "B");
		         bcls_ret->Tables["TABLE_11"].Columns.Add(DT_STRING, "C", "C");
				 bcls_ret->Tables["TABLE_11"].Columns.Add(DT_STRING, "D", "D");
				 bcls_ret->Tables["TABLE_11"].Rows.Add();
				 bcls_ret->Tables["TABLE_11"].Rows.Add();
				 bcls_ret->Tables["TABLE_11"].Rows.Add();
			for (int count = 0; count < dt.Rows.get_Count();count++) {
				Log::Trace("", __FUNCTION__, "dt_Count1", dt.Rows[count]["location"].ToString());
				if (dt.Rows[count]["location"].ToString() != "B1"&& dt.Rows[count]["location"].ToString() != "B2" && dt.Rows[count]["location"].ToString() != "B3") {
					Log::Trace("", __FUNCTION__, "dt_Count22222222222", dt.Rows[count]["location"].ToString());

					//bcls_ret->Tables["TABLE_11"].Rows.Add();

				}
				else {
					
					Log::Trace("", __FUNCTION__, "dt_Count33333333333333333", dt.Rows[count]["location"].ToString());

					if (dt.Rows[count]["location"].ToString() == "B1") {
						bcls_ret->Tables["TABLE_11"].Rows[0]["MAT_NO"] = dt.Rows[count]["MAT_NO"].ToString();
						
					}
					if (dt.Rows[count]["location"].ToString() == "B2") {
						bcls_ret->Tables["TABLE_11"].Rows[1]["MAT_NO"] = dt.Rows[count]["MAT_NO"].ToString();

					}
					if (dt.Rows[count]["location"].ToString() == "B3") {
						bcls_ret->Tables["TABLE_11"].Rows[2]["MAT_NO"] = dt.Rows[count]["MAT_NO"].ToString();
						Log::Trace("", __FUNCTION__, "B3333", bcls_ret->Tables["TABLE_11"].Rows[2]["MAT_NO"].ToString());
					}
				}



				


			}
			



		Log::Trace("", "", "B1111111111");


		//7  grid 
		bcls_ret->Tables.Add("TABLE_12");
		Log::Trace("", "", "1212121212");

		sql = "SELECT  mat_no,value(hold_flag,' '),stock_place_no  as location,nvl(slab_cut_time,' ') as slab_cut_time,' ',' ',' ',nvl(prec_slab_no,  ' '),nvl(fix_flag,' '),' ',' ',' '  " 
			  "from tmmsm01       "
			  "where stock_place_no like '601' and mat_position in('1', '2') ";
		Log::Info("", __FUNCTION__, "===sql==== [{0}]", sql);
		Db::QueryTable(sql, bcls_ret->Tables["TABLE_12"]);
		Log::Trace("", "", "1212121212");

		cmd_inq.SetCommandText(sql);
		cmd_inq.ExecuteReader();
		cmd_inq.Close();

		Log::Trace("", "", "1212121212");


		// 12 grid 
		bcls_ret->Tables.Add("TABLE_13");
		Log::Trace("", "", "1313131313");

		sql = "SELECT  mat_no, value(hold_flag, ' '), stock_place_no  as location, nvl(slab_cut_time, ' ') as slab_cut_time, ' ', ' ', ' ', nvl(prec_slab_no, ' '), nvl(fix_flag, ' '), ' ', ' ', ' '  " 
			"from tmmsm01       "
			"where stock_place_no like '602' and mat_position in('1', '2') ";
		Log::Info("", __FUNCTION__, "===sql==== [{0}]", sql);
		Db::QueryTable(sql, bcls_ret->Tables["TABLE_13"]);
		Log::Trace("", "", "1313131313");

		cmd_inq.SetCommandText(sql);
		cmd_inq.ExecuteReader();
		cmd_inq.Close();

		Log::Trace("", "", "1313131313");

		//8   gird  热轧
		bcls_ret->Tables.Add("TABLE_14");
		Log::Trace("", "", "1414141414");

		sql = "SELECT  a.mat_no,value(hold_flag,' '),a.rest_roller_no as location,nvl(b.slab_cut_time,' ') as slab_cut_time,' ',' ',' ',  "
			"nvl(b.prec_slab_no,  ' '),nvl(a.fix_flag,' '),nvl(a.qual_id,' '),nvl(a.auto_wt_flag,' '),nvl(a.hot_send_flag,' ')  "
			"from TWMM1 a left outer join(select * from tmmsm01 where mat_position in('1', '2')) b on  a.mat_no = b.mat_no  "
			"where REST_ROLLER_NO = 'R1' "
			"order by slab_cut_time desc ";
		Log::Info("", __FUNCTION__, "===sql==== [{0}]", sql);
		Db::QueryTable(sql, bcls_ret->Tables["TABLE_14"]);
		Log::Trace("", "", "1414141414");

		cmd_inq.SetCommandText(sql);
		cmd_inq.ExecuteReader();
		cmd_inq.Close();

		Log::Trace("", "", "1414141414");




		// 9 gird  热轧
		bcls_ret->Tables.Add("TABLE_15");
		Log::Trace("", "", "1515151515");

		sql = " SELECT  a.mat_no,value(hold_flag,' '),a.rest_roller_no as location,nvl(b.slab_cut_time,' ') as slab_cut_time,' ',' ',' ',   "
			  "nvl(b.prec_slab_no,  ' '),nvl(a.fix_flag,' '),nvl(a.qual_id,' '),nvl(a.auto_wt_flag,' '),nvl(a.hot_send_flag,' ')    "
			  "from TWMM1 a left outer join(select * from tmmsm01 where mat_position in('1', '2')) b on  a.mat_no = b.mat_no   "
			  "where REST_ROLLER_NO = 'R2'   "
			  "order by slab_cut_time desc ";
		Log::Info("", __FUNCTION__, "===sql==== [{0}]", sql);
		Db::QueryTable(sql, bcls_ret->Tables["TABLE_15"]);
		Log::Trace("", "", "1515151515");

		cmd_inq.SetCommandText(sql);
		cmd_inq.ExecuteReader();
		cmd_inq.Close();

		Log::Trace("", "", "1515151515");


		
		//// --库位明细
		////LOGIC_STOCK_NO：区域号  STOCK_PLACE_NO 库位号  HEAT_NO：炉号   STOCK_NUM：材料数量  STOCK_PLACE_TYPE库位类型 STOCK_STATUS 库位状态 REMARK 库位说明备注

		//bcls_ret->Tables.Add("TABLE_4");
		//// G twmg10    E 一堆   H twm04  C TWM04    B  TMMBW01
		////sql = "SELECT G.*, E.*,H.LOGIC_STOCK_NO,H.STOCK_PLACE_TYPE, H.STOCK_STATUS, H.REMARK FROM TWMG10 G LEFT JOIN ( "
		////	" SELECT C.STOCK_PLACE_NO,B.HEAT_NO ,SUM(1) AS STOCK_NUM FROM TWMA2 A, TMMBW01 B, TWM04  C	"
		////	"  WHERE A.MAT_NO = B.MAT_NO AND A.STOCK_PLACE_NO = C.STOCK_PLACE_NO	AND C.STOCK_NO IN " + STOCK_NO +
		////	"  GROUP BY C.STOCK_PLACE_NO,B.HEAT_NO) E ON G.ITEM_CODE = E.STOCK_PLACE_NO	"
		////	" LEFT JOIN TWM04 H ON G.ITEM_CODE = H.STOCK_PLACE_NO WHERE G.FUNC_ID ='" + FUNC_ID + "'";

		// sql = "SELECT G.*,Z.*,H.LOGIC_STOCK_NO,H.STOCK_PLACE_TYPE, H.STOCK_STATUS AS STOCK_STATUS ,Z.HOLD_FLAG AS HOLD_FLAG, H.REMARK,E.STOCK_NUM FROM TWMG10 G "
		//	" LEFT JOIN (SELECT COUNT(1) AS STOCK_NUM,A.STOCK_PLACE_NO FROM TWMA2 A,TWM04 H WHERE A.STOCK_PLACE_NO =H.STOCK_PLACE_NO GROUP BY A.STOCK_PLACE_NO ) E ON G.ITEM_CODE =E.STOCK_PLACE_NO "
		//	" LEFT JOIN (SELECT B.* FROM TWM04 A,TMMSM01 B WHERE A.STOCK_PLACE_NO=B.STOCK_PLACE_NO ) Z ON G.ITEM_CODE =Z.STOCK_PLACE_NO "
		//	 " LEFT JOIN TWM04 H    "

		//	 "ON G.ITEM_CODE = H.STOCK_PLACE_NO WHERE G.FUNC_ID = '" + FUNC_ID + "'    " ;
		//Db::QueryTable(sql, bcls_ret->Tables["TABLE_4"]);
		//Log::Trace("", "", "444444");

		////材料明细
		//bcls_ret->Tables.Add("TABLE_5");
		//Log::Trace("", "", "3333333");
		//sql = "SELECT * FROM TMMSM01  WHERE STOCK_NO='" + STOCK_NO1 + "' ORDER BY STOCK_PLACE_NO,LAYERNO DESC";
		//Log::Info("", __FUNCTION__, "===sql==== [{0}]", sql);
		//Db::QueryTable(sql, bcls_ret->Tables["TABLE_5"]);
		//Log::Trace("", "", "3333333");


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

