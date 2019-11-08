#pragma once
#include "Compiler.h"

const int MaxAvaiReg = (18-2);

typedef struct {
	std::string name;	//占用寄存器的名字
	bool occupied;		//是否被全局寄存器占用
	bool available;		//当前寄存器是否可用
	bool global;		//当前寄存器是否被分给全局变量
	int offset;			//当前分配变量在函数的位置
	int idx;			//数组变量指示下标
	bool first_hit;		//给全局寄存器
}RegRecord;

enum RegType {
	CONST_RG, RET_RG, T_RG, S_RG, NFIND, FIND,
};

// mips代码保存结构体
typedef struct {
	mid_op mid_op;
	std::string* src1;
	std::string* src2;
	std::string* dst;
} Mipscode;