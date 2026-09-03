/*
*  程序名称			: cm_a02102_rcv
*  程序描述			: 配车反馈
*
*  	2023-11-20 	聂媛媛			(ADD)程序建立
*			... ...
* **************************************************************************** */
/*<remark>=========================================================
<summary>
配车反馈信息接收
<para>数据库表：TWM0D(用车反馈表)         </para>
</summary>
<returns>电文处理成功与否</returns>
===========================================================</remark>*/

/* C/C++ 的标准头文件部分 */
#include "stdafx.h"
#include "epex.h"
//#include "x_psi_tel.h"
using namespace BM2;
using namespace BM2::Data;
using namespace BM2::Data::DbClient;

//// service入口
BM2F_ENTERACE_TELE(cm_a02102_rcv)
/* ***** -EP_SYSTEM_HEAD_END ***** */
int f_cm_a02102_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/*新增电文头部分*/

	int  blkNum = 0;

	CString		c_msgtype = "";
	CString		c_freeuse1 = "";
	CString		c_freeuse2 = "";
	CString		c_freeuse3 = "";
	CString		c_freeuse4 = "";
	CString		c_freeuse5 = "";
	CString		c_send_key = "";
	CString     input_t_name = "";
	CString     input_t_rout1 = "";
	CString     input_t_rout2 = "";
	CString     input_send_key = "";
	CString     output_t_name = "";
	CString     output_t_rout1 = "";
	CString     output_t_rout2 = "";
	CString     output_send_key = "";

	/*添加并设置块名*/


	
	/* ***** 静态变量定义 ***** */
	int doFlag = 0;
	int row_count = 0, i = 0, ret = 0;
	CString	record_name = "";
	CString lpsz_user_id, c_datetime = s.datetime;
	CString	lpsz_out_div;
	CString	c_plan_no = "";
	CString	c_truck_no = "";

	EIClass sm_bcls_rec;

	CModel twm0d("TWM0D");
	CModel twmsmzd02("TWMSMZD02");
	CModel twmsmzd02_1("TWMSMZD02");


	/* ***** 电文变量定义 ***** */



	/* ***** 程序变量 ***** */
	CString c_user = " ", c_tc_no = " ", c_mat_kind = " ", datetime = " ";

	/* ***** 数据库SQL操作字符串 ***** */
	CString	sqlstr(""), sqlstr_1(""), s_message("");

	/* ***** 数据库操作类定义 ***** */
	CDbCommand execute_sql(conn);
	CDbCommand cmd_sql(conn);

	/* ***** 应用程序开始处理 ***** */
	try
	{
		/* ***** 获取电文号 ***** */
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		c_tc_no = s.username;
		c_user = c_tc_no;

		/* ***** 解析电文 ***** */
		twm0d.MergeFrom(bcls_rec->Tables["a02102"].Rows[0]);
		twm0d["PRO_FLAG"] = bcls_rec->Tables["a02102"].Rows[0]["DEAL_FLAG"];
		twm0d.TrimOrBlank();

		if (twm0d["PLAN_NO"].ToString().Trim() == "")
		{
			s_message = "配车计划号：为空！";
			strcpy(s.msg, s_message);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		twm0d["REC_CREATOR"] = c_tc_no;		     /* 记录创建责任者 */
		twm0d["REC_CREATE_TIME"] = datetime;		 /* 记录创建时刻 */
		twmsmzd02["CODE_CLASS"] = "WM011";
		twmsmzd02_1["CODE_CLASS"] = "WM01";
	//*************************判断操作标记是I*******************************************************************//
		if (twm0d["PRO_FLAG"].ToString() == "I")
		{
			//将倒运计划置为可用
			sqlstr = " UPDATE TWMSM60 SET ARCHIVE_FLAG='1' WHERE PLAN_NO='" + twm0d["PLAN_NO"].ToString() + "' ";
			Log::Trace("sqlstr", __FUNCTION__, sqlstr);
			cmd_sql.SetCommandText(sqlstr);
			cmd_sql.ExecuteNonQuery();
			cmd_sql.Close();

			/*sqlstr = " DELETE FROM TWMSMZD02 WHERE CODE_CLASS='WM011' ";
			Log::Trace("sqlstr", __FUNCTION__, sqlstr);
			cmd_sql.SetCommandText(sqlstr);
			cmd_sql.ExecuteNonQuery();
			cmd_sql.Close();*/
			//**************************发货材料进行循环处理*************************************************//
			for (i = 0; i < bcls_rec->Tables["a02102_1"].Rows.get_Count(); i++)
			{
				twm0d.Reset();
				twm0d.MergeFrom(bcls_rec->Tables["a02102_1"].Rows[i]);
				twm0d["PLAN_NO"] = bcls_rec->Tables["a02102"].Rows[0]["PLAN_NO"];
				twm0d["LOADING_PLAN_NO"] = bcls_rec->Tables["a02102"].Rows[0]["PLAN_NO"];
				twm0d["VEHICLE_TYPE_NAME"] = bcls_rec->Tables["a02102_1"].Rows[i]["TRUCK_MODEL_DESC"].ToString().Trim();
				twm0d["VEHICLE_MODEL"] = bcls_rec->Tables["a02102_1"].Rows[i]["TRUCK_MODEL"].ToString().Trim();
			    twm0d["TRUCK_MODEL"] = bcls_rec->Tables["a02102_1"].Rows[i]["TRUCK_MODEL"].ToString().Trim();

				
				twm0d.Delete("PLAN_NO,TRUCK_NO,TRUCK_BOARD_NO");
				Log::Trace("", "", "TRUCK_NO={0}", (const char*)twm0d["TRUCK_NO"].ToString());
				twm0d.TrimOrBlank();
				twm0d.Insert();

				/*twmsmzd02["CODE"] = twm0d["TRUCK_NO"].ToString().Trim() == "" ? twm0d["TRUCK_BOARD_NO"].ToString() : twm0d["TRUCK_NO"].ToString();
				twmsmzd02["REC_CREATE_TIME"] = datetime;
				twmsmzd02["REC_CREATOR"] = s.userid;
				twmsmzd02.Insert();

				twmsmzd02_1["CODE"] = twm0d["TRUCK_NO"].ToString().Trim() == "" ? twm0d["TRUCK_BOARD_NO"].ToString() : twm0d["TRUCK_NO"].ToString();
				if (!twmsmzd02_1.Query("CODE_CLASS,CODE"))
				{
					twmsmzd02_1["CODE_DESC_1_CONTENT"] = twmsmzd02_1["CODE"];
					twmsmzd02_1["REC_CREATE_TIME"] = datetime;
					twmsmzd02_1["REC_CREATOR"] = s.userid;
					twmsmzd02_1.Insert();
				}*/

			}

		}
		//*************************判断操作标记是D*******************************************************************//
		if (twm0d["PRO_FLAG"].ToString() == "D")		{
			//将倒运计划置为不可用
			sqlstr = " UPDATE TWMSM60 SET ARCHIVE_FLAG=' ' WHERE PLAN_NO='" + twm0d["PLAN_NO"].ToString() + "' ";
			Log::Trace("sqlstr", __FUNCTION__, sqlstr);
			cmd_sql.SetCommandText(sqlstr);
			cmd_sql.ExecuteNonQuery();
			cmd_sql.Close();
			//*********************************************************************************************//
			for (i = 0; i < bcls_rec->Tables["a02102_1"].Rows.get_Count(); i++)
			{
				twm0d.Reset();
				twm0d.MergeFrom(bcls_rec->Tables["a02102_1"].Rows[i]);
				twm0d["PLAN_NO"] = bcls_rec->Tables["a02102"].Rows[0]["PLAN_NO"];				
				twm0d["VEHICLE_TYPE_NAME"] = bcls_rec->Tables["a02102_1"].Rows[i]["TRUCK_MODEL_DESC"].ToString().Trim();
				twm0d["VEHICLE_MODEL"] = bcls_rec->Tables["a02102_1"].Rows[i]["TRUCK_MODEL"].ToString().Trim();

				
				twm0d.Delete("PLAN_NO,TRUCK_NO,TRUCK_BOARD_NO");
			}

		}

		//*************************判断操作标记是C :计划关闭*******************************************************************//
		if (twm0d["PRO_FLAG"].ToString() == "C")
		{
			//将倒运计划置为不可用
			sqlstr = " UPDATE TWMSM60 SET ARCHIVE_FLAG=' ' WHERE PLAN_NO='" + twm0d["PLAN_NO"].ToString() + "' ";
			Log::Trace("sqlstr", __FUNCTION__, sqlstr);
			cmd_sql.SetCommandText(sqlstr);
			cmd_sql.ExecuteNonQuery();
			cmd_sql.Close();
			for (i = 0; i < bcls_rec->Tables["a02102_1"].Rows.get_Count(); i++)
			{
				twm0d.Reset();
				twm0d.MergeFrom(bcls_rec->Tables["a02102_1"].Rows[i]);				
				twm0d["PLAN_NO"] = bcls_rec->Tables["a02102"].Rows[0]["PLAN_NO"].ToString().Trim();
				twm0d["VEHICLE_TYPE_NAME"] = bcls_rec->Tables["a02102_1"].Rows[i]["TRUCK_MODEL_DESC"].ToString().Trim();
				twm0d["VEHICLE_MODEL"] = bcls_rec->Tables["a02102_1"].Rows[i]["TRUCK_MODEL"].ToString().Trim();

				sqlstr = CString("update twm0d set "
					" PRO_FLAG = 'C' "
					" where plan_no_tw =@plan_no_tw   and truck_no = @truck_no AND TRUCK_BOARD_NO =@TRUCK_BOARD_NO ");

				Log::Trace("sqlstr", __FUNCTION__, sqlstr);
				cmd_sql.SetCommandText(sqlstr);
				cmd_sql.Parameters.Set("plan_no_tw", twm0d["PLAN_NO"].ToString());
				cmd_sql.Parameters.Set("truck_no", twm0d["TRUCK_NO"].ToString());
				cmd_sql.Parameters.Set("TRUCK_BOARD_NO", twm0d["TRUCK_BOARD_NO"].ToString());
				cmd_sql.ExecuteNonQuery();
				cmd_sql.Close();

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
		Log::Debug("", __FUNCTION__, "error=[{0}]", s.msg);
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