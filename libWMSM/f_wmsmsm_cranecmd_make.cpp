/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:      jinquan
Version:     1.1.1
Date:        2017-03-13 10:35:08
Description: 板坯吊车命令做成函数
**************************************************/

//框架头文件
#include "stdafx.h"

BM2_FUNCTION_IMPORT
int f_wm00_pileinfocal(CString stock_no, CString stock_place_no, EIClass * bcls_ret, CDbConnection * conn);  //垛位最大高度、重量修正
int f_wmsmsm_cranecmd_update(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn); //替换命令
int f_wmsmsm_cranecmd_seq_upt(CString stock_oper_order, EIClass * bcls_ret, CDbConnection * conn);//更新顺序号
int f_auto(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

BM2_FUNCTION_EXPORT
int f_wmsmsm_cranecmd_make(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	//程序内部变量 
	int doFlag = 0;
	int SEQ = 0;
	int SEQ_A = 0;
	int SEQ_B = 0;
	int maxseqno = 9999999;

	//数据库SQL操作字符串
	CString sqlstr = "";
	CString sqlstr_update = "";
	
	//数据块
	CDataTable Table_mat;                  //存放传入材料信息
	CDataTable Table_position;             //存放传入材料位置上的材料信息
	CDataTable Table_position_from;        //存放From垛位
	CDataTable Table_auto;                 //存放传入材料信息
	CDataTable Table_mat_send;             //存放传入材料信息

	//数据块
	CDataTable logic_rule;                         //twma8

	//定义表实体对象
	CModel twma7_in = CModel("TWMA7");
	CModel twma7_q = CModel("TWMA7");
	CModel twm04 = CModel("TWM04");
	CModel twm04_q = CModel("TWM04");
	CModel tmmsm01 = CModel("TMMSM01");
	//业务变量
	CString dateTime = " ";
	CString stock_oper_order = "";
	CString mat_no = "";
	CString hall_no = "";
	CString logic_stock_no = "";
	CString SEQ_NO = "";
	CString prod_seq_no = "";
	CString stock_place_no_from = "";
	CString v_table_name = "";
	CDecimal v_count = 0;
	CString stock_to = "";
	CString hall_to = "";
	CString lg_hc = ""; 
	CString plc = ""; 
	CString order_type = "";
	CString mov_hall = ""; 
	CString crane_no = "";

	CDbCommand cmd_inq(conn);
	try
	{
		//检验传入数据块
		if (!bcls_rec->Tables.Contains("CMD_MAKE"))
		{
			sprintf(s.msg, "函数f_wmsmsm_cranecmd_make中找不到接收块名[CMD_MAKE]");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//取系统时间
		dateTime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		//新增列
		Table_position_from.Columns.Add(DT_STRING, "POSITION_FROM");

		EIClass bcls_rec_update;
		bcls_rec_update.Tables[0].set_TableName("CMD_UPDATE");
		//bcls_rec_update.Tables[0].set_TableName("CMD_UPDATE");CRANE_NO
		bcls_rec_update.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
		bcls_rec_update.Tables[0].Columns.Add(DT_STRING, "CRANE_INST_STATUS");
		bcls_rec_update.Tables[0].Columns.Add(DT_STRING, "STOCK_NO");
		bcls_rec_update.Tables[0].Columns.Add(DT_STRING, "STOCK_NO_FROM");
		bcls_rec_update.Tables[0].Columns.Add(DT_STRING, "HALL_NO_FR");
		bcls_rec_update.Tables[0].Columns.Add(DT_DECIMAL, "YARD_LAYER_FROM");
		bcls_rec_update.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO_FROM");
		bcls_rec_update.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO_FIN");
		bcls_rec_update.Tables[0].Columns.Add(DT_STRING, "STOCK_OPER_ORDER_NEW");
		bcls_rec_update.Tables[0].Columns.Add(DT_STRING, "STOCK_OPER_ORDER_OLD");
		bcls_rec_update.Tables[0].Columns.Add(DT_STRING, "HALL_NO_TO");
		bcls_rec_update.Tables[0].Columns.Add(DT_STRING, "STOCK_NO_TO");
		bcls_rec_update.Tables[0].Columns.Add(DT_STRING, "LOGIC_STOCK_NO");
		bcls_rec_update.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO_TO_NEW");
		bcls_rec_update.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO_TO_OLD");
		bcls_rec_update.Tables[0].Columns.Add(DT_STRING, "STOCK_OPER_ORDER_FIN"); 
		bcls_rec_update.Tables[0].Columns.Add(DT_DECIMAL, "BATCH_TASK_NO");
		bcls_rec_update.Tables[0].Columns.Add(DT_DECIMAL, "BATCH_NO");
		bcls_rec_update.Tables[0].Columns.Add(DT_STRING, "DEV_DIV");
		bcls_rec_update.Tables[0].Columns.Add(DT_DECIMAL, "CRANE_CMDGRPNO_OLD");
		bcls_rec_update.Tables[0].Columns.Add(DT_STRING,  "SEND_FLAG");
		bcls_rec_update.Tables[0].Columns.Add(DT_STRING, "CRANE_NO");
		bcls_rec_update.Tables[0].Rows.Add();

		EIClass bcls_rec_group;
		bcls_rec_group.Tables[0].set_TableName("CMD_GROUP");
		bcls_rec_group.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO");

		//推荐库位结果
		EIClass bcls_ret_place;

		//推荐逻辑区域结果
		EIClass bcls_ret_logic;

		EIClass bcls_rec_tr;
		bcls_rec_tr.Tables[0].set_TableName("CMD_TR_REM");
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
		bcls_rec_tr.Tables[0].Rows.Add();


		//推荐库位
		EIClass bcls_rec_auto;
		bcls_rec_auto.Tables[0].set_TableName("BLK_INPUT");
		bcls_rec_auto.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
		bcls_rec_auto.Tables[0].Columns.Add(DT_STRING, "HALL_NO");
		bcls_rec_auto.Tables[0].Columns.Add(DT_STRING, "LOGIC_STOCK_NO");
		bcls_rec_auto.Tables[0].Columns.Add(DT_DECIMAL, "BASE_X");
		bcls_rec_auto.Tables[0].Columns.Add(DT_DECIMAL, "BASE_Y");
		bcls_rec_auto.Tables[0].Columns.Add(DT_STRING, "STNO_RULE_FLAG");
		bcls_rec_auto.Tables[0].Columns.Add(DT_STRING, "ORDER_RULE_FLAG");
		bcls_rec_auto.Tables[0].Columns.Add(DT_DECIMAL, "WIDTH_DIFF");
		bcls_rec_auto.Tables[0].Rows.Add();

		

		EIClass bcls_rec_auto_1;
		bcls_rec_auto_1.Tables[0].set_TableName("AUTO_INFO_IN");
		bcls_rec_auto_1.Tables[0].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");
		bcls_rec_auto_1.Tables[0].Columns.Add(DT_STRING, "HALL_NO");
		bcls_rec_auto_1.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
		bcls_rec_auto_1.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO_FROM");
		bcls_rec_auto_1.Tables[0].Rows.Add();

		//获取库位推荐参数
		CDataTable   Table_Flag;
		Log::Trace("", __FUNCTION__, "记录数	= [{0}]", bcls_rec->Tables["CMD_MAKE"].Rows.get_Count());
		
		for (int i = 0; i < bcls_rec->Tables["CMD_MAKE"].Rows.get_Count(); i++)
		{
			crane_no = bcls_rec->Tables["CMD_MAKE"].Rows[i]["CRANE_NO"].ToString();
			stock_oper_order = bcls_rec->Tables["CMD_MAKE"].Rows[i]["STOCK_OPER_ORDER"].ToString();
			mat_no = bcls_rec->Tables["CMD_MAKE"].Rows[i]["MAT_NO"].ToString();	
			if (bcls_rec->Tables[0].Columns.Contains("PLC") == true)
			{
				plc = bcls_rec->Tables[0].Rows[0]["PLC"].ToString();
			}
			if (bcls_rec->Tables[0].Columns.Contains("ORDER_TYPE") == true)
			{
				order_type = bcls_rec->Tables[0].Rows[0]["ORDER_TYPE"].ToString();
			}
			if (bcls_rec->Tables[0].Columns.Contains("MOV_HALL") == true) //过跨倒垛生成第二段吊运命令
			{
				mov_hall = bcls_rec->Tables[0].Rows[0]["MOV_HALL"].ToString();
			}

			Log::Info("", __FUNCTION__, "crane_no		= [{0}]", crane_no);
			Log::Trace("", __FUNCTION__, "传入材料号：  【{0}】", mat_no);
			Log::Trace("", __FUNCTION__, "传入业务类型：【{0}】", stock_oper_order);
			Log::Trace("", __FUNCTION__, "plc：  【{0}】", plc);
			Log::Trace("", __FUNCTION__, "命令种类 order_type：  【{0}】", order_type);
			Log::Trace("", __FUNCTION__, "过跨倒垛生成第二段吊运命令标记 mov_hall：  【{0}】", mov_hall);
			
			//检验传入数据
			if (mat_no.Trim() == "")
			{
				sprintf(s.msg, "传入材料号为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (stock_oper_order.Trim() == "")
			{
				sprintf(s.msg, "传入业务类型为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (!bcls_rec->Tables["CMD_MAKE"].Columns.Contains("STOCK_PLACE_NO_TO"))
			{
				sprintf(s.msg, "未传目标库位字段");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			twm04["STOCK_PLACE_NO"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["STOCK_PLACE_NO_TO"].ToString().Trim();
			twm04.Query("STOCK_PLACE_NO");
			stock_to = twm04["STOCK_NO"].ToString().Trim();
			if (!bcls_rec->Tables["CMD_MAKE"].Columns.Contains("HALL_NO_TO"))
			{
				hall_to = twm04["HALL_NO"].ToString().Trim();
			}
			else
			{
				hall_to = bcls_rec->Tables["CMD_MAKE"].Rows[i]["HALL_NO_TO"].ToString().Trim();
			}
			if (!bcls_rec->Tables["CMD_MAKE"].Columns.Contains("HALL_NO_TO"))
			{
				stock_to = twm04["HALL_NO"].ToString().Trim();			
			}
			else
			{
				stock_to = bcls_rec->Tables["CMD_MAKE"].Rows[i]["STOCK_NO_TO"].ToString().Trim();
			}
			Log::Trace("", __FUNCTION__, " hall_to：  【{0}】", hall_to);
			Log::Trace("", __FUNCTION__, " stock_to：  【{0}】", stock_to);
			Log::Trace("", __FUNCTION__, " twm04.STOCK_PLACE_TYPE：  【{0}】", twm04["STOCK_PLACE_TYPE"].ToString().Trim());
			Log::Trace("", __FUNCTION__, " twm04.STOCK_NO：  【{0}】", twm04["STOCK_NO"].ToString().Trim());
			
			//2022-03-23  黎展富要求 设备库库位只能放一块板坯
			if (twm04["STOCK_NO"].ToString().Trim() == "A21" && twm04["STOCK_PLACE_TYPE"].ToString().Trim() == "5")
			{
				tmmsm01.Reset();
				tmmsm01["STOCK_NO"] = "A21";
				tmmsm01["STOCK_PLACE_NO"] = twm04["STOCK_PLACE_NO"].ToString().Trim();
				tmmsm01["IN_FLAG"] = "1";
				int count = tmmsm01.QueryCount("STOCK_NO,STOCK_PLACE_NO,IN_FLAG");
				Log::Trace("", __FUNCTION__, " 库位【{0}】上堆放板坯块数：【{1}】", twm04["STOCK_PLACE_NO"].ToString().Trim(), count);
				if (count > 0)
				{
					sprintf(s.msg, "目标库位[" + twm04["STOCK_PLACE_NO"].ToString().Trim() + "]上已有板坯,此库位只能放一块板坯，请选择其他库位操作。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}

			Log::Trace("", __FUNCTION__, "1");
			sqlstr = " SELECT COUNT(1) FROM TMMSM01"
				" WHERE MAT_NO = @mat_no";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("mat_no", mat_no);
			v_count = cmd_inq.ExecuteScalar();
			if (v_count == 1)
			{
				v_table_name = "TMMSM01";
			}
			else
			{
				sqlstr = " SELECT COUNT(1) FROM TMMHP01"
					" WHERE MAT_NO = @mat_no";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("mat_no", mat_no);
				v_count = cmd_inq.ExecuteScalar();
				if (v_count == 1)
				{
					v_table_name = "TMMHP01";
				}
				else
				{
					sprintf(s.msg, "板坯/热卷主档找不到材料号.");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}
			twm04_q["STOCK_PLACE_NO"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["STOCK_PLACE_NO_FROM"].ToString().Trim();
			twm04_q.Query("STOCK_PLACE_NO");
			//获取材料信息
			Table_mat.Clear();
			if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("STOCK_PLACE_NO_FROM")
				&& bcls_rec->Tables["CMD_MAKE"].Rows[i]["STOCK_PLACE_NO_FROM"].ToString().Trim() != "" && twm04_q.QueryCount("STOCK_PLACE_NO") > 0)
			{
				Log::Trace("", __FUNCTION__, "2");
				sqlstr = " SELECT a.MAT_NO,b.DEV_DIV,b.X_FROM,b.Y_FROM,a.MAT_KIND,a.MAT_THICK,a.MAT_WIDTH,a.MAT_LEN,a.MAT_THEORY_WT,b.STOCK_NO,b.STOCK_PLACE_NO,a.LAYERNO,b.HALL_NO,b.remark " 
					" FROM  " + v_table_name + " a, TWM04 b WHERE a.MAT_NO = '" + mat_no + "' and b.STOCK_PLACE_NO = '" + bcls_rec->Tables["CMD_MAKE"].Rows[i]["STOCK_PLACE_NO_FROM"].ToString() + "'";
			}
			else if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("STOCK_PLACE_NO_FROM")
				&& (bcls_rec->Tables["CMD_MAKE"].Rows[i]["STOCK_PLACE_NO_FROM"].ToString().Trim() == "" || twm04_q.QueryCount("STOCK_PLACE_NO") == 0))
			{
				Log::Trace("", __FUNCTION__, "22");
				sqlstr = " SELECT a.MAT_NO,b.DEV_DIV,b.X_FROM,b.Y_FROM,a.MAT_KIND,a.MAT_THICK,a.MAT_WIDTH,a.MAT_LEN,a.MAT_THEORY_WT,b.STOCK_NO,b.STOCK_PLACE_NO,a.LAYERNO,b.HALL_NO,b.remark "
					" FROM  " + v_table_name + " a, TWM04 b WHERE a.stock_place_no = b.stock_place_no and a.MAT_NO = '" + mat_no + "' ";
			}
			else
			{
				Log::Trace("", __FUNCTION__, "3");
				sqlstr = " SELECT a.MAT_NO,c.DEV_DIV,c.X_FROM,c.Y_FROM,a.MAT_KIND,a.MAT_THICK,a.MAT_WIDTH,a.MAT_LEN,a.MAT_THEORY_WT,b.STOCK_NO,b.STOCK_PLACE_NO,a.LAYERNO,b.HALL_NO,b.remark "
					" FROM  "+v_table_name+" a, TWMA2 b,TWM04 c where c.STOCK_PLACE_NO=b.STOCK_PLACE_NO AND a.MAT_NO = b.MAT_NO and a.MAT_NO = '" + mat_no + "'";
			}	
			Log::Trace("", __FUNCTION__, "4");
			Log::Trace("", __FUNCTION__, "sqlstr:{0}", sqlstr);
			Db::QueryTable(sqlstr, Table_mat);
			Log::Trace("", __FUNCTION__, "4。1");
			if (Table_mat.Rows.get_Count() == 0)
			{
				sprintf(s.msg, "材料[%s]信息不存在", (const char*)mat_no);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			Log::Trace("", __FUNCTION__, "4。1。5");
			Log::Trace("", __FUNCTION__, "X_FROM：  【{0}】", Table_mat.Rows[0]["X_FROM"].ToDecimal());
			Log::Trace("", __FUNCTION__, "Y_FROM：  【{0}】", Table_mat.Rows[0]["Y_FROM"].ToDecimal());
			Log::Trace("", __FUNCTION__, "4。2");
			twma7_in["MAT_NO"] = mat_no;
			if (twma7_in.Query("MAT_NO")  )
			{
				if (twma7_in.QueryCount("MAT_NO")>0)
				{
					strcpy(s.msg, "该材料" + twma7_in["MAT_NO"].ToString() + "已生成吊运命令，不能做该操作！");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				Log::Trace("", __FUNCTION__, "5");
			/*	if (twma7_in["STOCK_OPER_ORDER"].ToString().SubstringNE(0, 1) == "2" 
					&& twma7_in["STOCK_PLACE_NO_FROM"].ToString().Trim() != "H-1A")
				{
					sprintf(s.msg, "材料已存在命令，且不能替换");
					throw CApplicationException(-1, s.msg, log.Location);
				}*/
				Log::Trace("", __FUNCTION__, "6");
				
			}
	
			if (stock_oper_order.SubstringNE(0, 1) == "1")
			{		
				Log::Trace("", __FUNCTION__, "入库命令");

				//2022-03-23  黎展富要求 设备库库位只能放一块板坯
				twma7_q.Reset();
				twma7_q["MAT_NO"] = mat_no;
				twma7_q.Query("MAT_NO");
				if (twm04["STOCK_NO"].ToString().Trim() == "A21" && twm04["STOCK_PLACE_TYPE"].ToString().Trim() == "5" && twma7_q["STOCK_PLACE_NO_TO"].ToString().Trim() == twm04["STOCK_PLACE_NO"].ToString().Trim())
				{
					sprintf(s.msg, "目标库位[" + twm04["STOCK_PLACE_NO"].ToString().Trim() + "]上已有行车命令,此库位只能放一块板坯，请选择其他库位操作。");
					throw CApplicationException(-1, s.msg, log.Location);
				}

				twma7_in.Reset();
				twma7_in["REC_CREATE_TIME"] = dateTime;
				twma7_in["REC_CREATOR"] = s.userid;
				twma7_in["MAT_NO"] = mat_no;
				twma7_in["STOCK_OPER_ORDER"] = stock_oper_order;
				twma7_in["CRANE_INST_CODE"] = stock_oper_order.SubstringNE(0, 1);
				twma7_in["CRANE_INST_STATUS"] = "0";
					
				//写材料信息
				twma7_in["MAT_KIND"] = Table_mat.Rows[0]["MAT_KIND"].ToString();
				twma7_in["MAT_SHAPE_FLAG"] = "1";
				twma7_in["MAT_ACT_THICK"] = Table_mat.Rows[0]["MAT_THICK"].ToDecimal();
				twma7_in["MAT_ACT_WIDTH"] = Table_mat.Rows[0]["MAT_WIDTH"].ToDecimal();
				twma7_in["MAT_ACT_LEN"] = Table_mat.Rows[0]["MAT_LEN"].ToDecimal();
				twma7_in["MAT_ACT_WT"] = Table_mat.Rows[0]["MAT_THEORY_WT"].ToDecimal();
					
				//写当前垛位信息
				twma7_in["STOCK_NO"] = Table_mat.Rows[0]["STOCK_NO"];
				twma7_in["STOCK_NO_FROM"] = Table_mat.Rows[0]["STOCK_NO"];
				twma7_in["HALL_NO_FR"] = Table_mat.Rows[0]["HALL_NO"];
				twma7_in["STOCK_PLACE_NO_FROM"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["STOCK_PLACE_NO_FROM"].ToString().Trim(); //Table_mat.Rows[0]["STOCK_PLACE_NO"];
				if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("YARD_LAYER_FROM")
					&& bcls_rec->Tables["CMD_MAKE"].Rows[i]["YARD_LAYER_FROM"].ToString()!="")
				{

					twma7_in["YARD_LAYER_FROM"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["YARD_LAYER_FROM"].ToString();
				}
				else
				{
					twma7_in["YARD_LAYER_FROM"] = Table_mat.Rows[0]["LAYERNO"];
				}
				Log::Trace("", __FUNCTION__, "4。4。1");
				//写目标区域
				twma7_in["CRANE_NO"] = crane_no;
				twma7_in["STOCK_OPER_ORDER"] = stock_oper_order;
				twma7_in["STOCK_NO_TO"] = stock_to;
				twma7_in["HALL_NO_TO"] = hall_to;

				if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("STOCK_NO_FIN"))
				{
					twma7_in["STOCK_NO_FIN"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["STOCK_NO_FIN"].ToString().Trim();
				}
				else
				{
					twma7_in["STOCK_NO_FIN"] = twma7_in["STOCK_NO_TO"];
				}

				if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("HALL_NO_FIN"))
				{
					twma7_in["HALL_NO_FIN"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["HALL_NO_FIN"].ToString().Trim();
				}
				else
				{
					twma7_in["HALL_NO_FIN"] = twma7_in["HALL_NO_TO"];
				}
				if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("LOGIC_STOCK_NO"))
				{
					twma7_in["LOGIC_STOCK_NO"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["LOGIC_STOCK_NO"].ToString().Trim();
				}

				if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("STOCK_OPER_ORDER_FIN"))
				{
					twma7_in["STOCK_OPER_ORDER_FIN"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["STOCK_OPER_ORDER_FIN"].ToString().Trim();
				}
				else
				{
					twma7_in["STOCK_OPER_ORDER_FIN"] = stock_oper_order;
				}
					

				if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("CMD_METHOD"))
				{
					twma7_in["CMD_METHOD"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["CMD_METHOD"].ToString().Trim();
				}
				if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("MAT_STATUS"))
				{
					twma7_in["MAT_STATUS"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["MAT_STATUS"].ToString().Trim();
				}
				if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("MAT_DESTION"))
				{
					twma7_in["MAT_DESTION"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["MAT_DESTION"].ToString().Trim();
				}
				if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("MAT_THEORY_WT"))
				{
					twma7_in["MAT_THEORY_WT"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["MAT_THEORY_WT"].ToString().Trim();
				}
				if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("HEAD_TAIL_WITH_DIFF"))
				{
					twma7_in["HEAD_TAIL_WITH_DIFF"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["HEAD_TAIL_WITH_DIFF"].ToString().Trim();
				}
				if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("UNIT_CODE") )
				{
					twma7_in["UNIT_CODE"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["UNIT_CODE"].ToString().Trim();
				}

				if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("STOCK_PLACE_NO_FIN") )
				{
					twma7_in["STOCK_PLACE_NO_FIN"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["STOCK_PLACE_NO_FIN"].ToString().Trim();
				}
					
				if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("STOCK_PLACE_NO_TO"))
				{
					twma7_in["STOCK_PLACE_NO_TO"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["STOCK_PLACE_NO_TO"].ToString().Trim();
				}
				else
				{
					twma7_in["STOCK_PLACE_NO_TO"] = " ";
				}
				if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("CRANE_CMDGRPNO"))
				{				
					   
					twma7_in["CRANE_CMDGRPNO"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["CRANE_CMDGRPNO"].ToDecimal();
				}
				else
				{
					twma7_in["CRANE_CMDGRPNO"] = 0;
				}
				Log::Trace("", __FUNCTION__, "4。4。2");
				prod_seq_no = EPGetNextSeq("WM_CMD_SEQ", conn);
				if (doFlag < 0 || prod_seq_no.Trim() == "")
				{
					sprintf(s.msg, "获取流水号失败，请查看EPED21【WM_CMD_SEQ】是否正常!");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				twma7_in["CMD_SEQ"] = dateTime.SubstringNE(2, 6) + prod_seq_no; // 10位 
					
				twma7_in["MOVE_TYPE"] = twma7_in["STOCK_OPER_ORDER"];
				twma7_in["STOCK_PLACE_NO_TO"].ToString().Trim() == "";
				//twma7_in["TR_NO"] = "1";//台车号   
				//CRANE_NO
				twma7_in["CRANE_NO"] = crane_no;//台车号 
				if (twma7_in["STOCK_PLACE_NO_TO"].ToString().Trim() == "")
				{
				
						bcls_rec_auto_1.Tables[0].Rows[0]["MAT_NO"] = Table_mat.Rows[0]["MAT_NO"].ToString();
						bcls_rec_auto_1.Tables[0].Rows[0]["STOCK_OPER_ORDER"] = stock_oper_order;
						bcls_rec_auto_1.Tables[0].Rows[0]["HALL_NO"] =  hall_to;					
						bcls_rec_auto_1.Tables[0].Rows[0]["STOCK_PLACE_NO_FROM"] = twma7_in["STOCK_PLACE_NO_FROM"].ToString();
						doFlag = f_auto(&bcls_rec_auto_1, &bcls_ret_logic, conn);
						if (doFlag != 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}
						twma7_in["LOGIC_STOCK_NO"] = bcls_ret_logic.Tables[0].Rows[0]["LOGIC_STOCK_NO"].ToString();
						twma7_in["STOCK_PLACE_NO_TO"] = bcls_ret_logic.Tables[0].Rows[0]["STOCK_PLACE_NO"].ToString();
				
				}
				
			
				if (twma7_in["STOCK_PLACE_NO_TO"].ToString().Trim() != "")
				{
					//更新垛位状态
					doFlag = f_wm00_pileinfocal(twma7_in["STOCK_NO"].ToString(), twma7_in["STOCK_PLACE_NO_TO"].ToString(), bcls_ret, conn);
					if (doFlag != 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}

				if (twma7_in["STOCK_PLACE_NO_FIN"].ToString().Trim() != "")
				{
					//更新垛位状态
					doFlag = f_wm00_pileinfocal(twma7_in["STOCK_NO_FIN"].ToString(), twma7_in["STOCK_PLACE_NO_FIN"].ToString(), bcls_ret, conn);
					if (doFlag != 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}

				lg_hc = EPGetNextSeq("LG_HC", conn);
				lg_hc = dateTime.Substring(0, 8) + lg_hc;
				Log::Trace("", __FUNCTION__, "lg_hc：  【{0}】", lg_hc);

				Log::Trace("", __FUNCTION__, "4。4。4");
				twma7_in["MAIN_MAT_NO"] = (CDecimal::Parse(lg_hc)).ToString();
				twma7_in.Insert();
				Log::Trace("", __FUNCTION__, "4。4。5");

			}		
			else if (stock_oper_order.SubstringNE(0, 1) == "3")
			{
	
				Log::Trace("", __FUNCTION__, "倒跺命令");

				//2022-03-23  黎展富要求 设备库库位只能放一块板坯
				twma7_q.Reset();
				twma7_q["MAT_NO"] = mat_no;
				twma7_q.Query("MAT_NO");
				if (twm04["STOCK_NO"].ToString().Trim() == "A21" && twm04["STOCK_PLACE_TYPE"].ToString().Trim() == "5" && twma7_q["STOCK_PLACE_NO_TO"].ToString().Trim() == twm04["STOCK_PLACE_NO"].ToString().Trim())
				{
					sprintf(s.msg, "目标库位[" + twm04["STOCK_PLACE_NO"].ToString().Trim() + "]上已有行车命令,此库位只能放一块板坯，请选择其他库位操作。");
					throw CApplicationException(-1, s.msg, log.Location);
				}

				Table_position_from.Rows.Add();
				Table_position_from.Rows[SEQ]["POSITION_FROM"] = Table_mat.Rows[0]["STOCK_PLACE_NO"];
				SEQ++;

				if (Table_mat.Rows[0]["DEV_DIV"].ToString().Trim() == "4") //辊道位置，不考虑上层倒垛
				{
					sqlstr = "select mat_no,stock_place_no,layerno from twma2 a where mat_no='" + Table_mat.Rows[0]["MAT_NO"].ToString() + "'";
				}
				else if (mov_hall.Trim() == "1")
				{
					sqlstr = "select mat_no,stock_place_no,layerno from twma2 a where mat_no='" + Table_mat.Rows[0]["MAT_NO"].ToString() + "'";
				}
				else
				{
					sqlstr = "select mat_no,stock_place_no,layerno from twma2 a where stock_place_no='" + Table_mat.Rows[0]["STOCK_PLACE_NO"].ToString() + "' and INT(layerno)>=INT(" + Table_mat.Rows[0]["LAYERNO"].ToString() + ") and a.mat_no not in (select mat_no from twma7 b where a.mat_no = b.mat_no) order by INT(a.layerno) desc";
				}
				Log::Trace("", __FUNCTION__, "sqlstr:{0}", sqlstr);
				Db::QueryTable(sqlstr, Table_position);

				if (Table_position.Rows.get_Count() > 1)
				{
					sprintf(s.msg, "材料号[%s]上方有板坯号，不允许直接吊运倒跺", (const char*)Table_mat.Rows[0]["MAT_NO"]);
					throw CApplicationException(-1, s.msg, log.Location);
				}

				Log::Trace("", __FUNCTION__, "上层个数:{0}", Table_position.Rows.get_Count() - 1);
				for (int j = 0; j < Table_position.Rows.get_Count(); j++)
				{
					Log::Trace("", __FUNCTION__, "生成材料{0}命令", Table_position.Rows[j]["MAT_NO"].ToString());
					Table_mat.Clear();
					//获取材料信息
					sqlstr = "select a.MAT_NO,a.MAT_KIND,a.MAT_THICK,a.MAT_WIDTH,a.MAT_LEN,a.MAT_THEORY_WT,b.STOCK_NO,c.HALL_NO,b.STOCK_PLACE_NO,b.LAYERNO,c.X_FROM,c.Y_FROM,c.LOGIC_STOCK_NO "
						" from " + v_table_name + " a,twma2 b,twm04 c"
					         " where c.STOCK_PLACE_NO=b.STOCK_PLACE_NO AND a.MAT_NO=b.MAT_NO and a.MAT_NO='" + Table_position.Rows[j]["mat_no"].ToString() + "'";
					Db::QueryTable(sqlstr, Table_mat);
					if (Table_mat.Rows.get_Count() == 0)
					{
						sprintf(s.msg, "材料[%s]信息不存在", (const char*)Table_position.Rows[j]["MAT_NO"]);
						throw CApplicationException(-1, s.msg, log.Location);
					}

					twma7_in.Reset();
					twma7_in["REC_CREATE_TIME"] = dateTime;
					twma7_in["REC_CREATOR"] = s.userid;
					twma7_in["MAT_NO"] = Table_position.Rows[j]["MAT_NO"].ToString();
					twma7_in["STOCK_OPER_ORDER"] = stock_oper_order;
					twma7_in["CRANE_INST_CODE"] = stock_oper_order.SubstringNE(0, 1);
					twma7_in["CRANE_INST_STATUS"] = "0";

					//写材料信息
					twma7_in["MAT_KIND"] = Table_mat.Rows[0]["MAT_KIND"].ToString();
					twma7_in["MAT_SHAPE_FLAG"] = "1";
					twma7_in["MAT_ACT_THICK"] = Table_mat.Rows[0]["MAT_THICK"].ToDecimal();
					twma7_in["MAT_ACT_WIDTH"] = Table_mat.Rows[0]["MAT_WIDTH"].ToDecimal();
					twma7_in["MAT_ACT_LEN"] = Table_mat.Rows[0]["MAT_LEN"].ToDecimal();
					twma7_in["MAT_ACT_WT"] = Table_mat.Rows[0]["MAT_THEORY_WT"].ToDecimal();

					//写当前垛位信息
					twma7_in["STOCK_NO"] = Table_mat.Rows[0]["STOCK_NO"];
					twma7_in["STOCK_NO_FROM"] = Table_mat.Rows[0]["STOCK_NO"];
					twma7_in["HALL_NO_FR"] = Table_mat.Rows[0]["HALL_NO"].ToString();
					twma7_in["STOCK_PLACE_NO_FROM"] = Table_mat.Rows[0]["STOCK_PLACE_NO"]; 
					if (mov_hall.Trim() == "1") //过跨倒垛第二段命令，起吊位置 取传入参数的值
					{
						twma7_in["STOCK_PLACE_NO_FROM"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["STOCK_PLACE_NO_FROM"].ToString().Trim();
					}
					twma7_in["YARD_LAYER_FROM"] = Table_mat.Rows[0]["LAYERNO"];
					
					//写目标垛位信息
					if (j == Table_position.Rows.get_Count() - 1)
					{
						Log::Trace("", __FUNCTION__, "test 111 ");
						twma7_in["STOCK_OPER_ORDER"] = stock_oper_order;
						
						twma7_in["HALL_NO_TO"] = hall_to;
					
						
						twma7_in["STOCK_NO_TO"] = stock_to;
						if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("LOGIC_STOCK_NO"))
						{
							twma7_in["LOGIC_STOCK_NO"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["LOGIC_STOCK_NO"].ToString().Trim();
						}

						if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("STOCK_OPER_ORDER_FIN")
							&& bcls_rec->Tables["CMD_MAKE"].Rows[i]["STOCK_OPER_ORDER_FIN"].ToString().Trim()!="")
						{
							twma7_in["STOCK_OPER_ORDER_FIN"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["STOCK_OPER_ORDER_FIN"].ToString().Trim();
						}
						else
						{
							twma7_in["STOCK_OPER_ORDER_FIN"] = stock_oper_order;
						}
		
						if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("VEHICLE_NO"))
						{
							twma7_in["VEHICLE_NO"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["VEHICLE_NO"].ToString().Trim();
						}

						if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("HALL_NO_FIN")
							&& bcls_rec->Tables["CMD_MAKE"].Rows[i]["HALL_NO_FIN"].ToString().Trim()!="")
						{
							twma7_in["HALL_NO_FIN"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["HALL_NO_FIN"].ToString().Trim();
							Log::Trace("", __FUNCTION__, "001 HALL_NO_FIN = [{0}] ", twma7_in["HALL_NO_FIN"].ToString());
						}
						else
						{
							twma7_in["HALL_NO_FIN"] = twma7_in["HALL_NO_TO"];
							Log::Trace("", __FUNCTION__, "002 HALL_NO_FIN = [{0}] ", twma7_in["HALL_NO_FIN"].ToString());
						}

						if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("STOCK_NO_FIN")
							&& bcls_rec->Tables["CMD_MAKE"].Rows[i]["STOCK_NO_FIN"].ToString().Trim()!="")
						{
							twma7_in["STOCK_NO_FIN"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["STOCK_NO_FIN"].ToString().Trim();
						}
						else
						{
							twma7_in["STOCK_NO_FIN"] = twma7_in["STOCK_NO_TO"];
						}

						if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("MAIN_MAT_NO"))
						{
							twma7_in["MAIN_MAT_NO"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["MAIN_MAT_NO"].ToString().Trim();
						}

						if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("UNIT_CODE"))
						{
							twma7_in["UNIT_CODE"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["UNIT_CODE"].ToString().Trim();
						}
					
						if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("CRANE_CMDGRPNO")
							&& bcls_rec->Tables["CMD_MAKE"].Rows[i]["CRANE_CMDGRPNO"].ToDecimal() != 0)
						{
							twma7_in["CRANE_CMDGRPNO"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["CRANE_CMDGRPNO"].ToDecimal();
						}

						if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("CMD_METHOD"))
						{
							twma7_in["CMD_METHOD"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["CMD_METHOD"].ToString().Trim();
						}
						if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("MAT_STATUS"))
						{
							twma7_in["MAT_STATUS"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["MAT_STATUS"].ToString().Trim();
						}
						if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("MAT_DESTION"))
						{
							twma7_in["MAT_DESTION"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["MAT_DESTION"].ToString().Trim();
						}
						if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("MAT_THEORY_WT"))
						{
							twma7_in["MAT_THEORY_WT"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["MAT_THEORY_WT"].ToString().Trim();
						}
						if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("HEAD_TAIL_WITH_DIFF"))
						{
							twma7_in["HEAD_TAIL_WITH_DIFF"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["HEAD_TAIL_WITH_DIFF"].ToString().Trim();
						}
						if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("BATCH_TASK_NO")
							&& bcls_rec->Tables["CMD_MAKE"].Rows[i]["BATCH_TASK_NO"].ToDecimal() != 0)
						{
							twma7_in["BATCH_TASK_NO"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["BATCH_TASK_NO"].ToDecimal();
						}

						if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("STOCK_PLACE_NO_FIN")
							&& bcls_rec->Tables["CMD_MAKE"].Rows[i]["STOCK_PLACE_NO_FIN"].ToString().Trim() != "")
						{
							twma7_in["STOCK_PLACE_NO_FIN"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["STOCK_PLACE_NO_FIN"].ToString().Trim();
							Log::Trace("", __FUNCTION__, "111 STOCK_PLACE_NO_FIN = [{0}] ", twma7_in["STOCK_PLACE_NO_FIN"].ToString());
						}
						else if (stock_oper_order!="32")
						{
							twma7_in["STOCK_PLACE_NO_FIN"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["STOCK_PLACE_NO_TO"].ToString().Trim();
							Log::Trace("", __FUNCTION__, "222 STOCK_PLACE_NO_FIN = [{0}] ", twma7_in["STOCK_PLACE_NO_FIN"].ToString());
						}

						if (stock_oper_order == "32")//过跨倒跺
						{
							if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("STOCK_PLACE_NO_TO")
								&& bcls_rec->Tables["CMD_MAKE"].Rows[i]["STOCK_PLACE_NO_TO"].ToString().Trim()!="")
							{
								twma7_in["STOCK_PLACE_NO_TO"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["STOCK_PLACE_NO_TO"].ToString();								
							}
							else
							{
								//过跨命令
								bcls_rec_tr.Tables["CMD_TR_REM"].Rows[0]["MAT_NO"] = mat_no;
								bcls_rec_tr.Tables["CMD_TR_REM"].Rows[0]["HALL_TO"] = twma7_in["HALL_NO_TO"];
								bcls_rec_tr.Tables["CMD_TR_REM"].Rows[0]["HALL_FR"] = twma7_in["HALL_NO_FR"];
								bcls_rec_tr.Tables["CMD_TR_REM"].Rows[0]["STOCK_PLACE_NO"] = twma7_in["STOCK_PLACE_NO_FROM"];
								bcls_rec_tr.Tables["CMD_TR_REM"].Rows[0]["STOCK_OPER_ORDER_FIN"] = twma7_in["STOCK_OPER_ORDER_FIN"];
								bcls_rec_tr.Tables["CMD_TR_REM"].Rows[0]["MAT_THEORY_WT"] = twma7_in["MAT_ACT_WT"];
								bcls_rec_tr.Tables["CMD_TR_REM"].Rows[0]["MAT_THICK"] = twma7_in["MAT_ACT_THICK"];
								bcls_rec_tr.Tables["CMD_TR_REM"].Rows[0]["MAT_WIDTH"] = twma7_in["MAT_ACT_WIDTH"];
								bcls_rec_tr.Tables["CMD_TR_REM"].Rows[0]["MAT_LEN"] = twma7_in["MAT_ACT_LEN"];
								bcls_rec_tr.Tables["CMD_TR_REM"].Rows[0]["LAYERNO"] = twma7_in["YARD_LAYER_FROM"].ToString();

								/*doFlag = f_wmsmsm_cranecmd_tr_rem(&bcls_rec_tr, bcls_ret, conn);*/
								Log::Trace("", __FUNCTION__, "sqlst2132");
								if (doFlag != 0)
								{
									Log::Trace("", __FUNCTION__, "sqlst2132");
									throw CApplicationException(-1, s.msg, log.Location);
								}
								Log::Trace("", __FUNCTION__, "sqlst211132");
								twma7_in["STOCK_PLACE_NO_TO"] = bcls_rec->Tables[0].Rows[0]["STOCK_PLACE_NO_TO"].ToString().Trim();
								Log::Trace("", __FUNCTION__, "sqlst111132");
							}
						
							
						}
						else
						{
							if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("STOCK_PLACE_NO_TO"))
							{
								twma7_in["STOCK_PLACE_NO_TO"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["STOCK_PLACE_NO_TO"].ToString().Trim();

								Log::Trace("", __FUNCTION__, "目标材料库位号{0}", twma7_in["STOCK_PLACE_NO_TO"].ToString());
							}
							else
							{
								twma7_in["STOCK_PLACE_NO_TO"] = " ";
							}

						}

						//创建吊运流水号
						prod_seq_no = EPGetNextSeq("WM_CMD_SEQ", conn);
						if (doFlag < 0 || prod_seq_no.Trim() == "")
						{
							sprintf(s.msg, "获取流水号失败，请查看EPED21【WM_CMD_SEQ】是否正常!");
							throw CApplicationException(-1, s.msg, s.svc_name);
						}
						twma7_in["CMD_SEQ"] = dateTime.SubstringNE(2, 6) + prod_seq_no; // 10位 
					}
					else
					{					
						twma7_in["STOCK_OPER_ORDER"] = "30";
						twma7_in["STOCK_OPER_ORDER_FIN"] = "30";
						twma7_in["STOCK_NO_TO"] = Table_mat.Rows[0]["STOCK_NO"];
						twma7_in["HALL_NO_TO"] = Table_mat.Rows[0]["HALL_NO"];
						twma7_in["HALL_NO_FIN"] = Table_mat.Rows[0]["HALL_NO"];
						twma7_in["STOCK_NO_FIN"] = twma7_in["STOCK_NO_TO"];
						twma7_in["STOCK_PLACE_NO_TO"] = " ";		
						prod_seq_no = EPGetNextSeq("WM_CMD_SEQ", conn);
						if (doFlag < 0 || prod_seq_no.Trim() == "")
						{
							sprintf(s.msg, "获取流水号失败，请查看EPED21【WM_CMD_SEQ】是否正常!");
							throw CApplicationException(-1, s.msg, s.svc_name);
						}
						twma7_in["CMD_SEQ"] = dateTime.SubstringNE(2, 6) + prod_seq_no; // 10位 
					}

					twma7_in["MOVE_TYPE"] = twma7_in["STOCK_OPER_ORDER"];
					twma7_in["MOVE_TYPE"] = twma7_in["STOCK_OPER_ORDER"];
					if (twma7_in["STOCK_PLACE_NO_TO"].ToString().Trim()=="")
					{
					
							bcls_rec_auto_1.Tables[0].Rows[0]["MAT_NO"] = Table_mat.Rows[0]["MAT_NO"].ToString();
							bcls_rec_auto_1.Tables[0].Rows[0]["STOCK_OPER_ORDER"] = twma7_in["STOCK_OPER_ORDER"].ToString();
							bcls_rec_auto_1.Tables[0].Rows[0]["HALL_NO"] =  hall_to;
							
							doFlag = f_auto(&bcls_rec_auto_1, &bcls_ret_logic, conn);
							if (doFlag != 0)
							{
								throw CApplicationException(-1, s.msg, log.Location);
							}
							twma7_in["LOGIC_STOCK_NO"] = bcls_ret_logic.Tables[0].Rows[0]["LOGIC_STOCK_NO"].ToString();
							twma7_in["STOCK_PLACE_NO_TO"] = bcls_ret_logic.Tables[0].Rows[0]["STOCK_PLACE_NO"].ToString();
						}
					
					if (stock_oper_order != "32"&&twma7_in["STOCK_PLACE_NO_FIN"].ToString().Trim() == "")
					{
						twma7_in["STOCK_PLACE_NO_FIN"] = twma7_in["STOCK_PLACE_NO_TO"];
						Log::Trace("", __FUNCTION__, "333 STOCK_PLACE_NO_FIN = [{0}] ", twma7_in["STOCK_PLACE_NO_FIN"].ToString());
					}
					
					//twma7_in["TR_NO"] = "1";//台车号
					//CRANE_NO
					twma7_in["CRANE_NO"] = crane_no;//台车号
					//twma7_in["SEND_FLAG"] = "0";
					

					if (twma7_in["STOCK_PLACE_NO_TO"].ToString().Trim() != "")
					{
						//更新垛位状态
						doFlag = f_wm00_pileinfocal(twma7_in["STOCK_NO"].ToString(), twma7_in["STOCK_PLACE_NO_TO"].ToString(), bcls_ret, conn);
						if (doFlag != 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}
					}

					if (twma7_in["STOCK_PLACE_NO_FIN"].ToString().Trim() != "")
					{
						//更新垛位状态
						doFlag = f_wm00_pileinfocal(twma7_in["STOCK_NO_FIN"].ToString(), twma7_in["STOCK_PLACE_NO_FIN"].ToString(), bcls_ret, conn);
						if (doFlag != 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}
					}

					lg_hc = EPGetNextSeq("LG_HC", conn);
					lg_hc = dateTime.Substring(0, 8) + lg_hc;

					twma7_in["MAIN_MAT_NO"] = (CDecimal::Parse(lg_hc)).ToString();
					twma7_in.TrimOrBlank();
					Log::Trace("", __FUNCTION__, "444 STOCK_PLACE_NO_FIN = [{0}] ", twma7_in["STOCK_PLACE_NO_FIN"].ToString());
					twma7_in.Insert();

					Log::Trace("", __FUNCTION__, "lg_hc：  【{0}】", lg_hc);

				}
			}	
			else if(stock_oper_order.SubstringNE(0, 1) == "2")
			{
			

					Log::Trace("", __FUNCTION__, "出库命令");
					Table_position_from.Rows.Add();
					Table_position_from.Rows[SEQ]["POSITION_FROM"] = Table_mat.Rows[0]["STOCK_PLACE_NO"];
					SEQ++;

					if (Table_mat.Rows[0]["DEV_DIV"].ToString().Trim() == "4") //辊道位置，不考虑上层倒垛
					{
						sqlstr = "select mat_no,stock_place_no,layerno from twma2 a where mat_no='" + Table_mat.Rows[0]["MAT_NO"].ToString() + "'";
					}
					else
					{
						sqlstr = "select mat_no,stock_place_no,layerno from twma2 a where stock_place_no='" + Table_mat.Rows[0]["STOCK_PLACE_NO"].ToString() + "' and INT(layerno)>=INT(" + Table_mat.Rows[0]["LAYERNO"].ToString() + ") and a.mat_no not in (select mat_no from twma7 b where a.mat_no = b.mat_no) order by INT(a.layerno) desc";
					}
					Log::Trace("", __FUNCTION__, "sqlstr:{0}", sqlstr);
					Db::QueryTable(sqlstr, Table_position);

					if (Table_position.Rows.get_Count() > 1)
					{
						sprintf(s.msg, "材料号[%s]上方有板坯号，不允许直接吊运出库", (const char*)Table_mat.Rows[0]["MAT_NO"]);
						throw CApplicationException(-1, s.msg, log.Location);
					}

					Log::Trace("", __FUNCTION__, "上层个数:{0}", Table_position.Rows.get_Count() - 1);
					Log::Debug("", __FUNCTION__, "Table_position 记录数	= [{0}]", Table_position.Rows.get_Count());
					for (int j = 0; j < Table_position.Rows.get_Count(); j++)
					{
						Log::Trace("", __FUNCTION__, "生成材料{0}命令", Table_position.Rows[j]["MAT_NO"].ToString());
						Table_mat.Clear();
						//获取材料信息STOCK_PLACE_NO_TO
						sqlstr = "select a.MAT_NO,a.MAT_KIND,a.MAT_THICK,a.MAT_WIDTH,a.MAT_LEN,a.MAT_THEORY_WT,b.STOCK_NO,substr(b.STOCK_PLACE_NO,4,1) HALL_NO,b.STOCK_PLACE_NO,b.LAYERNO,c.X_FROM,c.Y_FROM,c.LOGIC_STOCK_NO "
							" from " + v_table_name + " a,twma2 b,twm04 c"
							" where c.STOCK_PLACE_NO=b.STOCK_PLACE_NO AND a.MAT_NO=b.MAT_NO and a.MAT_NO='" + Table_position.Rows[j]["mat_no"].ToString() + "'";
						Db::QueryTable(sqlstr, Table_mat);
						if (Table_mat.Rows.get_Count() == 0)
						{
							sprintf(s.msg, "材料[%s]信息不存在", (const char*)Table_position.Rows[j]["MAT_NO"]);
							throw CApplicationException(-1, s.msg, log.Location);
						}

						twma7_in.Reset();
						twma7_in["REC_CREATE_TIME"] = dateTime;
						twma7_in["REC_CREATOR"] = s.userid;
						twma7_in["MAT_NO"] = Table_position.Rows[j]["MAT_NO"].ToString();
						twma7_in["STOCK_OPER_ORDER"] = stock_oper_order;
						twma7_in["CRANE_INST_CODE"] = stock_oper_order.SubstringNE(0, 1);
						twma7_in["CRANE_INST_STATUS"] = "0";

						//写材料信息
						twma7_in["MAT_KIND"] = Table_mat.Rows[0]["MAT_KIND"].ToString();
						twma7_in["MAT_SHAPE_FLAG"] = "1";
						twma7_in["MAT_ACT_THICK"] = Table_mat.Rows[0]["MAT_THICK"].ToDecimal();
						twma7_in["MAT_ACT_WIDTH"] = Table_mat.Rows[0]["MAT_WIDTH"].ToDecimal();
						twma7_in["MAT_ACT_LEN"] = Table_mat.Rows[0]["MAT_LEN"].ToDecimal();
						twma7_in["MAT_ACT_WT"] = Table_mat.Rows[0]["MAT_THEORY_WT"].ToDecimal();

						//写当前垛位信息
						twma7_in["STOCK_NO"] = Table_mat.Rows[0]["STOCK_NO"];
						twma7_in["STOCK_NO_FROM"] = Table_mat.Rows[0]["STOCK_NO"];
						twma7_in["HALL_NO_FR"] = Table_mat.Rows[0]["STOCK_PLACE_NO"].ToString().SubstringNE(3, 1);
						twma7_in["STOCK_PLACE_NO_FROM"] = Table_mat.Rows[0]["STOCK_PLACE_NO"];
						twma7_in["YARD_LAYER_FROM"] = Table_mat.Rows[0]["LAYERNO"];

						//写目标垛位信息
						if (j == Table_position.Rows.get_Count() - 1)
						{
							twma7_in["STOCK_OPER_ORDER"] = stock_oper_order;

							twma7_in["HALL_NO_TO"] = hall_to;

							twma7_in["STOCK_NO_TO"] = stock_to;
							if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("LOGIC_STOCK_NO"))
							{
								twma7_in["LOGIC_STOCK_NO"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["LOGIC_STOCK_NO"].ToString().Trim();
							}

							if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("STOCK_OPER_ORDER_FIN")
								&& bcls_rec->Tables["CMD_MAKE"].Rows[i]["STOCK_OPER_ORDER_FIN"].ToString().Trim() != "")
							{
								twma7_in["STOCK_OPER_ORDER_FIN"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["STOCK_OPER_ORDER_FIN"].ToString().Trim();
							}
							else
							{
								twma7_in["STOCK_OPER_ORDER_FIN"] = stock_oper_order;
							}

							if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("VEHICLE_NO"))
							{
								twma7_in["VEHICLE_NO"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["VEHICLE_NO"].ToString().Trim();
							}

							if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("HALL_NO_FIN")
								&& bcls_rec->Tables["CMD_MAKE"].Rows[i]["HALL_NO_FIN"].ToString().Trim() != "")
							{
								twma7_in["HALL_NO_FIN"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["HALL_NO_FIN"].ToString().Trim();
							}
							else
							{
								twma7_in["HALL_NO_FIN"] = " ";
							}

							if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("CMD_METHOD"))
							{
								twma7_in["CMD_METHOD"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["CMD_METHOD"].ToString().Trim();
							}
							if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("MAT_STATUS"))
							{
								twma7_in["MAT_STATUS"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["MAT_STATUS"].ToString().Trim();
							}
							if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("MAT_DESTION"))
							{
								twma7_in["MAT_DESTION"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["MAT_DESTION"].ToString().Trim();
							}
							if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("MAT_THEORY_WT"))
							{
								twma7_in["MAT_THEORY_WT"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["MAT_THEORY_WT"].ToString().Trim();
							}
							if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("HEAD_TAIL_WITH_DIFF"))
							{
								twma7_in["HEAD_TAIL_WITH_DIFF"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["HEAD_TAIL_WITH_DIFF"].ToString().Trim();
							}
							if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("STOCK_NO_FIN")
								&& bcls_rec->Tables["CMD_MAKE"].Rows[i]["STOCK_NO_FIN"].ToString().Trim() != "")
							{
								twma7_in["STOCK_NO_FIN"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["STOCK_NO_FIN"].ToString().Trim();
							}
							else
							{
								twma7_in["STOCK_NO_FIN"] = twma7_in["STOCK_NO_TO"];
							}

							if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("MAIN_MAT_NO"))
							{
								twma7_in["MAIN_MAT_NO"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["MAIN_MAT_NO"].ToString().Trim();
							}

							if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("UNIT_CODE"))
							{
								twma7_in["UNIT_CODE"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["UNIT_CODE"].ToString().Trim();
							}

							if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("CRANE_CMDGRPNO")
								&& bcls_rec->Tables["CMD_MAKE"].Rows[i]["CRANE_CMDGRPNO"].ToDecimal() != 0)
							{
								twma7_in["CRANE_CMDGRPNO"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["CRANE_CMDGRPNO"].ToDecimal();
							}

							if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("BATCH_TASK_NO")
								&& bcls_rec->Tables["CMD_MAKE"].Rows[i]["BATCH_TASK_NO"].ToDecimal() != 0)
							{
								twma7_in["BATCH_TASK_NO"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["BATCH_TASK_NO"].ToDecimal();
							}

							if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("STOCK_PLACE_NO_FIN")
								&& bcls_rec->Tables["CMD_MAKE"].Rows[i]["STOCK_PLACE_NO_FIN"].ToString().Trim() != "")
							{
								twma7_in["STOCK_PLACE_NO_FIN"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["STOCK_PLACE_NO_FIN"].ToString().Trim();
							}
							else if (stock_oper_order != "32")
							{
								twma7_in["STOCK_PLACE_NO_FIN"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["STOCK_PLACE_NO_TO"].ToString().Trim();
							}

							
							if (bcls_rec->Tables["CMD_MAKE"].Columns.Contains("STOCK_PLACE_NO_TO"))
								{
									twma7_in["STOCK_PLACE_NO_TO"] = bcls_rec->Tables["CMD_MAKE"].Rows[i]["STOCK_PLACE_NO_TO"].ToString().Trim();

									Log::Trace("", __FUNCTION__, "目标材料库位号{0}", twma7_in["STOCK_PLACE_NO_TO"].ToString());
								}
								else
								{
									twma7_in["STOCK_PLACE_NO_TO"] = " ";
								}

							//创建吊运流水号
							prod_seq_no = EPGetNextSeq("WM_CMD_SEQ", conn);
							if (doFlag < 0 || prod_seq_no.Trim() == "")
							{
								sprintf(s.msg, "获取流水号失败，请查看EPED21【WM_CMD_SEQ】是否正常!");
								throw CApplicationException(-1, s.msg, s.svc_name);
							}
							twma7_in["CMD_SEQ"] = dateTime.SubstringNE(2, 6) + prod_seq_no; // 10位 

							lg_hc = EPGetNextSeq("LG_HC", conn);
							lg_hc = dateTime.Substring(0, 8) + lg_hc;
							Log::Trace("", __FUNCTION__, "lg_hc22：  【{0}】", lg_hc);

						}
						else
						{
							twma7_in["STOCK_OPER_ORDER"] = "30";
							twma7_in["STOCK_OPER_ORDER_FIN"] = "30";
							twma7_in["STOCK_NO_TO"] = Table_mat.Rows[0]["STOCK_NO"];
							twma7_in["HALL_NO_TO"] = Table_mat.Rows[0]["HALL_NO"];
							twma7_in["HALL_NO_FIN"] = Table_mat.Rows[0]["HALL_NO"];
							twma7_in["STOCK_NO_FIN"] = twma7_in["STOCK_NO_TO"];
							twma7_in["STOCK_PLACE_NO_TO"] = " ";
							prod_seq_no = EPGetNextSeq("WM_CMD_SEQ", conn);
							if (doFlag < 0 || prod_seq_no.Trim() == "")
							{
								sprintf(s.msg, "获取流水号失败，请查看EPED21【WM_CMD_SEQ】是否正常!");
								throw CApplicationException(-1, s.msg, s.svc_name);
							}
							twma7_in["CMD_SEQ"] = dateTime.SubstringNE(2, 6) + prod_seq_no; // 10位 

							lg_hc = EPGetNextSeq("LG_HC", conn);
							lg_hc = dateTime.Substring(0, 8) + lg_hc;
							Log::Trace("", __FUNCTION__, "lg_hc33：  【{0}】", lg_hc);

						}

						twma7_in["MOVE_TYPE"] = twma7_in["STOCK_OPER_ORDER"];
						twma7_in["MOVE_TYPE"] = twma7_in["STOCK_OPER_ORDER"];
						if (twma7_in["STOCK_PLACE_NO_TO"].ToString().Trim() == "")
						{

							bcls_rec_auto_1.Tables[0].Rows[0]["MAT_NO"] = Table_mat.Rows[0]["MAT_NO"].ToString();
							bcls_rec_auto_1.Tables[0].Rows[0]["STOCK_OPER_ORDER"] = twma7_in["STOCK_OPER_ORDER"].ToString();
							bcls_rec_auto_1.Tables[0].Rows[0]["HALL_NO"] = hall_to;

							doFlag = f_auto(&bcls_rec_auto_1, &bcls_ret_logic, conn);
							if (doFlag != 0)
							{
								throw CApplicationException(-1, s.msg, log.Location);
							}
							twma7_in["LOGIC_STOCK_NO"] = bcls_ret_logic.Tables[0].Rows[0]["LOGIC_STOCK_NO"].ToString();
							twma7_in["STOCK_PLACE_NO_TO"] = bcls_ret_logic.Tables[0].Rows[0]["STOCK_PLACE_NO"].ToString();
						}

						if (stock_oper_order != "32"&&twma7_in["STOCK_PLACE_NO_FIN"].ToString().Trim() == "")
							twma7_in["STOCK_PLACE_NO_FIN"] = twma7_in["HALL_NO_FIN"];

						//twma7_in["SEND_FLAG"] = "0";
						twma7_in.TrimOrBlank();
						twma7_in.Insert();

					

						if (twma7_in["STOCK_PLACE_NO_TO"].ToString().Trim() != "")
						{
							//更新垛位状态
							doFlag = f_wm00_pileinfocal(twma7_in["STOCK_NO"].ToString(), twma7_in["STOCK_PLACE_NO_TO"].ToString(), bcls_ret, conn);
							if (doFlag != 0)
							{
								throw CApplicationException(-1, s.msg, log.Location);
							}
						}

						if (twma7_in["STOCK_PLACE_NO_FIN"].ToString().Trim() != "")
						{
							//更新垛位状态
							doFlag = f_wm00_pileinfocal(twma7_in["STOCK_NO_FIN"].ToString(), twma7_in["STOCK_PLACE_NO_FIN"].ToString(), bcls_ret, conn);
							if (doFlag != 0)
							{
								throw CApplicationException(-1, s.msg, log.Location);
							}
						}
					}
				
			}
			Log::Trace("", __FUNCTION__, "4。5");
		}
		Log::Trace("", __FUNCTION__, "4。6");
		//计算组吊号
		for (int i = 0; i < Table_position_from.Rows.get_Count(); i++)
		{
			Log::Trace("", __FUNCTION__, "4。7");
			if (stock_place_no_from.Trim() == "" || stock_place_no_from != Table_position_from.Rows[i]["POSITION_FROM"].ToString())
			{
				bcls_rec_group.Tables["CMD_GROUP"].Rows.Add();
				bcls_rec_group.Tables["CMD_GROUP"].Rows[SEQ_B]["STOCK_PLACE_NO"] = Table_position_from.Rows[i]["POSITION_FROM"].ToString();
				stock_place_no_from = Table_position_from.Rows[i]["POSITION_FROM"].ToString();
				SEQ_B++;
			}

		}
		
		if (bcls_rec_group.Tables["CMD_GROUP"].Rows.get_Count() > 0)
		{
			/*doFlag = f_wmsmsm_cranecmd_group(&bcls_rec_group, bcls_ret, conn);*/
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		
		
	
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		Log::Trace("", __FUNCTION__, "{0}", ex.GetMsg());
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。{1}", arguments, 1);
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


