/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:      JQ
Version:     1.1.1
Date:        2017-12-20
Description: 垛位推荐
**************************************************/

#include "stdafx.h"		// 框架头，不可删除 

BM2_FUNCTION_EXPORT
int f_auto_sm(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int k = 0;
	int j = 0;
	int n = 0;

	CString mat_no = "";
	CString stock_no = "";
	CString stock_oper_order = "";
	CString	in_flag = "";
	CString	stock_place_no = "";
	CString	stock_place_no1 = "";
	CString	stock_place_no2 = "";
	CString	rn_stock_place_no = "";
	CString	hall_no = "";

	CDecimal layernum = 0;
	CDecimal mat_num = 0;
	CDecimal slab_times = 0;

	CString sqlstr = "";
	CString sqlstr1 = "";

	CDbCommand execute_sql(conn);
	CDbCommand com_sql(conn);

	CModel twma1 = CModel("TMMSM01");
	CModel twm04 = CModel("TWM04");

	try
	{
		if (bcls_rec->Tables.IndexOf("AUTO_INFO_IN") < 0 ||
			bcls_rec->Tables["AUTO_INFO_IN"].Rows.get_Count() == 0)
		{
			sprintf(s.msg, "函数f_auto中找不到接收块名[AUTO_INFO_IN]或值为空");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		bcls_ret->Tables.Add("STOCK_PLACE_NO");
		bcls_ret->Tables["STOCK_PLACE_NO"].Columns.Add(DT_STRING, "STOCK_PLACE_NO");
		bcls_ret->Tables["STOCK_PLACE_NO"].Rows.Add();

		//获取传入参数
		mat_no = bcls_rec->Tables["AUTO_INFO_IN"].Rows[0]["MAT_NO"].ToString();
		stock_no = bcls_rec->Tables["AUTO_INFO_IN"].Rows[0]["STOCK_NO"].ToString();
		stock_oper_order = bcls_rec->Tables["AUTO_INFO_IN"].Rows[0]["STOCK_OPER_ORDER"].ToString();

		Log::Trace("", __FUNCTION__, "MAT_NO={0}", mat_no);
		Log::Trace("", __FUNCTION__, "STOCK_NO={0}", stock_no);
		Log::Trace("", __FUNCTION__, "STOCK_OPER_ORDER={0}", stock_oper_order);

		twma1["MAT_NO"] = mat_no;
		if (!twma1.Query("MAT_NO"))
		{
			sprintf(s.msg, "主档没有此材料信息【" + mat_no + "】.");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		if (stock_oper_order.SubstringNE(0,1) == "1")
		{
			Log::Trace("", __FUNCTION__, "-------- 入库推荐库位 --------");
			Log::Trace("", __FUNCTION__, " twma1.ORDER_NO = [{0}]", twma1["ORDER_NO"].ToString());
			//1.合同材：先按同合同推荐，然后推荐同内部钢种同宽度同长度库位，再推荐空库位。 
			//2.余材：  先按同炉推荐，  然后推荐同内部钢种同宽度同长度库位，再推荐空库位。
			if (twma1["ORDER_NO"].ToString().Trim() != "")
			{
				sqlstr =
					"select distinct stock_place_no, sum(mat_num) mat_num from( "
					"select distinct stock_place_no, count(1) mat_num from tmmsm01 "
					"where in_flag = '1' and stock_no = @stock_no and order_no = @order_no "
					"and stock_place_no in(select distinct stock_place_no from tmmsm01 where in_flag = '1' and stock_no = @stock_no and plan_no = ' ') "  //已编计划的板坯，禁止压坯
					"group by stock_place_no "
					"union all "
					"select distinct to_stock_place_no stock_place_no, count(1) mat_num from twma0 "
					"where to_stock_place_no > '  ' and stock_no = @stock_no and stock_oper_order like '1%' "
					"and mat_no in (select mat_no from tmmsm01 where order_no = @order_no) "
					"group by to_stock_place_no "
					") group by stock_place_no  order by stock_place_no " 
					;
			}
			else
			{
				sqlstr =
					"select distinct stock_place_no, sum(mat_num) mat_num from( "
					"select distinct stock_place_no, count(1) mat_num from tmmsm01 "
					"where in_flag = '1' and stock_no = @stock_no and heat_no = @heat_no "
					"and stock_place_no in (select distinct stock_place_no from tmmsm01 where in_flag = '1' and stock_no = @stock_no and plan_no = ' ') "  //已编计划的板坯，禁止压坯
					"group by stock_place_no "
					"union all "
					"select distinct to_stock_place_no stock_place_no, count(1) mat_num from twma0 "
					"where to_stock_place_no > '  ' and stock_no = @stock_no and stock_oper_order like '1%' "
					"and mat_no in (select mat_no from tmmsm01 where heat_no = @heat_no) "
					"group by to_stock_place_no "
					") group by stock_place_no  order by stock_place_no "
					;
			}
			
			execute_sql.SetCommandText(sqlstr);
			execute_sql.Parameters.Set("stock_no", stock_no);
			execute_sql.Parameters.Set("heat_no", twma1["HEAT_NO"].ToString());
			execute_sql.Parameters.Set("order_no", twma1["ORDER_NO"].ToString());
			execute_sql.ExecuteReader();
			while (execute_sql.Read())
			{
				k++;
				Log::Trace("", __FUNCTION__, "同合同推荐 tmmsm01 for k ={0}", k);
				stock_place_no = execute_sql.GetString(1);
				mat_num = execute_sql.GetDecimal(2);
				Log::Trace("", __FUNCTION__, "stock_place_no={0}", stock_place_no);
				Log::Trace("", __FUNCTION__, "mat_num={0}", mat_num);

				twm04.Reset();
				twm04["STOCK_PLACE_NO"] = stock_place_no;
				if (!twm04.Query("STOCK_PLACE_NO"))
				{
					sprintf(s.msg, "twm04没有此库位信息【" + stock_place_no + "】.");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (twm04["STOCK_PLACE_TYPE"].ToString().Trim() != "0")
				{
					Log::Trace("", __FUNCTION__, "非正常库位，不堆放");
					continue;
				}
				if (twm04["MAX_LAYER_COUNT"].ToDecimal() <= mat_num)
				{
					Log::Trace("", __FUNCTION__, "库位最大堆放数已满，不堆放");
					continue;
				}
				/*if (twm04["STOCK_STATUS"].ToString().Trim() == "1" || twm04["STOCK_STATUS"].ToString().Trim() == "9")
				{
					Log::Trace("", __FUNCTION__, "库位封锁，或已满，不堆放");
					continue;
				}*/

				layernum = 0;
				//计算推荐库位上材料总数量：已堆放数量 + 已推荐数量。
				sqlstr1 = " select sum(layernum) from ( "
					"select count(1) layernum from tmmsm01 where in_flag = '1' and stock_place_no = @stock_place_no and stock_no = 'F11' union all "
					"select count(1) layernum from twma0 where to_stock_place_no = @stock_place_no and stock_no = 'F11' and stock_oper_order like '1%' ) ";
				com_sql.SetCommandText(sqlstr1);
				com_sql.Parameters.Set("stock_place_no", stock_place_no);
				com_sql.ExecuteReader();
				if (com_sql.Read())
				{
					layernum = com_sql.GetDecimal(1);
				}
				com_sql.Close();
				Log::Trace("", __FUNCTION__, "111 现堆放层号 layernum={0}", layernum);

				if (twm04["MAX_LAYER_COUNT"].ToDecimal() <= layernum)
				{
					Log::Trace("", __FUNCTION__, "超过库位最大堆放数量[" + twm04["MAX_LAYER_COUNT"].ToString()+ "]，不能堆放");
					continue;
				}
				
				rn_stock_place_no = stock_place_no;
				break;
			}
			execute_sql.Close();

			
			Log::Trace("", __FUNCTION__, "------- 同合同推荐库位[{0}] -------", rn_stock_place_no);


			// 没有同合同库位，推荐同内部钢种同宽度同长度库位
			if (rn_stock_place_no.Trim() == "")
			{
				Log::Trace("", __FUNCTION__, "没有同合同库位，推荐同内部钢种同宽度同长度库位");
				Log::Trace("", __FUNCTION__, " twma1.SG_SIGN = [{0}]", twma1["SG_SIGN"].ToString());
				Log::Trace("", __FUNCTION__, " twma1.MAT_ACT_WIDTH = [{0}]", twma1["MAT_ACT_WIDTH"].ToString());
				sqlstr =
					"select distinct stock_place_no, sum(mat_num) mat_num from( "
					"select distinct stock_place_no, count(1) mat_num from tmmsm01 "
					"where in_flag = '1' and stock_no = @stock_no and st_no = @st_no and mat_act_width = @mat_act_width and mat_act_len = @mat_act_len "
					"and stock_place_no in (select distinct stock_place_no from tmmsm01 where in_flag = '1' and stock_no = @stock_no and plan_no = ' ') "  //已编计划的板坯，禁止压坯
					"group by stock_place_no "
					"union all "
					"select distinct to_stock_place_no stock_place_no, count(1) mat_num from twma0 "
					"where to_stock_place_no > '  ' and stock_no = @stock_no and stock_oper_order like '1%' "
					"and mat_no in (select mat_no from tmmsm01 where st_no = @st_no and mat_act_width = @mat_act_width and mat_act_len = @mat_act_len) "
					"group by to_stock_place_no "
					") group by stock_place_no  order by stock_place_no "
					;
				execute_sql.SetCommandText(sqlstr);
				execute_sql.Parameters.Set("stock_no", stock_no);
				execute_sql.Parameters.Set("st_no", twma1["SG_SIGN"].ToString());
				execute_sql.Parameters.Set("mat_act_width", twma1["MAT_ACT_WIDTH"].ToDecimal());
				execute_sql.Parameters.Set("mat_act_len", twma1["MAT_ACT_LEN"].ToDecimal());
				execute_sql.ExecuteReader();
				while (execute_sql.Read())
				{
					j++;
					Log::Trace("", __FUNCTION__, "twm04 for j ={0}", j);
					stock_place_no = execute_sql.GetString(1);
					hall_no = execute_sql.GetString(2);
					Log::Trace("", __FUNCTION__, "stock_place_no={0}", stock_place_no);
					Log::Trace("", __FUNCTION__, "hall_no={0}", hall_no);

					twm04.Reset();
					twm04["STOCK_PLACE_NO"] = stock_place_no;
					if (!twm04.Query("STOCK_PLACE_NO"))
					{
						sprintf(s.msg, "twm04没有此库位信息【" + stock_place_no + "】.");
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
					if (twm04["STOCK_PLACE_TYPE"].ToString().Trim() != "0")
					{
						Log::Trace("", __FUNCTION__, "非正常库位，不堆放");
						continue;
					}
					/*if (twm04["STOCK_STATUS"].ToString().Trim() == "1" || twm04["STOCK_STATUS"].ToString().Trim() == "9")
					{
						Log::Trace("", __FUNCTION__, "库位封锁，或已满，不堆放");
						continue;
					}*/
					

					layernum = 0;
					//计算推荐库位上材料总数量：已堆放数量 + 已推荐数量。
					sqlstr1 = " select sum(layernum) from ( "
						"select count(1) layernum from tmmsm01 where in_flag = '1' and  stock_place_no = @stock_place_no and stock_no = 'F11' union all "
						"select count(1) layernum from twma0 where to_stock_place_no = @stock_place_no and stock_no = 'F11' and stock_oper_order like '1%' ) ";
					com_sql.SetCommandText(sqlstr1);
					com_sql.Parameters.Set("stock_place_no", stock_place_no);
					com_sql.ExecuteReader();
					if (com_sql.Read())
					{
						layernum = com_sql.GetDecimal(1);
					}
					com_sql.Close();
					Log::Trace("", __FUNCTION__, "222 现堆放层号  layernum={0}", layernum);

					if (twm04["MAX_LAYER_COUNT"].ToDecimal() <= layernum)
					{
						Log::Trace("", __FUNCTION__, "超过库位最大堆放数量" + twm04["MAX_LAYER_COUNT"].ToString() + "，不能堆放");
						continue;
					}

					rn_stock_place_no = stock_place_no;
					break;
				}
				execute_sql.Close();
								
				Log::Trace("", __FUNCTION__, "------- 没有同合同库位，推荐同内部钢种同宽度同长度库位[{0}] -------", rn_stock_place_no);
			}//end if 没有同合同库位，推荐同内部钢种同宽度同长度库位

			//最后推荐新库位
			if (rn_stock_place_no.Trim() == "")
			{
				Log::Trace("", __FUNCTION__, "推荐新库位");  //MAT_PILE_MODE = 1 冷坯  2 回炉坯  3 热坯  4 不启用
				sqlstr =
					" select stock_place_no,hall_no from twm04 "
					"where stock_no = @stock_no and STOCK_PLACE_TYPE = '0' and MAT_PILE_MODE not in ('2','4') and STOCK_STATUS != '1' "
					"and stock_place_no not in (select distinct stock_place_no from tmmsm01 where in_flag = '1' and stock_no = @stock_no) " 
					"and stock_place_no not in (select distinct to_stock_place_no from twma0 where stock_oper_order like '1%' and stock_no = @stock_no) " 
					"order by stock_place_no  ";
				execute_sql.SetCommandText(sqlstr);
				execute_sql.Parameters.Set("stock_no", stock_no);
				execute_sql.ExecuteReader();
				while (execute_sql.Read())
				{
					n++;
					Log::Trace("", __FUNCTION__, "twm04 for n ={0}", n);
					stock_place_no = execute_sql.GetString(1);
					hall_no = execute_sql.GetString(2);
					Log::Trace("", __FUNCTION__, "stock_place_no={0}", stock_place_no);
					Log::Trace("", __FUNCTION__, "hall_no={0}", hall_no);

					twm04.Reset();
					twm04["STOCK_PLACE_NO"] = stock_place_no;
					if (!twm04.Query("STOCK_PLACE_NO"))
					{
						sprintf(s.msg, "twm04没有此库位信息【" + stock_place_no + "】.");
						throw CApplicationException(-1, s.msg, s.svc_name);
					}

					layernum = 0;
					//计算推荐库位上材料总数量：已堆放数量 + 已推荐数量。
					sqlstr1 = " select sum(layernum) from ( "
						"select count(1) layernum from tmmsm01 where in_flag = '1' and  stock_place_no = @stock_place_no and stock_no = 'F11' union all "
						"select count(1) layernum from twma0 where to_stock_place_no = @stock_place_no and stock_no = 'F11' and stock_oper_order like '1%' ) ";
					com_sql.SetCommandText(sqlstr1);
					com_sql.Parameters.Set("stock_place_no", stock_place_no);
					com_sql.ExecuteReader();
					if (com_sql.Read())
					{
						layernum = com_sql.GetDecimal(1);
					}
					com_sql.Close();
					Log::Trace("", __FUNCTION__, "333 现堆放层号  layernum={0}", layernum);

					if (twm04["MAX_LAYER_COUNT"].ToDecimal() <= layernum)
					{
						Log::Trace("", __FUNCTION__, "超过库位最大堆放数量" + twm04["MAX_LAYER_COUNT"].ToString() + "，不能堆放");
						continue;
					}
					
					rn_stock_place_no = stock_place_no;
					break;
				}
				execute_sql.Close();

				
				Log::Trace("", __FUNCTION__, "------- 最后推荐新库位 [{0}] -------", rn_stock_place_no);
			}//end if 最后推荐新库位

			Log::Trace("", __FUNCTION__, "------- 最后推荐库位[{0}] -------", rn_stock_place_no);
			bcls_ret->Tables["STOCK_PLACE_NO"].Rows[0]["STOCK_PLACE_NO"] = rn_stock_place_no;

		}
		else if (stock_oper_order.SubstringNE(0, 1) == "2")
		{
			Log::Trace("", __FUNCTION__, "-------- 出库推荐库位 --------");
		}
		else if(stock_oper_order.SubstringNE(0, 1) == "3")
		{
			Log::Trace("", __FUNCTION__, "-------- 倒垛推荐库位 --------");
		}
		else
		{
			Log::Trace("", __FUNCTION__, "-------- 无法识别的库操作类型 --------");
		}

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
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}