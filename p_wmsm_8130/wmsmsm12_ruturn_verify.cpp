/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         KE2111
Version:		1.0
Date:			2023-12-1
Description:	回退验证
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件

//函数申明


int f_mmsm99(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wmsmsm_allot_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wmsm_t8p302_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_mmsm_e2t8m1_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);

BM2F_ENTERACE(wmsmsm12_ruturn_verify);

int f_wmsmsm12_ruturn_verify(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */

	CModel twm41dj("TWM41DJ");
	CModel tmmsm01("TMMSM01");
	CModel tmmsm96("TMMSM96");

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



	/* 业务变量 */
	EIClass mm0099;
	mm0099.Tables[0].set_TableName("MM0099");
	mm0099.Tables[0].Columns.Add(tmmsm96);
	mm0099.Tables[0].Rows.Clear();

	EIClass bcls_allot;
	bcls_allot.Tables[0].Columns.Add(twm41dj);
	bcls_allot.Tables[0].Rows.Clear();

	EIClass mm00991;
	mm00991.Tables[0].set_TableName("MM0099");
	mm00991.Tables[0].Columns.Add(tmmsm96);
	mm00991.Tables[0].Rows.Clear();


	try
	{
		/*bcls_ret->Tables.Clear();
		bcls_ret->Tables.Add();
		bcls_ret->Tables[0].Columns.Add(dt)*/

		tmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsm01.Query("MAT_NO");
		if (tmmsm01["C_STATESIGN"].ToString().Trim() != "" && tmmsm01["C_STATESIGN"].ToString().Trim() != "0")
		{
			sprintf(s.msg, "材料[%s]调拨状态，不能回退.", (const char*)tmmsm01["MAT_NO"]);
			throw CApplicationException(-1, s.msg, log.Location);
		}


		twm41dj["C_BATCHUNIT"] = tmmsm01["MAT_NO"];
		sqlstr = " select * from(select * from TWM41DJ where 1 = 1 AND C_BATCHUNIT = '"+ twm41dj["C_BATCHUNIT"].ToString()+"'  order by REC_CREATE_TIME desc) where ROWNUM = 1 ";
		Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		Log::Trace("", __FUNCTION__, "xxxxx[{0}]  ", bcls_rec->Tables[0].Rows[0]["C_ACCEPTDEPT"].ToString());
		CString s_sg_grade_1 = Db::QueryCString(" select SG_GRADE_1 from tqmts0x where ST_NO=(select ST_NO from tmmsm01 where MAT_NO='" + tmmsm01["MAT_NO"].ToString() + "') ");
		if (cmd_inq.Read())
		{
			cmd_inq.Fetch(twm41dj);
			if (twm41dj["C_SENDDEPT"].ToString() != bcls_rec->Tables[0].Rows[0]["C_ACCEPTDEPT"].ToString())
			{
				sprintf(s.msg, "材料[%s]回退厂别发生变化，不能回退.", (const char*)tmmsm01["MAT_NO"]);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (twm41dj["DELIVERY_THICKNESS"].ToDecimal() != tmmsm01["MAT_THICK"].ToDecimal())
			{
				sprintf(s.msg, "材料[%s]厚度发生变化，不能回退.", (const char*)tmmsm01["MAT_NO"]);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (twm41dj["DELIVERY_WIDTH"].ToDecimal() != tmmsm01["MAT_WIDTH"].ToDecimal())
			{
				sprintf(s.msg, "材料[%s]宽度发生变化，不能回退.", (const char*)tmmsm01["MAT_NO"]);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (twm41dj["I_RESERVECOL3"].ToDecimal() != tmmsm01["MAT_LEN"].ToDecimal())
			{
				sprintf(s.msg, "材料[%s]长度发生变化，不能回退.", (const char*)tmmsm01["MAT_NO"]);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (twm41dj["N_SENDAMOUNT"].ToDecimal() != tmmsm01["MAT_WT"].ToDecimal())
			{
				sprintf(s.msg, "材料[%s]重量发生变化，不能回退.", (const char*)tmmsm01["MAT_NO"]);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (twm41dj["STEELGRADE"].ToString() != tmmsm01["ST_NO"].ToString())
			{
				sprintf(s.msg, "材料[%s]钢种发生变化，不能回退.", (const char*)tmmsm01["MAT_NO"]);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (twm41dj["C_REMARK"].ToString() != s_sg_grade_1)
			{
				sprintf(s.msg, "材料[%s]牌号发生变化，不能回退.", (const char*)tmmsm01["MAT_NO"]);
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		else {
			sprintf(s.msg, "材料[%s]未查到调拨信息，不能回退.", (const char*)tmmsm01["MAT_NO"]);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		cmd_inq.Close();

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
		twm41dj["C_ACCEPTDEPT"] = bcls_rec->Tables[0].Rows[0]["C_ACCEPTDEPT"].ToString();//接受工厂1

		twm41dj["C_SENDSTOCK"] = tmmsm01["LGORT"];//发送库房
		twm41dj["C_ACCEPTSTOCK"] = bcls_rec->Tables[0].Rows[0]["C_ACCEPTSTOCK"].ToString();//接受库房
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
		twm41dj["I_RESERVECOL4"] = "1";//调拨类型（0-正常调拨，1-回退调拨）
		twm41dj["C_INSTOCKSIGN"] = "3";
		twm41dj["C_ISFREEZE"] = "FREE";//库存类型-
		twm41dj["C_STOCKSPEC"] = "FREE";//特殊库存标识
		twm41dj["C_ORDERID"] = tmmsm01["ORDER_NO"];//合同号
		twm41dj["I_RESERVECOL3"] = tmmsm01["MAT_LEN"];
		twm41dj["C_ACHIEVEID"] = "1";
		twm41dj["C_TRUCKNUM"] = tmmsm01["TRUCK_NO"].ToString().TrimOrBlank();
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
		Log::Trace("", __FUNCTION__, "{0}", twm41dj["C_INSTOCKSIGN"].ToString());

		twm41dj.TrimOrBlank();
		twm41dj.Insert();
		twm41dj.MergeTo(bcls_allot.Tables[0], false);

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

		if (mm00991.Tables["MM0099"].Rows.get_Count() > 0)
		{
			doFlag = f_mmsm99(&mm00991, bcls_ret, conn);
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