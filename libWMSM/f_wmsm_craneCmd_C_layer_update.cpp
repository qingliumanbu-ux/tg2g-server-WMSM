/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:      jinquan
Version:     1.1.1
Date:        2016-12-20
Description: 库位层号更新刷新命令
**************************************************/

/* C/C++ 的标准头文件部分 */
#include "WM_Utility.h"	// 框架头，不可删除 
//#include "twma2.h"
//#include "twma1.h"
//#include "twm04.h"
//#include "twma7.h"

BM2_FUNCTION_IMPORT
int f_wmsm_CraneCmd_C_Make(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);	     //行车命令形成	

BM2_FUNCTION_EXPORT
int f_wmsm_craneCmd_C_layer_update(CString mat_no, EIClass * bcls_ret, CDbConnection * conn)
{
	/*程序内部变量*/
	int doFlag = 0;
	CString sqlstr = " ";
	CString dateTime = " ";
	CString left_cmd_flag = "0";
	CString right_cmd_flag = "0";
	CDecimal cmd_seq1 = 0;
	CDecimal cmd_seq2 = 0;

	/* 实体类定义 */
	//CTWM04 twm04(conn);
	//CTWM04 twm04_left(conn);
	//CTWM04 twm04_right(conn);
	//CTWMA2 twma2(conn);
	//CTWMA2 twma2_left(conn);
	//CTWMA2 twma2_right(conn);
	//CTWMA7 twma7_left(conn);
	//CTWMA7 twma7_right(conn);
	//CTWMA7 twma7(conn);

	CModel twm04 = CModel("TWM04");
	CModel twm04_left = CModel("TWM04");
	CModel twm04_right = CModel("TWM04");
	CModel twma2 = CModel("TWMA2");
	CModel twma2_left = CModel("TWMA2");
	CModel twma2_right = CModel("TWMA2");
	CModel twma7_left = CModel("TWMA7");
	CModel twma7_right = CModel("TWMA7");
	CModel twma7 = CModel("TWMA7");

	/*数据库操作类定义*/
	CDbCommand cmd_inq(conn);

	CDataTable cmd_seq;
	CDataTable cmd_seq_1;
	CDataTable cmd_seq_2;
	CDataTable cmd_seq_3;

	try
	{
		//项目自定义日志
		CTracer log(__FUNCTION__);

		//设置行车命令生成函数传入块
		EIClass bcls_rec_make;
		bcls_rec_make.Tables[0].set_TableName("WM00_CMD");
		bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
		bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "STOCK_NO_FROM");
		bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO_FROM");
		bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "STOCK_NO_TO");
		bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO_TO");
		bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");
		bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "STOCK_OPER_ORDER_FIN");
		bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "CMD_METHOD");
		bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "YARD_LAYER_FROM");
		bcls_rec_make.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO_FIN");

		Log::Trace("", __FUNCTION__, "mat_no={0}", mat_no);

		twma2["MAT_NO"] = mat_no;
		Log::Trace("", __FUNCTION__, "twma2.MAT_NO={0}", twma2["MAT_NO"].ToString());
		if (!twma2.Query("MAT_NO"))
		{
			strcpy(s.msg, "Incoming Material No is not exist!");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		twm04["STOCK_PLACE_NO"] = twma2["STOCK_PLACE_NO"].ToString();
		if (!twm04.Query("STOCK_PLACE_NO"))
		{
			sprintf(s.msg, "Current position No. is not exist", twm04["STOCK_PLACE_NO"].ToString());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		
		if (twm04["LAYERNO"].ToDecimal() == 1)
		{
			Log::Trace("", __FUNCTION__, " 库位{0}在第一层，无需判断", twm04["STOCK_PLACE_NO"].ToString());
			return doFlag;
		}
		

		twm04_left["STOCK_COL_NO"] = twm04["STOCK_COL_NO"].ToDecimal();
		twm04_left["STOCK_ROW_NO"] = twm04["STOCK_ROW_NO"].ToDecimal();
		twm04_left["STOCK_NO"] = twm04["STOCK_NO"].ToString();
		twm04_left["LAYERNO"] = 1;
		twm04_left.Query("STOCK_COL_NO,STOCK_ROW_NO,LAYERNO,STOCK_NO");
		twm04_left.TrimOrBlank();
		twma2_left["STOCK_PLACE_NO"] = twm04_left["STOCK_PLACE_NO"].ToString();
		if (!twma2_left.Query("STOCK_PLACE_NO"))
		{
			strcpy(s.msg, "库位左下没有材料");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		twma7_left["MAT_NO"] = twma2_left["MAT_NO"].ToString();
		if (twma7_left.Query("MAT_NO"))
		{
			Log::Trace("", __FUNCTION__, " 左下材料有命令");
			left_cmd_flag = "1";
		}
		else
		{
			Log::Trace("", __FUNCTION__, " 左下材料无命令");
		}

		twm04_right["STOCK_COL_NO"] = twm04["STOCK_COL_NO"].ToDecimal() + 1;
		twm04_right["STOCK_ROW_NO"] = twm04["STOCK_ROW_NO"].ToDecimal();
		twm04_right["STOCK_NO"] = twm04["STOCK_NO"].ToString();
		twm04_right["LAYERNO"] = 1;
		twm04_right.Query("STOCK_COL_NO,STOCK_ROW_NO,LAYERNO,STOCK_NO");
		twm04_right.TrimOrBlank();

		twma2_right["STOCK_PLACE_NO"] = twm04_right["STOCK_PLACE_NO"].ToString();
		if (!twma2_right.Query("STOCK_PLACE_NO"))
		{
			sprintf(s.msg, "There is not material in position's lower right");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		twma7_right["MAT_NO"] = twma2_right["MAT_NO"].ToString();
		if (twma7_right.Query("MAT_NO"))
		{
			Log::Trace("", __FUNCTION__, " 右下材料有命令");
			right_cmd_flag = "1";
		}
		else
		{
			Log::Trace("", __FUNCTION__, " 右下材料无命令");
		}
		
		if (right_cmd_flag == "1" || left_cmd_flag == "1")
		{
			twma7["MAT_NO"] = twma2["MAT_NO"].ToString();
			if (!twma7.Query("MAT_NO"))
			{
				Log::Trace("", __FUNCTION__, "材料无命令下层有命令，生成倒跺命令");
				bcls_rec_make.Tables[0].Rows.Add();
				bcls_rec_make.Tables[0].Rows[0]["MAT_NO"] = twma2["MAT_NO"].ToString();
				bcls_rec_make.Tables[0].Rows[0]["STOCK_NO_FROM"] = twma2["STOCK_NO"].ToString();
				bcls_rec_make.Tables[0].Rows[0]["STOCK_PLACE_NO_FROM"] = twma2["STOCK_PLACE_NO"].ToString();
				bcls_rec_make.Tables[0].Rows[0]["STOCK_NO_TO"] = twma2["MAT_NO"].ToString();
				bcls_rec_make.Tables[0].Rows[0]["STOCK_PLACE_NO_TO"] = " ";
				bcls_rec_make.Tables[0].Rows[0]["STOCK_OPER_ORDER"] = "31";
				bcls_rec_make.Tables[0].Rows[0]["YARD_LAYER_FROM"] = twma2["LAYERNO"].ToDecimal();
				doFlag = f_wmsm_CraneCmd_C_Make(&bcls_rec_make, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			twma7.Query("MAT_NO");
			Log::Trace("", __FUNCTION__, "{0},{1}", twma7["MAT_NO"].ToString(), twma7["CMD_SEQ"].ToString());
			Log::Trace("", __FUNCTION__, "{0},{1}", twma7_left["MAT_NO"].ToString(), twma7_left["CMD_SEQ"].ToString());
			Log::Trace("", __FUNCTION__, "{0},{1}", twma7_right["MAT_NO"].ToString(), twma7_right["CMD_SEQ"].ToString());
			if (twma7["CMD_SEQ"].ToDecimal() > twma7_right["CMD_SEQ"].ToDecimal())
			{
				cmd_seq1 = twma7["CMD_SEQ"].ToDecimal();
				twma7["CMD_SEQ"].ToDecimal() = twma7_right["CMD_SEQ"].ToDecimal();
				twma7_right["CMD_SEQ"].ToDecimal() = cmd_seq1;
				twma7.Update("CMD_SEQ","MAT_NO");
				twma7_right.Update("CMD_SEQ", "MAT_NO");
			}
			if (twma7["CMD_SEQ"].ToDecimal() > twma7_left["CMD_SEQ"].ToDecimal())
			{
				cmd_seq1 = twma7["CMD_SEQ"].ToDecimal();
				twma7["CMD_SEQ"].ToDecimal() = twma7_left["CMD_SEQ"].ToDecimal();
				twma7_left["CMD_SEQ"].ToDecimal() = cmd_seq1;
				twma7.Update("CMD_SEQ", "MAT_NO");
				twma7_left.Update("CMD_SEQ", "MAT_NO");
			}

		}
		else
		{
			Log::Trace("", __FUNCTION__, "材料下层无命令");
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
		Log::Trace("", __FUNCTION__, "11111111");
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
