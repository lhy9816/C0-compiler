#pragma once
#include "Compiler.h"
const int MaxErrorType = 100;

enum errorType {
	// �ʷ�����
	SQMISMATCH = 0, DQMISMATCH, INVALIDSQCHAR, INVALIDDQCHAR, LACKEQL, INVALIDGLOCHAR,
	// �﷨����
	ILLEGALVARTYPE, LACKSEMICOLON, NOINTORCHAR, 

	// ���ʹ���
	NOMEANING,
};


