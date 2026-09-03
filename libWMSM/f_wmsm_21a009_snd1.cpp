/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2024
Author:      05501061
Version:     1.0
Date:        2024-02-20 17:05:54
Description: 临钢坯装车实绩发送
**************************************************/

#include "stdafx.h"
#include "epex.h"
BM2_FUNCTION_EXPORT


int f_wmsm_21a009_snd1(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	int i = 0;
	int blkNum = 0;
	int count = 0;
	CString deal_flag = " ";
	CString plan_no = ""; 
	CString mission_no = "";
	CString trans_type = "";
	CString sqlstr = "";
	CString datetime = CDateTime::Now().ToString("yyyyMMddhhmmss");	//取系统时间
	//电文号
	CString cs_tc_no("");
	//电文变量
	EPEX epex(&s, conn);
	/*实体类定义*/
	CModel twmsm32("TWMSM32");
	CModel twmsm32m("TWMSM32M");
	/*数据库操作类定义*/
	CDbCommand cmd_inq(conn);
	try
	{
		blkNum = bcls_rec->Tables.IndexOf("21A009");
		if (blkNum < 0)
		{
			strcpy(s.msg, "传入数据块21A009不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (bcls_rec->Tables["21A009"].Columns.Contains("DEAL_FLAG"))
		{
			deal_flag = bcls_rec->Tables["21A009"].Rows[0]["DEAL_FLAG"];
		}
		count = bcls_rec->Tables["21A009"].Rows.get_Count();
		for (int a = 0; a < count; a++)
		{
			plan_no = bcls_rec->Tables["21A009"].Rows[a]["PLAN_NO"];
			mission_no = bcls_rec->Tables["21A009"].Rows[a]["MISSION_NO"];
			twmsm32["PLAN_NO"] = plan_no;
			twmsm32["MISSION_NO"] = mission_no;
			twmsm32.Query("PLAN_NO,MISSION_NO");
			trans_type = "5";//5：临钢坯
			cs_tc_no = "21A009";
			if (epex.Initialize(cs_tc_no) < 0)
			{
				strncpy(s.msg, (const char*)"电文初始化失败", sizeof(s.msg) - 1);
				s.flag = -1;
				doFlag = -1;
				return doFlag;
			}
			if (epex.SetValue("ZCHO_ZCSJ",0, twmsm32) < 0)
			{
				sprintf(s.msg, "系统出现异常，电文拼接出错");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (epex.SetValue("ZCHO_ZCSJ", "PRACTICE_NO", 0, mission_no) < 0)
			{
				sprintf(s.msg, "系统出现异常，DEAL_FLAG电文拼接出错");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
				if (epex.SetValue("ZCHO_ZCSJ", "DEAL_FLAG", 0, deal_flag) < 0)
				{
				sprintf(s.msg, "系统出现异常，DEAL_FLAG电文拼接出错");
				throw CApplicationException(-1, s.msg, s.svc_name);
				}
			if (epex.SetValue("ZCHO_ZCSJ", "TRANS_TYPE", 0, trans_type) < 0)
			{
				sprintf(s.msg, "系统出现异常，TRANS_TYPE电文拼接出错");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (epex.SetValue("ZCHO_ZCSJ", "LOAD_END_TIME", 0, datetime) < 0)
			{
				sprintf(s.msg, "系统出现异常，LOAD_END_TIME电文拼接出错");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			sqlstr = "select * from twmsm32m t where t.plan_no='" + plan_no + "' and t.mission_no='" + mission_no + "'";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			while (cmd_inq.Read())
			{
				twmsm32m.Reset();
				cmd_inq.Fetch(twmsm32m);
				if (epex.SetValue("ZCHO_ZCSJ1", "MATERIAL_CODE", i, "") < 0)//物料代码
				{
					sprintf(s.msg, "系统出现异常，MATERIAL_CODE电文拼接出错");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (epex.SetValue("ZCHO_ZCSJ1", "MATERIAL_NAME", i, "") < 0)//物料名称
				{
					sprintf(s.msg, "系统出现异常，MATERIAL_NAME电文拼接出错");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				
				if (epex.SetValue("ZCHO_ZCSJ1", "MAT_NO", i, twmsm32m["MAT_NO"].ToString()) < 0)//材料号
				{
					sprintf(s.msg, "系统出现异常，MAT_NO电文拼接出错");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				
				if (epex.SetValue("ZCHO_ZCSJ1", "SG_SIGN", i, twmsm32m["SG_SIGN"].ToString()) < 0)//牌号
				{
					sprintf(s.msg, "系统出现异常，SG_SIGN电文拼接出错");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (epex.SetValue("ZCHO_ZCSJ1", "LENGTH", i, twmsm32m["MAT_LEN"].ToString()) < 0)//长度
				{
					sprintf(s.msg, "系统出现异常，LENGTH电文拼接出错");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (epex.SetValue("ZCHO_ZCSJ1", "WIDTH", i, twmsm32m["MAT_WIDTH"].ToString()) < 0)//宽度
				{
					sprintf(s.msg, "系统出现异常，DEAL_FLAG电文拼接出错");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (epex.SetValue("ZCHO_ZCSJ1", "THICK", i, twmsm32m["MAT_THICK"].ToString()) < 0)//厚度
				{
					sprintf(s.msg, "系统出现异常，THICK电文拼接出错");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (epex.SetValue("ZCHO_ZCSJ1", "WEIGHT", i, twmsm32m["MAT_WT"].ToString()) < 0)//重量
				{
					sprintf(s.msg, "系统出现异常，WEIGHT电文拼接出错");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				i++;
			}
			cmd_inq.Close();
			if (epex.SendTele() < 0)
			{
				sprintf(s.msg, "电文发送失败");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			epex.Uninitialize();
		}

	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}


