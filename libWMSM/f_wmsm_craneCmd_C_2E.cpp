/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:      jinquan
Version:     1.1.1
Date:        2016-12-15
Description: 钢卷库发货命令生成函数
**************************************************/

//框架头文件
#include "WM_Utility.h"
//#include "twma7.h"
//#include "twm00a8.h"
//#include "twm04.h"
#include "h_wms0_pub.h"

BM2_FUNCTION_IMPORT
int f_wmsm_craneCmd_C_delete(CString stock_oper_order, CString vehicleno, EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);     //删除钢卷命令
int f_wm00_pileinfocal(CString stock_no, CString stock_place_no, EIClass * bcls_ret, CDbConnection * conn);                                //垛位最大高度、重量修正
int f_wmsm_CraneCmd_C_Make(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);                                                  //吊车命令做成函数

BM2_FUNCTION_EXPORT
int f_wmsm_craneCmd_C_2E(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{

	/*程序内部变量*/
	int doFlag = 0; 
	int seqcmd = 0;
	CString sqlstr = " ";
	CString sqlWhere = " ";
	CString dateTime = " ";                      //系统时间

	/*业务变量*/
	CString plan_no = " ";                        //计划号
	CString vehicle_no = " ";                     //车船号
	CString shipping_stock_place = " ";           //发货位置
	CString shipping_stock_no = " ";              //发货库区
	CString transfer_mode = " ";                  //运输工具
	CString mat_no = " ";                         //材料
	CString stock_place_no_to = " ";              
	CString stock_no_to = " ";                     

	/*数据存放块*/
	CDataTable plan_mat;                         //存放计划材料
	CDataTable dtCraneCmd;                       //存放命令
	CDataTable dtStockNo;                        //存放材料
	CDataTable dtmat1;                           //存放推荐材料
	CDataTable up_dtMat;                         //存放上层材料
	CDataTable dtMat;                            //存放材料
	CDataTable update_stock;                     //存放需要更新的库位

	/*定义表实体对象*/
	//CTWM00A8 twm00a8(conn);
	//CTWM04   twm04(conn);
	//CTWM04   twm04_fin(conn);
	//CTWM04   twm04_1(conn);
	//CTWM04   twm04_2(conn);
	//CTWMA7   twma7(conn);

	CModel twm00a8 = CModel("TWM00A8");
	CModel twm04 = CModel("TWM04");
	CModel twm04_fin = CModel("TWM04");
	CModel twm04_1 = CModel("TWM04");
	CModel twm04_2 = CModel("TWM04");
	CModel twma7 = CModel("TWMA7");

	try
	{
		//项目自定义日志
		CTracer log(__FUNCTION__);

		//取系统时间
		dateTime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		//数据库操作类定义
		CDbCommand cmd_inq(conn);

		EIClass bcls_rec_make;
		if (!bcls_rec_make.Tables.Contains("WM00_CMD"))
		{
			bcls_rec_make.Tables[0].set_TableName("WM00_CMD");
			bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
			bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "STOCK_NO_FROM");
			bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO_FROM");
			bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "STOCK_NO_TO");
			bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "STOCK_NO_FIN");
			bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO_TO");
			bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");
			bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "LAYERNO_FROM");
			bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "YARD_LAYER_TO");
			bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO_FIN");
			bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "STOCK_OPER_ORDER_FIN");
		}

       #pragma region   获取车船号、计划号、运输工具
		Log::Trace("", __FUNCTION__, "--------------------获取车船号、计划号、运输工具------------------");
		if (!bcls_rec->Tables[0].Columns.Contains("PLAN_NO") ||
			bcls_rec->Tables[0].Rows[0]["PLAN_NO"].ToString().Trim() == "")
		{
			strcpy(s.msg, "There is not Incoming plan no.");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		plan_no = bcls_rec->Tables[0].Rows[0]["PLAN_NO"];
		Log::Trace("", __FUNCTION__, "计划号：【{0}】", plan_no);

		if (!bcls_rec->Tables[0].Columns.Contains("VEHICLE_NO") ||
			bcls_rec->Tables[0].Rows[0]["VEHICLE_NO"].ToString().Trim() == "")
		{
			strcpy(s.msg, "There is not Incoming vehicle no.");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		vehicle_no = bcls_rec->Tables[0].Rows[0]["VEHICLE_NO"].ToString().Trim();
		Log::Trace("", __FUNCTION__, "车船号：【{0}】", vehicle_no);

		if (!bcls_rec->Tables[0].Columns.Contains("TRANSFER_MODE") ||
			bcls_rec->Tables[0].Rows[0]["TRANSFER_MODE"].ToString().Trim() == "")
		{
			strcpy(s.msg, "There is not Incoming transfer no.");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		transfer_mode = bcls_rec->Tables[0].Rows[0]["TRANSFER_MODE"].ToString().Trim();
		Log::Trace("", __FUNCTION__, "运输工具：【{0}】", transfer_mode);
        #pragma endregion

       #pragma region   删除发货命令
		Log::Trace("", __FUNCTION__, "--------------------删除发货命令------------------");
		doFlag = f_wmsm_craneCmd_C_delete("2E", vehicle_no, bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
        #pragma endregion

       #pragma region   查询计划材料信息 获取发货位置
		Log::Trace("", __FUNCTION__, "---------------查询计划材料信息----------------");
		sqlstr = "SELECT d.measure_wt_flag,a.mat_no,a.plan_no,a.plan_exec_seq_no,b.stock_no,b.hall_no,b.stock_place_no,b.layerno,c.stock_place_type,c.dev_div"
			" FROM twma0 a, twma2 b, twm04 c,twma1 d WHERE b.stock_place_no = c.stock_place_no(+) AND a.mat_no = b.mat_no AND a.mat_no = d.mat_no"
			" AND a.vehicle_no = '" + vehicle_no + "' AND A.PLAN_NO = '" + plan_no + "'";
		sqlstr += " ORDER BY a.plan_no";
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		WM_Utility::QueryData(sqlstr, plan_mat, conn);
		Log::Trace("", __FUNCTION__, "计划材料个数：{0}", plan_mat.Rows.get_Count());

		if (transfer_mode == "1")
		{
			Log::Trace("", __FUNCTION__, "运输工具为卡车");
			twm00a8["VEHICLE_NO"] = vehicle_no;
			if (!twm00a8.Query("VEHICLE_NO"))
			{
				Log::Trace("", __FUNCTION__, "没有发货位置，不产生命令");
				return doFlag;
			}
		}
		else if (transfer_mode == "2")
		{
			Log::Trace("", __FUNCTION__, "运输工具为火车");
			Log::Trace("", __FUNCTION__, "---------------获取发货位置----------------");
			twm00a8["VEHICLE_NO"] = vehicle_no;
			if (twm00a8.Query("VEHICLE_NO"))
			{
				shipping_stock_place = twm00a8["STOCK_PLACE_NO"];
				shipping_stock_no = twm00a8["STOCK_NO"];
			}
			else
			{
				Log::Trace("", __FUNCTION__, "没有发货位置，不产生命令");
				return doFlag;
			}
			//twm04.Reset();
			//twm04.STOCK_NO = shipping_stock_no;
			//twm04.DEV_DIV = "8";
			//twm04.STOCK_PLACE_TYPE = "D";
			//if (!twm04.Query("STOCK_NO,DEV_DIV,STOCK_PLACE_TYPE"))
			//{
			//	strcpy(s.msg, "没有查到翻到机位置");
			//	throw CApplicationException(-1, s.msg, log.Location);
			//}

			twm04_1.Reset();
			twm04_1["STOCK_NO"] = shipping_stock_no;
			twm04_1["STOCK_PLACE_TYPE"] = "4";
			if (!twm04_1.Query("STOCK_NO,STOCK_PLACE_TYPE"))
			{
				strcpy(s.msg, "Weighing position is not exist.");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			twm04_2.Reset();
			twm04_2["STOCK_NO"] = shipping_stock_no;
			twm04["DEV_DIV"] = "3";
			twm04_2["STOCK_PLACE_TYPE"] = "D";
			if (!twm04_1.Query("STOCK_NO,DEV_DIV,STOCK_PLACE_TYPE"))
			{
				strcpy(s.msg, "transfer car is not exist");
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		else
		{
			strcpy(s.msg, "transfer no. is error");
			throw CApplicationException(-1, s.msg, log.Location);
		}
       #pragma endregion

       #pragma region   写火车发货命令
		if (transfer_mode == "2")
		{
			for (int i = 0; i < plan_mat.Rows.get_Count(); i++)
			{
				Log::Trace("", __FUNCTION__, "判断材料{0}是否在当前库区", plan_mat.Rows[i]["MAT_NO"].ToString());
				if (plan_mat.Rows[i]["STOCK_NO"].ToString() == "C03")
				{
					Log::Trace("", __FUNCTION__, "材料在C03库区，不生成发货命令");
					continue;
				}
				if (plan_mat.Rows[i]["STOCK_NO"].ToString() == shipping_stock_no)
				{
					Log::Trace("", __FUNCTION__, "材料在当前库区，判断材料库位类型");	
/*					if (plan_mat.Rows[i]["STOCK_PLACE_TYPE"].ToString() == "D"&&plan_mat.Rows[i]["DEV_DIV"].ToString() == "8")
					{
						Log::Trace("", __FUNCTION__, "材料在翻到机上，生成到火车的命令");
						bcls_rec_make.Tables[0].Rows.Add();
						bcls_rec_make.Tables[0].Rows[seqcmd]["MAT_NO"] = plan_mat.Rows[i]["MAT_NO"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_NO_FROM"] = plan_mat.Rows[i]["STOCK_NO"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_FROM"] = plan_mat.Rows[i]["STOCK_PLACE_NO"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_NO_TO"] = plan_mat.Rows[i]["STOCK_NO"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_TO"] = shipping_stock_place;
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_OPER_ORDER"] = "2E";
						bcls_rec_make.Tables[0].Rows[seqcmd]["LAYERNO_FROM"] = plan_mat.Rows[i]["LAYERNO"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_FIN"] = shipping_stock_place;
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_OPER_ORDER_FIN"] = "2E";
						seqcmd++;
						
					}
					else */
					if (plan_mat.Rows[i]["STOCK_PLACE_TYPE"].ToString() == "4")
					{
						Log::Trace("", __FUNCTION__, "材料在称重点，生成到火车的命令");	
						bcls_rec_make.Tables[0].Rows.Add();
						bcls_rec_make.Tables[0].Rows[seqcmd]["MAT_NO"] = plan_mat.Rows[i]["MAT_NO"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_NO_FROM"] = plan_mat.Rows[i]["STOCK_NO"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_FROM"] = plan_mat.Rows[i]["STOCK_PLACE_NO"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_NO_TO"] = plan_mat.Rows[i]["STOCK_NO"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_TO"] = shipping_stock_place;
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_OPER_ORDER"] = "2E";
						bcls_rec_make.Tables[0].Rows[seqcmd]["LAYERNO_FROM"] = plan_mat.Rows[i]["LAYERNO"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_FIN"] = shipping_stock_place;
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_OPER_ORDER_FIN"] = "2E";
						seqcmd++;
					}
					else if (plan_mat.Rows[i]["DEV_DIV"].ToString() == "2")
					{
						Log::Trace("", __FUNCTION__, "材料在火车上，跳过");
						continue;
					}
					else
					{
						Log::Trace("", __FUNCTION__, "材料在其他库位，判断材料是否称重");
						if (plan_mat.Rows[i]["MEASURE_WT_FLAG"].ToString() == "1")
						{
							if (dtMat.Rows[i]["STOCK_PLACE_TYPE"].ToString() == "D"&&dtMat.Rows[i]["DEV_DIV"].ToString() == "1")
							{
								Log::Trace("", __FUNCTION__, "材料已称重且材料在火车上，命令完成");
								continue;
							}
							else
							{
								Log::Trace("", __FUNCTION__, "材料已称重但未在火车上，生成到火车的命令");
								bcls_rec_make.Tables[0].Rows.Add();
								bcls_rec_make.Tables[0].Rows[seqcmd]["MAT_NO"] = dtMat.Rows[i]["MAT_NO"].ToString();
								bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_NO_FROM"] = dtMat.Rows[i]["STOCK_NO"].ToString();
								bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_FROM"] = dtMat.Rows[i]["STOCK_PLACE_NO"].ToString();
								bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_NO_TO"] = dtMat.Rows[i]["STOCK_NO"].ToString();
								bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_TO"] = shipping_stock_place;
								bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_OPER_ORDER"] = "2E";
								bcls_rec_make.Tables[0].Rows[seqcmd]["YARD_LAYER_FROM"] = dtMat.Rows[i]["LAYERNO"].ToString();
								bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_FIN"] = shipping_stock_place;
								bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_OPER_ORDER_FIN"] = "2E";
								bcls_rec_make.Tables[0].Rows[seqcmd]["YARD_LAYER_TO"] = "1";
								seqcmd++;
							}
							
						}
						else
						{
							Log::Trace("", __FUNCTION__, "材料未称重，生成到称重点的命令");	
							bcls_rec_make.Tables[0].Rows.Add();
							bcls_rec_make.Tables[0].Rows[seqcmd]["MAT_NO"] = plan_mat.Rows[i]["MAT_NO"].ToString();
							bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_NO_FROM"] = plan_mat.Rows[i]["STOCK_NO"].ToString();
							bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_FROM"] = plan_mat.Rows[i]["STOCK_PLACE_NO"].ToString();
							bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_NO_TO"] = plan_mat.Rows[i]["STOCK_NO"].ToString();
							bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_TO"] = twm04_1["STOCK_PLACE_NO"].ToString();
							bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_OPER_ORDER"] = "2E";
							bcls_rec_make.Tables[0].Rows[seqcmd]["LAYERNO_FROM"] = plan_mat.Rows[i]["LAYERNO"].ToString();
							bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_FIN"] = shipping_stock_place;
							bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_OPER_ORDER_FIN"] = "2E";
							seqcmd++;
						}
					}										
				}
				else
				{
					Log::Trace("", __FUNCTION__, "材料不在当前库区，判断是否在过跨台车上");
					if (plan_mat.Rows[i]["DEV_DIV"].ToString() == "3"&&plan_mat.Rows[i]["STOCK_PLACE_TYPE"].ToString() == "D")
					{
						Log::Trace("", __FUNCTION__, "材料在过跨台车上，判断是否存在命令");
						if (twma7.Query("MAT_NO"))
						{
							Log::Trace("", __FUNCTION__, "材料已存在命令，判断是否是人工生成的命令");
							if (twma7["MAIN_MAT_NO"].ToString() == "1")
							{
								Log::Trace("", __FUNCTION__, "人工生成的命令,更新最终库位类型");
								twma7["REC_ERASE_TIME"] = dateTime;
								twma7["REC_ERASOR"] = s.userid;
								twma7["STOCK_PLACE_NO_FIN"] = shipping_stock_place;
								twma7["STOCK_OPER_ORDER_FIN"] = "2E";
								twma7.Update("REC_ERASE_TIME,REC_ERASOR,STOCK_PLACE_NO_FIN,STOCK_OPER_ORDER_FIN", "MAT_NO");
							}
							else
							{
								Log::Trace("", __FUNCTION__, "非人工生成，修改为发货命令");
								stock_place_no_to = twma7["STOCK_PLACE_NO_TO"].ToString();
								stock_no_to = twma7["STOCK_NO"].ToString();
								twma7["REC_ERASE_TIME"] = dateTime;
								twma7["REC_ERASOR"] = s.userid;
								twma7["STOCK_NO_TO"] = shipping_stock_no;
								twma7["STOCK_PLACE_NO_TO"] = shipping_stock_place;
								twma7["STOCK_OPER_ORDER"] = "2E";
								twma7["MOVE_TYPE"] = "2E";
								twma7["STOCK_PLACE_NO_FIN"] = shipping_stock_place;
								twma7["STOCK_OPER_ORDER_FIN"] = "2E";
								twma7["VEHICLE_NO"] = vehicle_no;
								twma7.Update("REC_ERASE_TIME,REC_ERASOR,STOCK_NO_TO,STOCK_PLACE_NO_TO,MOVE_TYPE,STOCK_OPER_ORDER,STOCK_PLACE_NO_FIN,STOCK_OPER_ORDER_FIN,VEHICLE_NO", "MAT_NO");
								doFlag = f_wm00_pileinfocal(stock_no_to, stock_place_no_to, bcls_ret, conn);
								if (doFlag != 0)
								{
									throw CApplicationException(-1, s.msg, log.Location);
								}
							}
						}
						else
						{
							Log::Trace("", __FUNCTION__, "材料不存在命令，生成发货命令");
							bcls_rec_make.Tables[0].Rows.Add();
							bcls_rec_make.Tables[0].Rows[seqcmd]["MAT_NO"] = plan_mat.Rows[i]["MAT_NO"].ToString();
							bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_NO_FROM"] = plan_mat.Rows[i]["STOCK_NO"].ToString();
							bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_FROM"] = plan_mat.Rows[i]["STOCK_PLACE_NO"].ToString();
							bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_NO_TO"] = plan_mat.Rows[i]["STOCK_NO"].ToString();
							bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_TO"] = twm04_2["STOCK_PLACE_NO"].ToString();
							bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_OPER_ORDER"] = "2E";
							bcls_rec_make.Tables[0].Rows[seqcmd]["LAYERNO_FROM"] = plan_mat.Rows[i]["LAYERNO"].ToString();
							bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_FIN"] = shipping_stock_place;
							bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_OPER_ORDER_FIN"] = "2E";
							seqcmd++;

						}
					}
					else
					{
						Log::Trace("", __FUNCTION__, "材料不在过跨台车上，生成到过跨台车的命令");
						twm04.Reset();
						twm04["STOCK_NO"] = plan_mat.Rows[i]["STOCK_NO"].ToString();
						twm04["DEV_DIV"] = "3";
						twm04["STOCK_PLACE_TYPE"] = "D";
						twm04.Query("STOCK_NO,DEV_DIV,STOCK_PLACE_TYPE");
						bcls_rec_make.Tables[0].Rows.Add();
						bcls_rec_make.Tables[0].Rows[seqcmd]["MAT_NO"] = plan_mat.Rows[i]["MAT_NO"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_NO_FROM"] = plan_mat.Rows[i]["STOCK_NO"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_FROM"] = plan_mat.Rows[i]["STOCK_PLACE_NO"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_NO_TO"] = plan_mat.Rows[i]["STOCK_NO"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_NO_FIN"] = shipping_stock_no;
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_TO"] = twm04["STOCK_PLACE_NO"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_OPER_ORDER"] = "32";
						bcls_rec_make.Tables[0].Rows[seqcmd]["LAYERNO_FROM"] = plan_mat.Rows[i]["LAYERNO"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_FIN"] = shipping_stock_place;
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_OPER_ORDER_FIN"] = "2E";
						seqcmd++;
					}
				}
			}
		}

#pragma endregion

       #pragma region   写卡车发货命令
		if (transfer_mode == "1")
		{		
			for (int i = 0; i < plan_mat.Rows.get_Count(); i++)
			{
				//if (plan_mat.Rows[i]["STOCK_NO"].ToString() == "C03")
				//{
				//	Log::Trace("", __FUNCTION__, "材料在C03库区，不生成发货命令");
				//	continue;
				//}
				twm04_fin["STOCK_NO"] = plan_mat.Rows[i]["STOCK_NO"].ToString();
				twm04_fin["DEV_DIV"] = "1";
				twm04_fin["STOCK_PLACE_TYPE"] = "D";
				if (!twm04_fin.Query("STOCK_NO,DEV_DIV,STOCK_PLACE_TYPE"))
				{
					strcpy(s.msg, "There is not car position");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				Log::Trace("", __FUNCTION__, "判断材料库位类型");
/*				if (plan_mat.Rows[i]["STOCK_PLACE_TYPE"].ToString() == "D"&&plan_mat.Rows[i]["DEV_DIV"].ToString() == "8")
				{
					Log::Trace("", __FUNCTION__, "材料在翻到机上，生成到卡车的命令");
					bcls_rec_make.Tables[0].Rows.Add();
					bcls_rec_make.Tables[0].Rows[seqcmd]["MAT_NO"] = plan_mat.Rows[i]["MAT_NO"].ToString();
					bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_NO_FROM"] = plan_mat.Rows[i]["STOCK_NO"].ToString();
					bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_FROM"] = plan_mat.Rows[i]["STOCK_PLACE_NO"].ToString();
					bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_NO_TO"] = plan_mat.Rows[i]["STOCK_NO"].ToString();
					bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_TO"] = twm04_fin.STOCK_PLACE_NO;
					bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_OPER_ORDER"] = "2E";
					bcls_rec_make.Tables[0].Rows[seqcmd]["LAYERNO_FROM"] = plan_mat.Rows[i]["LAYERNO"].ToString();
					bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_FIN"] = twm04_fin.STOCK_PLACE_NO;
					bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_OPER_ORDER_FIN"] = "2E";
					seqcmd++;

				}
				else*/ 
				if (plan_mat.Rows[i]["STOCK_PLACE_TYPE"].ToString() == "4")
				{
					Log::Trace("", __FUNCTION__, "材料在称重点，生成到卡车的命令");
					bcls_rec_make.Tables[0].Rows.Add();
					bcls_rec_make.Tables[0].Rows[seqcmd]["MAT_NO"] = plan_mat.Rows[i]["MAT_NO"].ToString();
					bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_NO_FROM"] = plan_mat.Rows[i]["STOCK_NO"].ToString();
					bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_FROM"] = plan_mat.Rows[i]["STOCK_PLACE_NO"].ToString();
					bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_NO_TO"] = plan_mat.Rows[i]["STOCK_NO"].ToString();
					bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_TO"] = twm04_fin["STOCK_PLACE_NO"].ToString();
					bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_OPER_ORDER"] = "2E";
					bcls_rec_make.Tables[0].Rows[seqcmd]["LAYERNO_FROM"] = plan_mat.Rows[i]["LAYERNO"].ToString();
					bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_FIN"] = twm04_fin["STOCK_PLACE_NO"].ToString();
					bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_OPER_ORDER_FIN"] = "2E";
					seqcmd++;
				}
				else if (plan_mat.Rows[i]["DEV_DIV"].ToString() == "1")
				{
					Log::Trace("", __FUNCTION__, "材料在卡车上，跳过");
					continue;
				}
				else
				{
					Log::Trace("", __FUNCTION__, "材料在其他库位，判断材料是否称重");
					if (plan_mat.Rows[i]["MEASURE_WT_FLAG"].ToString() == "1")
					{
						if (dtMat.Rows[i]["STOCK_PLACE_TYPE"].ToString() == "D"&&dtMat.Rows[i]["DEV_DIV"].ToString() == "1")
						{
							Log::Trace("", __FUNCTION__, "材料已称重且材料在卡车上，命令完成");
							continue;
						}
						else
						{
							Log::Trace("", __FUNCTION__, "材料已称重但未在卡车上，生成到卡车的命令");
							bcls_rec_make.Tables[0].Rows.Add();
							bcls_rec_make.Tables[0].Rows[seqcmd]["MAT_NO"] = dtMat.Rows[i]["MAT_NO"].ToString();
							bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_NO_FROM"] = dtMat.Rows[i]["STOCK_NO"].ToString();
							bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_FROM"] = dtMat.Rows[i]["STOCK_PLACE_NO"].ToString();
							bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_NO_TO"] = dtMat.Rows[i]["STOCK_NO"].ToString();
							bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_TO"] = twm04_fin["STOCK_PLACE_NO"].ToString();
							bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_FIN"] = twm04_fin["STOCK_PLACE_NO"].ToString();
							bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_OPER_ORDER"] = "2E";
							bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_OPER_ORDER_FIN"] = "2E";
							bcls_rec_make.Tables[0].Rows[seqcmd]["YARD_LAYER_TO"] = "1";
							bcls_rec_make.Tables[0].Rows[seqcmd]["YARD_LAYER_FROM"] = dtMat.Rows[i]["LAYERNO"].ToString();
							seqcmd++;
						}
					}
					else
					{
						Log::Trace("", __FUNCTION__, "材料未称重，生成到称重点的命令");
						twm04.Reset();
						twm04["STOCK_NO"] = plan_mat.Rows[i]["STOCK_NO"].ToString();
						twm04["STOCK_PLACE_TYPE"] = "4";
						if (!twm04.Query("STOCK_NO,STOCK_PLACE_TYPE"))
						{
							strcpy(s.msg, "Weighing position is not exist.");
							throw CApplicationException(-1, s.msg, log.Location);
						}
						bcls_rec_make.Tables[0].Rows.Add();
						bcls_rec_make.Tables[0].Rows[seqcmd]["MAT_NO"] = plan_mat.Rows[i]["MAT_NO"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_NO_FROM"] = plan_mat.Rows[i]["STOCK_NO"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_FROM"] = plan_mat.Rows[i]["STOCK_PLACE_NO"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_NO_TO"] = plan_mat.Rows[i]["STOCK_NO"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_TO"] = twm04["STOCK_PLACE_NO"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_OPER_ORDER"] = "2E";
						bcls_rec_make.Tables[0].Rows[seqcmd]["LAYERNO_FROM"] = plan_mat.Rows[i]["LAYERNO"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_FIN"] = twm04_fin["STOCK_PLACE_NO"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_OPER_ORDER_FIN"] = "2E";
						seqcmd++;
					}
				}
			}
		}
         #pragma endregion

		if (bcls_rec_make.Tables[0].Rows.get_Count() > 0)
		{
			doFlag = f_wmsm_CraneCmd_C_Make(&bcls_rec_make, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}


	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "Database processing error. sqlcode=[{0}].", arguments, 1);
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

