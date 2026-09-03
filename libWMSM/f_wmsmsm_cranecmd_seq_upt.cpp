/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:      jinquan
Version:     1.1.1
Date:        2017-3-16
Description: 更新命令流水号
**************************************************/

//框架头文件
#include "stdafx.h"

//int f_wmsmsm_u1dl03_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//发送命令

BM2_FUNCTION_EXPORT
int f_wmsmsm_cranecmd_seq_upt(CString stock_oper_order , EIClass * bcls_ret, CDbConnection * conn)
{
	/*定义表实体对象*/
	CModel twma7 = CModel("CTWMA7");

	/*程序内部变量*/
	int doFlag = 0;
	CString sqlstr = " ";
	CString seq_name = " ";

	CDataTable cmd_update;

	EIClass bcls_rec_send;
	bcls_rec_send.Tables[0].set_TableName("U1DL03");
	bcls_rec_send.Tables[0].Columns.Add(DT_STRING, "mat_no");
	bcls_rec_send.Tables[0].Columns.Add(DT_STRING, "origin_mat_no");
	bcls_rec_send.Tables[0].Columns.Add(DT_STRING, "stock_oper_order");
	bcls_rec_send.Tables[0].Columns.Add(DT_DECIMAL, "cmd_seq");
	bcls_rec_send.Tables[0].Columns.Add(DT_DECIMAL, "crane_cmdgrpno");
	bcls_rec_send.Tables[0].Columns.Add(DT_DECIMAL, "batch_no");
	bcls_rec_send.Tables[0].Columns.Add(DT_STRING, "stock_place_no_from");
	bcls_rec_send.Tables[0].Columns.Add(DT_DECIMAL, "yard_layer_from");
	bcls_rec_send.Tables[0].Columns.Add(DT_STRING, "stock_place_no_to");
	bcls_rec_send.Tables[0].Columns.Add(DT_STRING, "stock_oper_order_fin");
	bcls_rec_send.Tables[0].Columns.Add(DT_STRING, "remark");

	try
	{
		//项目自定义日志
		CTracer log(__FUNCTION__);
		seq_name = "SEQ_" + stock_oper_order.Trim();
		Log::Trace("", __FUNCTION__, "seq_name={0}", seq_name);

		sqlstr = "DROP SEQUENCE "+seq_name;
		Db::Execute(sqlstr);
		if (stock_oper_order.Trim().Substring(0, 1) == "1"
			|| stock_oper_order.Trim().Substring(0, 1) == "2"
			|| stock_oper_order.Trim().Substring(0, 1) == "3")
		{
			if (stock_oper_order.Trim() == "30")
			{
				sqlstr = "CREATE SEQUENCE  SEQ_30 AS INT START WITH 100000 INCREMENT BY 1 MINVALUE 100000   MAXVALUE 199999  CYCLE NO  CACHE ORDER";				
			}
			else if (stock_oper_order.Trim() == "1C")
			{
				sqlstr = "CREATE SEQUENCE  SEQ_1C AS INT START WITH 200000 INCREMENT BY 1 MINVALUE 200000   MAXVALUE 299999  CYCLE NO  CACHE ORDER";
			}
			else if (stock_oper_order.Trim() == "2B")
			{
				sqlstr = "CREATE SEQUENCE  SEQ_2B AS INT START WITH 300000 INCREMENT BY 1 MINVALUE 300000   MAXVALUE 399999  CYCLE NO  CACHE ORDER";
			}
			else if (stock_oper_order.Trim() == "2C")
			{
				sqlstr = "CREATE SEQUENCE  SEQ_2C AS INT START WITH 400000 INCREMENT BY 1 MINVALUE 400000   MAXVALUE 499999  CYCLE NO  CACHE ORDER";
			}
			else if (stock_oper_order.Trim() == "1A")
			{
				sqlstr = "CREATE SEQUENCE  SEQ_1A AS INT START WITH 500000 INCREMENT BY 1 MINVALUE 500000   MAXVALUE 599999  CYCLE NO  CACHE ORDER";
			}
			else if (stock_oper_order.Trim() == "1B")
			{
				sqlstr = "CREATE SEQUENCE  SEQ_1B AS INT START WITH 600000 INCREMENT BY 1 MINVALUE 600000   MAXVALUE 699999  CYCLE NO  CACHE ORDER";
			}
			else if (stock_oper_order.Trim() == "1G")
			{
				sqlstr = "CREATE SEQUENCE  SEQ_1G AS INT START WITH 700000 INCREMENT BY 1 MINVALUE 700000   MAXVALUE 799999  CYCLE NO  CACHE ORDER";
			}
			else if (stock_oper_order.Trim() == "2A")
			{
				sqlstr = "CREATE SEQUENCE  SEQ_2A AS INT START WITH 800000 INCREMENT BY 1 MINVALUE 800000   MAXVALUE 899999  CYCLE NO  CACHE ORDER";
			}
			else if (stock_oper_order.Trim() == "2E")
			{
				sqlstr = "CREATE SEQUENCE  SEQ_2E AS INT START WITH 900000 INCREMENT BY 1 MINVALUE 900000   MAXVALUE 999999  CYCLE NO  CACHE ORDER";
			}
			else if (stock_oper_order.Trim() == "2G")
			{
				sqlstr = "CREATE SEQUENCE  SEQ_2G AS INT START WITH 1000000 INCREMENT BY 1 MINVALUE 1000000   MAXVALUE 1099999  CYCLE NO  CACHE ORDER";
			}
			else if (stock_oper_order.Trim() == "31")
			{
				sqlstr = "CREATE SEQUENCE  SEQ_31 AS INT START WITH 1100000 INCREMENT BY 1 MINVALUE 1100000   MAXVALUE 1199999  CYCLE NO  CACHE ORDER";
			}
			else if (stock_oper_order.Trim() == "32")
			{
				sqlstr = "CREATE SEQUENCE  SEQ_32 AS INT START WITH 1200000 INCREMENT BY 1 MINVALUE 1200000   MAXVALUE 1299999  CYCLE NO  CACHE ORDER";
			}
			else if (stock_oper_order.Trim() == "1D")
			{
				sqlstr = "CREATE SEQUENCE  SEQ_1D AS INT START WITH 1300000 INCREMENT BY 1 MINVALUE 1300000   MAXVALUE 1399999  CYCLE NO  CACHE ORDER";
			}
			else if (stock_oper_order.Trim() == "1Z")
			{
				sqlstr = "CREATE SEQUENCE  SEQ_1Z AS INT START WITH 1400000 INCREMENT BY 1 MINVALUE 1400000   MAXVALUE 1499999  CYCLE NO  CACHE ORDER";
			}
			else
			{
				return doFlag;
			}
			Db::Execute(sqlstr);

			sqlstr = "select * from twma7 where stock_oper_order='" + stock_oper_order + "'  order by cmd_seq,yard_layer_from";
			Db::QueryTable(sqlstr, cmd_update);

			for (int i = 0; i < cmd_update.Rows.get_Count(); i++)
			{
				twma7.Reset();
				twma7["MAT_NO"] = cmd_update.Rows[i]["MAT_NO"];
				sqlstr = "values nextval for " + seq_name;
				//twma7["CMD_SEQ"] = Db::QueryCDecimal(sqlstr);
				twma7["CMD_SEQ"] = Db::QueryCDecimal(sqlstr);
				twma7.Update("CMD_SEQ", "MAT_NO");

				bcls_rec_send.Tables[0].Rows.Add();
				bcls_rec_send.Tables[0].Rows[i]["mat_no"] = twma7["MAT_NO"].ToString();
				bcls_rec_send.Tables[0].Rows[i]["stock_oper_order"] = twma7["STOCK_OPER_ORDER"].ToString();
				bcls_rec_send.Tables[0].Rows[i]["cmd_seq"] = twma7["CMD_SEQ"].ToDecimal();
				bcls_rec_send.Tables[0].Rows[i]["crane_cmdgrpno"] = twma7["CRANE_CMDGRPNO"].ToDecimal();
				bcls_rec_send.Tables[0].Rows[i]["batch_no"] = twma7["BATCH_TASK_NO"].ToDecimal();
				bcls_rec_send.Tables[0].Rows[i]["stock_place_no_from"] = twma7["STOCK_PLACE_NO_FROM"].ToString();
				bcls_rec_send.Tables[0].Rows[i]["yard_layer_from"] = twma7["YARD_LAYER_FROM"].ToString();
				bcls_rec_send.Tables[0].Rows[i]["stock_place_no_to"] = twma7["STOCK_PLACE_NO_TO"].ToString();
				bcls_rec_send.Tables[0].Rows[i]["stock_oper_order_fin"] = twma7["STOCK_OPER_ORDER_FIN"].ToString();
				bcls_rec_send.Tables[0].Rows[i]["remark"] = "seq_upt";
			}
			//发送命令
			/*doFlag = f_wmsmsm_u1dl03_snd(&bcls_rec_send, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}*/
		}
		else
		{
			if (stock_oper_order == "BATCH")
			{
				sqlstr = "CREATE SEQUENCE  SEQ_BATCH AS INT START WITH 1000001 INCREMENT BY 1 MINVALUE 1000001   MAXVALUE 9999999  CYCLE NO  CACHE ORDER";
				Db::Execute(sqlstr);
			}
			else if (stock_oper_order == "BATCH_INS")
			{
				sqlstr = "CREATE SEQUENCE  SEQ_BATCH_INS AS INT START WITH 1 INCREMENT BY 1 MINVALUE 1   MAXVALUE 10000  CYCLE NO  CACHE ORDER";
				Db::Execute(sqlstr);
			}
			else if (stock_oper_order == "BATCH_2C")
			{
				sqlstr = "CREATE SEQUENCE  SEQ_BATCH_2C AS INT START WITH 10001 INCREMENT BY 1 MINVALUE 10001   MAXVALUE 1000000  CYCLE NO  CACHE ORDER";
				Db::Execute(sqlstr);
			}
			else if (stock_oper_order == "BATCH_GROUP")
			{
				sqlstr = "CREATE SEQUENCE  SEQ_BATCH_GROUP AS INT START WITH 1 INCREMENT BY 1 MINVALUE 1   MAXVALUE 9999999  CYCLE NO  CACHE ORDER";
				Db::Execute(sqlstr);
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