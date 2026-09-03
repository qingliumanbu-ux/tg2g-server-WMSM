/* **************************************************************************
*	Copyright (c) Baosight Corporation 2008 . All Rights Reserved.
*  	BM2PES 宝信生产执行系统
*****************************************************************************
*  程序名称			: f_wm_cmd_make
*  程序描述			: 指令生成函数
*  备注说明			:
*  修改历史			:
*  		2022-09-19 仓库产品化			(ADD)程序建立
*			... ...
* **************************************************************************** */
/* ***************************传入参数********************************
传入块名：WM_CMD
MAT_NO                     材料号                  非空
STOCK_OPER_ORDER           库业务类型              非空
STOCK_PLACE_NO_TO          目标库位号              非空
STOCK_NO_TO                目标库区                非空
TC_NO                      发送电文                可空
* **************************************************************************** */
/* C/C++ 的标准头文件部分 */
#include "stdafx.h"		// 框架头，不可删除 
string mat_no_str_sm(EIClass* bcls_rec);
BM2_FUNCTION_EXPORT
int f_wmsmsm_cmd_auto(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wmsmsm_cranecmd_make(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	/* ***** 程序变量 ***** */
	int doFlag = 0;
	CString v_datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString v_mat_no = "";
	CString v_stock_oper_order = "";
	CString v_stock_no_to = "";
	CString v_stock_place_no_from = "";
	CString v_stock_place_no_to = "";
	CString table_name = "TMMSM01";
	/* ***** 数据库SQL操作字符串 ***** */
	CString	sqlstr = "";
	CString mat_no_s = "";
	CDataTable Table_mat;
	CDataTable Table_mat_up;
	int block_flag = 0;
	int table_count = 0;
	CModel twma7 =  CModel("TWMA7");
	CModel twma7_up = CModel("TWMA7");
	CModel twma2 = CModel("TWMA2");
	CModel twm04 = CModel("TWM04");
	CModel twm04_old = CModel("TWM04");
	EIClass bcls_rec_cmd_auto;
	bcls_rec_cmd_auto.Tables[0].set_TableName("AUTO_INFO_IN");
	bcls_rec_cmd_auto.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
	bcls_rec_cmd_auto.Tables[0].Columns.Add(DT_STRING, "HALL_NO");
	bcls_rec_cmd_auto.Tables[0].Columns.Add(DT_STRING, "STOCK_NO");
	bcls_rec_cmd_auto.Tables[0].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");
	bcls_rec_cmd_auto.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO");
	bcls_rec_cmd_auto.Tables[0].Columns.Add(DT_STRING, "TABLE_NAME");
	bcls_rec_cmd_auto.Tables[0].Rows.Add();
	/* ***** 应用程序开始处理 ***** */
	try
	{
		if (!bcls_rec->Tables.Contains("WM_CMD")){
			sprintf(s.msg, "函数f_wm_cmd_make中找不到接收块名[WM_CMD]");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (bcls_rec->Tables["WM_CMD"].Rows.get_Count() == 0){
			Log::Trace("", __FUNCTION__, "函数f_wmhrhr_cranecmd_make中传入数据为空");
			return doFlag;
		}
		//炼钢按块生成命令
		if (bcls_rec->Tables["WM_CMD"].Columns.Contains("MAT_NUM") && bcls_rec->Tables["WM_CMD"].Rows[0]["MAT_NUM"].ToString() != "0" && bcls_rec->Tables["WM_CMD"].Rows[0]["MAT_NUM"].ToString().Trim() != "")
		{
			table_name = "TMMSM01";
			//根据起始位置找到材料
			CDataTable Table_mat_se;
			Table_mat.Columns.Add(DT_STRING,"MAT_NO");
			Table_mat.Columns.Add(DT_STRING, "STOCK_OPER_ORDER");
			Table_mat.Columns.Add(DT_STRING, "STOCK_PLACE_NO_TO");
			Table_mat.Columns.Add(DT_STRING, "STOCK_NO_TO");
			for (int i = 0; i < bcls_rec->Tables["WM_CMD"].Rows.get_Count(); i++)
			{
				Table_mat_se.Clear();
				sqlstr = "select mat_no from twma2 where stock_place_no ='" + bcls_rec->Tables["WM_CMD"].Rows[i]["STOCK_PLACE_NO_FROM"].ToString() + "'where mat_no not in (select mat_no from twma7) order by LAYERNO DESC  FETCH FIRST "+ bcls_rec->Tables["WM_CMD"].Rows[0]["MAT_NUM"].ToString().Trim() +"  ROW ONLY";
				Log::Trace("", __FUNCTION__, "sqlstr按块查询[{0}]", sqlstr);
				Db::QueryTable(sqlstr, Table_mat_se);
				for (int j = 0; j < Table_mat_se.Rows.get_Count(); j++)
				{
					Table_mat.Rows.Add();
					Table_mat.Rows[Table_mat.Rows.get_Count() - 1]["MAT_NO"] = Table_mat_se.Rows[j]["MAT_NO"];
					Table_mat.Rows[Table_mat.Rows.get_Count() - 1]["STOCK_OPER_ORDER"] = bcls_rec->Tables["WM_CMD"].Rows[i]["STOCK_OPER_ORDER"].ToString();
					Table_mat.Rows[Table_mat.Rows.get_Count() - 1]["STOCK_PLACE_NO_TO"] = bcls_rec->Tables["WM_CMD"].Rows[i]["STOCK_PLACE_NO_TO"].ToString();
					Table_mat.Rows[Table_mat.Rows.get_Count() - 1]["STOCK_NO_TO"] = bcls_rec->Tables["WM_CMD"].Rows[i]["STOCK_NO_TO"].ToString();
					Table_mat.Rows[Table_mat.Rows.get_Count() - 1]["MAIN_MAT_NO"]=EPGetNextSeq("WM_CMD_SEQ", conn);//就当做是组吊号吧
				}
			}
			block_flag = 1;
			table_count = Table_mat.Rows.get_Count();
		}
		else 
		{
			/*if (Db::QueryCDecimal("select count(1) from tmmsm01 where mat_no='" + bcls_rec->Tables["WM_CMD"].Rows[0]["MAT_NO"].ToString().Trim() + "'") == 1)
			{
				table_name = "TMMSM01";
			}
			else if (Db::QueryCDecimal("select count(1) from tmmcr01 where mat_no='" + bcls_rec->Tables["WM_CMD"].Rows[0]["MAT_NO"].ToString().Trim() + "'") == 1)
			{
				table_name = "TMMCR01";
			}*/
			mat_no_s = mat_no_str_sm(bcls_rec);
			table_count = bcls_rec->Tables["WM_CMD"].Rows.get_Count();
		}
		CModel twma1 = CModel(table_name);
		CModel twma1_up = CModel(table_name);
		for (int i = 0; i < table_count; i++)
		{
			if (block_flag == 1)
			{
				v_mat_no = Table_mat.Rows[i]["MAT_NO"].ToString();
				v_stock_oper_order = Table_mat.Rows[i]["STOCK_OPER_ORDER"].ToString();
				v_stock_no_to = Table_mat.Rows[i]["STOCK_NO_TO"].ToString();
				v_stock_place_no_to = Table_mat.Rows[i]["STOCK_PLACE_NO_TO"].ToString();
				if (v_stock_place_no_to.Trim() == "")
				{
					sprintf(s.msg, "函数f_wmhrhr_cranecmd_make中v_stock_place_no_to传入值为空");
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			else
			{
				v_mat_no = bcls_rec->Tables["WM_CMD"].Rows[i]["MAT_NO"].ToString();
				v_stock_oper_order = bcls_rec->Tables["WM_CMD"].Rows[i]["STOCK_OPER_ORDER"].ToString();
				v_stock_no_to = bcls_rec->Tables["WM_CMD"].Rows[i]["STOCK_NO_TO"].ToString();
				v_stock_place_no_to = bcls_rec->Tables["WM_CMD"].Rows[i]["STOCK_PLACE_NO_TO"].ToString();
			}
			if (bcls_rec->Tables["WM_CMD"].Columns.Contains("STOCK_PLACE_NO_FROM") && bcls_rec->Tables["WM_CMD"].Rows[i]["STOCK_PLACE_NO_FROM"].ToString().Trim() != ""){
				v_stock_place_no_from = bcls_rec->Tables["WM_CMD"].Rows[0]["STOCK_PLACE_NO_FROM"].ToString();
			}
			Log::Trace("", __FUNCTION__, "v_stock_no_to{0}", v_stock_no_to);
			Log::Trace("", __FUNCTION__, "v_stock_place_no_to{0}", v_stock_place_no_to);
			Log::Trace("", __FUNCTION__, "v_mat_no{0}", v_mat_no);
			Log::Trace("", __FUNCTION__, "v_stock_oper_order{0}", v_stock_oper_order);
			Log::Trace("", __FUNCTION__, "v_stock_place_no_from{0}", v_stock_place_no_from);
			if (v_mat_no.Trim() == "")
			{
				sprintf(s.msg, "函数f_wmhrhr_cranecmd_make中v_mat_no传入值为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (v_stock_oper_order.Trim() == "")
			{
				sprintf(s.msg, "函数f_wmhrhr_cranecmd_make中v_stock_oper_order传入值为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			twma7.Reset();
			twma7["MAT_NO"] = v_mat_no;
			twma1.Reset();
			twma1["MAT_NO"] = v_mat_no;
			twma1.Query("MAT_NO");
			twma2.Reset();
			twma2["MAT_NO"] = v_mat_no;
			twma2.Query("MAT_NO");
			if (v_stock_place_no_from == "")//如果传了值取传值，未传则取当前垛位
			{
				v_stock_place_no_from = twma2["STOCK_PLACE_NO"].ToString().Trim();
				Log::Trace("", __FUNCTION__, "v_stock_place_no_to{0}", v_stock_place_no_to);
			}
			twm04_old.Reset();
			twm04.Reset();
			twm04_old["STOCK_PLACE_NO"] = v_stock_place_no_from;
			if (!twm04_old.Query("STOCK_PLACE_NO"))
			{
				sprintf(s.msg, "函数f_wm_cmd_make中v_stock_place_no_from起始垛位在04表不存在");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (v_stock_place_no_to.Trim()=="")//测试一下，LZ
			{
				bcls_rec_cmd_auto.Tables["AUTO_INFO_IN"].Rows[0]["MAT_NO"] = v_mat_no;
				bcls_rec_cmd_auto.Tables["AUTO_INFO_IN"].Rows[0]["STOCK_NO"] = twm04_old["STOCK_NO"];
				bcls_rec_cmd_auto.Tables["AUTO_INFO_IN"].Rows[0]["HALL_NO"] = twm04_old["HALL_NO"];
				bcls_rec_cmd_auto.Tables["AUTO_INFO_IN"].Rows[0]["STOCK_OPER_ORDER"] = v_stock_oper_order;
				bcls_rec_cmd_auto.Tables["AUTO_INFO_IN"].Rows[0]["TABLE_NAME"] = table_name;
				doFlag = f_wmsmsm_cmd_auto(&bcls_rec_cmd_auto, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
				v_stock_place_no_to = bcls_rec_cmd_auto.Tables["AUTO_INFO_IN"].Rows[0]["STOCK_PLACE_NO"].ToString().Trim();
			}		

			twm04["STOCK_PLACE_NO"] = v_stock_place_no_to;				
			if (!twm04.Query("STOCK_PLACE_NO"))
			{
				sprintf(s.msg, "函数f_wm_cmd_make中v_stock_place_no_to目标垛位在04表不存在");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (v_stock_no_to.Trim() == "")
			{
				v_stock_no_to= twm04["STOCK_NO"].ToString().Trim();
			}
			if (twma1["MAT_KIND"].ToString() == "SM")//板坯和卷分开
			{
				Log::Trace("", __FUNCTION__, "材料为板坯");
				if (twma7.QueryCount("MAT_NO") > 0)
				{
					Log::Trace("", __FUNCTION__, "材料已有指令[{0}]", (const char*)v_mat_no);
					continue;
				}
				else
				{

					//上层倒垛
					if (block_flag != 1)//按块命令无上层倒垛
					{
						if (v_stock_oper_order[0] != '1')
						{
							sqlstr = "select * from twma2 where stock_place_no ='" + v_stock_place_no_from + "' and mat_no not in(select mat_no from twma7) and mat_no not in ('" + mat_no_s + "')";
							Log::Trace("", __FUNCTION__, "sqlstr板上层倒垛[{0}]", sqlstr);
							Table_mat_up.Clear();
							Db::QueryTable(sqlstr, Table_mat_up);
							for (int j = 0; j < Table_mat_up.Rows.get_Count(); j++)
							{
								twma1_up.Reset();
								twma7_up.Reset();
								twma1_up["MAT_NO"] = Table_mat_up.Rows[j]["MAT_NO"].ToString();
								twma1_up.Query("MAT_NO");
								twma7_up["MAT_NO"] = Table_mat_up.Rows[j]["MAT_NO"].ToString();
								twma7_up["REC_CREATOR"] = s.userid;
								twma7_up["REC_CREATE_TIME"] = v_datetime;
								twma7_up["MAT_KIND"] = twma1_up["MAT_KIND"].ToString();
								twma7_up["MAT_ACT_THICK"] = twma1_up["MAT_ACT_THICK"];
								twma7_up["MAT_ACT_WIDTH"] = twma1_up["MAT_ACT_WIDTH"];
								twma7_up["MAT_ACT_LEN"] = twma1_up["MAT_ACT_LEN"];
								twma7_up["MAT_ACT_WT"] = twma1_up["MAT_ACT_WT"];
								twma7_up["CRANE_INST_STATUS"] = "1";
								twma7_up["STOCK_NO_TO"] = v_stock_no_to;
								bcls_rec_cmd_auto.Tables["AUTO_INFO_IN"].Rows[0]["MAT_NO"] = v_mat_no;
								bcls_rec_cmd_auto.Tables["AUTO_INFO_IN"].Rows[0]["STOCK_NO"] = twm04_old["STOCK_NO"];
								bcls_rec_cmd_auto.Tables["AUTO_INFO_IN"].Rows[0]["HALL_NO"] = twm04_old["HALL_NO"];
								bcls_rec_cmd_auto.Tables["AUTO_INFO_IN"].Rows[0]["STOCK_OPER_ORDER"] = v_stock_oper_order;
								bcls_rec_cmd_auto.Tables["AUTO_INFO_IN"].Rows[0]["TABLE_NAME"] = table_name;
								doFlag = f_wmsmsm_cmd_auto(&bcls_rec_cmd_auto, bcls_ret, conn);
								if (doFlag < 0)
								{
									throw CApplicationException(-1, s.msg, log.Location);
								}
								twma7_up["STOCK_PLACE_NO_TO"] = bcls_rec_cmd_auto.Tables["AUTO_INFO_IN"].Rows[0]["STOCK_PLACE_NO"].ToString().Trim();//需要垛位推荐
								twma7_up["MAIN_MAT_NO"] = v_mat_no;
								twma7_up["CMD_SEQ"] = EPGetNextSeq("WM_CMD_SEQ", conn);
								twma7_up["STOCK_OPER_ORDER"] = "32";
								twma2["MAT_NO"] = Table_mat_up.Rows[j]["MAT_NO"].ToString();
								twma7_up["STOCK_NO_FROM"] = twma2["STOCK_NO"];
								twma7_up["STOCK_NO"] = twma2["STOCK_NO"];
								twma7_up["STOCK_PLACE_NO_FROM"] = twma2["STOCK_PLACE_NO"];
								twma7_up["HALL_NO_FR"] = twm04_old["HALL_NO"].ToString().Trim();
								twma7_up["YARD_LAYER_FROM"] = Table_mat_up.Rows[j]["LAYERNO"].ToString();
								twma7_up.Insert();
							}
						}
						twma7["MAIN_MAT_NO"] = v_mat_no;
					}
					else
					{
						twma7["MAIN_MAT_NO"]= Table_mat.Rows[i]["MAIN_MAT_NO"].ToString();
					}
					twma7["REC_CREATOR"] = s.userid; 
					twma7["REC_CREATE_TIME"] = v_datetime;
					twma7["MAT_KIND"] = twma1["MAT_KIND"].ToString();
					twma7["MAT_ACT_THICK"] = twma1["MAT_ACT_THICK"];
					twma7["MAT_ACT_WIDTH"] = twma1["MAT_ACT_WIDTH"];
					twma7["MAT_ACT_LEN"] = twma1["MAT_ACT_LEN"];
					twma7["MAT_ACT_WT"] = twma1["MAT_ACT_WT"];
					twma7["CRANE_INST_STATUS"] = "1";
					twma7["STOCK_NO_TO"] = v_stock_no_to;
					twma7["STOCK_PLACE_NO_TO"] = v_stock_place_no_to;
					twma7["STOCK_OPER_ORDER"] = v_stock_oper_order;
					twma7["CMD_SEQ"] = EPGetNextSeq("WM_CMD_SEQ", conn);
					if (v_stock_oper_order[0] == '1')
					{
						Log::Trace("", __FUNCTION__, "入库");
						twma7["STOCK_NO"] = v_stock_no_to;
						twma7["STOCK_NO_FROM"] = twm04_old["STOCK_NO"];
						twma7["STOCK_NO"] = twm04_old["STOCK_NO"];
						twma7["STOCK_PLACE_NO_FROM"] = twm04_old["STOCK_PLACE_NO"];
						twma7["HALL_NO_FR"] = twm04_old["HALL_NO"].ToString().Trim();
						twma7["YARD_LAYER_FROM"] = 1;
					}
					else
					{
						twma2["MAT_NO"] = v_mat_no;
						if (!twma2.Query("MAT_NO"))
						{
							sprintf(s.msg, "函数f_wm_cmd_make中材料不在库中");
							throw CApplicationException(-1, s.msg, log.Location);
						}
						twma7["STOCK_NO_FROM"] = twma2["STOCK_NO"];
						twma7["STOCK_NO"] = twma2["STOCK_NO"];
						twma7["STOCK_PLACE_NO_FROM"] = twma2["STOCK_PLACE_NO"];
						twma7["HALL_NO_FR"] = twm04_old["HALL_NO"].ToString().Trim();
						twma7["YARD_LAYER_FROM"] = twma2["LAYERNO"];
						Log::Trace("", __FUNCTION__, "HALL_NOOLD{0}", twm04_old["HALL_NO"].ToString().Trim());
						Log::Trace("", __FUNCTION__, "HALL_N{0}", twm04["HALL_NO"].ToString().Trim());
						//看情况是否需要过跨,需要过跨转换为过跨倒垛
						if (twm04_old["HALL_NO"].ToString().Trim() != twm04["HALL_NO"].ToString().Trim())
						{//需要过跨，转换数据
							Log::Trace("", __FUNCTION__, "需要过跨");
							twma7["STOCK_NO_TO"] = v_stock_no_to;
							twma7["HALL_NO_TO"] = twm04_old["HALL_NO"].ToString().Trim();
							twma7["STOCK_PLACE_NO_TO"] = " ";//过跨车号待定，根据现场实际情况判断
							twma7["STOCK_OPER_ORDER"] = "32";
							twma7["STOCK_NO_FIN"] = v_stock_no_to;
							twma7["HALL_NO_FIN"] = twm04["HALL_NO"].ToString().Trim();
							twma7["STOCK_PLACE_NO_FIN"] = v_stock_place_no_to;
							twma7["STOCK_OPER_ORDER_FIN"] = v_stock_oper_order;
						}
					}
					twma7.TrimOrBlank();
					
					twma7.Insert();
				}
			}
			else
			{
				Log::Trace("", __FUNCTION__, "材料为其他形态[{0}]", twma1["MAT_KIND"].ToString());
				continue;
			}
		}

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CString str = ex.GetMsg() + "\r\n" + sqlstr;
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), 399);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), 399);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}

string mat_no_str_sm(EIClass* bcls_rec)
{
	string mat_no = "";
	int j = bcls_rec->Tables["WM_CMD"].Rows.get_Count();
	for (int i = 0; i < j; i++)
	{
		if (i != j - 1)
		{
			mat_no = mat_no + bcls_rec->Tables["WM_CMD"].Rows[i]["MAT_NO"].ToString() + "','";
		}
		else
		{
			mat_no = mat_no + bcls_rec->Tables["WM_CMD"].Rows[i]["MAT_NO"].ToString();
		}
	}
	return mat_no;
}
