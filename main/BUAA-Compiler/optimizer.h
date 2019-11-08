#pragma once
#include "Compiler.h"

const int MaxCodeBlocks = 400;
const int LHY = -16;
const int MaxFunNum = 100;
const int MaxCflNodes = 100;
const int MaxDagNodes = 500;

typedef struct {
	std::string name;				// 当前块名称
	std::string j_name;				// 目标块名称
	bool is_print;					// 需要打印吗？
	int idx;						// 当前基本块的序号
	int first_line_num;				// 基本块第一行代码
	int last_line_num;				// 基本块最后一行代码
	int pre_blocks[MaxCodeBlocks];	// 前面指向本块的block的序号
	int pre_blocks_cnt;				// 前面指向本块的count
	int follow_block;				// 直接后继块的序号
	int j_block;					// 跳转后继块的序号
}Blocks;


typedef struct {
	int begin_block_line;
	int end_block_line;
	int begin_block;
	int end_block;
	std::string name;
}funBlocks;

// 引用计数法数据结构
typedef struct {
	std::string name;
	int ref_count;
	bool global;
}varRegref;

typedef std::vector<varRegref> funRegref;
typedef std::map<std::string, funRegref> RegrefTab;

// 冲突图
typedef struct {
	int join[1000];
	int join_num;
	bool connect[1000];
}conflictGraph;

// DAG节点表
typedef struct {
	std::string name;			// 结点名称			
	int idx;					// dag图中序数
	bool modified;				// 表示该结点对应的符号是否被重新定义过
}dagNodelist;

typedef struct {
	std::string name;					// 结点名称			
	std::string leaf_ass_name;
	mid_op op;							// 结点运算符
	int idx;							// 结点在dag图中位置
	int value;							// 判断该语句的值可否确定,若是参数则代表参数的idx
	int child_1;
	int child_2;
	int prt_num;
	std::set<std::string> name_candidates;// 历史定义过的名字集

	bool isLeaf;						// 是否是叶节点
}dagNode;
