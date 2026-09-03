/*=========================================================================
//程序名称:		f_wmsm_slab_no
//隶属子系统:	WM
//产品名称:		炼钢二厂匹配订单号(命令板坯)
//创建人员:		lizhen
//创建时间:		2024-1-1

//-----------------------------------------------------------------------
//功能描述:	 炼钢二厂匹配订单号(命令板坯)
//条件描述：
//数据库表:
//
//主调用函数:
//
//-----------------------------------------------------------------------
//函数功能:     板坯质量判定校验
//传入参数:     制造命令号，长，宽，厚，钢种，传入命令板坯(?)	
//传出参数:     slab_no,no_slab_cause
//处理流程:
//=========================================================================*/


#include "stdafx.h"



BM2_FUNCTION_EXPORT
int f_wmsm_slab_no(CString pono, CDecimal mat_len, CDecimal mat_width, CDecimal mat_thick, CString st_no, CString mat_destion, CString &slab_no, vector<CString>& slab_no1, CString &no_slab_cause, CDbConnection* conn)
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

	
	CDecimal L = 0;		//板坯长度
	CDecimal W = 0;		//板坯宽度
	CDecimal Wb = 0;	//头宽
	CDecimal Wt = 0;	//尾宽
	//Decimal v_slab_tapper_width_lmt = 30;//宽差
	CString s_slab_no = " ";//预定板坯号
	CString A = " ";	//精整标记
	CString B = " ";	//改钢标记
	CString C = " ";	//改钢类型
	CString v_mat_no = " "; //板坯号
	//char v_pch_judge_code[1+1] = " ";//性能判定
	CString v_hardness_group = " ";//硬度
	CString v_st_no = " "; //出钢记号

	CModel tpssm01("TPSSM01");
	CModel tpssm11("TPSSM11");
	CModel tpssm03("TPSSM03");
	CModel tmmsm01("TMMSM01");

	CDbCommand cmd_inq(conn);


	CString sqlstr = "";

	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


	try
	{
		Log::Trace("", __FUNCTION__, "s_pono[{0}]v_mat_len[{1}]v_mat_width[{2}]v_mat_thick[{3}]s_st_no[{4}]",
			pono, mat_len, mat_width, mat_thick, st_no);
		if (pono.Trim() == "") {
			strcpy(s.msg, "制造命令号不可为空!");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (pono.SubstringNE(0,1) == "9") {
			strcpy(s.msg, "手创的制造命令，不能匹配命令板坯!");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (st_no.Trim() == "") {
			strcpy(s.msg, "出钢记号不可为空!");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		tpssm03["PONO"] = pono;
		tpssm03["SLAB_PROD_FLAG"] = "0";
		/*int slab_c = tpssm03.QueryCount("PONO,SLAB_PROD_FLAG");
		Log::Trace("", __FUNCTION__, "slab_c【{0}】", slab_c);
		if (slab_c <=0)
		{
			slab_no = " ";
			no_slab_cause = "该PONO"+pono+"下没有可使用的制造命令板坯号。";
			return 0;
		}*/
		EIClass temp;
		if (mat_destion == "41") {
			sqlstr = " SELECT max(LSLAB_NO)                                                      SLAB_NO,\
				max(PONO)                                                          PONO,\
				max(STRAND_NO)                                                     STRAND_NO,\
				max(SLAB_WIDTH)                                                    SLAB_WIDTH,\
				max(SLAB_THICK)                                                    SLAB_THICK,\
				max(LSLAB_NO_LENGTH)                                               SLAB_LEN,\
				max(ORDER_NO)                                                      ORDER_NO,\
				max(LSLAB_NO_LENGTH_MAX)                                           SLAB_MAX_LEN,\
				max(LSLAB_NO_LENGTH_MIN)                                           SLAB_MIN_LEN,\
				max(SLAB_DEST)                                                     SLAB_DEST,\
				(LISTAGG(DISTINCT(SLAB_NO), '/') WITHIN GROUP(ORDER BY SLAB_NO)) SLAB_NO1\
				FROM TPSSM03\
				WHERE LSLAB_NO != SLAB_NO\
				and PONO = '"+pono+"'\
				and substr2(PONO, 0, 1) != '9'\
				and SLAB_PROD_FLAG = '0'\
				group by LSLAB_NO ";
		}
		else
		{
			sqlstr = " select * from tpssm03 where  SLAB_PROD_FLAG='0' AND CAST_LOT_NO = (select CAST_LOT_NO from tpssm01 where PONO = '" + pono + "') and substr2(PONO, 0, 1) != '9' AND PONO IN (SELECT PONO FROM TPSSM01 WHERE  ST_NO='" + st_no + "') ";
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(temp.Tables[0]);
		cmd_inq.Close();
		if (temp.Tables[0].Rows.get_Count() <= 0)
		{
			slab_no = " ";
			no_slab_cause = "该PONO" + pono + "下没有可使用的制造命令板坯号。";
			return 0;
		}


		tpssm01["PONO"] = pono;
	
		if (!tpssm01.Query("PONO"))
		{
			slab_no = " ";
			no_slab_cause = "未找到该PONO" + pono + "";
			return 0;
		}
		else {
			if (tpssm01["PONO_STATUS"].ToString() == "91")
			{
			
				slab_no = " ";
				no_slab_cause = "该PONO"+pono+"已经炉次确定，不可进行命令板坯匹配。";
				return 0;
			}
			if (tpssm01["ST_NO"].ToString() != st_no)
			{
				slab_no = " ";
				no_slab_cause = "出钢记号不合，不可进行命令板坯匹配。";
				return 0;
			}
		}

		
		int s_c = 0;
		if (mat_destion == "41") {
			sqlstr = "  SELECT max(LSLAB_NO)                                                      SLAB_NO,\
				max(PONO)                                                          PONO,\
				max(STRAND_NO)                                                     STRAND_NO,\
				max(SLAB_WIDTH)                                                    SLAB_WIDTH,\
				max(SLAB_THICK)                                                    SLAB_THICK,\
				max(LSLAB_NO_LENGTH)                                               SLAB_LEN,\
				max(ORDER_NO)                                                      ORDER_NO,\
				max(LSLAB_NO_LENGTH_MAX)                                           SLAB_MAX_LEN,\
				max(LSLAB_NO_LENGTH_MIN)                                           SLAB_MIN_LEN,\
				max(SLAB_DEST)                                                     SLAB_DEST,\
				(LISTAGG(DISTINCT(SLAB_NO), '/') WITHIN GROUP(ORDER BY SLAB_NO)) LSLAB_NO\
				FROM TPSSM03\
				WHERE LSLAB_NO != SLAB_NO\
				and PONO = '" + pono + "'\
				and substr2(PONO, 0, 1) != '9'\
				and SLAB_PROD_FLAG = '0'\
				group by LSLAB_NO ";
		}
		else
		{
			sqlstr = " select * from tpssm03 where  SLAB_PROD_FLAG='0' AND CAST_LOT_NO = (select CAST_LOT_NO from tpssm01 where PONO = '" + pono + "') and substr2(PONO, 0, 1) != '9'  AND PONO IN (SELECT PONO FROM TPSSM01 WHERE  ST_NO='" + st_no + "') ";
		}
		Log::Trace("", __FUNCTION__, "sqlstr【{0}】", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			
			cmd_inq.Fetch(tpssm03);
			if (mat_len > tpssm03["SLAB_MAX_LEN"].ToDecimal()
				|| mat_len > tpssm03["SLAB_MAX_LEN"].ToDecimal()) {
				Log::Trace("", __FUNCTION__, "mat_len【{0}】tpssm03[SLAB_LEN]【{1}】", mat_len, tpssm03["SLAB_LEN"].ToDecimal());
				slab_no = " ";
				no_slab_cause = "铸坯长度不符。";
				continue;
			}
			if (mat_width != tpssm03["SLAB_WIDTH"].ToDecimal()) {
				slab_no = " ";
				no_slab_cause = "铸坯宽度不符。";
				continue;
			}
			if (mat_thick != tpssm03["SLAB_THICK"].ToDecimal()) {
				slab_no = " ";
				no_slab_cause = "铸坯厚度不符。";
				continue;
			}
			slab_no = tpssm03["SLAB_NO"].ToString();
			
			size_t pos = 0;
			CString token;
			std::string delimiter = "/";
			string LSLAB_NO = string(tpssm03["LSLAB_NO"]);

			while ((pos = LSLAB_NO.find(delimiter)) != std::string::npos) {
				token = LSLAB_NO.substr(0, pos);
				Log::Trace("", __FUNCTION__, "fddfgtyuijhbhjkolp【{0}】", token);
				slab_no1.push_back(token);
				LSLAB_NO.erase(0, pos + delimiter.length());
			}
			Log::Trace("", __FUNCTION__, "G KUEHRIGJWFIOEWGUHBRPEIWFP【{0}】", CString(LSLAB_NO));
			slab_no1.push_back(LSLAB_NO);
			/*for (CString value : slab_no1) {
				Log::Trace("", __FUNCTION__, "fddfgtyuijhbhjkolp【{0}】", value);
			}
			*/	//Log::Trace("", __FUNCTION__, "fddfgtyuijhbhjkolp【{0}】", (const char*)slab_no1);
				
			
			s_c++;
			if (slab_no.Trim() != "") {
				no_slab_cause = " ";
				break;
			}
			
		}
		if (s_c == 0) {
			slab_no = " ";
			no_slab_cause = "未找到合适的预定板坯信息。";
		}
		cmd_inq.Close();
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "Database processing error. sqlcode=[{0}]."   /*_RES("GCRSS0000006") *//*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
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

