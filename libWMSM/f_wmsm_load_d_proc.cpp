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

#include "math.h"
BM2_FUNCTION_IMPORT


BM2_FUNCTION_EXPORT
int f_wmsm_load_d_proc(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/*程序用变量*/
	int doFlag = 0;
	int ret = 0;
	int sqlid = 0;
	int blckNum = -1;

	int isLock = 1;//0为封锁，1为合格
	int blkNum = 0;


	/* Pro*c 标准头文件部分  */
	CString deal_flag = "";
	/********表结构引用*********/
	CModel twmsm61("TWMSM61");



	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);
	CDbCommand cmd_inq2(conn);
	CDbCommand cmd_inq3(conn);
	CString sqlstr = "";

	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


	try
	{
	
		sqlstr = " delete from wl_load_record where sj_no='" + bcls_rec->Tables["21A009"].Rows[0]["PRACTICE_NO"].ToString() + "' ";
		Log::Trace("", __FUNCTION__, "sqlstr		= [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			twmsm61.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			Log::Trace("", __FUNCTION__, "PRACTICE_NO= [{0}]", twmsm61["PRACTICE_NO"].ToString());
			Log::Trace("", __FUNCTION__, "mmat_no= [{0}]", twmsm61["MAT_NO"].ToString());
			sqlstr = " delete from wl_load_record_det																															 \
		where SJ_NO = '" + bcls_rec->Tables["21A009"].Rows[0]["PRACTICE_NO"].ToString() + "' and MAT_NO='"+ twmsm61["MAT_NO"].ToString() +"' ";
			Log::Trace("", __FUNCTION__, "sqlstr		= [{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();
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
