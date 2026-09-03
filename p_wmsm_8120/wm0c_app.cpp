/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         ljnie
Version:		1.0
Date:			2016/7/6 15:34:36
Description:	库位材料倒垛查询
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"
//程序用头文件
//#include "twm04.h"
//#include "twma1.h"



BM2_FUNCTION_IMPORT
int f_mm0011(CString SeqName, CDecimal SeqLen, CString &SeqNo, CDbConnection * conn);	//获取流水号
//函数申明
int f_epes_get_auth_other(const char *iuser, int irestype, EIClass *bcls_ret, CDbConnection * conn);

//int f_cm_7z8t01_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//给物流发送电文 

/*<remark>=========================================================
///<summary>
///库位材料倒垛查询
///<para>
///2.排序方式：STOCK_NO,HALL_NO
///</para>
///<para>数据库表：TWM04 仓库跨号信息查询；TWMA2
///<returns>返回符合查询条件的仓库跨号信息</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wm0c_app);

int f_wm0c_app(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	EPEX epex;

	/* 程序内部变量 */
	int doFlag = 0;
	int ret = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString datetime_next = CDateTime::Now().AddDays(+1).ToString("yyyyMMddHHmmss");

	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */
	//CTWM04 twm04(conn);
	//CTWMA1 twma1_q(conn);
	CModel twm0c = CModel("TWM0C");
	CModel twm0d = CModel("TWM0D");
	CModel twm01 = CModel("TWM01");
	CModel tsi0021_to = CModel("TSI0021");

	/* 业务变量 */
	CString oper_type("");
	CString stock_no("");
	CString aim_stock_no("");
	CString loading_plan_no("");
	CString v_loading_plan_no("");
	CString app_remark("");
	CString prod_seq_no("");
	CString truck_model("");
	CString pond_flag("");
	CString prod_code("");
	CString prod_cname("");
	CString prod_shift("");
	CString prod_group("");
	CString data("");
	CString transfer_plan_no("");
	CString balance_code("");
	CString expiry_date("");
	CString annt_name("");
	CString tel("");
	CString back_c1("");
	CString truck_no("");
	CString app_date("");
	CString pro_flag("");
	

	CDecimal vehicle_num = 0;
	CDecimal weight_app = 0;
	CDecimal stock_num = 0;

	//CString s_tc_no = "JOJL01";

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlstr1 = "";
	CString sqlwhere = "";
	CString sqlorderby = "";
	CString s_userid("");


	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	EIClass bcls_rec_send;
	bcls_rec_send.Tables[0].Columns.Add(DT_STRING, "DEAL_FLAG");
	bcls_rec_send.Tables[0].Columns.Add(DT_STRING, "PLAN_NO");
	bcls_rec_send.Tables[0].Columns.Add(DT_STRING, "LOAD_CODE_AREA");
	bcls_rec_send.Tables[0].Columns.Add(DT_STRING, "LOAD_NAME_AREA");
	bcls_rec_send.Tables[0].Columns.Add(DT_STRING, "UNLOAD_CODE_AREA");
	bcls_rec_send.Tables[0].Columns.Add(DT_STRING, "UNLOAD_NAME_AREA");
	bcls_rec_send.Tables[0].Columns.Add(DT_STRING, "BALANCE_CODE");
	bcls_rec_send.Tables[0].Columns.Add(DT_STRING, "EXPIRY_DATE");
	bcls_rec_send.Tables[0].Columns.Add(DT_STRING, "PROD_CODE");
	bcls_rec_send.Tables[0].Columns.Add(DT_STRING, "PROD_CNAME");
	bcls_rec_send.Tables[0].Columns.Add(DT_STRING, "S_USERID");
	bcls_rec_send.Tables[0].Columns.Add(DT_STRING, "TRUCK_MODEL");
	bcls_rec_send.Tables[0].Columns.Add(DT_DECIMAL, "PLAN_WGNUM");
	bcls_rec_send.Tables[0].Columns.Add(DT_DECIMAL, "PLAN_QTY");
	bcls_rec_send.Tables[0].Columns.Add(DT_STRING, "ANNT_NAME");
	bcls_rec_send.Tables[0].Columns.Add(DT_STRING, "TEL");
	bcls_rec_send.Tables[0].Columns.Add(DT_STRING, "BACK_C1");
	bcls_rec_send.Tables[0].Columns.Add(DT_STRING, "APP_REMARK");
	bcls_rec_send.Tables[0].Rows.Clear();
	bcls_rec_send.Tables[0].Rows.Add();

	try
	{

		// 获取前台传入参数
		s_userid = s.userid;
		//获取库区授权
		CString stock_no_auth = "' '";
		EIClass *bcls_auth = new EIClass;
		if (f_epes_get_auth_other(s.userid, 5, bcls_auth, conn) != 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
		for (int fetchRowCount = 0; fetchRowCount < bcls_auth->Tables[0].Rows.get_Count(); fetchRowCount++)
		{
			stock_no_auth += ", '" + bcls_auth->Tables[0].Rows[fetchRowCount]["name"].ToString() + "' ";
		}
		delete bcls_auth;



		if (bcls_rec->Tables[0].Columns.Contains("OPER_TYPE"))
			oper_type = bcls_rec->Tables[0].Rows[0]["OPER_TYPE"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("STOCK_NO"))
			stock_no = bcls_rec->Tables[0].Rows[0]["STOCK_NO"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("AIM_STOCK_NO"))
			aim_stock_no = bcls_rec->Tables[0].Rows[0]["AIM_STOCK_NO"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("APP_REMARK"))
			app_remark = bcls_rec->Tables[0].Rows[0]["APP_REMARK"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("VEHICLE_NUM"))
			vehicle_num = bcls_rec->Tables[0].Rows[0]["VEHICLE_NUM"].ToDecimal();
		if (bcls_rec->Tables[0].Columns.Contains("WEIGHT_APP"))
			weight_app = bcls_rec->Tables[0].Rows[0]["WEIGHT_APP"].ToDecimal();
		if (bcls_rec->Tables[0].Columns.Contains("TRUCK_MODEL"))
			truck_model = bcls_rec->Tables[0].Rows[0]["TRUCK_MODEL"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("POND_FLAG"))
			pond_flag = bcls_rec->Tables[0].Rows[0]["POND_FLAG"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("LOADING_PLAN_NO"))
			v_loading_plan_no = bcls_rec->Tables[0].Rows[0]["LOADING_PLAN_NO"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("TRANSFER_PLAN_NO"))
			transfer_plan_no = bcls_rec->Tables[0].Rows[0]["TRANSFER_PLAN_NO"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("BALANCE_CODE"))
			balance_code = bcls_rec->Tables[0].Rows[0]["BALANCE_CODE"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("EXPIRY_DATE"))
			expiry_date = bcls_rec->Tables[0].Rows[0]["EXPIRY_DATE"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("ANNT_NAME"))
			annt_name = bcls_rec->Tables[0].Rows[0]["ANNT_NAME"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("TEL"))
			tel = bcls_rec->Tables[0].Rows[0]["TEL"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("BACK_C1"))  //计划开始时间
			back_c1 = bcls_rec->Tables[0].Rows[0]["BACK_C1"].ToString();


		/* ***** 打印输入参数 ***** */
		Log::Trace("", __FUNCTION__, "oper_type\t[{0}]", oper_type);
		Log::Trace("", __FUNCTION__, "stock_no\t[{0}]", stock_no);
		Log::Trace("", __FUNCTION__, "aim_stock_no\t[{0}]", aim_stock_no);
		Log::Trace("", __FUNCTION__, "app_remark\t[{0}]", app_remark);
		Log::Trace("", __FUNCTION__, "vehicle_num\t[{0}]", vehicle_num);
		Log::Trace("", __FUNCTION__, "weight_app\t[{0}]", weight_app);
		Log::Trace("", __FUNCTION__, "truck_model\t[{0}]", truck_model);
		Log::Trace("", __FUNCTION__, "pond_flag\t[{0}]", pond_flag);
		Log::Trace("", __FUNCTION__, "v_loading_plan_no\t[{0}]", v_loading_plan_no);
		Log::Trace("", __FUNCTION__, "transfer_plan_no\t[{0}]", transfer_plan_no);
		Log::Trace("", __FUNCTION__, "BALANCE_CODE\t[{0}]", balance_code);
		Log::Trace("", __FUNCTION__, "expiry_date\t[{0}]", expiry_date);
		Log::Trace("", __FUNCTION__, "ANNT_NAME\t[{0}]", annt_name);
		Log::Trace("", __FUNCTION__, "tel\t[{0}]", tel);
		Log::Trace("", __FUNCTION__, "back_c1\t[{0}]", back_c1);


		if (stock_no.Trim() == "")
		{
			sprintf(s.msg, "库区号不能为空！");
			throw CApplicationException(-1, s.msg, log.Location);
		}


		if (oper_type.Trim() == "I")
		{

			if (aim_stock_no.Trim() == "")
			{
				sprintf(s.msg, "目标库区号不能为空！");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (balance_code.Trim() == "")
			{
				sprintf(s.msg, "结算方式号不能为空！");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (vehicle_num == 0)
			{
				sprintf(s.msg, "车数不能为空！");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (weight_app == 0)
			{
				sprintf(s.msg, "运量不能为空！");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (expiry_date.Trim() == "")
			{
				sprintf(s.msg, "有效时间不能为空！");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (truck_model.Trim() == "")//车型
			{
				truck_model = " ";
			}
			if (pond_flag.Trim() == "")//过磅标记
			{
				pond_flag = "1"; //1 不过磅
			}
			if (annt_name.Trim() == "")//联系人
			{
				sprintf(s.msg, "联系人不能为空！");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (tel.Trim() == "")//过磅标记
			{
				sprintf(s.msg, "联系方式不能为空！");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			//校验库区流向
			/*sqlstr = " SELECT COUNT(*) FROM TEP0002 WHERE CODE_CLASS = 'WM88' "
				"AND code_desc_1_content = @from_stock_no AND code_desc_2_content = @to_stock_no ";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("from_stock_no", stock_no);
			cmd_inq.Parameters.Set("to_stock_no", aim_stock_no);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				stock_num = cmd_inq.GetDecimal(1);
			}
			Log::Trace("", __FUNCTION__, "库区流向 stock_num =[{0}]", stock_num);
			if (stock_num == 0)
			{
				sprintf(s.msg, "请维护小代码WM88的库区流向.");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			cmd_inq.Close();*/
		}

		twm01["STOCK_NO"] = stock_no;
		tsi0021_to["STOCK_NO"] = aim_stock_no;
		twm01.Query();
		tsi0021_to.Query();

		//库区中文名称
		sqlstr = "SELECT STOCK_DESC FROM TSI0021 WHERE STOCK_NO =@from_stock_no";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("from_stock_no", stock_no);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			bcls_rec_send.Tables[0].Rows[0]["LOAD_NAME_AREA"] = cmd_inq.GetString(1);
		}
		cmd_inq.Close();

		sqlstr = "SELECT STOCK_DESC FROM TSI0021 WHERE STOCK_NO =@to_stock_no";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("to_stock_no", aim_stock_no);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			bcls_rec_send.Tables[0].Rows[0]["UNLOAD_NAME_AREA"] = cmd_inq.GetString(1);
		}
		cmd_inq.Close();

		if (twm01["MAT_KIND"].ToString().Trim() == "SM" && twm01["STOCK_NO"].ToString().Trim() == "SA1")
		{
			prod_code = "AA";
			prod_cname = "板坯";
		}
		else if (twm01["MAT_KIND"].ToString().Trim() == "SM" && twm01["STOCK_NO"].ToString().Trim() == "SA2")
		{
			prod_code = "BA";
			prod_cname = "方坯";
		}

		/*f_epep_get_shift_group_day("HR", datetime, prod_shift, prod_group, data, conn);
		Log::Trace("", __FUNCTION__, "班次 prod_shift	= [{0}]", prod_shift);
		Log::Trace("", __FUNCTION__, "班组 prod_group	= [{0}]", prod_group);*/

		//发送用车申请电文
		if (oper_type.Trim() == "I" || oper_type.Trim() == "D" || oper_type.Trim() == "C")
		{
			if (oper_type.Trim() == "I")
			{
				prod_seq_no = EPGetNextSeq("WM_PLAN_NO", conn);
				prod_seq_no.Format("%.4d", prod_seq_no);
				Log::Trace("", __FUNCTION__, "prod_seq_no\t[{0}]", prod_seq_no);

				loading_plan_no = stock_no + datetime.SubstringNE(0, 8) + prod_seq_no;

				twm0c.Reset();
				twm0c["REC_CREATOR"] = s.userid;
				twm0c["REC_CREATE_TIME"] = datetime;
				twm0c["PRO_FLAG"] = oper_type;
				twm0c["LOADING_PLAN_NO"] = loading_plan_no;
				twm0c["FACTORY_DIV"] = twm01["FACTORY_DIV"];
				twm0c["STOCK_NO"] = stock_no;
				twm0c["AIM_STOCK_NO"] = aim_stock_no;
				twm0c["APP_DATE"] = datetime.SubstringNE(0, 8);
				twm0c["EXPIRY_DATE"] = expiry_date;
				twm0c["VEHICLE_NUM"] = vehicle_num;
				twm0c["WEIGHT_APP"] = weight_app;
				twm0c["APP_REMARK"] = app_remark;
				twm0c["PONDER_MARK"] = pond_flag;
				twm0c["BACK_C1"] = truck_model; //车型
				twm0c["SHIFT_NO"] = prod_shift;
				twm0c["TRANSFER_PLAN_NO"] = transfer_plan_no;
				twm0c["BALANCE_CODE"] = balance_code;
				twm0c["BACK_C2"] = annt_name; //联系人
				twm0c["BACK_C3"] = tel; //联系方式
				twm0c["BACK_C4"] = back_c1; //联系方式
				twm0c.TrimOrBlank();
				twm0c.Insert();
			}

			if (oper_type.Trim() == "D")
			{
				loading_plan_no = v_loading_plan_no;
				twm0c["LOADING_PLAN_NO"] = loading_plan_no;
				twm0d["LOADING_PLAN_NO"] = loading_plan_no;
				twm0c.Query("LOADING_PLAN_NO");

				if (twm0c["SHIFT_NO"].ToString().Trim() == "Y")
				{
					sprintf(s.msg, "此用车计划[" + loading_plan_no + "]运输过钢材，不能取消!");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				if (twm0c["PRO_FLAG"].ToString().Trim() != "C" && twm0c["EXPIRY_DATE"].ToString().Trim() >= datetime.Substring(0, 8))
				{
					twm0c.Reset();
					twm0c["REC_ERASOR"] = s.userid;
					twm0c["REC_ERASE_TIME"] = datetime;
					twm0c["PRO_FLAG"] = oper_type;
					twm0c["LOADING_PLAN_NO"] = loading_plan_no;
					twm0c.Update("PRO_FLAG,REC_ERASOR,REC_ERASE_TIME", "LOADING_PLAN_NO");

					twm0d.Reset();
					twm0d["REC_ERASOR"] = s.userid;
					twm0d["REC_ERASE_TIME"] = datetime;
					twm0d["PRO_FLAG"] = oper_type;
					twm0d["LOADING_PLAN_NO"] = loading_plan_no;
					twm0d.Update("PRO_FLAG,REC_ERASOR,REC_ERASE_TIME", "LOADING_PLAN_NO");
					/*if (twm0d.QueryCount("LOADING_PLAN_NO") > 0)
					{
					sprintf(s.msg, "该计划号物流已反馈车号不允许用车申请取消！");
					throw CApplicationException(-1, s.msg, log.Location);
					}*/
				}
				else{
					sprintf(s.msg, "该计划已被关闭或已过有效日期！！");
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			else if (oper_type.Trim() == "C")
			{
				loading_plan_no = v_loading_plan_no;
				twm0c.Reset();
				twm0c["REC_ERASOR"] = s.userid;
				twm0c["REC_ERASE_TIME"] = datetime;
				twm0c["PRO_FLAG"] = oper_type;
				twm0c["LOADING_PLAN_NO"] = loading_plan_no;
				twm0c.Update("PRO_FLAG,REC_ERASOR,REC_ERASE_TIME", "LOADING_PLAN_NO");

				twm0d.Reset();
				twm0d["REC_ERASOR"] = s.userid;
				twm0d["REC_ERASE_TIME"] = datetime;
				twm0d["PRO_FLAG"] = oper_type;
				twm0d["LOADING_PLAN_NO"] = loading_plan_no;
				twm0d.Update("PRO_FLAG,REC_ERASOR,REC_ERASE_TIME", "LOADING_PLAN_NO");
			}

			if (oper_type.Trim() == "C")
			{
				bcls_rec_send.Tables[0].Rows[0]["DEAL_FLAG"] = oper_type;
				bcls_rec_send.Tables[0].Rows[0]["PLAN_NO"] = loading_plan_no;
			}
			else
			{

				/*sqlstr = "SELECT BALANCE_CODE,EXPIRY_DATE FROM TWM0C WHERE LOADING_PLAN_NO = @loading_plan_no";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("loading_plan_no", loading_plan_no);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					bcls_rec_send.Tables[0].Rows[0]["BALANCE_CODE"] = cmd_inq.GetString(1);
					bcls_rec_send.Tables[0].Rows[0]["EXPIRY_DATE"] = cmd_inq.GetString(2) += "235959";
				}
				cmd_inq.Close();

				bcls_rec_send.Tables[0].Rows[0]["DEAL_FLAG"] = oper_type;
				bcls_rec_send.Tables[0].Rows[0]["PLAN_NO"] = loading_plan_no;
				bcls_rec_send.Tables[0].Rows[0]["LOAD_CODE_AREA"] = stock_no;
				bcls_rec_send.Tables[0].Rows[0]["UNLOAD_CODE_AREA"] = aim_stock_no;
				bcls_rec_send.Tables[0].Rows[0]["PLAN_WGNUM"] = vehicle_num;
				bcls_rec_send.Tables[0].Rows[0]["PLAN_QTY"] = weight_app;
				bcls_rec_send.Tables[0].Rows[0]["PROD_CODE"] = prod_code;
				bcls_rec_send.Tables[0].Rows[0]["PROD_CNAME"] = prod_cname;
				bcls_rec_send.Tables[0].Rows[0]["S_USERID"] = s_userid;
				bcls_rec_send.Tables[0].Rows[0]["TRUCK_MODEL"] = truck_model;
				bcls_rec_send.Tables[0].Rows[0]["ANNT_NAME"] = annt_name;
				bcls_rec_send.Tables[0].Rows[0]["TEL"] = tel;
				bcls_rec_send.Tables[0].Rows[0]["BACK_C1"] = back_c1;
				bcls_rec_send.Tables[0].Rows[0]["APP_REMARK"] = app_remark;*/

			}

			/*doFlag = f_cm_7z8t01_snd(&bcls_rec_send, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}*/

		}
		else if (oper_type.Trim() == "M")
		{
			Log::Trace("", __FUNCTION__, "模拟配车");

			if (bcls_rec->Tables[0].Columns.Contains("TRUCK_NO"))
				truck_no = bcls_rec->Tables[0].Rows[0]["TRUCK_NO"].ToString();

			if (bcls_rec->Tables[0].Columns.Contains("APP_DATE"))
				app_date = bcls_rec->Tables[0].Rows[0]["APP_DATE"].ToString();

			if (bcls_rec->Tables[0].Columns.Contains("PRO_FLAG"))
				pro_flag = bcls_rec->Tables[0].Rows[0]["PRO_FLAG"].ToString();

			if (truck_no.Trim() == "")//车号
			{
				sprintf(s.msg, "车号不能为空！");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (pro_flag.Trim() != "I")
			{
				sprintf(s.msg, "计划非申请状态，不能配车！");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			
			
			twm0d.TrimOrBlank();
			twm0d["REC_CREATOR"] = s.userid;
			twm0d["REC_CREATE_TIME"] = datetime;
			twm0d["PRO_FLAG"] = "I";
			twm0d["LOADING_PLAN_NO"] = v_loading_plan_no;
			twm0d["STOCK_NO"] = stock_no;
			twm0d["AIM_STOCK_NO"] = aim_stock_no;
			twm0d["TRUCK_NO"] = truck_no;
			twm0d["DATE_TIME"] = app_date;
			twm0d["EXPIRY_DATE"] = expiry_date.SubstringNE(0, 8);
			twm0d["VEHICLE_NUM"] = 1;

			//twm0d.Print();
			twm0d.Insert();

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
	return doFlag;

}

