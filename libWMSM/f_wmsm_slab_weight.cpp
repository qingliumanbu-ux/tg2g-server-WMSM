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
#include "math.h"
BM2_FUNCTION_IMPORT	


BM2_FUNCTION_EXPORT
int f_wmsm_slab_weight(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/*程序用变量*/
	int doFlag = 0;
	int ret = 0;
	int sqlid = 0;
	int blckNum = -1;

	int isLock = 1;//0为封锁，1为合格

	/* Pro*c 标准头文件部分  */

	/********表结构引用*********/
	CDecimal v_mat_act_width = 0.0;
	CDecimal v_mat_act_thick = 0.0;
	CDecimal v_mat_act_len = 0.0;
	CString v_strand_no = "";
	CDecimal v_slab_density = 0.0;
	CDecimal v_slab_wtadj_factor = 0.0;
	long v_mm_to_m_kg = 1000000000;//毫米转换米（体积 -- 吨）
	CString v_st_no = " ";
	CDecimal v_wtadj_factor = 0.0;
	CDecimal v_slab_cut_shk_factor = 0.0;
	CDecimal v_theory_wt = 0;
	CDecimal v_theory_wt_act = 0; //实际外发理论重量


	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);

	CString sqlstr = "";

	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


	try
	{
		EDLog(1, 1, "----------------- begin -----------------");
		bcls_rec->GetSYS(&s);
		//接收板坯位置信息

		v_mat_act_width = bcls_rec->Tables[0].Rows[0]["MAT_ACT_WIDTH"];
		v_mat_act_thick = bcls_rec->Tables[0].Rows[0]["MAT_ACT_THICK"];
		v_mat_act_len = bcls_rec->Tables[0].Rows[0]["MAT_ACT_LEN"];
		v_strand_no = bcls_rec->Tables[0].Rows[0]["STRAND_NO"].ToString();
		v_st_no = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString();
		
		/* ***** 打印输入参数 ***** */
		Log::Trace("", __FUNCTION__, "mat_act_width[{0}],v_mat_act_thick[{1}]", v_mat_act_width, v_mat_act_thick);
		Log::Trace("", __FUNCTION__, "v_strand_no[{0}]", v_strand_no);
		//EDLog(1, 1, "aaa mat_act_width = [%d], mat_act_len = [%d]", v_mat_act_width, v_mat_act_thick);
		sqlstr =
			" SELECT SLAB_DENSITY, SLAB_WTADJ_FACTOR, SLAB_CUT_SHK_FACTOR FROM TQMTS9CC WHERE STRAND_NO = @v_strand_no  "
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("v_strand_no", v_strand_no);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			v_slab_density=cmd_inq.GetDecimal(1);
			v_slab_wtadj_factor = cmd_inq.GetDecimal(2);
			v_slab_cut_shk_factor = cmd_inq.GetDecimal(3);
		}
		cmd_inq.Close();

		Log::Trace("", __FUNCTION__, "v_slab_density[{0}],v_slab_wtadj_factor[{1}],v_slab_cut_shk_factor[{2}]", v_slab_density, v_slab_wtadj_factor, v_slab_cut_shk_factor);
		//获取板坯重量修正系数
		v_wtadj_factor = 1;

		/*sqlstr =
			" SELECT DECODE(SLAB_WTADJ_FACTOR, 0, 1, SLAB_WTADJ_FACTOR) FROM TQMTS08 WHERE ST_NO = @v_st_no  "
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("v_st_no", v_st_no);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			v_wtadj_factor = cmd_inq.GetDecimal(1);
		}
		cmd_inq.Close();*/

			;
		//20220531 实际理论重量
		v_theory_wt_act = 0;
		v_theory_wt = 0;
		//v_theory_wt = floor((v_mat_act_width * v_mat_act_thick * v_mat_act_len * v_slab_density * v_slab_wtadj_factor * v_wtadj_factor)/v_mm_to_m_kg + 0.5);//（四舍五入）
		v_theory_wt_act = ((v_mat_act_width * v_mat_act_thick * v_mat_act_len * v_slab_density * v_slab_wtadj_factor * v_wtadj_factor) / v_mm_to_m_kg ).Round(3);//（四舍五入）
		v_theory_wt = ((v_mat_act_width * v_mat_act_thick * v_mat_act_len * v_slab_density * v_slab_wtadj_factor * v_wtadj_factor) / v_mm_to_m_kg ).Round(3);//（四舍五入）
		
		bcls_ret->Tables[0].set_TableName("CSWT");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "THEORY_WT");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "THEORY_WT_ACT");
		bcls_ret->Tables[0].Rows.Add();
		bcls_ret->Tables[0].Rows[0]["THEORY_WT"] = v_theory_wt;
		bcls_ret->Tables[0].Rows[0]["THEORY_WT_ACT"] = v_theory_wt_act;
		
		Log::Trace("", __FUNCTION__, "theory_wt[{0}]", v_theory_wt);
		Log::Trace("", __FUNCTION__, "theory_wt_act[{0}]", v_theory_wt_act);
	


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
