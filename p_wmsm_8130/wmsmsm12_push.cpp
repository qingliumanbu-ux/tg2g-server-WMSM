/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         lizhen
Version:		1.0
Date:			2024-1-4
Description:	补装车
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件

//函数申明
int f_mmsm99(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wmsm_21a009_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wmsmsm_allot_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_mmsm_e2t8m1_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
/*<remark>=========================================================
///<summary>
///装车实绩查询
///<para>
///2.排序方式：
///</para>
///<para>数据库表：TWMSM61 装车实绩表；
///<returns>返回符合查询条件的实绩信息</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wmsmsm12_push);

int f_wmsmsm12_push(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */
	CModel tmmsm01 = CModel("TMMSM01");
	CModel tmmsm96 = CModel("TMMSM96");
	CModel twmsm61 = CModel("TWMSM61");
	CModel twmsm61_L = CModel("TWMSM61");
	CModel twm41dj = CModel("TWM41DJ");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlstr1 = "";
	CString sqlwhere = "";
	CString s_userid("");
	CString sqlstr_count;
	CString sqlstr_temp;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_con(conn);


	//系统的分页类信息。
	CPageInfo pageInfo;

	/* 业务变量 */
	CString practice_no("");
	//发送电文
	EIClass bcls_load;
	bcls_load.Tables[0].set_TableName("21A009");
	bcls_load.Tables[0].Columns.Add(twmsm61);
	bcls_load.Tables[0].Rows.Clear();

	EIClass bcls_allot;
	//bcls_load.Tables[0].set_TableName("21A009");
	bcls_allot.Tables[0].Columns.Add(twm41dj);
	bcls_allot.Tables[0].Rows.Clear();

	//调用物料事件
	EIClass mm0099;
	mm0099.Tables[0].set_TableName("MM0099");
	mm0099.Tables[0].Columns.Add(tmmsm96);
	mm0099.Tables[0].Rows.Clear();
	EIClass mm00991;
	mm00991.Tables[0].set_TableName("MM0099");
	mm00991.Tables[0].Columns.Add(tmmsm96);
	mm00991.Tables[0].Rows.Clear();

	EIClass t8e2m1;
	t8e2m1.Tables[0].set_TableName("E2T8M1");
	t8e2m1.Tables["E2T8M1"].Columns.Add(DT_STRING, "MAT_NO");
	t8e2m1.Tables[0].Rows.Clear();


	try {
		EIClass tmp;
		CString LOAD_CODE_FACTORY("");
		CString LOAD_CODE_AREA("");
		CString LOAD_CODE("");
		CString UNLOAD_CODE_FACTORY("");
		CString UNLOAD_CODE_AREA("");
		CString UNLOAD_CODE("");
		CString TRUCK_NO("");
		CString LOAD_END_TIME("");

		practice_no = bcls_rec->Tables[0].Rows[0]["PRACTICE_NO"].ToString();
		sqlstr = " select * from twmsm61 where PRACTICE_NO='" + practice_no + "' and UNLOAD_STATE='2' ";
		Log::Trace("", __FUNCTION__, "sqlstr=[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			cmd_inq.Fetch(twmsm61_L);
		}
		else
		{
			sprintf(s.msg, "当前装车单没有未卸车的材料，不能补妆车！");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		cmd_inq.Close();
		Log::Trace("", __FUNCTION__, "row=[{0}]", bcls_rec->Tables[0].Rows.get_Count());
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			if (tmmsm01["MAT_NO"].ToString().Trim() == "")
			{
				sprintf(s.sysmsg, "材料号不能为空");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (tmmsm01.Query("MAT_NO"))
			{
				if (tmmsm01["LOGISTICS_STATUS"].ToString() != "0"
					&& tmmsm01["LOGISTICS_STATUS"].ToString() != "1"
					&& tmmsm01["LOGISTICS_STATUS"].ToString() != "4")
				{
					sprintf(s.msg, "材料物流状态为[%s],不能装车.", (const char*)tmmsm01["LOGISTICS_STATUS"]);
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				//发装车实绩
				twmsm61.CopyFrom(twmsm61_L);
				
				twmsm61["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				twmsm61["REC_CREATOR"] = s.userid;
				twmsm61["MAT_NO"] = tmmsm01["MAT_NO"];
				twmsm61["WIDTH"] = tmmsm01["MAT_WIDTH"];
				Log::Info("", __FUNCTION__, "f_mmsm10_trace_linke   =[{0}]", __LINE__);
				twmsm61["LENGTH"] = tmmsm01["MAT_LEN"];
				twmsm61["THICK"] = tmmsm01["MAT_THICK"];
				twmsm61["WEIGHT"] = tmmsm01["MAT_WT"];
				twmsm61["SG_SIGN"] = tmmsm01["SG_SIGN"];
				twmsm61["HEAT_NO"] = tmmsm01["HEAT_NO"];
				twmsm61["ST_NO"] = tmmsm01["ST_NO"];
				twmsm61["MAT_LEN"] = tmmsm01["MAT_LEN"];
				twmsm61["MAT_WIDTH"] = tmmsm01["MAT_WIDTH"];
				twmsm61["MAT_WT"] = tmmsm01["MAT_WT"];
				twmsm61["MAT_THICK"] = tmmsm01["MAT_THICK"];
				
				
				
				Log::Info("", __FUNCTION__, "f_mmsm10_trace_linke   =[{0}]", __LINE__);
				//twmsm61["PRODUCT_TYPE"] = "1";
				twmsm61["TRANS_TYPE"] = "2";
				if (LOAD_CODE_FACTORY == "6390")
				{
					twmsm61["MATERIAL_CODE"] = "HAB000000000000000";
				}
				else
				{
					twmsm61["MATERIAL_CODE"] = "HAA000000000000000";
				}

				Log::Info("", __FUNCTION__, "f_mmsm10_trace_linke   =[{0}]", __LINE__);
				twm41dj["C_BATCHUNIT"] = tmmsm01["MAT_NO"];
				twm41dj["C_STATESIGN"] = "1";
				if (twm41dj.QueryCount("C_BATCHUNIT,C_STATESIGN")>0)
				{
					twmsm61["DEALY_FLAG"] = "4";
				}
				else
				{
					twmsm61["DEALY_FLAG"] = "1";
				}
				
				CString qx_type = Db::QueryCString("select CODE_DESC_3_CONTENT from TWMSMZD02 where CODE_CLASS='WM02' AND CODE='" + tmmsm01["GUIDE_DEST"].ToString() + "'");
				if (tmmsm01["UNLOAD_CODE"].ToString() == "TBZX01001" && qx_type.Find("3") < 0)
				{
					sprintf(s.msg, "发往太北站的材料必须是指导去向类别为3的去向！");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				twmsm61.Insert();
				twmsm61.MergeTo(bcls_load.Tables["21A009"], false);
				

				/*twmsm61.MergeFrom(bcls_rec->Tables[0].Rows[i]);*/
				CString c_acceptstock = " ";
				if ((LOAD_CODE_FACTORY == "6230"
					|| LOAD_CODE_FACTORY == "6320"
					|| LOAD_CODE_FACTORY == "6350"
					|| LOAD_CODE_FACTORY == "6390")&& twm41dj.QueryCount("C_BATCHUNIT,C_STATESIGN") == 0)
				{
					if (LOAD_CODE_FACTORY == "6230")
						c_acceptstock = "6235";
					if (LOAD_CODE_FACTORY == "6320")
						c_acceptstock = "6321";
					if (LOAD_CODE_FACTORY == "6350")
						c_acceptstock = "6351";
					if (LOAD_CODE_FACTORY == "6390")
						c_acceptstock = "6391";

					if (tmmsm01["C_STATESIGN"].ToString() != " "
						&& tmmsm01["C_STATESIGN"].ToString() != "0")
					{
					
						
						continue;
					}
					if (tmmsm01["COMPLEX_DECIDE_CODE"].ToString() != '1' && tmmsm01["MAT_DESTION"].ToString() != '11')
					{
						
						continue;
					}
					//发调拨单
					twm41dj.Reset();
					twm41dj.CopyFrom(tmmsm01);
					twm41dj["REC_CREATOR"] = s.userid;
					twm41dj["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
					twm41dj["C_DELIVERYID"] = "6240" + datetime.SubstringNE(0, 8) + Db::QueryCString("SELECT LPAD(TO_CHAR(DB_NO.NEXTVAL), 4, '0') FROM DUAl");
					twm41dj["C_QULITYTRACEID"] = tmmsm01["HEAT_NO"];//炉号
					twm41dj["C_BATCHID"] = tmmsm01["BATCH"];//批次号
					twm41dj["C_BATCHUNIT"] = tmmsm01["MAT_NO"];//
					twm41dj["C_SENDDEPT"] = "6240";//发送工厂
					twm41dj["C_ACCEPTDEPT"] = LOAD_CODE_FACTORY;//接受工厂1
					
					twm41dj["C_SENDSTOCK"] = tmmsm01["LGORT"];//发送库房
					twm41dj["C_ACCEPTSTOCK"] = c_acceptstock;//接受库房
					twm41dj["DELIVERY_THICKNESS"] = tmmsm01["MAT_THICK"];//厚度
					twm41dj["DELIVERY_WIDTH"] = tmmsm01["MAT_WIDTH"];//宽度1
					twm41dj["STEELGRADE"] = tmmsm01["ST_NO"];//钢牌号
					twm41dj["N_SENDAMOUNT"] = tmmsm01["MAT_ACT_WT"];//发送重量
					twm41dj["C_SENDUNIT"] = "TON";//发送单位
					twm41dj["N_ACCEPTAMOUNT"] = tmmsm01["MAT_WT"];//接收重量
					twm41dj["C_ACCEPTUNIT"] = "TON";//接收单位
					twm41dj["C_STATESIGN"] = "1";//调拨状态（1-未确认，2-接收，3-驳回）
					twm41dj["D_OPERATIONDATE"] = datetime;
					twm41dj["D_BILLDATE"] = datetime;
					twm41dj["T_OUTSTOCKTIME"] = datetime;
				
					{
						twm41dj["T_ACCEPTTIME"] = datetime;
						twm41dj["C_CLOSEGATETIME"] = datetime;
						twm41dj["T_UPLOADTIME"] = datetime;
						twm41dj["T_INSTOCKTIME"] = datetime;
						twm41dj["T_SALESCOMFIRMTIME"] = datetime;
						twm41dj["T_OVERRULETIME"] = datetime;
						twm41dj["D_REQUIREDATE"] = datetime;
					}

					twm41dj["I_STOCKMODE"] = "件次";
					twm41dj["C_REMARK"] = tmmsm01["SG_GRADE_1"];
					twm41dj["I_RESERVECOL4"] = "0";//调拨类型（0-正常调拨，1-回退调拨）
					twm41dj["C_INSTOCKSIGN"] = "3";
					twm41dj["C_ISFREEZE"] = "FREE";//库存类型-
					twm41dj["C_STOCKSPEC"] = "FREE";//特殊库存标识
					twm41dj["C_ORDERID"] = tmmsm01["ORDER_NO"];//合同号
					twm41dj["I_RESERVECOL3"] = tmmsm01["MAT_LEN"];
					twm41dj["C_ACHIEVEID"] = "1";
					twm41dj["C_TRUCKNUM"] = twmsm61_L["TRUCK_NO"].ToString().TrimOrBlank();
					if (tmmsm01["PRODUCT_FLAG"].ToString() == "1")
					{
						if (c_acceptstock == "6390")
						{
							twm41dj["C_PRODUCTID"] = "FAB000000000000000";
							twm41dj["C_PRODUCTNAME"] = "连铸中板坯";
						}
						else
						{
							twm41dj["C_PRODUCTID"] = "FAA000000000000000";
							twm41dj["C_PRODUCTNAME"] = "连铸卷板坯";
						}
					}
					else
					{
						if (c_acceptstock == "6390")
						{
							twm41dj["C_PRODUCTID"] = "HAB000000000000000";
							twm41dj["C_PRODUCTNAME"] = "连铸中板坯";
						}
						else
						{
							twm41dj["C_PRODUCTID"] = "HAA000000000000000";
							twm41dj["C_PRODUCTNAME"] = "连铸卷板坯";
						}
					}
					Log::Trace("", __FUNCTION__, "{0}", twm41dj["C_INSTOCKSIGN"].ToString());
					
					twm41dj.TrimOrBlank();
					twm41dj.Insert();
					twm41dj.MergeTo(bcls_allot.Tables[0], false);
					twmsm61.MergeTo(t8e2m1.Tables["E2T8M1"], false);



					tmmsm96.Reset();
					tmmsm96.CopyFrom(tmmsm01);
					tmmsm96["C_STATESIGN"] = "1";//1--正向调拨出库，3--正向调拨完成
					tmmsm96["C_DELIVERYID"] = twm41dj["C_DELIVERYID"];
					tmmsm96["C_DELIVERY_FAC"] = twm41dj["C_ACCEPTDEPT"];
					tmmsm96["C_DELIVERY_STOCK"] = twm41dj["C_ACCEPTSTOCK"];
					tmmsm96["TRAN_TIME"] = datetime;;
					tmmsm96["EVENT_ID"] = "MM76";
					tmmsm96["SYSTEM_ID"] = "MMSM";
					tmmsm96["EVENT_LINE_TYPE"] = "00";
					tmmsm96["FUNC_ID"] = s.svc_name;
					tmmsm96.MergeTo(mm00991.Tables["MM0099"], false);
				}

			
				Log::Trace("", __FUNCTION__, "LINKE=[{0}]", __LINE__);
				//15、调物流事件
				tmmsm96.Reset();
				tmmsm96.CopyFrom(tmmsm01);
				tmmsm96["LOGISTICS_STATUS"] = "2";//2--装车确认
				tmmsm96["FACTORY_TO"] = twmsm61["UNLOAD_CODE_FACTORY"];
				tmmsm96["DST_STOCK_CODE"] = twmsm61["UNLOAD_CODE_AREA"];
				tmmsm96["UNLOAD_CODE"] = twmsm61["UNLOAD_CODE"];
				tmmsm96["OUT_STOCK_TIME"] = datetime;
				tmmsm96["PRACTICE_NO"] = twmsm61["PRACTICE_NO"];
				tmmsm96["EVENT_ID"] = "MM77";
				tmmsm96["SYSTEM_ID"] = "MMSM";
				tmmsm96["EVENT_LINE_TYPE"] = "00";
				tmmsm96["FUNC_ID"] = s.svc_name;
				tmmsm96.MergeTo(mm0099.Tables["MM0099"], false);

				
			}
			else
			{
				sprintf(s.sysmsg, "材料号不存在");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
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

		if (mm00991.Tables["MM0099"].Rows.get_Count() > 0)
		{
			doFlag = f_mmsm99(&mm00991, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		
		//发装车实绩电文
		if (bcls_load.Tables[0].Rows.get_Count() > 0)
		{
			doFlag = f_wmsm_21a009_snd(&bcls_load, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}

		if (bcls_allot.Tables[0].Rows.get_Count() > 0) {
			doFlag = f_wmsmsm_allot_snd(&bcls_allot, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}

		if (t8e2m1.Tables["E2T8M1"].Rows.get_Count() > 0)
		{
			doFlag = f_mmsm_e2t8m1_snd(&t8e2m1, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "Database processing error，sqlcode = [{0}]." /* _RES("GCRSS0000006")*//*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
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