#pragma once
#include "Compiler.h"

// ��Ԫʽ������

enum mid_op {
	// �����
	ADD_M, SUB_M, MUL_M, DIV_M,
	// ��ֵ��
	ASS_M, NAS_M, ARGET_M, ARTO_M,
	// ��ת��
	BGJ_M, BGEJ_M, LSJ_M, LSEJ_M, EQJ_M, NEQJ_M, LABEL_M, J_M,
	// ��������������
	CONST_M, VARS_M, VARA_M,
	// ��д��
	SCANF_M, PRINTS_M, PRINTV_M, 
	// ������
	CALL_M, FUN_M, PUSH_M, RETV_M, RET_M, GET_M, EXIT_M

};

// ��Ԫʽ����ṹ��
typedef struct {
	mid_op mid_op;
	std::string* src1;
	std::string* src2;
	std::string* dst;
	bool is_print;
} Midcode;