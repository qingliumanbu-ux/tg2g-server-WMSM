/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      KE2111
Version:     1.0
Date:        2023-09-07
Description: 钢铁罐图形化初始化
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(wmsm01s2n_contrast)


int f_wmsm01s2n_contrast(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */


	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlstr1 = "";
	CString sqlwhere = "";
	CString s_userid("");

	CString ladle_no("");
	CString new_place("");
	CModel tmmsm01("TMMSM01");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_con(conn);
	CDataTable temp_table;

	//系统的分页类信息。
	CPageInfo pageInfo;

	try
	{

		Log::Trace(" ", " ", "jin");
		Log::Trace(" ", " ", " ", bcls_rec->Tables.get_Count());
		Log::Trace(" ", " ", " ", bcls_rec->Tables[1].Rows.get_Count());
		/*bcls_ret->Tables[0].Clone(bcls_rec->Tables[1]);
		bcls_ret->Tables[0].Clear();*/
		temp_table.Copy(bcls_rec->Tables[0]);
		temp_table.Columns.Add(DT_STRING, "SPARE_ITEM_0");
		if(!temp_table.Columns.Contains("HEAT_NO"))
			temp_table.Columns.Add(DT_STRING, "HEAT_NO");
		if (!temp_table.Columns.Contains("PONO"))
			temp_table.Columns.Add(DT_STRING, "PONO");
		if (!temp_table.Columns.Contains("MAT_NUM"))
			temp_table.Columns.Add(DT_DECIMAL, "MAT_NUM");
		if (!temp_table.Columns.Contains("MAT_THICK"))
			temp_table.Columns.Add(DT_DECIMAL, "MAT_THICK");
		if (!temp_table.Columns.Contains("MAT_WIDTH"))
			temp_table.Columns.Add(DT_DECIMAL, "MAT_WIDTH");
		if (!temp_table.Columns.Contains("MAT_LEN"))
			temp_table.Columns.Add(DT_DECIMAL, "MAT_LEN");
		Log::Trace(" ", " ", " ", bcls_rec->Tables[0].Rows.get_Count());
		Log::Trace(" ", " ", " ", bcls_rec->Tables[0].Columns.get_Count());
		Log::Trace(" ", " ", " ", temp_table.Rows.get_Count());
		Log::Trace(" ", " ", " ", temp_table.Columns.get_Count());
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			temp_table.Rows[i]["SPARE_ITEM_0"] = "1";//实物独有
			for (int j = 0; j < bcls_rec->Tables[1].Rows.get_Count(); j++)
			{
				if (temp_table.Rows[i]["MAT_NO"].ToString() == bcls_rec->Tables[1].Rows[j]["MAT_NO"].ToString())
				{
					if (temp_table.Rows[i]["STOCK_NO"] == bcls_rec->Tables[1].Rows[j]["STOCK_NO"]
						&& temp_table.Rows[i]["HEAT_NO"] == bcls_rec->Tables[1].Rows[j]["HEAT_NO"]
						&& temp_table.Rows[i]["ST_NO"] == bcls_rec->Tables[1].Rows[j]["ST_NO"]
						) {
						temp_table.Rows[i].Delete();
						i--;
						Log::Trace(" ", " ", "jin",i);
					}
					else
					{
						temp_table.Rows[i]["SPARE_ITEM_0"] = "2";//数据差异
					}
				}
			}
		}
		int t_b = temp_table.Rows.get_Count();
		for (int j = 0; j < bcls_rec->Tables[1].Rows.get_Count(); j++) {
			for (int i = 0; i < t_b; i++) {
				if (bcls_rec->Tables[1].Rows[j]["MAT_NO"] != temp_table.Rows[i]["MAT_NO"])
				{
					tmmsm01.Reset();
					tmmsm01.MergeFrom(bcls_rec->Tables[1].Rows[j]);
					tmmsm01["SPARE_ITEM_0"] = "3";//信息独有
					tmmsm01.MergeTo(temp_table);
					//temp_table.Rows[temp_table.Rows.get_Count()]["COMPARE_RESLUT"] = "3";
					break;
					//temp_table.Rows.Add();
					//temp_table.Rows[t_b+i].Merge(bcls_rec->Tables[1].Rows[j]);
					
				}
			}
		}
		bcls_ret->Tables[0].Copy(temp_table);
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "Database processing error，sqlcode = [{0}]." /* _RES("GCRSS0000006")*//*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
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
	cmd_inq.Close();

	return doFlag;
}


