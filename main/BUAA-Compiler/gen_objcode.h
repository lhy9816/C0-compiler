#pragma once
#include "Compiler.h"

const int MaxAvaiReg = (18-2);

typedef struct {
	std::string name;	//ռ�üĴ���������
	bool occupied;		//�Ƿ�ȫ�ּĴ���ռ��
	bool available;		//��ǰ�Ĵ����Ƿ����
	bool global;		//��ǰ�Ĵ����Ƿ񱻷ָ�ȫ�ֱ���
	int offset;			//��ǰ��������ں�����λ��
	int idx;			//�������ָʾ�±�
	bool first_hit;		//��ȫ�ּĴ���
}RegRecord;

enum RegType {
	CONST_RG, RET_RG, T_RG, S_RG, NFIND, FIND,
};

// mips���뱣��ṹ��
typedef struct {
	mid_op mid_op;
	std::string* src1;
	std::string* src2;
	std::string* dst;
} Mipscode;