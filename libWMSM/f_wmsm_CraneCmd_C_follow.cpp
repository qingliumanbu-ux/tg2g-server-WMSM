/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:      金权
Version:     1.1.1
Date:        2016-12-02 10:35:08
Description: 板坯吊车命令后续处理函数
**************************************************/

#include "WM_Utility.h"
//#include "twma7.h"
//#include "twma2.h"
//#include "twma0.h"
//#include "twm04.h"
//#include "twm00a8.h"

BM2_FUNCTION_IMPORT
int f_wmsm_CraneCmd_C_Make(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);	     //行车命令形成	
int f_wm00_pileinfocal(CString stock_no, CString stock_place_no, EIClass * bcls_ret, CDbConnection * conn);  //垛位最大高度、重量修正
int f_wmsm_craneCmd_C_layer_update(CString mat_no, EIClass * bcls_ret, CDbConnection * conn);



BM2_FUNCTION_EXPORT
int f_wmsm_CraneCmd_C_follow(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{

	/*程序内部变量*/
	int doFlag = 0;
	int seqcmd = 0;
	CString sqlstr = " ";
	CString matNo = " ";
	CString dateTime = " ";
	CString stock_oper_order_fin = " ";
	CString stock_oper_order = " ";
	CString stock_place_no = " ";
	CString stock_place_no_to = " "; 
	CString stock_place_no_fin = " "; 
	CString shipping_stock_place = " ";
	CString shipping_stock_no = " ";
	CString stock_no_to = " ";

	/*数据库操作类定义*/
	CDbCommand cmd_inq(conn);

	/*定义表实体对象*/
	//CTWMA7   twma7(conn);
	//CTWMA2   twma2(conn);
	//CTWMA0   twma0(conn);
	//CTWM04   twm04(conn);
	//CTWM04   twm04_fin(conn);
	//CTWM00A8   twm00a8(conn);
	CModel twma7 = CModel("TWMA7");
	CModel twma2 = CModel("TWMA2");
	CModel twma0 = CModel("TWMA0");
	CModel twm04 = CModel("TWM04");
	CModel twm04_fin = CModel("TWM04");
	CModel twm00a8 = CModel("TWM00A8");

	/*数据存放块*/
	CDataTable dtMat;                          //存放材料
	CDataTable dtCmd;                          //存放命令
	CDataTable dtCmd_1;                        //存放命令
	CDataTable dtStockNo;
	CDataTable dtmat1;


	try
	{
		//项目自定义日志
		CTracer log(__FUNCTION__);

		//取系统时间
		dateTime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		//判断是否存在指定块
		if (bcls_rec->Tables.IndexOf("WM00_FOLLOW") < 0 ||
			bcls_rec->Tables["WM00_FOLLOW"].Rows.get_Count() == 0)
		{
			Log::Trace("", __FUNCTION__, "没有传入材料数据");
			return doFlag;
		}

		EIClass bcls_rec_make;
		bcls_rec_make.Tables[0].set_TableName("WM00_CMD");
		bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
		bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "STOCK_NO_FROM");
		bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO_FROM");
		bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "STOCK_NO_TO");
		bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "STOCK_NO_FIN");
		bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO_TO");
		bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");
		bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "STOCK_OPER_ORDER_FIN");
		bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "UNIT_CODE");
		bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "YARD_LAYER_FROM");
		bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO_FIN");
		bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "YARD_LAYER_TO");

		//循环获取传入材料数据块
		for (int i = 0; i < bcls_rec->Tables["WM00_FOLLOW"].Rows.get_Count(); i++)
		{
			matNo = bcls_rec->Tables["WM00_FOLLOW"].Rows[i]["MAT_NO"].ToString().Trim();
			//判断是否传入材料号
			if (matNo == "")
			{
				Log::Trace("", __FUNCTION__, "传入材料号为空");
				continue;
			}
			Log::Trace("", __FUNCTION__, "材料号：【{0}】", matNo);
			twma2.Reset();
			twma2["MAT_NO"] = matNo;
			if (!twma2.Query("MAT_NO"))
			{
				Log::Trace("", __FUNCTION__, "没有查到材料库位信息");
				continue;
			}
			Log::Trace("", __FUNCTION__, "材料库位：【{0}】", twma2["STOCK_PLACE_NO"].ToString());

			twma7.Reset();
			twma7["MAT_NO"] = matNo;
			twma7["CRANE_INST_STATUS"] = "0";
			twma7["MAIN_MAT_NO"] = "0";
			twma7.Update("CRANE_INST_STATUS,MAIN_MAT_NO","MAT_NO");

			dtMat.Rows.Clear();
			sqlstr = " select a.layerno, a.mat_no,a.stock_no,a.stock_place_no,b.stock_place_type,c.stock_no_to,b.dev_div,c.stock_oper_order_fin,c.stock_place_no_fin,c.stock_no_fin,c.unit_code,c.vehicle_no,d.measure_wt_flag from twma2 a,twm04 b,twma7 c,twma1 d where a.mat_no=d.mat_no and a.stock_place_no=b.stock_place_no and c.mat_no=a.mat_no  and a.mat_no='" + matNo + "'";
			Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteQuery(dtMat);
			cmd_inq.Close();
			if (dtMat.Rows.get_Count() > 0)
			{
				stock_oper_order_fin = dtMat.Rows[0]["STOCK_OPER_ORDER_FIN"].ToString().Trim();
				stock_place_no_fin = dtMat.Rows[0]["STOCK_PLACE_NO_FIN"].ToString().Trim();
			}
			else
			{
				Log::Trace("", __FUNCTION__, "没有查到材料库位信息");
				continue;
			}

            if (stock_oper_order_fin == "2E")
			{
				Log::Trace("", __FUNCTION__, "材料{0}最终命令为发货命令，先删除命令", matNo);
				twma7.Delete();
				twma0["MAT_NO"] = matNo;
				if (!twma0.Query("MAT_NO"))
				{
					continue;
					//strcpy(s.msg, "材料不在发货计划中");
					//throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (twma0["TRANS_TOOL"].ToString() == "1")
				{
					twm04_fin["STOCK_NO"] = dtMat.Rows[i]["STOCK_NO"].ToString();
					twm04_fin["DEV_DIV"] = "1";
					twm04_fin["STOCK_PLACE_TYPE"] = "D";
					if (!twm04_fin.Query("STOCK_NO,DEV_DIV,STOCK_PLACE_TYPE"))
					{
						strcpy(s.msg, "没有查到卡车位置");
						throw CApplicationException(-1, s.msg, log.Location);
					}
					Log::Trace("", __FUNCTION__, "发货工具为卡车，判断材料库位类型");
/*					if (dtMat.Rows[i]["STOCK_PLACE_TYPE"].ToString() == "D"&&dtMat.Rows[i]["DEV_DIV"].ToString() == "8")
					{
						Log::Trace("", __FUNCTION__, "材料在翻到机上，生成到卡车的命令");

						bcls_rec_make.Tables[0].Rows.Add();
						bcls_rec_make.Tables[0].Rows[seqcmd]["MAT_NO"] = dtMat.Rows[i]["MAT_NO"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_NO_FROM"] = dtMat.Rows[i]["STOCK_NO"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_FROM"] = dtMat.Rows[i]["STOCK_PLACE_NO"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_NO_TO"] = dtMat.Rows[i]["STOCK_NO"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_TO"] = twm04_fin.STOCK_PLACE_NO;
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_FIN"] = twm04_fin.STOCK_PLACE_NO;
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_OPER_ORDER"] = "2E";
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_OPER_ORDER_FIN"] = "2E";
						bcls_rec_make.Tables[0].Rows[seqcmd]["YARD_LAYER_TO"] = "1";
						bcls_rec_make.Tables[0].Rows[seqcmd]["YARD_LAYER_FROM"] = dtMat.Rows[i]["LAYERNO"].ToString();
						seqcmd++;

					}
					else */
					if (dtMat.Rows[i]["STOCK_PLACE_TYPE"].ToString() == "4")
					{
						Log::Trace("", __FUNCTION__, "材料在称重点，生成到卡车的命令");
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
					else if (dtMat.Rows[i]["DEV_DIV"].ToString() == "1")
					{
						Log::Trace("", __FUNCTION__, "材料在卡车上，跳过");
						continue;
					}
					else
					{
						Log::Trace("", __FUNCTION__, "材料在其他库位，判断材料是否称重");
						if (dtMat.Rows[i]["MEASURE_WT_FLAG"].ToString() == "1")
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
							//twm04.Reset();
							//twm04.STOCK_NO = dtMat.Rows[i]["STOCK_NO"].ToString();
							//twm04.DEV_DIV = "8";
							//twm04.STOCK_PLACE_TYPE = "D";
							//if (!twm04.Query("STOCK_NO,DEV_DIV,STOCK_PLACE_TYPE"))
							//{
							//	strcpy(s.msg, "没有查到翻到机位置");
							//	throw CApplicationException(-1, s.msg, log.Location);
							//}
							//bcls_rec_make.Tables[0].Rows.Add();
							//bcls_rec_make.Tables[0].Rows[seqcmd]["MAT_NO"] = dtMat.Rows[i]["MAT_NO"].ToString();
							//bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_NO_FROM"] = dtMat.Rows[i]["STOCK_NO"].ToString();
							//bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_FROM"] = dtMat.Rows[i]["STOCK_PLACE_NO"].ToString();
							//bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_NO_TO"] = dtMat.Rows[i]["STOCK_NO"].ToString();
							//bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_TO"] = twm04.STOCK_PLACE_NO;
							//bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_OPER_ORDER"] = "2E";
							//bcls_rec_make.Tables[0].Rows[seqcmd]["YARD_LAYER_FROM"] = dtMat.Rows[i]["LAYERNO"].ToString();
							//bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_FIN"] = twm04_fin.STOCK_PLACE_NO;
							//bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_OPER_ORDER_FIN"] = "2E";
							//bcls_rec_make.Tables[0].Rows[seqcmd]["YARD_LAYER_TO"] = "1";
							//seqcmd++;
						}
						else
						{
							Log::Trace("", __FUNCTION__, "材料未称重，生成到称重点的命令");
							twm04.Reset();
							twm04["STOCK_NO"] = dtMat.Rows[i]["STOCK_NO"].ToString();
							twm04["STOCK_PLACE_TYPE"] = "4";
							if (!twm04.Query("STOCK_NO,STOCK_PLACE_TYPE"))
							{
								strcpy(s.msg, "Weighing position is not exist.");
								throw CApplicationException(-1, s.msg, log.Location);
							}
							bcls_rec_make.Tables[0].Rows.Add();
							bcls_rec_make.Tables[0].Rows[seqcmd]["MAT_NO"] = dtMat.Rows[i]["MAT_NO"].ToString();
							bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_NO_FROM"] = dtMat.Rows[i]["STOCK_NO"].ToString();
							bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_FROM"] = dtMat.Rows[i]["STOCK_PLACE_NO"].ToString();
							bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_NO_TO"] = dtMat.Rows[i]["STOCK_NO"].ToString();
							bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_TO"] = twm04["STOCK_PLACE_NO"].ToString();
							bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_OPER_ORDER"] = "2E";
							bcls_rec_make.Tables[0].Rows[seqcmd]["YARD_LAYER_FROM"] = dtMat.Rows[i]["LAYERNO"].ToString();
							bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_FIN"] = twm04_fin["STOCK_PLACE_NO"].ToString();
							bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_OPER_ORDER_FIN"] = "2E";
							bcls_rec_make.Tables[0].Rows[seqcmd]["YARD_LAYER_TO"] = "1";
							seqcmd++;
						}
					}
				}
				else if (twma0["TRANS_TOOL"].ToString() == "2")
				{
					Log::Trace("", __FUNCTION__, "发货工具为火车");
					twma0.Reset();
					twma0["MAT_NO"] = matNo;
					twma0.Query("MAT_NO");
					twm00a8["VEHICLE_NO"] = twma0["VEHICLE_NO"].ToString();
					if (twm00a8.Query("VEHICLE_NO"))
					{
						shipping_stock_place = twm00a8["STOCK_PLACE_NO"].ToString();
						shipping_stock_no = twm00a8["STOCK_NO"].ToString();
					}
					else
					{
						strcpy(s.msg, "There is no wagon position");
						throw CApplicationException(-1, s.msg, log.Location);
					}

					if (dtMat.Rows[i]["STOCK_NO"].ToString() == shipping_stock_no)
					{
						Log::Trace("", __FUNCTION__, "材料在当前库区，判断材料库位类型");
/*						if (dtMat.Rows[i]["STOCK_PLACE_TYPE"].ToString() == "D"&&dtMat.Rows[i]["DEV_DIV"].ToString() == "8")
						{
							Log::Trace("", __FUNCTION__, "材料在翻到机上，生成到火车的命令");
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
						else*/ 
						if (dtMat.Rows[i]["STOCK_PLACE_TYPE"].ToString() == "4")
						{
							Log::Trace("", __FUNCTION__, "材料在称重点，生成到火车的命令");
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
						else if (dtMat.Rows[i]["DEV_DIV"].ToString() == "2")
						{
							Log::Trace("", __FUNCTION__, "材料在火车上，跳过");
							continue;
						}
						else
						{
							Log::Trace("", __FUNCTION__, "材料在其他库位，判断材料是否称重");
							Log::Trace("", __FUNCTION__, "{0}", dtMat.Rows[i]["MEASURE_WT_FLAG"].ToString());
							if (dtMat.Rows[i]["MEASURE_WT_FLAG"].ToString() == "1")
							{
								Log::Trace("", __FUNCTION__, "{0}", dtMat.Rows[i]["STOCK_PLACE_TYPE"].ToString());
								Log::Trace("", __FUNCTION__, "{0}", dtMat.Rows[i]["DEV_DIV"].ToString());
								if (dtMat.Rows[i]["STOCK_PLACE_TYPE"].ToString() == "D"&& dtMat.Rows[i]["DEV_DIV"].ToString() == "1")
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
								twm04.Reset();
								twm04["STOCK_NO"] = dtMat.Rows[i]["STOCK_NO"].ToString();
								twm04["STOCK_PLACE_TYPE"] = "4";
								if (!twm04.Query("STOCK_NO,STOCK_PLACE_TYPE"))
								{
									strcpy(s.msg, "Weighing position is not exist.");
									throw CApplicationException(-1, s.msg, log.Location);
								}
								bcls_rec_make.Tables[0].Rows.Add();
								bcls_rec_make.Tables[0].Rows[seqcmd]["MAT_NO"] = dtMat.Rows[i]["MAT_NO"].ToString();
								bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_NO_FROM"] = dtMat.Rows[i]["STOCK_NO"].ToString();
								bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_FROM"] = dtMat.Rows[i]["STOCK_PLACE_NO"].ToString();
								bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_NO_TO"] = dtMat.Rows[i]["STOCK_NO"].ToString();
								bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_TO"] = twm04["STOCK_PLACE_NO"].ToString();
								bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_OPER_ORDER"] = "2E";
								bcls_rec_make.Tables[0].Rows[seqcmd]["YARD_LAYER_FROM"] = dtMat.Rows[i]["LAYERNO"].ToString();
								bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_FIN"] = shipping_stock_place;
								bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_OPER_ORDER_FIN"] = "2E";
								bcls_rec_make.Tables[0].Rows[seqcmd]["YARD_LAYER_TO"] = "1";
								seqcmd++;
							}
						}
					}
					else
					{
						Log::Trace("", __FUNCTION__, "材料不在当前库区，判断是否在过跨台车上");
						if (dtMat.Rows[i]["DEV_DIV"].ToString() == "3"&&dtMat.Rows[i]["STOCK_PLACE_TYPE"].ToString() == "D")
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
									twma7["VEHICLE_NO"] = twm00a8["VEHICLE_NO"].ToString();
									twma7["YARD_LAYER_TO"] = 1;
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
								Log::Trace("", __FUNCTION__, "材料不存在命令，过跨命令");
								twm04.Reset();
								twm04["STOCK_NO"] = shipping_stock_no;
								twm04["DEV_DIV"] = "3";
								twm04["STOCK_PLACE_TYPE"] = "D";
								twm04.Query("STOCK_NO,DEV_DIV,STOCK_PLACE_TYPE");
								bcls_rec_make.Tables[0].Rows.Add();
								bcls_rec_make.Tables[0].Rows[seqcmd]["MAT_NO"] = dtMat.Rows[i]["MAT_NO"].ToString();
								bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_NO_FROM"] = dtMat.Rows[i]["STOCK_NO"].ToString();
								bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_FROM"] = dtMat.Rows[i]["STOCK_PLACE_NO"].ToString();
								bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_NO_TO"] = dtMat.Rows[i]["STOCK_NO"].ToString();
								bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_NO_FIN"] = shipping_stock_no;
								bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_TO"] = twm04["STOCK_PLACE_NO"].ToString();
								bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_OPER_ORDER"] = "2E";
								bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_OPER_ORDER_FIN"] = "2E";
								bcls_rec_make.Tables[0].Rows[seqcmd]["YARD_LAYER_FROM"] = dtMat.Rows[i]["LAYERNO"].ToString();
								bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_FIN"] = shipping_stock_place;
								bcls_rec_make.Tables[0].Rows[seqcmd]["YARD_LAYER_TO"] = "1";
								seqcmd++;

							}
						}
						else
						{
							Log::Trace("", __FUNCTION__, "材料不在过跨台车上，生成过跨命令");
							bcls_rec_make.Tables[0].Rows.Add();
							bcls_rec_make.Tables[0].Rows[seqcmd]["MAT_NO"] = dtMat.Rows[i]["MAT_NO"].ToString();
							bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_NO_FROM"] = dtMat.Rows[i]["STOCK_NO"].ToString();
							bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_FROM"] = dtMat.Rows[i]["STOCK_PLACE_NO"].ToString();
							bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_NO_TO"] = dtMat.Rows[i]["STOCK_NO"].ToString();
							bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_TO"] = stock_place_no_to;
							bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_OPER_ORDER"] = "32";
							bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_OPER_ORDER_FIN"] = "2E";
							bcls_rec_make.Tables[0].Rows[seqcmd]["YARD_LAYER_FROM"] = dtMat.Rows[i]["LAYERNO"].ToString();
							bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_FIN"] = shipping_stock_place;
							bcls_rec_make.Tables[0].Rows[seqcmd]["YARD_LAYER_TO"] = "1";
							seqcmd++;
						}
					}
				}
				else
				{
					strcpy(s.msg, "材料发货方式不存在");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}		
			}
			else if (stock_oper_order_fin == "32")
			{
				Log::Trace("", __FUNCTION__, "最终库业务类型为32,判断材料是否在目的库区");
				if (dtMat.Rows[0]["STOCK_NO"].ToString().Trim() == dtMat.Rows[0]["STOCK_NO_FIN"].ToString().Trim())
				{
					Log::Trace("", __FUNCTION__, "在目的库区,判断材料是否在过跨台车上");
					if (dtMat.Rows[0]["DEV_DIV"].ToString().Trim() == "3"&&dtMat.Rows[0]["STOCK_PLACE_TYPE"].ToString().Trim() == "D")
					{
						Log::Trace("", __FUNCTION__, "材料在过跨台车上，生成到目标库区的命令");
						bcls_rec_make.Tables[0].Rows.Add();
						bcls_rec_make.Tables[0].Rows[seqcmd]["MAT_NO"] = dtMat.Rows[i]["MAT_NO"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["YARD_LAYER_FROM"] = dtMat.Rows[i]["LAYERNO"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_NO_TO"] = dtMat.Rows[i]["STOCK_NO"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_FROM"] = dtMat.Rows[i]["STOCK_PLACE_NO"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_TO"] = dtMat.Rows[i]["STOCK_PLACE_NO_FIN"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_FIN"] = dtMat.Rows[i]["STOCK_PLACE_NO_FIN"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_NO_FIN"] = dtMat.Rows[i]["STOCK_NO_FIN"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_NO_FROM"] = dtMat.Rows[i]["STOCK_NO"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_OPER_ORDER"] = "32";
						seqcmd++;
					}
					else
					{
						Log::Trace("", __FUNCTION__, "材料{0}不在过跨台车上，删除命令", matNo);
						twma7.Delete();
					}
				}
				else
				{
					Log::Trace("", __FUNCTION__, "材料不在目的库区");
					if (dtMat.Rows[0]["STOCK_PLACE_TYPE"].ToString().Trim() == "D"&&dtMat.Rows[0]["DEV_DIV"].ToString().Trim() == "3")
					{
						Log::Trace("", __FUNCTION__, "材料在过跨台车上");
						twm04["STOCK_NO"] = dtMat.Rows[0]["STOCK_NO_FIN"].ToString();
						twm04["DEV_DIV"] = "3";
						twm04["STOCK_PLACE_TYPE"] = "D";
						twm04.Query("STOCK_NO,DEV_DIV,STOCK_PLACE_TYPE");
						
						bcls_rec_make.Tables[0].Rows.Add();
						bcls_rec_make.Tables[0].Rows[seqcmd]["MAT_NO"] = dtMat.Rows[i]["MAT_NO"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["YARD_LAYER_FROM"] = dtMat.Rows[i]["LAYERNO"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_NO_TO"] = dtMat.Rows[i]["STOCK_NO"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_FROM"] = dtMat.Rows[i]["STOCK_PLACE_NO"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_TO"] = twm04["STOCK_PLACE_NO"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_OPER_ORDER"] = "32";
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_FIN"] = dtMat.Rows[i]["STOCK_PLACE_NO_FIN"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_NO_FIN"] = dtMat.Rows[i]["STOCK_NO_FIN"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_NO_FROM"] = dtMat.Rows[i]["STOCK_NO"].ToString();
						seqcmd++;

					}
					else
					{
						Log::Trace("", __FUNCTION__, "材料不在过跨台车上，生成到过跨台车的命令");
						twm04["STOCK_NO"] = dtMat.Rows[i]["STOCK_NO"].ToString();
						twm04["DEV_DIV"] = "3";
						twm04["STOCK_PLACE_TYPE"] = "D";
						twm04.Query("STOCK_NO,DEV_DIV,STOCK_PLACE_TYPE");
						bcls_rec_make.Tables[0].Rows.Add();
						bcls_rec_make.Tables[0].Rows[seqcmd]["MAT_NO"] = dtMat.Rows[i]["MAT_NO"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["YARD_LAYER_FROM"] = dtMat.Rows[i]["LAYERNO"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_NO_TO"] = dtMat.Rows[i]["STOCK_NO"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_FROM"] = dtMat.Rows[i]["STOCK_PLACE_NO"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_TO"] = twm04["STOCK_PLACE_NO"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_OPER_ORDER"] = "32";
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_PLACE_NO_FIN"] = dtMat.Rows[i]["STOCK_PLACE_NO_FIN"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_NO_FIN"] = dtMat.Rows[i]["STOCK_NO_FIN"].ToString();
						bcls_rec_make.Tables[0].Rows[seqcmd]["STOCK_NO_FROM"] = dtMat.Rows[i]["STOCK_NO"].ToString();
						seqcmd;
					}
				}
			}
			else if (stock_oper_order_fin == "2B")
			{
				twma7.Query("MAT_NO");
				if (twma7["STOCK_PLACE_NO_TO"].ToString() == twma2["STOCK_PLACE_NO"].ToString())
				{
					Log::Trace("", __FUNCTION__, "材料已到目标库位，删除材料命令{0}", matNo);
					twma7.Delete();
				}
				else
				{
					Log::Trace("", __FUNCTION__, "材料未到目标库位，生成命令{0}", matNo);
					bcls_rec_make.Tables[0].Rows.Add();
					bcls_rec_make.Tables[0].Rows[0]["MAT_NO"] = dtMat.Rows[i]["MAT_NO"].ToString();
					bcls_rec_make.Tables[0].Rows[0]["STOCK_NO_FROM"] = twma2["STOCK_NO"];
					bcls_rec_make.Tables[0].Rows[0]["STOCK_PLACE_NO_FROM"] = twma2["STOCK_PLACE_NO"].ToString();
					bcls_rec_make.Tables[0].Rows[0]["STOCK_NO_TO"] = twma2["STOCK_NO"].ToString();
					bcls_rec_make.Tables[0].Rows[0]["STOCK_PLACE_NO_TO"] = twma7["STOCK_PLACE_NO_TO"].ToString();
					bcls_rec_make.Tables[0].Rows[0]["STOCK_OPER_ORDER"] = stock_oper_order_fin;
					bcls_rec_make.Tables[0].Rows[0]["YARD_LAYER_FROM"] = twma2["LAYERNO"].ToDecimal();
					bcls_rec_make.Tables[0].Rows[0]["YARD_LAYER_TO"] = "1";
					bcls_rec_make.Tables[0].Rows[0]["UNIT_CODE"] = "C301";
				}

			}
			else
			{
				Log::Trace("", __FUNCTION__, "删除材料命令{0}", matNo);
				twma7.Delete();
			}

			Log::Trace("", __FUNCTION__, "检查下层");
			doFlag = f_wmsm_craneCmd_C_layer_update(matNo, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}

		}
		if (bcls_rec_make.Tables[0].Rows.get_Count() > 0)
		{
			doFlag = f_wmsm_CraneCmd_C_Make(&bcls_rec_make, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}

		/*Log::Trace("", __FUNCTION__, "检查下层");
		doFlag = f_wm00_craneCmd_C_layer_update(matNo, bcls_ret, conn);
		if (doFlag != 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}*/
		
		
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
