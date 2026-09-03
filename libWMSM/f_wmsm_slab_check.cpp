/*=========================================================================
//程序名称:		f_ymsmSlabQtCheck
//隶属子系统:	YM
//产品名称:		MG2SM梅钢二炼钢L3
//创建人员:		LWB
//创建时间:		2014-06-4
//修改人员:   LWB  (增加硬度组选择宽度差值)
//修改日期:   2020-08-12
//-----------------------------------------------------------------------
//功能描述:	 板坯质量判定校验流程
//条件描述：
//数据库表:
//
//主调用函数:
//
//-----------------------------------------------------------------------
//函数功能:     板坯质量判定校验
//传入参数:     板坯长度，宽度，头宽，尾宽，精整标记，改钢标记，改钢类型
//传出参数:     isLock 1为合格/0为封锁
//处理流程:
//=========================================================================*/

//#include "WM_Utility.h"
#include "stdafx.h"
#include "tqmts9cc.h"
#include "tmmsm01.h"
#include "tpssm03.h"
#include "math.h"
BM2_FUNCTION_IMPORT	


BM2_FUNCTION_EXPORT
int f_wmsm_slab_check(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/*程序用变量*/
	int doFlag = 0;
	int ret = 0;
	int sqlid = 0;
	int blckNum = -1;

	int isLock = 1;//0为封锁，1为合格

	CDecimal  v_slab_max_len = 0; //计划板坯长度最大值
	CDecimal  v_slab_min_len = 0; //计划板坯长度最小值
	/* Pro*c 标准头文件部分  */

	/********表结构引用*********/

	CTQMTS9CC tqmts9cc(conn);
	CTMMSM01 tmmsm01(conn);
	CTMMSM01 tpssm03(conn);
	CDecimal L = 0;		//板坯长度
	CDecimal W = 0;		//板坯宽度
	CDecimal Wb = 0;	//头宽
	CDecimal Wt = 0;	//尾宽
	//Decimal v_slab_tapper_width_lmt = 30;//宽差
	CString v_mat_destion = " ";//板坯去向
	CString A = " ";	//精整标记
	CString B = " ";	//改钢标记
	CString C = " ";	//改钢类型
	CString v_mat_no = " "; //板坯号
	//char v_pch_judge_code[1+1] = " ";//性能判定
	CString v_hardness_group = " ";//硬度
	CString v_st_no = " "; //出钢记号

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);
	CDbCommand cmd_inq2(conn);
	CDbCommand cmd_inq3(conn);
	CString sqlstr = "";

	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


	try
	{
		EDLog(1, 1, "-----------------f_ymsmSlabQtCheck begin(2020-08-12) -----------------");
		//接收板坯位置信息
		
		L = bcls_rec->Tables[0].Rows[0]["L"];
		W = bcls_rec->Tables[0].Rows[0]["W"];
		Wb = bcls_rec->Tables[0].Rows[0]["Wb"];
		Wt = bcls_rec->Tables[0].Rows[0]["Wt"];
		A = bcls_rec->Tables[0].Rows[0]["A"].ToString();
		B = bcls_rec->Tables[0].Rows[0]["B"].ToString();
		C = bcls_rec->Tables[0].Rows[0]["C"].ToString();
		v_mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString();

		Log::Trace("", __FUNCTION__, "L[{0}],W[{1}],Wb[{2}],Wt[{3}]", L, W, Wb, Wt);
		Log::Trace("", __FUNCTION__, "A[{0}]", A);
		

		//获取板坯对应信息：出钢记号、性能封锁标志
		tmmsm01.MAT_NO = v_mat_no;
		tmmsm01.Query("MAT_NO");

		sqlstr =
			" SELECT SLAB_MAX_LEN,SLAB_MIN_LEN  FROM TPSSM03 WHERE PONO =@pono"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("pono", tmmsm01.PONO);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			v_slab_max_len = cmd_inq.GetDecimal(1);
			v_slab_min_len = cmd_inq.GetDecimal(2);
		}
		cmd_inq.Close();

		Log::Trace("", __FUNCTION__, "板坯出钢记号[{0}]", tmmsm01.ST_NO);
		Log::Trace("", __FUNCTION__, "板坯去向[{0}]", tmmsm01.MAT_DESTION);
	

		if (tmmsm01.MAT_DESTION != "00" && tmmsm01.MAT_DESTION != "01")
		{
			tmmsm01.MAT_DESTION = "10";
			Log::Trace("", __FUNCTION__, "板坯去向[{0}]", tmmsm01.MAT_DESTION);

		}
		//20221028：根据王劲松联系，强制热送2热轧01的板坯头尾宽差允许为50mm
		if ((tmmsm01.HOT_SEND_FLAG == "2"|| tmmsm01.HOT_SEND_FLAG == "3") && tmmsm01.MAT_DESTION == "01")
		{
			tqmts9cc.SLAB_TAPPER_WIDTH_LMT = 50;
		}
		else
		{
			//如果宽度有偏差则需要从静态表中取宽度偏差允许值
			//if(abs(Wb-Wt) > 30)
			//{
			EDLog(1, 1, "取规格要求标准");

			/********根据硬度组获取tqmts9cc表中规格允许参数数据***********/
			//取出钢记号的前两位为作为特殊硬度组
			v_hardness_group = tmmsm01.ST_NO.Substring(1,2);

			sqlstr =
				" SELECT * FROM tqmts9cc WHERE STRAND_NO = @mat_destion  AND STEEL_GROUP =@v_hardness_group FETCH FIRST 1 ROWS ONLY "
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("mat_destion", tmmsm01.MAT_DESTION);
			cmd_inq.Parameters.Set("v_hardness_group", v_hardness_group);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				cmd_inq.Fetch(tqmts9cc);
			}
			else 
			{
				//EDLog(1,1, "没有找到特殊的硬度组");
				//取出钢记号对应的硬度组
				/*sqlstr =
					" SELECT HARDNESS_GROUP FROM TQMTS0X WHERE ST_NO = @tmmsm01.ST_NO "
					;
				cmd_inq1.SetCommandText(sqlstr);
				cmd_inq1.Parameters.Set("tmmsm01.ST_NO", tmmsm01.ST_NO);
				cmd_inq1.ExecuteReader();
				if (cmd_inq1.Read())
				{
					v_hardness_group = cmd_inq1.GetString(1);
				}
				cmd_inq1.Close();*/

				v_hardness_group = "00";
				
				sqlstr =
					" SELECT * FROM tqmts9cc WHERE STRAND_NO = @mat_destion  AND STEEL_GROUP =@v_hardness_group FETCH FIRST 1 ROWS ONLY "
					;
				cmd_inq1.SetCommandText(sqlstr);
				cmd_inq1.Parameters.Set("mat_destion", tmmsm01.MAT_DESTION);
				cmd_inq1.Parameters.Set("v_hardness_group", v_hardness_group);
				cmd_inq1.ExecuteReader();
				if (cmd_inq1.Read())
				{
					cmd_inq1.Fetch(tqmts9cc);
				}
				else
				{
					//EDLog(1,1, "宽差 = [%f]",v_slab_tapper_width_lmt);
				    //找不到则取默认值
					
					sqlstr =
						" SELECT * FROM tqmts9cc WHERE STRAND_NO = @mat_destion  AND STEEL_GROUP =@v_hardness_group FETCH FIRST 1 ROWS ONLY "
						;
					cmd_inq2.SetCommandText(sqlstr);
					cmd_inq2.Parameters.Set("mat_destion", tmmsm01.MAT_DESTION);
					cmd_inq2.Parameters.Set("v_hardness_group", v_hardness_group);
					cmd_inq2.ExecuteReader();
					if (cmd_inq2.Read())
					{
						cmd_inq2.Fetch(tqmts9cc);
					}
					else
					{
						sqlstr =
							" SELECT * FROM tqmts9cc WHERE STRAND_NO = @mat_destion  AND STEEL_GROUP ='00' FETCH FIRST 1 ROWS ONLY "
							;
						cmd_inq3.SetCommandText(sqlstr);
						cmd_inq3.Parameters.Set("mat_destion", tmmsm01.MAT_DESTION);
						cmd_inq3.ExecuteReader();
						if (cmd_inq3.Read())
						{
							cmd_inq3.Fetch(tqmts9cc);
						}
						else
						{
							EDLog(1, 1, "没有找到，取默认值");
							tqmts9cc.SLAB_TAPPER_WIDTH_LMT = 30;
						}
						cmd_inq3.Close();
					}
					cmd_inq2.Close();
				}		
				
			}
			cmd_inq.Close();

		}

		if (tqmts9cc.SLAB_TAPPER_WIDTH_LMT < 30)
		{
			tqmts9cc.SLAB_TAPPER_WIDTH_LMT = 30;
		}

		//EDLog(1,1, "宽差 = [%f]",v_slab_tapper_width_lmt);
		//判断开始
		//精整标记
		EDLog(1, 1, "精整标记判断 A[{0}]", A);
		if (A >= "3")
		{
			EDLog(1, 1, "板坯未处置完，不能判合格");
			//strcpy(s.msg, "板坯未处置完，不能判合格");
			isLock = 0;
			strcpy(s.msg, "板坯未处置完，不能判合格");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		//改钢标记
		Log::Trace("", __FUNCTION__, "B[{0}],C[{1}]", B,C);
		if (B > "00")//需改钢
		{
			if (C == " " ||C == "5")
			{
				//strcpy(s.msg, "板坯需要改钢，不能判合格。");
				EDLog(1, 1, "板坯需要改钢，不能判合格");
				isLock = 0;
				strcpy(s.msg, "板坯需要改钢，不能判合格");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}
		//宽差判断
		if ((Wb - Wt) > 0)
		{
			if (Wb - Wt > tqmts9cc.SLAB_TAPPER_WIDTH_LMT)
			{
				strcpy(s.msg, "板坯宽差超标准，不能判合格。");
				isLock = 0;
				strcpy(s.msg, "板坯宽差超标准，不能判合格。");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}
		else if ((Wb - Wt) < 0)
		{
			if (-(Wb - Wt) > tqmts9cc.SLAB_TAPPER_WIDTH_LMT)
			{
				strcpy(s.msg, "板坯宽差超标准，不能判合格。");
				isLock = 0;
				strcpy(s.msg, "板坯宽差超标准，不能判合格。");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}
		
		if ((W - (Wb + Wt) / 2) > 0)
		{
			if ((W - (Wb + Wt) / 2) > 15)
			{
				strcpy(s.msg, "板坯宽度信息与头尾宽度平均值不满足要求，不能判合格。");
				isLock = 0;
				strcpy(s.msg, "板坯宽度信息与头尾宽度平均值不满足要求，不能判合格。");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}
		else if ((W - (Wb + Wt) / 2) < 0)
		{
			if (-(W - (Wb + Wt) / 2) > 15)
			{
				strcpy(s.msg, "板坯宽度信息与头尾宽度平均值不满足要求，不能判合格。");
				isLock = 0;
				strcpy(s.msg, "板坯宽度信息与头尾宽度平均值不满足要求，不能判合格。");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}
		

		//规格(长度)判断
		EDLog(1, 1, "长度判断");
		if (L > (tqmts9cc.SLAB_FIX_L_MAX + 50) || L < (tqmts9cc.SLAB_FIX_S_MIN - 30) || (tqmts9cc.SLAB_FIX_S_MAX < L && tqmts9cc.SLAB_FIX_L_MIN > L))
		{
			EDLog(1, 1, "不满足标准，长坯上限 = [%d], 长坯下限 = [%d], 短坯上限 = [%d], 短片下限 = [%d]", tqmts9cc.SLAB_FIX_L_MAX, tqmts9cc.SLAB_FIX_L_MIN, tqmts9cc.SLAB_FIX_S_MAX, tqmts9cc.SLAB_FIX_S_MIN);
			if (tmmsm01.PREC_SLAB_NO !=" ")
			{
				if (L > v_slab_max_len || L < v_slab_min_len)
				{
					EDLog(1, 1, "不满足标准，也不满足计划上限 = [%d], 长坯下限 = [%d]", v_slab_max_len, v_slab_min_len);
					isLock = 0;
					strcpy(s.msg, "板坯长度不满足，不能判合格。");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}
			else
			{
				isLock = 0;
				strcpy(s.msg, "板坯长度不满足，不能判合格。");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}
		EDLog(1, 1, "宽度判断");
		if (W > tqmts9cc.SLAB_MAX_WIDTH || W < tqmts9cc.SLAB_MIN_WIDTH)
		{
			EDLog(1, 1, "不满足标准，宽度上限 = [%d], 宽度下限 = [%d]", tqmts9cc.SLAB_MAX_WIDTH, tqmts9cc.SLAB_MIN_WIDTH);
			isLock = 0;
			strcpy(s.msg, "板坯宽度不满足标准");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		//规格(长度与宽度匹配)判断
		if (W > 1320 || Wb > 1320 || Wt > 1320)
		{
			if (L < 4500 || (7000 <= L && 8000 > L))
			{
				//strcpy(s.msg, "板坯长度与宽度不匹配，不能判合格。");
				EDLog(1, 1, "板坯长度与宽度不匹配，不能判合格");
				isLock = 0;
				strcpy(s.msg, "板坯长度与宽度不匹配，不能判合格");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}
		else if (W < 900 || Wb < 900 || Wt < 900)
		{
			if ((L > 4600 && 7000 > L) || 9600 < L)
			{
				//strcpy(s.msg, "板坯长度与宽度不匹配，不能判合格。");
				EDLog(1, 1, "板坯长度与宽度不匹配，不能判合格");
				isLock = 0;
				strcpy(s.msg, "板坯长度与宽度不匹配，不能判合格");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}

		EDLog(1, 1, "v_pch_judge_code = [%s]", tmmsm01.PCH_JUDGE_CODE);
		if (tmmsm01.PCH_JUDGE_CODE == "1")
		{
			EDLog(1, 1, "板坯低倍封锁未释放，不能判合格");
			//strcpy(s.msg, "板坯低倍封锁未释放，不能判合格");
			isLock = 0;
			strcpy(s.msg, "板坯低倍封锁未释放，不能判合格");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		blckNum = bcls_ret->AtBlkName("SQCK");
		if (-1 != blckNum)
		{
			bcls_ret->SetColVal(blckNum, 1, "isLock", isLock);
		}
		else
		{
			blckNum = bcls_ret->AddBlock();
			bcls_ret->SetBlkName(blckNum, "SQCK");
			bcls_ret->SetColName(blckNum, 1, "isLock");//合格/封锁
			bcls_ret->SetColVal(blckNum, 1, "isLock", isLock);
		}


	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "Database processing error. sqlcode=[{0}]." /* _RES("GCRSS0000006")*//*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		Log::Trace("", __FUNCTION__, "数据库SQL出错信息	= [{0}]", str);
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch (const CApplicationException& ex)
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

	return(doFlag);
}
