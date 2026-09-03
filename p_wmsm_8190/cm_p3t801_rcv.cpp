/*
*  程序名称			: cm_p3t801_rcv
*  程序描述			: 2250板坯装车信息电文
*
*  	2023-11-9 	李振			(ADD)程序建立
*			... ...
* **************************************************************************** */
/*<remark>=========================================================
 <summary>
 2250板坯装车信息电文
 1、更新计划，添加材料
 2、发送调拨单

 <para>数据库表：TWMSM61(装车实绩表)         </para>
 </summary>
 <returns>电文处理成功与否</returns>
===========================================================</remark>*/

/* C/C++ 的标准头文件部分 */

#include "stdafx.h"
#include "epex.h"
int f_mmsm99(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wm00_queue(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
BM2_FUNCTION_IMPORT
int f_wmsm_21a009_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wmsmsm_allot_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_mmsm_e2t8m1_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);


BM2F_ENTERACE_TELE(cm_p3t801_rcv)
int f_cm_p3t801_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);
	/* ***** 静态变量定义 ***** */
	int doFlag = 0;
	int fetchRowCount = 0;
	int row_count = 0, i = 0, ret = 0;
	int blkNum = 0;
	int wm00que_count = 0;
	int mm0099_count = 0;


	CString lpsz_user_id, c_datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString	lpsz_out_div;
	EIClass sm_bcls_rec;

	CModel tmmsm96("TMMSM96");
	CModel twmsm61("TWMSM61");
	CModel twm41dj("TWM41DJ");
	CModel twmsm64("TWMSM64");
	CModel tmmsm01("TMMSM01");
	CModel hmmsm01("HMMSM01");

	/* ***** 电文变量定义 ***** */
	CString    c_mat_no;
	CString    c_ready_bill_no;
	CString    c_order_no;
	CString    c_red_cause_desc;
	CString    c_rec_revisor;
	CString    c_rec_revise_time;
	CString    c_red_flag;


	/* ***** 程序变量 ***** */
	CString c_user = " ", c_tc_no = " ";

	/* ***** 数据库SQL操作字符串 ***** */
	CString	sqlstr(""), sqlstr1(""), sqlstr2(""), sqlstr3(""), sqlstr4(""), sqlstr5("");
	CString v_shift_no(""), v_shift_group("");
	/* ***** 数据库操作类定义 ***** */
	CDbCommand cmd_sql(conn);

	CString plan_no,load_fac,load_area,load_code,unload_fac,unload_area,unload_code;

	/* ***** 应用程序开始处理 ***** */
	if (!bcls_rec->Tables.Contains("MM0099")) {
		bcls_rec->Tables.Add("MM0099");
		bcls_rec->Tables["MM0099"].Columns.Add(tmmsm96);
	}
	bcls_rec->Tables["MM0099"].Rows.Clear();


	bool t_table = true;
	EIClass bcls_load;
	bcls_load.Tables[0].set_TableName("21A009");
	bcls_load.Tables[0].Columns.Add(twmsm61);
	bcls_load.Tables[0].Rows.Clear();

	EIClass bcls_allot;
	//bcls_load.Tables[0].set_TableName("21A009");
	bcls_allot.Tables[0].Columns.Add(twm41dj);
	bcls_allot.Tables[0].Rows.Clear();

	EIClass mm0099;
	mm0099.Tables[0].set_TableName("MM0099");
	mm0099.Tables[0].Columns.Add(tmmsm96);
	mm0099.Tables[0].Rows.Clear();

	EIClass t8e2m1;
	t8e2m1.Tables[0].set_TableName("E2T8M1");
	t8e2m1.Tables["E2T8M1"].Columns.Add(DT_STRING, "MAT_NO");
	t8e2m1.Tables[0].Rows.Clear();

	try
	{
		
		CString	v_practice_no = "XG6240" + c_datetime.SubstringNE(0, 8) + Db::QueryCString("SELECT LPAD(TO_CHAR(ZC_SJ.NEXTVAL), 4, '0') FROM DUAl");
		sqlstr = " select PLAN_NO,LOAD_CODE_FACTORY,LOAD_CODE_AREA,LOAD_CODE,UNLOAD_CODE_FACTORY,UNLOAD_CODE_AREA,UNLOAD_CODE\
				from twmsm60\
			where DEAL_FLAG = 'I'\
				AND LOAD_CODE = '" + bcls_rec->Tables["ZCHO_2250ZCXX_RFC1"].Rows[0]["LOAD_PLACE"].ToString() + "'\
				AND UNLOAD_CODE = '" + bcls_rec->Tables["ZCHO_2250ZCXX_RFC1"].Rows[0]["UNLOAD_PLACE"].ToString() + "'\
				AND SUBSTR2(PLAN_START_TIME, 0, 8) = '" + c_datetime.SubstringNE(0, 8) + "' ";
		Log::Trace("", __FUNCTION__, "sqlstr{0}", sqlstr);
		cmd_sql.SetCommandText(sqlstr);
		cmd_sql.ExecuteReader();
		if (cmd_sql.Read())
		{
			 plan_no = cmd_sql.GetString(1);
			 load_fac = cmd_sql.GetString(2);
			 load_area = cmd_sql.GetString(3);
			 load_code = cmd_sql.GetString(4);
			 unload_fac = cmd_sql.GetString(5);
			 unload_area = cmd_sql.GetString(6);
			 unload_code = cmd_sql.GetString(7);
		}
		else {
			sprintf(s.msg, "没有该装卸点的计划。" );
			throw CApplicationException(-1, s.msg, log.Location);
		}
		cmd_sql.Close();



		for (int i = 0; i < bcls_rec->Tables["ZCHO_2250ZCXX_RFC1"].Rows.get_Count(); i++)
		{
			t_table = true;
			tmmsm01["SLAB_NO"]= bcls_rec->Tables["ZCHO_2250ZCXX_RFC1"].Rows[i]["MARKERNUMBER"].ToString();
			hmmsm01["SLAB_NO"] = bcls_rec->Tables["ZCHO_2250ZCXX_RFC1"].Rows[i]["MARKERNUMBER"].ToString();

			if (!tmmsm01.Query("SLAB_NO"))
			{
				if (!hmmsm01.Query("SLAB_NO"))
				{
					/*sprintf(s.msg, "材料号[%s]不存在。", (const char*)tmmsm01["SLAB_NO"]);
					throw CApplicationException(-1, s.msg, log.Location);*/
					continue;
				}
				t_table = false;
				tmmsm01.CopyFrom(hmmsm01);
			}

			twmsm64.Reset();
			twmsm64["REC_CREATE_TIME"] = c_datetime;
			twmsm64["MAT_NO"] = tmmsm01["MAT_NO"];
			twmsm64["SLAB_NO"] = tmmsm01["SLAB_NO"];
			twmsm64["BATCH"] = tmmsm01["BATCH"];
			twmsm64["DEAL_FLAG"] = bcls_rec->Tables["ZCHO_2250ZCXX_RFC1"].Rows[i]["DEAL_FLAG"].ToString();
			twmsm64["TRANS_TYPE"] = bcls_rec->Tables["ZCHO_2250ZCXX_RFC1"].Rows[i]["TRANS_TYPE"].ToString();
			twmsm64["TRUCK_NO"] = bcls_rec->Tables["ZCHO_2250ZCXX_RFC1"].Rows[i]["MCARTID"].ToString();
			twmsm64["TRUCK_BOARD_NO"] = bcls_rec->Tables["ZCHO_2250ZCXX_RFC1"].Rows[i]["SCARTID"].ToString();
			twmsm64["LOAD_CODE_FACTORY"] = bcls_rec->Tables["ZCHO_2250ZCXX_RFC1"].Rows[i]["LOAD_FAC"].ToString();
			twmsm64["LOAD_CODE_AREA"] = bcls_rec->Tables["ZCHO_2250ZCXX_RFC1"].Rows[i]["LOAD_AREA"].ToString();
			twmsm64["LOAD_STOCK_CODE"] = bcls_rec->Tables["ZCHO_2250ZCXX_RFC1"].Rows[i]["LOAD_STOCK_CODE"].ToString();
			twmsm64["LOAD_CODE"] = bcls_rec->Tables["ZCHO_2250ZCXX_RFC1"].Rows[i]["LOAD_PLACE"].ToString();
			twmsm64["UNLOAD_CODE_FACTORY"] = bcls_rec->Tables["ZCHO_2250ZCXX_RFC1"].Rows[i]["UNLOAD_FAC"].ToString();
			twmsm64["UNLOAD_CODE_AREA"] = bcls_rec->Tables["ZCHO_2250ZCXX_RFC1"].Rows[i]["UNLOAD_AREA"].ToString();
			twmsm64["UNLOAD_STOCK_CODE"] = bcls_rec->Tables["ZCHO_2250ZCXX_RFC1"].Rows[i]["UNLOAD_ARER_CODE"].ToString();
			twmsm64["UNLOAD_CODE"] = bcls_rec->Tables["ZCHO_2250ZCXX_RFC1"].Rows[i]["UNLOAD_PLACE"].ToString();
			twmsm64.TrimOrBlank();
			if (!twmsm64.Query("MAT_NO,DEAL_FLAG")&& bcls_rec->Tables["ZCHO_2250ZCXX_RFC1"].Rows[i]["DEAL_FLAG"].ToString() == "I")
			{
				twmsm64.Insert();
			}
			if (bcls_rec->Tables["ZCHO_2250ZCXX_RFC1"].Rows[i]["DEAL_FLAG"].ToString() == "D")
			{
				twmsm64.Delete("MAT_NO");
			}
				

			
			twmsm61.Reset();
			twmsm61.CopyFrom(tmmsm01);
			twmsm61.MergeFrom(bcls_rec->Tables["ZCHO_2250ZCXX_RFC1"].Rows[i]);
			if (twmsm61["DEAL_FLAG"].ToString() == "I")
			{
				if (tmmsm01["LOGISTICS_STATUS"].ToString() != "0"
					&& tmmsm01["LOGISTICS_STATUS"].ToString() != "1"
					&& tmmsm01["LOGISTICS_STATUS"].ToString() != "4")
				{
					/*sprintf(s.msg, "材料物流状态为[%s],不能装车.", (const char*)tmmsm01["LOGISTICS_STATUS"]);
					throw CApplicationException(-1, s.msg, log.Location);*/
					twmsm64["BACK_C1"] = "S";
					twmsm64.Update("BACK_C1", "MAT_NO");
					continue;
				}
				twmsm61["PLAN_NO"] = plan_no;

				twmsm61["TRUCK_NO"] = bcls_rec->Tables["ZCHO_2250ZCXX_RFC1"].Rows[i]["MCARTID"].ToString();
				twmsm61["TRUCK_BOARD_NO"] = bcls_rec->Tables["ZCHO_2250ZCXX_RFC1"].Rows[i]["SCARTID"].ToString();
				twmsm61["LOAD_CODE_FACTORY"] = bcls_rec->Tables["ZCHO_2250ZCXX_RFC1"].Rows[i]["LOAD_FAC"].ToString();
				twmsm61["LOAD_CODE_AREA"] = bcls_rec->Tables["ZCHO_2250ZCXX_RFC1"].Rows[i]["LOAD_AREA"].ToString();
				twmsm61["LOAD_STOCK_CODE"] = bcls_rec->Tables["ZCHO_2250ZCXX_RFC1"].Rows[i]["LOAD_STOCK_CODE"].ToString();
				twmsm61["LOAD_CODE"] = bcls_rec->Tables["ZCHO_2250ZCXX_RFC1"].Rows[i]["LOAD_PLACE"].ToString();
				twmsm61["UNLOAD_CODE_FACTORY"] = bcls_rec->Tables["ZCHO_2250ZCXX_RFC1"].Rows[i]["UNLOAD_FAC"].ToString();
				twmsm61["UNLOAD_CODE_AREA"] = bcls_rec->Tables["ZCHO_2250ZCXX_RFC1"].Rows[i]["UNLOAD_AREA"].ToString();
				twmsm61["UNLOAD_STOCK_CODE"] = bcls_rec->Tables["ZCHO_2250ZCXX_RFC1"].Rows[i]["UNLOAD_ARER_CODE"].ToString();
				twmsm61["UNLOAD_CODE"] = bcls_rec->Tables["ZCHO_2250ZCXX_RFC1"].Rows[i]["UNLOAD_PLACE"].ToString();
				twmsm61["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				twmsm61["REC_CREATOR"] = s.userid;
				twmsm61["PRACTICE_NO"] = v_practice_no;
				twmsm61["SG_SIGN"] = tmmsm01["SG_GRADE_1"];;
				twmsm61["WIDTH"] = tmmsm01["MAT_WIDTH"];
				twmsm61["LENGTH"] = tmmsm01["MAT_LEN"];
				twmsm61["THICK"] = tmmsm01["MAT_THICK"];
				twmsm61["WEIGHT"] = tmmsm01["MAT_WT"];
				twmsm61["DEAL_FLAG"] = bcls_rec->Tables["ZCHO_2250ZCXX_RFC1"].Rows[i]["DEAL_FLAG"].ToString();
				twmsm61["UNLOAD_STATE"] = "2";
				twmsm61["LOAD_END_TIME"] = c_datetime;
				twmsm61["OUT_STOCK_TIME"] = c_datetime;
				f_epep_get_shift_group("SMCP", c_datetime, v_shift_no, v_shift_group, conn);
				twmsm61["SHIFT_NO"] = v_shift_no;
				twmsm61["SHIFT_GROUP"] = v_shift_group;
				twmsm61["TRANS_TYPE"] = "2";
				twm41dj["C_BATCHUNIT"] = tmmsm01["MAT_NO"];
				twm41dj["C_STATESIGN"] = "1";
				if (unload_fac == "6310"
					|| unload_fac == "6320"
					|| unload_fac == "6340"
					|| unload_fac == "6350"
					|| unload_fac == "6380"
					|| unload_fac == "6390")
				{
					twmsm61["DEALY_FLAG"] = "1";
				}
				else
				{
					twmsm61["DEALY_FLAG"] = "3";
				}

				if (bcls_rec->Tables["ZCHO_2250ZCXX_RFC1"].Rows[i]["UNLOAD_FAC"].ToString() == "6390")
				{
					twmsm61["MATERIAL_CODE"] = "HAB000000000000000";
				}
				else
				{
					twmsm61["MATERIAL_CODE"] = "HAA000000000000000";
				}


				twmsm61.TrimOrBlank();
			}
			else
			{
				sqlstr = " SELECT * FROM TWMSM61 WHERE MAT_NO='" + twmsm61["MAT_NO"].ToString() + "' AND UNLOAD_STATE='2' ORDER BY REC_CREATE_TIME DESC  ";
				Log::Trace("", __FUNCTION__, "{0}", sqlstr);
				cmd_sql.SetCommandText(sqlstr);
				cmd_sql.ExecuteReader();
				if (cmd_sql.Read())
				{
					cmd_sql.Fetch(twmsm61);
				}
				twmsm61["DEAL_FLAG"] = "D";
				cmd_sql.Close();
			}
		
			
			
		
			CString c_acceptstock = " ";
			if(bcls_rec->Tables["ZCHO_2250ZCXX_RFC1"].Rows[i]["DEAL_FLAG"].ToString()=="I"&& t_table)
			{
				if (unload_fac == "6310"
					|| unload_fac == "6320"
					|| unload_fac == "6340"
					|| unload_fac == "6350"
					|| unload_fac == "6380"
					|| unload_fac == "6390")
				{
					if (unload_fac == "6310")
						c_acceptstock = "6311";
					if (unload_fac == "6320")
						c_acceptstock = "6321";
					if (unload_fac == "6340")
						c_acceptstock = "6341";
					if (unload_fac == "6350")
						c_acceptstock = "6351";
					if (unload_fac == "6390")
						c_acceptstock = "6391";

					if (tmmsm01["C_STATESIGN"].ToString() != " "
						&& tmmsm01["C_STATESIGN"].ToString() != "0")
					{
						/*sprintf(s.msg, "材料调拨状态为[%s]，不能调拨.", (const char*)tmmsm01["LOGISTICS_STATUS"]);
						throw CApplicationException(-1, s.msg, log.Location);*/
						twmsm64["BACK_C1"] = "S";
						twmsm64.Update("BACK_C1", "MAT_NO");
						continue;
					}
					if (tmmsm01["COMPLEX_DECIDE_CODE"].ToString() != '1' && tmmsm01["MAT_DESTION"].ToString() != '11')
					{
						/*sprintf(s.msg, "材料未综判不能调拨.", (const char*)tmmsm01["LOGISTICS_STATUS"]);
						throw CApplicationException(-1, s.msg, log.Location);*/
						twmsm64["BACK_C6"] = "材料未综判不能调拨";
						twmsm64.Update("BACK_C6", "MAT_NO");
						continue;
					}
					//发调拨单
					twm41dj.Reset();
					twm41dj.CopyFrom(tmmsm01);
					twm41dj["REC_CREATOR"] = s.userid;
					twm41dj["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
					twm41dj["C_DELIVERYID"] = "6240" + c_datetime.SubstringNE(0, 8) + Db::QueryCString("SELECT LPAD(TO_CHAR(DB_NO.NEXTVAL), 4, '0') FROM DUAl");
					twm41dj["C_QULITYTRACEID"] = tmmsm01["HEAT_NO"];//炉号
					twm41dj["C_BATCHID"] = tmmsm01["BATCH"];//批次号
					twm41dj["C_BATCHUNIT"] = tmmsm01["MAT_NO"];//
					twm41dj["C_SENDDEPT"] = "6240";//发送工厂
					twm41dj["C_ACCEPTDEPT"] = unload_fac;//接受工厂1
					if (twm41dj["C_ACCEPTDEPT"].ToString() != "6310" && tmmsm01["PRODUCT_FLAG"].ToString() == "1")
					{
						/*sprintf(s.msg, "成品只能调往6310！");
						throw CApplicationException(-1, s.msg, log.Location);*/
						twmsm64["BACK_C6"] = "成品只能调往6310!";
						twmsm64.Update("BACK_C6", "MAT_NO");
						continue;
					}
					twm41dj["C_SENDSTOCK"] = tmmsm01["LGORT"];//发送库房
					twm41dj["C_ACCEPTSTOCK"] = c_acceptstock;//接受库房
					twm41dj["DELIVERY_THICKNESS"] = tmmsm01["MAT_THICK"];//厚度
					twm41dj["DELIVERY_WIDTH"] = tmmsm01["MAT_WIDTH"];//宽度1
					twm41dj["STEELGRADE"] = tmmsm01["ST_NO"];//钢牌号
					twm41dj["N_SENDAMOUNT"] = tmmsm01["MAT_ACT_WT"];//发送重量
					twm41dj["C_SENDUNIT"] = "TON";//发送单位
					twm41dj["N_ACCEPTAMOUNT"] = tmmsm01["MAT_WT"];//接收重量
					twm41dj["C_ACCEPTUNIT"] = "TON";//接收单位
					twm41dj["C_STATESIGN"] = "1";//调拨状态（1-未确认，2-接收，3-驳回）
					twm41dj["D_OPERATIONDATE"] = c_datetime;
					twm41dj["D_BILLDATE"] = c_datetime;
					twm41dj["T_OUTSTOCKTIME"] = c_datetime;
					if (twm41dj["C_ACCEPTDEPT"].ToString() == "6360")//2250
					{
						twm41dj["T_ACCEPTTIME"] = c_datetime;
						twm41dj["C_CLOSEGATETIME"] = c_datetime;
						twm41dj["T_UPLOADTIME"] = c_datetime;
						twm41dj["T_INSTOCKTIME"] = c_datetime;
						twm41dj["T_SALESCOMFIRMTIME"] = c_datetime;
						twm41dj["T_OVERRULETIME"] = c_datetime;
						twm41dj["D_REQUIREDATE"] = c_datetime;
					}

					twm41dj["I_STOCKMODE"] = "件次";
					twm41dj["C_REMARK"] = tmmsm01["SG_GRADE_1"];
					twm41dj["I_RESERVECOL4"] = "0";//调拨类型（0-正常调拨，1-回退调拨）
					twm41dj["C_INSTOCKSIGN"] = "3";
					twm41dj["C_ISFREEZE"] = "FREE";//库存类型-
					twm41dj["C_STOCKSPEC"] = "FREE";//特殊库存标识
					twm41dj["C_ORDERID"] = tmmsm01["ORDER_NO"];//合同号
					twm41dj["I_RESERVECOL3"] = tmmsm01["MAT_LEN"];
					twm41dj["C_ACHIEVEID"] = "1";
					twm41dj["C_TRUCKNUM"] = bcls_rec->Tables["ZCHO_2250ZCXX_RFC1"].Rows[i]["MCARTID"].ToString().TrimOrBlank();
					if (tmmsm01["PRODUCT_FLAG"].ToString() == "1")
					{
						if (c_acceptstock != "6390")
						{
							twm41dj["C_PRODUCTID"] = "FAB000000000000000";
							twm41dj["C_PRODUCTNAME"] = "连铸中板坯";
						}
						else
						{
							twm41dj["C_PRODUCTID"] = "FAA000000000000000";
							twm41dj["C_PRODUCTNAME"] = "连铸卷板坯";
						}
					}
					else
					{
						if (c_acceptstock != "6390")
						{
							twm41dj["C_PRODUCTID"] = "HAB000000000000000";
							twm41dj["C_PRODUCTNAME"] = "连铸中板坯";
						}
						else
						{
							twm41dj["C_PRODUCTID"] = "HAA000000000000000";
							twm41dj["C_PRODUCTNAME"] = "连铸卷板坯";
						}
					}
					Log::Trace("", __FUNCTION__, "{0}", twm41dj["C_INSTOCKSIGN"].ToString());
					if (twm41dj["C_ACCEPTDEPT"].ToString() == "6310" && tmmsm01["PRODUCT_FLAG"].ToString() == "1" && tmmsm01["ORDER_NO"].ToString().Trim() == "")
					{
						/*sprintf(s.msg, "调往型材的成品不能为余材！");
						throw CApplicationException(-1, s.msg, log.Location);*/
						twmsm64["BACK_C6"] = "调往型材的成品不能为余材!";
						twmsm64.Update("BACK_C6", "MAT_NO");
						continue;
					}
					twm41dj.TrimOrBlank();
					twm41dj.Insert();
					twm41dj.MergeTo(bcls_allot.Tables[0], false);

					tmmsm96.Reset();
					tmmsm96.CopyFrom(tmmsm01);
					tmmsm96["C_STATESIGN"] = "1";//1--正向调拨出库，3--正向调拨完成
					tmmsm96["C_DELIVERYID"] = twm41dj["C_DELIVERYID"];
					tmmsm96["C_DELIVERY_FAC"] = twm41dj["C_ACCEPTDEPT"];
					tmmsm96["C_DELIVERY_STOCK"] = twm41dj["C_ACCEPTSTOCK"];
					tmmsm96["TRAN_TIME"] = c_datetime;
					tmmsm96["EVENT_ID"] = "MM76";
					tmmsm96["SYSTEM_ID"] = "MMSM";
					tmmsm96["EVENT_LINE_TYPE"] = "00";
					tmmsm96["FUNC_ID"] = s.svc_name;
					tmmsm96.MergeTo(mm0099.Tables["MM0099"], false);
				}

			}
			
			
			//twmsm61.Insert();
			twmsm61.MergeTo(bcls_load.Tables["21A009"], false);
			if (bcls_rec->Tables["ZCHO_2250ZCXX_RFC1"].Rows[i]["DEAL_FLAG"].ToString() == "I")
			{
				twmsm61.Insert();
				if (t_table)
				{
					twmsm61.MergeTo(t8e2m1.Tables["E2T8M1"], false);
				}
				
			}
			else
			{
				twmsm61.Delete("PRACTICE_NO,MAT_NO");
			}

			

			
			if (t_table)
			{
				tmmsm96.Reset();
				tmmsm96.CopyFrom(tmmsm01);
				if (bcls_rec->Tables["ZCHO_2250ZCXX_RFC1"].Rows[i]["DEAL_FLAG"].ToString() == "I")
				{
					tmmsm96["LOGISTICS_STATUS"] = "2";//2--装车确认
				}
				else
				{
					tmmsm96["LOGISTICS_STATUS"] = "0";//2--装车确认
				}
				tmmsm96["FACTORY_TO"] = twmsm61["UNLOAD_CODE_FACTORY"];
				tmmsm96["DST_STOCK_CODE"] = twmsm61["UNLOAD_CODE_AREA"];
				tmmsm96["UNLOAD_CODE"] = twmsm61["UNLOAD_CODE"];
				tmmsm96["PRACTICE_NO"] = twmsm61["PRACTICE_NO"];
				tmmsm96["OUT_STOCK_TIME"] = c_datetime;
				tmmsm96["EVENT_ID"] = "MM77";
				tmmsm96["SYSTEM_ID"] = "MMSM";
				tmmsm96["EVENT_LINE_TYPE"] = "00";
				tmmsm96["FUNC_ID"] = s.svc_name;
				tmmsm96.MergeTo(bcls_rec->Tables["MM0099"], false);
				Log::Info("", __FUNCTION__, "f_mmsm10_trace_linke   =[{0}]", __LINE__);

				
			}
			else  
			{
				hmmsm01["LOGISTICS_STATUS"] = "2";
				hmmsm01["FACTORY_TO"] = twmsm61["UNLOAD_CODE_FACTORY"];
				hmmsm01["DST_STOCK_CODE"] = twmsm61["UNLOAD_CODE_AREA"];
				hmmsm01["UNLOAD_CODE"] = twmsm61["UNLOAD_CODE"];
				hmmsm01["PRACTICE_NO"] = twmsm61["PRACTICE_NO"];
				hmmsm01["OUT_STOCK_TIME"] = c_datetime;
				hmmsm01.Update("LOGISTICS_STATUS,FACTORY_TO,DST_STOCK_CODE,UNLOAD_CODE,PRACTICE_NO,OUT_STOCK_TIME", "MAT_NO");
			}
			twmsm64["BACK_C1"] = "S";
			twmsm64.Update("BACK_C1", "MAT_NO");
			//continue;
		}
		
		if (bcls_rec->Tables["MM0099"].Rows.get_Count()>0)
		{
			doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		if (bcls_load.Tables[0].Rows.get_Count() > 0)
		{
			doFlag = f_wmsm_21a009_snd(&bcls_load, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		if (bcls_allot.Tables[0].Rows.get_Count() > 0) {
			doFlag = f_wmsmsm_allot_snd(&bcls_allot, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		//调用物料事件
		if (mm0099.Tables["MM0099"].Rows.get_Count() > 0)
		{
			doFlag = f_mmsm99(&mm0099, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		if (t8e2m1.Tables["E2T8M1"].Rows.get_Count() > 0)
		{
			doFlag = f_mmsm_e2t8m1_snd(&t8e2m1, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
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
	catch (const CApplicationException& ex)
	{
		//	strncpy(s.msg, (const char*)ex.GetMsg(), 399); //返回前台，与EI.EITuxedo.CallService()方法返回的EI.EIInfo对象的sys_info.msg参数对应
		s.flag = ex.GetCode();       //返回前台，与EI.EITuxedo.CallService()方法返回的EI.EIInfo对象的sys_info.flag参数对应
		Log::Error("", __FUNCTION__, "error=[{0}]", s.msg);
		doFlag = -1;
	}

	catch (const CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), 399); //返回前台，与EI.EITuxedo.CallService()方法返回的EI.EIInfo对象的sys_info.msg参数对应
		s.flag = ex.GetCode();       //返回前台，与EI.EITuxedo.CallService()方法返回的EI.EIInfo对象的sys_info.flag参数对应
		doFlag = -1;
	}

	return doFlag;
}
