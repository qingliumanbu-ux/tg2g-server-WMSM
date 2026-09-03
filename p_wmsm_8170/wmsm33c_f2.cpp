/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     lizhen
Version:    1.0
Date:       2025-7-16
Description: 成品大炉号新增
**************************************************/
//框架头文件
#include "stdafx.h"

int f_mmsm_get_density(CString ST_NO, CDecimal& MAT_DENSITY, CDbConnection* conn);//通过钢种计算密度
int f_mmsm99(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_mmsmacsh_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wmsmsm_stock_in(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//调用仓库接口，进行板坯入库 
int f_mm0011(CString SeqName, CDecimal SeqLen, CString& SeqNo, CDbConnection* conn);	//获取流水号
// service入口
BM2F_ENTERACE(wmsm33c_f2)

int f_wmsm33c_f2(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	
	
	CString v_station_id = "";
	CString v_resume_seq_no = "";//序号
	CString v_shll_seq = "";//收货履历序号

	

	CDbCommand cmd_inq(conn);
	CModel tmmsm01("TMMSM01");
	CModel tmmsm96("TMMSM96");
	CModel hmmsm01("HMMSM01");
	CModel tmmsm33c("TMMSM33C");
	CModel tmmsm3e("TMMSM3E");
	CModel tmmsm33shll("TMMSM33SHLL");//收货履历表，原始记录，每次收货新增进表后 不再更改改该数据  主键:材料号，序号
	try
	{
		CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		//调用物料事件
		EIClass mm0099;
		mm0099.Tables[0].set_TableName("MM0099");
		mm0099.Tables[0].Columns.Add(tmmsm96);
		mm0099.Tables[0].Rows.Clear();

		EIClass mm00991;
		mm00991.Tables[0].set_TableName("MM0099");
		mm00991.Tables[0].Columns.Add(tmmsm96);
		mm00991.Tables[0].Rows.Clear();

		int blkNum = bcls_rec->Tables.IndexOf("MMSMACSH");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MMSMACSH");
			bcls_rec->Tables["MMSMACSH"].Columns.Add(tmmsm96);
		}

		bcls_rec->Tables["MMSMACSH"].Columns.Add(DT_STRING, "DEAL_FLAG");

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tmmsm01.TrimOrBlank();
			

			/* 检查输入参数合法性 */
			if (tmmsm01["MAT_NO"].ToString().Trim() == "")
			{
				strcpy(s.msg, "材料号不能为空");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (tmmsm01["MAT_NO"].ToString().GetLength() > 20)
			{
				sprintf(s.msg, _RES("MMHRS0000242")/*材料号长度不能超过20位*/);
				sprintf(s.sysmsg, _RES("MMHRS0000242")/*材料号长度不能超过20位*/);
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (tmmsm01["ST_NO"].ToString().Trim() == "")
			{
				strcpy(s.msg, "内部钢种不能为空");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (tmmsm01["PRODUCT_FLAG"].ToString().Trim() == "")
			{
				strcpy(s.msg, "成品标记不能为空");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (tmmsm01["MAT_WT"].ToDecimal() <= 0)
			{
				strcpy(s.msg, "材料重量不能小于0!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			tmmsm01["PONO"] = tmmsm01["HEAT_NO"];
			tmmsm01["BATCH"] = tmmsm01["MAT_NO"];
			tmmsm01["SM_PLAN_NOL2"] = tmmsm01["HEAT_NO"];
			tmmsm01["PRINT_NO"] = tmmsm01["SLAB_NO"];
			tmmsm01["PREC_SLAB_NO"] = "";           /*预定板坯号*/
			tmmsm01["PONO_SLAB"] = " ";           /*命令板坯号*/
			tmmsm01["PONO_SLAB_1"] = " ";           /*命令板坯号*/
			tmmsm01["PONO_SLAB_2"] = " ";           /*命令板坯号*/
			tmmsm01["PONO_SLAB_3"] = " ";           /*命令板坯号*/
			tmmsm01["PONO_SLAB_4"] = " ";           /*命令板坯号*/
			tmmsm01["PONO_SLAB_5"] = " ";           /*命令板坯号*/
			tmmsm01["PONO_SLAB_6"] = " ";           /*命令板坯号*/
			tmmsm01["PONO_SLAB_7"] = " ";           /*命令板坯号*/
			tmmsm01["PONO_SLAB_8"] = " ";           /*命令板坯号*/
			tmmsm01["PONO_SLAB_9"] = " ";           /*命令板坯号*/
			tmmsm01["PONO_SLAB_10"] = " ";           /*命令板坯号*/
			tmmsm01["PONO_SLAB_11"] = " ";           /*命令板坯号*/
			tmmsm01["PONO_SLAB_12"] = " ";           /*命令板坯号*/
			tmmsm01["MAT_SHAPE_FLAG"] = "1";
			tmmsm01["STOCK_NO"] = "SYA";
			Log::Trace("", __FUNCTION__, "11");
			tmmsm01["FIX_SLAB_NUM"] = 0;
			tmmsm01["LSLAB_NO"] = " ";
			tmmsm01["ORDER_NO"] = " ";
			tmmsm01["INITIAL_ORDER_NO"] = tmmsm01["ORDER_NO"];//初始合同号
			tmmsm01["REPAIR_FLAG"] = "0";                  /*返修标记C1*/
			tmmsm01["HOLD_FLAG"] = "0";                  /*封锁标记C1*/
			tmmsm01["SURFACE_DECIDE_CODE"] = "1";              /*表面判定代码C1*/
			tmmsm01["SURFACE_DECIDE_MAKER"] = " ";             /*表面判定责任者*/
			tmmsm01["PCH_JUDGE_CODE"] = "0";              /*性能判定代码C1*/
			tmmsm01["COMPLEX_DECIDE_CODE"] = "0";              /*综合判定代码C1*/
			tmmsm01["SLABTOP_FLAG"] = "0";              /*板坯TOP点确认标志*/

			tmmsm01["IN_FLAG"] = "0";              /*入库标记*/
			tmmsm01["TRANSFER_FLAG"] = "0";              /*转库计划标记*/
			tmmsm01["PRODUCT_PACK_FLAG"] = "0";              /*成品包装标志*/
			tmmsm01["CONFM_FLAG"] = "0";              /*准发确认标记*/
			tmmsm01["APP_DECIDE_FLAG"] = "0";              /*现货申报标记*/
			tmmsm01["STOCK_PLACE_NO"] = "GD";
			tmmsm01["COE_A"] = 0;
			tmmsm01["COE_B"] = 0;
			tmmsm01["MEND_FLAG"] = "0";
			tmmsm01["LGORT"] = "";
			tmmsm01["LOGISTICS_STATUS"] = "0";
			tmmsm01["DEV_CODE"] = tmmsm01["UNIT_CODE"];

			//余材原因
			tmmsm01["REMAINDER_REASON"] = " ";
			tmmsm01["HOLD_FLAG"] = "0";
			tmmsm01["RCV_MAT_FLAG"] = "N";  //收货标记  N 未收货
			tmmsm01["RECV_MAT_TIME"] = " ";
			tmmsm01["RECEIVE_WEIGHT"] = 0;
			tmmsm01["USAGE_DECISION"] = " ";

			//判废
			tmmsm01["SCRAP_TIME"] = " ";
			tmmsm01["SCRAP_MAKER"] = " ";
			tmmsm01["SCRAP_CAUSE_CODE"] = " ";
			tmmsm01["SCRAP_REMARK"] = " ";

			//物流调拨
			tmmsm01["C_STATESIGN"] = "0";
			tmmsm01["LOGISTICS_STATUS"] = "0";
			tmmsm01["C_DELIVERY_FAC"] = " ";
			tmmsm01["C_DELIVERY_STOCK"] = " ";
			tmmsm01["PRE_LOAD_FLAG"] = " ";
			tmmsm01["FACTORY_TO"] = " ";
			tmmsm01["DST_STOCK_CODE"] = " ";
			tmmsm01["UNLOAD_CODE"] = " ";
			tmmsm01["TRAN_TIME"] = " ";
			tmmsm01["TRAN_END_TIME"] = " ";
			tmmsm01["C_DELIVERYID"] = " ";
			tmmsm01["HAND_OVER_GROUP"] = " ";
			tmmsm01["C_ISHOTSEND"] = " ";
			tmmsm01["OUT_STOCK_TIME"] = " ";
			tmmsm01["LOAD_SCHEME_NO"] = " ";
			tmmsm01["PRACTICE_NO"] = " ";

			CString v_bmzl = "";
			sqlstr = " SELECT CODE_DESC_1_CONTENT FROM TWMSMZD02 WHERE  CODE_CLASS ='MMBMZL' and code='" + tmmsm01["SURF_QUALITY"].ToString() + "' ";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				v_bmzl = cmd_inq.GetString(1);
			}
			cmd_inq.Close();

			if (v_bmzl.Find("调宽") >= 0)
			{
				tmmsm01["ADJUST_WIDTH_MARK"] = "1";
			}

			/* 材料是否在当前档 */
			if (tmmsm01.QueryCount("MAT_NO") > 0)
			{
				sprintf(s.msg, "材料[%s]已在当前档存在!", (const char*)tmmsm01["MAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			else
			{
				hmmsm01["MAT_NO"] = tmmsm01["MAT_NO"];
				if (0 < hmmsm01.QueryCount("MAT_NO"))
				{
					sprintf(s.msg, "材料号[%s]已存在，但已归档!", (const char*)hmmsm01["MAT_NO"].ToString());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}

			/* 设置主档表初始值 */
			tmmsm01["FACTORY_DIV"] = "LG1";
			tmmsm01["FACTORY_STORE"] = "LG1";
			tmmsm01["MAT_ORIGIN"] = "5";				//材料来源大类 5-清盘库
			tmmsm01["MAT_LINE_TYPE"] = "SM";             //物料产线类型
			tmmsm01["MAT_KIND"] = "SM";             //物料种类  
			
			tmmsm01["MAT_TRACK_NO"] = CDateTime::Now().ToString("yyyyMMddHHmmssmsff").Substring(0, 18);
			tmmsm01["IN_FLAG"] = "0";
			tmmsm01["HOLD_FLAG"] = "0";
			tmmsm01["TRANSFER_FLAG"] = "0";
			tmmsm01["PCH_JUDGE_CODE"] = "0";
			tmmsm01["COMPLEX_DECIDE_CODE"] = "0";
			tmmsm01["CONFM_FLAG"] = "0";
			tmmsm01["APP_DECIDE_FLAG"] = "0";
			tmmsm01["REPAIR_FLAG"] = "0";
			tmmsm01["SURFACE_DECIDE_CODE"] = "1";					//表面判定代码(默认合格)
			tmmsm01["SURFACE_DECIDE_TIME"] = datetime;				//表面判定时间  
			tmmsm01["SURFACE_DECIDE_MAKER"] = s.userid;				//表面判定责任者   
			tmmsm01["PROD_MAKER"] = s.userid;
			tmmsm01["SLAB_HEAD_WIDTH"] = tmmsm01["MAT_WIDTH"];
			tmmsm01["SLAB_TAIL_WIDTH"] = tmmsm01["MAT_WIDTH"];
			tmmsm01["MAT_ACT_THICK"] = tmmsm01["MAT_THICK"];
			tmmsm01["MAT_ACT_WIDTH"] = tmmsm01["MAT_WIDTH"];
			tmmsm01["MAT_ACT_LEN"] = tmmsm01["MAT_LEN"];
			if (tmmsm01["SLAB_CUT_TIME"].ToString().Trim() == "")
			{
				tmmsm01["SLAB_CUT_TIME"] = datetime;
			}
			tmmsm01["PROD_TIME"] = tmmsm01["SLAB_CUT_TIME"];
			tmmsm01["MEASURE_WT_FLAG"] = "1";
			tmmsm01["MAT_THEORY_WT"] = tmmsm01["MAT_WT"].ToDecimal().Round(3);
			
			tmmsm01["MAT_ACT_WT"] = 0;
			tmmsm01["MEASURE_WT"] = tmmsm01["MAT_WT"];
			tmmsm01["L2_THEORY_WT"] = tmmsm01["MAT_WT"];
			tmmsm01["L3_CALTHEROY_WT"] = tmmsm01["MAT_WT"];
			tmmsm01["RECEIVE_WEIGHT"] = tmmsm01["MAT_WT"];
			tmmsm01["MAT_NUM"] = 1;
			tmmsm01["MAT_TUBE"] = tmmsm01["MAT_NUM"];
			//根据钢种查询钢牌号，将钢牌号更新掉  工艺卡牌号  跟sg_sign不同，sg_sign从tpssm03表获取
			if (tmmsm01["ST_NO"].ToString() != "")
			{
				sqlstr = "SELECT  SG_GRADE_1,C_DIV  FROM TQMTS0X  WHERE ST_NO = '" + tmmsm01["ST_NO"].ToString().Trim() + "'";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					tmmsm01["SG_GRADE_1"] = cmd_inq.GetString(1);
					tmmsm01["C_DIV"] = cmd_inq.GetString(2);
				}
				cmd_inq.Close();
			}

			//将不为12的碳锈区分，改为12
			if (tmmsm01["C_DIV"].ToString().Trim() != "")
			{
				if (tmmsm01["C_DIV"].ToString().Trim() == "4")
				{
					tmmsm01["C_DIV"] = "1";
				}
				if (tmmsm01["C_DIV"].ToString().Trim() == "3" || tmmsm01["C_DIV"].ToString().Trim() == "5")
				{
					tmmsm01["C_DIV"] = "2";
				}
			}
			
			if (true)
			{
				CDecimal v_code_wt = 0;//计算重量的系数
				doFlag = f_mmsm_get_density(tmmsm01["ST_NO"].ToString(), v_code_wt,conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
				tmmsm01["PRODUTE_CAL_WT"] = ((tmmsm01["MAT_ACT_THICK"].ToDecimal() / 1000) * (tmmsm01["MAT_ACT_WIDTH"].ToDecimal() / 1000) * (tmmsm01["MAT_ACT_LEN"].ToDecimal() / 1000) * v_code_wt).Round(3);

			}


			//获取连铸初判数据，获取不到赋默认值A
			tmmsm01["CASTING_PRE_JUDGMENT"] = "A";
			tmmsm01["REC_CREATE_TIME"] = datetime;			//记录创建时刻         
			tmmsm01["REC_CREATOR"] = s.userid;			//记录创建责任者      
			tmmsm01.TrimOrBlank();
			
			tmmsm96.Reset();
			tmmsm96.CopyFrom(tmmsm01);
			tmmsm96["EVENT_ID"] = "MM02";
			tmmsm96["EVENT_LINE_TYPE"] = "SM";
			tmmsm96["SYSTEM_ID"] = "MMSM";
			tmmsm96["FUNC_ID"] = s.svc_name;
			tmmsm96.MergeTo(mm0099.Tables["MM0099"], false);
			
			
			doFlag = f_mm0011("TMMSM3E_seq", 8, v_resume_seq_no, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
			tmmsm3e.CopyFrom(tmmsm01);
			tmmsm3e["PROD_TIME"] = datetime;
			tmmsm3e["RECEIVE_BACK_STATUS"] = "W";
			tmmsm3e["REMARK"] = "收货等待反馈";
			tmmsm3e["RESUME_SEQ_NO"] = datetime + v_resume_seq_no;
			tmmsm3e.TrimOrBlank();
			tmmsm3e.Insert();

			//将数据新增进收货履历表里
			tmmsm33shll.CopyFrom(tmmsm01);
			tmmsm33shll["MAT_WT"] = tmmsm01["RECEIVE_WEIGHT"];
			tmmsm33shll["MAT_ACT_WT"] = tmmsm01["RECEIVE_WEIGHT"];
			tmmsm33shll["QUALIFIED_WT"] = tmmsm01["RECEIVE_WEIGHT"];//合格产量
			tmmsm33shll["MAT_ACT_WIDTH"] = tmmsm01["MAT_WIDTH"];
			tmmsm33shll["MAT_ACT_LEN"] = tmmsm01["MAT_LEN"];
			tmmsm33shll["MAT_ACT_THICK"] = tmmsm01["MAT_THICK"];
			tmmsm33shll["RCV_MAT_FLAG"] = "W";//N 未收货  W等待(等L4的反馈)  S收货成功 


			doFlag = f_mm0011("TMMSM33SHLL_SEQ", 8, v_shll_seq, conn);
			if (doFlag < 0 || v_shll_seq.Trim() == "")
			{
				strcpy(s.msg, "物料材料跟踪号后8位流水号生成错误！");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}


			tmmsm33shll["REC_CREATE_TIME"] = datetime;
			tmmsm33shll["REC_CREATOR"] = s.userid;


			tmmsm33shll["RESUME_SEQ_NO"] = datetime + v_shll_seq;
			tmmsm33shll.TrimOrBlank();
			tmmsm33shll.Insert();

			tmmsm96.CopyFrom(tmmsm01);
			tmmsm96["EVENT_ID"] = "MM3H";
			tmmsm96["EVENT_LINE_TYPE"] = "SM";
			tmmsm96["SYSTEM_ID"] = "MMSM";
			tmmsm96["FUNC_ID"] = "mmsmacshf4_pro";
			tmmsm96["MAT_NO"] = tmmsm01["MAT_NO"];
			tmmsm96["RCV_MAT_FLAG"] = "W";
			tmmsm96["EVENT_DESC"] = "收货确认等待反馈";
			tmmsm96.MergeTo(mm00991.Tables["MM0099"], false);

			bcls_rec->Tables["MMSMACSH"].Rows.Add();
			bcls_rec->Tables["MMSMACSH"].Rows[i].Merge(tmmsm96);
			bcls_rec->Tables["MMSMACSH"].Rows[i]["DEAL_FLAG"] = "N";
		}
		//调用物料事件
		if (mm0099.Tables["MM0099"].Rows.get_Count() > 0)
		{
			doFlag = f_mmsm99(&mm0099, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		
		tmmsm33c.Reset();
		tmmsm33c["REC_CREATOR"] = s.userid;
		tmmsm33c["REC_CREATE_TIME"] = datetime;
		tmmsm33c["HEAT_NO"] = tmmsm01["HEAT_NO"];
		tmmsm33c["ST_NO"] = tmmsm01["ST_NO"];
		tmmsm33c["UNIT_CODE"] = tmmsm01["UNIT_CODE"];
		tmmsm33c.Insert();
		
		if (mm00991.Tables["MM0099"].Rows.get_Count() > 0)
		{
			doFlag = f_mmsm99(&mm00991, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}

		if (bcls_rec->Tables["MMSMACSH"].Rows.get_Count() > 0)
		{
			doFlag = f_mmsmacsh_snd(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
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
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
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
