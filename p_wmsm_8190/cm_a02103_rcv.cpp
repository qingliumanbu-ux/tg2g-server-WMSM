/*
*  程序名称			: cm_a02103_rcv
*  程序描述			: 不用
*
*  	2023-11-20 	李振			(ADD)程序建立
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
BM2F_ENTERACE_TELE(cm_a02103_rcv)
/* ***** -EP_SYSTEM_HEAD_END ***** */
int f_cm_a02103_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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