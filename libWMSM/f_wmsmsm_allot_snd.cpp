/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:      李振
Version:     1.0
Date:        2016-03-25 11:35:08
Description: 调拨单发送
**************************************************/

#include "stdafx.h"
#include "epex.h"
#include<vector>
int f_wmsm_t80ry0_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);


BM2_FUNCTION_EXPORT
int f_wmsmsm_allot_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	EPEX epex;

	int doFlag = 0;
	int ret = 0;
	CString s_tc_no = " ";
	CString sqlstr = "";
	CString sh_fac = "";
	CString sh_area = "";
	CString c_mat_destion = " ";
	

	//定义表实体对象 
	CModel twm41dj("TWM41DJ");
	CModel tmmsm01("TMMSM01");

	CDbCommand comm(conn);

	try
	{
		sh_fac = bcls_rec->Tables[0].Rows[0]["C_ACCEPTDEPT"];
		sh_area = bcls_rec->Tables[0].Rows[0]["C_ACCEPTSTOCK"];
		if (sh_fac == "6210")//一厂碳线
		{
			s_tc_no = "T8V001";
		}
		else if (sh_fac == "6220")//二厂南区
		{
			s_tc_no = "T8T701";
		}
		else if (sh_fac == "6230")//一厂不锈钢
		{
			s_tc_no = "T8U001";
		}
		else if (sh_fac == "6310")//型材-60
		{
			s_tc_no = "2160YX";
		}
		else if (sh_fac == "6320")//线材-70
		{
			s_tc_no = "2170YX";
		}
		else if (sh_fac == "6340")//中板-50
		{
			s_tc_no = "2150YX";
		}
		else if (sh_fac == "6350")//1549-30
		{
			s_tc_no = "2130YX";
		}
		else if (sh_fac == "6360")//2250
		{
			if (sh_area == "6361")
			{
				s_tc_no = "T8P304";
			}
			else if (sh_area == "6364")
			{
				s_tc_no = "T8G001";
			}
			
		}
		else if (sh_fac == "6380")//临钢
		{
			
		}
		else if (sh_fac == "6390")//4300-
		{
			s_tc_no = "T82401";
		}
		else if (sh_fac == "6240")//逆送
		{
			sh_fac = bcls_rec->Tables[0].Rows[0]["C_SENDDEPT"];
			if (sh_fac == "6210")//一厂碳线
			{
				s_tc_no = "T8V001";
			}
			else if (sh_fac == "6220")//二厂南区
			{
				s_tc_no = "T8T701";
			}
			else if (sh_fac == "6230")//一厂不锈钢
			{
				s_tc_no = "T8U001";
			}
			else if (sh_fac == "6310")//型材-60
			{
				s_tc_no = "2160YX";
			}
			else if (sh_fac == "6320")//线材-70
			{
				s_tc_no = "2170YX";
			}
			else if (sh_fac == "6340")//中板-50
			{
				s_tc_no = "2150YX";
			}
			else if (sh_fac == "6350")//1549-30
			{
				s_tc_no = "2130YX";
			}
			else if (sh_fac == "6360")//2250
			{
				if (sh_area == "6361")
				{
					s_tc_no = "T8P304";
				}
				else if (sh_area == "6364")
				{
					s_tc_no = "T8G001";
				}

			}
			else if (sh_fac == "6380")//临钢
			{
				return 0;
			}
			else if (sh_fac == "6390")//4300-
			{
				s_tc_no = "T82401";
			}
			else {
				sprintf(s.sysmsg, "初始化电文失败", (const char*)s_tc_no);
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}
		else {
			sprintf(s.sysmsg, "初始化电文失败", (const char*)s_tc_no);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		Log::Trace("", __FUNCTION__, "line=[{0}]", __LINE__);
		//初始化
		if (sh_fac == "6380")
		{
			EIClass dbsq;
			dbsq.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
			dbsq.Tables[0].Columns.Add(DT_STRING, "C_DELIVERYID");
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				twm41dj.Reset();
				twm41dj.MergeFrom(bcls_rec->Tables[0].Rows[i]);
				dbsq.Tables[0].Rows.Clear();
				dbsq.Tables[0].Rows.Add();
				dbsq.Tables[0].Rows[0]["MAT_NO"] = twm41dj["C_BATCHUNIT"];
				dbsq.Tables[0].Rows[0]["C_DELIVERYID"] = twm41dj["C_DELIVERYID"];
				doFlag = f_wmsm_t80ry0_snd(&dbsq, bcls_ret, conn);
				if (doFlag != 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			
			
			return 0;
		}
		else
		{
			ret = epex.Initialize(s_tc_no);
			if (ret < 0)
			{
				CFormattable arguments[] = { s_tc_no }; // 定义参数列表的数组
				CMessageFormat::Format(s.msg, _RES("YM00S0000514")/*初始化电文[{0}]失败。*/, arguments, 1);
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				twm41dj.MergeFrom(bcls_rec->Tables[0].Rows[i]);

				Log::Trace("", __FUNCTION__, "line=[{0}]", __LINE__);
				if (s_tc_no == "T8P304" || s_tc_no == "T82401" || s_tc_no == "T8U001" || s_tc_no == "T8V001" || s_tc_no == "T8T701")
				{
					//拼电文数据
					if (epex.SetValue("MES_MM_GM", 0, twm41dj) < 0)
					{
						strcpy(s.msg, _RES("GCRSS0000015"));//系统出现异常，电文拼接出错，请联系系统维护人员。
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
					if (s_tc_no == "T82401")
					{
						if (epex.SetValue("MES_MM_GM_RFC", "MSGTYPE", 0, "MES_MM_GM_RFC") < 0
							|| epex.SetValue("MES_MM_GM_RFC", "input_t_name", 0, "MES_MM_GM_RFC") < 0
							|| epex.SetValue("MES_MM_GM_RFC", "output_t_name", 0, "MES_MM_GM_RFC") < 0
							|| epex.SetValue("MES_MM_GM_RFC", "input_t_rout1", 0, twm41dj["C_SENDDEPT"].ToString() + "_" + twm41dj["C_SENDSTOCK"].ToString()) < 0
							|| epex.SetValue("MES_MM_GM_RFC", "output_t_rout2", 0, twm41dj["C_ACCEPTDEPT"].ToString() + "_" + twm41dj["C_ACCEPTSTOCK"].ToString()) < 0)
						{
							strcpy(s.msg, _RES("GCRSS0000015"));//系统出现异常，电文拼接出错，请联系系统维护人员。
							throw CApplicationException(-1, s.msg, s.svc_name);
						}
					}
					else
					{
						if (epex.SetValue("MES_MM_GM_RFC", "MSGTYPE", 0, "MES_MM_GM_RFC") < 0
							|| epex.SetValue("MES_MM_GM_RFC", "it_name", 0, "MES_MM_GM_RFC") < 0
							|| epex.SetValue("MES_MM_GM_RFC", "ot_name", 0, "MES_MM_GM_RFC") < 0
							|| epex.SetValue("MES_MM_GM_RFC", "it_rout1", 0, twm41dj["C_SENDDEPT"].ToString() + "_" + twm41dj["C_SENDSTOCK"].ToString()) < 0
							|| epex.SetValue("MES_MM_GM_RFC", "ot_rout1", 0, twm41dj["C_ACCEPTDEPT"].ToString() + "_" + twm41dj["C_ACCEPTSTOCK"].ToString()) < 0)
						{
							strcpy(s.msg, _RES("GCRSS0000015"));//系统出现异常，电文拼接出错，请联系系统维护人员。
							throw CApplicationException(-1, s.msg, s.svc_name);
						}
					}
					vector<CString> date_l = { "D_BILLDATE", "D_OPERATIONDATE", "T_ACCEPTTIME", "C_CLOSEGATETIME", "T_UPLOADTIME", "T_OUTSTOCKTIME",
						"T_INSTOCKTIME","T_SALESCOMFIRMTIME","T_OVERRULETIME","D_REQUIREDATE" };
					CString CODE_NAME = "";
					CString C_V = "";
					for (int i = 0; i < date_l.size(); i++)
					{
						Log::Trace("", __FUNCTION__, "twm41djToString()=[{0}][{1}]", twm41dj[date_l[i]].ToString(), (const char*)date_l[i]);
						if (twm41dj[date_l[i]].ToString().GetLength() == 14)
						{
							Log::Trace("", __FUNCTION__, "ghgtf=[{0}]", twm41dj[date_l[i]].ToString());
							Log::Trace("", __FUNCTION__, "ghgtf=[{0}]", twm41dj[date_l[i]].ToString().Substring(0, 10));
							Log::Trace("", __FUNCTION__, "ghgtf=[{0}]", twm41dj[date_l[i]].ToString().Substring(10, 2));
							Log::Trace("", __FUNCTION__, "ghgtf=[{0}]", twm41dj[date_l[i]].ToString().Substring(12, 2));
							CODE_NAME = date_l[i];
							C_V = twm41dj[date_l[i]].ToString().Substring(0, 10) + ":"
								+ twm41dj[date_l[i]].ToString().Substring(10, 2) + ":" + twm41dj[date_l[i]].ToString().Substring(12, 2);
							Log::Trace("", __FUNCTION__, "CODE_NAME=[{0}]C_V=[{1}]", CODE_NAME, C_V);
							if (epex.SetValue("MES_MM_GM", CODE_NAME, 0, C_V) < 0)
							{
								strcpy(s.msg, _RES("GCRSS0000015"));//系统出现异常，电文拼接出错，请联系系统维护人员。
								throw CApplicationException(-1, s.msg, s.svc_name);
							}
						}
					}
					//if (twm41dj["D_BILLDATE"].ToString().GetLength()==14)//制单日期
					//{
					//	if (epex.SetValue("MES_MM_GM", "D_BILLDATE", 0, twm41dj["D_BILLDATE"].ToString().Substring(0,10)+":"
					//		+ twm41dj["D_BILLDATE"].ToString().Substring(10, 2) + ":" + twm41dj["D_BILLDATE"].ToString().Substring(12, 2)) < 0)
					//	{
					//		strcpy(s.msg, _RES("GCRSS0000015"));//系统出现异常，电文拼接出错，请联系系统维护人员。
					//		throw CApplicationException(-1, s.msg, s.svc_name);
					//	}
					//}
					if (s_tc_no == "T8T701")
					{
						tmmsm01["MAT_NO"] = twm41dj["C_BATCHUNIT"];
						tmmsm01.Query("MAT_NO");
						if (tmmsm01["MAT_DESTION"].ToString() == "10")
							c_mat_destion = "1549热轧去向";
						else if (tmmsm01["MAT_DESTION"].ToString() == "11")
							c_mat_destion = "2250热轧去向";
						else if (tmmsm01["MAT_DESTION"].ToString() == "20")
							c_mat_destion = "型材厂去向";
						else if (tmmsm01["MAT_DESTION"].ToString() == "30")
							c_mat_destion = "不锈线材去向";
						else if (tmmsm01["MAT_DESTION"].ToString() == "40")
							c_mat_destion = "不锈热轧去向";
						else if (tmmsm01["MAT_DESTION"].ToString() == "41")
							c_mat_destion = "4300厚板去向";
						else if (tmmsm01["MAT_DESTION"].ToString() == "50")
							c_mat_destion = "外卖去向";
						else if (tmmsm01["MAT_DESTION"].ToString() == "60")
							c_mat_destion = "二钢南区去向";
						else
							c_mat_destion = " ";
						
						if (epex.SetValue("MES_MM_GM", "C_RESERVECOL2", 0, tmmsm01["CASTING_PRE_JUDGMENT"].ToString()) < 0
							|| epex.SetValue("MES_MM_GM", "C_APPLYUNIT", 0, tmmsm01["APN"].ToString()) < 0
							|| epex.SetValue("MES_MM_GM", "C_RESERVECOL1", 0, c_mat_destion) < 0)
						{
							strcpy(s.msg, _RES("GCRSS0000015"));//系统出现异常，电文拼接出错，请联系系统维护人员。
							throw CApplicationException(-1, s.msg, s.svc_name);
						}
					}
				}
				else
				{
					Log::Trace("", __FUNCTION__, "line=[{0}]", __LINE__);
					//拼电文数据
					if (epex.SetValue(0, twm41dj) < 0)
					{
						strcpy(s.msg, _RES("GCRSS0000015"));//系统出现异常，电文拼接出错，请联系系统维护人员。
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
					Log::Trace("", __FUNCTION__, "line=[{0}]", __LINE__);
				}


				Log::Trace("", __FUNCTION__, "line=[{0}]", __LINE__);

				if (epex.SendTele() < 0)
				{
					strcpy(s.msg, _RES("GCRSS0000032")/*电文发送失败。*/);
					sprintf(s.sysmsg, "[%s]发送失败", (const char*)s_tc_no);
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				Log::Trace("", __FUNCTION__, "line=[{0}]", __LINE__);

			}




			epex.Uninitialize();
		}
		

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000021")/*信息读取失败。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1; //数据库异常时返回-1，事务将被回滚
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

