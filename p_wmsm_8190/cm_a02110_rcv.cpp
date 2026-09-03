/*
*  程序名称			: cm_a02110_rcv
*  程序描述			: 铁路车辆上离道信息
*
*  	2023-11-20 	聂媛媛			(ADD)程序建立
*			... ...
* **************************************************************************** */
/*<remark>=========================================================
<summary>
铁路车辆上离道信息
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
BM2F_ENTERACE_TELE(cm_a02110_rcv)
/* ***** -EP_SYSTEM_HEAD_END ***** */
int f_cm_a02110_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);
	/* 程序用变量 */
	int doFlag = 0;

	CString   datetime;            /* 取时间 */
	CString   userid = " ";           /* 登陆用户 */
	int blkNum = 0;

	//CTracer log(__FUNCTION__);
	//int 	doFlag = 0; 
	int 	i = 0;
	int RowCount = 0;
	CString v_rec_time = " ";			/* 接收电文时间 */
	CString sql = " ";
	CString sql_ins = " ";
	CString sql_inq1 = " ";
	CString sql_inq2 = " ";
	CString sql_inq3 = " ";
	CString sql_inq4 = " ";
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_sql(conn);
	CDbCommand cmd_sql1(conn);
	CDbCommand cmd_sql2(conn);
	CDbCommand cmd_sql3(conn);

	CString REC_ERASE_TIME = " ";
	CString REC_ERASOR = " ";

	CString DEAL_FLAG = " ";
	CString STATION_TRACK = " ";
	CString STATION_TRACK_CNAME = " ";
	CString KEY_WORDFIVE = " ";
	CString WAGONNO = " ";
	CString WAGONNO_1 = " ";
	CString WORK_TYPE = " ";
	CString WORK_TIME = " ";
	CString WORK_TIME_1 = " ";
	CString REMARK_1 = " ";
	CString REMARK_2 = " ";
	CString REMARK_3 = " ";
	CString REMARK_4 = " ";
	CString REMARK_5 = " ";
	CString REC_CREATE_TIME = " ";
	CString REC_CREATOR = " ";
	CString BACK_CODE_3 = " ";

	// 数据库SQL操作字符串
	CString  sqlstr("");

	/*电文头部分*/
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

	try
	{
		v_rec_time = CDateTime::Now().ToString("yyyyMMddHHmmss");
		RowCount = bcls_rec->Tables["A02110_1"].Rows.get_Count();
		Log::Trace("", __FUNCTION__, "RowCount [{0}]  ", RowCount);
		for (i = 0; i < RowCount; i++)
		{

			CString DEAL_FLAG = " ";
			CString STATION_TRACK = " ";
			CString STATION_TRACK_CNAME = " ";
			CString KEY_WORDFIVE = " ";
			CString WAGONNO = " ";
			CString WORK_TYPE = " ";
			CString WORK_TIME = " ";
			CString REMARK_1 = " ";
			CString REMARK_2 = " ";
			CString REMARK_3 = " ";
			CString REMARK_4 = " ";
			CString REMARK_5 = " ";
			/* ***** 获取输入参数 ***** */
			DEAL_FLAG = bcls_rec->Tables["A02110"].Rows[0]["deal_flag"].ToString();
			STATION_TRACK = bcls_rec->Tables["A02110"].Rows[0]["station_track"].ToString();
			STATION_TRACK_CNAME = bcls_rec->Tables["A02110"].Rows[0]["station_track_cname"].ToString();
			REMARK_1 = bcls_rec->Tables["A02110"].Rows[0]["remark_1"].ToString();
			REMARK_2 = bcls_rec->Tables["A02110"].Rows[0]["remark_2"].ToString();
			REMARK_3 = bcls_rec->Tables["A02110"].Rows[0]["remark_3"].ToString();
			REMARK_4 = bcls_rec->Tables["A02110"].Rows[0]["remark_4"].ToString();
			REMARK_5 = bcls_rec->Tables["A02110"].Rows[0]["remark_5"].ToString();
			KEY_WORDFIVE = bcls_rec->Tables["A02110_1"].Rows[i]["key_wordfive"].ToString();//
			WAGONNO = bcls_rec->Tables["A02110_1"].Rows[i]["wagonno"].ToString();
			WORK_TYPE = bcls_rec->Tables["A02110_1"].Rows[i]["work_type"].ToString();//
			WORK_TIME = bcls_rec->Tables["A02110_1"].Rows[i]["work_time"].ToString();//


			REC_CREATE_TIME = v_rec_time;
			REC_CREATOR = s.userid;

			Log::Trace("", __FUNCTION__, "DEAL_FLAG ", DEAL_FLAG);
			Log::Trace("", __FUNCTION__, "STATION_TRACK ", STATION_TRACK);
			Log::Trace("", __FUNCTION__, "STATION_TRACK_CNAME ", STATION_TRACK_CNAME);

			//增加判断，如果数据库表TPSHRPK中已经存在该计划号和入口材料号的计划信息，则先删除，再新增。
			CString sql_inq = "";
			CDecimal countmm = 0;


			if (DEAL_FLAG == "I")//新增DEAL_FLAG == "I"
			{
				//离道信息暂不保存，有需要去掉注释
				/*sql_ins = CString("insert into table (DEAL_FLAG, "
					"STATION_TRACK, "
					"STATION_TRACK_CNAME, "
					"KEY_WORDFIVE, "
					"WAGONNO, "
					"WORK_TYPE, "

					"WORK_TIME, "
					"REMARK_1, "
					"REMARK_2, "
					"REMARK_3, "
					"REMARK_4, "
					"REMARK_5, "
					"INGOT_TYPE, "

					"REC_CREATE_TIME,"
					"REC_CREATOR )"
					" values "
					" ('" + DEAL_FLAG + "', "
					"'" + STATION_TRACK + "', "
					"'" + STATION_TRACK_CNAME + "', "
					"'" + KEY_WORDFIVE + "', "
					"'" + WAGONNO + "', "
					"'" + WORK_TYPE + "', "

					"'" + WORK_TIME + "', "
					"'" + REMARK_1 + "', "
					"'" + REMARK_2 + "', "
					"'" + REMARK_3 + "', "
					" to_number('" + REMARK_4 + "') , "
					" to_number('" + REMARK_5 + "') , "
					"'" + BACK_CODE_3 + "', "

					" '" + REC_CREATE_TIME + "', "
					" '" + REC_CREATOR + "')"

					);

				Log::Trace("", __FUNCTION__, "sql_ins={0}  ", sql_ins);
				cmd_sql.SetCommandText(sql_ins);
				cmd_sql.ExecuteNonQuery();
				cmd_sql.Close();*/

				CString BACK_CODE_2 = "";
				if (WORK_TYPE == "-")
				{
					BACK_CODE_2 = "1";
				}
				if (WORK_TYPE == "+")
				{
					//BACK_CODE_2 = "2";
					sql_inq2 = CString("delete from twm0g "
						"where STATION_TRACK= '" + STATION_TRACK + "'"
						"and WAGONNO =  '" + WAGONNO + "' ");
					Log::Trace("sql_inq2", __FUNCTION__, sql_inq2);
					cmd_sql.SetCommandText(sql_inq2);
					cmd_sql.ExecuteNonQuery();
					cmd_sql.Close();
				}
				REC_ERASE_TIME = v_rec_time;
				REC_ERASOR = s.userid;
				Log::Trace("", __FUNCTION__, "$$$$$$$$$$$$$$$$$$$$$$$={0}  ", "$$$$$$$$$$$$$$$$$$$$$$$");
				sql_inq3 = CString("update twm0g set "
					"IN_TIME = '" + WORK_TIME + "',"
					"BACK_CODE_2 = '" + BACK_CODE_2 + "',"
					"REC_REVISOR = '" + REC_ERASOR + "',"
					"REC_REVISE_TIME = '" + REC_ERASE_TIME + "'"
					"where STATION_TRACK= '" + STATION_TRACK + "'"
					"and WAGONNO =  '" + WAGONNO + "' "
					"and REC_CREATE_TIME = (select distinct max(t1.REC_CREATE_TIME) from twm0g t1 where t1.station_track='" + STATION_TRACK + "')");
				Log::Trace("", __FUNCTION__, "sql_inq3={0}  ", sql_inq3);
				cmd_sql.SetCommandText(sql_inq3);
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




