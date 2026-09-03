/*************************************************
版权: Baosight Software LTD.co Copyright (c) 2012

功能: 
修改历史:
日期:________;修改人:________; 需求提出人:________
变更内容:
**************************************************/

/*框架头文件*/
#include "stdafx.h" 
#include "epex.h"
int f_wm00_hposition(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
BM2F_ENTERACE(wmsmsmdwtj_dwtj)
struct CX1J197B
{
	CDecimal STRAND_NO;   //流号
	CString MAT_NO;   //材料号
	CString LOC_CODE;   //位置代码
	CDecimal MAT_ACT_WT;   //材料实际重量
	CDecimal HEAD_WIDTH;   //头部宽度
	CDecimal TAIL_WIDTH;   //尾部宽度
	CString JUDGE_CODE;   //判定结果
	CString ARCHIVE_CODE;   //缺陷代码

	void MergeFrom(CDataRow& row)
	{
		CDataTable& table = row.get_Table();
		if (table.Columns.Contains("STRAND_NO"))
		if (row["STRAND_NO"] != CDBNull::Value)
			this->STRAND_NO = (CDecimal)row["STRAND_NO"];
		if (table.Columns.Contains("MAT_NO"))
		if (row["MAT_NO"] != CDBNull::Value && (CString)row["MAT_NO"] != "")
			this->MAT_NO = (CString)row["MAT_NO"];
		if (table.Columns.Contains("LOC_CODE"))
		if (row["LOC_CODE"] != CDBNull::Value && (CString)row["LOC_CODE"] != "")
			this->LOC_CODE = (CString)row["LOC_CODE"];
		if (table.Columns.Contains("MAT_ACT_WT"))
		if (row["MAT_ACT_WT"] != CDBNull::Value)
			this->MAT_ACT_WT = (CDecimal)row["MAT_ACT_WT"];
		if (table.Columns.Contains("HEAD_WIDTH"))
		if (row["HEAD_WIDTH"] != CDBNull::Value)
			this->HEAD_WIDTH = (CDecimal)row["HEAD_WIDTH"];
		if (table.Columns.Contains("TAIL_WIDTH"))
		if (row["TAIL_WIDTH"] != CDBNull::Value)
			this->TAIL_WIDTH = (CDecimal)row["TAIL_WIDTH"];
		if (table.Columns.Contains("JUDGE_CODE"))
		if (row["JUDGE_CODE"] != CDBNull::Value && (CString)row["JUDGE_CODE"] != "")
			this->JUDGE_CODE = (CString)row["JUDGE_CODE"];
		if (table.Columns.Contains("ARCHIVE_CODE"))
		if (row["ARCHIVE_CODE"] != CDBNull::Value && (CString)row["ARCHIVE_CODE"] != "")
			this->ARCHIVE_CODE = (CString)row["ARCHIVE_CODE"];

	}
};

int f_wmsmsmdwtj_dwtj(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	/*打印程序起止日志*/
	CTracer log(__FUNCTION__);
	Log::Debug("", __FUNCTION__, "wmsmsmj3_rcm--------开始");
	/*定义公v_mat_no用变量*/
	int doFlag = 0;
	CString  sqlstr = "";
	CString  sql = "";
	CString v_mat_no,v_crane_no = "";
	CString  dateNow14 = CDateTime::Now().ToString("yyyyMMddHHmmss");  //14位日期变量

	//定义实体类
	CX1J197B x1j197b;

	CDbCommand cmd_inq(conn);
	CDbCommand cmd(conn);

	//EIClass bcls_temp;

	try
	{
		x1j197b.MergeFrom(bcls_rec->Tables[0].Rows[0]);//将电文内容放入结构体
		x1j197b.MAT_NO = "30000000000";
		x1j197b.LOC_CODE = bcls_rec->Tables[0].Rows[0]["STOCK_PLACE_POSITION"];
		if (x1j197b.LOC_CODE.Trim() == ""){
			s.flag = -1;
			strcpy(s.msg, "高低位不能为空！");
			return -1;
		}
		if (x1j197b.MAT_NO.GetLength() < 10)
		{
			s.flag = -1;
			strcpy(s.msg, "板坯号不正确！");
			return -1;
		}
		;

		//20160913 laiwenbin 针对高低位做判断
		if (0 == strcmp(x1j197b.MAT_NO, "30000000000"))
		{
			bcls_rec->AddColName(1, "loc_code");
			bcls_rec->SetColVal(1, 1, "loc_code", x1j197b.LOC_CODE);
			Log::Trace("", __FUNCTION__, "========================f_wm00_hposition开始==============================");
			doFlag = f_wm00_hposition(bcls_rec, bcls_ret, conn);
			Log::Trace("", __FUNCTION__, "========================f_wm00_hposition结束==============================");
			if (doFlag != 0)
			{
	/*			EDLog(1, 1, "f_ymsm_hposition处理失败.");
				doFlag = 0;*/

				s.flag = -1;
				doFlag = -1;
				throw CApplicationException(doFlag, s.msg, s.svc_name);
			}
			else
			{
				strcpy(s.msg, "处理成功！");
				doFlag = 0;
				/*s.sqlcode = 0;
				goto l_return;*/
			}
		}
	}

	catch (CDbException& ex)         //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006"), arguments, 1); /*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;                  //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)//捕获应用错误
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
