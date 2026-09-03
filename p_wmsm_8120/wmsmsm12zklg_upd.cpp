/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:
Version:		1.0
Date:			2016-03-05
Description:	出库修改
**************************************************/

//框架头文件
#include "stdafx.h"
//#include "smhs.h"

//程序用头文件
//#include "twma0.h"
//#include "twma1.h"
//#include "twma2.h"
//#include "twm01.h"



//函数申明
BM2_FUNCTION_IMPORT



BM2F_ENTERACE(wmsmsm12zklg_upd)
int f_wmsmsm12zklg_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	/* 实体类定义 */

	CModel twm41dj("TWM41DJ");
	CModel twm42("TWM42");
	CModel twm41("TWM41");
	/* 数据库SQL操作字符串 */
	CString sqlstr = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);



	try
	{

		/* 获得输入参数 */
		for (int i = 0; i < bcls_rec->Tables[1].Rows.get_Count(); i++)
		{
			twm42.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			if (!twm42.Query("TRANSFER_PLAN_NO,TRANSFER_BILL_NO,MAT_NO")){
				sprintf(s.msg, "材料号" + twm42["MAT_NO"].ToString() + "在转库材料表中不存在");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			/*获取第一条记录的原车号信息*/
			if (i == bcls_rec->GetRow(1) && strcmp(tymsm53.mat_no, " ") != 0)
			{
				strcpy(v_aim_store, "000");
				//获取目的库区
				EXEC SQL
					SELECT AIM_STORE
				INTO : v_aim_store
					   FROM TYMSM52
					   WHERE	TRANSFER_PLAN_NO = TRIM(:tymsm53.transfer_plan_no)
					   AND TRANSFER_BILL_NO = TRIM(:tymsm53.transfer_bill_no)
					   ;
				if (((0 == strcmp("TA1", v_aim_store)) || (0 == strcmp("TB1", v_aim_store)) || (0 == strcmp("TC1", v_aim_store))))
				{
					//获取原车号
					EXEC SQL
						SELECT TRIM(WAGONNO_TS)
					INTO :v_wagonno_ts_old
						  FROM TYMSM53
						  WHERE	MAT_NO = : tymsm53.mat_no
						  AND TRANSFER_PLAN_NO = TRIM(:tymsm53.transfer_plan_no)
						  AND TRANSFER_BILL_NO = TRIM(:tymsm53.transfer_bill_no)
						  ;
				}
			}
			//更新修改信息
			if (strcmp(tymsm53.mat_no, " ") != 0)
			{
				EXEC SQL
					update tymsm53
					set DELEGATE_SEQ = :tymsm53.delegate_seq,
					wagonno_ts = : tymsm53.wagonno_ts
				where mat_no = : tymsm53.mat_no
				AND TRANSFER_PLAN_NO = TRIM(:tymsm53.transfer_plan_no)
					AND TRANSFER_BILL_NO = TRIM(:tymsm53.transfer_bill_no)
					;
			}
			if (i == bcls_rec->GetRow(1) && strcmp(tymsm53.mat_no, " ") != 0 && 0 != strcmp(tymsm53.wagonno_ts, v_wagonno_ts_old))
			{
				//发送外转库时称重请求
				EDLog(1, 1, "-------二炼钢至厂外库称重电文开始----目的库区 ：%s---", v_aim_store);
				blckSndWeight.SetColName(1, 1, "delegate_seq");
				blckSndWeight.SetColVal(1, 1, "delegate_seq", tymsm53.delegate_seq);
				doFlag = f_ym193103_snd(&blckSndWeight, bcls_ret);

				if (0 != doFlag)
				{
					strcpy(s.msg, "发送外转库称重电文出错！");
					goto l_return;
				}
			}
		}

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		Log::Trace("", __FUNCTION__, "s.flag[{0}]", s.flag);
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

