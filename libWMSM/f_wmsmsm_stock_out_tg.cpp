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
//#include "twma0.h" 
//#include "twma1.h"
//#include "twma2.h"
//#include "twma4.h"
//#include "twm01.h"



BM2_FUNCTION_IMPORT
int f_wmsmsm_stock_update(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);

BM2_FUNCTION_IMPORT
int f_wmsmsm_mm0099(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);


BM2_FUNCTION_IMPORT
int f_wmsmsm_stock_log(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);

BM2_FUNCTION_IMPORT
int f_wmsm_21a009_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wmsm_load_proc(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);

int f_mmsm99(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wm00_queue(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);



BM2_FUNCTION_IMPORT
//int f_wm00_craneCmdMake_follow(CString matNo, CDbConnection * conn);

BM2_FUNCTION_EXPORT
int f_wmsmsm_stock_out_tg(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	/* ***** 程序变量 ***** */
	int doFlag = 0;
	int blkNum = 0;
	int rowcount = 0;
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
	CString v_shift_no(""), v_shift_group("");
	/* ***** 数据库SQL操作字符串 ***** */
	CString	sqlstr = " ";
	/*装车实绩号*/
	CString v_practice_no = " ";

	/* ***** 数据库操作类定义 ***** */
	CDbCommand comm(conn);
	CDbCommand comm1(conn);

	/* ***** 定义表实体对象 ***** */
	//CTWMA0 twma0(conn);
	//CTWMA1 twma1(conn);
	//CTWMA2 twma2(conn);
	//CTWMA2 twma2_old(conn);
	//CTWMA4 twma4(conn);
	//CTWM01 twm01(conn);
	
	CModel twma1 = CModel("TMMSM01");
	CModel twma2 = CModel("TWMA2");
	CModel twma2_old = CModel("TWMA2");
	CModel twma4 = CModel("TWMA4");
	CModel twm01 = CModel("TWM01");
	CModel twma0 = CModel("TWMA0");
	CModel twmsm61 = CModel("TWMSM61");
	CModel twmsm62 = CModel("TWMSM62");
	CModel tmmsm96 = CModel("TMMSM96");
	CModel twm41dj = CModel("TWM41DJ");

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


	//调用生产函数
	EIClass bcls_rec_wmpm99;
	bcls_rec_wmpm99.Tables[0].set_TableName("WMPM99");
	bcls_rec_wmpm99.Tables[0].Columns.Add(twma1);
	bcls_rec_wmpm99.Tables[0].Rows.Clear();

	//调用计划函数
	EIClass bcls_rec_wmps99;
	bcls_rec_wmps99.Tables[0].set_TableName("WMPS99");
	bcls_rec_wmps99.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
	bcls_rec_wmps99.Tables[0].Columns.Add(DT_STRING, "MAT_KIND");
	bcls_rec_wmps99.Tables[0].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");
	bcls_rec_wmps99.Tables[0].Columns.Add(DT_STRING, "STOCK_OPER_ORDER_DIV");
	bcls_rec_wmps99.Tables[0].Rows.Clear();


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




	//入库队列
	EIClass bcls_stock_que;
	bcls_stock_que.Tables[0].set_TableName("WM00QUE");
	bcls_stock_que.Tables["WM00QUE"].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");
	bcls_stock_que.Tables["WM00QUE"].Columns.Add(DT_STRING, "STOCK_OPER_ORDER_DIV");
	bcls_stock_que.Tables["WM00QUE"].Columns.Add(DT_STRING, "MAT_NO");
	bcls_stock_que.Tables["WM00QUE"].Columns.Add(DT_STRING, "MAT_NUM");
	bcls_stock_que.Tables["WM00QUE"].Columns.Add(DT_STRING, "PLAN_NO");
	bcls_stock_que.Tables["WM00QUE"].Columns.Add(DT_STRING, "PLAN_EXEC_SEQ_NO");
	bcls_stock_que.Tables["WM00QUE"].Columns.Add(DT_STRING, "STOCK_NO");
	bcls_stock_que.Tables["WM00QUE"].Columns.Add(DT_STRING, "TRANS_TOOL");
	bcls_stock_que.Tables["WM00QUE"].Columns.Add(DT_STRING, "PRE_UNIT_CODE");
	bcls_stock_que.Tables["WM00QUE"].Columns.Add(DT_STRING, "NEXT_UNIT_CODE");
	bcls_stock_que.Tables["WM00QUE"].Columns.Add(DT_STRING, "MAT_DESTION");
	bcls_stock_que.Tables["WM00QUE"].Columns.Add(DT_STRING, "OPER_FLAG");
	bcls_stock_que.Tables["WM00QUE"].Columns.Add(DT_STRING, "FROM_STOCK_NO");
	bcls_stock_que.Tables["WM00QUE"].Columns.Add(DT_STRING, "FROM_STOCK_PLACE_NO");

	bcls_stock_que.Tables["WM00QUE"].Columns.Add(DT_STRING, "SEQ_ID");                 //顺序号
	bcls_stock_que.Tables["WM00QUE"].Columns.Add(DT_STRING, "LAYERNO");                //层号
	bcls_stock_que.Tables["WM00QUE"].Columns.Add(DT_STRING, "STOCK_PLACE_POSITION");   //车内顺序号

	bcls_stock_que.Tables["WM00QUE"].Rows.Clear();

	//发送电文
	EIClass bcls_load;
	bcls_load.Tables[0].set_TableName("21A009");
	bcls_load.Tables[0].Columns.Add(twmsm61);
	bcls_load.Tables[0].Rows.Clear();

	//调用物料事件
	EIClass mm0099;
	mm0099.Tables[0].set_TableName("MM0099");
	mm0099.Tables[0].Columns.Add(tmmsm96);
	mm0099.Tables[0].Rows.Clear();

	blkNum = bcls_rec->Tables.IndexOf("WM00QUE");
	if (blkNum < 0)
	{
		bcls_rec->Tables.Add("WM00QUE");
		bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");
		bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "MAT_NO");
		bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "MAT_NUM");
		bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "UNIT_CODE");
		bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "OPER_FLAG");
		bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "STOCK_NO");
	}
	bcls_rec->Tables["WM00QUE"].Rows.Clear();


	/* ***** 应用程序开始处理 ***** */
	try
	{
		if (!bcls_rec->Tables.Contains("WM_STOCK"))
		{
			sprintf(s.msg, "函数f_wm00_stock_in中找不到接收块名[WM_STOCK]");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		v_practice_no = "XG6240" + v_datetime.SubstringNE(0, 8) + Db::QueryCString("SELECT LPAD(TO_CHAR(ZC_SJ.NEXTVAL), 4, '0') FROM DUAl");
		for (int iRow = 0; iRow < bcls_rec->Tables["WM_STOCK"].Rows.get_Count(); iRow++)
		{
			Log::Trace("", __FUNCTION__, "iRow=[{0}]",  iRow);
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

			
			if (bcls_rec->Tables["WM_STOCK"].Columns.Contains("LAYERNO"))
			{
				LAYERNO = bcls_rec->Tables["WM_STOCK"].Rows[0]["LAYERNO"].ToString().Trim();
			}
			if (bcls_rec->Tables["WM_STOCK"].Columns.Contains("STOCK_PLACE_POSITION"))
			{
				STOCK_PLACE_POSITION = bcls_rec->Tables["WM_STOCK"].Rows[0]["STOCK_PLACE_POSITION"].ToString().Trim();
			}

			Log::Trace("", __FUNCTION__, "传入参数v_mat_no\t[{0}]", v_mat_no);
			
			Log::Trace("", __FUNCTION__, "传入参数v_stock_no\t[{0}]", v_stock_no);
			Log::Trace("", __FUNCTION__, "传入参数v_stock_place_no\t[{0}]", v_stock_place_no);
			Log::Trace("", __FUNCTION__, "传入参数v_layerno\t[{0}]", v_layerno);
			Log::Trace("", __FUNCTION__, "传入参数v_stock_place_position\t[{0}]", v_stock_place_position);
			Log::Trace("", __FUNCTION__, "传入参数v_crane_no\t[{0}]", v_crane_no);
			Log::Trace("", __FUNCTION__, "传入参数v_vehicle_no\t[{0}]", v_vehicle_no);

			Log::Trace("", __FUNCTION__, "传入参数SEQ_ID\t[{0}]", SEQ_ID);
			Log::Trace("", __FUNCTION__, "传入参数LAYERNO\t[{0}]", LAYERNO);
			Log::Trace("", __FUNCTION__, "传入参数STOCK_PLACE_POSITION\t[{0}]", STOCK_PLACE_POSITION);

			
			if (v_mat_no.Trim() == "")
			{
				sprintf(s.msg, "材料号不能为空.");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			


			//2. 检查A1
			twma1["MAT_NO"] = v_mat_no;
			if (!twma1.Query("MAT_NO"))
			{
				sprintf(s.msg, "TWMA1没有查询到[%s]。", (const char*)v_mat_no);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (twma1["LOGISTICS_STATUS"].ToString() != "0"
				&& twma1["LOGISTICS_STATUS"].ToString() != "1"
				&& twma1["LOGISTICS_STATUS"].ToString() != "4")
			{
				sprintf(s.msg, "材料[%s]物流状态为[%s],不能装车.",(const char*)twma1["MAT_NO"]);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			
			twma1["IN_FLAG"] = "3";
			twma1.Update("IN_FLAG", "MAT_NO");

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
			Log::Trace("", __FUNCTION__, "LINKE=[{0}]", __LINE__, iRow);
			bcls_rec_stock_upd.Tables["WM_STOCK_UPDATE"].Rows[iRow]["CRANE_NO"] = v_crane_no;
			Log::Trace("", __FUNCTION__, "LINKE=[{0}]", __LINE__);




			//5. 删A0（按材料号、类型）
			twma0["MAT_NO"] = v_mat_no;
			
			if (twma0.QueryCount("MAT_NO")>0)
			{
				twma0.Delete("MAT_NO");
			}
			Log::Trace("", __FUNCTION__, "LINKE=[{0}]", __LINE__);
			//6. MM0099
		//调用物料跟踪
			bcls_rec_wmmm99.Tables["WMMM99"].Rows.Add();
			int kk = bcls_rec_wmmm99.Tables["WMMM99"].Rows.get_Count() - 1;
			bcls_rec_wmmm99.Tables["WMMM99"].Rows[kk]["MAT_NO"] = twma1["MAT_NO"];
			bcls_rec_wmmm99.Tables["WMMM99"].Rows[kk]["STOCK_OPER_ORDER"] = "2A";
			bcls_rec_wmmm99.Tables["WMMM99"].Rows[kk]["OLD_STOCK_NO"] = twma2_old["STOCK_NO"];
			bcls_rec_wmmm99.Tables["WMMM99"].Rows[kk]["OLD_STOCK_PLACE_NO"] = twma2_old["STOCK_PLACE_NO"];
			bcls_rec_wmmm99.Tables["WMMM99"].Rows[kk]["OLD_LAYER_NO"] = twma2_old["LAYERNO"];
			bcls_rec_wmmm99.Tables["WMMM99"].Rows[kk]["AIM_STORE"] = v_stock_no;
			bcls_rec_wmmm99.Tables["WMMM99"].Rows[kk]["VEHICLE_NO"] = v_vehicle_no;
			Log::Trace("", __FUNCTION__, "LINKE=[{0}]", __LINE__);
			
			//发装车实绩
			twmsm61.CopyFrom(twma1);
			twmsm61.Print();
			twmsm61.MergeFrom(bcls_rec->Tables["WM_STOCK"].Rows[iRow]);
			twmsm61["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			if (twmsm61["UNLOAD_CODE"].ToString() == "6240ZTXD1")
			{
				sprintf(s.msg, "自提卸点，不允许发物流。", (const char*)v_mat_no);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			twmsm61["REC_CREATOR"] = s.userid;
			twmsm61["PRACTICE_NO"] = v_practice_no;
			twmsm61["WIDTH"] = twma1["MAT_WIDTH"];
			twmsm61["SG_SIGN"] = twma1["SG_GRADE_1"];
			twmsm61["LENGTH"] = twma1["MAT_LEN"];
			twmsm61["THICK"] = twma1["MAT_THICK"];
			twmsm61["WEIGHT"] = twma1["MAT_ACT_WT"];
			twmsm61["DEAL_FLAG"] = "I";
			//twmsm61["PRODUCT_TYPE"] = "1";
			twmsm61["UNLOAD_STATE"] = "2";
			twmsm61["LOAD_END_TIME"] = bcls_rec->Tables["WM_STOCK"].Rows[iRow]["OUT_STOCK_TIME"];
			if (bcls_rec->Tables["WM_STOCK"].Rows[iRow]["OUT_STOCK_TIME"].ToString().Trim() == "")
			{
				bcls_rec->Tables["WM_STOCK"].Rows[iRow]["OUT_STOCK_TIME"] = v_datetime;
			}
			f_epep_get_shift_group("SMCP", bcls_rec->Tables["WM_STOCK"].Rows[iRow]["OUT_STOCK_TIME"].ToString(), v_shift_no, v_shift_group, conn);
			twmsm61["SHIFT_NO"] = v_shift_no;
			twmsm61["SHIFT_GROUP"] = v_shift_group;
			twmsm61["TRANS_TYPE"] = "2";
			
			twm41dj["C_BATCHUNIT"] = twma1["MAT_NO"];
			twm41dj["C_STATESIGN"] = "1";
			if (twm41dj.QueryCount("C_BATCHUNIT,C_STATESIGN") > 0)
			{
				twmsm61["DEALY_FLAG"] = "4";
			}
			else
			{
				twmsm61["DEALY_FLAG"] = "3";
			}
			
			if (bcls_rec->Tables["WM_STOCK"].Rows[iRow]["UNLOAD_CODE_FACTORY"].ToString() == "6390")
			{
				twmsm61["MATERIAL_CODE"] = "HAB000000000000000";
			}
			else
			{
				twmsm61["MATERIAL_CODE"] = "HAA000000000000000";
			}
			CString qx_type = Db::QueryCString("select CODE_DESC_3_CONTENT from TWMSMZD02 where CODE_CLASS='WM02' AND CODE='" + twma1["GUIDE_DEST"].ToString() + "'");
			if (twma1["UNLOAD_CODE"].ToString() == "TBZX01001" && qx_type.Find("3") < 0)
			{
				sprintf(s.msg, "发往太北站的材料必须是指导去向类别为3的去向！");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (twmsm61["UNLOAD_CODE_FACTORY"].ToString()=="6240")
			{
				bcls_rec->Tables["WM00QUE"].Rows.Add();
				bcls_rec->Tables["WM00QUE"].Rows[rowcount]["STOCK_OPER_ORDER"] = "1L";
				bcls_rec->Tables["WM00QUE"].Rows[rowcount]["MAT_NO"] = twma1["MAT_NO"];
				bcls_rec->Tables["WM00QUE"].Rows[rowcount]["MAT_NUM"] = "1";
				bcls_rec->Tables["WM00QUE"].Rows[rowcount]["UNIT_CODE"] = twma1["UNIT_CODE"];
				bcls_rec->Tables["WM00QUE"].Rows[rowcount]["OPER_FLAG"] = "I";
				bcls_rec->Tables["WM00QUE"].Rows[rowcount]["STOCK_NO"] = "SYA";
				rowcount++;

				twmsm62.Reset();
				twmsm62.CopyFrom(twmsm61);
				twmsm62["UNLOAD_FLAG"] = "0";
				twmsm62["AFFIRM_FLAG"] = "0";
				twmsm62.Insert();
			}
			twmsm61.Insert();
			if (true) {
				twmsm61.MergeTo(bcls_load.Tables["21A009"], false);
			}
			
			Log::Trace("", __FUNCTION__, "LINKE=[{0}]", __LINE__);

			//13. 写A4
			twma4.CopyFrom(twma1);
			twma4["STOCK_OPER_ORDER"] = "2A";
			twma4["STOCK_NO"] = twma2_old["STOCK_NO"];// v_stock_no;
			twma4["STOCK_PLACE_NO"] = v_stock_place_no;
			twma4["LAYERNO"] = v_layerno;
			twma4["FROM_STOCK_NO"] = twma2_old["STOCK_NO"];
			twma4["FROM_STOCK_PLACE_NO"] = twma2_old["STOCK_PLACE_NO"];
			twma4["CRANE_NO"] = v_crane_no;
			twma4["VEHICLE_NO"] = v_vehicle_no;

			twma4["EVENT_DESC"] = "只装车";
			twma4["C_DELIVERYID"] = " ";
			twma4["C_ACCEPTDEPT"] = " ";
			twma4["C_ACCEPTSTOCK"] = " ";
			twma4["PRACTICE_NO"] = v_practice_no;
			twma4["TRUCK_NO"] = v_vehicle_no;
			twma4["TRUCK_BOARD_NO"] = v_vehicle_no;
			twma4["LOAD_CODE_FACTORY"] = twmsm61["LOAD_CODE_FACTORY"];
			twma4["LOAD_CODE_AREA"] = twmsm61["LOAD_CODE_AREA"];
			twma4["LOAD_CODE"] = twmsm61["LOAD_CODE"];
			twma4["UNLOAD_CODE"] = twmsm61["UNLOAD_CODE"];
			twma4["UNLOAD_CODE_AREA"] = twmsm61["UNLOAD_CODE_AREA"];
			twma4["UNLOAD_CODE_FACTORY"] = twmsm61["UNLOAD_CODE_FACTORY"];

			twma4.MergeTo(bcls_rec_stock_log.Tables["WM_STOCK_LOG"], false);
			Log::Trace("", __FUNCTION__, "LINKE=[{0}]", __LINE__);

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
			Log::Trace("", __FUNCTION__, "LINKE=[{0}]", __LINE__);
			
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

		//调用履历函数
		if (bcls_rec_stock_log.Tables[0].Rows.get_Count() > 0)
		{
			Log::Trace("", __FUNCTION__, "2222222222222222[{0}]", bcls_rec_stock_log.Tables[0].Rows.get_Count());
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
		if (bcls_rec->Tables["WM00QUE"].Rows.get_Count() > 0) {

			doFlag = f_wm00_queue(bcls_rec, bcls_ret, conn);
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
