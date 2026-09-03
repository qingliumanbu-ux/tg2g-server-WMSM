/* **************************************************************************
*	Copyright (c) Baosight Corporation 2008 . All Rights Reserved.
*  	BM2PES 宝信生产执行系统
*****************************************************************************
*  程序名称			: f_wm00_mm0099
*  程序描述			: 物料主档/物料同步电文处理
*  备注说明			:
*  修改历史			:
*  		henno 2016-09-28			(ADD)程序建立
*			... ...
* **************************************************************************** */
/* C/C++ 的标准头文件部分 */
#include "stdafx.h"		// 框架头，不可删除 
//#include "twma0.h" 
//#include "twma1.h"
//#include "twma2.h"
 



//外部函数声明
#if defined(_SYS_PES) || defined (_SYS_MES)
BM2_FUNCTION_IMPORT
int f_mm0099(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
#endif



BM2_FUNCTION_EXPORT
int f_wmsmsm_mm0099(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	/* ***** 程序变量 ***** */
	int doFlag = 0;
	CString v_datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString v_mat_no = "";
	CString v_stock_oper_order = "";
	CString v_stock_oper_order_div = "";
	CString v_old_stock_no = "";
	CString v_old_stock_place_no = "";
	CDecimal v_old_layer_no = 0;
	CString v_eventid = "";
	CString v_tcno = "";
	CString v_come_reject_cause = " ";
	CString v_aim_store = ""; 
	CString vehicle_no = " ";

	/* ***** 数据库SQL操作字符串 ***** */
	CString	sqlstr = "";

	/* ***** 数据库操作类定义 ***** */
	ParameterList paraList;
	RecordList recordList;
	Record record;
	CDbCommand comm(conn);

	/* ***** 定义表实体对象 ***** */
	CModel twma0 = CModel("TWMA0");
	CModel twma1 = CModel("TMMSM01");
	CModel twma2 = CModel("TWMA2");
	CModel twm04 = CModel("TWM04");



#if defined(_SYS_PES)
	/*CWM0000 xwm0000(conn);*/
#endif

	EIClass bcls_rec_mm99;
	bcls_rec_mm99.Tables[0].set_TableName("MM0099");
	bcls_rec_mm99.Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_ID");
	bcls_rec_mm99.Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_DESC");
	bcls_rec_mm99.Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");
	bcls_rec_mm99.Tables["MM0099"].Columns.Add(DT_STRING, "SYSTEM_ID");
	bcls_rec_mm99.Tables["MM0099"].Columns.Add(DT_STRING, "FUNC_ID");
	bcls_rec_mm99.Tables["MM0099"].Columns.Add(DT_STRING, "FACTORY_STORE");
	bcls_rec_mm99.Tables["MM0099"].Columns.Add(DT_STRING, "SLABTOP_FLAG");
	bcls_rec_mm99.Tables["MM0099"].Columns.Add(DT_STRING, "SLABTOP_TIME");
	bcls_rec_mm99.Tables["MM0099"].Columns.Add(DT_STRING, "SLAB_RETURN_CAUSE_CODE");
	bcls_rec_mm99.Tables["MM0099"].Columns.Add(DT_STRING, "BACK_MAT_REASON");
	bcls_rec_mm99.Tables["MM0099"].Columns.Add(DT_STRING, "AIM_STORE");
	bcls_rec_mm99.Tables["MM0099"].Columns.Add(DT_STRING, "COOL_FLAG");
	bcls_rec_mm99.Tables["MM0099"].Columns.Add(twma1);
	bcls_rec_mm99.Tables["MM0099"].Columns.Add(twma2);
	bcls_rec_mm99.Tables["MM0099"].Rows.Clear();


	EIClass bcls_rec_wm00;



	/* ***** 应用程序开始处理 ***** */
	try
	{
		if (!bcls_rec->Tables.Contains("WMMM99"))
		{
			sprintf(s.msg, "函数f_wm00_mm0099中找不到接收块名[WMMM99]");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		for (int iRow = 0; iRow < bcls_rec->Tables["WMMM99"].Rows.get_Count(); iRow++)
		{
			v_mat_no = bcls_rec->Tables["WMMM99"].Rows[iRow]["MAT_NO"].ToString().Trim();
			v_stock_oper_order = bcls_rec->Tables["WMMM99"].Rows[iRow]["STOCK_OPER_ORDER"].ToString().Trim();
			v_stock_oper_order_div = bcls_rec->Tables["WMMM99"].Rows[iRow]["STOCK_OPER_ORDER_DIV"].ToString().TrimOrBlank();
			v_old_stock_no = bcls_rec->Tables["WMMM99"].Rows[iRow]["OLD_STOCK_NO"].ToString().Trim();
			v_old_stock_place_no = bcls_rec->Tables["WMMM99"].Rows[iRow]["OLD_STOCK_PLACE_NO"].ToString().Trim();
			v_old_layer_no = bcls_rec->Tables["WMMM99"].Rows[iRow]["OLD_LAYER_NO"].ToDecimal();

			if (bcls_rec->Tables["WMMM99"].Columns.Contains("COME_REJECT_CAUSE"))
			{
				v_come_reject_cause = bcls_rec->Tables["WMMM99"].Rows[iRow]["COME_REJECT_CAUSE"].ToString().Trim();
			}
			if (bcls_rec->Tables["WMMM99"].Columns.Contains("AIM_STORE"))
			{
				v_aim_store = bcls_rec->Tables["WMMM99"].Rows[iRow]["AIM_STORE"].ToString().Trim();
			}
			if (bcls_rec->Tables["WMMM99"].Columns.Contains("VEHICLE_NO"))
			{
				vehicle_no = bcls_rec->Tables["WMMM99"].Rows[iRow]["VEHICLE_NO"].ToString().Trim();
			}

			Log::Trace("", __FUNCTION__, "传入参数 v_mat_no\t[{0}]", v_mat_no);
			Log::Trace("", __FUNCTION__, "传入参数 v_stock_oper_order\t[{0}]", v_stock_oper_order);
			Log::Trace("", __FUNCTION__, "传入参数 v_stock_oper_order_div\t[{0}]", v_stock_oper_order_div);
			Log::Trace("", __FUNCTION__, "传入参数 v_old_stock_no\t[{0}]", v_old_stock_no);
			Log::Trace("", __FUNCTION__, "传入参数 v_old_stock_place_no\t[{0}]", v_old_stock_place_no);
			Log::Trace("", __FUNCTION__, "传入参数 v_old_layer_no\t[{0}]", v_old_layer_no);
			Log::Trace("", __FUNCTION__, "传入参数 v_come_reject_cause\t[{0}]", v_come_reject_cause);
			Log::Trace("", __FUNCTION__, "传入参数 v_aim_store\t[{0}]", v_aim_store);
			Log::Trace("", __FUNCTION__, "传入参数 vehicle_no\t[{0}]", vehicle_no);

			if (v_mat_no.Trim() == "")
			{
				sprintf(s.msg, "传入的MAT_NO不能为空。");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (v_stock_oper_order.Trim() == "")
			{
				sprintf(s.msg, "传入的STOCK_OPER_ORDER不能为空。");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			twma1["MAT_NO"] = v_mat_no;
			if (!twma1.Query("MAT_NO"))
			{
				sprintf(s.msg, "TWMA1找不到材料[{%s}]", (const char*)twma1["MAT_NO"]);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if ((v_stock_oper_order[0] == '1' ||
				v_stock_oper_order[0] == '3') &&
				v_stock_oper_order_div != "D")
			{
				twma2.Reset();
				twma2["MAT_NO"] = v_mat_no;
				if (!twma2.Query("MAT_NO"))
				{
					sprintf(s.msg, "TWMA2找不到材料[{%s}]", (const char*)twma2["MAT_NO"]);
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}


#if defined(_SYS_PES) || defined (_SYS_MES)
			//获取事件号
			sqlstr =
				" SELECT EVENT_ID FROM TWM0B"
				" WHERE MAT_LINE_TYPE = @mat_line_type"
				" AND MAT_KIND = @mat_kind"
				" AND STOCK_OPER_ORDER = @stock_oper_order"
				" AND STOCK_OPER_ORDER_DIV = @stock_oper_order_div"
				" AND STOCK_NO = @stock_no"
				" AND TO_STOCK_NO = @to_stock_no"
				" AND MODULE_NAME = 'MM'";

			v_eventid = "";

			if (v_eventid.Trim() == "")
			{
				paraList.Set("mat_line_type", twma1["MAT_LINE_TYPE"].ToString());
				paraList.Set("mat_kind", twma1["MAT_KIND"].ToString());
				paraList.Set("stock_oper_order", v_stock_oper_order);
				paraList.Set("stock_oper_order_div", v_stock_oper_order_div);
				paraList.Set("stock_no", twma1["STOCK_NO"].ToString());
				paraList.Set("to_stock_no", v_aim_store);
				v_eventid = Db::QueryCString(sqlstr, paraList);
			}

			if (v_eventid.Trim() == "")
			{
				paraList.Set("mat_line_type", twma1["MAT_LINE_TYPE"]);
				paraList.Set("mat_kind", twma1["MAT_KIND"]);
				paraList.Set("stock_oper_order", v_stock_oper_order);
				paraList.Set("stock_oper_order_div", v_stock_oper_order_div);
				paraList.Set("stock_no", twma1["STOCK_NO"]);
				paraList.Set("to_stock_no", " ");
				v_eventid = Db::QueryCString(sqlstr, paraList);
			}

			if (v_eventid.Trim() == "")
			{
				paraList.Set("mat_line_type", twma1["MAT_LINE_TYPE"]);
				paraList.Set("mat_kind", twma1["MAT_KIND"]);
				paraList.Set("stock_oper_order", v_stock_oper_order);
				paraList.Set("stock_oper_order_div", v_stock_oper_order_div);
				paraList.Set("stock_no", " ");
				paraList.Set("to_stock_no", " ");
				v_eventid = Db::QueryCString(sqlstr, paraList);
			}

			if (v_eventid.Trim() == "")
			{
				paraList.Set("mat_line_type", "00");
				paraList.Set("mat_kind", twma1["MAT_KIND"]);
				paraList.Set("stock_oper_order", v_stock_oper_order);
				paraList.Set("stock_oper_order_div", v_stock_oper_order_div);
				paraList.Set("stock_no", twma1["STOCK_NO"]);
				paraList.Set("to_stock_no", " ");
				v_eventid = Db::QueryCString(sqlstr, paraList);
			}

			if (v_eventid.Trim() == "")
			{
				paraList.Set("mat_line_type", "00");
				paraList.Set("mat_kind", twma1["MAT_KIND"]);
				paraList.Set("stock_oper_order", v_stock_oper_order);
				paraList.Set("stock_oper_order_div", v_stock_oper_order_div);
				paraList.Set("stock_no", " ");
				paraList.Set("to_stock_no", " ");
				v_eventid = Db::QueryCString(sqlstr, paraList);
			}

			if (v_stock_oper_order == "2R")
			{
				if (v_eventid.Trim() == "")
				{
					//未配置不调用
					Log::Trace("", __FUNCTION__, "TWM0B表中未配置事件号。");
					sprintf(s.msg, "TWM0B表中未配置事件号");
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}

			if (v_eventid.Trim() == "")
			{
				if (v_stock_oper_order[0] == '1')
				{
					if (v_stock_oper_order_div.Trim() != "D")
					{
						v_eventid = "WM01";
					}
					else
					{
						v_eventid = "WM10";
					}
				}
				else if (v_stock_oper_order[0] == '2')
				{
					v_eventid = "WM02";
				}
				else if (v_stock_oper_order[0] == '3')
				{
					v_eventid = "WM03";
				}
			}

			Log::Trace("", __FUNCTION__, "v_eventid\t[{0}]", v_eventid);

			if (v_eventid.Trim() == "")
			{
				//未配置不调用
				Log::Trace("", __FUNCTION__, "TWM0B表中未配置事件号。");
				sprintf(s.msg, "TWM0B表中未配置事件号");
				throw CApplicationException(-1, s.msg, log.Location);
				continue;
			}

			if (v_eventid[0] == '@')
			{
				//配置不调用
				Log::Trace("", __FUNCTION__, "TWM0B表中配置事件号为@，不调用。");
				continue;
			}




			Log::Trace("", __FUNCTION__, "1111");

			//调用物料函数f_mm0099
			//bcls_rec_mm99.Tables["MM0099"].Rows.Clear();
			bcls_rec_mm99.Tables["MM0099"].Rows.Add();
			int row_count = bcls_rec_mm99.Tables["MM0099"].Rows.get_Count();
			bcls_rec_mm99.Tables["MM0099"].Rows[row_count - 1]["EVENT_ID"] = v_eventid;
			bcls_rec_mm99.Tables["MM0099"].Rows[row_count - 1]["EVENT_LINE_TYPE"] = "00";
			bcls_rec_mm99.Tables["MM0099"].Rows[row_count - 1]["SYSTEM_ID"] = "WM00";
			bcls_rec_mm99.Tables["MM0099"].Rows[row_count - 1]["FUNC_ID"] = s.svc_name;
			//bcls_rec_mm99.Tables["MM0099"].Rows[0].Merge(twma1);
			CDataTable dt_temp;
			dt_temp.Clear();
			twma1.MergeTo(dt_temp);
			bcls_rec_mm99.Tables["MM0099"].Rows[row_count - 1].Merge(dt_temp.Rows[0]);

			if (v_stock_oper_order[0] != '2' &&
				v_stock_oper_order_div.Trim() != "D")
			{
				//bcls_rec_mm99.Tables["MM0099"].Rows[0].Merge(twma2);
				dt_temp.Clear();
				twma2.MergeTo(dt_temp);
				bcls_rec_mm99.Tables["MM0099"].Rows[row_count - 1].Merge(dt_temp.Rows[0]);
			}

			bcls_rec_mm99.Tables["MM0099"].Rows[row_count - 1]["STOCK_OPER_ORDER"] = v_stock_oper_order;
			bcls_rec_mm99.Tables["MM0099"].Rows[row_count - 1]["AIM_STORE"] = v_aim_store;


			Log::Trace("", __FUNCTION__, "2222222222222222");
			if (twma2["STOCK_NO"].ToString().Trim() != "")
			{
				sqlstr =
					" SELECT MAT_LINE_TYPE, FACTORY_DIV FROM TWM01"
					" WHERE STOCK_NO = @stock_no";

				comm.SetCommandText(sqlstr);
				comm.Parameters.Set("stock_no", twma2["STOCK_NO"].ToString());
				comm.ExecuteReader();
				if (comm.Read())
				{
					bcls_rec_mm99.Tables["MM0099"].Rows[row_count - 1]["MAT_LINE_TYPE"] = comm.GetString(1);
					bcls_rec_mm99.Tables["MM0099"].Rows[row_count - 1]["FACTORY_STORE"] = comm.GetString(2);
				}
				comm.Close();


				//paraList.Set("stock_no", twma2["STOCK_NO"]);
				//record = Db::QueryFirst(sqlstr, paraList);

				//Log::Trace("", __FUNCTION__, "33335555555555333333333");
				//CString v_temp = record.GetCString("MAT_LINE_TYPE");
				//Log::Trace("", __FUNCTION__, "v_temp[{0}]", v_temp);
				//bcls_rec_mm99.Tables["MM0099"].Rows[0]["MAT_LINE_TYPE"] = v_temp;
				//Log::Trace("", __FUNCTION__, "aaaaaaaaaaaaaaaaaaa");

				//bcls_rec_mm99.Tables["MM0099"].Rows[0]["FACTORY_STORE"] = record.GetCString("FACTORY_DIV");
				//Log::Trace("", __FUNCTION__, "444444444444444");
				
			}

			Log::Trace("", __FUNCTION__, "555555555555555555");
			if (v_stock_oper_order[0] == '1')
			{
				bcls_rec_mm99.Tables["MM0099"].Rows[row_count - 1]["IN_FLAG"] = "1";
				bcls_rec_mm99.Tables["MM0099"].Rows[row_count - 1]["IN_STOCK_TIME"] = v_datetime;
			}
			else if (v_stock_oper_order[0] == '2')
			{
				bcls_rec_mm99.Tables["MM0099"].Rows[row_count - 1]["IN_FLAG"] = "0";
				bcls_rec_mm99.Tables["MM0099"].Rows[row_count - 1]["OUT_STOCK_TIME"] = v_datetime;
			}

			if (v_stock_oper_order.Trim() == "2X")
			{
				bcls_rec_mm99.Tables["MM0099"].Rows[row_count - 1]["SLABTOP_FLAG"] = "2";
				bcls_rec_mm99.Tables["MM0099"].Rows[row_count - 1]["SLABTOP_TIME"] = v_datetime;
				bcls_rec_mm99.Tables["MM0099"].Rows[row_count - 1]["COOL_FLAG"] = " ";
			}
			else if (v_stock_oper_order.Trim() == "1X")
			{
				bcls_rec_mm99.Tables["MM0099"].Rows[row_count - 1]["SLABTOP_FLAG"] = "3";
				bcls_rec_mm99.Tables["MM0099"].Rows[row_count - 1]["SLABTOP_TIME"] = v_datetime;
			}

			if (v_stock_oper_order_div.Trim() == "D")
			{
				bcls_rec_mm99.Tables["MM0099"].Rows[row_count - 1]["SLABTOP_FLAG"] = "0";
				//bcls_rec_mm99.Tables["MM0099"].Rows[0]["BACK_MAT_REASON"] = v_come_reject_cause;
				bcls_rec_mm99.Tables["MM0099"].Rows[row_count - 1]["SLAB_RETURN_CAUSE_CODE"] = v_come_reject_cause;
			}


			//doFlag = f_mm0099(&bcls_rec_mm99, bcls_ret, conn);
			//if (doFlag < 0)
			//{
			//	throw CApplicationException(-1, s.msg, log.Location);
			//}
#else
			//更新TWMA1
			twma1["REC_REVISOR"] = s.userid;
			twma1["REC_REVISE_TIME"] = v_datetime;
			twma1["OLD_STOCK_NO"] = twma1["STOCK_NO"];
			twma1["OLD_LOGIC_STOCK_NO"] = twma1["LOGIC_STOCK_NO"];
			twma1["STOCK_NO"] = twma2["STOCK_NO"];
			twma1["LOGIC_STOCK_NO"] = twma2["LOGIC_STOCK_NO"];
			twma1["STOCK_OPER_ORDER"] = v_stock_oper_order;
			if (v_stock_oper_order[0] == '1')
			{
				twma1["IN_FLAG"] = "1";
				twma1["IN_STOCK_TIME"] = v_datetime;
			}
			else if (v_stock_oper_order[0] == '2')
			{
				twma1["OUT_STOCK_TIME"] = v_datetime;
				twma1["IN_FLAG"] = "0";
			}

			twma1.Update(
				"REC_REVISOR,"
				"REC_REVISE_TIME,"
				"OLD_STOCK_NO,"
				"OLD_LOGIC_STOCK_NO,"
				"STOCK_NO,"
				"LOGIC_STOCK_NO,"
				"STOCK_OPER_ORDER,"
				"IN_FLAG,"
				"IN_STOCK_TIME,"
				"OUT_STOCK_TIME"
				,
				"MAT_NO");
#endif


			Log::Trace("", __FUNCTION__, "aaaaaaaaaaaaaaaaa");


		}
#if defined(_SYS_PES) || defined (_SYS_MES)
		if (bcls_rec_mm99.Tables[0].Rows.get_Count() > 0)
		{
			doFlag = f_mm0099(&bcls_rec_mm99, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
#endif
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };

		/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006"), arguments, 1);
		CString str = ex.GetMsg() + "\r\n" + sqlstr;

		/*返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应*/
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);

		/*数据库异常时返回-1，事务将被回滚*/
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), 399);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), 399);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}

