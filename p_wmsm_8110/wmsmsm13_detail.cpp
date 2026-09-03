/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         JHZHAO
Version:		1.0
Date:			2016-02-01
Description:	直供出坯确认材料明细查询
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件
//#include "tmmsm01.h"


//业务头文件


//函数申明

/*<remark>=========================================================
///<summary>
///直供出坯确认材料明细查询
///<para>
///</para>
///<para>数据库表：TMMSM01坯料主档表
///<returns>返回符合查询条件的材料明细信息</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wmsmsm13_detail);

int f_wmsmsm13_detail(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;

	/* 实体类定义 */
	//CTMMSM01 tmmsm01(conn);
	CModel tmmsm01 = CModel("TMMSM01");


	/* 数据库SQL操作字符串 */
	CString sqlstr = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		bcls_ret->Tables[0].Columns.Add(tmmsm01);

		tmmsm01["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		tmmsm01["UNIT_CODE"] = bcls_rec->Tables[0].Rows[0]["UNIT_CODE"].ToString().Trim();
		tmmsm01["ST_NO"] = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString().Trim();
		tmmsm01["SG_SIGN"] = bcls_rec->Tables[0].Rows[0]["SG_SIGN"].ToString().Trim();
		tmmsm01["PONO"] = bcls_rec->Tables[0].Rows[0]["PONO"].ToString().Trim();
		tmmsm01["ORDER_NO"] = bcls_rec->Tables[0].Rows[0]["ORDER_NO"].ToString().Trim();

		if (tmmsm01["HEAT_NO"].ToString().Trim() == "")
		{
			strcpy(s.msg, "熔炼号不能为空。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		sqlstr =
			" SELECT MAT_NO,ST_NO,MAT_ACT_THICK,MAT_ACT_WIDTH,MAT_ACT_LEN,MAT_ACT_WT,"
			" MAT_NUM,SLABTOP_FLAG,SLABTOP_TIME,MAT_DESTION,SLAB_DEST_1,DEST_FIN,OUT_STOCK_TIME,"
			" OUT_SM_STOCK_TIME,STOCK_PLACE_NO,CURRENT_STATUS,MEASURE_WT_FLAG,HOT_SEND_FLAG,HOT_CHARGE_FLAG,DEFECT_CODE "
			" FROM"
			" (SELECT MAT_NO,ST_NO,MAT_ACT_THICK,MAT_ACT_WIDTH,MAT_ACT_LEN,MAT_ACT_WT,"
			" MAT_NUM,SLABTOP_FLAG,SLABTOP_TIME,MAT_DESTION,SLAB_DEST_1,DEST_FIN,OUT_STOCK_TIME,"
			" OUT_SM_STOCK_TIME,STOCK_PLACE_NO,"
			" CASE WHEN STOCK_NO = '' AND SUBSTR(STOCK_PLACE_NO, 1, 2) = 'GD' THEN '辊道'"
			" WHEN STOCK_NO NOT LIKE 'Y%' THEN '下分厂已入库'"
			" WHEN STOCK_NO LIKE 'Y%' THEN '已出炼钢库'"
			" ELSE '其它'"
			" END AS CURRENT_STATUS,"
			//" DECODE(SUBSTR(STOCK_PLACE_NO, 1, 2),'GD','辊道',DECODE(IN_FLAG,'1','已入库','炼钢待入库')) CURRENT_STATUS,"
			" MEASURE_WT_FLAG,HOT_SEND_FLAG,HOT_CHARGE_FLAG,DEFECT_CODE"
			" FROM TMMSM01"
			" WHERE HEAT_NO = @heat_no"
			" UNION ALL"
			" SELECT MAT_NO,ST_NO,MAT_ACT_THICK,MAT_ACT_WIDTH,MAT_ACT_LEN,MAT_ACT_WT,MAT_NUM,"
			" SLABTOP_FLAG,SLABTOP_TIME,MAT_DESTION,SLAB_DEST_1,DEST_FIN,OUT_STOCK_TIME,"
			" OUT_SM_STOCK_TIME,STOCK_PLACE_NO,'已出炼钢库' CURRENT_STATUS,"
			" MEASURE_WT_FLAG,HOT_SEND_FLAG,HOT_CHARGE_FLAG,DEFECT_CODE"
			" FROM HMMSM01"
			" WHERE HEAT_NO = @heat_no)"
			" ORDER BY MAT_NO";

		sqlstr =
			" SELECT * FROM "
			" (SELECT T.*,"
			//" CASE WHEN STOCK_NO = ' ' THEN '辊道'"
			//" WHEN STOCK_NO <> ' ' AND STOCK_NO NOT LIKE 'Y%' THEN '下分厂已入库'"
			//" WHEN STOCK_NO LIKE 'Y%' AND STOCK_PLACE_NO <> ' ' THEN '炼钢已入库'"
			//" WHEN STOCK_NO LIKE 'Y%' AND STOCK_PLACE_NO = ' ' THEN '炼钢已出库'"
			//" ELSE '炼钢待入库'"
			//" END AS CURRENT_STATUS"
			" CASE WHEN STOCK_NO = ' ' AND HOT_SEND_FLAG IN ('1', '2') THEN '产出待热送'"
			" WHEN STOCK_NO = ' ' AND HOT_SEND_FLAG NOT IN ('1', '2')  THEN '产出待入库'"
			" WHEN T.IN_FLAG = '1' AND HOT_SEND_FLAG IN ('1', '2') AND EXISTS (SELECT NULL FROM TWM04 where stock_place_no = t.stock_place_no and stock_place_type = 'A')  THEN '热送临时下线'"
			" WHEN STOCK_NO <> ' ' AND STOCK_PLACE_NO <> ' '"
			" AND EXISTS (SELECT NULL FROM TWM01 T2"
			" WHERE T.STOCK_NO = T2.STOCK_NO"
			" AND MAT_LINE_TYPE <> 'SM') THEN '下分厂已入库'"
			" WHEN STOCK_NO <> ' ' AND STOCK_PLACE_NO <> ' '"
			" AND EXISTS (SELECT NULL FROM TWM01 T2"
			" WHERE T.STOCK_NO = T2.STOCK_NO"
			" AND MAT_LINE_TYPE = 'SM') THEN '炼钢已入库'"
			" WHEN STOCK_NO <> ' ' AND STOCK_PLACE_NO = ' '"
			" AND EXISTS (SELECT NULL FROM TWM01 T2"
			" WHERE T.STOCK_NO = T2.STOCK_NO"
			" AND MAT_LINE_TYPE = 'SM')"
			" AND EXISTS (SELECT NULL FROM TWMA0 T2"
			" WHERE T.MAT_NO = T2.MAT_NO AND T.STOCK_NO = T2.STOCK_NO"
			" AND T2.STOCK_OPER_ORDER = '1Q') THEN '下分厂拒收'"
			/*" WHEN STOCK_NO <> ' ' AND STOCK_PLACE_NO = ' '"
			" AND EXISTS (SELECT NULL FROM TWM01 T2"
			" WHERE T.STOCK_NO = T2.STOCK_NO"
			" AND MAT_LINE_TYPE = 'SM') THEN '炼钢已出库'"*/

			//依据物料主档LOGISTICS_STATUS值区分车辆出库和辊道出库  杨晓明 2020.4.24
			" WHEN STOCK_NO <> ' ' AND STOCK_PLACE_NO = ' ' " 
			" AND EXISTS(SELECT NULL FROM TWM01 T2 " 
			" WHERE T.STOCK_NO = T2.STOCK_NO "
			" AND MAT_LINE_TYPE = 'SM' AND LOGISTICS_STATUS = '2X') THEN '炼钢已辊道出库' "
			" WHEN STOCK_NO <> ' ' AND STOCK_PLACE_NO = ' ' "
			" AND EXISTS(SELECT NULL FROM TWM01 T2 "
			" WHERE T.STOCK_NO = T2.STOCK_NO "
			" AND MAT_LINE_TYPE = 'SM' AND LOGISTICS_STATUS = '2G') THEN '炼钢已车辆出库' "
			" WHEN STOCK_NO <> ' ' AND STOCK_PLACE_NO = ' ' "
			" AND EXISTS(SELECT NULL FROM TWM01 T2 "
			" WHERE T.STOCK_NO = T2.STOCK_NO "
			" AND MAT_LINE_TYPE = 'SM' AND LOGISTICS_STATUS != '2X' AND LOGISTICS_STATUS != '2G') THEN '炼钢已出库' "

			" END AS CURRENT_STATUS"
			" FROM TMMSM01 T WHERE HEAT_NO = @heat_no AND UNIT_CODE=@unit_code AND ST_NO=@st_no AND SG_SIGN=@sg_sign AND PONO=@pono AND ORDER_NO=@order_no"
			" UNION ALL"
			" SELECT T.*,"
			" '下分厂已入库' AS CURRENT_STATUS"
			" FROM HMMSM01 T WHERE HEAT_NO = @heat_no AND UNIT_CODE=@unit_code AND ST_NO=@st_no AND SG_SIGN=@sg_sign AND PONO=@pono AND ORDER_NO=@order_no)"
			" ORDER BY MAT_NO";

		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);

		Log::Trace("", __FUNCTION__, "HEAT_NO: {0}", tmmsm01["HEAT_NO"].ToString());
		Log::Trace("", __FUNCTION__, "UNIT_CODE: {0}", tmmsm01["UNIT_CODE"].ToString());
		Log::Trace("", __FUNCTION__, "ST_NO: {0}", tmmsm01["ST_NO"].ToString());
		Log::Trace("", __FUNCTION__, "SG_SIGN: {0}", tmmsm01["SG_SIGN"].ToString());
		Log::Trace("", __FUNCTION__, "PONO: {0}", tmmsm01["PONO"].ToString());
		Log::Trace("", __FUNCTION__, "ORDER_NO: {0}", tmmsm01["ORDER_NO"].ToString());

		cmd_inq.Parameters.Set("heat_no", tmmsm01["HEAT_NO"].ToString());
		cmd_inq.Parameters.Set("unit_code", tmmsm01["UNIT_CODE"].ToString());
		cmd_inq.Parameters.Set("st_no", tmmsm01["ST_NO"].ToString());
		cmd_inq.Parameters.Set("sg_sign", tmmsm01["SG_SIGN"].ToString());
		cmd_inq.Parameters.Set("pono", tmmsm01["PONO"].ToString());
		cmd_inq.Parameters.Set("order_no", tmmsm01["ORDER_NO"].ToString().TrimOrBlank());
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		Log::Trace("", __FUNCTION__, "count: {0}", bcls_ret->Tables[0].Rows.get_Count());
		/* 关闭数据库操作类 */
		cmd_inq.Close();


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