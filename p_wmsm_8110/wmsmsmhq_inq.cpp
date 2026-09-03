/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         JHZHAO
Version:		1.0
Date:			2016-03-05
Description:	xxx
**************************************************/

//框架头文件
#include "stdafx.h"

BM2F_ENTERACE(wmsmsmhq_inq);
int f_wmsmsmhq_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); //专门用来打log

	/* 程序内部变量 */
	int doFlag = 0;
	

	/* 实体类定义 */
	CModel twmhq01 = CModel("TWMHQ01");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	//系统的分页类信息。
	CPageInfo pageInfo;

	try
	{
		Log::Trace("", __FUNCTION__, "开始");
		CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss"); //获取当前系统的时间
		if (bcls_rec->Tables.Contains("ADD")) //传入块中的tables中是否包含 ADD
		{
			Log::Trace("", __FUNCTION__, "包含ADD");
			for (int i = 0; i < bcls_rec->Tables["ADD"].Rows.get_Count(); i++) //传入块的tables中的ADD数据的行数
			{
				twmhq01.Reset(); //重置CModel，列名还在，只是清除数据
				twmhq01.MergeFrom(bcls_rec->Tables["ADD"].Rows[i]); //把传入块的ADD表的所有行的数据给twmhq01数据块
				Log::Trace("", __FUNCTION__, "库区号[{0}]", twmhq01["STOCK_NO"].ToString());

				Log::Trace("", __FUNCTION__, "库区号长度[{0}]", twmhq01["STOCK_NO"].ToString().Trim().GetLength());
				if (twmhq01["STOCK_NO"].ToString().Trim().GetLength() >= 4)  //判断CModel中的STOCK_NO这列的数据的长度
				{
					sprintf(s.msg, "库区号:"+ twmhq01["STOCK_NO"].ToString().Trim() +"超长，无法新增");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (twmhq01["STOCK_NO"].ToString().Trim() == "") 
				{
					sprintf(s.msg, "库区号不能为空，请重新输入");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (twmhq01.QueryCount("STOCK_NO") != 0) //执行SQL中的查找count语句，来判断数据是否已存在
				{
					sprintf(s.msg, "库区号已存在，不能再新增");
					throw CApplicationException(-1, s.msg, log.Location);
				}

				Log::Trace("", __FUNCTION__, "4");
				twmhq01["REC_CREATE_TIME"] = datetime;
				Log::Trace("", __FUNCTION__, "5");
				twmhq01["REC_CREATOR"] = s.userid;
				Log::Trace("", __FUNCTION__, "1");
				//twmhq01.Print();
				Log::Trace("", __FUNCTION__, "2");
				twmhq01.Insert();
				//sqlstr = " insert into twmhq01 values('" + twmhq01["REC_CREATOR"] + "','" + twmhq01["REC_CREATE_TIME"] + "','','','','','')";
			}
			Log::Trace("", __FUNCTION__, "结束");
		}
		if (bcls_rec->Tables.Contains("MOD"))
		{
			Log::Trace("", __FUNCTION__, "包含MOD");
			/*CString stock_no = "";
			CString stock_desc = "";
			CString stock_no_class = "";*/

			for (int i = 0; i<bcls_rec->Tables["MOD"].Rows.get_Count(); i++)
			{
				//if (bcls_rec->Tables["MOD"].Columns.Contains("STOCK_NO")) //bcls_rec传入块的第0张表 是否 包含第0张表的STOCK_NO
				//{
				//	stock_no = bcls_rec->Tables["MOD"].Rows[i]["STOCK_NO"].ToString().Trim();
				//}
				//if (bcls_rec->Tables["MOD"].Columns.Contains("STOCK_DESC")) //bcls_rec传入块的第0张表 是否 包含第0张表的STOCK_NO
				//{
				//	stock_desc = bcls_rec->Tables["MOD"].Rows[i]["STOCK_DESC"].ToString().Trim();
				//	Log::Trace("", __FUNCTION__, "[{1}]  stock_desc[{0}]", stock_desc, __LINE__);
				//}
				//if (bcls_rec->Tables["MOD"].Columns.Contains("STOCK_NO_CLASS")) //bcls_rec传入块的第0张表 是否 包含第0张表的STOCK_NO
				//{
				//	stock_no_class = bcls_rec->Tables["MOD"].Rows[i]["STOCK_NO_CLASS"].ToString().Trim();
				//	Log::Trace("", __FUNCTION__, "[{1}]  stock_no_class[{0}]", stock_no_class, __LINE__);
				//}
				//Log::Trace("", __FUNCTION__, "stock_no[{0}]", stock_no);
				//Log::Trace("", __FUNCTION__, "stock_desc[{0}]", stock_desc);
				//Log::Trace("", __FUNCTION__, "stock_no_class[{0}]", stock_no_class);

				//sqlstr = " update twmhq01 set stock_desc    =  '" + stock_desc + "',stock_no_class = '" + stock_no_class + "' where stock_no = '" + stock_no + "'";
				//Log::Trace("", __FUNCTION__, "sqlstr[{0}]", sqlstr);
				//cmd_inq.SetCommandText(sqlstr);
				//cmd_inq.ExecuteNonQuery(); //直接执行数据库，不要返回值
				//cmd_inq.Close();

				twmhq01.Reset();
				twmhq01.MergeFrom(bcls_rec->Tables["MOD"].Rows[i]);
				Log::Trace("", __FUNCTION__, "库区号[{0}]", twmhq01["STOCK_NO"].ToString());

				twmhq01["REC_REVISE_TIME"] = datetime;
				twmhq01["REC_REVISOR"] = s.userid; //系统给的值
				twmhq01.Print();
				CString upd = "REC_REVISOR,STOCK_DESC,ARCHIVE_FLAG";

				twmhq01.Update(upd,"STOCK_NO"); //第一个参数：所有要修改的列。第二个参数：where后面的查询条件

				//sqlstr = " update twmhq01 set stock_desc    =  '" + twmhq01["STOCK_DESC"] + "',stock_no_class = '" + twmhq01["STOCK_NO_CLASS"] + "' where stock_no = '" + twmhq01["STOCK_NO"] + "'"
				//cmd_inq.SetCommandText(sqlstr);
				//cmd_inq.ExecuteNonQuery(); //直接执行数据库，不要返回值
				//cmd_inq.Close();
			}

		}

		if (bcls_rec->Tables.Contains("DEL"))
		{
			Log::Trace("", __FUNCTION__, "包含DEL");
			Log::Trace("", __FUNCTION__, "有[{0}]条数据", bcls_rec->Tables["DEL"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["DEL"].Rows.get_Count(); i++)
			{
				/*if (bcls_rec->Tables["DEL"].Columns.Contains("STOCK_NO"))
				{
					
				}*/
				//stock_no = bcls_rec->Tables["DEL"].Rows[i]["STOCK_NO"].ToString().Trim();
				//Log::Trace("", __FUNCTION__, "stock_no[{0}]", stock_no);
				//sqlstr = "delet twmhq01 where stock_no='" + stock_no + "'";
				//cmd_inq.SetCommandText(sqlstr);
				//cmd_inq.ExecuteNonQuery(); //直接执行数据库，不要返回值
				//cmd_inq.Close();

				twmhq01.Reset();
				twmhq01["STOCK_NO"] = bcls_rec->Tables["DEL"].Rows[i]["STOCK_NO"].ToString().Trim();
				twmhq01["STOCK_DESC"] = bcls_rec->Tables["DEL"].Rows[i]["STOCK_DESC"].ToString().Trim();
				Log::Trace("", __FUNCTION__, "stock_no[{0}]", twmhq01["STOCK_NO"]);
				Log::Trace("", __FUNCTION__, "STOCK_DESC[{0}]", twmhq01["STOCK_DESC"]);
				twmhq01.Delete("STOCK_NO");
				//delete twmhq01 where stock_no = 'S11'
			}
		}
		
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };

		/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006"), arguments, 1);
		CString str = ex.GetMsg() + "\r\n" + sqlstr;

		/*返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应*/
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);

		/*数据库异常时返回-1，事务将被回滚*/
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)  //捕获应用错误
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
	return doFlag;

}

