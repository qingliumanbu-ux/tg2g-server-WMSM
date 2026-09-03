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

//函数申明 
BM2_FUNCTION_IMPORT
int f_wmsmsm_stock_in(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);
int f_wmsmsm_stock_move(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);
BM2F_ENTERACE(wmsmsma7_crane_com)

int f_wmsmsma7_crane_com(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	/*打印程序起止日志*/
	CTracer log(__FUNCTION__);
	Log::Debug("", __FUNCTION__, "wmsmsmj3_crane--------开始");
	/*定义公用变量*/
	int doFlag = 0;
	CString  sqlstr = "";
	CString  sql = "";
	CString  dateNow14 = CDateTime::Now().ToString("yyyyMMddHHmmss");  //14位日期变量
	CString v_cmd_status ,targ_pos, cmd_status = "";
	CDecimal v_pile_main_no = 0;//2016-10-15：垛位主分区属性
	CDecimal v_pile_assis_no = 0;//2016-10-15：垛位辅助分区属性
	CString v_slab_diff = " ";//2016-10-15：板坯分区属性
	CString v_create_time = " ";
	CString l_cmd_status = "";
	CString v_store_area = " ";//分区属性交换标志(00：无，01：有)
	CString v_fieldno = " ";
	CString v_layer_no = " ";//行车中转命令用
	CString v_targ_pos_temp = " ";//直装命令用
	CDecimal v_num = 0;
	CDecimal v_slab_num = 0;
	CDecimal v_num_sour = 0;
	CDecimal v_pre_mat_num;
	CDecimal v_layerno_max = 0;
	CDecimal v_layerno = 0;
	CDecimal v_commit_num = 0;
	CDecimal fetchRowCount = 0;
	CDecimal v_slab_pile = 0;
	CDecimal v_slab_pile_1 = 0;
	CDecimal v_pile_height_max = 0;
	CDecimal l_pile_no_num = 0;//中转垛位当前板坯数量

	//实体类
	CDbCommand cmd_inq(conn);
	CDbCommand cmd(conn);


	//调用仓库入库主函数
	CModel twma0 = CModel("TWMA0");
	CModel twma2 = CModel("TWMA2");
	CModel twma7 = CModel("TWMA7");
	CModel twm04 = CModel("TWM04");
	CModel hwm00a7 = CModel("HWM00A7");
	CModel tmmsm01 = CModel("TMMSM01");

	EIClass bcls_stock_in;
	bcls_stock_in.Tables[0].set_TableName("WM_STOCK");
	bcls_stock_in.Tables[0].Columns.Add(twma0);
	bcls_stock_in.Tables[0].Columns.Add(twma2);
	bcls_stock_in.Tables[0].Rows.Clear();

	//调用仓库倒垛主函数
	EIClass bcls_stock_move;
	bcls_stock_move.Tables[0].set_TableName("WM_STOCK");
	bcls_stock_move.Tables[0].Columns.Add(twma0);
	bcls_stock_move.Tables[0].Columns.Add(twma2);
	bcls_stock_move.Tables[0].Rows.Clear();
	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			twma7.Reset();
			twma7.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			cmd_status = bcls_rec->Tables[0].Rows[i]["CMD_STATUS"];
			Log::Trace("", __FUNCTION__, "cmd_status = [{0}]", cmd_status);
			Log::Trace("", __FUNCTION__, "mat_No = [{0}]", twma7["MAT_NO"].ToString());

			if (!twma7.Query("CRANE_INST_CODE,MAT_NO")){
				s.flag = -1;
				strcpy(s.msg, " 吊车命令查询失败！");
				return -1;
			}

			if (twma7["CRANE_INST_STATUS"].ToString().Trim() == "0")
			{
				EDLog(1, 1, "命令没有确认");
				//根据命令号、板坯号更新行车命令  
				twma7["CRANE_INST_STATUS"] = cmd_status;
				twma7["CLIENT_IP"] = bcls_rec->Tables[0].Rows[i]["CLIENT_IP"];//保存操作机器的ip
				//fin_pos = ' ',
				twma7["REC_REVISE_TIME"] = dateNow14;
				twma7["REC_REVISOR"] = s.userid;
				twma7.Update("CRANE_INST_STATUS,CLIENT_IP,REC_REVISE_TIME,REC_REVISOR","CRANE_INST_CODE,MAT_NO");
				
				;
				//针对直接装车命令对目标垛位进行修改
				if (cmd_status !="1")
				{
					EDLog(1, 1, "进入直接装车目标垛位修改");
					if (strcmp(twma7["CRANE_INST_STATUS"].ToString().Trim(), "3") == 0)
					{
						v_targ_pos_temp= "F102";
					}
					else
					{
						v_targ_pos_temp="J101";
					}
					targ_pos = twma7["STOCK_PLACE_NO_TO"];
					twma7["CRANE_INST_STATUS"] = "1";
					twma7["STOCK_PLACE_NO_TO"] = v_targ_pos_temp;
					twma7.Update("CRANE_INST_STATUS,STOCK_PLACE_NO_TO", "CRANE_INST_CODE,MAT_NO");

					sqlstr = "update twm04 set PRE_COM_HEIGHT = PRE_COM_HEIGHT - 1 where stock_place_no = @targ_pos";
					cmd.Close();
					cmd.SetCommandText(sqlstr);
					Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
					cmd.Parameters.Set("targ_pos", targ_pos);
					cmd.ExecuteNonQuery();

					sqlstr = "update twm04 set PRE_COM_HEIGHT = PRE_COM_HEIGHT +1 where stock_place_no = @v_targ_pos_temp";
					cmd.Close();
					cmd.SetCommandText(sqlstr);
					Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
					cmd.Parameters.Set("v_targ_pos_temp", v_targ_pos_temp);
					cmd.ExecuteNonQuery();
				}

				//if (0 == strcmp(twma7.DIV_STATUS, "2") && strcmp(twma7.CRANE_INST_STATUS, "1") == 0)//针对板坯中转作业的行车命令
				//{
				//	EDLog(1, 1, "进入板坯中转作业命令确认流程");
				//

					//取当前垛位的最大层次
				//	EXEC SQL
				//		SELECT count(layer_no) + 1
				//	INTO :l_pile_no_num
				//		  FROM tymsm30
				//		  WHERE pile_no = : tymsm10.targ_pos
				//		  ;
				//	sprintf(v_layer_no, "%.2d", l_pile_no_num);  //转换为2位字符
				//	//更新垛位层次
				//	EXEC SQL
				//		update tymsm30
				//		set	pile_no = :tymsm10.targ_pos,
				//		layer_no = : v_layer_no,
				//		cmd_flag = '0'/*行车命令标志，1：命令中，0：无命令*/
				//	where end_flag = '0'	and mat_no = : tymsm10.mat_no
				//	;
				//	temp_flag = 1;
				//}
				//else 
				if (strcmp(twma7["CRANE_INST_STATUS"].ToString().Trim(), "1") == 0)//针对确认命令的板坯
				{
					EDLog(1, 1, "进入命令确认流程");
					v_cmd_status = 1;
					
					twm04.Reset();
					twm04["STOCK_PLACE_NO"] = twma7["STOCK_PLACE_NO_TO"];
						Log::Trace("", __FUNCTION__, "STOCK_PLACE_NO_TO = [{0}]", twma7["STOCK_PLACE_NO_TO"].ToString());
					if (!twm04.Query("STOCK_PLACE_NO")){
						s.flag = -1;
						strcpy(s.msg, "垛位" + twma7["STOCK_PLACE_NO_TO"].ToString()+"查询失败！");
						return -1;
					}
					v_pile_main_no = twm04["PILE_MAIN_NO"];
					v_pile_assis_no = twm04["PILE_ASSIS_NO"];
					v_store_area = twm04["STORE_AREA"];
					v_num = twm04["PILE_MAT_NUM_ACT"];
					v_pile_height_max = twm04["MAX_HEIGHT"];
					v_fieldno = twm04["FIELDNO"];

					sqlstr = "SELECT count(*)  from tmmsm01 where STOCK_PLACE_NO = @STOCK_PLACE_NO and MAT_POSITION = '2'";
					cmd_inq.Close();
					cmd_inq.SetCommandText(sqlstr);
					Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
					Log::Trace("", __FUNCTION__, "STOCK_PLACE_NO_TO = [{0}]", twma7["STOCK_PLACE_NO_TO"].ToString());
					cmd_inq.Parameters.Set("STOCK_PLACE_NO", twma7["STOCK_PLACE_NO_TO"]);
					cmd_inq.ExecuteReader();
					cmd_inq.Read();
					
					v_slab_num = cmd_inq.GetInt32(1);
						;
					if (v_num < 0)
					{
						EDLog(1, 1, "目标垛位的板坯数低于0");
						v_num = 0;
					}
					//修正板坯实际块数
					if (v_slab_num != v_num)
					{
						v_num = v_slab_num;
					}

					//2016-10-15：根据目标垛位取目标垛位的垛位主分区属性（只对下线的行车命令执行目标垛位属性维护）
					if (0 != strcmp(twma7["STOCK_PLACE_NO_TO"].ToString().Trim(), "H101") && (0 == strcmp(twma7["STOCK_PLACE_NO_TO"].ToString().Trim(), "I010") || 0 == strcmp(twma7["STOCK_PLACE_NO_TO"].ToString().Trim(), "I102")))
					{
						EDLog(1, 1, "进入目标垛位分区维护");
						//取板坯的分区属性值
						tmmsm01.Reset();
						tmmsm01["MAT_NO"] = twma7["MAT_NO"];
						if (!tmmsm01.Query("MAT_NO")){
							s.flag = -1;
							strcpy(s.msg, "板坯" + twma7["MAT_NO"].ToString() + "查询失败！");
							return -1;
						}
						v_slab_diff = tmmsm01["DEFECT_DEGREE_L_4"];

						v_slab_pile = atoi(v_slab_diff);//转换为数字型

						EDLog(1, 1, "板坯垛位属性值为 = [%s]", v_slab_diff);
						EDLog(1, 1, "板坯垛位属性值转换后为 = [%d]", v_slab_pile);

						EDLog(1, 1, "垛位主分区值为 = [%d]", v_pile_main_no);
						EDLog(1, 1, "垛位辅助分区值为 = [%d]", v_pile_assis_no);
						EDLog(1, 1, "垛位分区区别值为 = [%s]", v_store_area);

						//2016-10-15：如果行车命令中目标垛位的属性和板坯属性值一致则说明推荐与实际一致，更新主分区时间
						if (v_slab_pile == v_pile_main_no)
						{
							if (v_pile_height_max <= v_num + 1)//当垛位满时间初始化()修改时间：2018-2-27
							{
								EDLog(1, 1, "重置主分区更新时间");
								sqlstr = "UPDATE TWM04 SET FIELDNO_UPTIME = '20160101000000' WHERE stock_place_no = @stock_place_no";
								cmd.Close();
								cmd.SetCommandText(sqlstr);
								Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
								cmd.Parameters.Set("stock_place_no", twma7["STOCK_PLACE_NO_TO"].ToString().Trim());
								cmd.ExecuteNonQuery();
							}
							else if (v_num == 0)
							{
								EDLog(1, 1, "更新主分区更新时间");
								sqlstr = "UPDATE TWM04 SET FIELDNO_UPTIME = '" + dateNow14 + "' WHERE stock_place_no = @stock_place_no";
								cmd.Close();
								cmd.SetCommandText(sqlstr);
								Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
								cmd.Parameters.Set("stock_place_no", twma7["STOCK_PLACE_NO_TO"].ToString().Trim());
								cmd.ExecuteNonQuery();							
							}
						}
						else
						{
							EDLog(1, 1, "进入目标垛位分区交换判断");
							if (v_num == 0 && 0 == strcmp(v_store_area, "1"))//垛位允许扩展，并且是空垛位
							{
								if (v_slab_pile <10)
								{
									v_slab_pile_1 = 5;
								}
								if ((twma7["DIV_STATUS"].ToString().Trim() == "1") || (v_slab_pile_1 == v_pile_assis_no) || ((v_slab_pile / 10).Round(0) * 10 == v_pile_assis_no))
								{
									EDLog(1, 1, "交换目标垛位主辅助分区属性");
									sqlstr = "update TWM04 set pile_main_no = @v_slab_pile, table_no = @v_pile_main_no,"
										" store_area = '2',fieldno_uptime = @dateNow14  where stock_place_no =@stock_place_no";
									cmd.Close();
									cmd.SetCommandText(sqlstr);
									Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
									cmd.Parameters.Set("v_slab_pile", v_slab_pile);
									cmd.Parameters.Set("v_pile_main_no", v_pile_main_no);
									cmd.Parameters.Set("dateNow14", dateNow14);
									cmd.Parameters.Set("stock_place_no", twma7["STOCK_PLACE_NO_TO"].ToString().Trim());
									cmd.ExecuteNonQuery();
								}
							}
						}
					}

					v_layerno = v_num + 1;//层次为目标垛位板坯数+1

					//sprintf(tmmsm01.LAYERNO, "%.2d", v_layerno);  //转换为2位字符
					tmmsm01["LAYERNO"] = v_layerno;
					sqlstr = "update tmmsm01	set	stock_place_no =@stock_place_no,"
						" layerno = @layerno,	cmd_flag = '0'"/*行车命令标志，1：命令中，0：无命令*/
					" where  mat_no = @mat_no; ";
					cmd.Close();
					cmd.SetCommandText(sqlstr);
					Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
					cmd.Parameters.Set("layerno", tmmsm01["LAYERNO"]);
					cmd.Parameters.Set("mat_no", twma7["MAT_NO"].ToString().Trim());
					cmd.Parameters.Set("stock_place_no", twma7["STOCK_PLACE_NO_TO"].ToString().Trim());
					cmd.ExecuteNonQuery();
					

					EDLog(1, 1, "tmmsm01.layerno=%s", tmmsm01["LAYERNO"].ToString());

					sqlstr = "update TWM04	set PILE_MAT_NUM_ACT = PILE_MAT_NUM_ACT - 1"
						" where stock_place_no = @stock_place_no";/*更新源垛位板坯块数*/
					cmd.Close();
					cmd.SetCommandText(sqlstr);
					Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
					cmd.Parameters.Set("stock_place_no", twma7["STOCK_PLACE_NO_TO"].ToString().Trim());
					cmd.ExecuteNonQuery();

					sqlstr = "update TWM04 set PILE_MAT_NUM_ACT = PILE_MAT_NUM_ACT - 1"
							" where stock_place_no = @stock_place_no";/*更新源垛位板坯块数*/
					cmd.Close();
					cmd.SetCommandText(sqlstr);
					Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
					cmd.Parameters.Set("stock_place_no", twma7["STOCK_PLACE_NO_FROM"].ToString().Trim());
					cmd.ExecuteNonQuery();
					
					sqlstr = "update TWM04 set PILE_MAT_NUM_ACT = @v_num + 1,"/*更新目标垛位板坯块数*/
						" pre_mat_num = pre_mat_num - 1	"/*更新目标垛位板坯预约块数*/
						" where stock_place_no = @stock_place_no";
					cmd.Close();
					cmd.SetCommandText(sqlstr);
					Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
					cmd.Parameters.Set("stock_place_no", twma7["STOCK_PLACE_NO_TO"].ToString().Trim());
					cmd.Parameters.Set("v_num", v_num);
					cmd.ExecuteNonQuery();

					//更新源垛位状态
					sqlstr = "update twm04 set STOCK_STATUS='0' where stock_place_no = @stock_place_no"
						" AND (SELECT COUNT(*) FROM TWMA7 WHERE STOCK_PLACE_NO_FROM=@stock_place_no AND CRANE_INST_STATUS='0')=0";//更新目标垛位板坯预约块数
					cmd.Close();
					cmd.SetCommandText(sqlstr);
					Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
					cmd.Parameters.Set("stock_place_no", twma7["STOCK_PLACE_NO_FROM"].ToString().Trim());
					cmd.ExecuteNonQuery();


					//归档
					hwm00a7.Reset();
					hwm00a7.CopyFrom(twma7);
					hwm00a7["SVC_NAME"] = s.svc_name;
					hwm00a7["CLIENT_IP"] = s.fore_ip;
					hwm00a7["REC_ERASOR"] = s.userid;
					hwm00a7["REC_ERASE_TIME"] = dateNow14;
					hwm00a7.Insert();

					twma7.Delete("MAT_NO,CRANE_INST_CODE");

					//产品化一些倒垛会空
					if (twma7["MOVE_TYPE"].ToString().Trim() == ""){
						twma7["MOVE_TYPE"] = "D";
					}

					//
					//入库调用入库函数
					if (twma7["MOVE_TYPE"].ToString().Trim() == "I"){//入库
						//调用仓库入库主函数
						bcls_stock_in.Tables["WM_STOCK"].Rows.Add();
						bcls_stock_in.Tables["WM_STOCK"].Rows[i]["MAT_NO"] = twma7["MAT_NO"];
						bcls_stock_in.Tables["WM_STOCK"].Rows[i]["STOCK_OPER_ORDER"] = "1B";
						bcls_stock_in.Tables["WM_STOCK"].Rows[i]["STOCK_OPER_ORDER_DIV"] = "1";
						bcls_stock_in.Tables["WM_STOCK"].Rows[i]["STOCK_NO"] = twm04["STOCK_NO"];
						bcls_stock_in.Tables["WM_STOCK"].Rows[i]["STOCK_PLACE_NO"] = twma7["STOCK_PLACE_NO_TO"];
						bcls_stock_in.Tables["WM_STOCK"].Rows[i]["ROWNO"] = " ";
						bcls_stock_in.Tables["WM_STOCK"].Rows[i]["COLUMN_NO"] = " ";
						bcls_stock_in.Tables["WM_STOCK"].Rows[i]["LAYERNO"] = 0;
						bcls_stock_in.Tables["WM_STOCK"].Rows[i]["STOCK_PLACE_POSITION"] = " ";
					}
					else if (twma7["MOVE_TYPE"].ToString().Trim() == "O"){//出库

					}
					else  if (twma7["MOVE_TYPE"].ToString().Trim() == "D"){//倒垛
						//调用仓库倒垛主函数
						bcls_stock_move.Tables["WM_STOCK"].Rows.Add();
						bcls_stock_move.Tables["WM_STOCK"].Rows[i]["MAT_NO"] = twma7["MAT_NO"];
						bcls_stock_move.Tables["WM_STOCK"].Rows[i]["STOCK_OPER_ORDER"] = "30";
						bcls_stock_move.Tables["WM_STOCK"].Rows[i]["STOCK_NO"] = twm04["STOCK_NO"];
						bcls_stock_move.Tables["WM_STOCK"].Rows[i]["STOCK_PLACE_NO"] = twma7["STOCK_PLACE_NO_TO"];
						bcls_stock_move.Tables["WM_STOCK"].Rows[i]["ROWNO"] = " ";
						bcls_stock_move.Tables["WM_STOCK"].Rows[i]["COLUMN_NO"] = " ";
						bcls_stock_move.Tables["WM_STOCK"].Rows[i]["LAYERNO"] = 0;
						bcls_stock_move.Tables["WM_STOCK"].Rows[i]["STOCK_PLACE_POSITION"] = " ";
					}
					else{
						sprintf(s.msg, "%s", "行车移动类型出错！");
						doFlag = -1;
						throw CApplicationException(-1, s.msg, log.Location);
					}
				
					//行车命令确认发送垛位同步信息
					//bcls_rec_f.AddColName(1, "mat_no");
					//bcls_rec_f.AddColName(1, "stock_place_no");
					//bcls_rec_f.AddColName(1, "layerno");
					//bcls_rec_f.SetColVal(1, 1, "mat_no", tymsm10.mat_no);
					//bcls_rec_f.SetColVal(1, 1, "stock_place_no", tymsm10.targ_pos);
					//bcls_rec_f.SetColVal(1, 1, "layerno", tmmsm01.layerno);
					//doFlag = f_ym1900dd_snd(&bcls_rec_f, &bcls_ret_f);
					//if (doFlag != 0)
					//{
					//	bcls_ret_f.GetSYS(&s);
					//	//EDLog(1,1,"发送垛位同步f_ym1900dd_snd()失败 msg = [%s]",s.msg);
					//	//doFlag = -1;
					//	//goto l_return;
					//}
				}
				//else//如果是取消命令的话
				//{
				//	EDLog(1, 1, "进入命令取消流程");
				//	v_cmd_status = 0;
				//	if (0 == strcmp(tymsm10.comd_div, "2"))//针对板坯中转作业的行车命令
				//	{
				//		EXEC SQL update tymsm30 set cmd_flag = '0' where end_flag = '0' and mat_no = :tymsm10.mat_no;//行车命令标志，1：命令中，0：无命令    
				//	}
				//	else
				//	{
				//		EXEC SQL update tymsm01 set pre_mat_num = pre_mat_num - 1 where stock_place_no = :tymsm10.targ_pos;//更新目标垛位板坯预约块数
				//		EXEC SQL update tmmsm01 set cmd_flag = '0' where mat_position = '2' and mat_no = :tymsm10.mat_no;//行车命令标志，1：命令中，0：无命令
				//	}
				//}
			}
		}

		//增加对源垛位的处理  
		//EDLog(1, 1, "对源垛位进行分区属性维护");
		//if (0 > strcmp(tymsm10.source_pos, "600") && temp_flag == 0)//对中转命令不处理
		//{
		//	v_pile_assis_no = 0;
		//	v_pile_main_no = 0;
		//	strcpy(v_store_area, " ");
		//	EXEC SQL
		//		select slab_num_act, store_area, pile_assis_no, pile_main_no, pre_mat_num
		//	into : v_num_sour, : v_store_area, : v_pile_assis_no, : v_pile_main_no, : v_pre_mat_num
		//		   from tymsm01
		//	where stock_place_no = : tymsm10.source_pos;

		//	EDLog(1, 1, "垛位实际块数 = [%d]", v_num_sour);
		//	EDLog(1, 1, "垛位预约块数 = [%d]", v_pre_mat_num);

		//	if (v_num_sour < 0)//如果实际块数变为负数则默认为0
		//	{
		//		v_num_sour = 0;
		//		EXEC SQL
		//			update tymsm01
		//			set slab_num_act = 0,
		//			stock_status = '0'
		//		where stock_place_no = :tymsm10.source_pos;
		//	}
		//	if (v_pre_mat_num < 0)//如果预约块数变为负数则默认为0
		//	{
		//		EXEC SQL update tymsm01 set pre_mat_num = 0 where stock_place_no = :tymsm10.source_pos;
		//	}
		//	if ((v_num_sour + v_pre_mat_num) == 0)//如果该垛位无板坯
		//	{
		//		if (0 == strcmp(v_store_area, "2"))//如果该垛位属性被交换过则需要交换回来，同时将垛位分区更新时间初始化
		//		{
		//			EDLog(1, 1, "交换源垛位主辅助分区属性");
		//			EXEC SQL
		//				update tymsm01
		//				set pile_main_no = table_no,
		//				table_no = ' ',
		//				store_area = '1',
		//				fieldno_uptime = '20160101000000'
		//			where stock_place_no = :tymsm10.source_pos;
		//		}
		//		else//如果该垛位无板坯则将垛位分区更新时间初始化
		//		{
		//			EDLog(1, 1, "源垛位主分区更新时间初始化");
		//			EXEC SQL
		//				update tymsm01
		//				set fieldno_uptime = '20160101000000'
		//			where stock_place_no = :tymsm10.source_pos;
		//		}
		//	}
		//}

		////源垛位状态维护
		//if (temp_flag == 0)//对中转命令不处理
		//{
		//	EXEC SQL
		//		select count(1)
		//	into :v_commit_num
		//		  from tymsm10
		//	where source_pos = : tymsm10.source_pos and cmd_status = '0';

		//	EDLog(1, 1, "v_commit_num = %d", v_commit_num);
		//	EDLog(1, 1, "source_pos = %s", tymsm10.source_pos);

		//	if (v_commit_num == 0)
		//	{
		//		EDLog(1, 1, "无行车命令垛位，解锁垛位！");
		//		EXEC SQL update tymsm01 set stock_status = '0' where stock_place_no = :tymsm10.source_pos;
		//	}
		//}
//入库函数
		if (bcls_stock_in.Tables["WM_STOCK"].Rows.get_Count() > 0)
		{
			Log::Debug("", __FUNCTION__, "调用函数=[{0}]", "f_wmsmsm_stock_in");

			doFlag = f_wmsmsm_stock_in(&bcls_stock_in, bcls_ret, conn);

			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			Log::Debug("", __FUNCTION__, "----调用函数f_wmsmsm_stock_in结束---");
		}
	//倒垛函数	
		if (bcls_stock_move.Tables["WM_STOCK"].Rows.get_Count() > 0)
		{
			Log::Debug("", __FUNCTION__, "调用函数=[{0}]", "f_wmsmsm_stock_move");

			doFlag = f_wmsmsm_stock_move(&bcls_stock_move, bcls_ret, conn);

			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			Log::Debug("", __FUNCTION__, "----调用函数f_wmsmsm_stock_move结束---");
		}
		

		Log::Debug("", __FUNCTION__, "wmsmsmj3_crane--------结束");

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
