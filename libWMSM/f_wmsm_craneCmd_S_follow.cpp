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
//#include "twm000f.h"

BM2_FUNCTION_IMPORT
//int f_wmsmsm_craneCmd_S2C_depiler(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);   //板坯库备料命令生成函数
int f_wmsm_cranecmd_S_2E_do(CString mat_no, EIClass * bcls_ret, CDbConnection * conn);	         //板坯库发货命令生成函数
int f_wmsm_CraneCmd_S_Make(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);	     //行车命令形成	
int f_wmsm_craneCmd_layer_update(CString stock_place_no, CString vehicle_no, CString flag, EIClass * bcls_ret, CDbConnection * conn);
int f_wmsm_craneCmd_S_2C_do(CString stock_place_no_fin, EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
//int f_wm00_pileinfocal(CString stock_no, CString stock_place_no, EIClass * bcls_ret, CDbConnection * conn);  //垛位最大高度、重量修正



BM2_FUNCTION_EXPORT
int f_wmsm_craneCmd_S_follow(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{

	/*程序内部变量*/
	int doFlag = 0;
	CString sqlstr = " ";
	CString matNo = " ";
	CString dateTime = " ";
	CString stock_oper_order_fin = " ";
	CString stock_oper_order = " ";
	CString stock_place_no = " ";
	CString stock_place_no_to = " ";
	CString stock_place_no_fin = " ";

	/*数据库操作类定义*/
	CDbCommand cmd_inq(conn);

	/*定义表实体对象*/
	//CTWMA7   twma7(conn);
	//CTWMA2   twma2(conn);
	//CTWMA0   twma0(conn);
	//CTWM04   twm04(conn);
	//CTWM000F twm000f(conn);
	CModel twma7 = CModel("TWMA7");
	CModel twma2 = CModel("TWMA2");
	CModel twma0 = CModel("TWMA0");
	CModel twm04 = CModel("TWM04");
	CModel twm000f = CModel("TWM000F");

	/*数据存放块*/
	CDataTable dtMat;                          //存放材料
	CDataTable dtCmd;                          //存放命令
	CDataTable dtCmd_1;                        //存放命令
	CDataTable dtStockNo;
	CDataTable dtmat1;
	CDataTable down_dtmat;


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
			strcpy(s.msg, "No incoming data.");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//设置行车命令生成函数传入块
		EIClass bcls_rec_make;
		bcls_rec_make.Tables[0].set_TableName("WM00_CMD");
		bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
		bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "STOCK_NO_FROM");
		bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO_FROM");
		bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "STOCK_NO_TO");
		bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "STOCK_NO_FIN");
		bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO_TO");
		bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO_FIN");
		bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");
		bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "CMD_METHOD");
		bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "YARD_LAYER_FROM");
		bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "CMD_SEQ");
		bcls_rec_make.Tables[0].Rows.Add();

		//循环获取传入材料数据块
		for (int i = 0; i < bcls_rec->Tables["WM00_FOLLOW"].Rows.get_Count(); i++)
		{
			matNo = bcls_rec->Tables["WM00_FOLLOW"].Rows[i]["MAT_NO"].ToString().Trim();
			Log::Trace("", __FUNCTION__, "材料号：【{0}】", matNo);
			//判断是否传入材料号
			if (matNo == "")
			{
				strcpy(s.msg, "Mat No. is empty.");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			twma2.Reset();
			twma2["MAT_NO"] = matNo;
			twm000f.Reset();
			twm000f["MAT_NO"] = matNo;
			twma7.Reset();
			twma7["MAT_NO"] = matNo;
			twma7["CRANE_INST_STATUS"] = "0";
			twma7["MAIN_MAT_NO"] = "0";
			twma7.Update("CRANE_INST_STATUS,MAIN_MAT_NO", "MAT_NO");

			twma2.Query("MAT_NO");

			if (twma2["STOCK_PLACE_NO"].ToString().Trim() == "")
			{
				Log::Trace("", __FUNCTION__, "材料库位为空已出库");
				if (twma7.Query("MAT_NO"))
				{
					twma7.Delete();
					doFlag = f_wmsm_craneCmd_layer_update(twma7["STOCK_PLACE_NO_FROM"].ToString(), " ", "", bcls_ret, conn);
					if (doFlag != 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				continue;
			}
			else if (twm000f.Query("MAT_NO") && twm000f["STOCK_PLACE_NO_FIN"].ToString().Trim() != "")
			{
				twma7.Delete();
				doFlag = f_wmsm_craneCmd_S_2C_do(twm000f["STOCK_PLACE_NO_FIN"].ToString(), bcls_rec, bcls_ret, conn);
				if (doFlag != 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
				Log::Trace("", __FUNCTION__, "检查下层1");
				doFlag = f_wmsm_craneCmd_layer_update(twma2["STOCK_PLACE_NO"].ToString(), " ", "", bcls_ret, conn);
				if (doFlag != 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
				continue;
			}

			dtMat.Rows.Clear();
			sqlstr = " select a.layerno, a.mat_no,a.stock_no,a.stock_place_no,b.stock_place_type,c.stock_no_to,c.stock_no_fin,b.dev_div,c.stock_oper_order_fin,c.stock_place_no_fin,c.stock_place_no_to,c.unit_code,c.vehicle_no from twma2 a,twm04 b,twma7 c where a.stock_place_no=b.stock_place_no and c.mat_no=a.mat_no  and a.mat_no='" + matNo + "'";
			Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteQuery(dtMat);
			cmd_inq.Close();
			if (dtMat.Rows.get_Count() > 0)
			{
				stock_oper_order_fin = dtMat.Rows[0]["STOCK_OPER_ORDER_FIN"].ToString().Trim();
				stock_place_no_fin = dtMat.Rows[0]["STOCK_PLACE_NO_FIN"].ToString().Trim();
				Log::Trace("", __FUNCTION__, "最终库业务类型：【{0}】", stock_oper_order_fin);
				if (stock_oper_order_fin == "2C")
				{
					//if (twma2.STOCK_PLACE_NO == dtMat.Rows[0]["STOCK_PLACE_NO_TO"].ToString())
					//{
					//	Log::Trace("", __FUNCTION__, "删除材料备料命令{0}", matNo);
					//	twma7.Delete();
					//	doFlag = f_wm00_craneCmd_S_2C_do(dtMat.Rows[i]["STOCK_PLACE_NO_FIN"].ToString(), bcls_rec, bcls_ret, conn);
					//	if (doFlag != 0)
					//	{
					//		throw CApplicationException(-1, s.msg, log.Location);
					//	}
					//	continue;
					//}
					//else
					//{
					//	Log::Trace("", __FUNCTION__, "刷新备料命令");
					//	EIClass bcls_rec_2c;
					//	bcls_rec_2c.Tables[0].Columns.Add(DT_STRING, "UNIT_CODE");
					//	bcls_rec_2c.Tables[0].Rows.Add();
					//	bcls_rec_2c.Tables[0].Rows[0]["UNIT_CODE"] = dtMat.Rows[0]["UNIT_CODE"].ToString();
					//	Log::Trace("", __FUNCTION__, "传入机组号为：{0}", dtMat.Rows[0]["UNIT_CODE"].ToString());
					//	doFlag = f_wm00_craneCmd_S2C_depiler(&bcls_rec_2c, bcls_ret, conn);
					//	if (doFlag < 0)
					//	{
					//		throw CApplicationException(-1, s.msg, log.Location);
					//	}
					//	continue;
					//}

				}
				else if (stock_oper_order_fin == "2E")
				{
					Log::Trace("", __FUNCTION__, "删除材料发货命令{0}", matNo);
					twma7.Delete();
					doFlag = f_wmsm_cranecmd_S_2E_do(matNo, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				else if (stock_oper_order_fin == "32")
				{
					if (dtMat.Rows[0]["STOCK_NO"].ToString().Trim() == dtMat.Rows[0]["STOCK_NO_FIN"].ToString().Trim())
					{
						Log::Trace("", __FUNCTION__, "材料在目的库区");
						if (dtMat.Rows[0]["DEV_DIV"].ToString().Trim() == "3"&&dtMat.Rows[0]["STOCK_PLACE_TYPE"].ToString().Trim() == "D")
						{
							Log::Trace("", __FUNCTION__, "材料在过跨台车上，生成到目标库区的命令");

							bcls_rec_make.Tables[0].Rows[0]["MAT_NO"] = dtMat.Rows[0]["MAT_NO"].ToString();
							bcls_rec_make.Tables[0].Rows[0]["YARD_LAYER_FROM"] = dtMat.Rows[0]["LAYERNO"].ToString();
							bcls_rec_make.Tables[0].Rows[0]["STOCK_NO_TO"] = dtMat.Rows[0]["STOCK_NO"].ToString();
							bcls_rec_make.Tables[0].Rows[0]["STOCK_PLACE_NO_FROM"] = dtMat.Rows[0]["STOCK_PLACE_NO"].ToString();
							if (dtMat.Rows[0]["STOCK_PLACE_NO_FIN"].ToString().Trim() != "")
							{
								bcls_rec_make.Tables[0].Rows[0]["STOCK_PLACE_NO_TO"] = dtMat.Rows[0]["STOCK_PLACE_NO_FIN"].ToString();
								bcls_rec_make.Tables[0].Rows[0]["STOCK_PLACE_NO_FIN"] = dtMat.Rows[0]["STOCK_PLACE_NO_FIN"].ToString();
							}
							else
							{
								bcls_rec_make.Tables[0].Rows[0]["STOCK_PLACE_NO_TO"] = " ";
							}
							bcls_rec_make.Tables[0].Rows[0]["STOCK_OPER_ORDER"] = "32";
							//调用函数
							doFlag = f_wmsm_CraneCmd_S_Make(&bcls_rec_make, bcls_ret, conn);
							if (doFlag != 0)
							{
								throw CApplicationException(-1, s.msg, log.Location);
							}
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
							Log::Trace("", __FUNCTION__, "材料在过跨台车上，生成到目标库区过跨台车的命令");
							twm04["STOCK_NO"] = dtMat.Rows[0]["STOCK_NO_FIN"].ToString();
							twm04["DEV_DIV"] = "3";
							twm04["STOCK_PLACE_TYPE"] = "D";
							twm04.Query("STOCK_NO,DEV_DIV,STOCK_PLACE_TYPE");
							bcls_rec_make.Tables[0].Rows[0]["MAT_NO"] = dtMat.Rows[0]["MAT_NO"].ToString();
							bcls_rec_make.Tables[0].Rows[0]["YARD_LAYER_FROM"] = dtMat.Rows[0]["LAYERNO"].ToString();
							bcls_rec_make.Tables[0].Rows[0]["STOCK_NO_TO"] = dtMat.Rows[0]["STOCK_NO_FIN"].ToString().Trim();
							bcls_rec_make.Tables[0].Rows[0]["STOCK_NO_FIN"] = dtMat.Rows[0]["STOCK_NO_FIN"].ToString().Trim();
							bcls_rec_make.Tables[0].Rows[0]["STOCK_PLACE_NO_FROM"] = dtMat.Rows[0]["STOCK_PLACE_NO"].ToString();
							bcls_rec_make.Tables[0].Rows[0]["STOCK_PLACE_NO_TO"] = twm04["STOCK_PLACE_NO"];
							bcls_rec_make.Tables[0].Rows[0]["STOCK_OPER_ORDER"] = "32";
							if (dtMat.Rows[0]["STOCK_PLACE_NO_FIN"].ToString().Trim() != "")
							{
								bcls_rec_make.Tables[0].Rows[0]["STOCK_PLACE_NO_FIN"] = dtMat.Rows[0]["STOCK_PLACE_NO_FIN"].ToString();
							}
							else
							{
								bcls_rec_make.Tables[0].Rows[0]["STOCK_PLACE_NO_FIN"] = " ";
							}
							//调用函数
							doFlag = f_wmsm_CraneCmd_S_Make(&bcls_rec_make, bcls_ret, conn);
							if (doFlag != 0)
							{
								throw CApplicationException(-1, s.msg, log.Location);
							}
						}
						else
						{
							Log::Trace("", __FUNCTION__, "材料不在过跨台车上，生成到过跨台车的命令");
							twm04["STOCK_NO"] = dtMat.Rows[0]["STOCK_NO"].ToString();
							twm04["DEV_DIV"] = "3";
							twm04["STOCK_PLACE_TYPE"] = "D";
							twm04.Query("STOCK_NO,DEV_DIV,STOCK_PLACE_TYPE");

							bcls_rec_make.Tables[0].Rows[0]["MAT_NO"] = dtMat.Rows[0]["MAT_NO"].ToString();
							bcls_rec_make.Tables[0].Rows[0]["YARD_LAYER_FROM"] = dtMat.Rows[0]["LAYERNO"].ToString();
							bcls_rec_make.Tables[0].Rows[0]["STOCK_NO_TO"] = dtMat.Rows[0]["STOCK_NO"].ToString();
							bcls_rec_make.Tables[0].Rows[0]["STOCK_PLACE_NO_FROM"] = dtMat.Rows[0]["STOCK_PLACE_NO"].ToString();
							bcls_rec_make.Tables[0].Rows[0]["STOCK_PLACE_NO_TO"] = twm04["STOCK_PLACE_NO"].ToString();
							bcls_rec_make.Tables[0].Rows[0]["STOCK_OPER_ORDER"] = "32";
							if (dtMat.Rows[0]["STOCK_PLACE_NO_FIN"].ToString().Trim() != "")
							{
								bcls_rec_make.Tables[0].Rows[0]["STOCK_PLACE_NO_FIN"] = dtMat.Rows[0]["STOCK_PLACE_NO_FIN"].ToString();
							}
							else
							{
								bcls_rec_make.Tables[0].Rows[0]["STOCK_PLACE_NO_FIN"] = " ";
							}
							//调用函数
							doFlag = f_wmsm_CraneCmd_S_Make(&bcls_rec_make, bcls_ret, conn);
							if (doFlag != 0)
							{
								throw CApplicationException(-1, s.msg, log.Location);
							}
						}
					}
				}
				else
				{
					Log::Trace("", __FUNCTION__, "其他情况，判断材料所在库位");

					if (dtMat.Rows[0]["DEV_DIV"].ToString().Trim() == "3")
					{
						Log::Trace("", __FUNCTION__, "材料在过跨台车上，生成到目标库区的命令");

						bcls_rec_make.Tables[0].Rows[0]["MAT_NO"] = dtMat.Rows[0]["MAT_NO"].ToString();
						bcls_rec_make.Tables[0].Rows[0]["YARD_LAYER_FROM"] = dtMat.Rows[0]["LAYERNO"].ToString();
						bcls_rec_make.Tables[0].Rows[0]["STOCK_NO_TO"] = dtMat.Rows[0]["STOCK_NO"].ToString();
						bcls_rec_make.Tables[0].Rows[0]["STOCK_PLACE_NO_FROM"] = dtMat.Rows[0]["STOCK_PLACE_NO"].ToString();
						bcls_rec_make.Tables[0].Rows[0]["STOCK_PLACE_NO_TO"] = " ";
						bcls_rec_make.Tables[0].Rows[0]["STOCK_OPER_ORDER"] = "32";
						//调用函数
						doFlag = f_wmsm_CraneCmd_S_Make(&bcls_rec_make, bcls_ret, conn);
						if (doFlag != 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}
					}
					else if (dtMat.Rows[0]["DEV_DIV"].ToString().Trim() == "2" || dtMat.Rows[0]["DEV_DIV"].ToString().Trim() == "1")
					{
						Log::Trace("", __FUNCTION__, "材料在火车或卡车上，更新from位置");
						twma7["STOCK_PLACE_NO_FROM"] = dtMat.Rows[0]["STOCK_PLACE_NO"].ToString();
						twma7.Update("STOCK_PLACE_NO_FROM", "MAT_NO");

					}
					else
					{
						Log::Trace("", __FUNCTION__, "材料在正常库位上，判断下层是否有命令");
						dtMat.Rows[0]["LAYERNO"] = dtMat.Rows[0]["LAYERNO"].ToDecimal() - 1;
						sqlstr = "select CMD_SEQ from twma7 a,twma2 b where a.mat_no=b.mat_no and b.layerno='" + dtMat.Rows[0]["LAYERNO"].ToString() + "' and b.stock_place_no='" + dtMat.Rows[0]["STOCK_PLACE_NO"].ToString() + "'";
						Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
						cmd_inq.SetCommandText(sqlstr);
						cmd_inq.ExecuteQuery(down_dtmat);
						cmd_inq.Close();
						if (down_dtmat.Rows.get_Count() > 0)
						{
							Log::Trace("", __FUNCTION__, "材料{0}下层有命令，生成倒跺命令", matNo);
							twma7.Delete("MAT_NO");
							dtMat.Rows[0]["LAYERNO"] = dtMat.Rows[0]["LAYERNO"].ToDecimal() + 1;

							bcls_rec_make.Tables[0].Rows[0]["MAT_NO"] = dtMat.Rows[0]["MAT_NO"].ToString();
							bcls_rec_make.Tables[0].Rows[0]["YARD_LAYER_FROM"] = dtMat.Rows[0]["LAYERNO"].ToString();
							bcls_rec_make.Tables[0].Rows[0]["STOCK_NO_TO"] = dtMat.Rows[0]["STOCK_NO"].ToString();
							bcls_rec_make.Tables[0].Rows[0]["STOCK_PLACE_NO_FROM"] = dtMat.Rows[0]["STOCK_PLACE_NO"].ToString();
							bcls_rec_make.Tables[0].Rows[0]["STOCK_PLACE_NO_TO"] = " ";
							bcls_rec_make.Tables[0].Rows[0]["STOCK_OPER_ORDER"] = "31";
							bcls_rec_make.Tables[0].Rows[0]["CMD_SEQ"] = down_dtmat.Rows[0]["CMD_SEQ"];
							//调用函数
							doFlag = f_wmsm_CraneCmd_S_Make(&bcls_rec_make, bcls_ret, conn);
							if (doFlag != 0)
							{
								throw CApplicationException(-1, s.msg, log.Location);
							}
						}
						else
						{
							Log::Trace("", __FUNCTION__, "删除材料命令{0}", matNo);
							twma7.Delete();
						}
					}
				}
			}

			Log::Trace("", __FUNCTION__, "检查下层2");
			doFlag = f_wmsm_craneCmd_layer_update(twma2["STOCK_PLACE_NO"].ToString(), " ", " ", bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}

		}



	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };

		/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/
		CMessageFormat::Format(s.msg, "Database processing error. sqlcode=[{0}].", arguments, 1);
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
