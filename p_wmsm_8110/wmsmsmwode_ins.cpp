/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         JHZHAO
Version:		1.0
Date:			2016-03-05
Description:	xxx
**************************************************/

//框架头文件
#include "stdafx.h"

BM2F_ENTERACE(wmsmsmwode_ins);
int f_wmsmsmwode_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	

	/* 实体类定义 */
	CModel twmhq02 = CModel("TWMHQ02");

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
		CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		if (bcls_rec->Tables.Contains("ADD"))
		{
			Log::Trace("", __FUNCTION__, "包含ADD");
			for (int i = 0;i< bcls_rec->Tables["ADD"].Rows.get_Count(); i++)
			{
				twmhq02.Reset();
				twmhq02.MergeFrom(bcls_rec->Tables["ADD"].Rows[i]);
				Log::Trace("", __FUNCTION__, "库区号[{0}]", twmhq02["STOCK_NO"].ToString());
				Log::Trace("", __FUNCTION__, "库区号[{0}]的长度", twmhq02["STOCK_NO"].ToString().Trim().GetLength());
				if (twmhq02["STOCK_NO"].ToString().Trim().GetLength() >= 4)  //判断CModel中的STOCK_NO这列的数据的长度
				{
					sprintf(s.msg, "库区号:" + twmhq02["STOCK_NO"].ToString().Trim() + "超长，无法新增");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (twmhq02["STOCK_NO"].ToString().Trim() == "")
				{
					sprintf(s.msg, "库区号不能为空，请重新输入");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (twmhq02.QueryCount("STOCK_NO") != 0) //执行SQL中的查找count语句，来判断数据是否已存在
				{
					sprintf(s.msg, "库区号已存在，不能再新增");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				Log::Trace("", __FUNCTION__, "第[{0}]行",__LINE__);
				twmhq02["REC_CREATE_TIME"] = datetime;
				Log::Trace("", __FUNCTION__, "第[{0}]行", __LINE__);
				twmhq02["REC_CREATOR"] = s.userid;
				Log::Trace("", __FUNCTION__, "第[{0}]行", __LINE__);
				twmhq02.Insert();
			}
			Log::Trace("", __FUNCTION__, "第[{0}]行", __LINE__);

		}

		if (bcls_rec->Tables.Contains("MOD"))
		{
			Log::Trace("", __FUNCTION__, "包含MOD");
			for (int i = 0; i < bcls_rec->Tables["MOD"].Rows.get_Count(); i++)
			{
				twmhq02.Reset();
				twmhq02.MergeFrom(bcls_rec->Tables["MOD"].Rows[i]);
				Log::Trace("", __FUNCTION__, "库区号[{0}]", twmhq02["STOCK_NO"].ToString());
				twmhq02["REC_REVISE_TIME"] = datetime;
				
				twmhq02["REC_REVISOR"] = s.userid; //系统给的值
				CString upd = "STOCK_NO_CLASS,MAT_NO,FACTORY_DIV,STOCK_DESC";
				twmhq02.Update(upd, "STOCK_NO");

			}
			Log::Trace("", __FUNCTION__, "第[{0}]行", __LINE__);
		}

		if (bcls_rec->Tables.Contains("DEL"))
		{
			Log::Trace("", __FUNCTION__, "包含DEL");
			Log::Trace("", __FUNCTION__, "有[{0}]条数据", bcls_rec->Tables["DEL"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["DEL"].Rows.get_Count(); i++)
			{
				twmhq02.Reset();
				twmhq02["STOCK_NO"] = bcls_rec->Tables["DEL"].Rows[i]["STOCK_NO"].ToString().Trim();
				twmhq02["STOCK_DESC"] = bcls_rec->Tables["DEL"].Rows[i]["STOCK_DESC"].ToString().Trim();
				Log::Trace("", __FUNCTION__, "stock_no[{0}]", twmhq02["STOCK_NO"]);
				Log::Trace("", __FUNCTION__, "stock_desc[{0}]", twmhq02["STOCK_DESC"]);
				twmhq02.Delete("STOCK_NO,STOCK_DESC");

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

