#pragma once
#include "Compiler.h"
// ���ʵ�ö�ٱ�������



//const int MaxReserve = 15;
//const int MaxOp = 22;

enum tokenType {
	// ����ֹͣ����
	NONE = -1,
	// ������ ��15��
	MAINTK = 0, CONSTTK, INTTK, CHARTK, VOIDTK, IFTK, ELSETK, DOTK, WHILETK,
	SWITCHTK, CASETK, DEFAULTTK, SCANFTK, PRINTFTK, RETURNTK,
	// �ֽ�������������22��
	ASSIGNSY, PLUSSY, SUBSY, MULTSY, DIVSY, LESSY, LESEQSY, GRTSY,
	GRTEQSY, NEQUALSY, EQUALSY, COMMASY, COLONSY, SEMICOLONSY, LSBRASY,
	RSBRASY, LMBRASY, RMBRASY, LBBRASY, RBBRASY, SIGQUOSY, DUBQUOSY,
	// �û������ʶ������4��
	IDEN, STRINGUSR, CHARUSR, NUMUSR,
	
};


