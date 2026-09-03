/* ****************************************************************************
*	Copyright (c) Baosight Corporation 2008 . All Rights Reserved.
*  	BM2PES 宝信生产执行系统
*****************************************************************************
*  程序名称			: smhr02_inq_m
*  程序描述			: 厚板出厂准发材料查询
*  备注说明			:
*  修改历史			:
*  		wuxin			(ADD)程序建立
*			... ...
* **************************************************************************** */
/***** C/C++ 的标准头文件部分 *****/
#include "stdafx.h"
using namespace BM2;
using namespace BM2::Data;
using namespace BM2::Data::DbClient;

int f_smsm02_inq_m(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);

BM2F_ENTERACE(smsm02_inq_m1)
/* ***** -EP_SYSTEM_HEAD_END ***** */
int f_smsm02_inq_m1(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 静态变量定义 ***** */
	int doFlag = 0;
	int fetchRowCount = 0;
	int row_count = 0;

	CModel tsmpe02 = CModel("TSMPE02");

	/* ***** 程序变量 ***** */
	CString c_user = s.userid, c_mat_kind = " ", datetime = " ";
	//CString c_stock_no=" ",c_stock_no_end=" ",c_query_type=" ";

	CString c_stock_no = " ";
	CString c_ready_bill_no = " ";
	CString c_bill_of_lading_no = " ";
	CString c_order_no_from = " ";
	CString c_order_no_to = " ";
	CString c_mat_no_from = " ";
	CString c_mat_no_to = " ";
	CString c_factory_div = " ";
	CString c_code = " ";
	CString c_order_no = " ";
	CString c_stock_place_no = ""; //在制品转库标志
	CString c_delivy_plan_type = " "; //计划类型 0：出厂发货 按合同发  1：2：转库发货 按材料发
	/* ***** 数据库SQL操作字符串 ***** */
	CString	sqlstr(""), sqlstr1(""), sqlstr2(""), sqlstr3(""), sqlstr4(""), sqlstr0("");
	CString sqlstr5("");

	/* ***** 数据库操作类定义 ***** */
	CDbCommand execute_sql(conn);

	/* ***** 应用程序开始处理 ***** */
	try
	{
		/* ***** 获取前台参数  ***** */

		if (bcls_rec->Tables[0].Columns.Contains("stock_place_no") == true)
			c_factory_div = bcls_rec->Tables[0].Rows[0]["factory_div"].ToString().TrimOrBlank();
		if (bcls_rec->Tables[0].Columns.Contains("stock_place_no") == true)
			c_bill_of_lading_no = bcls_rec->Tables[0].Rows[0]["bill_of_lading_no"].ToString().TrimOrBlank();
		if (bcls_rec->Tables[0].Columns.Contains("stock_place_no") == true)
			c_order_no = bcls_rec->Tables[0].Rows[0]["order_no"].ToString().TrimOrBlank();
		if (bcls_rec->Tables[0].Columns.Contains("stock_place_no") == true)//在制品转库标记
		{
			c_stock_place_no = bcls_rec->Tables[0].Rows[0]["stock_place_no"].ToString().TrimOrBlank();
		}
		if (c_stock_place_no.Trim() == ""){
			c_stock_place_no = "N";
		}
		if (bcls_rec->Tables[0].Columns.Contains("delivy_plan_type") == true)//计划类型 0：出厂发货 按合同发  1：2：转库发货 按材料发
		{
			c_delivy_plan_type = bcls_rec->Tables[0].Rows[0]["delivy_plan_type"].ToString().TrimOrBlank();
		}

		if (c_bill_of_lading_no == ""){
			sprintf(s.msg, "请选择对应的计划号！");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (c_order_no == ""){
			sprintf(s.msg, "请选择对应的合同！");
			throw CApplicationException(-1, s.msg, log.Location);
		}


		Log::Trace("", __FUNCTION__, "传入参数c_factory_div = [{0}]", c_factory_div);
		Log::Trace("", __FUNCTION__, "传入参数c_bill_of_lading_no = [{0}]", c_bill_of_lading_no);
		Log::Trace("", __FUNCTION__, "传入参数c_order_no = [{0}]", c_order_no);
		Log::Trace("", __FUNCTION__, "传入参数c_stock_place_no = [{0}]", c_stock_place_no);
		Log::Trace("", __FUNCTION__, "传入参数c_delivy_plan_type = [{0}]", c_delivy_plan_type);
		/* ***** 获取库区号  ***** */
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			/*if (c_delivy_plan_type.Trim() == "0")
			{
				sqlstr0 = CString(
					" SELECT * FROM tsmpe02 where order_no = @order_no  and  stacking_no = ' ' and confm_status in('4','6')  ");
			}
			else
			{
				sqlstr0 = CString(
					" SELECT * FROM tsmpe02 where bill_of_lading_no = @bill_of_lading_no ");
			}*/

			sqlstr1 = CString(
				" select a.*,b.*                      "
				"	 from tsmpe02 a,tmmsm01 b           "
				"	 where a.mat_no  = b.mat_no         "
				"    and a.order_no = @order_no       "
				"    and a.stacking_no = ' '       "
				//"	   and a.bill_of_lading_no = @bill_of_lading_no "
				"	   and a.confm_status in('4','6')         "//2.接收成功 4.已确认 6.出厂计划已接收 8.开始发货 9.发货完成
				);

			sqlstr2 = CString(
				" select a.*,b.*                      "
				"	 from tsmpe02 a,tmmsm01 b           "
				"	 where a.mat_no  = b.mat_no         "
				//"	   and a.bill_of_lading_no = @bill_of_lading_no "
				"    and a.order_no = @order_no       "
				"    and a.stacking_no = ' '       "
				"	   and a.confm_status in('4','6')         "
				);
			sqlstr3 = CString(
				"select code_desc_1_content from tep0002 where code_class = 'SM28' and code = @code"
				);

			sqlstr4 = CString(
				"select * from tmmsm01 where plan_no = @logistics_no"
				);
			sqlstr5 = "select DISTINCT DELIVY_PLAN_TYPE,"
				"(select sum(stacking_wt) from tsmpe11 where stacking_status = '4' and bill_of_lading_no = a.bill_of_lading_no and order_no = a.order_no) TRUCK_WT"
				" from tsmsm10 a     where 1 = 1     AND DELIVY_PLAN_STATUS <= '4'   and stock_place_no != 'Y' and bill_of_lading_no = @bill_of_lading_no "
				//"order by a.PRG_SEND_TIME desc"
				;
			break;
		}

		//获取对应计划类型
		if (c_bill_of_lading_no.Trim() = ""){
			execute_sql.SetCommandText(sqlstr5);
			execute_sql.Parameters.Set("bill_of_lading_no", c_bill_of_lading_no);
			execute_sql.ExecuteReader();
			if (execute_sql.Read()){
				c_delivy_plan_type = execute_sql.GetString(1);

			}
			execute_sql.Close();
			Log::Trace("", __FUNCTION__, "传入参数c_order_no = [{0}]", c_order_no);
			Log::Trace("", __FUNCTION__, "传入参数c_delivy_plan_type = [{0}]", c_delivy_plan_type);

			if (c_delivy_plan_type.Trim() == "0")
			{
				sqlstr0 = CString(
					" SELECT * FROM tsmpe02 where order_no = @order_no  and  stacking_no = ' ' and confm_status in('4','6')  ");
			}
			else
			{
				sqlstr0 = CString(
					" SELECT * FROM tsmpe02 where bill_of_lading_no = @bill_of_lading_no ");
			}
		}


		if (c_stock_place_no == "Y")//在制品转库
		{
			Log::Trace("", __FUNCTION__, "sqlstr4 = [{0}]", sqlstr4);
			execute_sql.SetCommandText(sqlstr4);
			execute_sql.Parameters.Set("bill_of_lading_no", c_bill_of_lading_no);

			execute_sql.ExecuteQuery(bcls_ret->Tables[0]);
			execute_sql.Close();
		}
		else//成品发货
		{
			/* ***** 执行SQL   ***** */
			Log::Trace("", __FUNCTION__, "sqlstr0 = [{0}]", sqlstr0);
			sqlstr = sqlstr0;
			execute_sql.SetCommandText(sqlstr);
			execute_sql.Parameters.Set("bill_of_lading_no", c_bill_of_lading_no);
			execute_sql.Parameters.Set("order_no", c_order_no);
			execute_sql.ExecuteReader();
			while (execute_sql.Read())
			{
				execute_sql.Fetch(tsmpe02);
			}
			execute_sql.Close();

			if (0 == tsmpe02["MAT_KIND"].ToString().Compare("SM"))
			{
				sqlstr = sqlstr1;
			}

			if (0 == tsmpe02["MAT_KIND"].ToString().Compare("SM"))
			{
				sqlstr = sqlstr2;
			}

			Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			execute_sql.SetCommandText(sqlstr);
			execute_sql.Parameters.Set("bill_of_lading_no", c_bill_of_lading_no);
			execute_sql.Parameters.Set("order_no", c_order_no);

			//		 execute_sql.ExecuteReader();
			//	     while(execute_sql.Read())
			//	         {
			//              execute_sql.Fetch(tsmpe02);
			//		      tsmpe02.MergeTo(bcls_ret->Tables[0], false);
			//	         }
			//	      execute_sql.Close();

			execute_sql.ExecuteQuery(bcls_ret->Tables[0]);
			execute_sql.Close();

			bcls_ret->Tables[0].Columns.Add(DT_STRING, "CONFM_STATUS2");
			for (int i = 0; i < bcls_ret->Tables[0].Rows.get_Count(); i++)
			{
				c_code = bcls_ret->Tables[0].Rows[i]["confm_status"].ToString();
				sqlstr = sqlstr3;
				execute_sql.Parameters.Clear();
				execute_sql.SetCommandText(sqlstr);
				execute_sql.Parameters.Set("code", c_code);
				EDLog(1, 1, c_code);
				execute_sql.ExecuteReader();

				while (execute_sql.Read())
				{
					bcls_ret->Tables[0].Rows[i]["CONFM_STATUS2"] = execute_sql.GetString(1);
				}

				execute_sql.Close();
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
