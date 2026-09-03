/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      05501061
Version:     1.0
Date:        2023-11-20 15:13:47
Description: 厂内转运用车计划
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2_FUNCTION_EXPORT


int f_wmsm_21a008_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	int blkNum = 0;
	CString sqlstr = " ";
	//电文号
	CString cs_tc_no("");
	//电文变量
	EPEX epex(&s, conn);
	/* 实体类定义 */
	CModel twmsm60 = CModel("twmsm60");//卸车实绩表

	try
	{
		CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		

		/* 判断是否存在指定块 */
		blkNum = bcls_rec->Tables.IndexOf("21A008");
		if (blkNum < 0)
		{
			strcpy(s.msg, "传入数据块 21A008 不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		cs_tc_no = "21A008";
		//电文初始化
		if (epex.Initialize(cs_tc_no) < 0)
		{
			strncpy(s.msg, (const char*)"电文初始化失败", sizeof(s.msg) - 1);
			s.flag = -1;
			doFlag = -1;
			return doFlag;
		}
		for (int i = 0; i < bcls_rec->Tables["21A008"].Rows.get_Count(); i++)
		{
			twmsm60.MergeFrom(bcls_rec->Tables["21A008"].Rows[i]);
			Send.Result.Tables["zcho_cnzycjh_rfc"].Rows[0]["deal_flag"] = deal_flag;//操作标记
			Send.Result.Tables["zcho_cnzycjh_rfc"].Rows[0]["pc_plan_no"] = " ";//拼车计划号
			Send.Result.Tables["zcho_cnzycjh_rfc"].Rows[0]["plan_no"] = plan_no;//计划号
			Send.Result.Tables["zcho_cnzycjh_rfc"].Rows[0]["require_tr_time"] = tym01dy.USE_TRUCK_TIME;//请求用车时间
			Send.Result.Tables["zcho_cnzycjh_rfc"].Rows[0]["LOAD_CODE_FACTORY"] = tym01dy.LOAD_CODE_FACTORY;//装车工场
			Send.Result.Tables["zcho_cnzycjh_rfc"].Rows[0]["LOAD_CODE_AREA"] = tym01dy.LOAD_CODE_AREA;//装车区域
			Send.Result.Tables["zcho_cnzycjh_rfc"].Rows[0]["ycdw"] = "6310";//用车单位
			Send.Result.Tables["zcho_cnzycjh_rfc"].Rows[0]["LOAD_CODE"] = tym01dy.LOAD_CODE;//装点
			Send.Result.Tables["zcho_cnzycjh_rfc"].Rows[0]["UNLOAD_CODE_FACTORY"] = tym01dy.UNLOAD_CODE_FACTORY;//卸点工厂
			Send.Result.Tables["zcho_cnzycjh_rfc"].Rows[0]["UNLOAD_CODE_AREA"] = tym01dy.UNLOAD_CODE_AREA;//卸点区域
			Send.Result.Tables["zcho_cnzycjh_rfc"].Rows[0]["dg_unit_code"] = "6310";//发料单位
			Send.Result.Tables["zcho_cnzycjh_rfc"].Rows[0]["receive_unit_co"] = tym01dy.UNLOAD_CODE_FACTORY;//收料单位
			Send.Result.Tables["zcho_cnzycjh_rfc"].Rows[0]["goods_code"] = tym01dy.PROD_CODE;//品名
			Send.Result.Tables["zcho_cnzycjh_rfc"].Rows[0]["truck_model"] = tym01dy.TRUCK_MODEL;//车型
			Send.Result.Tables["zcho_cnzycjh_rfc"].Rows[0]["wagon_num"] = " ";//计划车数
			Send.Result.Tables["zcho_cnzycjh_rfc"].Rows[0]["quantity"] = plan_weight;//计划运量
			Send.Result.Tables["zcho_cnzycjh_rfc"].Rows[0]["meterage_type"] = "1";//过磅方式
			Send.Result.Tables["zcho_cnzycjh_rfc"].Rows[0]["bal_style"] = " ";//结算方式
			Send.Result.Tables["zcho_cnzycjh_rfc"].Rows[0]["hszg_flag"] = " ";//废钢回收直供标记
			Send.Result.Tables["zcho_cnzycjh_rfc"].Rows[0]["ydcfpch"] = " ";//原代成分批次号
			Send.Result.Tables["zcho_cnzycjh_rfc"].Rows[0]["rcjyph"] = " ";//入厂检验批号
			Send.Result.Tables["zcho_cnzycjh_rfc"].Rows[0]["remark"] = tym01dy.REMARK;//备注
			Send.Result.Tables["zcho_cnzycjh_rfc"].Rows[0]["plan_start_ti"] = tym01dy.PLAN_START_TIME;//计划开始时间
			Send.Result.Tables["zcho_cnzycjh_rfc"].Rows[0]["plan_end_ti"] = tym01dy.PLAN_END_TIME;//计划结束时间
			if (epex.SetValue(i, twmsm60) < 0)
			{
				sprintf(s.msg, "发送电文失败，原因[%s]", epex.GetMsg());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

		}

		//电文发送
		if (epex.SendTele() < 0)
		{
			strncpy(s.msg, (const char*)"电文发送失败", sizeof(s.msg) - 1);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		else
		{
			Log::Trace("", __FUNCTION__, "发送电文成功");
		}

	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}


