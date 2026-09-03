/*=========================================================================
//程序名称:		f_wmsm_wm07_proc
//隶属子系统:	WM
//产品名称:		出库归档主函数
//创建人员:		李振
//创建时间:		2024-1-1

//-----------------------------------------------------------------------
//功能描述:	 出库归档主函数
//条件描述：
//数据库表:
//
//主调用函数:
//
//-----------------------------------------------------------------------
//函数功能:     出库归档主函数
//传入参数:     材料号
//传出参数:     
//处理流程:
//=========================================================================*/

//#include "WM_Utility.h"
#include "stdafx.h"
int f_mmsm99(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
BM2_FUNCTION_IMPORT


BM2_FUNCTION_EXPORT
int f_wmsm_wm07_proc(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/*程序用变量*/
	int doFlag = 0;
	int ret = 0;
	int sqlid = 0;
	int blckNum = -1;

	int isLock = 1;//0为封锁，1为合格

	/* Pro*c 标准头文件部分  */
	CModel tmmsm96("TMMSM96");
	


	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);

	CString sqlstr = "";

	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	EIClass mm0099;
	mm0099.Tables[0].set_TableName("MM0099");
	mm0099.Tables[0].Columns.Add(tmmsm96);
	mm0099.Tables[0].Rows.Clear();



	try
	{
		Log::Trace("", __FUNCTION__, "LINKE=[{0}]", "----------------- begin -----------------");
		for (int i = 0; i < bcls_rec->Tables["WM07"].Rows.get_Count(); i++)
		{
			tmmsm96.Reset();
			tmmsm96["EVENT_ID"] = "WM07";
			tmmsm96["SYSTEM_ID"] = "MMSM";
			tmmsm96["EVENT_LINE_TYPE"] = "00";
			tmmsm96["FUNC_ID"] = s.svc_name;
			tmmsm96["MAT_NO"] = bcls_rec->Tables["WM07"].Rows[i]["MAT_NO"].ToString();
			tmmsm96["ARCHIVE_TIME"] = datetime;
			tmmsm96.MergeTo(mm0099.Tables["MM0099"], false);
		}
		if (mm0099.Tables[0].Rows.get_Count() > 0)
		{
			doFlag = f_mmsm99(&mm0099, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
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
