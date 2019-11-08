#pragma once
#include "Compiler.h"
const int MaxErrorType = 100;

enum errorType {
	// 词法分析
	SQMISMATCH = 0, DQMISMATCH, INVALIDSQCHAR, INVALIDDQCHAR, LACKEQL, INVALIDGLOCHAR,
	// 语法分析
	ILLEGALVARTYPE, LACKSEMICOLON, NOINTORCHAR, 

	// 打酱油凑数
	NOMEANING,
};


