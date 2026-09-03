/*
*  程序名称			: cm_A02106_rcv
*  程序描述			: 股道上车辆信息电文接收
*
*  	2023-11-20 	聂媛媛			(ADD)程序建立
*			... ...
* **************************************************************************** */
/*<remark>=========================================================
<summary>
股道上车辆信息电文接收
<para>数据库表：TWM0G(股道上车辆信息表)         </para>
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



BM2F_ENTERACE_TELE(cm_a02106_rcv)
/* ***** -EP_SYSTEM_HEAD_END ***** */
int f_cm_a02106_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

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
	EIClass sm_bcls_rec;

	CModel twm0g("TWM0G");
	CModel twm0g_1("TWM0G");

	/* ***** 电文变量定义 ***** */
	CString	 sqlstr0(""), str_ch(""), str_gdh("");
	CString	  sqlstr3(""), str_ch1(""), str_ch2("");

	CString	str_intime(""), str_sldbz(""), str_yjzcsj(""), str_yjzczrz(""), str_zcxkz("");

	/* ***** 程序变量 ***** */
	CString c_user = " ", c_tc_no = " ", c_mat_kind = " ", datetime = " ";

	/* ***** 数据库SQL操作字符串 ***** */
	CString	sqlstr(""), sqlstr_1(""), s_message("");

	/* ***** 数据库操作类定义 ***** */
	CDbCommand execute_sql(conn);

	/* ***** 应用程序开始处理 ***** */
	try
	{
		/* ***** 获取电文号 ***** */
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		c_tc_no = s.username;
		c_user = c_tc_no;

		/* ***** 解析电文 ***** */

		twm0g["DEAL_FLAG"] = bcls_rec->Tables["A02106"].Rows[0]["DEAL_FLAG"];
		
		twm0g["STATION_TRACK"] = bcls_rec->Tables["A02106"].Rows[0]["STATION_TRACK"];
		if (twm0g["STATION_TRACK"].ToString().Trim()== "")
		{
			s_message = "股道号：为空！";
			strcpy(s.msg, s_message);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		Log::Trace("", __FUNCTION__, "传入参数 station_track = [{0}]", twm0g["STATION_TRACK"].ToString().Trim());

		twm0g["REC_CREATOR"] = c_tc_no;		     /* 记录创建责任者 */
		twm0g["REC_CREATE_TIME"] = datetime;		 /* 记录创建时刻 */


		//*************************判断操作标记是I*******************************************************************//
		if (twm0g["DEAL_FLAG"].ToString().Trim() == "I")
		{

			for (i = 0; i < bcls_rec->Tables["A02106_1"].Rows.get_Count(); i++)
			{
				twm0g.Reset();
				twm0g.MergeFrom(bcls_rec->Tables["A02106_1"].Rows[i]);
				twm0g["SEQ_NOW"] = bcls_rec->Tables["A02106_1"].Rows[i]["SEQ_NO"].ToDouble();
				twm0g["INSPECTION_STATUS_O"] = bcls_rec->Tables["A02106_1"].Rows[i]["INSPECTION_STATUS_O"].ToString().TrimOrBlank();
				twm0g["STATION_TRACK"] = bcls_rec->Tables["A02106"].Rows[0]["STATION_TRACK"];
				twm0g["STATION_TRACK_DESC"] = bcls_rec->Tables["A02106"].Rows[0]["STATION_TRACK_CNAME"].ToString().TrimOrBlank();
				sqlstr3 = "select * from twm0g where STATION_TRACK=@station_track and wagonno =@wagonno";
				execute_sql.SetCommandText(sqlstr3);
				execute_sql.Parameters.Set("station_track", twm0g["STATION_TRACK"].ToString().Trim());
				execute_sql.Parameters.Set("wagonno", twm0g["WAGONNO"].ToString().Trim());
				execute_sql.ExecuteReader();
				if (execute_sql.Read())
				{
					execute_sql.Fetch(twm0g_1);
					twm0g["IN_TIME"] = twm0g_1["IN_TIME"];
					twm0g["BACK_CODE_2"] = twm0g_1["BACK_CODE_2"];
					twm0g["REC_REVISE_TIME"] = twm0g_1["REC_REVISE_TIME"];
					twm0g["REC_REVISOR"] = twm0g_1["REC_REVISOR"];
					twm0g["ARCHIVE_STAMP_NO"] = twm0g_1["ARCHIVE_STAMP_NO"];
				}
				execute_sql.Close();
					twm0g_1.Delete("STATION_TRACK,WAGONNO");

					twm0g.TrimOrBlank();
					twm0g.Insert();
			}
			
		}

		if (twm0g["DEAL_FLAG"].ToString() == "D")
		{
			twm0g["STATION_TRACK"] = bcls_rec->Tables["A02106"].Rows[0]["STATION_TRACK"];
			for (i = 0; i < bcls_rec->Tables["A02106_1"].Rows.get_Count(); i++)
			{
				twm0g["WAGONNO"] = bcls_rec->Tables["A02106_1"].Rows[i]["WAGONNO"].ToString();
				twm0g.Delete("STATION_TRACK,WAGONNO");
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







