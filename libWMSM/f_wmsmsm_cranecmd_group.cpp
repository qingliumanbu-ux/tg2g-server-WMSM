/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:      JQ
Version:     1.1.1
Date:        2017-3-16
Description: 吊车命令组吊判断
**************************************************/
/*
组吊规则：
1. 总重量限制(60t)。
2. 夹钳深度限制：夹钳深度=夹钳上所有板坯的厚度之和。
3. 块数限制（≤4）。
4. 吊运组内板坯的宽度差：（最大宽度-最小宽度）≤设定的宽度差。
5. 吊运组内板坯的厚度差：（最大厚度-最小厚度）≤设定的厚度差。
6. 相邻板坯的中心点偏差：不包含“非自动模式吊运的板坯”。（行车无人化以后再启用此规则）
*/

/* C/C++ 的标准头文件部分 */
#include "stdafx.h"		// 框架头，不可删除 
#include "math.h"	

BM2_FUNCTION_EXPORT
int f_wmsmsm_cranecmd_group(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{

	//程序内部变量
	CString sqlstr = " ";
	int doFlag = 0;	
	
    //业务变量	
	CDecimal gropu_wt = 0;	
	CDecimal gropu_thi = 0;
	CDecimal gropu_no = 0;
	int group_num = 0;
	int content_Flag = 0;
	int new_flag = 0;
	
	//数据块
	CDataTable Table_mat;                  //存放传入材料信息
	CDataTable Table_car;                  //存放吊车信息

	try
	{
		//项目自定义日志
		CTracer log(__FUNCTION__);

		//检验传入数据块
		if (!bcls_rec->Tables.Contains("CMD_GROUP") || bcls_rec->Tables["CMD_GROUP"].Rows.get_Count() == 0)
		{
			sprintf(s.msg, "函数f_wmsmsm_cranecmd_group中找不到接收块名[CMD_GROUP]或值为空");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		

		//获取吊车属性
		//sqlstr = " select CRA_NUM_MAX,CRA_WEI_MAX,CRA_DEEPTH,CRA_THI_DIFF,CRA_WID_DIFF from twm06 where CRANE_NO='1C01'";
		//Db::QueryTable(sqlstr, Table_car);
		//if (Table_car.Rows.get_Count() ==0)
		//{
		//	sprintf(s.msg, "台车属性不存在");
		//	throw CApplicationException(-1, s.msg, log.Location);			
		//}
		//Log::Trace("", __FUNCTION__, "吊具最大块数：{0}", Table_car.Rows[0]["CRA_NUM_MAX"].ToDouble());
		//Log::Trace("", __FUNCTION__, "吊具最大重量：{0}", Table_car.Rows[0]["CRA_WEI_MAX"].ToDouble());
		//Log::Trace("", __FUNCTION__, "吊具深度：{0}", Table_car.Rows[0]["CRA_DEEPTH"].ToDouble());
		//Log::Trace("", __FUNCTION__, "吊具组合吊厚度差：{0}", Table_car.Rows[0]["CRA_THI_DIFF"].ToDouble());
		//Log::Trace("", __FUNCTION__, "吊具组合吊宽度差：{0}", Table_car.Rows[0]["CRA_WID_DIFF"].ToDouble());
		//		
		//for (int i = 0; i < bcls_rec->Tables["CMD_GROUP"].Rows.get_Count(); i++)
		//{
		//	Log::Trace("", __FUNCTION__, "垛位号：{0}", bcls_rec->Tables["CMD_GROUP"].Rows[i]["STOCK_PLACE_NO"].ToString());
		//	
		//	//从台车到台车不需要计算组吊号
		//	if (Db::QueryCDecimal("select COUNT(1) from twma7 A,TWM04 B where A.stock_place_no_to=B.stock_place_no AND STOCK_PLACE_NO_FROM='" + bcls_rec->Tables["CMD_GROUP"].Rows[i]["STOCK_PLACE_NO"].ToString() + "' and B.dev_div='3' AND B.STOCK_PLACE_TYPE='D'")>0
		//		&& Db::QueryCDecimal("select COUNT(1) from twma7 A,TWM04 B where A.STOCK_PLACE_NO_FROM=B.stock_place_no AND STOCK_PLACE_NO_FROM='" + bcls_rec->Tables["CMD_GROUP"].Rows[i]["STOCK_PLACE_NO"].ToString() + "' and B.dev_div='3' AND B.STOCK_PLACE_TYPE='D'")>0)
		//	{
		//		Log::Trace("", __FUNCTION__, "从台车到台车不需要计算组吊号");
		//		continue;
		//	}
		//	
		//	//获取垛位材料
		//	Table_mat.Rows.Clear();
		//	sqlstr = "select a.mat_no,a.mat_theory_wt,b.layerno,a.mat_thick,a.mat_width,c.send_flag,c.crane_cmdgrpno,c.stock_place_no_to,c.stock_oper_order,c.crane_inst_status,c.BATCH_NO from TOPHPMMS1 a,TWMA2 b,TWMA7 c  where a.mat_no=b.mat_no and c.mat_no=a.mat_no and b.stock_place_no='" + bcls_rec->Tables["CMD_GROUP"].Rows[i]["STOCK_PLACE_NO"].ToString() + "'  order by int(b.layerno) desc";
		//	Db::QueryTable(sqlstr, Table_mat);

		//	group_num = 0;
		//	gropu_wt = 0;
		//	gropu_no = 0;
		//	gropu_thi = 0;

		//	for (int j = 0; j < Table_mat.Rows.get_Count(); j++)
		//	{
		//		//材料信息			
		//		Log::Trace("", __FUNCTION__, "材料号：{0}", Table_mat.Rows[j]["MAT_NO"].ToString());
		//		Log::Trace("", __FUNCTION__, "层号：{0}", Table_mat.Rows[j]["LAYERNO"].ToString());
		//		Log::Trace("", __FUNCTION__, "重量：{0}", Table_mat.Rows[j]["MAT_THEORY_WT"].ToDecimal());
		//		Log::Trace("", __FUNCTION__, "宽度：{0}", Table_mat.Rows[j]["MAT_WIDTH"].ToDouble());
		//		Log::Trace("", __FUNCTION__, "厚度：{0}", Table_mat.Rows[j]["MAT_THICK"].ToDouble());
		//		Log::Trace("", __FUNCTION__, "组吊号：{0}", Table_mat.Rows[j]["CRANE_CMDGRPNO"].ToDecimal());
		//		Log::Trace("", __FUNCTION__, "命令类型：{0}", Table_mat.Rows[j]["STOCK_OPER_ORDER"].ToString());
		//		Log::Trace("", __FUNCTION__, "发送标记：{0}", Table_mat.Rows[j]["SEND_FLAG"].ToString());

		//		//已发送命令不再计算组吊(20171112 修改已发送命令也计算组吊)
		//		//上料命令不计算组吊
		//		//目标位置为空不计算组吊
		//		if (//Table_mat.Rows[j]["SEND_FLAG"].ToString().Trim() == "1" ||
		//			 Table_mat.Rows[j]["STOCK_OPER_ORDER"].ToString().Trim() == "2B"
		//			|| Table_mat.Rows[j]["STOCK_PLACE_NO_TO"].ToString().Trim() == "")
		//		{
		//			group_num = 0;
		//			gropu_wt = 0;
		//			gropu_no = 0;		
		//			gropu_thi = 0;
		//			continue;
		//		}

		//		if (Table_mat.Rows[j]["CRANE_CMDGRPNO"].ToDecimal() != 0)
		//		{
  //                  //判断上层组吊号是否都一致
		//			Log::Trace("", __FUNCTION__, "j：{0}", j);
		//			new_flag = 0;
		//			int break_flag = 0;
		//			for (int m = j - 1; m>=0; m--)
		//			{
		//				Log::Trace("", __FUNCTION__, "m：{0}", m);
		//				if (Table_mat.Rows[m]["CRANE_CMDGRPNO"].ToDecimal() == Table_mat.Rows[j]["CRANE_CMDGRPNO"].ToDecimal())
		//				{
		//					Log::Trace("", __FUNCTION__, "mnew_flag1：{0}", new_flag);
		//					if (new_flag==1)
		//					{
		//						Table_mat.Rows[j]["CRANE_CMDGRPNO"] = 0;
		//						j--;
		//						break_flag = 1;
		//						break;
		//					}
		//				}
		//				else
		//				{
		//					Log::Trace("", __FUNCTION__, "mnew_flag2：{0}", new_flag);
		//					new_flag = 1;
		//				}
		//			}
		//			Log::Trace("", __FUNCTION__, "jdddd：{0}", j);
		//			if (break_flag == 1)continue;

		//			if (group_num == 0)
		//			{
		//				Log::Trace("", __FUNCTION__, "11");
		//				group_num=1;
		//				gropu_no = Table_mat.Rows[j]["CRANE_CMDGRPNO"].ToDecimal();
		//				gropu_wt = Table_mat.Rows[j]["MAT_THEORY_WT"].ToDouble();
		//				gropu_thi = Table_mat.Rows[j]["MAT_THICK"].ToDouble();
		//			}
		//			else
		//			{
		//				if (Table_mat.Rows[j]["CRANE_CMDGRPNO"].ToDecimal() == gropu_no)
		//				{
		//					Log::Trace("", __FUNCTION__, "22");
		//					group_num++;
		//					gropu_wt = gropu_wt + Table_mat.Rows[j]["MAT_THEORY_WT"].ToDouble();
		//					gropu_thi = gropu_thi + Table_mat.Rows[j]["MAT_THICK"].ToDouble();
		//				}
		//				else
		//				{
		//					Log::Trace("", __FUNCTION__, "33");
		//					group_num = 1;
		//					gropu_wt = Table_mat.Rows[j]["MAT_THEORY_WT"].ToDouble();
		//					gropu_no = Table_mat.Rows[j]["CRANE_CMDGRPNO"].ToDecimal();
		//					gropu_thi = Table_mat.Rows[j]["MAT_THICK"].ToDouble();
		//				}
		//			}
		//			continue;
		//		}
		//		else
		//		{
		//			Log::Trace("", __FUNCTION__, "group_num1[{0}]", group_num);

		//			if (group_num == 0)
		//			{
		//				group_num = 1;
		//				gropu_wt = Table_mat.Rows[j]["MAT_THEORY_WT"].ToDouble();
		//				gropu_thi = Table_mat.Rows[j]["MAT_THICK"].ToDouble();
		//				gropu_no = 0;
		//			}
		//			else
		//			{
		//				if (Table_mat.Rows[j - 1]["SEND_FLAG"].ToString().Trim() == "1"
		//				&&Table_mat.Rows[j - 1]["CRANE_INST_STATUS"].ToString().Trim() != "0")
		//				{
		//					Log::Trace("", __FUNCTION__, "sended");
		//					//上层材料命令已发送
		//					group_num = 1;
		//					gropu_wt = Table_mat.Rows[j]["MAT_THEORY_WT"].ToDouble();
		//					gropu_thi = Table_mat.Rows[j]["MAT_THICK"].ToDouble();
		//					gropu_no = 0;
		//				}
		//				else
		//				{	
		//					Log::Trace("", __FUNCTION__, "nosended");
		//					Log::Trace("", __FUNCTION__, "STOCK_PLACE_NO_TO{0}", Table_mat.Rows[j]["STOCK_PLACE_NO_TO"].ToString());
		//					Log::Trace("", __FUNCTION__, "STOCK_PLACE_NO_TO{0}", Table_mat.Rows[j - 1]["STOCK_PLACE_NO_TO"].ToString());
		//					Log::Trace("", __FUNCTION__, "BATCH_NO{0}", Table_mat.Rows[j]["BATCH_NO"].ToString());
		//					Log::Trace("", __FUNCTION__, "BATCH_NO{0}", Table_mat.Rows[j - 1]["BATCH_NO"].ToString());
		//					Log::Trace("", __FUNCTION__, "gropu_wt{0}", gropu_wt);
		//					Log::Trace("", __FUNCTION__, "group_num{0}", group_num);
		//					Log::Trace("", __FUNCTION__, "gropu_thi{0}", gropu_thi);
		//					Log::Trace("", __FUNCTION__, "MAT_THEORY_WT{0}", Table_mat.Rows[j]["MAT_THEORY_WT"].ToDouble());
		//					Log::Trace("", __FUNCTION__, "MAT_THICK{0}", Table_mat.Rows[j]["MAT_THICK"].ToDouble());

		//					if (Table_mat.Rows[j]["STOCK_PLACE_NO_TO"].ToString() == Table_mat.Rows[j - 1]["STOCK_PLACE_NO_TO"].ToString()
		//						&& Table_mat.Rows[j]["BATCH_NO"].ToDecimal() == Table_mat.Rows[j - 1]["BATCH_NO"].ToDecimal()
		//						&& gropu_wt <= Table_car.Rows[0]["CRA_WEI_MAX"].ToDouble() - Table_mat.Rows[j]["MAT_THEORY_WT"].ToDouble()
		//						&& group_num < Table_car.Rows[0]["CRA_NUM_MAX"].ToDouble()
		//						&& gropu_thi <= Table_car.Rows[0]["CRA_DEEPTH"].ToDouble() - Table_mat.Rows[j]["MAT_THICK"].ToDouble())
		//					{								
		//						content_Flag = 0;

		//						//新增验证如果上面板坯宽，不能超过50
		//						if (Table_mat.Rows[j]["MAT_WIDTH"].ToDouble()<Table_mat.Rows[j - 1]["MAT_WIDTH"].ToDouble()
		//							&& Table_mat.Rows[j-1]["MAT_WIDTH"].ToDouble() - Table_mat.Rows[j]["MAT_WIDTH"].ToDouble()>50)
		//						{
		//							content_Flag = 1;
		//						}

		//						if (content_Flag)
		//						{
		//							for (int t = 0; t < group_num; t++)
		//							{
		//								if (fabs(Table_mat.Rows[j]["MAT_THICK"].ToDouble() - Table_mat.Rows[j - t]["MAT_THICK"].ToDouble()) <= Table_car.Rows[0]["CRA_THI_DIFF"].ToDouble()
		//									&& fabs(Table_mat.Rows[j]["MAT_WIDTH"].ToDouble() - Table_mat.Rows[j - t]["MAT_WIDTH"].ToDouble()) <= Table_car.Rows[0]["CRA_WID_DIFF"].ToDouble())
		//								{
		//									continue;
		//								}
		//								else
		//								{
		//									content_Flag = 1;
		//									break;
		//								}
		//							}
		//						}
		//						
		//						if (content_Flag == 0)
		//						{
		//							Log::Trace("", __FUNCTION__, "能组吊");
		//							if (group_num == 1)
		//							{
		//							
		//								sqlstr = "values nextval for SEQ_BATCH_GROUP";
		//								gropu_no = Db::QueryCDecimal(sqlstr);
		//								group_num++;
		//								gropu_wt = gropu_wt + Table_mat.Rows[j]["MAT_THEORY_WT"].ToDouble();
		//								gropu_thi = gropu_thi + Table_mat.Rows[j]["MAT_THICK"].ToDouble();
		//								sqlstr = "update twma7 set SEND_FLAG='0',CRANE_CMDGRPNO=" + gropu_no.ToString() + " where mat_no='"+Table_mat.Rows[j]["MAT_NO"].ToString()+"'";
		//								Log::Trace("", __FUNCTION__, "1{0}", sqlstr);
		//								Db::Execute(sqlstr);
		//								sqlstr = "update twma7 set SEND_FLAG='0',CRANE_CMDGRPNO=" + gropu_no.ToString() + " where mat_no='" + Table_mat.Rows[j-1]["MAT_NO"].ToString() + "'";
		//								Log::Trace("", __FUNCTION__, "2{0}", sqlstr);
		//								Db::Execute(sqlstr);
		//								continue;
		//							}
		//							else
		//							{
		//								group_num++;
		//								gropu_wt = gropu_wt + Table_mat.Rows[j]["MAT_THEORY_WT"].ToDouble();
		//								gropu_thi = gropu_thi + Table_mat.Rows[j]["MAT_THICK"].ToDouble();
		//								sqlstr = "update twma7 set SEND_FLAG='0',CRANE_CMDGRPNO=" + gropu_no.ToString() + " where mat_no='" + Table_mat.Rows[j]["MAT_NO"].ToString() + "'";
		//								Log::Trace("", __FUNCTION__, "3{0}", sqlstr);
		//								Db::Execute(sqlstr);
		//								continue;
		//							}
		//							
		//						}
		//						else
		//						{
		//							Log::Trace("", __FUNCTION__, "不能组吊1");
		//							group_num = 1;
		//							gropu_wt = Table_mat.Rows[j]["MAT_THEORY_WT"].ToDouble();
		//							gropu_thi = Table_mat.Rows[j]["MAT_THICK"].ToDouble();
		//							gropu_no = 0;
		//							continue;
		//						}
		//					}
		//					else
		//					{
		//						Log::Trace("", __FUNCTION__, "不能组吊2");
		//						group_num = 1;
		//						gropu_wt = Table_mat.Rows[j]["MAT_THEORY_WT"].ToDouble();
		//						gropu_thi = Table_mat.Rows[j]["MAT_THICK"].ToDouble();
		//						gropu_no = 0;
		//						continue;
		//					}

		//				}						
		//			}
		//			
		//		}
		//	}
		//	
		//}	
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