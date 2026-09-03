/*************************************************
版权: Baosight Software LTD.co Copyright (c) 2012
创建者：lizhen 
功能:
修改历史:
日期:________;修改人:________; 需求提出人:________
变更内容:
**************************************************/

/*框架头文件*/
#include "stdafx.h" 
#include "epex.h"

int f_mmsm99(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wmsm_21a009_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wmsm_load_proc(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wmsm_auto_db(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);

BM2F_ENTERACE(wmsmsm12p_zc)


int f_wmsmsm12p_zc(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	/*打印程序起止日志*/
	CTracer log(__FUNCTION__);
	Log::Debug("", __FUNCTION__, "wmsmsmj3_rcm--------开始");
	/*定义公v_mat_no用变量*/
	int doFlag = 0;
	CString  sqlstr = "";
	CString  sql = "";
	CString v_mat_no, v_crane_no = "";
	CString  dateNow14 = CDateTime::Now().ToString("yyyyMMddHHmmss");  //14位日期变量

	//定义实体类
	CModel twmsm12("TWMSM12");
	CModel hwmsm12("HWMSM12");
	CModel hmmsm01("HMMSM01");
	CModel tmmsm01("TMMSM01");
	CModel tmmsm34("TMMSM34");
	CModel tmmsm39("TMMSM39");
	CModel tmmsm96 = CModel("TMMSM96");
	CModel twmsm61 = CModel("TWMSM61");

	CDbCommand cmd_inq(conn);
	CDbCommand cmd(conn);
	/*出库时间*/
	CString out_stock_time("");
	/*运输方式*/
	CString trnp_mode_code("");
	/*卡车号*/
	CString truck_no("");
	/*装车任务号*/
	CString load_scheme_no("");
	CString if_db("");
	CString v_shift_no(""), v_shift_group(" "), v_shift_group1(" ");

	CString datetime= CDateTime::Now().ToString("yyyyMMddHHmmss");
	//EIClass bcls_temp;

	//调用物料事件
	EIClass mm0099;
	mm0099.Tables[0].set_TableName("MM0099");
	mm0099.Tables[0].Columns.Add(tmmsm96);
	mm0099.Tables[0].Rows.Clear();
	EIClass bcls_load;
	bcls_load.Tables[0].set_TableName("21A009");
	bcls_load.Tables[0].Columns.Add(twmsm61);
	bcls_load.Tables[0].Rows.Clear();

	EIClass bcls_auto_db;
	bcls_auto_db.Tables[0].Columns.Add(twmsm12);
	bcls_auto_db.Tables[0].Rows.Clear();
	int db_rows = 0;

	try
	{

		if (bcls_rec->Tables[0].Columns.Contains("SHIFT_GROUP"))
		{
			
			v_shift_group = bcls_rec->Tables[0].Rows[0]["SHIFT_GROUP"].ToString().TrimOrBlank();
			
		}
		if (v_shift_group.Trim()=="")
		{
			
			f_epep_get_shift_group("SMCP", datetime, v_shift_no, v_shift_group, conn);
		
		}
		Log::Trace("", "", "LINE", __LINE__, v_shift_group);
		f_epep_get_shift_group("SMCP", datetime, v_shift_no, v_shift_group1, conn);
		out_stock_time = bcls_rec->Tables[0].Rows[0]["OUT_STOCK_TIME"].ToString();
		trnp_mode_code = bcls_rec->Tables[0].Rows[0]["TRNP_MODE_CODE"].ToString();
		truck_no = bcls_rec->Tables[0].Rows[0]["TRUCK_NO"].ToString();
		load_scheme_no= bcls_rec->Tables[0].Rows[0]["UNLOAD_CODE"].ToString()+ datetime.SubstringNE(2,12) + Db::QueryCString("SELECT LPAD(TO_CHAR(YZ_SJ.NEXTVAL), 2, '0') FROM DUAl");
		if_db = bcls_rec->Tables[0].Rows[0]["IF_DB"].ToString();
		CString v_practice_no = "XG6240" + datetime.SubstringNE(0, 8) + Db::QueryCString("SELECT LPAD(TO_CHAR(ZC_SJ.NEXTVAL), 4, '0') FROM DUAl");
		Log::Trace("", __FUNCTION__, "out_stock_time		= [{0}][{1}]", out_stock_time, if_db);
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			twmsm12.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			twmsm12["LOAD_SCHEME_NO"] = load_scheme_no;
			twmsm12["REC_CREATE_TIME"] = datetime;
			twmsm12["REC_REVISOR"] = s.userid;
			twmsm12["SHIFT_NO"] = v_shift_no;
			twmsm12["SHIFT_GROUP"] = v_shift_group;
			twmsm12["ARCHIVE_FLAG"] = if_db;

			if (if_db=="1")
			{
				//hwmsm12.CopyFrom(twmsm12);
				twmsm12.Insert();
				hmmsm01["MAT_NO"] = twmsm12["MAT_NO"];
				if (!hmmsm01.Query("MAT_NO"))
				{
					sprintf(s.msg, "归档表表中无材料信息！！！");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				hmmsm01.CopyFrom(twmsm12);
				hmmsm01["LOGISTICS_STATUS"] = "1";//1--装车
				hmmsm01["PRE_LOAD_FLAG"] = "1";//1--装车
				hmmsm01["FACTORY_TO"] = twmsm12["UNLOAD_CODE_FACTORY"];
				hmmsm01["DST_STOCK_CODE"] = twmsm12["UNLOAD_CODE_AREA"];
				hmmsm01["LOAD_UP_TIME"] = out_stock_time;
				hmmsm01["PRACTICE_NO"] = v_practice_no;
				hmmsm01["LOAD_SCHEME_NO"] = load_scheme_no;
				CString	upd_str = " LOAD_UP_TIME,FACTORY_TO,DST_STOCK_CODE,UNLOAD_CODE,LOGISTICS_STATUS,PRE_LOAD_FLAG,PRACTICE_NO,LOAD_SCHEME_NO ";
				hmmsm01.Update(upd_str, "MAT_NO");

				
				if (twmsm12["UNLOAD_CODE_FACTORY"].ToString() == "WXK1")
				{
					//发装车实绩
					twmsm61.CopyFrom(hmmsm01);
					twmsm61.Print();
					twmsm61.MergeFrom(bcls_rec->Tables[0].Rows[i]);
					twmsm61["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
					twmsm61["REC_CREATOR"] = s.userid;
					twmsm61["ARCHIVE_FLAG"] = " ";
					twmsm61["PRACTICE_NO"] = v_practice_no;
					twmsm61["WIDTH"] = hmmsm01["MAT_WIDTH"];
					Log::Info("", __FUNCTION__, "f_mmsm10_trace_linke   =[{0}]", __LINE__);
					twmsm61["LENGTH"] = hmmsm01["MAT_LEN"];
					twmsm61["THICK"] = hmmsm01["MAT_THICK"];
					twmsm61["WEIGHT"] = hmmsm01["MAT_WT"];
					twmsm61["SG_SIGN"] = hmmsm01["SG_GRADE_1"];
					twmsm61["DEAL_FLAG"] = "I";
					twmsm61["UNLOAD_STATE"] = "2";
					twmsm61["LOAD_END_TIME"] = datetime;
					Log::Info("", __FUNCTION__, "f_mmsm10_trace_linke   =[{0}]", __LINE__);
					//twmsm61["PRODUCT_TYPE"] = "1";
					twmsm61["TRANS_TYPE"] = "2";
					if (bcls_rec->Tables[0].Rows[i]["UNLOAD_CODE_FACTORY"].ToString() == "6390")
					{
						twmsm61["MATERIAL_CODE"] = "HAB000000000000000";
					}
					else
					{
						twmsm61["MATERIAL_CODE"] = "HAA000000000000000";
					}

					Log::Info("", __FUNCTION__, "f_mmsm10_trace_linke   =[{0}]", __LINE__);
					if (bcls_rec->Tables[0].Rows[i]["TRNP_MODE_CODE"].ToString() == "2")
					{
						twmsm61["DEALY_FLAG"] = "1";
					}
					twmsm61.Insert();
					twmsm61.MergeTo(bcls_load.Tables["21A009"], false);
				}
				

			}
			else
			{
				twmsm12.Insert();

				tmmsm96.Reset();
				tmmsm96.CopyFrom(twmsm12);
				tmmsm96["LOGISTICS_STATUS"] = "1";//1--装车
				tmmsm96["PRE_LOAD_FLAG"] = "1";//1--装车
				tmmsm96["FACTORY_TO"] = twmsm12["UNLOAD_CODE_FACTORY"];
				tmmsm96["DST_STOCK_CODE"] = twmsm12["UNLOAD_CODE_AREA"];
				tmmsm96["LOAD_UP_TIME"] = out_stock_time;
				tmmsm96["LOAD_SCHEME_NO"] = load_scheme_no;
				tmmsm96["EVENT_ID"] = "MM78";
				tmmsm96["SYSTEM_ID"] = "MMSM";
				tmmsm96["EVENT_LINE_TYPE"] = "00";
				tmmsm96["FUNC_ID"] = s.svc_name;
				tmmsm96.MergeTo(mm0099.Tables["MM0099"], false);

				tmmsm01.Reset();
				tmmsm01["MAT_NO"] = twmsm12["MAT_NO"];
				tmmsm01.Query("MAT_NO");
				tmmsm34["MAT_NO"] = tmmsm01["MAT_NO"];
				tmmsm39["MAT_NO"] = tmmsm01["MAT_NO"];

				CString c_qux = Db::QueryCString(" select CODE_DESC_1_CONTENT from twmsmzd02 where CODE_CLASS='WM02' AND CODE='" + tmmsm01["GUIDE_DEST"].ToString() + "' ");

				if (tmmsm01["COMPLEX_DECIDE_CODE"].ToString()=="1"
					&&tmmsm01["PRODUCT_FLAG"].ToString()=="0"
					&& tmmsm01["LOGISTICS_STATUS"].ToString() == "0" 
					&& (tmmsm01["C_STATESIGN"].ToString() == "0"|| tmmsm01["C_STATESIGN"].ToString() == " ")
					&&((twmsm12["UNLOAD_CODE"].ToString()=="632002002"&& c_qux.Trim()=="三轧")
						|| (twmsm12["UNLOAD_CODE"].ToString() == "635003003" && c_qux.Trim() == "1549轧机")
						|| (twmsm12["UNLOAD_CODE"].ToString() == "639002001" && c_qux.Trim() == "五轧新线")
						|| ((twmsm12["UNLOAD_CODE"].ToString() == "631003006"|| twmsm12["UNLOAD_CODE"].ToString() == "631003007") && c_qux.Trim() == "型材厂")
						||(twmsm12["UNLOAD_CODE"].ToString() == "623004002")))
				{
					/*bcls_auto_db.Tables[0].Rows.Add();
					bcls_auto_db.Tables[0].Rows[db_rows]["MAT_NO"] = tmmsm01["MAT_NO"].ToString();
					bcls_auto_db.Tables[0].Rows[db_rows]["UNLOAD_CODE"] = twmsm12["UNLOAD_CODE"].ToString();*/
					if (tmmsm01["SURF_QUALITY"].ToString()=="47"|| (tmmsm01["SURF_QUALITY"].ToString() != "47" &&(tmmsm34.QueryCount("MAT_NO")>0|| tmmsm39.QueryCount("MAT_NO") > 0)))
					{
						twmsm12.MergeTo(bcls_auto_db.Tables[0], false);
						db_rows++;
					}
				}
				
			}
			
		}
		if (mm0099.Tables[0].Rows.get_Count() > 0) {
			doFlag = f_mmsm99(&mm0099, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
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
			doFlag = f_wmsm_load_proc(&bcls_load, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		if (bcls_rec->Tables[0].Rows.get_Count() == db_rows)
		{
			sqlstr = " INSERT INTO HWMSM12 SELECT * FROM TWMSM12 WHERE LOAD_SCHEME_NO='" + load_scheme_no + "'  ";
			Log::Trace("", __FUNCTION__, "sqlstr		= [{0}][{1}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();

			twmsm12["LOAD_SCHEME_NO"] = load_scheme_no;
			twmsm12.Delete("LOAD_SCHEME_NO");

			doFlag = f_wmsm_auto_db(&bcls_auto_db, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
	}
	catch (CDbException& ex)         //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006"), arguments, 1); /*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;                  //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)//捕获应用错误
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
