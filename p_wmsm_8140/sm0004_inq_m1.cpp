/* ****************************************************************************
*	Copyright (c) Baosight Corporation 2008 . All Rights Reserved.
*  	BM2PES 宝信生产执行系统
*****************************************************************************
*  程序名称			: sm0004_inq_m
*  程序描述			: 发货材料查询
*  备注说明			:
*  修改历史			:
*  		2011-12-27 	wuxin			(ADD)程序建立
*			... ...
* **************************************************************************** */
/***** C/C++ 的标准头文件部分 *****/
/***** C/C++ 的标准头文件部分 *****/
#include "stdafx.h"
using namespace BM2;
using namespace BM2::Data;
using namespace BM2::Data::DbClient;


int f_sm0004_inq_m(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);

/*<remark>=========================================================
/// <summary>
/// 发货材料查询
/// <para>
/// 1.根据传入的码单号，查询材料表信息。
/// </para>
/// <para>数据库表：TSMPE12(码单材料表)         </para>
/// <para>主调用函数：前台SM0004画面（点击）调用。   </para>
/// </summary>
/// <param name="c_stacking_no">码单号    </param>
/// <returns>对应的材料表信息</returns>
===========================================================</remark>*/
// service入口

BM2F_ENTERACE(sm0004_inq_m1)
/* ***** -EP_SYSTEM_HEAD_END ***** */
int f_sm0004_inq_m1(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 静态变量定义 ***** */
	int doFlag = 0;
	int fetchRowCount = 0;
	int row_count = 0;
	int hsmpe02_num = 0;

	/* ***** 程序变量 ***** */
	CString c_user = s.userid, c_mat_kind = " ", datetime = " ";
	CString c_stacking_no = " ";
	CString bill_of_lading_detailno = " ";
	CString stacking_no = " ";
	CString bill_of_lading_no = " ";
	CString delivy_plan_status = " ";
	CString c_stock_place_no = "";
	CString c_truck_no = "";

	/* ***** 数据库SQL操作字符串 ***** */
	CString	sqlstr(""), sqlstr1(""), sqlstr2(""), sqlstr3("");
	CString	sqlwhere("");

	CModel hsmpe02 = CModel("HSMPE02");

	/* ***** 数据库操作类定义 ***** */
	CDbCommand execute_sql(conn);

	/* ***** 应用程序开始处理 ***** */
	try
	{
		/* ***** 获取前台参数  ***** */
		//c_stacking_no = bcls_rec->Tables[0].Rows[0]["stacking_no"].ToString().TrimOrBlank();
		if (bcls_rec->Tables[0].Columns.Contains("bill_of_lading_detailno") == true)
			bill_of_lading_detailno = bcls_rec->Tables[0].Rows[0]["bill_of_lading_detailno"].ToString().TrimOrBlank();
		if (bcls_rec->Tables[0].Columns.Contains("stacking_no") == true)
			stacking_no = bcls_rec->Tables[0].Rows[0]["stacking_no"].ToString().TrimOrBlank();
		if (bcls_rec->Tables[0].Columns.Contains("bill_of_lading_no") == true)
			bill_of_lading_no = bcls_rec->Tables[0].Rows[0]["bill_of_lading_no"].ToString().TrimOrBlank();
		if (bcls_rec->Tables[0].Columns.Contains("delivy_plan_status") == true)
			delivy_plan_status = bcls_rec->Tables[0].Rows[0]["delivy_plan_status"].ToString().TrimOrBlank();
		if (bcls_rec->Tables[0].Columns.Contains("stock_place_no") == true)//在制品转库标记
		{
			c_stock_place_no = bcls_rec->Tables[0].Rows[0]["stock_place_no"].ToString().TrimOrBlank();
		}
		if (bcls_rec->Tables[0].Columns.Contains("Truck_no") == true)//车号
		{
			c_truck_no = bcls_rec->Tables[0].Rows[0]["Truck_no"].ToString().TrimOrBlank();
		}
		Log::Info("", __FUNCTION__, "bill_of_lading_detailno=[{0}]", bill_of_lading_detailno);
		Log::Info("", __FUNCTION__, "stacking_no=[{0}]", stacking_no);
		Log::Info("", __FUNCTION__, "bill_of_lading_no=[{0}]", bill_of_lading_no);
		Log::Info("", __FUNCTION__, "delivy_plan_status=[{0}]", delivy_plan_status);
		Log::Trace("", __FUNCTION__, "c_stock_place_no = [{0}]", c_stock_place_no);
		Log::Trace("", __FUNCTION__, "c_truck_no = [{0}]", c_truck_no);

		if (delivy_plan_status == "X")
		{
			hsmpe02.Reset();
			hsmpe02["BILL_OF_LADING_NO"] = bill_of_lading_no;
			hsmpe02_num = hsmpe02.QueryCount("BILL_OF_LADING_NO");
		}

		/* ***** 获取库区号  ***** */
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			//sqlstr1 = CString(
			//	//" select * from tsmpe12 where bill_of_lading_detailno = @bill_of_lading_detailno "
			//	"select (select stock_place_no from tmmsm01 where mat_no = t.mat_no) stock_place_no, "
			//	" t.* from tsmpe02 t where stacking_no = @stacking_no and bill_of_lading_no = @bill_of_lading_no "
			//	"union all "
			//	"select (select stock_place_no from hmmsm01 where mat_no = t.mat_no) stock_place_no, "
			//	" t.* from hsmpe02 t where stacking_no = @stacking_no and bill_of_lading_no = @bill_of_lading_no "
			//	);
			//手持机直接查当前档材料即可
			sqlstr1 = CString(
				//" select * from tsmpe12 where bill_of_lading_detailno = @bill_of_lading_detailno "
				"select (select stock_place_no from tmmsm01 where mat_no = t.mat_no) stock_place_no, "
				" t.* from tsmpe02 t where 1 =1 "
				);
			if (delivy_plan_status == "X")
			{
				if (hsmpe02_num > 0)
					sqlstr1 = CString("select (select stock_place_no from hmmsm01 where mat_no = t.mat_no fetch first 1 rows only) stock_place_no, t.* from hsmpe02 t where stacking_no = @stacking_no and bill_of_lading_no = @bill_of_lading_no ");
				else
					sqlstr1 = CString("select (select stock_place_no from tmmsm01 where mat_no = t.mat_no) stock_place_no, 'HP' mat_kind, t.* from tsmpea1 t where event_id = '5' and bill_of_lading_no = @bill_of_lading_no "); //event_id = '5' 出厂确认

			}


			if (c_stock_place_no == "Y")
			{
				if (delivy_plan_status == "X")
					sqlstr1 = CString("select t.* from hmmsm01 t where plan_no = @bill_of_lading_no ");
				else
					sqlstr1 = CString("select t.* from tmmsm01 t where plan_no = @bill_of_lading_no ");
			}

			break;
		}
		if (stacking_no != ""){
			sqlwhere += " and stacking_no = @stacking_no ";
		}
		if (bill_of_lading_no != ""){
			sqlwhere += " and bill_of_lading_no = @bill_of_lading_no ";
		}
		if (c_truck_no != ""){
			sqlwhere += " and vehicle_no = @truck_no ";
		}

		/* ***** 执行SQL   ***** */
		sqlstr = sqlstr1;
		execute_sql.SetCommandText(sqlstr);
		execute_sql.Parameters.Set("bill_of_lading_no", bill_of_lading_no);
		execute_sql.Parameters.Set("stacking_no", stacking_no);
		execute_sql.Parameters.Set("truck_no", c_truck_no);

		Log::Trace("", __FUNCTION__, "sqlstr=[{0}]", sqlstr);
		execute_sql.ExecuteQuery(bcls_ret->Tables[0]);

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
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1); //返回前台，与EI.EIManager.Instance.CallService(this.ef_args.formPartition,)方法返回的EI.EIInfo对象的sys_info.msg参数对应 
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
