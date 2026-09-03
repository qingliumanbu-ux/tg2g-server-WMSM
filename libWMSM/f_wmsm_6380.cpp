/*=========================================================================
//程序名称:		f_ymsmSlabQtCheck
//隶属子系统:	WM
//产品名称:
//创建人员:		LLZ
//创建时间:		2014-06-4
//修改人员:
//修改日期:
//-----------------------------------------------------------------------

//=========================================================================*/

//#include "WM_Utility.h"
#include "stdafx.h"
#include "epex.h"
#include "math.h"
BM2_FUNCTION_IMPORT


BM2_FUNCTION_EXPORT
int f_wmsm_6380(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/*程序用变量*/
	int doFlag = 0;
	int ret = 0;
	int sqlid = 0;
	int blckNum = -1;

	int isLock = 1;//0为封锁，1为合格
	int blkNum = 0;
	CString c_datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	EPEX epex;


	/* Pro*c 标准头文件部分  */
	CString deal_flag = "";
	/********表结构引用*********/
	CModel twmsm61("TWMSM61");
	CModel twm41dj("TWM41DJ");
	CModel tmmsm01("TMMSM01");
	CModel twmsm64("TWMSM64");
	CModel twmsm61lg("TWMSM61LG");


	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);
	CDbCommand cmd_inq2(conn);
	CDbCommand cmd_inq3(conn);
	CString sqlstr = "";

	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			twmsm61lg.Reset();
			twmsm61lg.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			if (!twmsm61lg.Query("PRACTICE_NO,MAT_NO"))
			{
				twmsm61lg["REC_CREATE_TIME"] = c_datetime;
				twmsm61lg.Insert();
			}
			


			tmmsm01.Reset();
			tmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			if (tmmsm01["COMPLEX_DECIDE_CODE"].ToString() != "1")
			{
				/*sprintf(s.msg, "成品不允许发往临钢！");
				throw CApplicationException(-1, s.msg, log.Location);*/
				twmsm61lg["BACK19"] = "未综判，不允许调拨";
				twmsm61lg.Update("BACK19", "PRACTICE_NO,MAT_NO");
				continue;
			}
			if (tmmsm01["PRODUCT_FLAG"].ToString() == "1")
			{
				/*sprintf(s.msg, "成品不允许发往临钢！");
				throw CApplicationException(-1, s.msg, log.Location);*/
				twmsm61lg["BACK19"] = "成品不允许发往临钢";
				twmsm61lg.Update("BACK19", "PRACTICE_NO,MAT_NO");
				continue;
			}
			twm41dj.Reset();
			twm41dj.CopyFrom(tmmsm01);
			twm41dj["REC_CREATOR"] = s.userid;
			twm41dj["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			twm41dj["C_DELIVERYID"] = "6240" + c_datetime.SubstringNE(0, 8) + Db::QueryCString("SELECT LPAD(TO_CHAR(DB_NO.NEXTVAL), 4, '0') FROM DUAl");
			twm41dj["C_QULITYTRACEID"] = tmmsm01["HEAT_NO"];//炉号
			twm41dj["C_BATCHID"] = tmmsm01["BATCH"];//批次号
			twm41dj["C_BATCHUNIT"] = tmmsm01["MAT_NO"];//
			twm41dj["C_SENDDEPT"] = "6240";//发送工厂
			twm41dj["C_ACCEPTDEPT"] = "6380";//接受工厂1
			twm41dj["C_SENDSTOCK"] = tmmsm01["LGORT"];//发送库房
			twm41dj["C_ACCEPTSTOCK"] = "6381";//接受库房
			twm41dj["DELIVERY_THICKNESS"] = tmmsm01["MAT_THICK"];//厚度
			twm41dj["DELIVERY_WIDTH"] = tmmsm01["MAT_WIDTH"];//宽度1
			twm41dj["STEELGRADE"] = tmmsm01["ST_NO"];//钢牌号
			twm41dj["N_SENDAMOUNT"] = tmmsm01["MAT_ACT_WT"];//发送重量
			twm41dj["C_SENDUNIT"] = "TON";//发送单位
			twm41dj["N_ACCEPTAMOUNT"] = tmmsm01["MAT_WT"];//接收重量
			twm41dj["C_ACCEPTUNIT"] = "TON";//接收单位
			twm41dj["C_STATESIGN"] = "1";//调拨状态（1-未确认，2-接收，3-驳回）
			twm41dj["D_OPERATIONDATE"] = c_datetime;
			twm41dj["D_BILLDATE"] = c_datetime;
			twm41dj["T_OUTSTOCKTIME"] = c_datetime;
			twm41dj["I_STOCKMODE"] = "件次";
			twm41dj["C_REMARK"] = tmmsm01["SG_GRADE_1"];
			twm41dj["I_RESERVECOL4"] = "0";//调拨类型（0-正常调拨，1-回退调拨）
			twm41dj["C_INSTOCKSIGN"] = "3";
			twm41dj["C_ISFREEZE"] = "FREE";//库存类型-
			twm41dj["C_STOCKSPEC"] = "FREE";//特殊库存标识
			twm41dj["C_ORDERID"] = tmmsm01["ORDER_NO"];//合同号
			twm41dj["I_RESERVECOL3"] = tmmsm01["MAT_LEN"];
			twm41dj["C_ACHIEVEID"] = "1";
			twm41dj["C_TRUCKNUM"] = bcls_rec->Tables[0].Rows[0]["TRUCK_NO"].ToString();
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

			twm41dj.Insert();

			//初始化
			CString	s_tc_no = "T80RY0";
			ret = epex.Initialize(s_tc_no);
			if (ret < 0)
			{
				CFormattable arguments[] = { s_tc_no }; // 定义参数列表的数组
				CMessageFormat::Format(s.msg, _RES("YM00S0000514")/*初始化电文[{0}]失败。*/, arguments, 1);
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			//拼电文数据
			if (epex.SetValue("MAT_NO", 0, tmmsm01["MAT_NO"].ToString()) < 0
				|| epex.SetValue("DEAL_FLAG", 0, "I") < 0
				|| epex.SetValue("PLANT", 0, "6240") < 0
				|| epex.SetValue("STGE_LOC", 0, tmmsm01["LGORT"].ToString()) < 0
				|| epex.SetValue("MOVE_PLANT", 0, "6380") < 0
				|| epex.SetValue("MOVE_STLOC", 0, "6381") < 0
				|| epex.SetValue("HEAD_TEXT", 0, " ") < 0)
			{
				strcpy(s.msg, _RES("GCRSS0000015"));//系统出现异常，电文拼接出错，请联系系统维护人员。
				throw CApplicationException(-1, s.msg, s.svc_name);
			}





			if (epex.SendTele() < 0)
			{
				strcpy(s.msg, _RES("GCRSS0000032")/*电文发送失败。*/);
				sprintf(s.sysmsg, "[%s]发送失败", (const char*)s_tc_no);
				throw CApplicationException(-1, s.msg, s.svc_name);
			}


			epex.Uninitialize();


			Log::Trace("", "", "line", __LINE__);

			twmsm61lg["BACK20"] = "S";
			twmsm61lg.Update("BACK20", "PRACTICE_NO,MAT_NO");
		}

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "Database processing error. sqlcode=[{0}]." /* _RES("GCRSS0000006")*//*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		Log::Trace("", __FUNCTION__, "数据库SQL出错信息	= [{0}]", str);
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
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
