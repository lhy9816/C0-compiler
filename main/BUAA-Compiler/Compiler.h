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
const int MaxInt = 75536;			// �޷�����������Ͻ�

class FuncItem;
class Item;

class Compiler
{
public:
	Compiler(char *path);
	// �����ļ�����
	string readFiletoString(char *str);
	/* ---------���в��Լ�--------- */
	string dalao = "���в��Լ�/16061006/";
	// ���뿪ʼ����		(main !!)
	void begin();

	~Compiler();

	

private:
	
	/* ---------�ʷ��������--------- */
	string origin_text;			// �ļ������ַ�����
	char ch;					// ��ǰ�����ַ�
	string strToken;			// �Ѿ������һ���ַ���
	int lineNum;				// ����Դ���������
	int intgNum;				// ����Ϊ����ʱ����������
	int pin;					// �����ļ�ʱ���ַ�pin
	int tok_idx;				// ��ǰ�ļ����Ѿ�ʶ�����tok����
	//int ret;					// �жϷ���ֵ�Ӷ����з���(��ʶ���뱣����)
	tokenType curToken;			// ��ǰʶ�����token��������		

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

	/* ---------�﷨�������--------- */
	bool have_main;
	int tmp_reg_idx;

	//bool find_in_set(const tokenType set[],const int n);
	template<typename T1, typename T2, typename T3>
	bool find_in_sets(T1 set, T2 token, T3 n);

	void program();
	void const_declare();
	void const_def();									// ������ű�Ĺ���������
	void var_declare();									
	void var_def(string name, tokenType retType);		// ������ű�Ĺ���������
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

	/* ---------���������--------- */
	// ������еĵ�һ���ַ������һ���ַ�
	int firstinLine, lastinLine;
	string *errorMsg[MaxErrorType];
	void errorManipulate(errorType id, string s = "default");
	void errorSkip(errorType id);
	void skip(const tokenType* kind, int n);
	void skip(tokenType kind);
	void errorSetup();

	/* -------���ű����----------*/
	// �������ű�һ��ȫ�ֳ������ű�һ��ȫ�ֱ������ű�
	map<string, Item*>global_const_tab;
	map<string, Item*>global_var_tab;
	map<string, FuncItem*>fun_tab;
	// ��ǰ��Ծ�������ű��ϴλ�Ծ�������ű�(��������mips����ʱ����)
	FuncItem* cur_active_funtab;
	FuncItem* last_active_funtab;
	stack<FuncItem*> fun_stack;
	// �ַ���vector��
	vector<string>string_tab;
	int string_idx;

	string* int2string(std::string *s, int value);

	bool check_defined(string name);
	Item * get_const_var_item(string name);
	FuncItem * get_func_item(string name);
	// ������ű� var�е�-1˵��ֻ����δ��ֵ
	void put_const(string name, tokenType kind, int value);
	void put_var(string name, tokenType kind, int value, int length);
	void put_func(string name, Genre genre, tokenType kind, int paranum);
	void print_symbol_item();

	/* -------�м�������----------*/
	vector<Midcode> mid_list;
	int global_var_offset;								// ȫ�ֱ�����ƫ����
	int label_idx;
	void gen_tmp_name(string* name);
	void gen_string_name(string * name);
	void gen_label_name(string * name);
	Midcode* push_mid_code(mid_op M, string * src1, string * src2, string * dst);
	void print_mid_code(Midcode* mid);
	void print_mid_code_after_dag(Midcode* mid);
	bool isVarType(tokenType type);
	vector<string> mid_sout;

	/* -------���ɴ������----------*/
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
	/* -------�Ż����----------*/
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
	int global_reg_num;										// �������ɳ�����
	//void dot_init();
	string int22string(int a);
	void print_item_dot(string* name, string* lines);
	void print_link_dot(string*a, string*b);
	void print_item_livedot(string* name, string* lines);
	void print_link_livedot(string*a, string*b);
	void print_item_dagpicdot(string* name, string* lines);
	void print_link_dagpicdot(string*a, string*b);
	void peephole_optimization();
	int find_var_offset(string name, funRegref* frr);		// �ҵ�ĳ�����ж�Ӧ������ƫ��
	void data_flow_analysis();
	void reg_alloc_init();
	void ref_count_begin(int begin, int end);
	void var_count_alloc(string name);
	void ref_count_regalloc();
	void const_propagation();
};

