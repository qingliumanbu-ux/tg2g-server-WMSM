/*=========================================================================
//程序名称:		f_ymsmSlabQtCheck
//隶属子系统:	WM
//产品名称:
//创建人员:		LLZ
//创建时间:		2014-06-4
//修改人员:
//修改日期:
//-----------------------------------------------------------------------

//=========================================================================*/

//#include "WM_Utility.h"
#include "stdafx.h"
#include "epex.h"
#include "math.h"
BM2_FUNCTION_IMPORT
BM2_FUNCTION_IMPORT
int f_wmsmsm_stock_log(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);



BM2_FUNCTION_IMPORT
int f_wmsm_21a009_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wmsm_load_proc(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wmsmsm_allot_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);

int f_mmsm99(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wmsm_t8p302_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_mmsm_e2t8m1_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);


BM2_FUNCTION_EXPORT
int f_wmsm_auto_db(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/*程序用变量*/
	int doFlag = 0;
	int ret = 0;
	int sqlid = 0;
	int blckNum = -1;

	int isLock = 1;//0为封锁，1为合格
	int blkNum = 0;
	CString c_datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString v_shift_no(""), v_shift_group("");
	CString s_c_acceptdept(""), s_c_acceptstock("");
	EPEX epex;


	/* Pro*c 标准头文件部分  */
	CString deal_flag = "";
	/********表结构引用*********/
	CModel twmsm61("TWMSM61");
	CModel twm41dj("TWM41DJ");
	CModel tmmsm01("TMMSM01");
	CModel twmsm64("TWMSM64");
	CModel twma0("TWMA0");
	CModel twmsm61lg("TWMSM61LG");
	CModel tmmsm96 = CModel("TMMSM96");
	CModel twma4 = CModel("TWMA4");


	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);
	CDbCommand cmd_inq2(conn);
	CDbCommand cmd_inq3(conn);
	CString sqlstr = "";

	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

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
	inblock.Tables[0].Columns.Add(tmmsm01);
	inblock.Tables[0].Rows.Clear();

	EIClass t8e2m1;
	t8e2m1.Tables[0].set_TableName("E2T8M1");
	t8e2m1.Tables["E2T8M1"].Columns.Add(DT_STRING, "MAT_NO");
	t8e2m1.Tables[0].Rows.Clear();

	try
	{
		CString v_practice_no = "XG6240" + datetime.SubstringNE(0, 8) + Db::QueryCString("SELECT LPAD(TO_CHAR(ZC_SJ.NEXTVAL), 4, '0') FROM DUAl");
		for (int iRow = 0; iRow < bcls_rec->Tables[0].Rows.get_Count(); iRow++)
		{
			if (bcls_rec->Tables[0].Rows[iRow]["UNLOAD_CODE"].ToString().SubstringNE(0, 4) == "6320")
			{
				s_c_acceptdept = "6320";
				s_c_acceptstock = "6321";
			}
			else if (bcls_rec->Tables[0].Rows[iRow]["UNLOAD_CODE"].ToString().SubstringNE(0, 4) == "6350")
			{
				s_c_acceptdept = "6350";
				s_c_acceptstock = "6351";
			}
			else if (bcls_rec->Tables[0].Rows[iRow]["UNLOAD_CODE"].ToString().SubstringNE(0, 4) == "6390")
			{
				s_c_acceptdept = "6390";
				s_c_acceptstock = "6391";
			}
			else if (bcls_rec->Tables[0].Rows[iRow]["UNLOAD_CODE"].ToString().SubstringNE(0, 4) == "6310")
			{
				s_c_acceptdept = "6310";
				s_c_acceptstock = "6311";
			}
			else if (bcls_rec->Tables[0].Rows[iRow]["UNLOAD_CODE"].ToString().SubstringNE(0, 4) == "6230")
			{
				s_c_acceptdept = "6230";
				s_c_acceptstock = "6235";
			}
			//获取传入参数
		
			tmmsm01["MAT_NO"] = bcls_rec->Tables[0].Rows[iRow]["MAT_NO"].ToString();
			if (!tmmsm01.Query("MAT_NO"))
			{
				sprintf(s.msg, "tmmsm01没有查询到[%s]。", (const char*)tmmsm01["MAT_NO"]);
				throw CApplicationException(-1, s.msg, log.Location);
			}
		

			//5. 删A0（按材料号、类型）
			twma0["MAT_NO"] = tmmsm01["MAT_NO"];
			twma0["STOCK_OPER_ORDER"] = "2A";

			twma0.Delete("MAT_NO, STOCK_OPER_ORDER");


			//发装车实绩
			twmsm61.CopyFrom(tmmsm01);
			//twmsm61.Print();
			twmsm61.MergeFrom(bcls_rec->Tables[0].Rows[iRow]);
			twmsm61["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			twmsm61["REC_CREATOR"] = s.userid;
			twmsm61["PRACTICE_NO"] = v_practice_no;
			twmsm61["SG_SIGN"] = tmmsm01["SG_GRADE_1"];
			twmsm61["WIDTH"] = tmmsm01["MAT_WIDTH"];
			Log::Info("", __FUNCTION__, "f_mmsm10_trace_linke   =[{0}]", __LINE__);
			twmsm61["LENGTH"] = tmmsm01["MAT_LEN"];
			twmsm61["THICK"] = tmmsm01["MAT_THICK"];
			twmsm61["WEIGHT"] = tmmsm01["MAT_ACT_WT"];
			twmsm61["DEAL_FLAG"] = "I";
			twmsm61["UNLOAD_STATE"] = "2";
			twmsm61["LOAD_END_TIME"] = bcls_rec->Tables[0].Rows[iRow]["OUT_STOCK_TIME"];
			if (bcls_rec->Tables[0].Rows[iRow]["OUT_STOCK_TIME"].ToString().Trim() == "")
			{
				bcls_rec->Tables[0].Rows[iRow]["OUT_STOCK_TIME"] = datetime;
			}
			f_epep_get_shift_group("SMCP", bcls_rec->Tables[0].Rows[iRow]["OUT_STOCK_TIME"].ToString(), v_shift_no, v_shift_group, conn);
			twmsm61["SHIFT_NO"] = v_shift_no;
			twmsm61["SHIFT_GROUP"] = v_shift_group;
			Log::Info("", __FUNCTION__, "f_mmsm10_trace_linke   =[{0}]", __LINE__);
			//twmsm61["PRODUCT_TYPE"] = "1";
			twmsm61["TRANS_TYPE"] = "2";
			if (bcls_rec->Tables[0].Rows[iRow]["UNLOAD_CODE_FACTORY"].ToString() == "6390")
			{
				twmsm61["MATERIAL_CODE"] = "HAB000000000000000";
			}
			else
			{
				twmsm61["MATERIAL_CODE"] = "HAA000000000000000";
			}

			Log::Info("", __FUNCTION__, "f_mmsm10_trace_linke   =[{0}]", __LINE__);

			twmsm61["DEALY_FLAG"] = "1";
			CString qx_type = Db::QueryCString("select CODE_DESC_3_CONTENT from TWMSMZD02 where CODE_CLASS='WM02' AND CODE='" + tmmsm01["GUIDE_DEST"].ToString() + "'");
			if (tmmsm01["UNLOAD_CODE"].ToString() == "TBZX01001" && qx_type.Find("3") < 0)
			{
				sprintf(s.msg, "发往太北站的材料必须是指导去向类别为3的去向！");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			twmsm61.Insert();
			twmsm61.MergeTo(bcls_load.Tables["21A009"], false);


			//发调拨单
			twm41dj.Reset();
			twm41dj.MergeFrom(bcls_rec->Tables[0].Rows[iRow]);
			twm41dj["REC_CREATOR"] = s.userid;
			twm41dj["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			twm41dj["C_DELIVERYID"] = "6240" + datetime.SubstringNE(0, 8) + Db::QueryCString("SELECT LPAD(TO_CHAR(DB_NO.NEXTVAL), 4, '0') FROM DUAl");
			twm41dj["C_QULITYTRACEID"] = tmmsm01["HEAT_NO"];//炉号
			twm41dj["C_BATCHID"] = tmmsm01["BATCH"];//批次号
			twm41dj["C_BATCHUNIT"] = tmmsm01["MAT_NO"];//
			twm41dj["C_SENDDEPT"] = "6240";//发送工厂
			twm41dj["C_ACCEPTDEPT"] = s_c_acceptdept;//接受工厂1
			twm41dj["C_SENDSTOCK"] = tmmsm01["LGORT"];//发送库房
			twm41dj["C_ACCEPTSTOCK"] = s_c_acceptstock;//接受库房
			twm41dj["DELIVERY_THICKNESS"] = tmmsm01["MAT_THICK"];//厚度
			twm41dj["DELIVERY_WIDTH"] = tmmsm01["MAT_WIDTH"];//宽度1
			twm41dj["STEELGRADE"] = tmmsm01["ST_NO"];//钢牌号
			twm41dj["N_SENDAMOUNT"] = tmmsm01["MAT_ACT_WT"];//发送重量
			twm41dj["C_SENDUNIT"] = "TON";//发送单位
			twm41dj["N_ACCEPTAMOUNT"] = tmmsm01["MAT_WT"];//接收重量
			twm41dj["C_ACCEPTUNIT"] = "TON";//接收单位
			twm41dj["C_STATESIGN"] = "1";//调拨状态（1-未确认，2-接收，3-驳回）
			twm41dj["D_OPERATIONDATE"] = datetime;
			twm41dj["D_BILLDATE"] = datetime;
			Log::Info("", __FUNCTION__, "f_mmsm10_trace_linke   =[{0}]", __LINE__);
			twm41dj["T_OUTSTOCKTIME"] = datetime;
			if (twm41dj["C_ACCEPTDEPT"].ToString() == "6360")//2250
			{
				twm41dj["T_ACCEPTTIME"] = datetime;
				twm41dj["C_CLOSEGATETIME"] = datetime;
				twm41dj["T_UPLOADTIME"] = datetime;
				twm41dj["T_INSTOCKTIME"] = datetime;
				twm41dj["T_SALESCOMFIRMTIME"] = datetime;
				twm41dj["T_OVERRULETIME"] = datetime;
				twm41dj["D_REQUIREDATE"] = datetime;
			}
			twm41dj["I_STOCKMODE"] = "件次";
			twm41dj["C_REMARK"] = tmmsm01["SG_GRADE_1"];
			twm41dj["I_RESERVECOL4"] = "0";//调拨类型（0-正常调拨，1-回退调拨）
			twm41dj["C_INSTOCKSIGN"] = "3";
			twm41dj["C_ISFREEZE"] = "FREE";//库存类型-
			Log::Info("", __FUNCTION__, "f_mmsm10_trace_linke   =[{0}]", __LINE__);
			twm41dj["C_STOCKSPEC"] = "FREE";//特殊库存标识
			twm41dj["C_ORDERID"] = tmmsm01["ORDER_NO"];//合同号
			twm41dj["I_RESERVECOL3"] = tmmsm01["MAT_LEN"];
			twm41dj["C_ACHIEVEID"] = "1";
			twm41dj["C_TRUCKNUM"] = twmsm61["TRUCK_NO"].ToString();
			Log::Info("", __FUNCTION__, "f_mmsm10_trace_linke   =[{0}]", __LINE__);
			if (tmmsm01["PRODUCT_FLAG"].ToString() == "1")
			{
				if (s_c_acceptdept != "6390")
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
				if (s_c_acceptdept != "6390")
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
			if (twm41dj["C_ACCEPTDEPT"].ToString() == "6310" && tmmsm01["PRODUCT_FLAG"].ToString() == "1" && tmmsm01["ORDER_NO"].ToString().Trim() == "")
			{
				sprintf(s.msg, "调往型材的成品不能为余材！");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			twm41dj.Insert();
			twm41dj.MergeTo(bcls_allot.Tables[0], false);

			//13. 写A4
			twma4.CopyFrom(tmmsm01);
			twma4["STOCK_OPER_ORDER"] = "2A";
			twma4["STOCK_NO"] = tmmsm01["STOCK_NO"];// v_stock_no;
			twma4["STOCK_PLACE_NO"] = tmmsm01["STOCK_PLACE_NO"];
			twma4["LAYERNO"] = tmmsm01["LAYERNO"];
			twma4["FROM_STOCK_NO"] = tmmsm01["STOCK_NO"];
			twma4["FROM_STOCK_PLACE_NO"] = tmmsm01["STOCK_PLACE_NO"];
			twma4["CRANE_NO"] = " ";
			twma4["VEHICLE_NO"] = " ";

			twma4["EVENT_DESC"] = "出库";
			twma4["C_DELIVERYID"] = twm41dj["C_DELIVERYID"];
			twma4["C_ACCEPTDEPT"] = twm41dj["C_ACCEPTDEPT"];
			twma4["C_ACCEPTSTOCK"] = twm41dj["C_ACCEPTSTOCK"];
			twma4["PRACTICE_NO"] = v_practice_no;
			twma4["TRUCK_NO"] = bcls_rec->Tables[0].Rows[iRow]["TRUCK_NO"].ToString();
			twma4["TRUCK_BOARD_NO"] = bcls_rec->Tables[0].Rows[iRow]["TRUCK_NO"].ToString();
			twma4["LOAD_CODE_FACTORY"] = twmsm61["LOAD_CODE_FACTORY"];
			twma4["LOAD_CODE_AREA"] = twmsm61["LOAD_CODE_AREA"];
			twma4["LOAD_CODE"] = twmsm61["LOAD_CODE"];
			twma4["UNLOAD_CODE"] = twmsm61["UNLOAD_CODE"];
			twma4["UNLOAD_CODE_AREA"] = twmsm61["UNLOAD_CODE_AREA"];
			twma4["UNLOAD_CODE_FACTORY"] = twmsm61["UNLOAD_CODE_FACTORY"];
			Log::Info("", __FUNCTION__, "bcls_rec_stock_log.Tables[WM_STOCK_LOG]   =[{0}]", bcls_rec_stock_log.Tables["WM_STOCK_LOG"].Columns.get_Count());
			twma4.MergeTo(bcls_rec_stock_log.Tables["WM_STOCK_LOG"], false);
			Log::Info("", __FUNCTION__, "bcls_rec_stock_log.Tables[WM_STOCK_LOG]   =[{0}]", bcls_rec_stock_log.Tables["WM_STOCK_LOG"].Columns.get_Count());

			if (tmmsm01["HR_SEND_FLAG"].ToString() == "1" && twm41dj["C_ACCEPTDEPT"].ToString() != "6360")
			{
				tmmsm01.MergeTo(inblock.Tables[0], false);
			}

			//15、调物流事件
			tmmsm96.Reset();
			tmmsm96.CopyFrom(tmmsm01);
			tmmsm96["LOGISTICS_STATUS"] = "2";//2--装车确认
			tmmsm96["FACTORY_TO"] = twmsm61["UNLOAD_CODE_FACTORY"];
			tmmsm96["DST_STOCK_CODE"] = twmsm61["UNLOAD_CODE_AREA"];
			tmmsm96["UNLOAD_CODE"] = twmsm61["UNLOAD_CODE"];
			tmmsm96["OUT_STOCK_TIME"] = datetime;
			tmmsm96["PRACTICE_NO"] = v_practice_no;
			tmmsm96["EVENT_ID"] = "MM78A";
			tmmsm96["SYSTEM_ID"] = "MMSM";
			tmmsm96["EVENT_LINE_TYPE"] = "00";
			tmmsm96["FUNC_ID"] = s.svc_name;
			tmmsm96["C_STATESIGN"] = "1";//1--正向调拨出库，3--正向调拨完成
			tmmsm96["C_DELIVERYID"] = twm41dj["C_DELIVERYID"];
			tmmsm96["C_DELIVERY_FAC"] = twm41dj["C_ACCEPTDEPT"];
			tmmsm96["C_DELIVERY_STOCK"] = twm41dj["C_ACCEPTSTOCK"];
			tmmsm96["TRAN_TIME"] = datetime;
			/*tmmsm96["EVENT_ID"] = "MM76";
			tmmsm96["SYSTEM_ID"] = "MMSM";
			tmmsm96["EVENT_LINE_TYPE"] = "00";
			tmmsm96["FUNC_ID"] = s.svc_name;*/
			tmmsm96.MergeTo(mm0099.Tables["MM0099"], false);
			Log::Info("", __FUNCTION__, "f_mmsm10_trace_linke   =[{0}]", __LINE__);

			tmmsm01.MergeTo(t8e2m1.Tables["E2T8M1"], false);
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
		CMessageFormat::Format(s.msg, "Database processing error. sqlcode=[{0}]." /* _RES("GCRSS0000006")*//*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		Log::Trace("", __FUNCTION__, "数据库SQL出错信息	= [{0}]", str);
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch (const CApplicationException& ex)
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

	return(doFlag);
}
