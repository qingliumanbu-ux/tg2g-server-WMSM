/* ****************************************************************************
*	Copyright (c) Baosight Corporation 2008 . All Rights Reserved.
*  	BM2PES 宝信生产执行系统
*****************************************************************************
*  程序名称			: smsm02_ins
*  程序描述			: 出厂实绩输入
*  备注说明			:
*  修改历史			:
*  		2011-12-27 吴新
* **************************************************************************** */
/***** C/C++ 的标准头文件部分 *****/
#include <stdio.h>
#include "stdafx.h"
using namespace BM2;
using namespace BM2::Data;
using namespace BM2::Data::DbClient;
//#include "x3000s2.h"



//int f_epep_get_shift_group(char * pszShiftClass, char * pszShiftTime, char * pszShiftNo, char * pszShiftGroup);	// 生成班次、班组
int f_sm00_bill_deliver(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);
int f_sm00_bill_transfer(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);
//int f_smsm02_ins(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);
BM2_FUNCTION_IMPORT
int f_sm00_md_no(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);
//int f_cm_gggi01_snd(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);

BM2F_ENTERACE(smsm02_ins1)
/* ***** -EP_SYSTEM_HEAD_END ***** */
int f_smsm02_ins1(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 静态变量定义 ***** */
	int doFlag = 0;
	int fetchRowCount = 0;
	int row_count = 0, ret = 0;
	int count_num = 1;

	CModel tsmpe02 = CModel("TSMPE02");
	CModel tsmpe00 = CModel("TSMPE00");
	CModel tsmpe10 = CModel("TSMPE10");
	CModel tsmpe11 = CModel("TSMPE11");
	CModel tsmsm09 = CModel("tsmsm09");
	CModel tsmsm10 = CModel("TSMSM10");
	CModel tsmsm10_v = CModel("TSMSM10");
	CModel tsmsm14 = CModel("TSMSM14");

	/* ***** 程序变量 ***** */
	CString c_user = s.userid, c_mat_kind = " ", datetime = " ";
	//CString c_stock_no=" ",c_stock_no_end=" ",c_query_type=" ";

	CString c_del_cause = " ", c_del_type = " ", c_dept_code = " ", c_dept_code_cname = " ", c_red_cause_code = " ", c_red_cause_desc = " ", c_out_mark = " ";
	CString c_confm_plan_no = " ", c_ready_bill_no = " ", c_confm_status = " ", c_mat_no = "''", c_order_no = " ";
	CString c_car_name = " ", c_fin_pos = " ", c_stock_place_no_to = " ", c_logistics_no = " ";
	CString c_factory_div = " ", c_bill_of_lading_no = " ", c_vehicle_no = " ", c_stock_no = " ", c_proc_type = " ", c_act_datetime = " ";
	CString weigh_app_no = " ", c_bill_of_lading_detailno = " ", c_vehicle_name = " ", c_carry_company_name = " ";
	CString c_stock_place_no = " ";//在制品转库标记
	CString mat_no = "", order_no = "", bill_of_lading_no = "";
	CString stacking_no_r = "发货成功，码单号：";
	CString t_bill_of_lading_no = " ";
	CString v_bill_of_lading_no = " ", v_logistics_no = " ", v_bill_of_lading_detailno = " ";
	CString v_delivery_mode = " ";
	CString v_mat_no = "''";
	CString delivy_maker = " ";
	CString c_unq_no = "";
	CDecimal mat_act_wt = 0;
	CDecimal mat_wt = 0, v_mat_wt = 0;
	CDecimal mat_num = 0;
	CDecimal red_num = 0;

	char  c_delivy_shift[5] = " ", c_delivy_group[10] = " ";
	char  c_datetime[15];

	/* ***** 数据库SQL操作字符串 ***** */
	CString	sqlstr(""), sqlstr0(""), sqlstr1(""), sqlstr2(""), sqlstr3(""), sqlstr4(""), sqlstr5(""), sqlstr6(""), sqlstr7("");

	/* ***** 数据库操作类定义 ***** */
	CDbCommand execute_sql(conn);
	CDbCommand execute_sql_01(conn);


	/* *******定义一个EIClass object */
	EIClass ds_ready_bill_no, ds_mat_no;

	ds_mat_no.Tables[0].Columns.Add(DT_STRING, "mat_no");                   // 材料号
	ds_mat_no.Tables[0].Columns.Add(DT_STRING, "crane_inst_type");          // 吊车命令类别
	ds_mat_no.Tables[0].Columns.Add(DT_STRING, "stock_place_no_to");        // 目标垛位号
	ds_mat_no.Tables[0].Columns.Add(DT_STRING, "fin_pos");                  // 最终位置
	ds_mat_no.Tables[0].Columns.Add(DT_STRING, "car_name");                 // 卡车名

	/* *******定义一个获取码单号 EIClass object */
	EIClass  ds_stacking_no_rec;
	ds_stacking_no_rec.Tables[0].Columns.Add(DT_STRING, "stock_no");
	ds_stacking_no_rec.Tables[0].Columns.Add(DT_STRING, "stacking_no");
	ds_stacking_no_rec.Tables[0].Columns.Add(DT_STRING, "factory_div");
	ds_stacking_no_rec.Tables[0].Rows.Add();
	EIClass  ds_stacking_no_ret;

	/* ***** 应用程序开始处理 ***** */
	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		EDLog(1, 1, "datetime =[%s]", (const char*)datetime);
		Log::Trace("", __FUNCTION__, "Table[0].Rows.Count = [{0}]", bcls_rec->Tables[0].Rows.get_Count());

		//	  c_factory_div        = bcls_rec->Tables[0].Rows[0]["factory_div"].ToString().TrimOrBlank();
		if (bcls_rec->Tables[0].Columns.Contains("VEHICLE_NO") == true)
			c_vehicle_no = bcls_rec->Tables[0].Rows[0]["VEHICLE_NO"].ToString().TrimOrBlank();
		//c_stock_no = bcls_rec->Tables[0].Rows[0]["stock_no"].ToString().TrimOrBlank();
		//c_proc_type = bcls_rec->Tables[0].Rows[0]["proc_type"].ToString().TrimOrBlank();
		//c_act_datetime = bcls_rec->Tables[0].Rows[0]["act_datetime"].ToString().TrimOrBlank();
		if (bcls_rec->Tables[0].Columns.Contains("vehicle_name") == true)
			c_vehicle_name = bcls_rec->Tables[0].Rows[0]["vehicle_name"].ToString().TrimOrBlank();
		if (bcls_rec->Tables[0].Columns.Contains("ORDER_NO") == true)
			c_order_no = bcls_rec->Tables[0].Rows[0]["ORDER_NO"].ToString().TrimOrBlank();
		if (bcls_rec->Tables[0].Columns.Contains("MAT_WT") == true)
			mat_wt = bcls_rec->Tables[0].Rows[0]["MAT_WT"].ToDecimal();

		if (bcls_rec->Tables[0].Columns.Contains("mat_num") == true)//在制品转库标记
		{
			mat_num = bcls_rec->Tables[0].Rows[0]["mat_num"].ToDecimal();
		}

		if (bcls_rec->Tables[0].Columns.Contains("carry_company_name") == true)
		{
			c_carry_company_name = bcls_rec->Tables[0].Rows[0]["carry_company_name"].ToString().TrimOrBlank();
		}
		if (bcls_rec->Tables[0].Columns.Contains("delivery_mode") == true)
		{
			v_delivery_mode = bcls_rec->Tables[0].Rows[0]["delivery_mode"].ToString().TrimOrBlank();
		}
		if (bcls_rec->Tables[0].Columns.Contains("stock_place_no") == true)//在制品转库标记
		{
			c_stock_place_no = bcls_rec->Tables[0].Rows[0]["stock_place_no"].ToString().TrimOrBlank();
		}
		if (bcls_rec->Tables[0].Columns.Contains("delivy_maker") == true)
		{
			delivy_maker = bcls_rec->Tables[0].Rows[0]["delivy_maker"].ToString().TrimOrBlank();
		}
		if (c_proc_type.Trim() == "") {
			c_proc_type = "B";
		}

		EDLog(1, 1, "c_vehicle_no =[%s]", (const char*)c_vehicle_no);
		EDLog(1, 1, "c_proc_type =[%s]", (const char*)c_proc_type);
		EDLog(1, 1, "c_act_datetime =[%s]", (const char*)c_act_datetime);
		EDLog(1, 1, "c_stock_no =[%s]", (const char*)c_stock_no);
		EDLog(1, 1, "c_vehicle_name =[%s]", (const char*)c_vehicle_name);      //装车方案
		EDLog(1, 1, "c_carry_company_name =[%s]", (const char*)c_carry_company_name);    //承运商 物流
		EDLog(1, 1, "delivery_mode =[%s]", (const char*)v_delivery_mode);    //是否出厂 TO 物流
		EDLog(1, 1, "c_stock_place_no =[%s]", (const char*)c_stock_place_no);    //在制品转库标记
		EDLog(1, 1, "v_mat_no =[%s]", (const char*)v_mat_no);
		EDLog(1, 1, "delivy_maker =[%s]", (const char*)delivy_maker);
		Log::Debug("", __FUNCTION__, "mat_wt=[{0}]", mat_wt);
		Log::Debug("", __FUNCTION__, "mat_num=[{0}]", mat_num);

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			if (bcls_rec->Tables[0].Columns.Contains("bill_of_lading_no") == true)
				bill_of_lading_no = bcls_rec->Tables[0].Rows[i]["bill_of_lading_no"].ToString().TrimOrBlank();
			//c_logistics_no = bcls_rec->Tables[0].Rows[i]["logistics_no"].ToString().TrimOrBlank();
			if (bcls_rec->Tables[0].Columns.Contains("bill_of_lading_detailno") == true)
				c_bill_of_lading_detailno = bcls_rec->Tables[i].Rows[i]["bill_of_lading_detailno"].ToString().TrimOrBlank();

			if (c_bill_of_lading_detailno == ""){
				tsmsm09["BILL_OF_LADING_NO"] = bill_of_lading_no;
				tsmsm09.Query("BILL_OF_LADING_NO");
				c_bill_of_lading_detailno = tsmsm09["BILL_OF_LADING_DETAILNO"];
			}

			if (bcls_rec->Tables[0].Columns.Contains("order_no") == true)
			{
				c_order_no = bcls_rec->Tables[0].Rows[0]["order_no"].ToString().TrimOrBlank();
			}

			//拼接材料号
			v_mat_no += ",'" + mat_no + "'";
			mat_wt = mat_wt + v_mat_wt;

			Log::Trace("", __FUNCTION__, "for i = [{0}]", i);
			EDLog(1, 1, "c_logistics_no =[%s]", (const char*)c_logistics_no);
			EDLog(1, 1, "c_bill_of_lading_detailno =[%s]", (const char*)c_bill_of_lading_detailno);
			EDLog(1, 1, "t_bill_of_lading_no =[%s]", (const char*)t_bill_of_lading_no);
			EDLog(1, 1, "c_order_no =[%s]", (const char*)c_order_no);

			//if (order_no != c_order_no && bill_of_lading_no != t_bill_of_lading_no){
			//	strcpy(s.msg, /*_RES("SM00C0000022")*/"装同一车，需要合同号和提货单号一样");
			//	throw CApplicationException(-1, s.msg, s.svc_name);
			//}

		}//end for 


		Log::Trace("", __FUNCTION__, "v_mat_no = [{0}]", v_mat_no);
		Log::Trace("", __FUNCTION__, "mat_wt = [{0}]", mat_wt);

		tsmsm10_v.Reset();
		tsmsm10_v["BILL_OF_LADING_NO"] = t_bill_of_lading_no;
		tsmsm10_v["BILL_OF_LADING_DETAILNO"] = c_bill_of_lading_detailno;
		tsmsm10_v["ORDER_NO"] = c_order_no;
		tsmsm10_v.Query("BILL_OF_LADING_NO, BILL_OF_LADING_DETAILNO, ORDER_NO");
		if (tsmsm10_v["PLAN_NUM"].ToDecimal() > 100)
		{
			strcpy(s.msg, _RES("计划下发的材料超过了100个，不能装车请重新下发货计划。")/*请输入车号.*/);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		if (c_vehicle_no == " ")
		{
			strcpy(s.msg, _RES("SM00C0000022")/*请输入车号.*/);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		if (c_act_datetime == " ") c_act_datetime = datetime;

		strcpy(c_datetime, (const char *)c_act_datetime);

		// 调用函数生成班次、班组
		ret = f_epep_get_shift_group("SM", c_datetime, c_delivy_shift, c_delivy_group);					// 调函数
		if (ret != 0)
		{
			sprintf(s.msg, "f_epep_get_shift_group函数调用出错.");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}


		/* ***** format sql ****** */
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			/*sqlstr1 = CString(
			" SELECT count(1) from tsmpe02 t where bill_of_lading_no = @bill_of_lading_no and confm_status = '4' "
			);*/
			sqlstr1 = CString(
				" SELECT count(1) from tsmpe02 t where confm_status = '4' " //'6' "
				"    and bill_of_lading_no = @bill_of_lading_no and order_no = @order_no "
				"    and bill_of_lading_detailno = @bill_of_lading_detailno "
				"    and mat_no in (" + v_mat_no + ") "
				);

			sqlstr2 = CString(
				"		UPDATE tsmpe02 SET                          "
				"  rec_revise_time = @datetime, "
				"  rec_revisor     = @c_user,       "
				"  vehicle_no      = @vehicle_no,    "
				"  delivy_maker      = @delivy_maker,    "
				"  confm_status    = '8'             "
				"  WHERE " //confm_status = '6'" //2.接收成功 4.已确认 6.出厂计划已接收 8.开始发货 9.发货完成
				"     bill_of_lading_no = @bill_of_lading_no and order_no = @order_no "
				"    and bill_of_lading_detailno = @bill_of_lading_detailno "
				"    and mat_no in (" + v_mat_no + ") "
				);

			sqlstr3 = CString(
				" SELECT count(1) from tsmpe02 t WHERE mat_no = @mat_no and confm_status = '4' " //2.接收成功 4.已确认 6.出厂计划已接收 8.开始发货 9.发货完成
				);

			sqlstr4 = CString(
				"		UPDATE tsmpe02 SET                          "
				"  rec_revise_time = @datetime, "
				"  rec_revisor     = @c_user,       "
				"  vehicle_no      = @vehicle_no,    "
				"  confm_status    = '8'             "
				"  WHERE mat_no in (" + v_mat_no + ") " //and confm_status = '6'"
				);

			sqlstr5 = CString(
				" SELECT count(1) from tsmpe02 where order_no = @order_no and out_mark <> '2' "
				);

			sqlstr6 = CString(
				" SELECT count(1) from tsmpe02 where red_flag > ' ' and mat_no in (" + v_mat_no + ") "
				);

			break;
		}


		//2018-8-11 插入tsmpe11表  begin
		if (c_stock_place_no.Trim() == "Y")//在制品转库标记
		{
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr1 = CString("select stock_no,sum(MAT_ACT_WT)  from tmmhr01 where plan_no = @logistics_no "
					"group by stock_no "
					);
				sqlstr3 = CString(
					" SELECT * FROM tsmsm10 WHERE bill_of_lading_detailno = @bill_of_lading_detailno and logistics_no =@logistics_no "
					);
			}
			sqlstr = sqlstr1;
			execute_sql.SetCommandText(sqlstr);
			execute_sql.Parameters.Set("bill_of_lading_no", t_bill_of_lading_no);
			execute_sql.Parameters.Set("bill_of_lading_detailno", c_bill_of_lading_detailno);
			execute_sql.Parameters.Set("logistics_no", c_logistics_no);
			execute_sql.ExecuteReader();
			while (execute_sql.Read())
			{
				tsmpe11["STOCK_NO"] = execute_sql.GetString(1);
				mat_act_wt = execute_sql.GetDecimal(2);

				/* ***** 根据库区生成码单号  ***** */
				ret = 0;
				ds_stacking_no_rec.Tables[0].Rows[0]["stock_no"] = tsmpe11["STOCK_NO"];
				ret = f_sm00_md_no(&ds_stacking_no_rec, &ds_stacking_no_ret, conn);
				if (ret < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				tsmpe11["STACKING_NO"] = ds_stacking_no_ret.Tables[0].Rows[0]["stacking_no"].ToString().TrimOrBlank();
				Log::Debug("", __FUNCTION__, "STACKING_NO=[{0}]", tsmpe02["STACKING_NO"].ToString());

				stacking_no_r = stacking_no_r + tsmpe02["STACKING_NO"].ToString().Trim() + "  ";
				Log::Debug("", __FUNCTION__, "stacking_no_r=[{0}]", stacking_no_r);

				/*  ***** 获取提单信息 ***** */
				sqlstr = sqlstr3;
				execute_sql_01.SetCommandText(sqlstr);
				execute_sql_01.Parameters.Set("bill_of_lading_no", t_bill_of_lading_no);
				execute_sql_01.Parameters.Set("bill_of_lading_detailno", c_bill_of_lading_detailno);
				execute_sql_01.Parameters.Set("logistics_no", c_logistics_no);
				execute_sql_01.ExecuteReader();
				while (execute_sql_01.Read())
				{
					execute_sql_01.Fetch(tsmsm10);
				}
				execute_sql_01.Close();

				tsmpe11["REC_CREATE_TIME"] = datetime;
				tsmpe11["REC_CREATOR"] = c_user;
				tsmpe11["REC_REVISE_TIME"] = " ";
				tsmpe11["REC_REVISOR"] = " ";
				tsmpe11["ARCHIVE_FLAG"] = " ";
				tsmpe11["FACTORY_DIV"] = "PA";
				tsmpe11["BILL_OF_LADING_NO"] = t_bill_of_lading_no;
				tsmpe11["VEHICLE_NO"] = c_vehicle_no;
				tsmpe11["ORDER_NO"] = tsmsm10["ORDER_NO"];
				tsmpe11["SG_SIGN"] = tsmsm10["SG_SIGN"];
				//tsmpe11.SG_STD = tsmpe00.SG_STD;
				tsmpe11["PROD_CODE"] = tsmsm10["PROD_CODE"];
				tsmpe11["DELIVY_TIME"] = datetime;
				tsmpe11["TRNP_MODE_CODE"] = tsmsm10["TRNP_MODE_CODE"];
				tsmpe11["CONSIGNE_NAME"] = tsmsm10["CONSIGNE_NAME"];
				tsmpe11["ORDER_CUST_CNAME"] = tsmsm10["ORDER_CUST_CNAME"];
				tsmpe11["BALANCE_USER_NAME"] = tsmsm10["BALANCE_USER_NAME"];
				tsmpe11["CONVEY_UNIT_NAME"] = tsmsm10["CONVEY_UNIT_NAME"];
				tsmpe11["DELIVY_PLACE_NAME"] = tsmsm10["DELIVY_PLACE_NAME"];
				tsmpe11["STACKING_PRINTS"] = 0;
				tsmpe11["DELIVY_REMARK"] = tsmsm10["DELIVY_REMARK"];
				tsmpe11["STACKING_WT"] = tsmsm10["PLAN_WT"];
				tsmpe11["STACKING_GROSS_WT"] = tsmsm10["PLAN_WT"];
				tsmpe11["BILL_OF_LADING_DETAILNO"] = tsmsm10["BILL_OF_LADING_DETAILNO"];
				tsmpe11["STACKING_NUM"] = tsmsm10["PLAN_NUM"];
				tsmpe11["STACKING_TUBE"] = tsmsm10["PLAN_NUM"];
				tsmpe11["MAT_ACT_WT"] = tsmsm10["PLAN_WT"];
				tsmpe11["LOGISTICS_NO"] = tsmsm10["LOGISTICS_NO"];

				/* 插入码单表 */
				if (!tsmpe11.Insert())
				{
					EDLog(1, 1, "Insert.tsmpe11 ERROR! ");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}
			execute_sql.Close();
		}
		else
		{
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr0 = CString("select count(1) "
					"from tsmpe02 where  "//order_no = @order_no
					" mat_no in (" + v_mat_no + ") and stacking_no > '  ' " //
					);

				sqlstr1 = CString("select stock_no,sum(MAT_WT) "
					"from tsmpe02 where order_no = @order_no "
					"and mat_no in (" + v_mat_no + ") "
					"group by stock_no "
					);
				sqlstr2 = CString(
					" UPDATE TSMPE02 SET  stacking_no = @stacking_no, "
					"  rec_revise_time = @datetime, "
					"  rec_revisor     = @c_user,       "
					"  vehicle_no      = @vehicle_no,    "
					"  delivy_maker      = @delivy_maker,    "
					"  bill_of_lading_no      = @bill_of_lading_no,    "
					"  bill_of_lading_detailno      = @bill_of_lading_detailno,    "
					"  confm_status    = '8'             "
					"                     WHERE  order_no = @order_no "
					"  and mat_no in (" + v_mat_no + ") "
					);
				sqlstr4 = CString(
					" SELECT * FROM tsmpe00 WHERE order_no = @order_no  "
					);
				sqlstr3 = CString(
					//" SELECT * FROM tsmpe10 WHERE bill_of_lading_no = @bill_of_lading_no "
					" SELECT * FROM tsmsm10 WHERE bill_of_lading_no = @bill_of_lading_no and bill_of_lading_detailno = @bill_of_lading_detailno and order_no =@order_no "
					);

				sqlstr5 = CString(
					"		select distinct factory_div  from tsmpe02   "
					"		 where order_no = @order_no  "
					);
				sqlstr6 = CString(
					" SELECT count(1) from tsmpe02 where red_flag = '1' and mat_no in (" + v_mat_no + ") "
					);

				sqlstr7 = CString(
					" SELECT SUM(STACKING_WT) from tsmpe11 t where STACKING_STATUS != 'X' "
					"    and bill_of_lading_no = @bill_of_lading_no "
					"    and bill_of_lading_detailno = @bill_of_lading_detailno "
					);

				//校验挑选材料是否已装车
				sqlstr = sqlstr0;
				execute_sql.SetCommandText(sqlstr);
				execute_sql.Parameters.Set("order_no", c_order_no);
				CDecimal stack_ing_num = execute_sql.ExecuteScalar();
				execute_sql.Close();

				Log::Debug("", __FUNCTION__, "sqlstr00000000=[{0}]", sqlstr);

				if (stack_ing_num > 0)
				{
					strcpy(s.msg, _RES("挑选的材料已装车，请重新挑选材料 ")/*请输入车号.*/);
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				sqlstr = sqlstr1;
				execute_sql.SetCommandText(sqlstr);
				execute_sql.Parameters.Set("bill_of_lading_no", t_bill_of_lading_no);
				execute_sql.Parameters.Set("bill_of_lading_detailno", c_bill_of_lading_detailno);
				execute_sql.Parameters.Set("order_no", c_order_no);
				execute_sql.ExecuteReader();
				while (execute_sql.Read())
				{
					tsmpe02["STOCK_NO"] = execute_sql.GetString(1);
					mat_act_wt = execute_sql.GetDecimal(2);

					/* ***** 根据库区生成码单号  ***** */
					ret = 0;
					ds_stacking_no_rec.Tables[0].Rows[0]["stock_no"] = tsmpe02["STOCK_NO"];
					ret = f_sm00_md_no(&ds_stacking_no_rec, &ds_stacking_no_ret, conn);
					if (ret < 0)
					{
						throw CApplicationException(-1, s.msg, s.svc_name);
					}

					tsmpe02["STACKING_NO"] = ds_stacking_no_ret.Tables[0].Rows[0]["stacking_no"].ToString().TrimOrBlank();
					Log::Debug("", __FUNCTION__, "STACKING_NO=[{0}]", tsmpe02["STACKING_NO"].ToString());

					stacking_no_r = stacking_no_r + tsmpe02["STACKING_NO"].ToString().Trim() + "  ";
					Log::Debug("", __FUNCTION__, "stacking_no_r=[{0}]", stacking_no_r);

					/* ***** 将码单号回写到材料表  ***** */
					sqlstr = sqlstr2;
					execute_sql_01.SetCommandText(sqlstr);
					execute_sql_01.Parameters.Set("stacking_no", tsmpe02["STACKING_NO"]);
					execute_sql_01.Parameters.Set("bill_of_lading_no", t_bill_of_lading_no);
					execute_sql_01.Parameters.Set("bill_of_lading_detailno", c_bill_of_lading_detailno);
					execute_sql_01.Parameters.Set("order_no", c_order_no);
					execute_sql_01.Parameters.Set("datetime", datetime);
					execute_sql_01.Parameters.Set("c_user", c_user);
					execute_sql_01.Parameters.Set("vehicle_no", c_vehicle_no);
					execute_sql_01.Parameters.Set("delivy_maker", delivy_maker);
					execute_sql_01.ExecuteNonQuery();
					execute_sql_01.Close();

					/*  ***** 获取提单信息 ***** */
					sqlstr = sqlstr3;
					execute_sql_01.SetCommandText(sqlstr);
					execute_sql_01.Parameters.Set("bill_of_lading_no", t_bill_of_lading_no);
					execute_sql_01.Parameters.Set("bill_of_lading_detailno", c_bill_of_lading_detailno);
					execute_sql_01.Parameters.Set("order_no", c_order_no);
					execute_sql_01.ExecuteReader();
					while (execute_sql_01.Read())
					{
						//execute_sql_01.Fetch(tsmpe10); 
						execute_sql_01.Fetch(tsmsm10);
					}
					execute_sql_01.Close();

					tsmpe02["ORDER_NO"] = tsmsm10["ORDER_NO"]; //取计划带下来的合同号

					/*  ***** 获取准发单据信息 ***** */
					sqlstr = sqlstr4;
					execute_sql_01.SetCommandText(sqlstr);
					execute_sql_01.Parameters.Set("order_no", tsmpe02["ORDER_NO"].ToString());

					execute_sql_01.ExecuteReader();
					if (execute_sql_01.Read())
					{
						execute_sql_01.Fetch(tsmpe00);
					}
					execute_sql_01.Close();

					/*  ***** 获取厂别 ***** */
					sqlstr = sqlstr6;
					execute_sql_01.SetCommandText(sqlstr);
					red_num = execute_sql_01.ExecuteScalar();
					Log::Info("", __FUNCTION__, "红冲材料数量 red_num=[{0}]", red_num);
					execute_sql_01.Close();
					if (red_num > 0)
					{
						sprintf(s.msg, "勾选的材料中有做红冲申请的，请重新查询再挑选材料装车.");
						throw CApplicationException(-1, s.msg, s.svc_name);
					}

					tsmpe02["FACTORY_DIV"] = "H1";

					//检验发货重量是否超计划重量
					sqlstr = sqlstr7;
					execute_sql_01.SetCommandText(sqlstr);
					CDecimal stacking_wt = execute_sql_01.ExecuteScalar();
					Log::Info("", __FUNCTION__, "已装车未发货重量 STACKING_WT=[{0}]", stacking_wt);
					execute_sql_01.Close();

					Log::Info("", __FUNCTION__, "计划发货重量 tsmsm10.PLAN_WT=[{0}]", tsmsm10["PLAN_WT"].ToString());
					Log::Info("", __FUNCTION__, "已发重量 tsmsm10.DELIVY_WT=[{0}]", tsmsm10["DELIVY_WT"].ToString());
					Log::Info("", __FUNCTION__, "本次装车重量 mat_wt=[{0}]", mat_wt);
					if (tsmsm10["PLAN_WT"].ToDecimal() < tsmsm10["DELIVY_WT"].ToDecimal() + mat_wt + stacking_wt)
					{
						sprintf(s.msg, "已发重量+本次装车重量+已装车未发货重量，大于了计划发货重量，不能发货.");
						throw CApplicationException(-1, s.msg, s.svc_name);
					}

					tsmpe11["REC_CREATE_TIME"] = datetime;
					tsmpe11["REC_CREATOR"] = c_user;
					tsmpe11["REC_REVISE_TIME"] = " ";
					tsmpe11["REC_REVISOR"] = " ";
					tsmpe11["ARCHIVE_FLAG"] = " ";
					tsmpe11["STACKING_NO"] = tsmpe02["STACKING_NO"];
					tsmpe11["FACTORY_DIV"] = tsmpe02["FACTORY_DIV"];
					tsmpe11["STOCK_NO"] = tsmpe02["STOCK_NO"];
					tsmpe11["BILL_OF_LADING_NO"] = t_bill_of_lading_no;

					tsmpe11["CONFM_PLAN_NO"] = tsmpe00["CONFM_PLAN_NO"];
					tsmpe11["READY_BILL_NO"] = tsmpe00["READY_BILL_NO"];

					tsmpe11["VEHICLE_NO"] = c_vehicle_no;
					tsmpe11["ORDER_NO"] = tsmpe02["ORDER_NO"];
					tsmpe11["EXPORT_FLAG"] = tsmpe00["EXPORT_FLAG"];
					tsmpe11["SG_SIGN"] = tsmpe00["SG_SIGN"];
					tsmpe11["SG_STD"] = tsmpe00["SG_STD"];
					tsmpe11["PROD_CODE"] = tsmpe00["PROD_CODE"];
					tsmpe11["PROD_CNAME"] = tsmpe00["PROD_CNAME"];
					tsmpe11["PROD_ENAME"] = tsmpe00["PROD_ENAME"];
					tsmpe11["ORDER_THICK"] = tsmpe00["ORDER_THICK"];
					tsmpe11["ORDER_WIDTH"] = tsmpe00["ORDER_WIDTH"];
					tsmpe11["ORDER_LEN"] = tsmpe00["ORDER_LEN"];
					tsmpe11["ORDER_LEN_MIN"] = tsmpe00["ORDER_MIN_LEN"];
					tsmpe11["ORDER_LEN_MAX"] = tsmpe00["ORDER_MAX_LEN"];
					tsmpe11["DELIVY_TIME"] = datetime;
					tsmpe11["DELIVY_SHIFT"] = tsmpe02["DELIVY_SHIFT"];
					tsmpe11["DELIVY_GROUP"] = tsmpe02["DELIVY_GROUP"];
					tsmpe11["DELIVY_MAKER"] = tsmpe02["DELIVY_MAKER"];
					//tsmpe11.TRNP_MODE_CODE = tsmpe10.TRNP_MODE_CODE;
					tsmpe11["TRNP_MODE_CODE"] = tsmsm10["TRNP_MODE_CODE"];
					tsmpe11["CONSIGNE_NAME"] = tsmsm10["CONSIGNE_NAME"];
					tsmpe11["ORDER_CUST_CNAME"] = tsmpe00["ORDER_CUST_CNAME"];
					//tsmpe11.BALANCE_USER_NAME = tsmpe10.BALANCE_USER_NAME;
					//tsmpe11.CONVEY_UNIT_NAME = tsmpe10.CONVEY_UNIT_NAME;
					//tsmpe11.DELIVY_PLACE_NAME = tsmpe10.DELIVY_PLACE_NAME;
					tsmpe11["BALANCE_USER_NAME"] = tsmsm10["BALANCE_USER_NAME"];
					tsmpe11["CONVEY_UNIT_NAME"] = tsmsm10["CONVEY_UNIT_NAME"];
					tsmpe11["DELIVY_PLACE_NAME"] = tsmsm10["DELIVY_PLACE_NAME"];
					tsmpe11["PRIVATE_ROUTE_CODE"] = tsmpe00["PRIVATE_ROUTE_CODE"];
					tsmpe11["PRIVATE_ROUTE_NAME"] = tsmpe00["PRIVATE_ROUTE_NAME"];
					tsmpe11["STACKING_PRINTS"] = 0;
					tsmpe11["DELIVY_REMARK"] = tsmsm10["DELIVY_REMARK"];
					tsmpe11["STACKING_WT"] = mat_wt;//  tsmsm10.PLAN_WT;
					tsmpe11["STACKING_GROSS_WT"] = mat_wt; // tsmsm10.PLAN_WT"];
					tsmpe11["BILL_OF_LADING_DETAILNO"] = tsmsm10["BILL_OF_LADING_DETAILNO"];
					tsmpe11["STACKING_STATUS"] = "4";
					tsmpe11["STACKING_NUM"] = mat_num; //tsmsm10.PLAN_NUM;
					tsmpe11["STACKING_TUBE"] = mat_num; //tsmsm10.PLAN_NUM;
					tsmpe11["MAT_ACT_WT"] = mat_wt; // tsmsm10.PLAN_WT;

					/*tsmsm10.BILL_OF_LADING_NO = tsmpe02.BILL_OF_LADING_NO;
					tsmsm10.ORDER_NO = tsmpe02.ORDER_NO;
					tsmsm10.Query("BILL_OF_LADING_NO,ORDER_NO");*/
					tsmpe11["LOGISTICS_NO"] = tsmsm10["LOGISTICS_NO"];
					// 1:二次过磅  2:复检  0:无要求 
					if (tsmsm10["MEASURE_WT_FLAG"].ToString().Trim() == "1" || tsmsm10["MEASURE_WT_FLAG"].ToString().Trim() == "2") //需要计量委托，生成计量申请号  产线 + 'YYMMDD' + 4位流水
					{
						CString seq_no = EPGetNextSeq("MM_PROD_SEQ_NO", conn); //6位
						weigh_app_no = "hr" + datetime.Substring(2, 6) + seq_no.Substring(2, 4);
						EDLog(1, 1, "weigh_app_no =[%s]", (const char*)weigh_app_no);

						tsmpe11["WEIGH_APP_NO"] = weigh_app_no;
					}

					/* 插入码单表 */
					if (!tsmpe11.Insert())
					{
						EDLog(1, 1, "Insert.tsmpe11 ERROR! ");
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
				}
				execute_sql.Close();
			}

		}//end if 在制品转库标记

		Log::Info("", __FUNCTION__, "Update... t_bill_of_lading_no=[{0}]", t_bill_of_lading_no);
		Log::Info("", __FUNCTION__, "Update... c_bill_of_lading_detailno=[{0}]", c_bill_of_lading_detailno);
		Log::Info("", __FUNCTION__, "Update... c_order_no=[{0}]", c_order_no);

		tsmsm10.Reset();
		tsmsm10["BILL_OF_LADING_NO"] = t_bill_of_lading_no;
		tsmsm10["BILL_OF_LADING_DETAILNO"] = c_bill_of_lading_detailno;
		tsmsm10["ORDER_NO"] = c_order_no;
		tsmsm10["DELIVY_PLAN_STATUS"] = "4";
		tsmsm10["OUT_STOCK_TIME"] = c_act_datetime;  //装车日期
		tsmsm10["VEHICLE_NAME"] = c_vehicle_name; //装车方案
		tsmsm10["VEHICLE_NO"] = c_vehicle_no;
		tsmsm10["CARRY_COMPANY_NAME"] = c_carry_company_name;//承运商 物流
		tsmsm10["DELIVERY_MODE"] = v_delivery_mode;  //是否出厂 TO 物流
		int num10 = tsmsm10.Update("DELIVY_PLAN_STATUS,OUT_STOCK_TIME,VEHICLE_NAME,VEHICLE_NO,CARRY_COMPANY_NAME,DELIVERY_MODE", "BILL_OF_LADING_NO,BILL_OF_LADING_DETAILNO,ORDER_NO");
		Log::Info("", __FUNCTION__, "Update... num10=[{0}]", num10);

		tsmsm09.Reset();
		tsmsm09["BILL_OF_LADING_NO"] = t_bill_of_lading_no;
		tsmsm09["BILL_OF_LADING_DETAILNO"] = c_bill_of_lading_detailno;
		tsmsm09["ORDER_NO"] = c_order_no;
		tsmsm09["VEHICLE_NAME"] = c_vehicle_name; //装车方案
		tsmsm09["VEHICLE_NO"] = c_vehicle_no;
		tsmsm09["CARRY_COMPANY_NAME"] = c_carry_company_name;//承运商 物流
		tsmsm09["DELIVERY_MODE"] = v_delivery_mode;  //是否出厂 TO 物流
		if (tsmsm09.QueryCount("BILL_OF_LADING_NO,BILL_OF_LADING_DETAILNO,ORDER_NO") == 0)
		{
			EDLog(1, 1, "计划还没下发完，不能操作 =[%s]", t_bill_of_lading_no);
			sprintf(s.msg, "计划还没下发完，不能操作.");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		tsmsm09["DELIVY_PLAN_STATUS"] = "4";         //计划执行
		tsmsm09["OUT_STOCK_TIME"] = c_act_datetime;  //装车日期
		int num09 = tsmsm09.Update("DELIVY_PLAN_STATUS,OUT_STOCK_TIME,VEHICLE_NAME,VEHICLE_NO,CARRY_COMPANY_NAME,DELIVERY_MODE", "BILL_OF_LADING_NO,BILL_OF_LADING_DETAILNO,ORDER_NO");
		Log::Info("", __FUNCTION__, "Update... num09=[{0}]", num09);

		//宽板在码单发货画面_SM0004P  手动计量委托
		//if (tsmsm10.MEASURE_WT_FLAG == "1" || tsmsm10.MEASURE_WT_FLAG == "2")   // 1:二次过磅  2:复检  0:无要求  
		//{
		//	if (bcls_rec->Tables.Contains("GGGI01") == false)
		//	{
		//		bcls_rec->Tables.Add("GGGI01");
		//		bcls_rec->Tables["GGGI01"].Columns.Add(DT_STRING, "BILL_OF_LADING_NO");
		//		bcls_rec->Tables["GGGI01"].Columns.Add(DT_STRING, "BILL_OF_LADING_DETAILNO");
		//		bcls_rec->Tables["GGGI01"].Columns.Add(DT_STRING, "LOGISTICS_NO");
		//		bcls_rec->Tables["GGGI01"].Rows.Add();
		//		bcls_rec->Tables["GGGI01"].Rows[0]["BILL_OF_LADING_NO"] = t_bill_of_lading_no;
		//		bcls_rec->Tables["GGGI01"].Rows[0]["LOGISTICS_NO"] = c_logistics_no;
		//		bcls_rec->Tables["GGGI01"].Rows[0]["BILL_OF_LADING_DETAILNO"] = c_bill_of_lading_detailno;
		//	}
		//	ret = f_cm_gggi01_snd(bcls_rec, bcls_ret, conn);//计量委托
		//	if (ret < 0)
		//	{
		//		sprintf(s.msg, "f_cm_gggi01_snd函数调用出错.");
		//		throw CApplicationException(-1, s.msg, s.svc_name);
		//	}
		//}
		//2018-8-11 插入tsmpe11表  end

		/* ***** 按材料输入实绩   ***** */
		if (0 != c_proc_type.Compare("M") && 0 != c_proc_type.Compare("B"))
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		//更新承运商实际承运重量
		tsmsm14["BILL_OF_LADING_NO"] = c_bill_of_lading_detailno;
		tsmsm14["VEHICLE_NO"] = c_vehicle_no;
		tsmsm14.Query("BILL_OF_LADING_NO,VEHICLE_NO");
		Log::Debug("", __FUNCTION__, "tsmsm14.ACT_LOAD_WT=[{0}]", tsmsm14["ACT_LOAD_WT"].ToString());
		Log::Debug("", __FUNCTION__, "tsmsm14.CARRY_USER_CODE=[{0}]", tsmsm14["CARRY_USER_CODE"].ToString());
		tsmsm14["ACT_LOAD_WT"] = tsmsm14["ACT_LOAD_WT"] + mat_wt;
		tsmsm14["REC_REVISE_TIME"] = datetime;
		tsmsm14["REC_REVISOR"] = s.userid;
		tsmsm14.Update("REC_REVISE_TIME,REC_REVISOR,ACT_LOAD_WT", "BILL_OF_LADING_NO,CARRY_USER_CODE");

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;
	}
	catch (const CApplicationException& ex)
	{
		//	strncpy(s.msg, (const char*)ex.GetMsg(), 399); //返回前台，与EI.EIManager.Instance.CallService(this.ef_args.formPartition,)方法返回的EI.EIInfo对象的sys_info.msg参数对应
		EDLog(1, 1, "error=[%s]", (const char*)s.msg);
		s.flag = ex.GetCode();       //返回前台，与EI.EIManager.Instance.CallService(this.ef_args.formPartition,)方法返回的EI.EIInfo对象的sys_info.flag参数对应
		doFlag = -1;
	}

	catch (const CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), 399); //返回前台，与EI.EIManager.Instance.CallService(this.ef_args.formPartition,)方法返回的EI.EIInfo对象的sys_info.msg参数对应
		s.flag = ex.GetCode();       //返回前台，与EI.EIManager.Instance.CallService(this.ef_args.formPartition,)方法返回的EI.EIInfo对象的sys_info.flag参数对应
		doFlag = -1;
	}

	return doFlag;
}
