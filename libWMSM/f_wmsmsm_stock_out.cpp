/* **************************************************************************
*	Copyright (c) Baosight Corporation 2008 . All Rights Reserved.
*  	BM2PES 宝信生产执行系统
*****************************************************************************
*  程序名称			: f_wm00_stock_out
*  程序描述			: 仓库出库主函数
*  备注说明			:
*  修改历史			:
*  		henno 2016-09-28			(ADD)程序建立
*			... ...
* **************************************************************************** */
/* C/C++ 的标准头文件部分 */
#include "stdafx.h"		// 框架头，不可删除 




BM2_FUNCTION_IMPORT
int f_wmsmsm_stock_update(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);

BM2_FUNCTION_IMPORT
int f_wmsmsm_mm0099(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);


BM2_FUNCTION_IMPORT
int f_wmsmsm_stock_log(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);



BM2_FUNCTION_IMPORT
int f_wmsm_21a009_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wmsm_load_proc(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wmsmsm_allot_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);

int f_mmsm99(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wmsm_t8p302_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_mmsm_e2t8m1_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);



BM2_FUNCTION_IMPORT
//int f_wm00_craneCmdMake_follow(CString matNo, CDbConnection * conn);

BM2_FUNCTION_EXPORT
int f_wmsmsm_stock_out(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	/* ***** 程序变量 ***** */
	int doFlag = 0;
	CString v_datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString v_mat_no = " ";
	CString v_stock_oper_order = " ";
	CString v_stock_no = " ";
	CString v_stock_place_no = " ";
	CString v_rowno = " ";
	CString v_column_no = "";
	CDecimal v_layerno = 0;
	CString v_stock_place_position = " ";
	CString v_vehicle_no = " ";
	CString v_crane_no = " ";

	CString SEQ_ID = "";                 //顺序号 件数
	CString LAYERNO = "";                 //层号
	CString STOCK_PLACE_POSITION = "";  //车内顺序号
	/*装车实绩号*/
	CString v_practice_no = " ";
	/*调拨单号*/
	CString v_c_deliveryid = " ";
	CString v_shift_no(""), v_shift_group("");
	/* ***** 数据库SQL操作字符串 ***** */
	CString	sqlstr = " ";

	/* ***** 数据库操作类定义 ***** */
	CDbCommand comm(conn);
	CDbCommand comm1(conn);

	/* ***** 定义表实体对象 ***** */
	
	CModel twma0 = CModel("TWMA0");
	CModel twma1 = CModel("TMMSM01");
	CModel twma2 = CModel("TWMA2");
	CModel twma2_old = CModel("TWMA2");
	CModel twma4 = CModel("TWMA4");
	CModel twm01 = CModel("TWM01");
	CModel twm41dj("TWM41DJ");
	CModel twmsm61 = CModel("TWMSM61");
	CModel tmmsm96 = CModel("TMMSM96");


	//调用物料函数
	EIClass bcls_rec_wmmm99;
	bcls_rec_wmmm99.Tables[0].set_TableName("WMMM99");
	bcls_rec_wmmm99.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
	bcls_rec_wmmm99.Tables[0].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");
	bcls_rec_wmmm99.Tables[0].Columns.Add(DT_STRING, "STOCK_OPER_ORDER_DIV");
	bcls_rec_wmmm99.Tables[0].Columns.Add(DT_STRING, "OLD_STOCK_NO");
	bcls_rec_wmmm99.Tables[0].Columns.Add(DT_STRING, "OLD_STOCK_PLACE_NO");
	bcls_rec_wmmm99.Tables[0].Columns.Add(DT_DECIMAL, "OLD_LAYER_NO");
	bcls_rec_wmmm99.Tables[0].Columns.Add(DT_STRING, "AIM_STORE");
	bcls_rec_wmmm99.Tables[0].Columns.Add(DT_STRING, "VEHICLE_NO");
	bcls_rec_wmmm99.Tables[0].Rows.Clear();


	


	//更新库位
	EIClass bcls_rec_stock_upd;
	bcls_rec_stock_upd.Tables[0].set_TableName("WM_STOCK_UPDATE");
	bcls_rec_stock_upd.Tables[0].Columns.Add(twma2);
	bcls_rec_stock_upd.Tables[0].Columns.Add(DT_STRING, "CRANE_NO");
	bcls_rec_stock_upd.Tables[0].Rows.Clear();


	//记录履历
	EIClass bcls_rec_stock_log;
	bcls_rec_stock_log.Tables[0].set_TableName("WM_STOCK_LOG");
	bcls_rec_stock_log.Tables[0].Rows.Clear();


	//发送电文
	EIClass bcls_load;
	bcls_load.Tables[0].set_TableName("21A009");
	bcls_load.Tables[0].Columns.Add(twmsm61);
	bcls_load.Tables[0].Rows.Clear();

	//发送电文
	EIClass bcls_allot;
	//bcls_load.Tables[0].set_TableName("21A009");
	bcls_allot.Tables[0].Columns.Add(twm41dj);
	bcls_allot.Tables[0].Rows.Clear();

	//调用物料事件
	EIClass mm0099;
	mm0099.Tables[0].set_TableName("MM0099");
	mm0099.Tables[0].Columns.Add(tmmsm96);
	mm0099.Tables[0].Rows.Clear();
	EIClass mm00991;
	mm00991.Tables[0].set_TableName("MM0099");
	mm00991.Tables[0].Columns.Add(tmmsm96);
	mm00991.Tables[0].Rows.Clear();

	EIClass inblock;
	inblock.Tables[0].Columns.Add(twma1);
	inblock.Tables[0].Rows.Clear();

	EIClass t8e2m1;
	t8e2m1.Tables[0].set_TableName("E2T8M1");
	t8e2m1.Tables["E2T8M1"].Columns.Add(DT_STRING, "MAT_NO");
	t8e2m1.Tables[0].Rows.Clear();


	/* ***** 应用程序开始处理 ***** */
	try
	{
		if (!bcls_rec->Tables.Contains("WM_STOCK"))
		{
			sprintf(s.msg, "函数f_wm00_stock_in中找不到接收块名[WM_STOCK]");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		v_practice_no= "XG6240" + v_datetime.SubstringNE(0, 8) + Db::QueryCString("SELECT LPAD(TO_CHAR(ZC_SJ.NEXTVAL), 4, '0') FROM DUAl");
		
		Log::Trace("", __FUNCTION__, "传入参数SEQ_ID\t[{0}]", bcls_rec->Tables["WM_STOCK"].Rows.get_Count());
		for (int iRow = 0; iRow < bcls_rec->Tables["WM_STOCK"].Rows.get_Count(); iRow++)
		{
			//获取传入参数
			twma0.Reset();
			twma0.MergeFrom(bcls_rec->Tables["WM_STOCK"].Rows[iRow]);
			twma0.TrimOrBlank();

			v_mat_no = twma0["MAT_NO"];
			v_stock_oper_order = twma0["STOCK_OPER_ORDER"];
			v_stock_no = twma0["STOCK_NO"];
			v_stock_place_no = twma0["STOCK_PLACE_NO"];
			v_layerno = twma0["LAYERNO"];
			v_stock_place_position = twma0["STOCK_PLACE_POSITION"];
			v_crane_no = twma0["CRANE_NO"];
			v_vehicle_no = twma0["VEHICLE_NO"];

			if (bcls_rec->Tables["WM_STOCK"].Columns.Contains("SEQ_ID"))
			{
				SEQ_ID = bcls_rec->Tables["WM_STOCK"].Rows[0]["SEQ_ID"].ToString().Trim();
			}
			if (bcls_rec->Tables["WM_STOCK"].Columns.Contains("LAYERNO"))
			{
				LAYERNO = bcls_rec->Tables["WM_STOCK"].Rows[0]["LAYERNO"].ToString().Trim();
			}
			if (bcls_rec->Tables["WM_STOCK"].Columns.Contains("STOCK_PLACE_POSITION"))
			{
				STOCK_PLACE_POSITION = bcls_rec->Tables["WM_STOCK"].Rows[0]["STOCK_PLACE_POSITION"].ToString().Trim();
			}

			Log::Trace("", __FUNCTION__, "传入参数v_mat_no\t[{0}]", v_mat_no);
			//Log::Trace("", __FUNCTION__, "传入参数v_stock_oper_order\t[{0}]", v_stock_oper_order);
			Log::Trace("", __FUNCTION__, "传入参数v_stock_no\t[{0}]", v_stock_no);
			Log::Trace("", __FUNCTION__, "传入参数v_stock_place_no\t[{0}]", v_stock_place_no);
			Log::Trace("", __FUNCTION__, "传入参数v_layerno\t[{0}]", v_layerno);
			Log::Trace("", __FUNCTION__, "传入参数v_stock_place_position\t[{0}]", v_stock_place_position);
			Log::Trace("", __FUNCTION__, "传入参数v_crane_no\t[{0}]", v_crane_no);
			Log::Trace("", __FUNCTION__, "传入参数v_vehicle_no\t[{0}]", v_vehicle_no);

			Log::Trace("", __FUNCTION__, "传入参数SEQ_ID\t[{0}]", SEQ_ID);
			Log::Trace("", __FUNCTION__, "传入参数LAYERNO\t[{0}]", LAYERNO);
			Log::Trace("", __FUNCTION__, "传入参数STOCK_PLACE_POSITION\t[{0}]", STOCK_PLACE_POSITION);

			if (v_stock_oper_order.Trim() == "")
			{
				sprintf(s.msg, "库操作指示不能为空.");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (v_stock_oper_order[0] != '2')
			{
				sprintf(s.msg, "库操作指示应为出库类事件.");
				continue;
			}
			if (v_mat_no.Trim() == "")
			{
				sprintf(s.msg, "材料号不能为空.");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (v_stock_no.Trim() == "")
			{
				/*sprintf(s.msg, "库区不能为空.");
				throw CApplicationException(-1, s.msg, log.Location);*/
			}

			


			//2. 检查A1
			twma1["MAT_NO"] = v_mat_no;
			if (!twma1.Query("MAT_NO"))
			{
				sprintf(s.msg, "TWMA1没有查询到[%s]。", (const char*)v_mat_no);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (twma1["C_STATESIGN"].ToString() != " "
				&& twma1["C_STATESIGN"].ToString() != "0")
			{
				sprintf(s.msg, "材料[%s]调拨状态为[%s]，不能调拨.", (const char*)v_mat_no,(const char*)twma1["C_STATESIGN"]);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (twma1["LOGISTICS_STATUS"].ToString() != "0"
				&& twma1["LOGISTICS_STATUS"].ToString() != "1"
				&& twma1["LOGISTICS_STATUS"].ToString() != "4")
			{
				sprintf(s.msg, "材料[%s]物流状态为[%s],不能装车.", (const char*)v_mat_no,(const char*)twma1["LOGISTICS_STATUS"]);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (twma1["COMPLEX_DECIDE_CODE"].ToString() != '1' && twma1["MAT_DESTION"].ToString() != '11')
			{
				sprintf(s.msg, "材料[%s]未综判不能调拨.", (const char*)v_mat_no);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if ((twma1["PRODUTE_CAL_WT"].ToDecimal()- twma1["MAT_ACT_WT"].ToDecimal()>0.5
				|| twma1["MAT_ACT_WT"].ToDecimal() - twma1["PRODUTE_CAL_WT"].ToDecimal() > 0.5)&& (twma1["MEND_FLAG"].ToString() == "0" || twma1["MEND_FLAG"].ToString() == " "))
			{
				/*sprintf(s.msg, "计算重量与系统重量差大于500kg，不能调拨.", (const char*)twma1["LOGISTICS_STATUS"]);
				throw CApplicationException(-1, s.msg, log.Location);*/
			}
			if (bcls_rec->Tables["WM_STOCK"].Rows[iRow]["C_ACCEPTDEPT"].ToString()=="6230"
				|| bcls_rec->Tables["WM_STOCK"].Rows[iRow]["C_ACCEPTDEPT"].ToString() == "6390"
				|| bcls_rec->Tables["WM_STOCK"].Rows[iRow]["C_ACCEPTDEPT"].ToString() == "6320"
				|| bcls_rec->Tables["WM_STOCK"].Rows[iRow]["C_ACCEPTDEPT"].ToString() == "6350"
				|| (bcls_rec->Tables["WM_STOCK"].Rows[iRow]["C_ACCEPTDEPT"].ToString() == "6310"&& twma1["PRODUCT_FLAG"].ToString()=="0"))
			{
				if (bcls_rec->Tables["WM_STOCK"].Rows[iRow]["C_ACCEPTDEPT"].ToString()!= bcls_rec->Tables["WM_STOCK"].Rows[iRow]["UNLOAD_CODE_FACTORY"].ToString())
				{
					sprintf(s.msg, "该材料[%s]调拨去向[%s]与物流去向[%s]不符.", (const char*)twma1["MAT_NO"], 
						(const char*)bcls_rec->Tables["WM_STOCK"].Rows[iRow]["C_ACCEPTDEPT"],(const char*)bcls_rec->Tables["WM_STOCK"].Rows[iRow]["UNLOAD_CODE_FACTORY"]);
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			//3. 检查A2
			twma2_old["MAT_NO"] = v_mat_no;
			if (!twma2_old.Query("MAT_NO"))
			{
				Log::Trace("", __FUNCTION__, "材料已不在库内。跳过");
				//continue;
				//sprintf(s.msg, "TWMA2没有查询到[%s]。", (const char*)v_mat_no);
				//throw CApplicationException(-1, s.msg, log.Location);
			}

			if (twma2_old["STOCK_PLACE_NO"].ToString().Trim() == "" &&
				twma2_old["OLD_STOCK_PLACE_NO"].ToString().Trim() != "")
			{
				//对于行车落下时  库位上原有的东西会被踢出去 记录原库位，按照原库位操作
				twma2_old["STOCK_PLACE_NO"] = twma2_old["OLD_STOCK_PLACE_NO"];
				twma2_old["STOCK_PLACE_POSITION"] = twma2_old["OLD_STOCK_PLACE_POSITION"];
			}
			if (bcls_rec->Tables["WM_STOCK"].Rows[iRow]["C_ACCEPTDEPT"].ToString().Trim() == ""
				|| bcls_rec->Tables["WM_STOCK"].Rows[iRow]["C_ACCEPTSTOCK"].ToString().Trim() == "")
			{
				sprintf(s.msg, "调拨工厂和调拨库房不能为空.");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			//如果没有目标库区，目标库区即为原库区
			if (v_stock_no.Trim() == "")
			{
				v_stock_no = twma2_old["STOCK_NO"];
			}


			//4. 更新库位（f_wmsmsm_stock_update）
			twma2["MAT_NO"] = v_mat_no;
			twma2["STOCK_NO"] = v_stock_no;
			twma2["STOCK_PLACE_NO"] = v_stock_place_no;
			Log::Trace("", __FUNCTION__, "111111111111111111111");
			twma2["LAYERNO"] = v_layerno;
			Log::Trace("", __FUNCTION__, "2222222222222222");
			twma2["STOCK_PLACE_POSITION"] = v_stock_place_position;
			twma2["VEHICLE_NO"] = v_vehicle_no;

			twma2.MergeTo(bcls_rec_stock_upd.Tables["WM_STOCK_UPDATE"], false);
			bcls_rec_stock_upd.Tables["WM_STOCK_UPDATE"].Rows[iRow]["CRANE_NO"] = v_crane_no;



			//5. 删A0（按材料号、类型）
			twma0["MAT_NO"] = v_mat_no;
			twma0["STOCK_OPER_ORDER"] = v_stock_oper_order;
			
			twma0.Delete("MAT_NO, STOCK_OPER_ORDER");
			


			//6. MM0099
			//调用物料跟踪
			bcls_rec_wmmm99.Tables["WMMM99"].Rows.Add();
			int kk = bcls_rec_wmmm99.Tables["WMMM99"].Rows.get_Count() - 1;
			bcls_rec_wmmm99.Tables["WMMM99"].Rows[kk]["MAT_NO"] = twma1["MAT_NO"];
			bcls_rec_wmmm99.Tables["WMMM99"].Rows[kk]["STOCK_OPER_ORDER"] = twma0["STOCK_OPER_ORDER"];
			bcls_rec_wmmm99.Tables["WMMM99"].Rows[kk]["OLD_STOCK_NO"] = twma2_old["STOCK_NO"];
			bcls_rec_wmmm99.Tables["WMMM99"].Rows[kk]["OLD_STOCK_PLACE_NO"] = twma2_old["STOCK_PLACE_NO"];
			bcls_rec_wmmm99.Tables["WMMM99"].Rows[kk]["OLD_LAYER_NO"] = twma2_old["LAYERNO"];
			bcls_rec_wmmm99.Tables["WMMM99"].Rows[kk]["AIM_STORE"] = v_stock_no; 
			bcls_rec_wmmm99.Tables["WMMM99"].Rows[kk]["VEHICLE_NO"] = v_vehicle_no;


			


		

			//发装车实绩
			twmsm61.CopyFrom(twma1);
			//twmsm61.Print();
			twmsm61.MergeFrom(bcls_rec->Tables["WM_STOCK"].Rows[iRow]);
			twmsm61["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			if (twmsm61["UNLOAD_CODE"].ToString() == "6240ZTXD1")
			{
				sprintf(s.msg, "自提卸点，不允许发物流。", (const char*)v_mat_no);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			twmsm61["REC_CREATOR"] = s.userid;
			twmsm61["PRACTICE_NO"] = v_practice_no;
			twmsm61["SG_SIGN"] = twma1["SG_GRADE_1"];
			twmsm61["WIDTH"] = twma1["MAT_WIDTH"];
			Log::Info("", __FUNCTION__, "f_mmsm10_trace_linke   =[{0}]", __LINE__);
			twmsm61["LENGTH"] = twma1["MAT_LEN"];
			twmsm61["THICK"] = twma1["MAT_THICK"];
			twmsm61["WEIGHT"] = twma1["MAT_ACT_WT"];
			twmsm61["DEAL_FLAG"] = "I";
			twmsm61["UNLOAD_STATE"] = "2";
			twmsm61["LOAD_END_TIME"] =  bcls_rec->Tables["WM_STOCK"].Rows[iRow]["OUT_STOCK_TIME"];
			if (bcls_rec->Tables["WM_STOCK"].Rows[iRow]["OUT_STOCK_TIME"].ToString().Trim() == "")
			{
				bcls_rec->Tables["WM_STOCK"].Rows[iRow]["OUT_STOCK_TIME"] = v_datetime;
			}
			f_epep_get_shift_group("SMCP", bcls_rec->Tables["WM_STOCK"].Rows[iRow]["OUT_STOCK_TIME"].ToString(), v_shift_no, v_shift_group, conn);
			twmsm61["SHIFT_NO"] = v_shift_no;
			twmsm61["SHIFT_GROUP"] = v_shift_group;
			Log::Info("", __FUNCTION__, "f_mmsm10_trace_linke   =[{0}]", __LINE__);
			//twmsm61["PRODUCT_TYPE"] = "1";
			twmsm61["TRANS_TYPE"] = "2";
			if (bcls_rec->Tables["WM_STOCK"].Rows[iRow]["UNLOAD_CODE_FACTORY"].ToString() == "6390")
			{
				twmsm61["MATERIAL_CODE"] = "HAB000000000000000";
			}
			else
			{
				twmsm61["MATERIAL_CODE"] = "HAA000000000000000";
			}
			
			Log::Info("", __FUNCTION__, "f_mmsm10_trace_linke   =[{0}]", __LINE__);
			
			twmsm61["DEALY_FLAG"] = "1";
			CString qx_type = Db::QueryCString("select CODE_DESC_3_CONTENT from TWMSMZD02 where CODE_CLASS='WM02' AND CODE='" + twma1["GUIDE_DEST"].ToString() + "'");
			if (twma1["UNLOAD_CODE"].ToString() == "TBZX01001" && qx_type.Find("3")<0)
			{
				sprintf(s.msg, "发往太北站的材料必须是指导去向类别为3的去向！");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			twmsm61.Insert();
			twmsm61.MergeTo(bcls_load.Tables["21A009"], false);
			

			//发调拨单
			twm41dj.Reset();
			twm41dj.MergeFrom(bcls_rec->Tables["WM_STOCK"].Rows[iRow]);
			twm41dj["REC_CREATOR"] = s.userid;
			twm41dj["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			twm41dj["C_DELIVERYID"] = "6240" + v_datetime.SubstringNE(0, 8) + Db::QueryCString("SELECT LPAD(TO_CHAR(DB_NO.NEXTVAL), 4, '0') FROM DUAl");
			twm41dj["C_QULITYTRACEID"] = twma1["HEAT_NO"];//炉号
			twm41dj["C_BATCHID"] = twma1["BATCH"];//批次号
			twm41dj["C_BATCHUNIT"] = twma1["MAT_NO"];//
			twm41dj["C_SENDDEPT"] = "6240";//发送工厂
			twm41dj["C_ACCEPTDEPT"] = bcls_rec->Tables["WM_STOCK"].Rows[iRow]["C_ACCEPTDEPT"];//接受工厂1
			if (twm41dj["C_ACCEPTDEPT"].ToString() != "6310" && twma1["PRODUCT_FLAG"].ToString() == "1")
			{
				sprintf(s.msg, "成品只能调往6310！");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			twm41dj["C_SENDSTOCK"] = twma1["LGORT"];//发送库房
			twm41dj["C_ACCEPTSTOCK"] = bcls_rec->Tables["WM_STOCK"].Rows[iRow]["C_ACCEPTSTOCK"];//接受库房
			twm41dj["DELIVERY_THICKNESS"] = twma1["MAT_THICK"];//厚度
			twm41dj["DELIVERY_WIDTH"] = twma1["MAT_WIDTH"];//宽度1
			twm41dj["STEELGRADE"] = twma1["ST_NO"];//钢牌号
			twm41dj["N_SENDAMOUNT"] = twma1["MAT_ACT_WT"];//发送重量
			twm41dj["C_SENDUNIT"] = "TON";//发送单位
			twm41dj["N_ACCEPTAMOUNT"] = twma1["MAT_WT"];//接收重量
			twm41dj["C_ACCEPTUNIT"] = "TON";//接收单位
			twm41dj["C_STATESIGN"] = "1";//调拨状态（1-未确认，2-接收，3-驳回）
			twm41dj["D_OPERATIONDATE"] = v_datetime;
			twm41dj["D_BILLDATE"] = v_datetime;
			Log::Info("", __FUNCTION__, "f_mmsm10_trace_linke   =[{0}]", __LINE__);
			twm41dj["T_OUTSTOCKTIME"] = v_datetime;
			if (twm41dj["C_ACCEPTDEPT"].ToString() == "6360")//2250
			{
				twm41dj["T_ACCEPTTIME"] = v_datetime;
				twm41dj["C_CLOSEGATETIME"] = v_datetime;
				twm41dj["T_UPLOADTIME"] = v_datetime;
				twm41dj["T_INSTOCKTIME"] = v_datetime;
				twm41dj["T_SALESCOMFIRMTIME"] = v_datetime;
				twm41dj["T_OVERRULETIME"] = v_datetime;
				twm41dj["D_REQUIREDATE"] = v_datetime;
			}
			twm41dj["I_STOCKMODE"] = "件次";
			twm41dj["C_REMARK"] = twma1["SG_GRADE_1"];
			twm41dj["I_RESERVECOL4"] = "0";//调拨类型（0-正常调拨，1-回退调拨）
			twm41dj["C_INSTOCKSIGN"] = "3";
			twm41dj["C_ISFREEZE"] = "FREE";//库存类型-
			Log::Info("", __FUNCTION__, "f_mmsm10_trace_linke   =[{0}]", __LINE__);
			twm41dj["C_STOCKSPEC"] = "FREE";//特殊库存标识
			twm41dj["C_ORDERID"] = twma1["ORDER_NO"];//合同号
			twm41dj["I_RESERVECOL3"] = twma1["MAT_LEN"];
			twm41dj["C_ACHIEVEID"] = "1";
			twm41dj["C_TRUCKNUM"] = twmsm61["TRUCK_NO"].ToString();
			Log::Info("", __FUNCTION__, "f_mmsm10_trace_linke   =[{0}]", __LINE__);
			if (twma1["PRODUCT_FLAG"].ToString()=="1")
			{
				if (bcls_rec->Tables["WM_STOCK"].Rows[iRow]["C_ACCEPTDEPT"].ToString() == "6390")
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
				if (bcls_rec->Tables["WM_STOCK"].Rows[iRow]["C_ACCEPTDEPT"].ToString() == "6390")
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
			Log::Info("", __FUNCTION__, "f_mmsm10_trace_linke   =[{0}]", __LINE__);
			if (twm41dj["C_ACCEPTDEPT"].ToString() == "6310" && twma1["PRODUCT_FLAG"].ToString() == "1" && twma1["ORDER_NO"].ToString().Trim() == "")
			{
				/*sprintf(s.msg, "调往型材的成品不能为余材！");
				throw CApplicationException(-1, s.msg, log.Location);*/
			}
			twm41dj.Insert();
			twm41dj.MergeTo(bcls_allot.Tables[0], false);

			//13. 写A4
			twma4.CopyFrom(twma1);
			twma4["STOCK_OPER_ORDER"] = v_stock_oper_order;
			twma4["STOCK_NO"] = twma2_old["STOCK_NO"];// v_stock_no;
			twma4["STOCK_PLACE_NO"] = v_stock_place_no;
			twma4["LAYERNO"] = v_layerno;
			twma4["FROM_STOCK_NO"] = twma2_old["STOCK_NO"];
			twma4["FROM_STOCK_PLACE_NO"] = twma2_old["STOCK_PLACE_NO"];
			twma4["CRANE_NO"] = v_crane_no;
			twma4["VEHICLE_NO"] = v_vehicle_no;

			twma4["EVENT_DESC"] = "出库";
			twma4["C_DELIVERYID"] = twm41dj["C_DELIVERYID"];
			twma4["C_ACCEPTDEPT"] = twm41dj["C_ACCEPTDEPT"];
			twma4["C_ACCEPTSTOCK"] = twm41dj["C_ACCEPTSTOCK"];
			twma4["PRACTICE_NO"] = v_practice_no;
			twma4["TRUCK_NO"] = v_vehicle_no;
			twma4["TRUCK_BOARD_NO"] = v_vehicle_no;
			twma4["LOAD_CODE_FACTORY"] = twmsm61["LOAD_CODE_FACTORY"];
			twma4["LOAD_CODE_AREA"] = twmsm61["LOAD_CODE_AREA"];
			twma4["LOAD_CODE"] = twmsm61["LOAD_CODE"];
			twma4["UNLOAD_CODE"] = twmsm61["UNLOAD_CODE"];
			twma4["UNLOAD_CODE_AREA"] = twmsm61["UNLOAD_CODE_AREA"];
			twma4["UNLOAD_CODE_FACTORY"] = twmsm61["UNLOAD_CODE_FACTORY"];
				Log::Info("", __FUNCTION__, "bcls_rec_stock_log.Tables[WM_STOCK_LOG]   =[{0}]", bcls_rec_stock_log.Tables["WM_STOCK_LOG"].Columns.get_Count());
			twma4.MergeTo(bcls_rec_stock_log.Tables["WM_STOCK_LOG"], false);
			Log::Info("", __FUNCTION__, "bcls_rec_stock_log.Tables[WM_STOCK_LOG]   =[{0}]", bcls_rec_stock_log.Tables["WM_STOCK_LOG"].Columns.get_Count());

			if (twma1["HR_SEND_FLAG"].ToString() == "1" && twm41dj["C_ACCEPTDEPT"].ToString() != "6360")
			{
				twma1.MergeTo(inblock.Tables[0], false);
			}

			//15、调物流事件
			tmmsm96.Reset();
			tmmsm96.CopyFrom(twma1);
			tmmsm96["LOGISTICS_STATUS"] = "2";//2--装车确认
			tmmsm96["FACTORY_TO"] = twmsm61["UNLOAD_CODE_FACTORY"];
			tmmsm96["DST_STOCK_CODE"] = twmsm61["UNLOAD_CODE_AREA"];
			tmmsm96["UNLOAD_CODE"] = twmsm61["UNLOAD_CODE"];
			tmmsm96["OUT_STOCK_TIME"] = v_datetime;
			tmmsm96["PRACTICE_NO"] = v_practice_no;
			tmmsm96["EVENT_ID"] = "MM77";
			tmmsm96["SYSTEM_ID"] = "MMSM";
			tmmsm96["EVENT_LINE_TYPE"] = "00";
			tmmsm96["FUNC_ID"] = s.svc_name;
			tmmsm96.MergeTo(mm0099.Tables["MM0099"], false);
			Log::Info("", __FUNCTION__, "f_mmsm10_trace_linke   =[{0}]", __LINE__);
			//16、调调拨事件
			
			tmmsm96.Reset();
			tmmsm96.CopyFrom(twma1);
			tmmsm96["C_STATESIGN"] = "1";//1--正向调拨出库，3--正向调拨完成
			tmmsm96["C_DELIVERYID"] = twm41dj["C_DELIVERYID"];
			tmmsm96["C_DELIVERY_FAC"] = twm41dj["C_ACCEPTDEPT"];
			tmmsm96["C_DELIVERY_STOCK"] = twm41dj["C_ACCEPTSTOCK"];
			tmmsm96["TRAN_TIME"] = v_datetime;
			tmmsm96["EVENT_ID"] = "MM76";
			tmmsm96["SYSTEM_ID"] = "MMSM";
			tmmsm96["EVENT_LINE_TYPE"] = "00";
			tmmsm96["FUNC_ID"] = s.svc_name;
			tmmsm96.MergeTo(mm00991.Tables["MM0099"], false);
			Log::Info("", __FUNCTION__, "f_mmsm10_trace_linke   =[{0}]", __LINE__);

			twma1.MergeTo(t8e2m1.Tables["E2T8M1"], false);
		}


		
		

		//调用更新库位函数
		if (bcls_rec_stock_upd.Tables[0].Rows.get_Count() > 0)
		{
			//doFlag = f_wmsmsm_stock_update(&bcls_rec_stock_upd, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}

		

		//调用物料/电文函数
		if (bcls_rec_wmmm99.Tables[0].Rows.get_Count() > 0)
		{
			doFlag = f_wmsmsm_mm0099(&bcls_rec_wmmm99, bcls_ret, conn);
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
		if (mm00991.Tables["MM0099"].Rows.get_Count() > 0)
		{
			doFlag = f_mmsm99(&mm00991, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}

		//调用履历函数
		if (bcls_rec_stock_log.Tables[0].Rows.get_Count() > 0)
		{
			doFlag = f_wmsmsm_stock_log(&bcls_rec_stock_log, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}

		//发装车实绩电文
		if (bcls_load.Tables[0].Rows.get_Count() > 0)
		{
			doFlag = f_wmsm_21a009_snd(&bcls_load, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
			doFlag = f_wmsm_load_proc(&bcls_load, bcls_ret, conn);
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

		if (inblock.Tables[0].Rows.get_Count() > 0) {
			doFlag = f_wmsm_t8p302_snd(&inblock, bcls_ret, conn);
			if (doFlag < 0) {
				throw CApplicationException(-1, s.msg, s.svc_name);
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
