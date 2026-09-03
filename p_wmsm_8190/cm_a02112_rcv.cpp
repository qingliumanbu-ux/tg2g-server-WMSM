/*
*  程序名称			: cm_a02112_rcv
*  程序描述			: 装卸点信息
*
*  	2023-11-20 	聂媛媛			(ADD)程序建立
*			... ...
* **************************************************************************** */
/*<remark>=========================================================
<summary>
装卸点信息
<para>数据库表：TWM0Z(装卸点信息表)         </para>
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
BM2F_ENTERACE_TELE(cm_a02112_rcv)
/* ***** -EP_SYSTEM_HEAD_END ***** */
int f_cm_a02112_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int doFlag = 0;

	CString sqlstr("");

	

	//系统时间
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	//数据库命令执行变量
	CDbCommand cmd(conn);
	CModel twm0z("TWM0Z");
	//添加并设置块名
	int index = 0;


	try
	{
		//传入参数接收
		
		
		for (int i = 0; i < bcls_rec->Tables["A02112"].Rows.get_Count(); i++)
		{
			twm0z.Reset();
			twm0z.MergeFrom(bcls_rec->Tables["A02112"].Rows[i]);
			if (twm0z["DEAL_FLAG"].ToString() == "I")
			{
				sqlstr = "SELECT * FROM twm0z t WHERE t.load_code = @load_code";

				cmd.SetCommandText(sqlstr);
				cmd.Parameters.Set("load_code", twm0z["ULPLACE"].ToString());
				cmd.ExecuteReader();
				CString name = "";
				CString value = "";
				if (cmd.Read())
				{
					twm0z.Update("ULNAME,FACTORY_CODE,FACTORY_NAME,AREA_CODE,AREA_NAME,LGORT_CODE", "ULPALCE");							
				}
				else
				{

					twm0z.Insert();
				}
			}
			else if (twm0z["DEAL_FLAG"].ToString() == "D")
			{
				twm0z.Delete("ULPALCE");
			}
			else
			{
				strncpy(s.msg, (const char*)"操作标记异常", sizeof(s.msg) - 1);
				s.flag = -1;
				doFlag = -1;
				return doFlag;
			}
		}
		

		/*设置系统返回参数*/
		sprintf(s.msg, _RES("GCRSS0000036")/*电文接收成功。*/);
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.msg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
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
	cmd.Close();

	return doFlag;
}