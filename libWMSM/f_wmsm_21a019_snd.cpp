/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      05501061
Version:     1.0
Date:        2023-11-22 10:45:20
Description: 非销售出厂计划发送
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2_FUNCTION_EXPORT


int f_wmsm_21a019_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	int blkNum = 0;
	int i = 0;
	int count = 0;
	CString deal_flag = " ";
	CString plan_no = "";
	CString plan_type = "";
	CString sqlstr = "";
	//电文号
	CString cs_tc_no("");
	//电文变量
	EPEX epex(&s, conn);
	/*实体类定义*/
	CModel twmsm30("TWMSM30");
	CModel twmsm30m("TWMSM30M");
	/*数据库操作类定义*/
	CDbCommand cmd_inq(conn);
	try
	{
		blkNum = bcls_rec->Tables.IndexOf("21A019");
		if (blkNum < 0)
		{
			strcpy(s.msg, "传入数据块21A019不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (bcls_rec->Tables["21A019"].Columns.Contains("DEAL_FLAG"))
		{
			deal_flag = bcls_rec->Tables["21A019"].Rows[0]["DEAL_FLAG"];
		}
		count = bcls_rec->Tables["21A019"].Rows.get_Count();
		for (int a = 0; a < count; a++)
		{
			plan_no = bcls_rec->Tables["21A019"].Rows[a]["PLAN_NO"];
			twmsm30["PLAN_NO"] = plan_no;
			twmsm30.Query("PLAN_NO");
			plan_type = "1";//1：非销售出厂-委外加工2：非采购进厂3:备件修理4：临钢废钢回收
			cs_tc_no = "21A019";
			if (epex.Initialize(cs_tc_no) < 0)
			{
				strncpy(s.msg, (const char*)"电文初始化失败", sizeof(s.msg) - 1);
				s.flag = -1;
				doFlag = -1;
				return doFlag;
			}
			if (epex.SetValue("21A019",0, twmsm30) < 0)
			{
				sprintf(s.msg, "系统出现异常，电文拼接出错");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (epex.SetValue("21A019", "deal_flag", 0, deal_flag) < 0)
			{
				sprintf(s.msg, "系统出现异常，DEAL_FLAG电文拼接出错");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (epex.SetValue("21A019", "FACTORY_DIV", 0, twmsm30["FACTORY_DIV1"].ToString()) < 0)
			{
				sprintf(s.msg, "系统出现异常，FACTORY_DIV电文拼接出错");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (epex.SetValue("21A019", "ULPLACE", 0, twmsm30["LOAD_CODE"].ToString()) < 0)
			{
				sprintf(s.msg, "系统出现异常，ULPLACE电文拼接出错");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (epex.SetValue("21A019", "AREA_CODE", 0, twmsm30["LOAD_CODE_AREA"].ToString()) < 0)
			{
				sprintf(s.msg, "系统出现异常，ULPLACE电文拼接出错");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			sqlstr = "select * from twmsm30m t where t.plan_no='" + plan_no + "'";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			while (cmd_inq.Read())
			{
				twmsm30m.Reset();
				cmd_inq.Fetch(twmsm30m);
				
				if (epex.SetValue("21A019_1","MAT_NO", i, twmsm30m["MAT_NO"].ToString()) < 0)//材料号
				{
					sprintf(s.msg, "系统出现异常，MAT_NO电文拼接出错");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				
				if (epex.SetValue("21A019_1", "SG_SIGN", i, twmsm30m["SG_SIGN"].ToString()) < 0)//牌号
				{
					sprintf(s.msg, "系统出现异常，SG_SIGN电文拼接出错");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (epex.SetValue("21A019_1", "LENGTH", i, twmsm30m["MAT_LEN"].ToString()) < 0)//长度
				{
					sprintf(s.msg, "系统出现异常，LENGTH电文拼接出错");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (epex.SetValue("21A019_1", "WIDTH", i, twmsm30m["MAT_WIDTH"].ToString()) < 0)//宽度
				{
					sprintf(s.msg, "系统出现异常，DEAL_FLAG电文拼接出错");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (epex.SetValue("21A019_1", "THICK", i, twmsm30m["MAT_THICK"].ToString()) < 0)//厚度
				{
					sprintf(s.msg, "系统出现异常，THICK电文拼接出错");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (epex.SetValue("21A019_1", "WEIGHT", i, twmsm30m["MAT_WT"].ToString()) < 0)//重量
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


