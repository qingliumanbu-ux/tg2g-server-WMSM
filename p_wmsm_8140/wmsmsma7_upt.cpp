/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:			QL
Version:		1.0
Date:			2016-08-05
Description:	吊车命令信息修改
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件

//函数申明

/*<remark>=========================================================
///<summary>
///吊车命令信息修改
///<para>
///2.排序方式：MAT_NO
///</para>
///<para>数据库表：TWMA7 行车命令表；
///<returns>修改传入的吊车命令信息</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wmsmsma7_upt)

int f_wmsmsma7_upt(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDecimal Count = 0;
	CString v_update = ""; 
	CString stock_place_no_to = "";
	CString stock_place_no_from = "";

	/* 实体类定义 */
	//CTWMA7 twma7(conn);
	//CTWM04 twm04(conn);
	CModel twma7 = CModel("TWMA7");
	CModel twm04_to = CModel("TWM04");
	CModel twm04_fr = CModel("TWM04");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);


	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{

			twma7.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			stock_place_no_to = twma7["STOCK_PLACE_NO_TO"].ToString();
			stock_place_no_from = twma7["STOCK_PLACE_NO_FROM"].ToString();

			Log::Trace("", "", "mat_no=[{0}]", twma7["MAT_NO"].ToString());
			Log::Trace("", "", "stock_place_no_to=[{0}]", stock_place_no_to);
			Log::Trace("", "", "stock_place_no_from=[{0}]", stock_place_no_from);

			if (twma7["MAT_NO"].ToString().Trim() == "")
			{
				sprintf(s.msg, "Material No. cannot be empty."); //材料号不能为空
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (twma7["STOCK_OPER_ORDER"].ToString().Substring(0, 1) != "1" &&
				twma7["STOCK_OPER_ORDER"].ToString().Substring(0, 1) != "3")
			{
				sprintf(s.msg, "Just can update instruction's target position of pile change/input."); //只能修改倒跺或入库的目标库位
				throw CApplicationException(-1, s.msg, log.Location);
			}
			twm04_to["STOCK_PLACE_NO"] = twma7["STOCK_PLACE_NO_TO"];
			twm04_to.Query("STOCK_PLACE_NO");
			if (//twm04_to["TOCK_PLACE_TYPE"].ToString() == "D" ||
				twm04_to["STOCK_PLACE_TYPE"].ToString() == "7")
			{
				sprintf(s.msg, "Target position cannot be equipment or depiler."); //目标库位不能为设备或备料区
				throw CApplicationException(-1, s.msg, log.Location);

			}
			twm04_fr["STOCK_PLACE_NO"] = stock_place_no_from;
			twm04_fr.Query("STOCK_PLACE_NO");

			if (twm04_to["HALL_NO"].ToString().Trim() != twm04_fr["HALL_NO"].ToString().Trim())
			{
				sprintf(s.msg, "修改后的起吊位和目标库位不在同一跨，请重新修改."); //材料号不能为空
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (!twma7.Query("MAT_NO"))
			{
				sprintf(s.msg, "Material [%s] is not exist and it cannot be updated.", (const char*)twma7["MAT_NO"]);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			twma7["REC_REVISE_TIME"] = datetime;
			twma7["REC_REVISOR"] = s.userid;
			twma7["STOCK_PLACE_NO_TO"] = stock_place_no_to;
			twma7["HALL_NO_TO"] = twm04_to["HALL_NO"].ToString().Trim();
			twma7["STOCK_PLACE_NO_FROM"] = stock_place_no_from;
			twma7["HALL_NO_FR"] = twm04_fr["HALL_NO"].ToString().Trim();
			v_update = "STOCK_PLACE_NO_FROM,HALL_NO_FR,STOCK_PLACE_NO_TO,HALL_NO_TO,REC_REVISE_TIME,REC_REVISOR";
			twma7.Update(v_update, "MAT_NO");
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
