/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:      JQ
Version:     1.1.1
Date:         2017-3-16
Description: 过跨台车选择
**************************************************/

/*
1.t
*/


/* C/C++ 的标准头文件部分 */
#include "stdafx.h"		// 框架头，不可删除 
#include "math.h"	

int f_wmsmsm_cranecmd_seq_upt(CString stock_oper_order, EIClass * bcls_ret, CDbConnection * conn);//更新顺序号
//int f_wmsmsm_u1dl03_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//发送命令

BM2_FUNCTION_EXPORT
int f_wmsmsm_cranecmd_tr_rem(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{

	//程序内部变量
	int doFlag = 0;
	int i = 0;
	CString sqlstr = " "; 
	CString sqlstr_1 = "2C";
	CDecimal maxseqno =9999999;

	//业务变量
	CDecimal max_wt = 0;                           //最大重量
	CDecimal max_height = 0;                       //最大高度
	CDecimal batch_no = 0;                         //批次号
	CDecimal batch_no_det = 0;                     //批次号
	int      mat_num = 4;                          //最大数量                        
	CString stock_place_no_L = " ";                //台车左边垛位号
	CString stock_place_no_R = " ";                //台车右边垛位号
	CString direction_flag = "L";                  
	CDecimal fit_flag = 0;                         //1-满足           
	CString tr_no = " ";      
	CString stock_place_no_to = " ";
	CString layerno = " ";
	CDecimal batch_no_rem = 0;
	int re_num = 0;                                //返回行数
	int seqno_1 = 0;

	EIClass bcls_rec_send;
	bcls_rec_send.Tables[0].set_TableName("U1DL03");
	bcls_rec_send.Tables[0].Columns.Add(DT_STRING, "mat_no");
	bcls_rec_send.Tables[0].Columns.Add(DT_STRING, "origin_mat_no");
	bcls_rec_send.Tables[0].Columns.Add(DT_STRING, "stock_oper_order");
	bcls_rec_send.Tables[0].Columns.Add(DT_DECIMAL, "cmd_seq");
	bcls_rec_send.Tables[0].Columns.Add(DT_DECIMAL, "crane_cmdgrpno");
	bcls_rec_send.Tables[0].Columns.Add(DT_DECIMAL, "batch_no");
	bcls_rec_send.Tables[0].Columns.Add(DT_STRING, "stock_place_no_from");
	bcls_rec_send.Tables[0].Columns.Add(DT_DECIMAL, "yard_layer_from");
	bcls_rec_send.Tables[0].Columns.Add(DT_STRING, "stock_place_no_to");
	bcls_rec_send.Tables[0].Columns.Add(DT_STRING, "stock_oper_order_fin");
	bcls_rec_send.Tables[0].Columns.Add(DT_STRING, "remark");

	EIClass bcls_rec_tr;
	bcls_rec_tr.Tables[0].set_TableName("CMD_TR_REM");
	bcls_rec_tr.Tables[0].Columns.Add(DT_STRING, "FLAG");
	bcls_rec_tr.Tables[0].Columns.Add(DT_STRING, "HALL_TO");
	bcls_rec_tr.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
	bcls_rec_tr.Tables[0].Columns.Add(DT_STRING, "HALL_FR");
	bcls_rec_tr.Tables[0].Columns.Add(DT_STRING, "LAYERNO");
	bcls_rec_tr.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO");
	bcls_rec_tr.Tables[0].Columns.Add(DT_STRING, "STOCK_OPER_ORDER_FIN");
	bcls_rec_tr.Tables[0].Columns.Add(DT_DECIMAL, "MAT_THEORY_WT");
	bcls_rec_tr.Tables[0].Columns.Add(DT_DECIMAL, "MAT_THICK");
	bcls_rec_tr.Tables[0].Columns.Add(DT_DECIMAL, "MAT_WIDTH");
	bcls_rec_tr.Tables[0].Columns.Add(DT_DECIMAL, "MAT_LEN");

	EIClass bcls_rec_cccc;

	//数据块
	CDataTable truck_tab; 
	CDataTable stock_tab;
	CDataTable batch_tab;
	CDataTable truck_place; 
	CDataTable cmd_mat;                            //存放命令材料
	CDataTable stock_mat;                          //库位材料
	CDataTable batch_mat;                          //每个批次的重量、高度
	CDataTable Table_car;

	//定义表实体对象
	CModel twma7 = CModel("TWMA7");
	CModel twm04 = CModel("TWM04");

	try
	{
		//项目自定义日志
		CTracer log(__FUNCTION__);
		Log::Trace("", __FUNCTION__, "BEGIN");
		bcls_ret->Tables[0].Columns.Clear();
		bcls_ret->Tables[0].Rows.Clear();
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "BATCH_NO");
	
		//检验传入数据块
		if (!bcls_rec->Tables.Contains("CMD_TR_REM"))
		{
			sprintf(s.msg, "函数f_wmsmsm_cranecmd_tr_rem中找不到接收块名[CMD_TR_REM]");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		for (int i = 0; i < bcls_rec->Tables["CMD_TR_REM"].Rows.get_Count(); i++)
		{
			//数据打印
			Log::Trace("", __FUNCTION__, "hall_to[{0}]",         bcls_rec->Tables["CMD_TR_REM"].Rows[i]["HALL_TO"].ToString());
			Log::Trace("", __FUNCTION__, "hall_fr[{0}]",         bcls_rec->Tables["CMD_TR_REM"].Rows[i]["HALL_FR"].ToString());
			Log::Trace("", __FUNCTION__, "mat_no[{0}]",          bcls_rec->Tables["CMD_TR_REM"].Rows[i]["MAT_NO"].ToString().Trim());
			Log::Trace("", __FUNCTION__, "stock_place_no[{0}]",  bcls_rec->Tables["CMD_TR_REM"].Rows[i]["STOCK_PLACE_NO"].ToString().Trim());
			Log::Trace("", __FUNCTION__, "layerno[{0}]",          bcls_rec->Tables["CMD_TR_REM"].Rows[i]["LAYERNO"].ToString().Trim());
			Log::Trace("", __FUNCTION__, "stock_oper_order[{0}]",bcls_rec->Tables["CMD_TR_REM"].Rows[i]["STOCK_OPER_ORDER_FIN"].ToString().Trim());
			Log::Trace("", __FUNCTION__, "mat_wt[{0}]",          bcls_rec->Tables["CMD_TR_REM"].Rows[i]["MAT_THEORY_WT"].ToDecimal());
			Log::Trace("", __FUNCTION__, "mat_height[{0}]",      bcls_rec->Tables["CMD_TR_REM"].Rows[i]["MAT_THICK"].ToDecimal());
			Log::Trace("", __FUNCTION__, "mat_width[{0}]",       bcls_rec->Tables["CMD_TR_REM"].Rows[i]["MAT_WIDTH"].ToDecimal());
			Log::Trace("", __FUNCTION__, "mat_len[{0}]",         bcls_rec->Tables["CMD_TR_REM"].Rows[i]["MAT_LEN"].ToDecimal());

			layerno = bcls_rec->Tables["CMD_TR_REM"].Rows[i]["LAYERNO"].ToString().Trim();

			//获取台车属性
			twm04["STOCK_PLACE_NO"] = bcls_rec->Tables["CMD_TR_REM"].Rows[i]["STOCK_PLACE_NO"].ToString().Trim();
			twm04.Query("STOCK_PLACE_NO");
			//sqlstr = "SELECT TR_NO FROM TWM04 WHERE HALL_NO='" + twm04["STOCK_PLACE_NO"].ToString().Trim() + "'";
			Log::Trace("", __FUNCTION__, "sqlstr[{0}]", sqlstr);
			twma7["MAT_NO"] = bcls_rec->Tables["CMD_TR_REM"].Rows[i]["MAT_NO"].ToString().Trim();
			Log::Trace("", __FUNCTION__, "sql3str[{0}]", sqlstr);

			twma7.Query("MAT_NO");
			CString tr_no = twm04["TR_NO"].ToString().Trim();
			Log::Trace("", __FUNCTION__, "sqlst13r[{0}]", twma7["MAT_NO"].ToString().Trim());
			twma7["CRANE_NO"] = tr_no;
			Log::Trace("", __FUNCTION__, "sqlst12[{0}]", sqlstr);
			twma7.Update("CRANE_NO","MAT_NO");
		

		//	//获取垛位属性
		//	sqlstr = "SELECT COLUMN_NO,ROWNO,LOGIC_STOCK_NO FROM TWM04 WHERE STOCK_PLACE_NO='" + bcls_rec->Tables["CMD_TR_REM"].Rows[i]["STOCK_PLACE_NO"].ToString().Trim() + "'";
		//	Db::QueryTable(sqlstr, stock_tab);
		//	tr_no = " ";
		//	int m = 0;

		//	if ((bcls_rec->Tables["CMD_TR_REM"].Rows[i]["HALL_TO"].ToString().Trim() == "F" && bcls_rec->Tables["CMD_TR_REM"].Rows[i]["HALL_FR"].ToString().Trim() == "G")
		//		||(bcls_rec->Tables["CMD_TR_REM"].Rows[i]["HALL_TO"].ToString().Trim() == "G" && bcls_rec->Tables["CMD_TR_REM"].Rows[i]["HALL_FR"].ToString().Trim() == "F"))
		//	{
		//		//推荐使用台车（根据优先级依次判断，都不满足用最后一个台车）
		//		
		//		Log::Trace("", __FUNCTION__, "get_Count[{0}]", truck_tab.Rows.get_Count());
		//		for (; m < truck_tab.Rows.get_Count(); m++)
		//		{
		//			if (m == truck_tab.Rows.get_Count() - 1)
		//			{
		//				tr_no = truck_tab.Rows[m]["TR_NO"].ToString();
		//				break;
		//			}
		//			Log::Trace("", __FUNCTION__, "MAX_LEN[{0}]", truck_tab.Rows[m]["MAX_LEN"].ToDouble());
		//			Log::Trace("", __FUNCTION__, "CMD_TR_REM[{0}]", bcls_rec->Tables["CMD_TR_REM"].Rows[i]["MAT_LEN"].ToDouble());
		//			Log::Trace("", __FUNCTION__, "MAX_WIDTH[{0}]", truck_tab.Rows[m]["MAX_WIDTH"].ToDouble());
		//			Log::Trace("", __FUNCTION__, "CMD_TR_REM[{0}]", bcls_rec->Tables["CMD_TR_REM"].Rows[i]["MAT_WIDTH"].ToDouble());
		//			if (truck_tab.Rows[m]["MAX_LEN"].ToDouble() >= bcls_rec->Tables["CMD_TR_REM"].Rows[i]["MAT_LEN"].ToDouble()
		//				&& truck_tab.Rows[m]["MAX_WIDTH"].ToDouble() >= bcls_rec->Tables["CMD_TR_REM"].Rows[i]["MAT_WIDTH"].ToDouble())
		//			{
		//				sqlstr = " SELECT COUNT(1) FROM TWM09 a  "
		//					" WHERE  "
		//					//" a.HALL_NO_FR = '" + bcls_rec->Tables["CMD_TR_REM"].Rows[i]["HALL_FR"].ToString() 
		//					// + "' and  a.HALL_NO_TO = '" + bcls_rec->Tables["CMD_TR_REM"].Rows[i]["HALL_TO"].ToString() 
		//					"a.COLUMN_FROM<='" + stock_tab.Rows[0]["COLUMN_NO"].ToString()
		//					+ "' and  a.COLUMN_TO >='" + stock_tab.Rows[0]["COLUMN_NO"].ToString()
		//					+ "' and  a.ROW_FROM<='" + stock_tab.Rows[0]["ROWNO"].ToString()
		//					+ "' and  a.ROW_TO>= '" + stock_tab.Rows[0]["ROWNO"].ToString()
		//					+ "' and  a.TR_NO = '" + truck_tab.Rows[m]["TR_NO"].ToString() + "'";
		//				Log::Trace("", __FUNCTION__, "sqlstr[{0}]", sqlstr);
		//				if (Db::QueryCDecimal(sqlstr)>0)
		//				{							
		//					break;
		//				}
		//				continue;

		//			}
		//			else
		//			{
		//				continue;
		//			}

		//		}
		//	}
		//	else
		//	{
		//		m = truck_tab.Rows.get_Count() - 1;
		//	}

		//	tr_no = truck_tab.Rows[m]["TR_NO"].ToString().Trim();
		//	Log::Trace("", __FUNCTION__, "tr_to[{0}]", tr_no);
		//	
		//	//查找台车的2个垛位
		//	sqlstr = "select stock_place_no from twm04 "
		//	         " where tr_no='" + tr_no + "'"
		//			 " and hall_no='" + bcls_rec->Tables["CMD_TR_REM"].Rows[i]["HALL_FR"].ToString() + "'"
		//			 " order by stock_place_no";
		//	Log::Trace("", __FUNCTION__, "sqlstr[{0}]", sqlstr);
		//	Db::QueryTable(sqlstr, truck_place);
		//	if (truck_place.Rows.get_Count() != 2)
		//	{
		//		sprintf(s.msg, "查找台车垛位出错");
		//		throw CApplicationException(-1, s.msg, log.Location);
		//	}
		//	stock_place_no_L = truck_place.Rows[0]["STOCK_PLACE_NO"];
		//	stock_place_no_R = truck_place.Rows[1]["STOCK_PLACE_NO"];

		//	if (stock_tab.Rows[0]["LOGIC_STOCK_NO"].ToString()=="07")
		//	{
		//		Log::Trace("", __FUNCTION__, "临时卸料区单独推荐");

		//		//获取吊车属性
		//		sqlstr = " select CRA_NUM_MAX,CRA_WEI_MAX,CRA_DEEPTH,CRA_THI_DIFF,CRA_WID_DIFF from twm06 where CRANE_NO='1C01'";
		//		Db::QueryTable(sqlstr, Table_car);
		//		if (Table_car.Rows.get_Count() == 0)
		//		{
		//			sprintf(s.msg, "台车属性不存在");
		//			throw CApplicationException(-1, s.msg, log.Location);
		//		}

		//		//如果该跺位已有过跨批次则用该批次
		//		sqlstr = " SELECT a.BATCH_NO, a.STOCK_PLACE_NO_TO, b.SUM_THICK, c.SUM_WT,b.SUM_WT_B ,b.SUM ,MAX(a.MAT_ACT_LEN) AS MAX_ACT_LEN,MIN(a.MAT_ACT_LEN) AS MIN_ACT_LEN,MAX(a.MAT_ACT_WIDTH) AS MAX_ACT_WIDTH,MIN(a.MAT_ACT_WIDTH) AS MIN_ACT_WIDTH "
		//			" FROM TWMA7 a"
		//			" LEFT JOIN(SELECT BATCH_NO, STOCK_PLACE_NO_TO, SUM(MAT_ACT_THICK) AS SUM_THICK, COUNT(1) AS SUM,SUM(MAT_ACT_WT) AS SUM_WT_B FROM TWMA7 GROUP BY BATCH_NO, STOCK_PLACE_NO_TO) b ON a.BATCH_NO = b.BATCH_NO AND a.STOCK_PLACE_NO_TO = b.STOCK_PLACE_NO_TO"
		//			" LEFT JOIN(SELECT BATCH_NO, SUM(MAT_ACT_WT) AS SUM_WT FROM  TWMA7 GROUP BY BATCH_NO) c ON a.BATCH_NO = c.BATCH_NO"
		//			" where a.STOCK_PLACE_NO_FROM = '" + bcls_rec->Tables["CMD_TR_REM"].Rows[i]["STOCK_PLACE_NO"].ToString().Trim() + "'"
		//			" AND a.STOCK_PLACE_NO_TO LIKE '" + tr_no + "%'"
		//			" GROUP BY a.BATCH_NO, a.STOCK_PLACE_NO_TO, b.SUM_THICK, c.SUM_WT, b.SUM,b.SUM_WT_B";
		//		Log::Trace("", __FUNCTION__, "sqlstr[{0}]", sqlstr);
		//		Db::QueryTable(sqlstr, batch_mat);

		//		//遍历满足条件的批次号
		//		batch_no = 0;
		//		fit_flag = 0;
		//		stock_place_no_to = "";
		//		for (int t = 0; t < batch_mat.Rows.get_Count(); t++)
		//		{
		//			if (truck_tab.Rows[m]["MAX_HEIGHT"].ToDouble() >= batch_mat.Rows[t]["SUM_THICK"].ToDouble() + bcls_rec->Tables["CMD_TR_REM"].Rows[i]["MAT_THICK"].ToDouble()
		//				&& truck_tab.Rows[m]["MAX_WT"].ToDouble() >= batch_mat.Rows[t]["SUM_WT"].ToDouble() + bcls_rec->Tables["CMD_TR_REM"].Rows[i]["MAT_THEORY_WT"].ToDouble()
		//				&& mat_num > batch_mat.Rows[t]["SUM"].ToDecimal()
		//				&& truck_tab.Rows[m]["LEN_DIFF_MAX"].ToDouble() >= fabs(bcls_rec->Tables["CMD_TR_REM"].Rows[i]["MAT_LEN"].ToDouble() - batch_mat.Rows[t]["MAX_ACT_LEN"].ToDouble())
		//				&& truck_tab.Rows[m]["LEN_DIFF_MAX"].ToDouble() >= fabs(bcls_rec->Tables["CMD_TR_REM"].Rows[i]["MAT_LEN"].ToDouble() - batch_mat.Rows[t]["MIN_ACT_LEN"].ToDouble())
		//				&& truck_tab.Rows[m]["WID_DIFF_MAX"].ToDouble() >= fabs(bcls_rec->Tables["CMD_TR_REM"].Rows[i]["MAT_WIDTH"].ToDouble() - batch_mat.Rows[t]["MAX_ACT_WIDTH"].ToDouble())
		//				&& truck_tab.Rows[m]["WID_DIFF_MAX"].ToDouble() >= fabs(bcls_rec->Tables["CMD_TR_REM"].Rows[i]["MAT_WIDTH"].ToDouble() - batch_mat.Rows[t]["MIN_ACT_WIDTH"].ToDouble()))
		//			{
		//				Log::Trace("", __FUNCTION__, "SUM_WT_B{0}", batch_mat.Rows[t]["SUM_WT_B"].ToDecimal());
		//				Log::Trace("", __FUNCTION__, "CRA_WEI_MAX{0}", Table_car.Rows[0]["CRA_WEI_MAX"].ToDecimal());
		//				Log::Trace("", __FUNCTION__, "CMD_TR_REM{0}", bcls_rec->Tables["CMD_TR_REM"].Rows[i]["MAT_THEORY_WT"].ToDecimal());
		//				if (batch_mat.Rows[t]["SUM_WT_B"].ToDouble() + bcls_rec->Tables["CMD_TR_REM"].Rows[i]["MAT_THEORY_WT"].ToDouble()<=Table_car.Rows[0]["CRA_WEI_MAX"].ToDouble())
		//				{
		//					batch_no = batch_mat.Rows[t]["BATCH_NO"].ToDecimal();
		//					stock_place_no_to = batch_mat.Rows[t]["STOCK_PLACE_NO_TO"].ToString();
		//					fit_flag = 1;
		//					break;
		//				}						
		//			}

		//			//if (t == batch_mat.Rows.get_Count() - 1)
		//			//{
		//			//	if (batch_mat.Rows[t]["STOCK_PLACE_NO_TO"].ToString() == stock_place_no_L)
		//			//	{
		//			//		batch_no = batch_mat.Rows[t]["BATCH_NO"].ToDecimal();
		//			//		stock_place_no_to = stock_place_no_R;
		//			//		fit_flag = 1;
		//			//		break;
		//			//	}
		//			//}
		//		}

		//		if (batch_no == 0 )
		//		{
		//			//查询目标跺位只有一个的过跨批次
		//			sqlstr = " SELECT a.BATCH_NO, a.STOCK_PLACE_NO_TO, b.SUM_THICK, c.SUM_WT, b.SUM, MAX(a.MAT_ACT_LEN) AS MAX_ACT_LEN, MIN(a.MAT_ACT_LEN) AS MIN_ACT_LEN, MAX(a.MAT_ACT_WIDTH) AS MAX_ACT_WIDTH, MIN(a.MAT_ACT_WIDTH) AS MIN_ACT_WIDTH"
		//				" FROM TWMA7 a"
		//				" LEFT JOIN(SELECT BATCH_NO, STOCK_PLACE_NO_TO, SUM(MAT_ACT_THICK) AS SUM_THICK, COUNT(1) AS SUM FROM TWMA7 GROUP BY BATCH_NO, STOCK_PLACE_NO_TO) b ON a.BATCH_NO = b.BATCH_NO AND a.STOCK_PLACE_NO_TO = b.STOCK_PLACE_NO_TO"
		//				" LEFT JOIN(SELECT BATCH_NO, SUM(MAT_ACT_WT) AS SUM_WT FROM  TWMA7 GROUP BY BATCH_NO) c ON a.BATCH_NO = c.BATCH_NO"
		//				" WHERE  a.HALL_NO_TO = '" + bcls_rec->Tables["CMD_TR_REM"].Rows[i]["HALL_TO"].ToString() + "'"
		//				" AND a.HALL_NO_FR = '" + bcls_rec->Tables["CMD_TR_REM"].Rows[i]["HALL_FR"].ToString() + "'"
		//				" AND a.BATCH_NO != " + truck_tab.Rows[0]["BATCH_TASK_NO"].ToString() +
		//				" AND a.BATCH_NO != " + truck_tab.Rows[1]["BATCH_TASK_NO"].ToString() +
		//				" AND a.BATCH_NO !=0"
		//				" AND a.STOCK_PLACE_NO_TO ='" + stock_place_no_L + "'"
		//				" AND a.STOCK_PLACE_NO_FROM NOT LIKE 'YS%'"
		//				" AND EXISTS(SELECT * FROM("
		//				" SELECT BATCH_NO, COUNT(1) AS COUNT FROM("
		//				" SELECT DISTINCT BATCH_NO, STOCK_PLACE_NO_TO FROM TWMA7"
		//				" WHERE  STOCK_PLACE_NO_TO LIKE '" + tr_no + "%')"
		//				" GROUP BY BATCH_NO)d WHERE COUNT = 1 and d.BATCH_NO = a.BATCH_NO)"
		//				" GROUP BY a.BATCH_NO, a.STOCK_PLACE_NO_TO, b.SUM_THICK, c.SUM_WT, b.SUM"
		//				" ORDER BY a.BATCH_NO, a.STOCK_PLACE_NO_TO";
		//			Log::Trace("", __FUNCTION__, "sqlstr[{0}]", sqlstr);
		//			Db::QueryTable(sqlstr, batch_mat);
		//			Log::Trace("", __FUNCTION__, "get_Count[{0}]", batch_mat.Rows.get_Count());
		//			if (batch_mat.Rows.get_Count() > 0)
		//			{
		//				batch_no = batch_mat.Rows[0]["BATCH_NO"].ToDecimal();
		//				stock_place_no_to = stock_place_no_R;
		//			}

		//		}

		//		if (batch_no == 0)
		//		{
		//			sqlstr = "values nextval for SEQ_" + sqlstr_1;
		//			batch_no = Db::QueryCDecimal(sqlstr);
		//			if (batch_no > maxseqno)
		//			{
		//				doFlag = f_wmsmsm_cranecmd_seq_upt(sqlstr_1, bcls_ret, conn);
		//				if (doFlag != 0)
		//				{
		//					throw CApplicationException(-1, s.msg, log.Location);
		//				}
		//				batch_no = Db::QueryCDecimal(sqlstr);
		//			}
		//			stock_place_no_to = stock_place_no_L;
		//		}

		//		bcls_ret->Tables[0].Rows.Add();
		//		bcls_ret->Tables[0].Rows[re_num]["STOCK_PLACE_NO"] = stock_place_no_to;
		//		bcls_ret->Tables[0].Rows[re_num]["BATCH_NO"] = batch_no;
		//		re_num++;

		//		Log::Trace("", __FUNCTION__, "BATCH_NO {0}", batch_no);
		//		Log::Trace("", __FUNCTION__, "STOCK_PLACE_NO {0}", stock_place_no_to);

		//	}
		//	else
		//	{
		//		//获取上层最大批次号
		//		sqlstr = "select max(BATCH_NO) from twma7 where STOCK_OPER_ORDER='32' AND STOCK_PLACE_NO_FROM='" + bcls_rec->Tables["CMD_TR_REM"].Rows[i]["STOCK_PLACE_NO"].ToString().Trim() + "' and int(YARD_LAYER_FROM)>INT('" + layerno + "') ";
		//		CDecimal max_batch_no = Db::QueryCDecimal(sqlstr);

		//		//获取下层最小批次号
		//		sqlstr = "select min(BATCH_NO) from twma7 where STOCK_OPER_ORDER='32' AND STOCK_PLACE_NO_FROM='" + bcls_rec->Tables["CMD_TR_REM"].Rows[i]["STOCK_PLACE_NO"].ToString().Trim() + "' and int(YARD_LAYER_FROM)<INT('" + layerno + "') ";
		//		CDecimal min_batch_no = Db::QueryCDecimal(sqlstr);

		//		//获取批次信息
		//		CString sqlstr_all = "SELECT a.BATCH_NO, a.STOCK_PLACE_NO_TO, b.SUM_THICK, c.SUM_WT ,b.SUM ,MAX(a.MAT_ACT_LEN) AS MAX_ACT_LEN,MIN(a.MAT_ACT_LEN) AS MIN_ACT_LEN,MAX(a.MAT_ACT_WIDTH) AS MAX_ACT_WIDTH,MIN(a.MAT_ACT_WIDTH) AS MIN_ACT_WIDTH "
		//			" FROM TWMA7 a"
		//			" LEFT JOIN(SELECT BATCH_NO, STOCK_PLACE_NO_TO, SUM(MAT_ACT_THICK) AS SUM_THICK, COUNT(1) AS SUM FROM TWMA7 GROUP BY BATCH_NO, STOCK_PLACE_NO_TO) b ON a.BATCH_NO = b.BATCH_NO AND a.STOCK_PLACE_NO_TO = b.STOCK_PLACE_NO_TO"
		//			" LEFT JOIN(SELECT BATCH_NO, SUM(MAT_ACT_WT) AS SUM_WT FROM  TWMA7 GROUP BY BATCH_NO) c ON a.BATCH_NO = c.BATCH_NO"
		//			" WHERE  a.HALL_NO_TO = '" + bcls_rec->Tables["CMD_TR_REM"].Rows[i]["HALL_TO"].ToString() + "'"
		//			" AND a.HALL_NO_FR = '" + bcls_rec->Tables["CMD_TR_REM"].Rows[i]["HALL_FR"].ToString() + "'"
		//			" AND a.BATCH_NO >=" + max_batch_no.ToString() +
		//			" AND a.BATCH_NO != " + truck_tab.Rows[0]["BATCH_TASK_NO"].ToString() +
		//			" AND a.BATCH_NO != " + truck_tab.Rows[1]["BATCH_TASK_NO"].ToString() +
		//			" AND a.BATCH_NO !=0"
		//			" AND a.STOCK_PLACE_NO_TO LIKE '" + tr_no + "%'"
		//			" AND a.STOCK_PLACE_NO_FROM NOT LIKE 'YS%'";

		//		if (min_batch_no > 0)sqlstr_all = sqlstr_all + " AND a.BATCH_NO <=" + min_batch_no.ToString();
		//		sqlstr_all = sqlstr_all + " GROUP BY a.BATCH_NO, a.STOCK_PLACE_NO_TO, b.SUM_THICK, c.SUM_WT, b.SUM"
		//			+ " ORDER BY a.BATCH_NO, a.STOCK_PLACE_NO_TO";
		//		Log::Trace("", __FUNCTION__, "sqlstr_all[{0}]", sqlstr_all);
		//		Db::QueryTable(sqlstr_all, batch_mat);

		//		//遍历满足条件的批次号
		//		batch_no = 0;
		//		fit_flag = 0;
		//		stock_place_no_to = "";
		//		for (int t = 0; t < batch_mat.Rows.get_Count(); t++)
		//		{
		//			Log::Trace("", __FUNCTION__, "BATCH_NO[{0}]", batch_mat.Rows[t]["BATCH_NO"].ToDecimal());
		//			Log::Trace("", __FUNCTION__, "STOCK_PLACE_NO_TO[{0}]", batch_mat.Rows[t]["STOCK_PLACE_NO_TO"].ToString());
		//			Log::Trace("", __FUNCTION__, "SUM[{0}]", batch_mat.Rows[t]["SUM"].ToDecimal());
		//			Log::Trace("", __FUNCTION__, "SUM_THICK[{0}]", batch_mat.Rows[t]["SUM_THICK"].ToDecimal());
		//			if (truck_tab.Rows[m]["MAX_HEIGHT"].ToDouble() >= batch_mat.Rows[t]["SUM_THICK"].ToDouble() + bcls_rec->Tables["CMD_TR_REM"].Rows[i]["MAT_THICK"].ToDouble()
		//				&& truck_tab.Rows[m]["MAX_WT"].ToDouble() >= batch_mat.Rows[t]["SUM_WT"].ToDouble() + bcls_rec->Tables["CMD_TR_REM"].Rows[i]["MAT_THEORY_WT"].ToDouble()
		//				&& mat_num > batch_mat.Rows[t]["SUM"].ToDecimal()
		//				&& truck_tab.Rows[m]["LEN_DIFF_MAX"].ToDouble() >= fabs(bcls_rec->Tables["CMD_TR_REM"].Rows[i]["MAT_LEN"].ToDouble() - batch_mat.Rows[t]["MAX_ACT_LEN"].ToDouble())
		//				&& truck_tab.Rows[m]["LEN_DIFF_MAX"].ToDouble() >= fabs(bcls_rec->Tables["CMD_TR_REM"].Rows[i]["MAT_LEN"].ToDouble() - batch_mat.Rows[t]["MIN_ACT_LEN"].ToDouble())
		//				&& truck_tab.Rows[m]["WID_DIFF_MAX"].ToDouble() >= fabs(bcls_rec->Tables["CMD_TR_REM"].Rows[i]["MAT_WIDTH"].ToDouble() - batch_mat.Rows[t]["MAX_ACT_WIDTH"].ToDouble())
		//				&& truck_tab.Rows[m]["WID_DIFF_MAX"].ToDouble() >= fabs(bcls_rec->Tables["CMD_TR_REM"].Rows[i]["MAT_WIDTH"].ToDouble() - batch_mat.Rows[t]["MIN_ACT_WIDTH"].ToDouble()))
		//			{
		//				batch_no = batch_mat.Rows[t]["BATCH_NO"].ToDecimal();
		//				stock_place_no_to = batch_mat.Rows[t]["STOCK_PLACE_NO_TO"].ToString();
		//				fit_flag = 1;
		//				break;
		//			}
		//		}

		//		Log::Trace("", __FUNCTION__, "fit_flag[{0}]", fit_flag);
		//		if (fit_flag == 1)
		//		{
		//			if (bcls_rec->Tables["CMD_TR_REM"].Columns.Contains("FLAG")
		//				&& bcls_rec->Tables["CMD_TR_REM"].Rows[i]["FLAG"].ToString() == "1")
		//			{
		//				twma7.Reset();
		//				twma7["MAT_NO"] = bcls_rec->Tables["CMD_TR_REM"].Rows[i]["MAT_NO"].ToString().Trim();
		//				twma7.Query("MAT_NO");
		//				twma7["BATCH_NO"] = batch_no;
		//				twma7["STOCK_PLACE_NO_TO"] = stock_place_no_to;
		//				twma7.Update("BATCH_NO,STOCK_PLACE_NO_TO", "MAT_NO");
		//				bcls_rec_send.Tables[0].Rows.Add();
		//				bcls_rec_send.Tables[0].Rows[seqno_1]["mat_no"] = twma7["MAT_NO"].ToString();
		//				bcls_rec_send.Tables[0].Rows[seqno_1]["stock_oper_order"] = twma7["STOCK_OPER_ORDER"].ToString().Trim();
		//				bcls_rec_send.Tables[0].Rows[seqno_1]["crane_cmdgrpno"] = 0;
		//				bcls_rec_send.Tables[0].Rows[seqno_1]["stock_place_no_from"] = twma7["STOCK_PLACE_NO_FROM"].ToString().Trim();
		//				bcls_rec_send.Tables[0].Rows[seqno_1]["yard_layer_from"] = twma7["YARD_LAYER_FROM"].ToString().Trim();
		//				bcls_rec_send.Tables[0].Rows[seqno_1]["stock_place_no_to"] = twma7["STOCK_PLACE_NO_TO"].ToString();
		//				bcls_rec_send.Tables[0].Rows[seqno_1]["remark"] = "update";
		//				bcls_rec_send.Tables[0].Rows[seqno_1]["stock_oper_order_fin"] = twma7["STOCK_OPER_ORDER_FIN"].ToString();
		//				bcls_rec_send.Tables[0].Rows[seqno_1]["batch_no"] = twma7["BATCH_NO"].ToDecimal();
		//				bcls_rec_send.Tables[0].Rows[seqno_1]["cmd_seq"] = twma7["CMD_SEQ"].ToDecimal();
		//				seqno_1++;
		//			}
		//			else
		//			{
		//				bcls_ret->Tables[0].Rows.Add();
		//				bcls_ret->Tables[0].Rows[re_num]["STOCK_PLACE_NO"] = stock_place_no_to;
		//				bcls_ret->Tables[0].Rows[re_num]["BATCH_NO"] = batch_no;
		//				re_num++;
		//				twma7["MAT_NO"] = bcls_rec->Tables["CMD_TR_REM"].Rows[i]["MAT_NO"].ToString().Trim();
		//				twma7["BATCH_NO"] = batch_no;
		//				twma7["STOCK_PLACE_NO_TO"] = stock_place_no_to;
		//				twma7.Update("BATCH_NO,STOCK_PLACE_NO_TO", "MAT_NO");
		//			}
		//		}
		//		else
		//		{
		//			Log::Trace("", __FUNCTION__, "min_batch_no[{0}]", min_batch_no);
		//			if (min_batch_no == 0)
		//			{
		//				Log::Trace("", __FUNCTION__, "stock_place_no_L[{0}]", stock_place_no_L);
		//				Log::Trace("", __FUNCTION__, "get_Count[{0}]", batch_mat.Rows.get_Count());
		//				if (batch_mat.Rows.get_Count()>0
		//					&& batch_mat.Rows[batch_mat.Rows.get_Count() - 1]["STOCK_PLACE_NO_TO"].ToString().Trim() == stock_place_no_L)
		//				{
		//					stock_place_no_to = stock_place_no_R;
		//					batch_no = batch_mat.Rows[batch_mat.Rows.get_Count() - 1]["BATCH_NO"].ToDecimal();
		//					if (bcls_rec->Tables["CMD_TR_REM"].Columns.Contains("FLAG")
		//						&& bcls_rec->Tables["CMD_TR_REM"].Rows[i]["FLAG"].ToString() == "1")
		//					{
		//						twma7.Reset();
		//						twma7["MAT_NO"] = bcls_rec->Tables["CMD_TR_REM"].Rows[i]["MAT_NO"].ToString().Trim();
		//						twma7.Query("MAT_NO");
		//						twma7["BATCH_NO"] = batch_no;
		//						twma7["STOCK_PLACE_NO_TO"] = stock_place_no_to;
		//						twma7.Update("BATCH_NO,STOCK_PLACE_NO_TO", "MAT_NO");
		//						bcls_rec_send.Tables[0].Rows.Add();
		//						bcls_rec_send.Tables[0].Rows[seqno_1]["mat_no"] = twma7["MAT_NO"].ToString();
		//						bcls_rec_send.Tables[0].Rows[seqno_1]["stock_oper_order"] = twma7["STOCK_OPER_ORDER"].ToString().Trim();
		//						bcls_rec_send.Tables[0].Rows[seqno_1]["crane_cmdgrpno"] = 0;
		//						bcls_rec_send.Tables[0].Rows[seqno_1]["stock_place_no_from"] = twma7["STOCK_PLACE_NO_FROM"].ToString().Trim();
		//						bcls_rec_send.Tables[0].Rows[seqno_1]["yard_layer_from"] = twma7["YARD_LAYER_FROM"].ToString().Trim();
		//						bcls_rec_send.Tables[0].Rows[seqno_1]["stock_place_no_to"] = twma7["STOCK_PLACE_NO_TO"].ToString();
		//						bcls_rec_send.Tables[0].Rows[seqno_1]["remark"] = "update";
		//						bcls_rec_send.Tables[0].Rows[seqno_1]["stock_oper_order_fin"] = twma7["STOCK_OPER_ORDER_FIN"].ToString();
		//						bcls_rec_send.Tables[0].Rows[seqno_1]["batch_no"] = twma7["BATCH_NO"].ToDecimal();
		//						bcls_rec_send.Tables[0].Rows[seqno_1]["cmd_seq"] = twma7["CMD_SEQ"].ToDecimal();
		//						seqno_1++;
		//					}
		//					else
		//					{
		//						bcls_ret->Tables[0].Rows.Add();
		//						bcls_ret->Tables[0].Rows[re_num]["STOCK_PLACE_NO"] = stock_place_no_to;
		//						bcls_ret->Tables[0].Rows[re_num]["BATCH_NO"] = batch_no;
		//						re_num++;
		//						twma7["MAT_NO"] = bcls_rec->Tables["CMD_TR_REM"].Rows[i]["MAT_NO"].ToString().Trim();
		//						twma7["BATCH_NO"] = batch_no;
		//						twma7["STOCK_PLACE_NO_TO"] = stock_place_no_to;
		//						twma7.Update("BATCH_NO,STOCK_PLACE_NO_TO", "MAT_NO");
		//					}
		//				}
		//				else
		//				{
		//					sqlstr = "values nextval for SEQ_" + sqlstr_1;
		//					batch_no = Db::QueryCDecimal(sqlstr);
		//					if (batch_no > maxseqno)
		//					{
		//						doFlag = f_wmsmsm_cranecmd_seq_upt(sqlstr_1, bcls_ret, conn);
		//						if (doFlag != 0)
		//						{
		//							throw CApplicationException(-1, s.msg, log.Location);
		//						}
		//						batch_no = Db::QueryCDecimal(sqlstr);
		//					}
		//					stock_place_no_to = stock_place_no_L;
		//					if (bcls_rec->Tables["CMD_TR_REM"].Columns.Contains("FLAG")
		//						&& bcls_rec->Tables["CMD_TR_REM"].Rows[i]["FLAG"].ToString() == "1")
		//					{
		//						twma7.Reset();
		//						twma7["MAT_NO"] = bcls_rec->Tables["CMD_TR_REM"].Rows[i]["MAT_NO"].ToString().Trim();
		//						twma7.Query("MAT_NO");
		//						twma7["BATCH_NO"] = batch_no;
		//						twma7["STOCK_PLACE_NO_TO"] = stock_place_no_to;
		//						twma7.Update("BATCH_NO,STOCK_PLACE_NO_TO", "MAT_NO");
		//						bcls_rec_send.Tables[0].Rows.Add();
		//						bcls_rec_send.Tables[0].Rows[seqno_1]["mat_no"] = twma7["MAT_NO"].ToString();
		//						bcls_rec_send.Tables[0].Rows[seqno_1]["stock_oper_order"] = twma7["STOCK_OPER_ORDER"].ToString().Trim();
		//						bcls_rec_send.Tables[0].Rows[seqno_1]["crane_cmdgrpno"] = 0;
		//						bcls_rec_send.Tables[0].Rows[seqno_1]["stock_place_no_from"] = twma7["STOCK_PLACE_NO_FROM"].ToString().Trim();
		//						bcls_rec_send.Tables[0].Rows[seqno_1]["yard_layer_from"] = twma7["YARD_LAYER_FROM"].ToString().Trim();
		//						bcls_rec_send.Tables[0].Rows[seqno_1]["stock_place_no_to"] = twma7["STOCK_PLACE_NO_TO"].ToString();
		//						bcls_rec_send.Tables[0].Rows[seqno_1]["remark"] = "update";
		//						bcls_rec_send.Tables[0].Rows[seqno_1]["stock_oper_order_fin"] = twma7["STOCK_OPER_ORDER_FIN"].ToString();
		//						bcls_rec_send.Tables[0].Rows[seqno_1]["batch_no"] = twma7["BATCH_NO"].ToDecimal();
		//						bcls_rec_send.Tables[0].Rows[seqno_1]["cmd_seq"] = twma7["CMD_SEQ"].ToDecimal();
		//						seqno_1++;
		//					}
		//					else
		//					{
		//						bcls_ret->Tables[0].Rows.Add();
		//						bcls_ret->Tables[0].Rows[re_num]["STOCK_PLACE_NO"] = stock_place_no_to;
		//						bcls_ret->Tables[0].Rows[re_num]["BATCH_NO"] = batch_no;
		//						re_num++;
		//						twma7["MAT_NO"] = bcls_rec->Tables["CMD_TR_REM"].Rows[i]["MAT_NO"].ToString().Trim();
		//						twma7["BATCH_NO"] = batch_no;
		//						twma7["STOCK_PLACE_NO_TO"] = stock_place_no_to;
		//						twma7.Update("BATCH_NO,STOCK_PLACE_NO_TO", "MAT_NO");
		//					}
		//				}
		//			}
		//			else if (!bcls_rec->Tables["CMD_TR_REM"].Columns.Contains("FLAG"))
		//			{
		//				//中间批次号无法推荐，下层重新计算
		//				//获取下层过跨材料
		//				sqlstr = "select MAT_NO,MAT_ACT_LEN,MAT_ACT_THICK,MAT_ACT_WIDTH,MAT_ACT_WT from twma7 "
		//					" where (STOCK_OPER_ORDER='32' OR YARD_LAYER_FROM='" + layerno + "') AND STOCK_PLACE_NO_FROM='" + bcls_rec->Tables["CMD_TR_REM"].Rows[i]["STOCK_PLACE_NO"].ToString().Trim() + "' and int(YARD_LAYER_FROM)<=INT('" + layerno + "') "
		//					" order by int(YARD_LAYER_FROM) desc";
		//				Log::Trace("", __FUNCTION__, "sqlstr[{0}]", sqlstr);
		//				Db::QueryTable(sqlstr, cmd_mat);

		//				sqlstr = "update twma7 set BATCH_NO=0 where  STOCK_PLACE_NO_FROM='" + bcls_rec->Tables["CMD_TR_REM"].Rows[i]["STOCK_PLACE_NO"].ToString().Trim() + "' and int(YARD_LAYER_FROM)<INT('" + layerno + "') ";
		//				Log::Trace("", __FUNCTION__, "sqlstr[{0}]", sqlstr);
		//				Db::Execute(sqlstr);
		//				bcls_rec_tr.Tables["CMD_TR_REM"].Rows.Add();
		//				for (int j = 0; j < cmd_mat.Rows.get_Count(); ++j)
		//				{
		//					twma7.Reset();
		//					twma7["MAT_NO"] = cmd_mat.Rows[j]["MAT_NO"].ToString();
		//					twma7.Query("MAT_NO");

		//					//过跨命令						
		//					if (j>0)
		//					{
		//						twma7["STOCK_PLACE_NO_TO"] = "";
		//						twma7["BATCH_NO"] = 0;
		//						twma7.Update("STOCK_PLACE_NO_TO,BATCH_NO", "MAT_NO");
		//						bcls_rec_tr.Tables["CMD_TR_REM"].Rows[0]["FLAG"] = "1";
		//						bcls_rec_tr.Tables["CMD_TR_REM"].Rows[0]["HALL_TO"] = twma7["HALL_NO_TO"];
		//						bcls_rec_tr.Tables["CMD_TR_REM"].Rows[0]["HALL_FR"] = twma7["HALL_NO_FR"];
		//						bcls_rec_tr.Tables["CMD_TR_REM"].Rows[0]["STOCK_OPER_ORDER_FIN"] = twma7["STOCK_OPER_ORDER_FIN"];
		//					}
		//					else
		//					{
		//						bcls_rec_tr.Tables["CMD_TR_REM"].Rows[0]["FLAG"] = "0";
		//						bcls_rec_tr.Tables["CMD_TR_REM"].Rows[0]["HALL_TO"] = bcls_rec->Tables["CMD_TR_REM"].Rows[i]["HALL_TO"].ToString();
		//						bcls_rec_tr.Tables["CMD_TR_REM"].Rows[0]["HALL_FR"] = bcls_rec->Tables["CMD_TR_REM"].Rows[i]["HALL_FR"].ToString();
		//						bcls_rec_tr.Tables["CMD_TR_REM"].Rows[0]["STOCK_OPER_ORDER_FIN"] = bcls_rec->Tables["CMD_TR_REM"].Rows[i]["STOCK_OPER_ORDER_FIN"].ToString();						
		//					}


		//					bcls_rec_tr.Tables["CMD_TR_REM"].Rows[0]["MAT_NO"] = twma7["MAT_NO"];
		//					bcls_rec_tr.Tables["CMD_TR_REM"].Rows[0]["STOCK_PLACE_NO"] = twma7["STOCK_PLACE_NO_FROM"];
		//					bcls_rec_tr.Tables["CMD_TR_REM"].Rows[0]["MAT_THEORY_WT"] = twma7["MAT_ACT_WT"];
		//					bcls_rec_tr.Tables["CMD_TR_REM"].Rows[0]["MAT_THICK"] = twma7["MAT_ACT_THICK"];
		//					bcls_rec_tr.Tables["CMD_TR_REM"].Rows[0]["MAT_WIDTH"] = twma7["MAT_ACT_WIDTH"];
		//					bcls_rec_tr.Tables["CMD_TR_REM"].Rows[0]["MAT_LEN"] = twma7["MAT_ACT_LEN"];
		//					bcls_rec_tr.Tables["CMD_TR_REM"].Rows[0]["LAYERNO"] = twma7["YARD_LAYER_FROM"].ToString();

		//					doFlag = f_wmsmsm_cranecmd_tr_rem(&bcls_rec_tr, &bcls_rec_cccc, conn);
		//					if (doFlag != 0)
		//					{
		//						Log::Trace("", __FUNCTION__, "s.msg1：{0}", s.msg);
		//						throw CApplicationException(-1, s.msg, log.Location);
		//					}
		//					if (j == 0)
		//					{
		//						bcls_ret->Tables[0].Rows.Add();
		//						bcls_ret->Tables[0].Rows[re_num]["STOCK_PLACE_NO"] = bcls_rec_cccc.Tables[0].Rows[0]["STOCK_PLACE_NO"];
		//						bcls_ret->Tables[0].Rows[re_num]["BATCH_NO"] = bcls_rec_cccc.Tables[0].Rows[0]["BATCH_NO"];
		//						re_num++;
		//					}

		//				}

		//			}
		//			else
		//			{
		//				Log::Trace("", __FUNCTION__, "s.msg2：{0}", s.msg);
		//				throw CApplicationException(-1, s.msg, log.Location);
		//			}
		//		}
		//	}					
		//}


		//Log::Trace("", __FUNCTION__, "BATCH_NO {0}", batch_no);
		//Log::Trace("", __FUNCTION__, "get_Count {0}", bcls_ret->Tables[0].Rows.get_Count());
		//Log::Trace("", __FUNCTION__, "STOCK_PLACE_NO {0}", stock_place_no_to);
		//if (bcls_rec_send.Tables[0].Rows.get_Count()>0)
		//{
		//	/*doFlag = f_wmsmsm_u1dl03_snd(&bcls_rec_send, bcls_ret, conn);
		//	if (doFlag != 0)
		//	{
		//		throw CApplicationException(-1, s.msg, log.Location);
		//	}*/
		}



	}
	catch (CDbException& ex)
	{
		Log::Trace("", __FUNCTION__, "GetMsg {0}", ex.GetMsg());
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "Database processing error. sqlcode=[{0}].", arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}
