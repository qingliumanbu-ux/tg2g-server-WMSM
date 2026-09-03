/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:      LZ
Version:     1.1.1
Date:        2022-10-20
Description: 垛位推荐
**************************************************/
#define MES_STOCK_RULE_POWER 50;
#define MES_varName(x) #x
#include "stdafx.h"		// 框架头，不可删除 
#include <list>
#pragma region 库图构建
class MesMatSm;

class MesStockplaceSm
{
public:
	MesStockplaceSm() {}
	~MesStockplaceSm() {}
	void AddMat();
	int Setinfo();
public:
	CString STOCK_PLACE_NO = "";                     //库位号
	CString STOCK_NO = "";                           //库号
	CString HALL_NO = "";                            //跨号
	CString LOGIC_STOCK_NO = "";                     //逻辑库号
	CString MAT_KIND = "";                           //物料种类
	CString STOCK_PLACE_TYPE = "";                   //库位类型
	CString DEV_DIV = "";                            //设备区分代码
	CString STOCK_STATUS = "";                       //库位状态
	CString DEV_NO = "";                             //设备编号
	CString ROWNO = "";                              //库位行号
	CString COLUMN_NO = "";                          //库位列号
	CDecimal X_RANGE = 0;                              //库位X轴坐标
	CDecimal Y_RANGE = 0;                            //库位Y轴坐标
	CDecimal WIDTH_DELTA = 0;                        //库位宽度差
	CDecimal WIDTH_DELTA_NEXT = 0;                   //库位相邻宽度差
	CDecimal REM_GRADE = 0;                          //库位推荐分数
	CDecimal MAX_LEN = 0;                            //库位最大长度
	CDecimal MAX_HEIGHT = 0;                         //库位最大高度
	CDecimal MAX_WIDTH = 0;                          //库位最大宽度
	CDecimal NOW_HEIGHT = 0;                         //库位当前高度
	list<MesMatSm> list_mat;
};

class MesMatSm
{
public:
	MesMatSm() {}
	~MesMatSm() {}
	int Setinfo(const CString &mat_no);
	int Setinfo(CString mat_no, MesStockplaceSm Stockplace);
public:
	//材料基本信息
	CString MAT_NO = "";                        //材料号
	CString MAT_KIND = "";                    //材料种类
	CString ORDER_NO = "";                    //合同号
	CString ORIGIN_MAT_NO = "";               //外购材料号
	CString PRODUCT_FLAG = "";                //成品标记
	CString MAT_STATUS = "";                  //材料状态码
	CString PONO = "";                        //制造命令号
	CString ST_NO = "";                       //出钢记号
	CString SG_SIGN = "";                     //牌号（钢级）
	CString NEXT_WHOLE_BACKLOG_CODE = "";     //下工序
	CString MAT_DESTION = "";                 //材料去向
	CString PLAN_NO = "";                       //计划号
	CString LAYERNO = "";                       //层号
	CDecimal HOOD_TIME = 0;                   //出钢记号
	CDecimal MAT_LEN = 0;                       //长度
	CDecimal MAT_THICK = 0;                     //厚度 
	CDecimal MAT_WIDTH = 0;                     //宽度
	MesStockplaceSm STOCK_PLACE;                //材料所在库位
	//材料命令信息
	CString STOCK_PLACE_NO_TO = "";           //吊车命令目标库位
	CString STOCK_OPER_ORDER = "";            //吊车命令类型
	CDecimal CRANE_CMDGRPNO = 0;              //吊车命令组号
	CDecimal CMD_SEQ = 0;                     //吊车命令流水号
	CString STOCK_OPER_ORDER_FIN = "";        //最终库业务类型
	CString CRANE_INST_STATUS = "";           //行车命令状态

};

class MesAreaSm
{
public:
	MesAreaSm() {}
	~MesAreaSm() {}
	int search(EIClass* bcls_ret);
	int find_finestock_place(const CString& STOCK_PLACE_NO);
	int find_finestock_placesm(CString& STOCK_PLACE_NO, const CString stock_oper_order, CDataTable tab_mst);
public:
	CString TABLE_NAME = "";         //主档表
	CString AREA_NO = "";         //区域号
	CString AREA_TYPE = "";       //区域类型
	CString AREA_SEARCH = "1";     //区域搜索方式
	CString AREA_SQL = "";        //区域SQL
	CString AREA_RPN = "";        //区域RPN
	CString RULE_1 = "";          //规则一
	CString RULE_2 = "";          //规则二
	CString RULE_3 = "";          //规则三
	CString RULE_4 = "";          //规则四
	CString RULE_5 = "";          //规则五
	CString RULE_6 = "";          //规则六
	CString RULE_7 = "";          //规则七
	CString RULE_8 = "";          //规则八
	CString RULE_9 = "";          //规则九
	CString RULE_10 = "";         //规则十
	CDecimal RULE_1_POWER = 0;           //规则一权重
	CDecimal RULE_2_POWER = 0;           //规则二权重
	CDecimal RULE_3_POWER = 0;           //规则三权重
	CDecimal RULE_4_POWER = 0;           //规则四权重
	CDecimal RULE_5_POWER = 0;           //规则五权重
	CDecimal RULE_6_POWER = 0;           //规则六权重
	CDecimal RULE_7_POWER = 0;           //规则七权重
	CDecimal RULE_8_POWER = 0;           //规则八权重
	CDecimal RULE_9_POWER = 0;           //规则九权重
	CDecimal RULE_10_POWER = 0;          //规则十权重
	list<MesStockplaceSm> list_stockplace;
};

int MesStockplaceSm::Setinfo()
{
	try
	{
		Log::Trace("", __FUNCTION__, "Setinfo1() begin");
		CDataTable tab_TEM;
		CString sql = " SELECT B.STOCK_PLACE_NO,B.STOCK_NO,B.HALL_NO,B.MAT_KIND,B.STOCK_PLACE_TYPE,B.DEV_DIV,B.DEV_NO,B.ROWNO,B.COLUMN_NO, "
			" B.X_RANGE,B.Y_RANGE,B.MAX_LEN,B.MAX_HEIGHT,B.MAX_WIDTH"
			" FROM TWM04 B WHERE B.STOCK_PLACE_NO='" + STOCK_PLACE_NO + "'";

		Log::Trace("", __FUNCTION__, "sql1:{0}", sql);
		Db::QueryTable(sql, tab_TEM);
		for (int i = 0; i < tab_TEM.Rows.get_Count(); ++i)
		{
			STOCK_PLACE_NO = tab_TEM.Rows[i]["STOCK_PLACE_NO"];
			STOCK_NO = tab_TEM.Rows[i]["STOCK_NO"];
			HALL_NO = tab_TEM.Rows[i]["HALL_NO"];
			MAT_KIND = tab_TEM.Rows[i]["MAT_KIND"];
			STOCK_PLACE_TYPE = tab_TEM.Rows[i]["STOCK_PLACE_TYPE"];
			DEV_DIV = tab_TEM.Rows[i]["DEV_DIV"];
			DEV_NO = tab_TEM.Rows[i]["DEV_NO"];
			ROWNO = tab_TEM.Rows[i]["ROWNO"];
			COLUMN_NO = tab_TEM.Rows[i]["COLUMN_NO"];
			X_RANGE = tab_TEM.Rows[i]["X_RANGE"];
			Y_RANGE = tab_TEM.Rows[i]["Y_RANGE"];
			//WIDTH_DELTA = tab_TEM.Rows[i]["WIDTH_DELTA"];
			//WIDTH_DELTA_NEXT = tab_TEM.Rows[i]["WIDTH_DELTA_NEXT"];
			MAX_LEN = tab_TEM.Rows[i]["MAX_LEN"];
			MAX_HEIGHT = tab_TEM.Rows[i]["MAX_HEIGHT"];
			MAX_WIDTH = tab_TEM.Rows[i]["MAX_WIDTH"];
			AddMat();
		}

		Log::Trace("", __FUNCTION__, "Setinfo() end");
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
	}
}

void MesStockplaceSm::AddMat()
{
	try
	{
		Log::Trace("", __FUNCTION__, "AddMat() begin");
		CString table_name("TMMSM01");
		/*if (MAT_KIND == "SM")
		{
			table_name = "TMMSM01";
		}
		else if (MAT_KIND == "HR")
		{
			table_name = "TMMSM01";
		}
		else if (MAT_KIND == "CR")
		{
			table_name = "TMMSM01";
		}*/
		CString sql = "SELECT A.MAT_NO,B.* FROM TWMA2 A left join " + table_name + " B ON A.MAT_NO=B.MAT_NO WHERE A.STOCK_PLACE_NO='" + STOCK_PLACE_NO + "'";
		CDataTable tab_TEM;
		Log::Trace("", __FUNCTION__, "sql1:{0}", sql);
		Db::QueryTable(sql, tab_TEM);
		for (int i = 0; i < tab_TEM.Rows.get_Count(); ++i)
		{
			MesMatSm mat;
			mat.MAT_NO = tab_TEM.Rows[i]["MAT_NO"];
			mat.MAT_KIND = tab_TEM.Rows[i]["MAT_KIND"];
			mat.ORDER_NO = tab_TEM.Rows[i]["ORDER_NO"];
			mat.MAT_THICK = tab_TEM.Rows[i]["MAT_THICK"];
			mat.MAT_WIDTH = tab_TEM.Rows[i]["MAT_WIDTH"];
			mat.SG_SIGN = tab_TEM.Rows[i]["SG_SIGN"];
			mat.STOCK_PLACE = *this;
			NOW_HEIGHT = NOW_HEIGHT + mat.MAT_THICK;
			list_mat.push_back(mat);
		}
		Log::Trace("", __FUNCTION__, "AddMat() end");
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
	}
}

int MesMatSm::Setinfo(const CString &mat_no)
{
	try
	{
		Log::Trace("", __FUNCTION__, "Setinfo1({0}) begin", mat_no);
		CString table_name("TMMSM01");
		CString  sql = "SELECT A.MAT_NO,B.STOCK_PLACE_NO FROM " + table_name + " A,TWMA2 B WHERE A.MAT_NO=B.MAT_NO AND A.MAT_NO='" + mat_no + "'";
		Log::Trace("", __FUNCTION__, "sql:{0}", sql);
		CDataTable tab_TEM;
		Db::QueryTable(sql, tab_TEM);
		if (tab_TEM.Rows.get_Count() > 0)
		{
			MesStockplaceSm stockplace;
			stockplace.STOCK_PLACE_NO = tab_TEM.Rows[0]["STOCK_PLACE_NO"];
			stockplace.Setinfo();
			for (int i = 0; i < tab_TEM.Rows.get_Count(); ++i)
			{
				MAT_NO = tab_TEM.Rows[i]["MAT_NO"];
				Log::Trace("", __FUNCTION__, "{0}", MAT_NO);
				//STOCK_PLACE = stockplace;
			}
		}
		Log::Trace("", __FUNCTION__, "Setinfo1 end");
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
	}
}
int MesAreaSm::find_finestock_placesm( CString& STOCK_PLACE_NO, const CString stock_oper_order, CDataTable tab_mst)//板坯的
{
	try
	{
		Log::Trace("", __FUNCTION__, "find_finestock_placesm begin");
		CString stock_place_no_emty = "";
		CString stock_place_no_nice = "";
		CDecimal stockvalue = 0;
		CDecimal maxstockvalue = 0;
		int i = 0;
		for (auto iter = list_stockplace.begin(); iter != list_stockplace.end(); iter++)//循环库位定义表
		{
			stockvalue = 0;
			Log::Trace("", __FUNCTION__, "stock_place = [{0}],[{1}]", iter->STOCK_PLACE_NO, i);
			Log::Trace("", __FUNCTION__, "DEV_DIV = [{0}],[{1}]", iter->DEV_DIV.Trim(), i);
			int size = iter->list_mat.size();
			Log::Trace("", __FUNCTION__, "size = [{0}]", size);
			for (auto iterl = iter->list_mat.begin(); iterl != iter->list_mat.end(); iterl++)
			{
				Log::Trace("", __FUNCTION__, "DEV_DIV = [{0}],[{1}]", iter->DEV_DIV.Trim(), i);
				if (tab_mst.Rows[0]["ORDER_NO"].ToString() == iterl->ORDER_NO)
				{
					stockvalue = stockvalue + RULE_1_POWER;
				}
				if (tab_mst.Rows[0]["SG_SIGN"].ToString() == iterl->SG_SIGN)
				{
					stockvalue = stockvalue + RULE_2_POWER;
				}
				if (tab_mst.Rows[0]["MAT_LEN"].ToDecimal() == (*iterl).MAT_LEN)//正常在一个范围内均可
				{
					stockvalue = stockvalue + RULE_3_POWER;
				}
				if (tab_mst.Rows[0]["MAT_DESTION"].ToString() == iterl->MAT_DESTION)
				{
					stockvalue = stockvalue + RULE_4_POWER;
				}
				if (tab_mst.Rows[0]["PLAN_NO"].ToString() == iterl->PLAN_NO)//要看计划顺序，顺序小无所谓
				{
					stockvalue = stockvalue + RULE_5_POWER;
				}
				if (tab_mst.Rows[0]["MAT_WIDTH"].ToDecimal() == (*iterl).MAT_WIDTH)//正常在一个范围内均可
				{
					stockvalue = stockvalue + RULE_6_POWER;
				}
				if (tab_mst.Rows[0]["MAT_NO"].ToString() ==  iterl->MAT_NO)
				{
					stockvalue = 0;
					break;
				}
				if (stock_oper_order.SubstringNE(0, 1) == "1")
				{
					if (iter->DEV_DIV.Trim() != "")
					{
						stockvalue = 0;
						break;
					}
				}
				if (maxstockvalue <= stockvalue)
				{
					maxstockvalue = stockvalue;
					stock_place_no_nice = iter->STOCK_PLACE_NO;
				}
			}
			i++;
			if (size == 0 && stock_place_no_emty.Trim() == "")//取最近的空垛位
			{
				stock_place_no_emty = iter->STOCK_PLACE_NO;
			}

		}
		STOCK_PLACE_NO = (stock_place_no_nice.Trim() == "") ? stock_place_no_emty : stock_place_no_nice;

		Log::Trace("", __FUNCTION__, "stock_place_no_nice = [{0}]", stock_place_no_nice);
		Log::Trace("", __FUNCTION__, "find_finestock_placesm end");
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		return  -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		return  -1;
	}
}
int MesAreaSm::search(EIClass* bcls_ret)
{
	try
	{
		CDataTable tab_TEM;
		//获取区域
		CString sql = " SELECT * FROM TWM04 WHERE STOCK_NO= '" + bcls_ret->Tables["AUTO_INFO_IN"].Rows[0]["STOCK_NO"].ToString() + "'";
		if (bcls_ret->Tables["AUTO_INFO_IN"].Rows[0]["HALL_NO"].ToString().Trim() != "")
		{
			sql += " AND HALL_NO= '" + bcls_ret->Tables["AUTO_INFO_IN"].Rows[0]["HALL_NO"].ToString().Trim() + "'";
		}
		//区域规则设定
		TABLE_NAME = bcls_ret->Tables["AUTO_INFO_IN"].Rows[0]["TABLE_NAME"].ToString().Trim();
		RULE_1 = "ORDER_NO";  //规则一
		RULE_2 = "NEXT_WHOLE_BACKLOG_CODE";                //规则二
		RULE_3 = "MAT_ACT_OUTER_DIA"; //卷看外径或者内外径差
		RULE_2_POWER = 10;
		RULE_3_POWER = 10;
		//if (bcls_ret->Tables["AUTO_INFO_IN"].Rows[0]["TABLE_NAME"].ToString().Trim()=="TMMSM01")
		//{
		//	RULE_2 = "SG_SIGN";                //规则二
		//	RULE_3 = "MAT_LEN";//板长不压短
		//	RULE_2_POWER = 10;
		//	RULE_3_POWER = 10;
		//}	   
		RULE_4 = "MAT_DESTION";//规则三
		RULE_5 = "PLAN_NO";            //规则四
		RULE_6 = "MAT_WIDTH";//板卷都有宽不压窄
		RULE_1_POWER = 10;
		RULE_4_POWER = 20;
		RULE_5_POWER = 10;
		RULE_6_POWER = 10;

		Db::QueryTable(sql, tab_TEM);
		if (tab_TEM.Rows.get_Count() > 0)
		{
			CString twm04 = "";
			CString sql_orderby = " ORDER BY HALL_NO";
			CDataTable tab_tem;
			//查出的库位排序
			//根据搜索方式排序库位,默认从中间到两边
			AREA_SQL = sql + sql_orderby;
			if (AREA_SEARCH.Trim() == "1")
			{
				Db::QueryTable(AREA_SQL, tab_TEM);
				for (int i = 0; i < tab_TEM.Rows.get_Count(); ++i)
				{
					MesStockplaceSm stock_place;
					stock_place.STOCK_PLACE_NO = tab_TEM.Rows[i]["STOCK_PLACE_NO"].ToString();
					stock_place.Setinfo();
					Log::Trace("", __FUNCTION__, "stock_place = [{0}]", stock_place.STOCK_PLACE_NO);
					list_stockplace.push_back(stock_place);
				}
			}
		}
		else
		{
			Log::Trace("", __FUNCTION__, "区域【{0}】不存在", bcls_ret->Tables["AUTO_INFO_IN"].Rows[0]["STOCK_NO"].ToString());
			return 0;
		}


	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		return  -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		return  -1;
	}

}
#pragma endregion

BM2_FUNCTION_EXPORT
int f_wmsmsm_cmd_auto(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	CString sql(""), sqlclumno(""), hall_no(""), table_name(""), stock_place_no("");

	CDataTable tab_mat;  //存放推荐材料



	bcls_ret->Tables[0].Columns.Clear();
	bcls_ret->Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO");
	bcls_ret->Tables[0].Columns.Add(DT_STRING, "LOGIC_STOCK_NO");

	try
	{
		if (bcls_rec->Tables.IndexOf("AUTO_INFO_IN") < 0 ||
			bcls_rec->Tables["AUTO_INFO_IN"].Rows.get_Count() == 0)
		{
			sprintf(s.msg, "函数f_auto中找不到接收块名[AUTO_INFO_IN]或值为空");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//获取传入参数
		CString mat_no = bcls_rec->Tables["AUTO_INFO_IN"].Rows[0]["MAT_NO"].ToString();
		CString stock_no = bcls_rec->Tables["AUTO_INFO_IN"].Rows[0]["STOCK_NO"].ToString();
		CString stock_oper_order = bcls_rec->Tables["AUTO_INFO_IN"].Rows[0]["STOCK_OPER_ORDER"].ToString();
		if (bcls_rec->Tables["AUTO_INFO_IN"].Columns.Contains("HALL_NO"))
		{
			hall_no = bcls_rec->Tables["AUTO_INFO_IN"].Rows[0]["HALL_NO"].ToString();
		}
		table_name = bcls_rec->Tables["AUTO_INFO_IN"].Rows[0]["TABLE_NAME"].ToString();
		sqlclumno = "MAT_NO,ORDER_NO,NEXT_WHOLE_BACKLOG_CODE,MAT_ACT_OUTER_DIA,MAT_DESTION,PLAN_NO,MAT_WIDTH";//卷和板字段不同
		if (table_name == "TMMSM01")
		{
			sqlclumno = "MAT_NO,ORDER_NO,SG_SIGN,MAT_LEN,MAT_DESTION,PLAN_NO,MAT_WIDTH";
		}
		sql = " select " + sqlclumno + " from " + table_name + " where mat_no='" + mat_no + "'";

		Log::Trace("", __FUNCTION__, "sql={0}", sql);
		Db::QueryTable(sql, tab_mat);
		if (tab_mat.Rows.get_Count() < 1)
		{
			sprintf(s.msg, "函数f_wm_cmd_auto中" + table_name + "未找到材料【" + mat_no + "】");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		tab_mat.Columns.Add(DT_STRING, "STOCK_OPER_ORDER");
		tab_mat.Rows[0]["STOCK_OPER_ORDER"] = stock_oper_order;
		Log::Trace("", __FUNCTION__, "MAT_NO={0}", mat_no);
		Log::Trace("", __FUNCTION__, "STOCK_NO={0}", stock_no);
		Log::Trace("", __FUNCTION__, "STOCK_OPER_ORDER={0}", stock_oper_order);
		Log::Trace("", __FUNCTION__, "TABLE_NAME={0}", table_name);
		MesAreaSm  area_auto;
		area_auto.search(bcls_rec);
		area_auto.find_finestock_placesm(stock_place_no, stock_oper_order, tab_mat);
		bcls_rec->Tables[0].Rows[0]["STOCK_PLACE_NO"] = stock_place_no;
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
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}