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

#include "math.h"
BM2_FUNCTION_IMPORT


BM2_FUNCTION_EXPORT
int f_wmsm_load_proc(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/*程序用变量*/
	int doFlag = 0;
	int ret = 0;
	int sqlid = 0;
	int blckNum = -1;

	int isLock = 1;//0为封锁，1为合格
	int blkNum = 0;

	
	/* Pro*c 标准头文件部分  */
	CString deal_flag = "";
	/********表结构引用*********/
	CModel twmsm61("TWMSM61");
	


	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);
	CDbCommand cmd_inq2(conn);
	CDbCommand cmd_inq3(conn);
	CString sqlstr = "";

	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


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
		twmsm61.MergeFrom(bcls_rec->Tables["21A009"].Rows[0]);

		Log::Trace("", __FUNCTION__, "mmat_no= [{0}]", twmsm61["MAT_NO"].ToString());
		sqlstr = " insert into WL_LOAD_RECORD (ARRI_STATION, ARRI_STATION_CODE, BACK1, BACK10, BACK2, BACK3, BACK4, BACK5, BACK6, BACK7,\
			BACK8, BACK9, BOARD_NO, DELA_FLAG, DG_UNIT_CODE, DG_UNIT_NAME, GJ_MODEL,												  \
			LOAD_CODE, LOAD_CODE_AREA, LOAD_CODE_FACTORY, LOAD_END_TIME, LOAD_LINE_NO, LOAD_NAME,									  \
			LOAD_SCHEME_NO, LOAD_STOCK_CODE, LOAD_WAY, MODEL_MAX, MODEL_MIX, ONDUTY_SN, OPER_NAME,									  \
			OPER_NO, PLAN_LOAD_END_TIME, PLAN_NO, RECEIVE_UNIT, RECEIVE_UNIT_CODE, SHIFTNO,											  \
			SJ_NO, STOCK_ONTRAIN, TRANS_TYPE, TRUCK_MODEL, TRUCK_MODEL_DESC, TRUCK_NO,												  \
			WAGONNO, Z_METAGE_NUM, Z_METAGE_WEIGHT, TIMESTAMPS,ID,SEND_AREA,IS_SEND)												\
			select ARRI_STATION_NAME ARRI_STATION,																					  \
			ARRI_STATION_CODE,																										  \
			BACK1,																													  \
			0                 BACK10,																								  \
			BACK2,																													  \
			BACK3,																													  \
			BACK4,																													  \
			BACK5,																													  \
			0                 BACK6,																								  \
			0                 BACK7,																								  \
			0                 BACK8,																								  \
			0                 BACK9,																								  \
			TRUCK_BOARD_NO    BOARD_NO,																								  \
			DEAL_FLAG         DELA_FLAG,																							  \
			DG_UNIT_CODE,																											  \
			DG_UNIT_NAME,																											  \
			GJ_MODEL,																												  \
			LOAD_CODE,																												  \
			LOAD_CODE_AREA,																											  \
			LOAD_CODE_FACTORY,																										  \
			sysdate           LOAD_END_TIME,																						  \
			LOAD_LINE_NO,																											  \
			LOAD_NAME,																												  \
			LOAD_SCHEME_NO,																											  \
			LOAD_STOCK_CODE,																										  \
			LOAD_WAY,																												  \
			MODEL_MAX,																												  \
			MODEL_MIN         MODEL_MIX,																							  \
			SHIFT_GROUP       ONDUTY_SN,																							  \
			EMP_NAME          OPER_NAME,																							  \
			EMP_CODE          OPER_NO,																								  \
			sysdate           PLAN_LOAD_END_TIME,																					  \
			PLAN_NO,																												  \
			RECEIVE_UNIT,																											  \
			RECEIVE_UNIT_CODE,																										  \
			SHIFT_NO          SHIFTNO,																								  \
			PRACTICE_NO       SJ_NO,																								  \
			STOCK_ONTRAIN,																											  \
			TRANS_TYPE,																												  \
			TRUCK_MODEL,																											  \
			TRUCK_MODEL_DESC,																										  \
			TRUCK_NO,																												  \
			WAGONNO,																												  \
			Z_METAGE_NUM,																											  \
			Z_METAGE_WEIGHT,																										  \
			sysdate,WL_LOAD.nextval,DECODE(ARCHIVE_FLAG,'1','j','g'),'1'																													  \
			from twmsm61																											  \
	    	where PRACTICE_NO = '"+ bcls_rec->Tables["21A009"].Rows[0]["PRACTICE_NO"].ToString() +"'								 \
			and MAT_NO = NVL('" + bcls_rec->Tables["21A009"].Rows[0]["MAT_NO"].ToString() + "',' ') ";
		//Log::Trace("", __FUNCTION__, "Mmat_no = [{0}]", bcls_rec->Tables["21A009"].Rows[0]["MAT_NO"].ToString());
		Log::Trace("", __FUNCTION__, "sqlstr		= [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " insert into WL_LOAD_RECORD_DET (BACK11, BACK12, BACK13, BACK14, BACK15, BACK16, BACK17, BACK18, BACK19, BACK20,					 \
			CONFIRM_TIME, DEALY_FLAG, MAT_ACT_WT, MAT_LEN, MAT_NO, MAT_THICK,																		 \
			MAT_WIDTH, PRODUCT_TYPE, PROD_CODE, PROD_NAME, SG_SIGN, SJ_NO,																			 \
			UNLOAD_AREA, UNLOAD_CODE_AREA, UNLOAD_FAC, UNLOAD_PLACE, ID, MAIN_ID, IS_SEND, SEND_AREA)																			 \
			select BACK11,																															 \
			BACK12,																																	 \
			BACK13,																																	 \
			BACK14,																																	 \
			BACK15,																																	 \
			0                   BACK16,																												 \
			0                   BACK17,																												 \
			0                   BACK18,																												 \
			0                   BACK19,																												 \
			0                   BACK20,																												 \
			sysdate             CONFIRM_TIME,																										 \
			DEALY_FLAG,																																 \
			MAT_WT              MAT_ACT_WT,																											 \
			MAT_LEN,																																 \
			MAT_NO,																																	 \
			MAT_THICK,																																 \
			MAT_WIDTH,																																 \
			'1'                 PRODUCT_TYPE,																										 \
			' '                 PROD_CODE,																											 \
			' '                 PROD_NAME,																											 \
			SG_SIGN,																																 \
			PRACTICE_NO         SJ_NO,																												 \
			UNLOAD_CODE_AREA    UNLOAD_AREA,																										 \
			UNLOAD_CODE_AREA,																														 \
			UNLOAD_CODE_FACTORY UNLOAD_FAC,																											 \
			UNLOAD_CODE         UNLOAD_PLACE,																										 \
			 WL_LOAD.nextval,																														  \
			(select ID from WL_LOAD_RECORD where SJ_NO = '" + bcls_rec->Tables["21A009"].Rows[0]["PRACTICE_NO"].ToString() + "'),					  \
			'Y',																																	   \
			DECODE(ARCHIVE_FLAG, '1', 'j', 'g')																										 \
			from twmsm61																															 \
		where PRACTICE_NO = '" + bcls_rec->Tables["21A009"].Rows[0]["PRACTICE_NO"].ToString() + "' ";
		Log::Trace("", __FUNCTION__, "sqlstr		= [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

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
