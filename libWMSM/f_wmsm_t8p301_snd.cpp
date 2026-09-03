/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      KE2111
Version:     1.0
Date:        2023-11-20 14:28:44
Description: 板坯信息电文
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_mmsm99(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);

int f_wmsm_t8p301_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString sqlstr1 = " ";

	//电文号
	CString cs_tc_no("");
	//电文变量
	EPEX epex(&s, conn);

	/* 实体类定义 */
	CModel tmmsm01 = CModel("TMMSM01");
	CModel tpssm03 = CModel("TPSSM03");
	CModel tqmts29 = CModel("TQMTS29");
	CModel tmmsm96("TMMSM96");
	//
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);

	//系统当前时间
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	EIClass mm0099;
	mm0099.Tables[0].set_TableName("MM0099");
	mm0099.Tables[0].Columns.Add(tmmsm96);
	mm0099.Tables[0].Rows.Clear();
	try
	{
		
		Log::Info("", __FUNCTION__, "line   =[{0}]", __LINE__);
		cs_tc_no = "T8P301";
		Log::Info("", __FUNCTION__, "line   =[{0}]", __LINE__);
		
		Log::Info("", __FUNCTION__, "line   =[{0}]", __LINE__);
		//电文初始化
		if (epex.Initialize(cs_tc_no) < 0)
		{
			Log::Info("", __FUNCTION__, "line   =[{0}]", __LINE__);
			strncpy(s.msg, (const char*)"电文初始化失败", sizeof(s.msg) - 1);
			s.flag = -1;
			doFlag = -1;
			return doFlag;
		}
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			
			Log::Info("", __FUNCTION__, "line   =[{0}]", __LINE__);
			tmmsm01.Reset();
			tmmsm01["MAT_NO"] = bcls_rec->Tables[0].Rows[i]["MAT_NO"].ToString();
			tmmsm01.Query("MAT_NO");

			//20240516  成品坯不发  mfj  
			if (tmmsm01["PRODUCT_FLAG"].ToString().Trim() == "1")
			{
				continue;
			}
			CString old_if_hr = Db::QueryCString(" select CODE_DESC_3_CONTENT from TWMSMZD02 where CODE_CLASS = 'WM02' and CODE =(select GUIDE_DEST from tmmsm01 where MAT_NO='" + tmmsm01["MAT_NO"].ToString() + "') ");
			if (old_if_hr.Find("1") < 0)
			{
				continue;
			}

			tmmsm96.Reset();
			tmmsm96.CopyFrom(tmmsm01);
			tmmsm96["HR_SEND_FLAG"] = "1";//1--发送0--取消
			tmmsm96["SENDTOHRTIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tmmsm96["EVENT_ID"] = "MM79";
			tmmsm96["SYSTEM_ID"] = "MMSM";
			tmmsm96["EVENT_LINE_TYPE"] = "00";
			tmmsm96["FUNC_ID"] = s.svc_name;
			tmmsm96.MergeTo(mm0099.Tables["MM0099"], false);

			if (tmmsm01["PONO_SLAB"].ToString().Trim() != "")
			{
				tpssm03["SLAB_NO"] = tmmsm01["PONO_SLAB"];
				tpssm03.Query("SLAB_NO");
			}
			Log::Info("", __FUNCTION__, "line   =[{0}]", __LINE__);
			if (tmmsm01["SLAB_CUT_TIME"].ToString().Trim()=="")
			{
				tmmsm01["SLAB_CUT_TIME"] = datetime;
			}
			if (epex.SetValue("VirtSlabID", 0, tmmsm01["LSLAB_NO"].ToString()) < 0//虚拟板坯号
				|| epex.SetValue("MarkerNumber", 0, tmmsm01["SLAB_NO"].ToString()) < 0//板坯号
				|| epex.SetValue("PONumber", 0, tmmsm01["ORDER_NO"].ToString()) < 0//合同号
				|| epex.SetValue("SlabCuttingTime", 0, tmmsm01["SLAB_CUT_TIME"].ToString().Substring(0,10)+":"+ tmmsm01["SLAB_CUT_TIME"].ToString().Substring(10, 2)+":"+ 
					tmmsm01["SLAB_CUT_TIME"].ToString().Substring(12, 2)) < 0//切割时间
				|| epex.SetValue("MatNo", 0, tmmsm01["MAT_CODE"].ToString()) < 0//物料编码
				|| epex.SetValue("BatchNumber", 0, tmmsm01["BATCH"].ToString()) < 0//材料号
				|| epex.SetValue("SteelGrade", 0, tmmsm01["ST_NO"].ToString()) < 0//内部钢种
				|| epex.SetValue("Width", 0, tmmsm01["MAT_WIDTH"].ToString()) < 0//宽
				|| epex.SetValue("WidthTail", 0, tmmsm01["SLAB_TAIL_WIDTH"].ToString()) < 0//尾宽
				|| epex.SetValue("WidthHead", 0, tmmsm01["SLAB_HEAD_WIDTH"].ToString()) < 0//头宽
				|| epex.SetValue("TaperSlab", 0, tmmsm01["ADJUST_WIDTH_MARK"].ToString()) < 0//调宽标记
				|| epex.SetValue("TaperWidthStart", 0, tmmsm01["SLAB_TAPER_WIDTH_START"].ToString()) < 0//调宽位置
				|| epex.SetValue("Taperlength", 0, tmmsm01["SLAB_TAPER_WIDTHLENGTH"].ToString()) < 0//调宽长度
				|| epex.SetValue("Lengh", 0, tmmsm01["MAT_LEN"].ToString()) < 0//长
				|| epex.SetValue("Thickness", 0, tmmsm01["MAT_THICK"].ToString()) < 0//厚
				|| epex.SetValue("Weight", 0, tmmsm01["MAT_WT"].ToDecimal()*1000) < 0//重量
				|| epex.SetValue("ChemistryMode", 0, "1") < 0//成分模式
				|| epex.SetValue("CCUSAGE", 0, tpssm03["APN"].ToString()) < 0//用途
				|| epex.SetValue("SURFACEGRINDINGMANNER", 0,"1") < 0//表面
				|| epex.SetValue("CX_MAT_No", 0, tmmsm01["MAT_NO"].ToString()) < 0//材料号
				|| epex.SetValue("SG_GRADE", 0, tmmsm01["SG_GRADE_1"].ToString()) < 0//钢种
				|| epex.SetValue("length", 0,405) < 0
				|| epex.SetValue("ID", 0, 0) < 0
				|| epex.SetValue("des", 0, tmmsm01["LGORT"].ToString()) < 0
				|| epex.SetValue("time", 0, datetime.Substring(0, 10) + ":" + datetime.Substring(10, 2) + ":" +
					datetime.Substring(12, 2)) < 0
				)
			{
				sprintf(s.msg, "发送电文失败，原因[%s]", epex.GetMsg());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			Log::Info("", __FUNCTION__, "line   =[{0}]", __LINE__);
			EIClass TEMP;
			sqlstr = " select ELM_CODE,ELM_ACT from tqmts29 where HEAT_NO='" + tmmsm01["HEAT_NO"].ToString() + "' ";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteQuery(TEMP.Tables[0]);
			cmd_inq.Close();
			int yuansu = 0;
			Log::Info("", __FUNCTION__, "line   =[{0}]", __LINE__);
			sqlstr = " select TC_ITEM_NAME,REMARK from TEXT2 where TC_NO='T8P301' AND REMARK!=' ' ";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			while (cmd_inq.Read())
			{
				yuansu = 0;
				//Log::Info("", __FUNCTION__, "line   =[{0}]", __LINE__, TEMP.Tables[0].Rows[i]["ELM_ACT"].ToDecimal());
				for (int i = 0; i < TEMP.Tables[0].Rows.get_Count(); i++) {
					if (cmd_inq.GetString(2) == TEMP.Tables[0].Rows[i]["ELM_CODE"].ToString()) {
						if (epex.SetValue(cmd_inq.GetString(1), 0, (TEMP.Tables[0].Rows[i]["ELM_ACT"].ToDecimal()*1000).Round(0)) < 0) {
							sprintf(s.msg, "发送电文失败，原因[%s]", epex.GetMsg());
							throw CApplicationException(-1, s.msg, s.svc_name);
						}
						yuansu++;
					}
				}
				if (yuansu == 0)//说明没有该元素
				{
					if (epex.SetValue(cmd_inq.GetString(1), 0, 0) < 0) {
						sprintf(s.msg, "发送电文失败，原因[%s]", epex.GetMsg());
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
				}
			}
			cmd_inq.Close();

			Log::Info("", __FUNCTION__, "line   =[{0}]", __LINE__);

			//电文发送
			if (epex.SendTele() < 0)
			{
				strncpy(s.msg, (const char*)"电文发送失败", sizeof(s.msg) - 1);
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			Log::Info("", __FUNCTION__, "line   =[{0}]", __LINE__);
			Log::Info("", __FUNCTION__, "line   =[{0}]", __LINE__);

		
			Log::Info("", __FUNCTION__, "line   =[{0}]", __LINE__);
		}

		if (mm0099.Tables[0].Rows.get_Count() > 0)
		{
			doFlag = f_mmsm99(&mm0099, bcls_ret, conn);
			if (doFlag < 0) {
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}
		epex.Uninitialize();//释放
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


