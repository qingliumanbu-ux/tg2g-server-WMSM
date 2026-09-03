/* **************************************************************************
*	Copyright (c) Baosight Corporation 2008 . All Rights Reserved.
*  	BM2PES 宝信生产执行系统
*****************************************************************************
*  程序名称			: f_wm00_stock_udpate
*  程序描述			: 更新库位跟踪表
*  备注说明			:
*  修改历史			:
*  		henno 2016-09-28			(ADD)程序建立
*			... ...
* **************************************************************************** */
/* C/C++ 的标准头文件部分 */
#include "stdafx.h"		// 框架头，不可删除 




BM2_FUNCTION_IMPORT
int f_wm00_cal_layerno(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

//垛位最大高度、重量修正
BM2_FUNCTION_IMPORT
int f_wm00_pileinfocal(CString stock_no, CString stock_place_no, EIClass * bcls_ret, CDbConnection * conn);

BM2_FUNCTION_IMPORT
int f_wm00_stock_chk(CString stockPlaceNo, CDbConnection *conn);

//修正垛位上的材料层号
int f_wmsmsm_modi_pile_layerno(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

BM2_FUNCTION_EXPORT
int f_wmsmsm_stock_update(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	/* ***** 程序变量 ***** */
	int doFlag = 0;
	CString v_datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDecimal v_layerno = 0;

	CString v_crane_no = " ";
	CString v_vehicle_no = " ";

	/* ***** 数据库SQL操作字符串 ***** */
	CString	sqlstr = " ";

	/* ***** 数据库操作类定义 ***** */
	CDbCommand comm(conn);
	CDbCommand comm1(conn);

	/* ***** 定义表实体对象 ***** */
	CModel twm01 = CModel("TWM01");
	CModel twm04 = CModel("TWM04");
	CModel twma2_old = CModel("TWMA2");
	CModel twma2 = CModel("TWMA2");


	//层号计算
	EIClass rec_stack_layer;
	EIClass ret_stack_layer;
	rec_stack_layer.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO");
	rec_stack_layer.Tables[0].Columns.Add(DT_STRING, "STOCK_NO");
	rec_stack_layer.Tables[0].Rows.Add();


	EIClass modi_pile_layer;
	modi_pile_layer.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO");
	modi_pile_layer.Tables[0].Columns.Add(DT_STRING, "STOCK_NO");
	modi_pile_layer.Tables[0].Columns.Add(DT_DECIMAL, "LAYERNO");
	modi_pile_layer.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
	modi_pile_layer.Tables[0].Rows.Add();



	/* ***** 应用程序开始处理 ***** */
	try
	{
		if (!bcls_rec->Tables.Contains("WM_STOCK_UPDATE"))
		{
			sprintf(s.msg, "函数f_wm00_stock_udpate中找不到接收块名[WM_STOCK_UPDATE]");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		for (int iRow = 0; iRow < bcls_rec->Tables["WM_STOCK_UPDATE"].Rows.get_Count(); iRow++)
		{
			//获取传入参数
			twma2.Reset();
			twma2.MergeFrom(bcls_rec->Tables["WM_STOCK_UPDATE"].Rows[iRow]);
			twma2.TrimOrBlank();

			if (bcls_rec->Tables["WM_STOCK_UPDATE"].Columns.Contains("CRANE_NO"))
			{
				v_crane_no = bcls_rec->Tables["WM_STOCK_UPDATE"].Rows[iRow]["CRANE_NO"].ToString().Trim();
			}
			if (bcls_rec->Tables["WM_STOCK_UPDATE"].Columns.Contains("CRANE_NO"))
			{
				v_vehicle_no = bcls_rec->Tables["WM_STOCK_UPDATE"].Rows[iRow]["VEHICLE_NO"].ToString().Trim();
			}

			Log::Trace("", __FUNCTION__, "传入参数 twma2.MAT_NO\t[{0}]", twma2["MAT_NO"].ToString());
			Log::Trace("", __FUNCTION__, "传入参数 twma2.STOCK_NO\t[{0}]", twma2["STOCK_NO"].ToString());
			Log::Trace("", __FUNCTION__, "传入参数 twma2.STOCK_PLACE_NO\t[{0}]", twma2["STOCK_PLACE_NO"].ToString());
			Log::Trace("", __FUNCTION__, "传入参数 twma2.LAYERNO\t[{0}]", twma2["LAYERNO"].ToDecimal());
			Log::Trace("", __FUNCTION__, "传入参数 v_crane_no\t[{0}]", v_crane_no);
			Log::Trace("", __FUNCTION__, "传入参数 v_vehicle_no\t[{0}]", v_vehicle_no);


			//清除老库位
			twma2_old.Reset();
			twma2_old["MAT_NO"] = twma2["MAT_NO"];
			if (twma2_old.Query("MAT_NO"))
			{


				if (twma2_old["STOCK_PLACE_NO"].ToString().Trim() == "" &&
					twma2_old["OLD_STOCK_PLACE_NO"].ToString().Trim() != "")
				{
					//对于行车落下时  库位上原有的东西会被踢出去 记录原库位，按照原库位操作
					twma2_old["STOCK_PLACE_NO"] = twma2_old["OLD_STOCK_PLACE_NO"];
					twma2_old["STOCK_PLACE_POSITION"] = twma2_old["OLD_STOCK_PLACE_POSITION"];
				}


				//有原库位
				Log::Trace("", __FUNCTION__, "原位置 twma2_old.STOCK_NO\t[{0}]", twma2_old["STOCK_NO"].ToString());
				Log::Trace("", __FUNCTION__, "原位置 twma2_old.STOCK_PLACE_NO\t[{0}]", twma2_old["STOCK_PLACE_NO"].ToString());
				Log::Trace("", __FUNCTION__, "原位置 twma2_old.LAYERNO\t[{0}]", twma2_old["LAYERNO"].ToString());

				twma2_old.Delete("MAT_NO");

				////////////////////////将该垛位上被移走材料上面的层号需要 - 1////////////////////// 
				modi_pile_layer.Tables[0].Rows[0]["STOCK_PLACE_NO"] = twma2_old["STOCK_PLACE_NO"];
				modi_pile_layer.Tables[0].Rows[0]["STOCK_NO"] = twma2_old["STOCK_NO"];
				modi_pile_layer.Tables[0].Rows[0]["LAYERNO"] = twma2_old["LAYERNO"];

				doFlag = f_wmsmsm_modi_pile_layerno(&modi_pile_layer, bcls_ret, conn);
				if (doFlag != 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}


				////////////////////////计算老库位的状态///////////////////////////////// 
				doFlag = f_wm00_pileinfocal(twma2_old["STOCK_NO"].ToString(),
					twma2_old["STOCK_PLACE_NO"].ToString(), bcls_ret, conn);
				if (doFlag != 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}


			//插入新库位
			if (twma2["STOCK_PLACE_NO"].ToString().Trim() != "")
			{
				twm01["STOCK_NO"] = twma2["STOCK_NO"];
				if (!twm01.Query("STOCK_NO"))
				{
					CFormattable arguments[] = { twm01["STOCK_NO"].ToString() };// 定义参数列表的数组
					//printf(s.msg, _RES("YM00S0000750")/*库区号：[ {0}] 不存在!*/, arguments, 1);
					CString s_message = "库区号：" + twm01["STOCK_NO"].ToString() + "不存在!";
					sprintf(s.msg, s_message);
					throw CApplicationException(-1, s.msg, log.Location);
				}

				twm04["STOCK_PLACE_NO"] = twma2["STOCK_PLACE_NO"];

				if (!twm04.Query("STOCK_PLACE_NO"))
				{
					sprintf(s.msg, "库位号不存在");
					throw CApplicationException(-1, s.msg, log.Location);
				}

				//计算入库层号（f_wm00_cal_layerno）
				rec_stack_layer.Tables[0].Rows[0]["STOCK_PLACE_NO"] = twma2["STOCK_PLACE_NO"];
				rec_stack_layer.Tables[0].Rows[0]["STOCK_NO"] = twma2["STOCK_NO"];

				doFlag = f_wm00_cal_layerno(&rec_stack_layer, &ret_stack_layer, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				v_layerno = ret_stack_layer.Tables[0].Rows[0]["LAYERNO"].ToDecimal();



				///插入twma2
				//twma2.CopyFrom(twm04);
				twma2["REC_CREATOR"] = twma2_old["REC_CREATOR"];
				twma2["REC_CREATE_TIME"] = twma2_old["REC_CREATE_TIME"];
				twma2["REC_REVISOR"] = s.userid;
				twma2["REC_REVISE_TIME"] = v_datetime;
				twma2["STOCK_NO"] = twm04["STOCK_NO"];
				twma2["STOCK_PLACE_NO"] = twm04["STOCK_PLACE_NO"];

				twma2["STOCK_OPER_TIME"] = v_datetime;
				twma2["FACTORY_DIV"] = twm01["FACTORY_DIV"];

				twma2["LAYERNO"] = v_layerno;
				twma2["VEHICLE_NO"] = v_vehicle_no;

				twma2.TrimOrBlank();
				twma2.Insert();

				////////////////////////计算新库位的状态///////////////////////////////// 
				doFlag = f_wm00_pileinfocal(twma2["STOCK_NO"].ToString(), twma2["STOCK_PLACE_NO"].ToString(), bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}


				///校验库位是否超限
				doFlag = f_wm00_stock_chk(twma2["STOCK_PLACE_NO"].ToString(), conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
		}
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };

		/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006"), arguments, 1);
		CString str = ex.GetMsg() + "\r\n" + sqlstr;

		/*返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应*/
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);

		/*数据库异常时返回-1，事务将被回滚*/
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), 399);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), 399);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}
