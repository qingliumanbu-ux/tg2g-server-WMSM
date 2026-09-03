/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:      JQ
Version:     1.1.1
Date:        2017-12-20
Description: 垛位推荐
**************************************************/

#include "stdafx.h"		// 框架头，不可删除 
#include "math.h"
#include<list>

extern int f_auto(EIClass* blkIn, EIClass* blkOut, CDbConnection* dbConn);

class WmsStockplace;
class WmsMat;
class WmsMap;
class WmsPeople;
class WmsMain;
CDataTable Tab_Sub;

class WmsMat
{
public:
	WmsMat();
	~WmsMat();
	int Setinfo(CString mat_no, CString stock_place_no_from);
	CString GetClass();
	CString GetCode(CString code);
public:
	CString MAT_NO = "";                        //材料号
	CString PLAN_NO = "";                       //计划号
	CString LAYERNO = "";                       //材料层号
	CString LOGIC_STOCK_NO = "";                //材料所在逻辑区
	CString STOCK_PLACE_NO_TO = "";             //材料目标库位
	CString STOCK_PLACE_NO = "";                //材料库位
	CString PILE_INDEX = "";                    //材料配山指标
	CDecimal HOOD_TIME = 0;                     //材料出钢记号
	CDecimal MAT_LEN = 0;                       //材料长度
	CDecimal MAT_THICK = 0;                     //材料厚度
	CDecimal MAT_WIDTH = 0;                     //材料宽度
	CDecimal MAT_WT = 0;                        //材料重量
	CDecimal X_RANGE = 0;                       //材料X
	CDecimal Y_RANGE = 0;                       //材料Y
	CDecimal COLUMN_NO = 0;                     //材料所在库位列号
	CString STOCK_PALCE_TO_NEXT = "";           //上层材料目标库位
	CString STOCK_PALCE_TO_DOWN = "";           //下层材料目标库位
	CString CODE13 = "";                        //13位码
	CString NEXT_CODE = "";                     //下道机组
	CString MAT_STATUS = "";                    //材料状态
	CString MAT_CLASS = "";                     //材料类别
	CString HALL_NO = "";                       //跨号
	CString NEXT_DIRECTION = "";                //材料流向
	CDecimal MAT_PLAN_NUM = 0;                  //材料计划顺序号

};
WmsMat::WmsMat()
{
}
WmsMat::~WmsMat()
{
}

int WmsMat::Setinfo(CString mat_no, CString stock_place_no_from)
{
	try
	{
		CString Sql = "SELECT B.MAT_NO,A.PLAN_NO,A.ST_NO,A.MAT_LEN,A.MAT_THICK,A.MAT_WIDTH,B.X_FROM,B.Y_FROM,B.LAYERNO,B.STOCK_PLACE_NO,B.LOGIC_STOCK_NO,B.PACK_NO,B.ROWNO AS COLUMN_NO,A.MAT_ACT_WT,A.SUB_BACKLOG_CODE,A.MAT_STATUS "
			" FROM (select B.MAT_NO,B.PLAN_NO,B.ST_NO,B.MAT_LEN,B.MAT_THICK,B.MAT_WIDTH,B.SUB_BACKLOG_CODE,B.MAT_STATUS,B.MAT_ACT_WT from TMMSM01 B) A "
			" LEFT JOIN(SELECT MAT_NO, C.X_FROM, C.Y_FROM, B.LAYERNO, B.STOCK_PLACE_NO, C.LOGIC_STOCK_NO,B.PACK_NO ,C.ROWNO FROM TWMA2 B, TWM04 C WHERE B.STOCK_PLACE_NO = C.STOCK_PLACE_NO)B ON A.MAT_NO = B.MAT_NO "
			//" LEFT JOIN TOPHPPSIP2 E ON A.MAT_NO = E.IN_MAT_NO AND E.TYPE!=''"
			" WHERE  A.MAT_NO = '" + mat_no + "'";
		CDataTable cmd_mat;
		Db::QueryTable(Sql, cmd_mat);
		if (cmd_mat.Rows.get_Count() == 0)
		{
			return -1;
		}
		else
		{
			MAT_NO = cmd_mat.Rows[0]["MAT_NO"];
			PLAN_NO = cmd_mat.Rows[0]["PLAN_NO"];
			MAT_LEN = cmd_mat.Rows[0]["MAT_LEN"];
			MAT_WIDTH = cmd_mat.Rows[0]["MAT_WIDTH"];
			MAT_THICK = cmd_mat.Rows[0]["MAT_THICK"];
			LAYERNO = cmd_mat.Rows[0]["LAYERNO"];
			X_RANGE = cmd_mat.Rows[0]["X_FROM"];
			Y_RANGE = cmd_mat.Rows[0]["Y_FROM"];
			PILE_INDEX = "";
			COLUMN_NO = cmd_mat.Rows[0]["COLUMN_NO"].ToDecimal();
			STOCK_PLACE_NO = cmd_mat.Rows[0]["STOCK_PLACE_NO"];
			LOGIC_STOCK_NO = cmd_mat.Rows[0]["LOGIC_STOCK_NO"];
			CODE13 = "";//GetCode(cmd_mat.Rows[0]["CODE"].ToString());
			NEXT_CODE = cmd_mat.Rows[0]["SUB_BACKLOG_CODE"];
			MAT_STATUS = cmd_mat.Rows[0]["MAT_STATUS"];
			MAT_WT = cmd_mat.Rows[0]["MAT_ACT_WT"];
			NEXT_DIRECTION = cmd_mat.Rows[0]["PACK_NO"];
			MAT_PLAN_NUM = 0;
			//MAT_CLASS = GetClass();
			
			CDecimal layerno_nest = cmd_mat.Rows[0]["LAYERNO"].ToDecimal() + 1;
			Sql = "SELECT STOCK_PLACE_NO_TO FROM TWMA7 WHERE STOCK_PLACE_NO_FROM='" + STOCK_PLACE_NO + "' AND YARD_LAYER_FROM='" + layerno_nest.ToString() + "' AND SUBSTR (STOCK_OPER_ORDER, 1,1)!='2'";
			STOCK_PALCE_TO_NEXT = Db::QueryCString(Sql);
			CDecimal layerno_down = cmd_mat.Rows[0]["LAYERNO"].ToDecimal() - 1;
			Sql = "SELECT STOCK_PLACE_NO_TO FROM TWMA7 WHERE STOCK_PLACE_NO_FROM='" + STOCK_PLACE_NO + "' AND YARD_LAYER_FROM='" + layerno_down.ToString() + "' AND SUBSTR (STOCK_OPER_ORDER, 1,1)!='2'";
			STOCK_PALCE_TO_DOWN = Db::QueryCString(Sql);

			Log::Trace("", __FUNCTION__, "STOCK_PALCE_TO_NEXT {0} $$$$$", STOCK_PALCE_TO_NEXT);
			Log::Trace("", __FUNCTION__, "STOCK_PALCE_TO_DOWN {0} $$$$$", STOCK_PALCE_TO_DOWN);
			if (STOCK_PLACE_NO.Trim() == "")
			{
				Sql = " SELECT X_FROM, Y_FROM, STOCK_PLACE_NO, LOGIC_STOCK_NO,ROWNO from twm04 where stock_place_no='" + stock_place_no_from + "'";
				CDataTable cmd_stock;
				Db::QueryTable(Sql, cmd_stock);
				if (cmd_stock.Rows.get_Count() > 0)
				{
					STOCK_PLACE_NO = cmd_stock.Rows[0]["STOCK_PLACE_NO"];
					LOGIC_STOCK_NO = cmd_stock.Rows[0]["LOGIC_STOCK_NO"];
					X_RANGE = cmd_stock.Rows[0]["X_FROM"];
					Y_RANGE = cmd_stock.Rows[0]["Y_FROM"];
				}
			}
			return 0;
		}
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

CString WmsMat::GetClass()
{
	CString mat_class = "";
	

	return mat_class;
}

CString WmsMat::GetCode(CString code)
{
	if (code.GetLength() < 12) return " ";
	CString re_code = "";
	for (int j = 0; j < Tab_Sub.Rows.get_Count(); ++j)
		re_code = re_code + code.Substring(atoi(Tab_Sub.Rows[j]["SUB_BEGIN"].ToString()), atoi(Tab_Sub.Rows[j]["SUB_NUM"].ToString()));


	return re_code;
}

class WmsStockplace
{
public:
	WmsStockplace();
	~WmsStockplace();
	void AddMat(CDbConnection* dbConn);
public:
	CString STOCK_PLACE_NO = "";                       //库位号
	CString ROWNO = "";                              //库位行号
	CString COLUMN_NO = "";                          //库位列号
	CDecimal X_RANGE = 0;                              //库位X轴坐标
	CDecimal Y_RANGE = 0;                            //库位Y轴坐标
	CDecimal WIDTH_DELTA = 0;                        //库位宽度差
	CDecimal WIDTH_DELTA_NEXT = 0;                   //库位相邻宽度差
	CDecimal LEN_DELTA = 0;                          //库位长度差
	CDecimal THICK_DELTA = 0;                        //库位厚度差
	CDecimal REM_GRADE = 0;                          //库位推荐分数
	CDecimal MAX_LEN = 0;                            //库位最大长度
	CDecimal MAX_HEIGHT = 0;                         //库位最大高度
	CDecimal MAX_WIDTH = 0;                          //库位最大宽度
	CDecimal MAX_WT = 0;                             //库位最大重量
	CDecimal MAX_COUNT = 0;                          //库位最大数量
	CDecimal NOW_HEIGHT = 0;                         //库位当前高度
	CDecimal NOW_COUNT = 0;                          //库位当前数量
	CDecimal NOW_WT = 0;                             //库位当前重量
	CDecimal CMD_FLAG = 0;                           //库位是否存在命令
	CDecimal PLAN_FLAG = 0;                          //库位是否切割命令
	CDecimal MR_PLAN_FLAG = 0;                       //库位是否轧制命令
	CDecimal MAT_MAX_WIDTH = 0;                      //库位材料最大宽度
	CDecimal MAT_MIN_WIDTH = 0;                      //库位材料最小宽度
	CDecimal MAT_MAX_LEN = 0;                        //库位材料最大长度
	CDecimal MAT_MIN_LEN = 0;                        //库位材料最小长度
	CDecimal MAT_MAX_THICK = 0;                       //库位材料最大厚度
	CDecimal MAT_MIN_THICK = 0;                      //库位材料最小厚度
	CDecimal MAT_LAYERNO_WIDTH = 0;                  //库位最上层材料宽度
	CDecimal MAT_LAYERNO_LEN = 0;                    //库位最上层材料长度
	CDecimal MAT_LAYERNO_THICK = 0;                  //库位最上层材料厚度
	CDecimal MIN_PLAN_NUM = 0;                       //库位最小计划顺序号
	CString  PILE_INDEX = "";                         //材料配山指标
	CString  CODE13 = "";                             //13位码
	CString  STOCK_CLASS = "";                        //库位类别
	CString  MIN_CLASS = "";                          //最小材料类别
	CString  FRIST_MAT_NO = "";                       //最上层材料号
	CString  FRIST_PLAN_NO = "";                      //最上层材料计划号
	CString  FRIST_DIRECTION = "";                      //最上层流向
	CDecimal MAX_Y = 0;                  //
	CDecimal MIN_Y = 0;                  //
	CDecimal MAX_X = 0;                  //
	CDecimal MIN_X = 0;                  //
	list<WmsMat> list_mat;
};
WmsStockplace::WmsStockplace()
{

}
WmsStockplace::~WmsStockplace()
{
}

void WmsStockplace::AddMat(CDbConnection* dbConn)
{
	try
	{
		
		
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

class WmsMap
{
public:
	WmsMap();
	~WmsMap();
	void AddStockplace(CDbConnection* dbConn);
public:
	CString Map_Num = "";                        //地图编号
	CString Hall_no = "";                      //跨
	CDecimal Start_Cloumn = 0;                 //起始列
	CDecimal Max_Cloumn = 0;                   //最大列
	CDecimal Min_Cloumn = 0;                   //最小列
	CString Logic_stock_no = "";               //区域
	CString Rem_Stock_place = "";              //推荐库位
	CDecimal Dev_no = 0;                         //区域分区
	CDecimal Search_mode = 0;                    //搜索方式
	CDecimal X_CD = 0;                         //X坐标
	CDecimal Y_CD = 0;                         //Y坐标
	list<WmsStockplace> list_stock_place;

};
WmsMap::WmsMap()
{

}
WmsMap::~WmsMap()
{

}

void WmsMap::AddStockplace(CDbConnection* conn)
{
	try
	{
		

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

class WmsPeople
{
public:
	WmsPeople();
	~WmsPeople();
	int Search_Map(WmsMap Map, WmsMat mat_info, CString *rem_stock_place, CDecimal next_num, CString out_code13, CString hun_flag, CString stock_oper_order, CString hall_no);
public:
	list<WmsMap> list_map;
};
WmsPeople::WmsPeople()
{

}
WmsPeople::~WmsPeople()
{

}

int WmsPeople::Search_Map(WmsMap Map, WmsMat mat_info, CString *rem_stock_place, CDecimal next_num, CString out_code13, CString hun_flag, CString stock_oper_order, CString hall_no)
{
	try
	{
		
		Log::Trace("", __FUNCTION__, "Search_Map() end");
		return 0;
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

class WmsMain
{
public:
	WmsMain(CDbConnection* dbConn);
	~WmsMain();
	int Setinfo(CString mat_no);


public:
	WmsMat InputMat;                    //需要推荐材料
	WmsPeople People;

protected:
	CDbConnection* dbConn;
	CDbCommand dbCmd;
public:
	void CreatePeople(CString hall_no, CString stock_oper_order, WmsMat InputMat);
};
WmsMain::WmsMain(CDbConnection* dbConn) : dbConn(dbConn), dbCmd(dbConn)
{

}
WmsMain::~WmsMain()
{

}

void WmsMain::CreatePeople(CString hall_no, CString stock_oper_order, WmsMat InputMat)
{
	try
	{
		Log::Trace("", __FUNCTION__, "CreatePeople() begin");
		
		Log::Trace("", __FUNCTION__, "CreatePeople() end");
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


BM2_FUNCTION_EXPORT
int f_auto(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	CModel twm04 = CModel("TWM04");
	CModel tmmsm01 = CModel("TMMSM01");
	CDataTable Table_st;
	CDataTable Table_mt;
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
		CString mat_no = bcls_rec->Tables["AUTO_INFO_IN"].Rows[0]["MAT_NO"].ToString().Trim();
		CString hall_no = bcls_rec->Tables["AUTO_INFO_IN"].Rows[0]["HALL_NO"].ToString().Trim();
		CString stock_oper_order = bcls_rec->Tables["AUTO_INFO_IN"].Rows[0]["STOCK_OPER_ORDER"].ToString().Trim();
		CDecimal next_num = 0;
		CString out_code13 = "";
		CString hun_flag = "";
		CString stock_place_no_from = "";
		CString layerno_from = "";
		CString rem_flag = "";
		CString sql = "";
		CString sqlst = "";
		CString stock_place_no = "";
		if (bcls_rec->Tables["AUTO_INFO_IN"].Columns.Contains("NEXT_NUM"))
			next_num = bcls_rec->Tables["AUTO_INFO_IN"].Rows[0]["NEXT_NUM"].ToDecimal();
		if (bcls_rec->Tables["AUTO_INFO_IN"].Columns.Contains("LAYERNO_FROM"))
			layerno_from = bcls_rec->Tables["AUTO_INFO_IN"].Rows[0]["LAYERNO_FROM"].ToString();
		if (bcls_rec->Tables["AUTO_INFO_IN"].Columns.Contains("OUT_CODE13"))
			out_code13 = bcls_rec->Tables["AUTO_INFO_IN"].Rows[0]["OUT_CODE13"].ToString();
		if (bcls_rec->Tables["AUTO_INFO_IN"].Columns.Contains("HUN_FLAG"))
			hun_flag = bcls_rec->Tables["AUTO_INFO_IN"].Rows[0]["HUN_FLAG"].ToString();
		if (bcls_rec->Tables["AUTO_INFO_IN"].Columns.Contains("STOCK_PLACE_NO_FROM"))
			stock_place_no_from = bcls_rec->Tables["AUTO_INFO_IN"].Rows[0]["STOCK_PLACE_NO_FROM"].ToString();
		if (bcls_rec->Tables["AUTO_INFO_IN"].Columns.Contains("REM_FLAG"))
			rem_flag = bcls_rec->Tables["AUTO_INFO_IN"].Rows[0]["REM_FLAG"].ToString();

		Log::Trace("", __FUNCTION__, "MAT_NO  = [{0}]", mat_no);
		Log::Trace("", __FUNCTION__, "HALL_NO  = [{0}]", hall_no);
		Log::Trace("", __FUNCTION__, "STOCK_OPER_ORDER  = [{0}]", stock_oper_order);
		Log::Trace("", __FUNCTION__, "NEXT_NUM  = [{0}]", next_num);
		Log::Trace("", __FUNCTION__, "OUT_CODE13  = [{0}]", out_code13);
		Log::Trace("", __FUNCTION__, "hun_flag  = [{0}]", hun_flag);
		Log::Trace("", __FUNCTION__, "stock_place_no_from  = [{0}]", stock_place_no_from);
		Log::Trace("", __FUNCTION__, "layerno_from  = [{0}]", layerno_from);
		Log::Trace("", __FUNCTION__, "rem_flag  = [{0}]", rem_flag);
		if (stock_oper_order == "32" && hall_no.Trim() != "F")
		{
			Log::Trace("", __FUNCTION__, "过跨不推荐");
			bcls_ret->Tables[0].Rows.Add();
			bcls_ret->Tables[0].Rows[0]["LOGIC_STOCK_NO"] = " ";
			bcls_ret->Tables[0].Rows[0]["STOCK_PLACE_NO"] = " ";
			return 0;
		}

		if (mat_no == "" || hall_no == "" || stock_oper_order == "")
		{
			sprintf(s.msg, "传入数据有空值");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		

		//初始化推荐材料信息
		WmsMat MatInfo;
		if (MatInfo.Setinfo(mat_no, stock_place_no_from) < 0)
		{
			Log::Trace("", __FUNCTION__, "材料[{0}]信息不存在", mat_no);
			bcls_ret->Tables[0].Rows.Add();
			bcls_ret->Tables[0].Rows[0]["LOGIC_STOCK_NO"] = " ";
			bcls_ret->Tables[0].Rows[0]["STOCK_PLACE_NO"] = " ";
			return 0;
		}
		Log::Trace("", __FUNCTION__, "MAT_THICK[{0}]", MatInfo.MAT_THICK);
		if (MatInfo.MAT_THICK <= 190)
		{
			Log::Trace("", __FUNCTION__, "[{0}]不推荐", mat_no);
			bcls_ret->Tables[0].Rows.Add();
			bcls_ret->Tables[0].Rows[0]["LOGIC_STOCK_NO"] = " ";
			bcls_ret->Tables[0].Rows[0]["STOCK_PLACE_NO"] = " ";
			return 0;
		}


		WmsMain main(conn);
		main.CreatePeople(hall_no, stock_oper_order, MatInfo);
		/*if (main.People.list_map.size()>0)
		{*/
			sql = "SELECT STOCK_PLACE_NO,MAX_HEIGHT,MAX_WIDTH,MAX_LEN,MAX_WT,"
				"PRE_COM_HEIGHT,PRE_MAT_NUM,PRE_COM_WT,"
				"PILE_MAT_HEI_ACT,PILE_MAT_NUM_ACT,PILE_MAT_WT_ACT FROM TWM04 WHERE HALL_NO='" + hall_no + "' AND STOCK_STATUS !='9' and STOCK_NO ='A21' ";
			sqlst = " SELECT MAT_ACT_LEN,MAT_ACT_THICK,MAT_ACT_WIDTH,MAT_ACT_WT FROM TMMSM01 WHERE MAT_NO='" + mat_no + "'";
			Log::Trace("", __FUNCTION__, "sql[{0}]", sql);
			Db::QueryTable(sql, Table_st);
			Log::Trace("", __FUNCTION__, "Table_st.Rows.get_Count()[{0}]", Table_st.Rows.get_Count());
			if (Table_st.Rows.get_Count() == 0)
			{
				Log::Trace("", __FUNCTION__, "材料[{0}]推荐区域失败", mat_no);
				return 0;
			}
			Log::Trace("", __FUNCTION__, "sqlst[{0}]", sql);
			Db::QueryTable(sqlst, Table_mt);
			for (int i = 0; i < Table_st.Rows.get_Count(); i++)
			{
				if (Table_mt.Rows[0]["MAT_ACT_LEN"].ToDecimal() < Table_st.Rows[i]["MAX_LEN"].ToDecimal());
				{	
					if (Table_mt.Rows[0]["MAT_ACT_WIDTH"].ToDecimal() < Table_st.Rows[i]["MAX_WIDTH"].ToDecimal());
					{			
						if (Table_mt.Rows[0]["MAT_ACT_THICK"].ToDecimal() < (Table_st.Rows[i]["MAX_HEIGHT"].ToDecimal() - (Table_st.Rows[i]["PILE_MAT_HEI_ACT"].ToDecimal() + Table_st.Rows[i]["PRE_COM_HEIGHT"].ToDecimal())))
						{				
							if (Table_mt.Rows[0]["MAT_ACT_WT"].ToDecimal() < (Table_st.Rows[i]["MAX_WT"].ToDecimal() - (Table_st.Rows[i]["PILE_MAT_WT_ACT"].ToDecimal() + Table_st.Rows[i]["PRE_COM_WT"].ToDecimal())))
							{
								stock_place_no =Table_st.Rows[i]["STOCK_PLACE_NO"].ToString().Trim();
							}
						}
					}
				}
				if (stock_place_no != "")
				{
					Log::Trace("", __FUNCTION__, "推荐成功");
					Log::Trace("", __FUNCTION__, "推荐区域为:[{0}]", stock_place_no);
					bcls_ret->Tables[0].Rows.Add();
					bcls_ret->Tables[0].Rows[0]["STOCK_PLACE_NO"] = stock_place_no;
					bcls_ret->Tables[0].Rows[0]["LOGIC_STOCK_NO"] = " ";
					break;
				}
			}
			if (stock_place_no == "")
			{
				Log::Trace("", __FUNCTION__, "111材料[{0}]推荐区域失败", mat_no);
			}
		
			
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