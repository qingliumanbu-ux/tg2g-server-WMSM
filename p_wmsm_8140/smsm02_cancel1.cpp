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
int f_sm00_bill_transfer(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);
int f_smsm02_cancel(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);

BM2F_ENTERACE(smsm02_cancel1)
/* ***** -EP_SYSTEM_HEAD_END ***** */
int f_smsm02_cancel1(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 静态变量定义 ***** */
	int doFlag = 0;
	int fetchRowCount = 0;
	int row_count = 0, ret = 0;
	int stacking_y = 0, stacking_n = 0;

	CModel tsmpe11 = CModel("TSMPE11");
	CModel tsmsm09 = CModel("TSMSM09");
	CModel tsmsm10 = CModel("TSMSM10");
	CModel tsmsm14 = CModel("TSMSM14");

	/* ***** 程序变量 ***** */
	CString c_user = s.userid, c_mat_kind = " ", datetime = " ";
	//CString c_stock_no=" ",c_stock_no_end=" ",c_query_type=" ";

	CString c_del_cause = " ", c_del_type = " ", c_dept_code = " ", c_dept_code_cname = " ", c_red_cause_code = " ", c_red_cause_desc = " ", c_out_mark = " ";
	CString c_confm_plan_no = " ", c_ready_bill_no = " ", c_confm_status = " ", c_mat_no = " ";
	CString c_car_name = " ", c_fin_pos = " ", c_stock_place_no_to = " ", c_bill_of_lading_detailno = " ", c_stacking_no = " ";
	CString c_factory_div = " ", c_bill_of_lading_no = " ", c_vehicle_no = " ", c_stock_no = " ", c_proc_type = " ", c_act_datetime = " ";
	CString c_stock_place_no = " "; //在制品转库标记


	char  c_delivy_shift[5] = " ", c_delivy_group[10] = " ";
	char  c_datetime[15];

	/* ***** 数据库SQL操作字符串 ***** */
	CString	sqlstr(""), sqlstr0(""), sqlstr1(""), sqlstr2(""), sqlstr3(""), sqlstr4(""), sqlstr5(""), sqlstr6("");

	/* ***** 数据库操作类定义 ***** */
	CDbCommand execute_sql(conn);

	/* *******定义一个EIClass object */
	EIClass ds_ready_bill_no, ds_mat_no;

	ds_mat_no.Tables[0].Columns.Add(DT_STRING, "mat_no");                   // 材料号
	ds_mat_no.Tables[0].Columns.Add(DT_STRING, "crane_inst_type");          // 吊车命令类别
	ds_mat_no.Tables[0].Columns.Add(DT_STRING, "stock_place_no_to");        // 目标垛位号
	ds_mat_no.Tables[0].Columns.Add(DT_STRING, "fin_pos");                  // 最终位置
	ds_mat_no.Tables[0].Columns.Add(DT_STRING, "car_name");                 // 卡车名

	/* ***** 应用程序开始处理 ***** */
	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		//	  c_factory_div        = bcls_rec->Tables[0].Rows[0]["factory_div"].ToString().TrimOrBlank();
		//    c_bill_of_lading_no  = bcls_rec->Tables[0].Rows[0]["bill_of_lading_no"].ToString().TrimOrBlank();
		//c_vehicle_no = bcls_rec->Tables[0].Rows[0]["vehicle_no"].ToString().TrimOrBlank();
		//c_stock_no = bcls_rec->Tables[0].Rows[0]["stock_no"].ToString().TrimOrBlank();
		//c_proc_type = bcls_rec->Tables[0].Rows[0]["proc_type"].ToString().TrimOrBlank();
		//c_act_datetime = bcls_rec->Tables[0].Rows[0]["act_datetime"].ToString().TrimOrBlank();

		//EDLog(1, 1, "c_vehicle_no =[%s]", (const char*)c_vehicle_no);
		//EDLog(1, 1, "c_proc_type =[%s]", (const char*)c_proc_type);
		//EDLog(1, 1, "c_act_datetime =[%s]", (const char*)c_act_datetime);
		//EDLog(1, 1, "c_stock_no =[%s]", (const char*)c_stock_no);

		//if (c_vehicle_no == " ")
		//{
		//	strcpy(s.msg, _RES("SM00C0000022")/*请输入车号.*/);
		//	throw CApplicationException(-1, s.msg, s.svc_name);
		//}

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
			//准发计划状态 confm_status:  6.材料待出库(出厂计划已接收) 8.发货开始  9.发货完成
			/*sqlstr1 = CString(
			" SELECT count(1) from tsmpe02 t where bill_of_lading_no = @bill_of_lading_no and confm_status = '6' "
			);*/
			sqlstr1 = CString(
				" SELECT count(1) from tsmpe02 t where stacking_no = @stacking_no and bill_of_lading_no = @bill_of_lading_no and confm_status = '8' "
				);

			sqlstr2 = CString(
				"		UPDATE tsmpe02 SET                          "
				"  rec_revise_time = @datetime, "
				"  rec_revisor     = @c_user,       "
				"  delivy_time     = ' ', "
				"  delivy_shift    = ' ', "
				"  delivy_group    = ' ', "
				"  delivy_maker    = ' ',    "
				"  vehicle_no      = ' ',    "
				//"  bill_of_lading_no      = ' ',    "
				//"  bill_of_lading_detailno      = ' ',    "
				"  confm_status    = '6',     "
				"  stacking_no     = ' '     "   //装车取消，清空码单号
				"  WHERE  bill_of_lading_no = @bill_of_lading_no and stacking_no = @stacking_no and confm_status = '8' "
				);

			/*sqlstr3 = CString(
			" SELECT count(1) from tsmpe02 t WHERE mat_no = @mat_no and confm_status = '6' "
			);*/
			sqlstr3 = CString(
				" SELECT count(1) from tsmpe02 t WHERE mat_no = @mat_no and confm_status = '8' "
				);

			sqlstr4 = CString(
				"		UPDATE tsmpe02 SET                          "
				"  rec_revise_time = @datetime, "
				"  rec_revisor     = @c_user,       "
				"  delivy_time     = ' ', "
				"  delivy_shift    = ' ', "
				"  delivy_group    = ' ', "
				"  delivy_maker    = ' ',    "
				"  vehicle_no      = ' ',    "
				//"  bill_of_lading_no      = ' ',    "
				//"  bill_of_lading_detailno      = ' ',    "
				"  c/nfm_status    = '6'     "
				"  WHERE mat_no = @mat_no and confm_status = '8'"
				);

			sqlstr5 = CString(
				" SELECT count(1) from tsmpe02 where bill_of_lading_no = @bill_of_lading_no and out_mark <> '2' "
				);

			sqlstr6 = CString(
				" select stacking_wt,vehicle_no from tsmpe11 where bill_of_lading_no = @bill_of_lading_no and bill_of_lading_detailno = @bill_of_lading_detailno  and stacking_status != 'X' "
				);

			break;
		}

		/* ***** 按提单输入实绩   ***** */

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			Log::Debug("", __FUNCTION__, "for i =[{0}]", i);
			stacking_y = 0;
			stacking_n = 0;
			c_bill_of_lading_no = bcls_rec->Tables[0].Rows[i]["bill_of_lading_no"].ToString().TrimOrBlank();
			c_bill_of_lading_detailno = bcls_rec->Tables[0].Rows[i]["bill_of_lading_detailno"].ToString().TrimOrBlank();
			c_stacking_no = bcls_rec->Tables[0].Rows[i]["stacking_no"].ToString().TrimOrBlank();
			if (bcls_rec->Tables[0].Columns.Contains("stock_place_no") == true)//在制品转库标记
			{
				c_stock_place_no = bcls_rec->Tables[0].Rows[0]["stock_place_no"].ToString().TrimOrBlank();
			}

			Log::Info("", __FUNCTION__, "c_bill_of_lading_no=[{0}]", c_bill_of_lading_no);
			Log::Info("", __FUNCTION__, "c_bill_of_lading_detailno=[{0}]", c_bill_of_lading_detailno);
			Log::Info("", __FUNCTION__, "c_stacking_no=[{0}]", c_stacking_no);
			Log::Info("", __FUNCTION__, "c_stock_place_no=[{0}]", c_stock_place_no);

			if (c_bill_of_lading_no == " ")
			{
				break;
			}

			//删tsmpe11表 发货码单表记录
			tsmpe11.Reset();
			tsmpe11["BILL_OF_LADING_NO"] = c_bill_of_lading_no;
			tsmpe11["BILL_OF_LADING_DETAILNO"] = c_bill_of_lading_detailno;
			tsmpe11["STACKING_NO"] = c_stacking_no;
			tsmpe11.Query("STACKING_NO");
			if (tsmpe11["STACKING_STATUS"].ToString().Trim() == "X")
			{
				sprintf(s.msg, "此码单已结案，不能装车取消：" + c_stacking_no);
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (tsmpe11["STACKING_STATUS"].ToString().Trim() != "X")
			{
				Log::Debug("", __FUNCTION__, "有可装车取消的码单");

				Log::Debug("", __FUNCTION__, "STACKING_WT=[{0}]", tsmpe11["STACKING_WT"].ToString());
				Log::Debug("", __FUNCTION__, "VEHICLE_NO=[{0}]", tsmpe11["VEHICLE_NO"].ToString());

				//更新承运商实际承运重量
				tsmsm14["BILL_OF_LADING_NO"] = c_bill_of_lading_detailno;
				tsmsm14["VEHICLE_NO"] = tsmpe11["VEHICLE_NO"];
				tsmsm14.Query("BILL_OF_LADING_NO,VEHICLE_NO");
				Log::Debug("", __FUNCTION__, "tsmsm14.ACT_LOAD_WT=[{0}]", tsmsm14["ACT_LOAD_WT"].ToString());
				Log::Debug("", __FUNCTION__, "tsmsm14.CARRY_USER_CODE=[{0}]", tsmsm14["CARRY_USER_CODE"].ToString());
				tsmsm14["ACT_LOAD_WT"] = tsmsm14["ACT_LOAD_WT"].ToDecimal() - tsmpe11["STACKING_WT"].ToDecimal();
				tsmsm14["REC_REVISE_TIME"] = datetime;
				tsmsm14["REC_REVISOR"] = s.userid;
				tsmsm14.Update("REC_REVISE_TIME,REC_REVISOR,ACT_LOAD_WT", "BILL_OF_LADING_NO,CARRY_USER_CODE");

				tsmpe11.Delete("BILL_OF_LADING_DETAILNO,BILL_OF_LADING_NO,STACKING_NO");
			}

			if (tsmpe11.QueryCount("BILL_OF_LADING_NO,BILL_OF_LADING_DETAILNO") == 0)
			{
				Log::Debug("", __FUNCTION__, "更新发货表状态");
				tsmsm09.Reset();
				tsmsm09["BILL_OF_LADING_NO"] = c_bill_of_lading_no;
				tsmsm09["BILL_OF_LADING_DETAILNO"] = c_bill_of_lading_detailno;
				tsmsm09["LOGISTICS_NO"] = c_stacking_no;
				tsmsm09["DELIVY_PLAN_STATUS"] = "1";         //计划释放
				tsmsm09["OUT_STOCK_TIME"] = " ";  //装车日期
				tsmsm09["MEASURE_WT_FLAG"] = " ";//现在成品库发货计量委托是人工决定是否要计量申请, 装车取消后，情况计量委托标记
				tsmsm09["PRINT_FLAG"] = " ";
				tsmsm09["VEHICLE_NO"] = " ";
				tsmsm09.Update("DELIVY_PLAN_STATUS,OUT_STOCK_TIME,MEASURE_WT_FLAG,PRINT_FLAG,VEHICLE_NO", "BILL_OF_LADING_DETAILNO,BILL_OF_LADING_NO");

				tsmsm10.Reset();
				tsmsm10["BILL_OF_LADING_NO"] = c_bill_of_lading_no;
				tsmsm10["BILL_OF_LADING_DETAILNO"] = c_bill_of_lading_detailno;
				tsmsm10["LOGISTICS_NO"] = c_stacking_no;
				tsmsm10["DELIVY_PLAN_STATUS"] = "1";
				tsmsm10["OUT_STOCK_TIME"] = " ";  //装车日期
				tsmsm10["MEASURE_WT_FLAG"] = " ";//现在成品库发货计量委托是人工决定是否要计量申请, 装车取消后，情况计量委托标记
				tsmsm10["VEHICLE_NO"] = " ";
				tsmsm10.Update("DELIVY_PLAN_STATUS,OUT_STOCK_TIME,MEASURE_WT_FLAG,VEHICLE_NO", "BILL_OF_LADING_DETAILNO,BILL_OF_LADING_NO");
			}

			if (c_stock_place_no.Trim() == "N")
			{
				/* ***** 检查该提单下的材料是否合法  ***** */
				sqlstr = sqlstr1;
				execute_sql.SetCommandText(sqlstr);
				execute_sql.Parameters.Clear();
				execute_sql.Parameters.Set("bill_of_lading_detailno", c_bill_of_lading_detailno);
				execute_sql.Parameters.Set("bill_of_lading_no", c_bill_of_lading_no);
				execute_sql.Parameters.Set("stacking_no", c_stacking_no);
				row_count = execute_sql.ExecuteScalar().ToInt32();
				execute_sql.Close();

				if (0 == row_count)
				{
					EDLog(1, 1, "没有可操作的材料");
					sprintf(s.msg, "物流计划[" + c_bill_of_lading_no + "]没有可操作的材料.");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				/* ***** 修改发货材料的提单号、卡车号、  ***** */
				sqlstr = sqlstr2;
				execute_sql.SetCommandText(sqlstr);
				execute_sql.Parameters.Set("datetime", datetime);
				execute_sql.Parameters.Set("c_user", c_user);
				execute_sql.Parameters.Set("delivy_time", c_act_datetime);
				execute_sql.Parameters.Set("delivy_shift", c_delivy_shift);
				execute_sql.Parameters.Set("delivy_group", c_delivy_group);
				execute_sql.Parameters.Set("vehicle_no", c_vehicle_no);
				execute_sql.Parameters.Set("bill_of_lading_detailno", c_bill_of_lading_detailno);
				execute_sql.Parameters.Set("bill_of_lading_no", c_bill_of_lading_no);
				execute_sql.Parameters.Set("stacking_no", c_stacking_no);
				execute_sql.ExecuteNonQuery();
				execute_sql.Close();

				/* ***** 调用发货处理函数  ***** */
				/*EDLog(1, 1, "wuxin01");
				EDLog(1, 1, "Running f_sm00_bill_deliver");
				ret = 0;
				ret = f_sm00_bill_deliver(bcls_rec, bcls_ret, conn);
				///ret = -1;
				if (ret < 0)
				{
				throw CApplicationException(-1, s.msg, s.svc_name);
				}*/

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

			}//end 在制品转库标记

		}//end fro

		/* ***** 按材料输入实绩   ***** */
		/*if (0 != c_proc_type.Compare("M") && 0 != c_proc_type.Compare("B"))
		{
		throw CApplicationException(-1, s.msg, s.svc_name);
		}*/


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
