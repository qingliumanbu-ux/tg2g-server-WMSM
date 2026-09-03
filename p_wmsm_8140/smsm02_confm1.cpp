/* ****************************************************************************
*	Copyright (c) Baosight Corporation 2008 . All Rights Reserved.
*  	BM2PES 宝信生产执行系统
*****************************************************************************
*  程序名称			: smhr02_ins
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
//int f_sm00_bill_deliver_z(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);
int f_sm00_bill_transfer(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);
//int f_smhr02_confm(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);
BM2_FUNCTION_IMPORT
int f_sm00_md_no(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);

int f_mmsm99(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);

BM2F_ENTERACE(smsm02_confm1)
/* ***** -EP_SYSTEM_HEAD_END ***** */
int f_smsm02_confm1(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 静态变量定义 ***** */
	int doFlag = 0;
	int fetchRowCount = 0;
	int row_count = 0, ret = 0;
	int remark_share_no = 0;
	int blkNum = 0;
	int i_count = 0;

	CModel tsmpe02 = CModel("TSMPE02");
	CModel tsmpe11 = CModel("TSMPE11");
	CModel tsmsm09 = CModel("TSMSM09");
	CModel tsmsm10 = CModel("TSMSM10");

	/* ***** 程序变量 ***** */
	CString c_user = s.userid, c_mat_kind = " ", datetime = " ";
	//CString c_stock_no=" ",c_stock_no_end=" ",c_query_type=" ";

	CString c_del_cause = " ", c_del_type = " ", c_dept_code = " ", c_dept_code_cname = " ", c_red_cause_code = " ", c_red_cause_desc = " ", c_out_mark = " ";
	CString c_confm_plan_no = " ", c_ready_bill_no = " ", c_confm_status = " ", c_mat_no = " ";
	CString c_car_name = " ", c_fin_pos = " ", c_stock_place_no_to = " ", c_bill_of_lading_detailno = " ";
	CString c_factory_div = " ", c_bill_of_lading_no = " ", c_vehicle_no = " ", c_stock_no = " ", c_proc_type = " ", c_act_datetime = " ";
	CString weigh_app_no = " ", c_logistics_no = "";
	CString stacking_no_r = "发货成功，码单号：";
	CString c_remark_share = "";
	CString c_stock_place_no = ""; //在制品转库标记
	CString c_stacking_no = "";
	CString c_order_no = "";


	char  c_delivy_shift[5] = " ", c_delivy_group[10] = " ";
	char  c_datetime[15];

	/* ***** 数据库SQL操作字符串 ***** */
	CString	sqlstr(""), sqlstr0(""), sqlstr1(""), sqlstr2(""), sqlstr3(""), sqlstr4(""), sqlstr5(""), sqlstr6("");
	CString sqlstr7("");

	/* ***** 数据库操作类定义 ***** */
	CDbCommand execute_sql(conn);
	CDbCommand execute_sql_01(conn);
	CDbCommand cmd_inq(conn);

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

	EIClass bcls_sm00;
	if (!bcls_sm00.Tables.Contains("SM00"))
	{
		bcls_sm00.Tables[0].set_TableName("SM00");
		bcls_sm00.Tables[0].Columns.Add(DT_STRING, "bill_of_lading_no");
		bcls_sm00.Tables[0].Columns.Add(DT_STRING, "bill_of_lading_detailno");
		bcls_sm00.Tables[0].Columns.Add(DT_STRING, "logistics_no");
		bcls_sm00.Tables[0].Columns.Add(DT_STRING, "stacking_no");
		bcls_sm00.Tables[0].Rows.Add();
	}

	/* 添加与设置块名 */
	blkNum = bcls_rec->Tables.IndexOf("MM0099");
	if (blkNum < 0)
	{
		bcls_rec->Tables.Add("MM0099");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_ID");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "SYSTEM_ID");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "FUNC_ID");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_NO");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_WT");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_ACT_WT");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_GROSS_WT");//材料毛重
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MEASURE_WT_FLAG");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_NUM");

	}
	/* ***** 应用程序开始处理 ***** */
	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


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
			sqlstr1 = CString(
				" SELECT count(1) from tsmpe02 t where bill_of_lading_no = @bill_of_lading_no and confm_status = '8' "
				);

			sqlstr2 = CString(
				"  UPDATE tsmpe02 SET  "
				"  delivy_time     = @delivy_time, "
				"  delivy_shift    = @delivy_shift, "
				"  delivy_group    = @delivy_group, "
				"  delivy_maker    = @c_user,   "
				"  confm_status    = '9'   "
				"  WHERE bill_of_lading_no = @bill_of_lading_no and confm_status = '8' "
				);

			sqlstr5 = CString(
				" SELECT count(1) from tsmpe02 where bill_of_lading_no = @bill_of_lading_no and out_mark <> '2' "
				);

			sqlstr6 = CString(
				" SELECT out_mark from tsmpe02 where mat_no = @mat_no "
				);
			sqlstr0 = CString(
				" SELECT count(1) from tsmsm10 t where remark_share = @remark_share and delivy_plan_status < '4' "
				);
			
			sqlstr7 = CString(
				"SELECT DISTINCT BILL_OF_LADING_NO ,STACKING_NO,ORDER_NO FROM tsmpe02 t WHERE 1 = 1 "
				); 

			break;
		}

		/* ***** 按提单输入实绩   ***** */

		Log::Info("", __FUNCTION__, "***** Tables[0].Rows=[{0}]", bcls_rec->Tables[0].Rows.get_Count());
		if (bcls_rec->Tables[0].Columns.Contains("bill_of_lading_no") == true)
			c_bill_of_lading_no = bcls_rec->Tables[0].Rows[0]["bill_of_lading_no"].ToString().TrimOrBlank();
		if (bcls_rec->Tables[0].Columns.Contains("bill_of_lading_detailno") == true)
			c_bill_of_lading_detailno = bcls_rec->Tables[0].Rows[0]["bill_of_lading_detailno"].ToString().TrimOrBlank();
		if (bcls_rec->Tables[0].Columns.Contains("logistics_no") == true)
			c_logistics_no = bcls_rec->Tables[0].Rows[0]["logistics_no"].ToString().TrimOrBlank();
		if (bcls_rec->Tables[0].Columns.Contains("stacking_no") == true)
			c_stacking_no = bcls_rec->Tables[0].Rows[0]["stacking_no"].ToString().TrimOrBlank();
		c_vehicle_no = bcls_rec->Tables[0].Rows[0]["vehicle_no"].ToString().TrimOrBlank();
		if (bcls_rec->Tables[0].Columns.Contains("STOCK_PLACE_NO") == true) //在制品转库标记
		{
			c_stock_place_no = bcls_rec->Tables[0].Rows[0]["STOCK_PLACE_NO"].ToString().TrimOrBlank();
		}
		if (bcls_rec->Tables[0].Columns.Contains("ORDER_NO") == true) //合同号
		{
			c_order_no = bcls_rec->Tables[0].Rows[0]["ORDER_NO"].ToString().TrimOrBlank();
		}
		if (c_vehicle_no == " ")
		{
			strcpy(s.msg, _RES("请输入车号")/*请输入车号.*/);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		if (c_bill_of_lading_no == "" || c_stacking_no == ""){
			sqlstr7 += "AND VEHICLE_NO = '" + c_vehicle_no + "'";
			execute_sql_01.SetCommandText(sqlstr7);
			execute_sql_01.ExecuteReader();
			
			while (execute_sql_01.Read()){
				//for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)//循环码单
				//{

					Log::Info("", __FUNCTION__, "**************** for i = [{0}]", i_count++);
					c_bill_of_lading_no = execute_sql_01.GetString(1);
					c_stacking_no = execute_sql_01.GetString(2);
					c_order_no = execute_sql_01.GetString(3);

					//Log::Info("", __FUNCTION__, "**************** for i = [{0}]", i);
					//if (bcls_rec->Tables[0].Columns.Contains("bill_of_lading_no") == true)
					//	c_bill_of_lading_no = bcls_rec->Tables[0].Rows[i]["bill_of_lading_no"].ToString().TrimOrBlank();
					//if (bcls_rec->Tables[0].Columns.Contains("bill_of_lading_detailno") == true)
					//	c_bill_of_lading_detailno = bcls_rec->Tables[0].Rows[i]["bill_of_lading_detailno"].ToString().TrimOrBlank();
					if (c_bill_of_lading_detailno == ""){
						tsmsm09["BILL_OF_LADING_NO"] = c_bill_of_lading_no;
						tsmsm09.Query("BILL_OF_LADING_NO");
						c_bill_of_lading_detailno = tsmsm09["BILL_OF_LADING_DETAILNO"];
						//c_remark_share = tsmsm09.REMARK_SHARE;
					}
					//if (bcls_rec->Tables[0].Columns.Contains("logistics_no") == true)
					//	c_logistics_no = bcls_rec->Tables[0].Rows[i]["logistics_no"].ToString().TrimOrBlank();
					////c_remark_share = bcls_rec->Tables[0].Rows[i]["remark_share"].ToString().Trim();
					//if (bcls_rec->Tables[0].Columns.Contains("stacking_no") == true)
					//	c_stacking_no = bcls_rec->Tables[0].Rows[i]["stacking_no"].ToString().TrimOrBlank();
					EDLog(1, 1, "c_bill_of_lading_no =[%s]", (const char*)c_bill_of_lading_no);
					EDLog(1, 1, "c_bill_of_lading_detailno =[%s]", (const char*)c_bill_of_lading_detailno);
					EDLog(1, 1, "c_logistics_no =[%s]", (const char*)c_logistics_no);
					EDLog(1, 1, "c_remark_share =[%s]", (const char*)c_remark_share);
					EDLog(1, 1, "c_stacking_no =[%s]", (const char*)c_stacking_no);
					if (c_bill_of_lading_no == " ")
					{
						strcpy(s.msg, _RES("请输计划号")/*请输入车号.*/);
						throw CApplicationException(-1, s.msg, s.svc_name);
						break;
					}

					//c_bill_of_lading_no = bcls_rec->Tables[0].Rows[i]["bill_of_lading_no"].ToString().TrimOrBlank();
					//c_vehicle_no = bcls_rec->Tables[0].Rows[i]["vehicle_no"].ToString().TrimOrBlank();
					//c_bill_of_lading_detailno = bcls_rec->Tables[0].Rows[i]["bill_of_lading_detailno"].ToString().TrimOrBlank();

					//if (bcls_rec->Tables[0].Columns.Contains("STOCK_PLACE_NO") == true) //在制品转库标记
					//{
					//	c_stock_place_no = bcls_rec->Tables[0].Rows[i]["STOCK_PLACE_NO"].ToString().TrimOrBlank();
					//}
					//if (bcls_rec->Tables[0].Columns.Contains("ORDER_NO") == true) //合同号
					//{
					//	c_order_no = bcls_rec->Tables[0].Rows[i]["ORDER_NO"].ToString().TrimOrBlank();
					//}

					EDLog(1, 1, "c_vehicle_no =[%s]", (const char*)c_vehicle_no);
					EDLog(1, 1, "c_stock_place_no =[%s]", (const char*)c_stock_place_no);
					EDLog(1, 1, "c_order_no =[%s]", (const char*)c_order_no);

					//if (c_vehicle_no == " ")
					//{
					//	strcpy(s.msg, _RES("请输入车号")/*请输入车号.*/);
					//	throw CApplicationException(-1, s.msg, s.svc_name);
					//}

					sqlstr = "select MAT_NO,MAT_WT from tsmpe02 A,tsmpe11 B where A.BILL_OF_LADING_NO = B.BILL_OF_LADING_NO  "
						" AND A.BILL_OF_LADING_NO =@bill_of_lading_no AND B.STACKING_NO=@c_stacking_no "
						;

					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Clear();
					cmd_inq.Parameters.Set("bill_of_lading_no", c_bill_of_lading_no);
					cmd_inq.Parameters.Set("c_stacking_no", c_stacking_no);
					cmd_inq.ExecuteReader();

					while (cmd_inq.Read())
					{
						fetchRowCount++;

						tsmpe02["MAT_NO"] = cmd_inq.GetString(1);
						tsmpe02["MAT_WT"] = cmd_inq.GetDecimal(2);

						bcls_rec->Tables["MM0099"].Rows.Clear();
						bcls_rec->Tables["MM0099"].Rows.Add(); // 创建一行
						bcls_rec->Tables["MM0099"].Rows[0]["EVENT_ID"] = "MMZ3";
						bcls_rec->Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "SM";
						bcls_rec->Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "MMSM";
						bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"] = "cm_8w7z01_rcv";
						bcls_rec->Tables["MM0099"].Rows[0]["MAT_NO"] = tsmpe02["MAT_NO"];
						bcls_rec->Tables["MM0099"].Rows[0]["MAT_WT"] = tsmpe02["MAT_WT"];
						bcls_rec->Tables["MM0099"].Rows[0]["MAT_ACT_WT"] = tsmpe02["MAT_WT"];
						bcls_rec->Tables["MM0099"].Rows[0]["MEASURE_WT_FLAG"] = "1";
						bcls_rec->Tables["MM0099"].Rows[0]["MAT_NUM"] = 1;
						doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
						if (doFlag < 0)
						{
							throw CApplicationException(-1, s.msg, s.svc_name);
						}

						Log::Trace("", "", "CCC");
					}
					cmd_inq.Close();

					if (c_stock_place_no.Trim() == "Y") //在制品转库
					{
						tsmsm09.Reset();
						tsmsm09["BILL_OF_LADING_NO"] = c_bill_of_lading_no;
						tsmsm09["BILL_OF_LADING_DETAILNO"] = c_bill_of_lading_detailno;
						tsmsm09["LOGISTICS_NO"] = c_logistics_no;
						tsmsm09.Query();
						if (tsmsm09["DELIVY_PLAN_STATUS"].ToString().Trim() == "X")
						{
							EDLog(1, 1, "计划已结案,不能再发货");
							sprintf(s.msg, "计划已结案,不能再发货.");
							throw CApplicationException(-1, s.msg, s.svc_name);
						}
						//有拼车的出厂计划，必须要一起发货
						if (c_remark_share != "")
						{
							remark_share_no = 0;
							sqlstr = sqlstr0;
							execute_sql.SetCommandText(sqlstr);
							Log::Trace("", __FUNCTION__, "**** sqlstr = [{0}]", sqlstr);
							execute_sql.Parameters.Clear();
							execute_sql.Parameters.Set("c_remark_share", c_remark_share);
							execute_sql.Parameters.Set("bill_of_lading_no", c_bill_of_lading_no);
							remark_share_no = execute_sql.ExecuteScalar().ToInt32();
							Log::Trace("", __FUNCTION__, "**** remark_share_no = [{0}]", remark_share_no);
							execute_sql.Close();

							if (remark_share_no > 0)
							{
								EDLog(1, 1, "拼车标记还有未装车的计划，不能发货");
								sprintf(s.msg, "拼车标记[" + c_remark_share + "]还有未装车的计划,不能发货.");
								throw CApplicationException(-1, s.msg, s.svc_name);
							}
						}

						tsmpe11.Reset();
						tsmpe11["BILL_OF_LADING_NO"] = c_bill_of_lading_no;
						tsmpe11["BILL_OF_LADING_DETAILNO"] = c_bill_of_lading_detailno;
						tsmpe11["LOGISTICS_NO"] = c_logistics_no;
						tsmpe11["DELIVY_TIME"] = c_act_datetime;
						tsmpe11["DELIVY_SHIFT"] = c_delivy_shift;
						tsmpe11["DELIVY_GROUP"] = c_delivy_group;
						tsmpe11["DELIVY_MAKER"] = c_user;
						tsmpe11.Update("DELIVY_TIME,DELIVY_SHIFT,DELIVY_GROUP,DELIVY_MAKER", "BILL_OF_LADING_NO,BILL_OF_LADING_DETAILNO");

						/* ***** 调用发货处理函数  ***** */
						EDLog(1, 1, "Running f_sm00_bill_deliver");
						bcls_sm00.Tables[0].Rows[0]["bill_of_lading_no"] = c_bill_of_lading_no;
						bcls_sm00.Tables[0].Rows[0]["bill_of_lading_detailno"] = c_bill_of_lading_detailno;
						bcls_sm00.Tables[0].Rows[0]["logistics_no"] = c_logistics_no;
						bcls_sm00.Tables[0].Rows[0]["stacking_no"] = tsmpe11["STACKING_NO"];

						ret = 0;
						/*ret = f_sm00_bill_deliver_z(&bcls_sm00, bcls_ret, conn);
						///ret = -1;
						if (ret < 0)
						{
							throw CApplicationException(-1, s.msg, s.svc_name);
						}*/
					}
					else
					{

						EDLog(1, 1, "datetime =[%s]", (const char*)datetime);
						EDLog(1, 1, "c_user =[%s]", (const char*)c_user);
						EDLog(1, 1, "c_act_datetime =[%s]", (const char*)c_act_datetime);
						EDLog(1, 1, "c_delivy_shift =[%s]", (const char*)c_delivy_shift);
						EDLog(1, 1, "c_delivy_group =[%s]", (const char*)c_delivy_group);
						EDLog(1, 1, "c_vehicle_no =[%s]", (const char*)c_vehicle_no);
						EDLog(1, 1, "c_bill_of_lading_no =[%s]", (const char*)c_bill_of_lading_no);
						/* ***** 修改发货材料的提单号、卡车号、  ***** */
						sqlstr =
							"  UPDATE tsmpe02 SET "
							"  delivy_time  = '" + c_act_datetime + "' , delivy_shift = '" + c_delivy_shift + "' , delivy_group  = '" + c_delivy_group + "' , "
							"  confm_status = '9'   "
							"  WHERE confm_status = '8'  and  bill_of_lading_no = '" + c_bill_of_lading_no + "' and stacking_no = '" + c_stacking_no + "' "
							;

						EDLog(1, 1, "sqlstr =[%s]", (const char*)sqlstr);
						execute_sql.SetCommandText(sqlstr);
						execute_sql.Parameters.Clear();
						//execute_sql.Parameters.Set("delivy_time1", c_act_datetime);
						//execute_sql.Parameters.Set("delivy_shift1", c_delivy_shift);
						//execute_sql.Parameters.Set("delivy_group1", c_delivy_group);
						//execute_sql.Parameters.Set("c_user1", c_user);
						//execute_sql.Parameters.Set("bill_of_lading_no", c_bill_of_lading_no);
						//execute_sql.Parameters.Set("stacking_no", c_stacking_no);
						execute_sql.ExecuteNonQuery();
						execute_sql.Close();

						EDLog(1, 1, "UPDATE TSMPE02 OK ");
						EDLog(1, 1, "c_bill_of_lading_detailno =[%s]", (const char*)c_bill_of_lading_detailno);

						tsmsm10.Reset();
						tsmsm10["BILL_OF_LADING_NO"] = c_bill_of_lading_no;
						tsmsm10.Query("BILL_OF_LADING_NO");
						Log::Trace("", __FUNCTION__, "*** tsmsm10.DELIVY_PLAN_STATUS =[{0}]", tsmsm10["DELIVY_PLAN_STATUS"].ToString());
						if (tsmsm10["DELIVY_PLAN_STATUS"].ToString().Trim() == "X" || tsmsm10["DELIVY_PLAN_STATUS"].ToString().Trim() == "F")
						{
							EDLog(1, 1, "计划已结案,不能再发货");
							sprintf(s.msg, "计划已结案,不能再发货.");
							throw CApplicationException(-1, s.msg, s.svc_name);
						}
						EDLog(1, 1, "tsmsm10 OK ");

						//有拼车的出厂计划，必须要一起发货
						if (c_remark_share != "")
						{
							EDLog(1, 1, "有拼车的出厂计划，必须要一起发货 ");
							remark_share_no = 0;
							sqlstr = sqlstr0;
							execute_sql.SetCommandText(sqlstr);
							Log::Trace("", __FUNCTION__, "**** sqlstr = [{0}]", sqlstr);
							execute_sql.Parameters.Clear();
							execute_sql.Parameters.Set("remark_share", c_remark_share);
							execute_sql.Parameters.Set("bill_of_lading_no", c_bill_of_lading_no);
							remark_share_no = execute_sql.ExecuteScalar().ToInt32();
							Log::Trace("", __FUNCTION__, "**** remark_share_no = [{0}]", remark_share_no);
							execute_sql.Close();

							if (remark_share_no > 0)
							{
								EDLog(1, 1, "拼车标记还有未装车的计划，不能发货");
								sprintf(s.msg, "拼车标记[" + c_remark_share + "]还有未装车的计划,不能发货.");
								throw CApplicationException(-1, s.msg, s.svc_name);
							}
						}

						tsmpe11.Reset();
						tsmpe11["BILL_OF_LADING_NO"] = c_bill_of_lading_no;
						tsmpe11["BILL_OF_LADING_DETAILNO"] = c_bill_of_lading_detailno;
						tsmpe11["STACKING_NO"] = c_stacking_no;
						//tsmpe11.Query("BILL_OF_LADING_NO,BILL_OF_LADING_DETAILNO,LOGISTICS_NO");
						//Log::Trace("", __FUNCTION__, "*** tsmpe11.WEIGH_APP_NO =[{0}]", tsmpe11.WEIGH_APP_NO);
						//Log::Trace("", __FUNCTION__, "*** tsmpe11.PASS_FLAG =[{0}]", tsmpe11.PASS_FLAG);
						//if (tsmpe11.WEIGH_APP_NO.Trim() != "" && tsmpe11.PASS_FLAG != "Y" )// 1:二次过磅  2:复检  0:无要求 
						//{
						//	EDLog(1, 1, "还没复检或者复检结果不通过,不能发货");
						//	sprintf(s.msg, "还没复检或者复检结果不通过不能发货.");
						//	throw CApplicationException(-1, s.msg, s.svc_name);
						//}

						tsmpe11["DELIVY_TIME"] = c_act_datetime;
						tsmpe11["DELIVY_SHIFT"] = c_delivy_shift;
						tsmpe11["DELIVY_GROUP"] = c_delivy_group;
						tsmpe11["DELIVY_MAKER"] = c_user;
						tsmpe11["STACKING_STATUS"] = "X";
						tsmpe11.Update("DELIVY_TIME,DELIVY_SHIFT,DELIVY_GROUP,DELIVY_MAKER,STACKING_STATUS", "BILL_OF_LADING_NO,BILL_OF_LADING_DETAILNO,STACKING_NO");
						EDLog(1, 1, "tsmpe11 OK ");

						/* ***** 调用发货处理函数  ***** */
						EDLog(1, 1, "Running f_sm00_bill_deliver");
						bcls_sm00.Tables[0].Rows[0]["bill_of_lading_no"] = c_bill_of_lading_no;
						bcls_sm00.Tables[0].Rows[0]["bill_of_lading_detailno"] = c_bill_of_lading_detailno;
						bcls_sm00.Tables[0].Rows[0]["logistics_no"] = c_logistics_no;
						bcls_sm00.Tables[0].Rows[0]["stacking_no"] = tsmpe11["STACKING_NO"];

						ret = 0;
						ret = f_sm00_bill_deliver(&bcls_sm00, bcls_ret, conn);
						///ret = -1;
						if (ret < 0)
						{
							throw CApplicationException(-1, s.msg, s.svc_name);
						}

					}
					//			if(c_bill_of_lading_no.Substring(0,1) == "1")
					//			{
					//			  ret = 0;
					//			  ret = f_sm00_bill_deliver(bcls_rec,bcls_ret,conn);
					//			  ///ret = -1;
					//			  if (ret < 0)
					//			  {
					//               throw CApplicationException(-1, s.msg, s.svc_name);
					//			  }
					//			}
					//			//EDLog(1,1,c_bill_of_lading_no.Substring(0,1));
					//			if(c_bill_of_lading_no.Substring(0,1) == "9")
					//			{
					//			}

				//}//end for
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
