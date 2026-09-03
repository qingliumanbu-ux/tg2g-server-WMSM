/*
*  程序名称			: cm_a02111_rcv
*  程序描述			: 铁路临钢坯装车实绩
*
*  	2023-11-9 	李振			(ADD)程序建立
*			... ...
* **************************************************************************** */
/*<remark>=========================================================
 <summary>
 铁路临钢坯装车实绩
 1、记录装车实绩
 2、材料归档
 3、通知制造
 <para>数据库表：TWMSM61LG(装车实绩表(临钢))         </para>
 </summary>
 <returns>电文处理成功与否</returns>
===========================================================</remark>*/

/* C/C++ 的标准头文件部分 */


#include "stdafx.h"
#include "epex.h"
int f_mmsm99(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wmsm_t80ry0_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);


BM2F_ENTERACE_TELE(cm_a02111_rcv)
int f_cm_a02111_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);
	/* ***** 静态变量定义 ***** */
	int doFlag = 0;
	int fetchRowCount = 0;
	int row_count = 0, i = 0, ret = 0;
	EPEX epex;


	CString lpsz_user_id, c_datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString	lpsz_out_div;
	EIClass sm_bcls_rec;

	CModel tmmsm96("TMMSM96");
	CModel tmmsm01("TMMSM01");
	CModel twm41dj("TWM41DJ");
	CModel twmsm61lg("TWMSM61LG");
	CModel twmsm64("TWMSM64");

	/* ***** 电文变量定义 ***** */
	CString    c_mat_no;
	CString    c_ready_bill_no;
	CString    c_order_no;
	CString    c_red_cause_desc;
	CString    c_rec_revisor;
	CString    c_rec_revise_time;
	CString    c_red_flag;


	/* ***** 程序变量 ***** */
	CString c_user = " ", c_tc_no = " ";

	/* ***** 数据库SQL操作字符串 ***** */
	CString	sqlstr(""), sqlstr1(""), sqlstr2(""), sqlstr3(""), sqlstr4(""), sqlstr5(""),sql("");

	/* ***** 数据库操作类定义 ***** */
	CDbCommand execute_sql(conn);
	CDbCommand cmd_inq(conn);

	/* ***** 应用程序开始处理 ***** */
	if (!bcls_rec->Tables.Contains("MM0099")) {
		bcls_rec->Tables.Add("MM0099");
		bcls_rec->Tables["MM0099"].Columns.Add(tmmsm96);
	}
	EIClass mm0099;
	mm0099.Tables[0].set_TableName("MM0099");
	mm0099.Tables[0].Columns.Add(tmmsm96);
	mm0099.Tables[0].Rows.Clear();
	
	try
	{
		if (!bcls_rec->Tables.IndexOf("A02111_1")) {
			sprintf(s.msg, "电文不包含A02111_1表。");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (bcls_rec->Tables["A02111"].Rows[0]["DEAL_FLAG"].ToString()=="I")
		{
			for (int i = 0; i < bcls_rec->Tables["A02111_1"].Rows.get_Count(); i++) {
				twmsm61lg.Reset();
				twmsm61lg.MergeFrom(bcls_rec->Tables[0].Rows[0]);
				twmsm61lg.MergeFrom(bcls_rec->Tables["A02111_1"].Rows[i]);
				Log::Trace("", "", "line", __LINE__);
				//twmsm61lg.Print();
				if (!twmsm61lg.Query("PRACTICE_NO,MAT_NO"))
				{
					twmsm61lg["REC_CREATE_TIME"] = c_datetime;
					twmsm61lg.Insert();
				}
				

				tpcommit(0);
				tpbegin(0, 0);

				Log::Trace("", "", "line", __LINE__);
				tmmsm01["MAT_NO"] = twmsm61lg["MAT_NO"];
				if (!tmmsm01.Query("MAT_NO"))
				{
					continue;
				}
				Log::Trace("", "", "line", __LINE__);

				if (tmmsm01["COMPLEX_DECIDE_CODE"].ToString()!="1")
				{
					/*sprintf(s.msg, "成品不允许发往临钢！");
					throw CApplicationException(-1, s.msg, log.Location);*/
					twmsm61lg["BACK19"] = "未综判，不允许调拨";
					twmsm61lg.Update("BACK19", "PRACTICE_NO,MAT_NO");
					continue;
				}
				if (tmmsm01["PRODUCT_FLAG"].ToString() == "1")
				{
					/*sprintf(s.msg, "成品不允许发往临钢！");
					throw CApplicationException(-1, s.msg, log.Location);*/
					twmsm61lg["BACK19"] = "成品不允许发往临钢";
					twmsm61lg.Update("BACK19", "PRACTICE_NO,MAT_NO");
					continue;
				}

				twm41dj.Reset();
				twm41dj.CopyFrom(tmmsm01);
				twm41dj["REC_CREATOR"] = s.userid;
				twm41dj["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				twm41dj["C_DELIVERYID"] = "6240" + c_datetime.SubstringNE(0, 8) + Db::QueryCString("SELECT LPAD(TO_CHAR(DB_NO.NEXTVAL), 4, '0') FROM DUAl");
				twm41dj["C_QULITYTRACEID"] = tmmsm01["HEAT_NO"];//炉号
				twm41dj["C_BATCHID"] = tmmsm01["BATCH"];//批次号
				twm41dj["C_BATCHUNIT"] = tmmsm01["MAT_NO"];//
				twm41dj["C_SENDDEPT"] = "6240";//发送工厂
				twm41dj["C_ACCEPTDEPT"] = "6380";//接受工厂1
				twm41dj["C_SENDSTOCK"] = tmmsm01["LGORT"];//发送库房
				twm41dj["C_ACCEPTSTOCK"] = "6381";//接受库房
				twm41dj["DELIVERY_THICKNESS"] = tmmsm01["MAT_THICK"];//厚度
				twm41dj["DELIVERY_WIDTH"] = tmmsm01["MAT_WIDTH"];//宽度1
				twm41dj["STEELGRADE"] = tmmsm01["ST_NO"];//钢牌号
				twm41dj["N_SENDAMOUNT"] = tmmsm01["MAT_ACT_WT"];//发送重量
				twm41dj["C_SENDUNIT"] = "TON";//发送单位
				twm41dj["N_ACCEPTAMOUNT"] = tmmsm01["MAT_WT"];//接收重量
				twm41dj["C_ACCEPTUNIT"] = "TON";//接收单位
				twm41dj["C_STATESIGN"] = "1";//调拨状态（1-未确认，2-接收，3-驳回）
				twm41dj["D_OPERATIONDATE"] = c_datetime;
				twm41dj["D_BILLDATE"] = c_datetime;
				twm41dj["T_OUTSTOCKTIME"] = c_datetime;
				twm41dj["I_STOCKMODE"] = "件次";
				twm41dj["C_REMARK"] = tmmsm01["SG_GRADE_1"];
				twm41dj["I_RESERVECOL4"] = "0";//调拨类型（0-正常调拨，1-回退调拨）
				twm41dj["C_INSTOCKSIGN"] = "3";
				twm41dj["C_ISFREEZE"] = "FREE";//库存类型-
				twm41dj["C_STOCKSPEC"] = "FREE";//特殊库存标识
				twm41dj["C_ORDERID"] = tmmsm01["ORDER_NO"];//合同号
				twm41dj["I_RESERVECOL3"] = tmmsm01["MAT_LEN"];
				twm41dj["C_ACHIEVEID"] = "1";
				twm41dj["C_TRUCKNUM"] = bcls_rec->Tables["A02111"].Rows[0]["WAGONNO"].ToString();
				if (tmmsm01["PRODUCT_FLAG"].ToString() == "1")
				{
					twm41dj["C_PRODUCTID"] = "FAA000000000000000";
					twm41dj["C_PRODUCTNAME"] = "连铸卷板坯";
				}
				else
				{
					twm41dj["C_PRODUCTID"] = "HAA000000000000000";
					twm41dj["C_PRODUCTNAME"] = "连铸卷板坯";
				}
				
				twm41dj.Insert();

				tmmsm96.Reset();
				tmmsm96.CopyFrom(tmmsm01);
				CString v_shift_no = " ";
				CString v_shift_group = " ";
				f_epep_get_shift_group("SMCP", c_datetime, v_shift_no, v_shift_group, conn);
				tmmsm96["C_STATESIGN"] = "1";//1--正向调拨出库，3--正向调拨完成
				tmmsm96["C_DELIVERYID"] = twm41dj["C_DELIVERYID"].ToString();
				tmmsm96["C_DELIVERY_FAC"] = "6380";
				tmmsm96["C_DELIVERY_STOCK"] = "6381";
				tmmsm96["TRAN_TIME"] = c_datetime;
				tmmsm96["HAND_OVER_GROUP"] = v_shift_group;
				tmmsm96["EVENT_ID"] = "MM76";
				tmmsm96["SYSTEM_ID"] = "MMSM";
				tmmsm96["EVENT_LINE_TYPE"] = "00";
				tmmsm96["FUNC_ID"] = s.svc_name;
				tmmsm96.MergeTo(mm0099.Tables["MM0099"], false);

				//初始化
				CString	s_tc_no = "T80RY0";
				ret = epex.Initialize(s_tc_no);
				if (ret < 0)
				{
					CFormattable arguments[] = { s_tc_no }; // 定义参数列表的数组
					CMessageFormat::Format(s.msg, _RES("YM00S0000514")/*初始化电文[{0}]失败。*/, arguments, 1);
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				//拼电文数据
				if (epex.SetValue("MAT_NO", 0, tmmsm01["MAT_NO"].ToString()) < 0
					|| epex.SetValue("DEAL_FLAG", 0, "I") < 0
					|| epex.SetValue("PLANT", 0, "6240") < 0
					|| epex.SetValue("STGE_LOC", 0, tmmsm01["LGORT"].ToString()) < 0
					|| epex.SetValue("MOVE_PLANT", 0, "6380") < 0
					|| epex.SetValue("MOVE_STLOC", 0, "6381") < 0
					|| epex.SetValue("HEAD_TEXT", 0, " ") < 0)
				{
					strcpy(s.msg, _RES("GCRSS0000015"));//系统出现异常，电文拼接出错，请联系系统维护人员。
					throw CApplicationException(-1, s.msg, s.svc_name);
				}





				if (epex.SendTele() < 0)
				{
					strcpy(s.msg, _RES("GCRSS0000032")/*电文发送失败。*/);
					sprintf(s.sysmsg, "[%s]发送失败", (const char*)s_tc_no);
					throw CApplicationException(-1, s.msg, s.svc_name);
				}


				epex.Uninitialize();


				Log::Trace("", "", "line", __LINE__);

				twmsm61lg["BACK20"] = "S";
				twmsm61lg.Update("BACK20", "PRACTICE_NO,MAT_NO");

			}
		}
		else if (bcls_rec->Tables["A02111"].Rows[0]["DEAL_FLAG"].ToString() == "D")
		{
			//for (int i = 0; i < bcls_rec->Tables["A02111_1"].Rows.get_Count(); i++) {
			//	twmsm61lg.Reset();
			//	twmsm61lg.MergeFrom(bcls_rec->Tables[0].Rows[0]);
			//	twmsm61lg.MergeFrom(bcls_rec->Tables["A02111_1"].Rows[i]);
			//	Log::Trace("", "", "line", __LINE__);
			//	//twmsm61lg.Print();
			//	twmsm61lg.Delete("PRACTICE_NO,MAT_NO");
			//	Log::Trace("", "", "line", __LINE__);
			//	tmmsm01["MAT_NO"] = twmsm61lg["MAT_NO"];
			//	if (tmmsm01.Query("MAT_NO"))
			//	{
			//		continue;
			//	}
			//	Log::Trace("", "", "line", __LINE__);

			//	

			//	//初始化
			//	CString	s_tc_no = "T80RY0";
			//	ret = epex.Initialize(s_tc_no);
			//	if (ret < 0)
			//	{
			//		CFormattable arguments[] = { s_tc_no }; // 定义参数列表的数组
			//		CMessageFormat::Format(s.msg, _RES("YM00S0000514")/*初始化电文[{0}]失败。*/, arguments, 1);
			//		throw CApplicationException(-1, s.msg, s.svc_name);
			//	}

			//	//拼电文数据
			//	if (epex.SetValue("MAT_NO", 0, tmmsm01["MAT_NO"].ToString()) < 0
			//		|| epex.SetValue("DEAL_FLAG", 0, "I") < 0
			//		|| epex.SetValue("PLANT", 0, "6380") < 0
			//		|| epex.SetValue("STGE_LOC", 0, "6381") < 0
			//		|| epex.SetValue("MOVE_PLANT", 0, "6240") < 0
			//		|| epex.SetValue("MOVE_STLOC", 0, "6242") < 0
			//		|| epex.SetValue("HEAD_TEXT", 0, " ") < 0)
			//	{
			//		strcpy(s.msg, _RES("GCRSS0000015"));//系统出现异常，电文拼接出错，请联系系统维护人员。
			//		throw CApplicationException(-1, s.msg, s.svc_name);
			//	}





			//	if (epex.SendTele() < 0)
			//	{
			//		strcpy(s.msg, _RES("GCRSS0000032")/*电文发送失败。*/);
			//		sprintf(s.sysmsg, "[%s]发送失败", (const char*)s_tc_no);
			//		throw CApplicationException(-1, s.msg, s.svc_name);
			//	}


			//	epex.Uninitialize();


			//	Log::Trace("", "", "line", __LINE__);

			//}
		}
		
		
		if (mm0099.Tables["MM0099"].Rows.get_Count() > 0)
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
		Log::Error("", __FUNCTION__, "error=[{0}]", s.msg);
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
