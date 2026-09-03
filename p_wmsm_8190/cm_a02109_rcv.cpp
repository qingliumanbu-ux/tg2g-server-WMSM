/*
*  程序名称			: cm_A02109_rcv
*  程序描述			: 汽运、铁路卸车实绩
*
*  	2023-11-20 	聂媛媛			(ADD)程序建立
*			... ...
* **************************************************************************** */
/*<remark>=========================================================
<summary>
汽运、铁路卸车实绩，针对二钢装车实绩修改卸车标记,
<para>数据库表：TWMSM61(装车实绩表)         </para>
根据异常卸货标记判断，若为空，代表正常，J代表拒收
正常，卸车，卸车状态更新，确认状态改为9，若不是今日的计划，则判断关闭，更新TMMSM01物流状态
</summary>
<returns>电文处理成功与否</returns>
===========================================================</remark>*/

/* C/C++ 的标准头文件部分 */
#include "stdafx.h"
#include "epex.h"
#include "CUtils.h"
//#include "x_psi_tel.h"
using namespace BM2;
using namespace BM2::Data;
using namespace BM2::Data::DbClient;

int f_mmsm99(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wmsm_wm07_proc(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wmsm_6380(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wmsmsm_stock_in(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//调用仓库接口，进行板坯入库   太钢定制

//// service入口
BM2F_ENTERACE_TELE(cm_a02109_rcv)
/* ***** -EP_SYSTEM_HEAD_END ***** */
int f_cm_a02109_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/*新增电文头部分*/

	int  blkNum = 0;

	CString		c_msgtype = "";
	CString		c_freeuse1 = "";
	CString		c_freeuse2 = "";
	CString		c_freeuse3 = "";
	CString		c_freeuse4 = "";
	CString		c_freeuse5 = "";
	CString		c_send_key = "";
	CString     input_t_name = "";
	CString     input_t_rout1 = "";
	CString     input_t_rout2 = "";
	CString     input_send_key = "";
	CString     output_t_name = "";
	CString     output_t_rout1 = "";
	CString     output_t_rout2 = "";
	CString     output_send_key = "";

	/*添加并设置块名*/



	/* ***** 静态变量定义 ***** */
	int doFlag = 0;
	int row_count = 0, i = 0, ret = 0;
	CString	record_name = "";
	CString lpsz_user_id, c_datetime = s.datetime;
	CString	lpsz_out_div;
	CString	c_plan_no = "";
	CString	c_truck_no = "";

	EIClass sm_bcls_rec;

	CModel twmsm61("TWMSM61");
	CModel tmmsm01("TMMSM01");
	CModel hmmsm01("HMMSM01");
	CModel tmmsm96("TMMSM96");
	CModel twmsm12("TWMSM12");
	CModel hwmsm12("HWMSM12");


	/* ***** 电文变量定义 ***** */



	/* ***** 程序变量 ***** */
	CString c_user = " ", c_tc_no = " ", c_mat_kind = " ", datetime = " ";

	/* ***** 数据库SQL操作字符串 ***** */
	CString	sqlstr(""), upd_str(""), s_message("");

	/* ***** 数据库操作类定义 ***** */
	CDbCommand execute_sql(conn);
	CDbCommand cmd_sql(conn);
	//调用物料事件
	EIClass mm0099;
	mm0099.Tables[0].set_TableName("MM0099");
	mm0099.Tables[0].Columns.Add(tmmsm96);
	mm0099.Tables[0].Rows.Clear();

	//调用归档函数
	EIClass wm07;
	wm07.Tables[0].Columns.Add(DT_STRING,"MAT_NO");
	wm07.Tables[0].Rows.Clear();
	int row_07 = 0;

	blkNum = bcls_rec->Tables.IndexOf("WM_STOCK");
	if (blkNum < 0)
	{
		bcls_rec->Tables.Add("WM_STOCK");
		bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "MAT_NO");
		bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");        //库业务类型
		bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_DECIMAL, "STOCK_OPER_ORDER_DIV");   //业务类型内区分
		bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "STOCK_NO");				//库号
		bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "STOCK_PLACE_NO");			//材料库位号
		bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "ROWNO");					//行号
		bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "COLUMN_NO");				//列号
		bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "LAYERNO");					//层号
		bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "STOCK_PLACE_POSITION");	//库位内位置
	}
	bcls_rec->Tables["WM_STOCK"].Rows.Clear();
	int wm_stock_count = 0;

	EIClass DB_6380;
	DB_6380.Tables[0].Columns.Add(tmmsm01);
	DB_6380.Tables[0].Rows.Clear();


	/* ***** 应用程序开始处理 ***** */
	try
	{
		/* ***** 获取电文号 ***** */
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		c_tc_no = s.username;
		c_user = c_tc_no;

		/* ***** 获取电文 ***** */

		sleep(10);
		//异常卸货标记：Y：异常  J：拒收
		CString yc_flag = bcls_rec->Tables["A02109"].Rows[0]["YC_FLAG"];
		//操作标记：U：更新
		CString deal_flag = bcls_rec->Tables["A02109"].Rows[0]["DEAL_FLAG"].ToString().Trim();//咨询反馈：卸车不会有U，只会为I，所以不校验处理标记
		
		
		//更新
		
		for (int i = 0; i < bcls_rec->Tables["A02109_1"].Rows.get_Count(); i++)
		{
			if (bcls_rec->Tables["A02109_1"].Rows[i]["BACK12"].ToString().SubstringNE(0, 4) != "21JF")
			{

				twmsm61["PLAN_NO"] = bcls_rec->Tables["A02109_1"].Rows[i]["BACK12"].ToString();	//计划号
				twmsm61["PRACTICE_NO"] = bcls_rec->Tables["A02109_1"].Rows[i]["BACK13"].ToString();		//实绩号
				upd_str = "";
				twmsm61["MAT_NO"] = bcls_rec->Tables["A02109_1"].Rows[i]["MAT_NO"].ToString().Trim();			//材料号
				tmmsm01["MAT_NO"] = bcls_rec->Tables["A02109_1"].Rows[i]["MAT_NO"].ToString().Trim();			//材料号
				hmmsm01["MAT_NO"] = bcls_rec->Tables["A02109_1"].Rows[i]["MAT_NO"].ToString().Trim();			//材料号
				twmsm12["MAT_NO"] = bcls_rec->Tables["A02109_1"].Rows[i]["MAT_NO"].ToString().Trim();			//材料号

				if (!twmsm61.Query("PRACTICE_NO,MAT_NO") && twmsm61["PRACTICE_NO"].ToString() != "")
				{
					strncpy(s.msg, (const char*)"没有该材料的装车信息。", sizeof(s.msg) - 1);
					s.flag = -1;
					doFlag = -1;
					return doFlag;
				}
				//获取对应实绩

				//只能更新待卸车
				if (twmsm61["UNLOAD_STATE"].ToString() == "2")
				{
					
					//if (bcls_rec->Tables["A02109"].Rows[0]["TRANS_TYPE"].ToString().Trim() != "")
					//{
					//	twmsm61["TRANS_TYPE"] = bcls_rec->Tables["A02109"].Rows[0]["TRANS_TYPE"].ToString().Trim();					//运输方式
					//	upd_str += "TRANS_TYPE,";
					//}
					//if (bcls_rec->Tables["A02109"].Rows[0]["TRUCK_NO"].ToString().Trim() != "")
					//{
					//	twmsm61["TRUCK_NO"] = bcls_rec->Tables["A02109"].Rows[0]["TRUCK_NO"].ToString().Trim();						//车号
					//	upd_str += "TRUCK_NO,";
					//}
					//if (bcls_rec->Tables["A02109"].Rows[0]["TRUCK_BOARD_NO"].ToString().Trim() != "")
					//{
					//	twmsm61["TRUCK_BOARD_NO"] = bcls_rec->Tables["A02109"].Rows[0]["TRUCK_BOARD_NO"].ToString().Trim();			//车板号
					//	upd_str += "TRUCK_BOARD_NO,";
					//}
					if (bcls_rec->Tables["A02109"].Rows[0]["UNLOAD_CODE_FACTORY"].ToString().Trim() != "")
					{
						twmsm61["UNLOAD_CODE_FACTORY"] = bcls_rec->Tables["A02109"].Rows[0]["UNLOAD_CODE_FACTORY"].ToString().Trim();	//卸点工厂
						upd_str += "UNLOAD_CODE_FACTORY,";
					}
					if (bcls_rec->Tables["A02109"].Rows[0]["UNLOAD_CODE_AREA"].ToString().Trim() != "")
					{
						twmsm61["UNLOAD_CODE_AREA"] = bcls_rec->Tables["A02109"].Rows[0]["UNLOAD_CODE_AREA"].ToString().Trim();		//卸点区域
						upd_str += "UNLOAD_CODE_AREA,";
					}
					if (bcls_rec->Tables["A02109"].Rows[0]["UNLOAD_STOCK_CODE"].ToString().Trim() != "")
					{
						twmsm61["UNLOAD_STOCK_CODE"] = bcls_rec->Tables["A02109"].Rows[0]["UNLOAD_STOCK_CODE"].ToString().Trim();		//卸点库区代码
						upd_str += "UNLOAD_STOCK_CODE,";
					}
					if (bcls_rec->Tables["A02109"].Rows[0]["UNLOAD_CODE"].ToString().Trim() != "")
					{
						twmsm61["UNLOAD_CODE"] = bcls_rec->Tables["A02109"].Rows[0]["UNLOAD_CODE"].ToString().Trim();					//卸点
						upd_str += "UNLOAD_CODE,";
					}
					if (bcls_rec->Tables["A02109"].Rows[0]["UNLOAD_END_TIME"].ToString().Trim() != "")
					{
						twmsm61["UNLOAD_END_TIME"] = bcls_rec->Tables["A02109"].Rows[0]["UNLOAD_END_TIME"].ToString().Trim();			//卸车结束时间
						upd_str += "UNLOAD_END_TIME,";
					}
					else
					{
						twmsm61["UNLOAD_END_TIME"] = datetime;			//卸车结束时间
						upd_str += "UNLOAD_END_TIME,";
					}
					if (bcls_rec->Tables["A02109"].Rows[0]["EMP_CODE"].ToString().Trim() != "")
					{
						twmsm61["EMP_CODE"] = bcls_rec->Tables["A02109"].Rows[0]["EMP_CODE"].ToString().Trim();						//操作员工号
						upd_str += "EMP_CODE,";
					}
					if (bcls_rec->Tables["A02109"].Rows[0]["EMP_NAME"].ToString().Trim() != "")
					{
						twmsm61["EMP_NAME"] = bcls_rec->Tables["A02109"].Rows[0]["EMP_NAME"].ToString().Trim();						//操作员姓名
						upd_str += "EMP_NAME,";
					}
					if (bcls_rec->Tables["A02109_1"].Rows[0]["DEDUCT_WEIGHT"].ToString().Trim() != "")
					{
						twmsm61["DEDUCT_WEIGHT"] = bcls_rec->Tables["A02109_1"].Rows[i]["DEDUCT_WEIGHT"].ToString().Trim();						//钢牌号
						upd_str += "DEDUCT_WEIGHT,";
					}

					if (tmmsm01.Query("MAT_NO")) {
						if (yc_flag.Trim() == "")							//卸车
						{
							twmsm61["UNLOAD_STATE"] = "3";
							twmsm61["AFFIRM_FLAG"] = "9";
							upd_str += "UNLOAD_STATE,AFFIRM_FLAG,";
							//15、调物流事件
							tmmsm96.Reset();
							tmmsm96.CopyFrom(tmmsm01);
							tmmsm96["MAT_NO"] = twmsm61["MAT_NO"];
							tmmsm96["LOGISTICS_STATUS"] = "3";//2--装车确认
							tmmsm96["FACTORY_TO"] = twmsm61["UNLOAD_CODE_FACTORY"];
							tmmsm96["DST_STOCK_CODE"] = twmsm61["UNLOAD_CODE_AREA"];
							tmmsm96["EVENT_ID"] = "MM77";
							tmmsm96["SYSTEM_ID"] = "MMSM";
							tmmsm96["EVENT_LINE_TYPE"] = "00";
							tmmsm96["FUNC_ID"] = s.svc_name;
							tmmsm96.MergeTo(mm0099.Tables["MM0099"], false);

							tmmsm01.Query("MAT_NO");
							if (tmmsm01["LOGISTICS_STATUS"].ToString() == "3"
								&& tmmsm01["C_STATESIGN"].ToString() == "3")
							{
								wm07.Tables[0].Rows.Add();
								wm07.Tables[0].Rows[row_07]["MAT_NO"] = tmmsm01["MAT_NO"].ToString();
								row_07++;
							}

							CString if_lg = Db::QueryCString("select CODE_DESC_3_CONTENT from TWMSMZD02 where CODE_CLASS='WM02' AND CODE ='" + tmmsm01["GUIDE_DEST"].ToString() + "'");
							if (if_lg.Find("4") >= 0&& tmmsm96["FACTORY_TO"].ToString()=="TBZX")
							{
								tmmsm01.MergeTo(DB_6380.Tables[0], false);
							}
							
						}
						else if (yc_flag == "J")					//拒收
						{
							twmsm61["UNLOAD_STATE"] = "4";
							upd_str += "UNLOAD_STATE,";
							//15、调物流事件
							tmmsm96.Reset();
							tmmsm96.CopyFrom(tmmsm01);
							tmmsm96["MAT_NO"] = twmsm61["MAT_NO"];
							tmmsm96["LOGISTICS_STATUS"] = "4";//2--装车确认
							tmmsm96["FACTORY_TO"] = twmsm61["UNLOAD_CODE_FACTORY"];
							tmmsm96["DST_STOCK_CODE"] = twmsm61["UNLOAD_CODE_AREA"];
							tmmsm96["EVENT_ID"] = "MM77";
							tmmsm96["SYSTEM_ID"] = "MMSM";
							tmmsm96["EVENT_LINE_TYPE"] = "00";
							tmmsm96["FUNC_ID"] = s.svc_name;
							tmmsm96.MergeTo(mm0099.Tables["MM0099"], false);

							if (true) //
							{
								bcls_rec->Tables["WM_STOCK"].Rows.Add();
								bcls_rec->Tables["WM_STOCK"].Rows[wm_stock_count]["MAT_NO"] = tmmsm01["MAT_NO"];
								bcls_rec->Tables["WM_STOCK"].Rows[wm_stock_count]["STOCK_OPER_ORDER"] = "1Q";					//库业务类型
								bcls_rec->Tables["WM_STOCK"].Rows[wm_stock_count]["STOCK_OPER_ORDER_DIV"] = "1";					//业务类型内区分
								bcls_rec->Tables["WM_STOCK"].Rows[wm_stock_count]["STOCK_NO"] = "SYA";			//库号
								bcls_rec->Tables["WM_STOCK"].Rows[wm_stock_count]["STOCK_PLACE_NO"] = "SYA";						//材料库位号
								bcls_rec->Tables["WM_STOCK"].Rows[wm_stock_count]["ROWNO"] = " ";								//行号
								bcls_rec->Tables["WM_STOCK"].Rows[wm_stock_count]["COLUMN_NO"] = " ";							//列号
								bcls_rec->Tables["WM_STOCK"].Rows[wm_stock_count]["LAYERNO"] = 0;								//层号
								bcls_rec->Tables["WM_STOCK"].Rows[wm_stock_count]["STOCK_PLACE_POSITION"] = "1";				//库位内位置
								wm_stock_count++;
							}
							tmmsm01["PRE_LOAD_FLAG"] = "0";
							tmmsm01.Update("PRE_LOAD_FLAG", "MAT_NO");
						}
					}
					else if (hmmsm01.Query("MAT_NO"))
					{
						if (yc_flag.Trim() == "") {
							twmsm61["UNLOAD_STATE"] = "3";
							twmsm61["AFFIRM_FLAG"] = "9";
							upd_str += "UNLOAD_STATE,AFFIRM_FLAG,";
							hmmsm01["LOGISTICS_STATUS"] = "3";
							hmmsm01.Update("LOGISTICS_STATUS", "MAT_NO");
						}
						else if (yc_flag == "J")
						{
							twmsm61["UNLOAD_STATE"] = "4";
							upd_str += "UNLOAD_STATE,";
							hmmsm01["LOGISTICS_STATUS"] = "4";
							hmmsm01.Update("LOGISTICS_STATUS", "MAT_NO");

						}
					}
					else {
						strncpy(s.msg, (const char*)"查不到材料", sizeof(s.msg) - 1);
						s.flag = -1;
						doFlag = -1;
						return doFlag;
					}

					if (twmsm12.Query("MAT_NO"))
					{
						hwmsm12.CopyFrom(twmsm12);
						hwmsm12.Insert();
						twmsm12.Delete("MAT_NO");
					}
					

					if (upd_str.GetLength() > 1)
					{
						upd_str = upd_str.Substring(0, upd_str.GetLength() - 1);
						Log::Trace("", __FUNCTION__, "upd_str = {0}", upd_str);
					}

					twmsm61.Update(upd_str, "PRACTICE_NO,MAT_NO");




				}
				else
				{
					if (twmsm61["MAT_NO"].ToString() != "")

					{
						//strncpy(s.msg, (const char*)"材料装车实绩卸车状态不为[待卸车]", sizeof(s.msg) - 1);
						continue;
					}
					

					
				}
			}
		}
		if (twmsm61["PRACTICE_NO"].ToString().Trim() == "")
		{
			twmsm61["PRACTICE_NO"] = bcls_rec->Tables["A02109"].Rows[0]["PRACTICE_NO"].ToString().Trim();
		}
		sqlstr = " update WL_LOAD_RECORD set IS_SEND='2' where SJ_NO='" + twmsm61["PRACTICE_NO"].ToString() + "' ";
		Log::Trace("", "", "sqlstr", sqlstr);
		cmd_sql.SetCommandText(sqlstr);
		cmd_sql.ExecuteNonQuery();
		cmd_sql.Close();

		//调用物料事件
		if (mm0099.Tables["MM0099"].Rows.get_Count() > 0)
		{
			doFlag = f_mmsm99(&mm0099, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		//调用归档函数
		if (wm07.Tables[0].Rows.get_Count() > 0)
		{
			//doFlag = f_wmsm_wm07_proc(&wm07, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		if (bcls_rec->Tables["WM_STOCK"].Rows.get_Count() > 0) {
			Log::Trace("", "", "WM_STOCK", bcls_rec->Tables["WM_STOCK"].Rows.get_Count());
			//doFlag = f_wmsmsm_stock_in(bcls_rec, bcls_ret, conn);  //太钢定制   产出时入库
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		if (DB_6380.Tables[0].Rows.get_Count() > 0)
		{
			doFlag = f_wmsm_6380(&DB_6380, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		/*设置系统返回参数*/
		sprintf(s.msg, _RES("GCRSS0000036")/*电文接收成功。*/);

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
	catch (const CApplicationException& ex)
	{
		//	strncpy(s.msg, (const char*)ex.GetMsg(), 399); //返回前台，与EI.EITuxedo.CallService()方法返回的EI.EIInfo对象的sys_info.msg参数对应
		s.flag = ex.GetCode();       //返回前台，与EI.EITuxedo.CallService()方法返回的EI.EIInfo对象的sys_info.flag参数对应
		Log::Debug("", __FUNCTION__, "error=[{0}]", s.msg);
		doFlag = -1;
	}

	catch (const CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), 399); //返回前台，与EI.EITuxedo.CallService()方法返回的EI.EIInfo对象的sys_info.msg参数对应
		s.flag = ex.GetCode();       //返回前台，与EI.EITuxedo.CallService()方法返回的EI.EIInfo对象的sys_info.flag参数对应
		doFlag = -1;
	}

	return doFlag;
}