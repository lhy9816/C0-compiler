#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm> 
#include <stack>
#include <set>
#include <time.h>
#include <stdlib.h>
#include "error.h"
#include "lexical.h"
#include "symbolitem.h"
#include "grammar.h"
#include "gen_midcode.h"
#include "gen_objcode.h"
#include "optimizer.h"
#define OPT (1)
using namespace std;

const int MaxReserve = 15;
const int MaxOp = 22;
const int MaxUsr = 4;
const int MaxInt = 75536;			// 无符号整数最大上界

class FuncItem;
class Item;

class Compiler
{
public:
	Compiler(char *path);
	// 读入文件函数
	string readFiletoString(char *str);
	/* ---------大佬测试集--------- */
	string dalao = "大佬测试集/16061006/";
	// 编译开始函数		(main !!)
	void begin();

	~Compiler();

	

private:
	
	/* ---------词法分析相关--------- */
	string origin_text;			// 文件读入字符串中
	char ch;					// 当前读入字符
	string strToken;			// 已经读入的一类字符串
	int lineNum;				// 读入源程序的行数
	int intgNum;				// 读入为数字时的整形数字
	int pin;					// 读入文件时的字符pin
	int tok_idx;				// 当前文件的已经识别出的tok序数
	//int ret;					// 判断返回值从而进行分类(标识符与保留字)
	tokenType curToken;			// 当前识别出的token的类别序号		

	string reserve[MaxReserve];
	string op[MaxOp];
	string usr[MaxUsr];
	int error_count;

	void wordSetup();
	void otptwordSetup();
	void lexicalSetup();
	void getNextchar();
	void skipSpace();
	void toLower();
	bool isLetter();
	bool isLetter(string name);
	bool isDigit();
	bool isChar();
	bool isVarType();
	tokenType searchReserve(string s);
	tokenType searchOp(string s);
	int transtoNum();
	void catToken();
	void reTract();
	int getSym();

	/* ---------语法分析相关--------- */
	bool have_main;
	int tmp_reg_idx;

	//bool find_in_set(const tokenType set[],const int n);
	template<typename T1, typename T2, typename T3>
	bool find_in_sets(T1 set, T2 token, T3 n);

	void program();
	void const_declare();
	void const_def();									// 将填符号表的工作放这里
	void var_declare();									
	void var_def(string name, tokenType retType);		// 将填符号表的工作放这里
	void statelist();
	void statement();
	void compoundstat();

	void fun_call(FuncItem* it);
	void fun_def();
	void void_iden_fun(string name, tokenType retType);
	void ret_iden_fun(string name, tokenType retType);
	void main_fun(string name, tokenType retType);

	void assign_st(string* name);
	void if_st();
	void cond(bool* certain, int* value, string* name);
	void do_while_st();
	void switch_st();
	void one_case(tokenType cal, string* end_label, string* next_label,string* tmp_name);
	void read_st();
	void write_st();
	void return_st();

	void paralist(string name, tokenType retType);
	void declare_head(string* name, tokenType* retType);
	tokenType expr(bool* is_const, int* value, string* tmp_name);
	tokenType item(bool* is_const, int* value, string* tmp_name);
	tokenType factor(bool* is_const, int* value, string* tmp_name);
	void integer(int* value);

	bool checkType(tokenType id, tokenType token);

	/* ---------错误处理相关--------- */
	// 出错的行的第一个字符与最后一个字符
	int firstinLine, lastinLine;
	string *errorMsg[MaxErrorType];
	void errorManipulate(errorType id, string s = "default");
	void errorSkip(errorType id);
	void skip(const tokenType* kind, int n);
	void skip(tokenType kind);
	void errorSetup();

	/* -------符号表相关----------*/
	// 建立符号表，一个全局常量符号表，一个全局变量符号表
	map<string, Item*>global_const_tab;
	map<string, Item*>global_var_tab;
	map<string, FuncItem*>fun_tab;
	// 当前活跃函数符号表，上次活跃函数符号表(用于生成mips代码时跳回)
	FuncItem* cur_active_funtab;
	FuncItem* last_active_funtab;
	stack<FuncItem*> fun_stack;
	// 字符串vector表
	vector<string>string_tab;
	int string_idx;

	string* int2string(std::string *s, int value);

	bool check_defined(string name);
	Item * get_const_var_item(string name);
	FuncItem * get_func_item(string name);
	// 登入符号表 var中的-1说明只定义未赋值
	void put_const(string name, tokenType kind, int value);
	void put_var(string name, tokenType kind, int value, int length);
	void put_func(string name, Genre genre, tokenType kind, int paranum);
	void print_symbol_item();

	/* -------中间代码相关----------*/
	vector<Midcode> mid_list;
	int global_var_offset;								// 全局变量的偏移量
	int label_idx;
	void gen_tmp_name(string* name);
	void gen_string_name(string * name);
	void gen_label_name(string * name);
	Midcode* push_mid_code(mid_op M, string * src1, string * src2, string * dst);
	void print_mid_code(Midcode* mid);
	void print_mid_code_after_dag(Midcode* mid);
	bool isVarType(tokenType type);
	vector<string> mid_sout;

	/* -------生成代码相关----------*/
#if OPT
	int para_offset;
	stack<int> para_offset_stack;
	vector<Mipscode> mips_list;
	RegRecord* regs;
	void generateMips();
	void gen_mips_code(string* op, string* dst, string* src1, string* src2);
	void asciiz_setup();
	void simu_reg_alloc(vector<int>* reg);
	int find_func_idx(string name);
	void init_reg(RegRecord* reg_list);
	void get_reg(string* reg_name, RegType* reg_kind, string* name, bool* hit);
	void get_res_reg(string* reg_name, RegType* reg_kind, string* name, bool* hit);
	void get_arr_reg(string* reg_name, RegType* reg_kind, string* name);
	void put_in_reg(string* reg_name, string* var_name);
	void put_in_memory(string* reg_name, string* var_name);
	string get_ts_reg(int n, RegType* rg);
	void handle_init(Midcode* mid, int i);
	void handle_push(Midcode* mid, int i);
	void handle_call(Midcode* mid, int i);
	void handle_branch(Midcode* mid, int i);
	void handle_jmp(Midcode* mid, int i);
	void handle_scanf(Midcode* mid, int i);
	void handle_printf(Midcode* mid, int i);
	void handle_return(Midcode* mid, int i);
	void handle_operation(Midcode* mid, int i);

	void handle_label(string* label);
	void handle_assign(Midcode* mid, int i);
	void handle_arrto(Midcode* mid, int i);
	void handle_arrget(Midcode* mid, int i);

	void handle_exit();

	bool reg_is_const(string* name);
	
	void clear_tmp_reg(int i);
#else
	/*old*/
	int para_offset;
	stack<int> para_offset_stack;
	void generateMips();
	void gen_mips_code(string* op, string* dst, string* src1, string* src2);
	void asciiz_setup();
	void simu_reg_alloc(vector<int>* reg);
	void init_reg(RegRecord* reg_list);
	void get_reg(string* reg_name, RegType* reg_kind, string* name, bool* hit);
	void get_reg(string* reg_name, RegType* reg_kind, string* name, string* idx);
	void put_in_reg(string* reg_name, string* var_name);
	void put_in_memory(string* reg_name, string* var_name);
	string get_ts_reg(int n, RegType* rg);
	void handle_init(Midcode* mid);
	void handle_push(Midcode* mid);
	void handle_call(Midcode* mid);
	void handle_branch(Midcode* mid);
	void handle_jmp(Midcode* mid);
	void handle_scanf(Midcode* mid);
	void handle_printf(Midcode* mid);
	void handle_return(Midcode* mid);
	void handle_operation(Midcode* mid);

	void handle_label(string* label);
	void handle_assign(Midcode* mid);
	void handle_arrto(Midcode* mid);
	void handle_arrget(Midcode* mid);

	void handle_exit();

	bool reg_is_const(string* name);
	bool reg_is_reg(string* name);
	bool reg_is_ret(string* name);
#endif
	/* -------优化相关----------*/
	Blocks code_blocks[MaxCodeBlocks];
	RegrefTab reg_ref_tab;
	funRegref* cur_fun_reg_ref;
	int code_blocks_idx;
	void read_mips_list();
	void dag_init();
	void dag_manipulate();
	int find_dag_list(dagNodelist** dnl, int length, string name, dagNodelist* node);
	int dag_list_num;
	int dag_node_num;
	bool*** outflow;
	bool*** def;
	int** cur_alloc_reg;
	funBlocks fun_blocks[MaxFunNum];
	int fun_blocks_idx;
	conflictGraph conflict_nodes[MaxFunNum][MaxCflNodes];
	int cflt_idx[MaxFunNum];
	int global_reg_num;										// 可以做成超参数
	//void dot_init();
	string int22string(int a);
	void print_item_dot(string* name, string* lines);
	void print_link_dot(string*a, string*b);
	void print_item_livedot(string* name, string* lines);
	void print_link_livedot(string*a, string*b);
	void print_item_dagpicdot(string* name, string* lines);
	void print_link_dagpicdot(string*a, string*b);
	void peephole_optimization();
	int find_var_offset(string name, funRegref* frr);		// 找到某函数中对应变量的偏移
	void data_flow_analysis();
	void reg_alloc_init();
	void ref_count_begin(int begin, int end);
	void var_count_alloc(string name);
	void ref_count_regalloc();
	void const_propagation();
};

