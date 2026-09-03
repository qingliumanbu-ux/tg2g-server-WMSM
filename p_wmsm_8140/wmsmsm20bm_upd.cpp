/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         JHZHAO
Version:		1.0
Date:			2016-03-05
Description:	待入库材料信息查询
**************************************************/

//框架头文件
#include "stdafx.h"
//#include "smhs.h"

//函数申明

/*<remark>=========================================================
///<summary>
///待入库材料信息查询
///<para>
///2.排序方式：队列写入时间
///</para>
///<para>数据库表：TWMA0 倒躲队列；TWMA1 物料主档表
///<returns>返回符合查询条件的队列信息</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wmsmsm20bm_upd)
int f_wmsmsm20bm_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	int count = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


	/* 实体类定义 */
	//CTWM20BM twm20bm(conn);
	CModel twm20bm = CModel("TWM20BM");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";

	/* 数据库操作类定义 */
	CDbCommand comm(conn);

	//系统的分页类信息。
	CPageInfo pageInfo;

	try
	{
		// 获取前台传入参数 
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			twm20bm.Reset();
			twm20bm["REPORT_DATE"] = bcls_rec->Tables[0].Rows[i]["REPORT_DATE"].ToString();
			twm20bm["FACTORY_DIV"] = bcls_rec->Tables[0].Rows[i]["FACTORY_DIV"].ToString();
			twm20bm["STOCK_NO"] = bcls_rec->Tables[0].Rows[i]["STOCK_NO"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("CONDITION_01"))
				twm20bm["CONDITION_01"] = bcls_rec->Tables[0].Rows[i]["CONDITION_01"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("CONDITION_02"))
				twm20bm["CONDITION_02"] = bcls_rec->Tables[0].Rows[i]["CONDITION_02"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("CONDITION_03"))
				twm20bm["CONDITION_03"] = bcls_rec->Tables[0].Rows[i]["CONDITION_03"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("CONDITION_04"))
				twm20bm["CONDITION_04"] = bcls_rec->Tables[0].Rows[i]["CONDITION_04"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("CONDITION_05"))
				twm20bm["CONDITION_05"] = bcls_rec->Tables[0].Rows[i]["CONDITION_05"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("CONDITION_06"))
				twm20bm["CONDITION_06"] = bcls_rec->Tables[0].Rows[i]["CONDITION_06"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("CONDITION_07"))
				twm20bm["CONDITION_07"] = bcls_rec->Tables[0].Rows[i]["CONDITION_07"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("CONDITION_08"))
				twm20bm["CONDITION_08"] = bcls_rec->Tables[0].Rows[i]["CONDITION_08"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("CONDITION_09"))
				twm20bm["CONDITION_09"] = bcls_rec->Tables[0].Rows[i]["CONDITION_09"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("CONDITION_10"))
				twm20bm["CONDITION_10"] = bcls_rec->Tables[0].Rows[i]["CONDITION_10"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("CONDITION_11"))
				twm20bm["CONDITION_11"] = bcls_rec->Tables[0].Rows[i]["CONDITION_11"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("CONDITION_12"))
				twm20bm["CONDITION_12"] = bcls_rec->Tables[0].Rows[i]["CONDITION_12"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("CONDITION_13"))
				twm20bm["CONDITION_13"] = bcls_rec->Tables[0].Rows[i]["CONDITION_13"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("CONDITION_14"))
				twm20bm["CONDITION_14"] = bcls_rec->Tables[0].Rows[i]["CONDITION_14"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("CONDITION_15"))
				twm20bm["CONDITION_15"] = bcls_rec->Tables[0].Rows[i]["CONDITION_15"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("CONDITION_16"))
				twm20bm["CONDITION_16"] = bcls_rec->Tables[0].Rows[i]["CONDITION_16"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("CONDITION_17"))
				twm20bm["CONDITION_17"] = bcls_rec->Tables[0].Rows[i]["CONDITION_17"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("CONDITION_18"))
				twm20bm["CONDITION_18"] = bcls_rec->Tables[0].Rows[i]["CONDITION_18"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("CONDITION_19"))
				twm20bm["CONDITION_19"] = bcls_rec->Tables[0].Rows[i]["CONDITION_19"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("CONDITION_20"))
				twm20bm["CONDITION_20"] = bcls_rec->Tables[0].Rows[i]["CONDITION_20"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("CONDITION_21"))
				twm20bm["CONDITION_21"] = bcls_rec->Tables[0].Rows[i]["CONDITION_21"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("CONDITION_22"))
				twm20bm["CONDITION_22"] = bcls_rec->Tables[0].Rows[i]["CONDITION_22"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("CONDITION_23"))
				twm20bm["CONDITION_23"] = bcls_rec->Tables[0].Rows[i]["CONDITION_23"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("CONDITION_24"))
				twm20bm["CONDITION_24"] = bcls_rec->Tables[0].Rows[i]["CONDITION_24"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("CONDITION_25"))
				twm20bm["CONDITION_25"] = bcls_rec->Tables[0].Rows[i]["CONDITION_25"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("CONDITION_26"))
				twm20bm["CONDITION_26"] = bcls_rec->Tables[0].Rows[i]["CONDITION_26"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("CONDITION_27"))
				twm20bm["CONDITION_27"] = bcls_rec->Tables[0].Rows[i]["CONDITION_27"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("CONDITION_28"))
				twm20bm["CONDITION_28"] = bcls_rec->Tables[0].Rows[i]["CONDITION_28"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("CONDITION_29"))
				twm20bm["CONDITION_29"] = bcls_rec->Tables[0].Rows[i]["CONDITION_29"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("CONDITION_30"))
				twm20bm["CONDITION_30"] = bcls_rec->Tables[0].Rows[i]["CONDITION_30"].ToString();
			twm20bm["ADJ_NUM"] = bcls_rec->Tables[0].Rows[i]["ADJ_NUM"].ToDecimal();
			twm20bm["ADJ_WT"] = bcls_rec->Tables[0].Rows[i]["ADJ_WT"].ToDecimal();
			twm20bm["REC_REVISOR"] = s.userid;
			twm20bm["REC_REVISE_TIME"] = datetime;


			if (twm20bm["REPORT_DATE"].ToString().Trim() == "")
			{
				sprintf(s.msg, "账期不能为空。");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (twm20bm["STOCK_NO"].ToString().Trim() != "")
			{
				twm20bm["FACTORY_DIV"] = " ";
			}

			if (twm20bm["STOCK_NO"].ToString().Trim() == "" &&
				twm20bm["FACTORY_DIV"].ToString().Trim() == "")
			{
				sprintf(s.msg, "厂别/库区不能都为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}




			count = twm20bm.QueryCount(
				"REPORT_DATE,"
				"FACTORY_DIV,"
				"STOCK_NO,"
				"ITEM_ENAME,"
				"CONDITION_01,"
				"CONDITION_02,"
				"CONDITION_03,"
				"CONDITION_04,"
				"CONDITION_05,"
				"CONDITION_06,"
				"CONDITION_07,"
				"CONDITION_08,"
				"CONDITION_09,"
				"CONDITION_10,"
				"CONDITION_11,"
				"CONDITION_12,"
				"CONDITION_13,"
				"CONDITION_14,"
				"CONDITION_15,"
				"CONDITION_16,"
				"CONDITION_17,"
				"CONDITION_18,"
				"CONDITION_19,"
				"CONDITION_20,"
				"CONDITION_21,"
				"CONDITION_22,"
				"CONDITION_23,"
				"CONDITION_24,"
				"CONDITION_25,"
				"CONDITION_26,"
				"CONDITION_27,"
				"CONDITION_28,"
				"CONDITION_29,"
				"CONDITION_30"
				);

			if (count == 0)
			{
				twm20bm.Insert();
			}
			else
			{
				twm20bm.Update(
					"REC_REVISOR,"
					"REC_REVISE_TIME,"
					"ADJ_NUM,"
					"ADJ_WT"
					,
					"REPORT_DATE,"
					"FACTORY_DIV,"
					"STOCK_NO,"
					"ITEM_ENAME,"
					"CONDITION_01,"
					"CONDITION_02,"
					"CONDITION_03,"
					"CONDITION_04,"
					"CONDITION_05,"
					"CONDITION_06,"
					"CONDITION_07,"
					"CONDITION_08,"
					"CONDITION_09,"
					"CONDITION_10,"
					"CONDITION_11,"
					"CONDITION_12,"
					"CONDITION_13,"
					"CONDITION_14,"
					"CONDITION_15,"
					"CONDITION_16,"
					"CONDITION_17,"
					"CONDITION_18,"
					"CONDITION_19,"
					"CONDITION_20,"
					"CONDITION_21,"
					"CONDITION_22,"
					"CONDITION_23,"
					"CONDITION_24,"
					"CONDITION_25,"
					"CONDITION_26,"
					"CONDITION_27,"
					"CONDITION_28,"
					"CONDITION_29,"
					"CONDITION_30");
			}


#pragma region 计算期末数据
			sqlstr =
				" UPDATE TWM20BM SET"
				" END_WT = INIT_WT"
				" + INSTOCK_01"
				" + INSTOCK_02"
				" + INSTOCK_03"
				" + INSTOCK_04"
				" + INSTOCK_05"
				" + INSTOCK_06"
				" + INSTOCK_07"
				" + INSTOCK_08"
				" + INSTOCK_09"
				" + INSTOCK_10"
				" + INSTOCK_11"
				" + INSTOCK_12"
				" + INSTOCK_13"
				" + INSTOCK_14"
				" + INSTOCK_15"
				" + INSTOCK_16"
				" + INSTOCK_17"
				" + INSTOCK_18"
				" + INSTOCK_19"
				" + INSTOCK_20"
				" - OUTSTOCK_01"
				" - OUTSTOCK_02"
				" - OUTSTOCK_03"
				" - OUTSTOCK_04"
				" - OUTSTOCK_05"
				" - OUTSTOCK_06"
				" - OUTSTOCK_07"
				" - OUTSTOCK_08"
				" - OUTSTOCK_09"
				" - OUTSTOCK_10"
				" - OUTSTOCK_11"
				" - OUTSTOCK_12"
				" - OUTSTOCK_13"
				" - OUTSTOCK_14"
				" - OUTSTOCK_15"
				" - OUTSTOCK_16"
				" - OUTSTOCK_17"
				" - OUTSTOCK_18"
				" - OUTSTOCK_19"
				" - OUTSTOCK_20"
				" + ADJ_WT"
				","
				" END_NUM = INIT_NUM"
				" + INSTOCK_NUM_01"
				" + INSTOCK_NUM_02"
				" + INSTOCK_NUM_03"
				" + INSTOCK_NUM_04"
				" + INSTOCK_NUM_05"
				" + INSTOCK_NUM_06"
				" + INSTOCK_NUM_07"
				" + INSTOCK_NUM_08"
				" + INSTOCK_NUM_09"
				" + INSTOCK_NUM_10"
				" + INSTOCK_NUM_11"
				" + INSTOCK_NUM_12"
				" + INSTOCK_NUM_13"
				" + INSTOCK_NUM_14"
				" + INSTOCK_NUM_15"
				" + INSTOCK_NUM_16"
				" + INSTOCK_NUM_17"
				" + INSTOCK_NUM_18"
				" + INSTOCK_NUM_19"
				" + INSTOCK_NUM_20"
				" - OUTSTOCK_NUM_01"
				" - OUTSTOCK_NUM_02"
				" - OUTSTOCK_NUM_03"
				" - OUTSTOCK_NUM_04"
				" - OUTSTOCK_NUM_05"
				" - OUTSTOCK_NUM_06"
				" - OUTSTOCK_NUM_07"
				" - OUTSTOCK_NUM_08"
				" - OUTSTOCK_NUM_09"
				" - OUTSTOCK_NUM_10"
				" - OUTSTOCK_NUM_11"
				" - OUTSTOCK_NUM_12"
				" - OUTSTOCK_NUM_13"
				" - OUTSTOCK_NUM_14"
				" - OUTSTOCK_NUM_15"
				" - OUTSTOCK_NUM_16"
				" - OUTSTOCK_NUM_17"
				" - OUTSTOCK_NUM_18"
				" - OUTSTOCK_NUM_19"
				" - OUTSTOCK_NUM_20"
				" + ADJ_NUM"
				" WHERE REPORT_DATE = @report_date"
				" AND FACTORY_DIV = @factory_div"
				" AND STOCK_NO = @stock_no"
				" AND CONDITION_01 = @condition_01"
				" AND CONDITION_02 = @condition_02"
				" AND CONDITION_03 = @condition_03"
				" AND CONDITION_04 = @condition_04"
				" AND CONDITION_05 = @condition_05"
				" AND CONDITION_06 = @condition_06"
				" AND CONDITION_07 = @condition_07"
				" AND CONDITION_08 = @condition_08"
				" AND CONDITION_09 = @condition_09"
				" AND CONDITION_10 = @condition_10"
				" AND CONDITION_11 = @condition_11"
				" AND CONDITION_12 = @condition_12"
				" AND CONDITION_13 = @condition_13"
				" AND CONDITION_14 = @condition_14"
				" AND CONDITION_15 = @condition_15"
				" AND CONDITION_16 = @condition_16"
				" AND CONDITION_17 = @condition_17"
				" AND CONDITION_18 = @condition_18"
				" AND CONDITION_19 = @condition_19"
				" AND CONDITION_20 = @condition_20"
				" AND CONDITION_21 = @condition_21"
				" AND CONDITION_22 = @condition_22"
				" AND CONDITION_23 = @condition_23"
				" AND CONDITION_24 = @condition_24"
				" AND CONDITION_25 = @condition_25"
				" AND CONDITION_26 = @condition_26"
				" AND CONDITION_27 = @condition_27"
				" AND CONDITION_28 = @condition_28"
				" AND CONDITION_29 = @condition_29"
				" AND CONDITION_30 = @condition_30"
				;

			comm.SetCommandText(sqlstr);
			comm.Parameters.Set("report_date", twm20bm["REPORT_DATE"].ToString());
			comm.Parameters.Set("factory_div", twm20bm["FACTORY_DIV"].ToString());
			comm.Parameters.Set("stock_no", twm20bm["STOCK_NO"].ToString());
			comm.Parameters.Set("condition_01", twm20bm["CONDITION_01"].ToString());
			comm.Parameters.Set("condition_02", twm20bm["CONDITION_02"].ToString());
			comm.Parameters.Set("condition_03", twm20bm["CONDITION_03"].ToString());
			comm.Parameters.Set("condition_04", twm20bm["CONDITION_04"].ToString());
			comm.Parameters.Set("condition_05", twm20bm["CONDITION_05"].ToString());
			comm.Parameters.Set("condition_06", twm20bm["CONDITION_06"].ToString());
			comm.Parameters.Set("condition_07", twm20bm["CONDITION_07"].ToString());
			comm.Parameters.Set("condition_08", twm20bm["CONDITION_08"].ToString());
			comm.Parameters.Set("condition_09", twm20bm["CONDITION_09"].ToString());
			comm.Parameters.Set("condition_10", twm20bm["CONDITION_10"].ToString());
			comm.Parameters.Set("condition_11", twm20bm["CONDITION_11"].ToString());
			comm.Parameters.Set("condition_12", twm20bm["CONDITION_12"].ToString());
			comm.Parameters.Set("condition_13", twm20bm["CONDITION_13"].ToString());
			comm.Parameters.Set("condition_14", twm20bm["CONDITION_14"].ToString());
			comm.Parameters.Set("condition_15", twm20bm["CONDITION_15"].ToString());
			comm.Parameters.Set("condition_16", twm20bm["CONDITION_16"].ToString());
			comm.Parameters.Set("condition_17", twm20bm["CONDITION_17"].ToString());
			comm.Parameters.Set("condition_18", twm20bm["CONDITION_18"].ToString());
			comm.Parameters.Set("condition_19", twm20bm["CONDITION_19"].ToString());
			comm.Parameters.Set("condition_20", twm20bm["CONDITION_20"].ToString());
			comm.Parameters.Set("condition_21", twm20bm["CONDITION_21"].ToString());
			comm.Parameters.Set("condition_22", twm20bm["CONDITION_22"].ToString());
			comm.Parameters.Set("condition_23", twm20bm["CONDITION_23"].ToString());
			comm.Parameters.Set("condition_24", twm20bm["CONDITION_24"].ToString());
			comm.Parameters.Set("condition_25", twm20bm["CONDITION_25"].ToString());
			comm.Parameters.Set("condition_26", twm20bm["CONDITION_26"].ToString());
			comm.Parameters.Set("condition_27", twm20bm["CONDITION_27"].ToString());
			comm.Parameters.Set("condition_28", twm20bm["CONDITION_28"].ToString());
			comm.Parameters.Set("condition_29", twm20bm["CONDITION_29"].ToString());
			comm.Parameters.Set("condition_30", twm20bm["CONDITION_30"].ToString());


			comm.ExecuteNonQuery();

#pragma endregion
		}

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

