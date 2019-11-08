#pragma once
#include "Compiler.h"

// 四元式操作符

enum mid_op {
	// 运算符
	ADD_M, SUB_M, MUL_M, DIV_M,
	// 赋值类
	ASS_M, NAS_M, ARGET_M, ARTO_M,
	// 跳转类
	BGJ_M, BGEJ_M, LSJ_M, LSEJ_M, EQJ_M, NEQJ_M, LABEL_M, J_M,
	// 常量变量定义类
	CONST_M, VARS_M, VARA_M,
	// 读写类
	SCANF_M, PRINTS_M, PRINTV_M, 
	// 函数类
	CALL_M, FUN_M, PUSH_M, RETV_M, RET_M, GET_M, EXIT_M

};

// 四元式保存结构体
typedef struct {
	mid_op mid_op;
	std::string* src1;
	std::string* src2;
	std::string* dst;
	bool is_print;
} Midcode;