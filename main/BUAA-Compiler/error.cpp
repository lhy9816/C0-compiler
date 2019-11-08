#include "Compiler.h"

#define DEBUG (1)
#if DEBUG
#define ERROR_OUT cout
#else
#define ERROR_OUT errout
#endif //DEBUG
// 错误处理语句

std::string SQMISMATCHMSG = "\'无法匹配";
std::string DQMISMATCHMSG = "字符串的\"无法匹配";
std::string INVALIDSQCHARMSG = "\'和\'中包含的字符不合文法";
std::string INVALIDDQCHARMSG = "\"和\"中包含的字符不合文法";

std::string LACKEQLMSG = "!后面没出现=";
string INVALIDGLOCHARMSG = "出现文法规定之外的终结符";
string ILLEGALVARTYPEMSG = "不合法的类型！";
string LACKSEMICOLONMSG = "语句结尾缺少 \';\'";
string NOINTORCHARMSG = "期望给出类型int或char，但是没有找到 ！";

ofstream errout("output/error_out.txt");

// 错误处理函数
void Compiler::errorManipulate(errorType id, string s) {
	// 换行符的个数统计
	int count = 0;
	this->firstinLine = 0;
	this->error_count++;
	// 找到出错的行并输出该行
	for (int i = 0;i < origin_text.size();i++) {
		if (count == lineNum) {
			while (origin_text[i] != '\n' && origin_text[i] != '\0') {
				i++;
			}
			lastinLine = i - 1;
			break;
		}
		if (origin_text[i] == '\n') {
			firstinLine = i + 1;
			count++;
		}
	}
	// 输出该行
	ERROR_OUT << "ERROR in line " << lineNum + 1 << " : " << endl;
	ERROR_OUT << "---> ";
	for (int i = firstinLine;i <= lastinLine;i++)
		ERROR_OUT << origin_text[i];
	ERROR_OUT << endl;
	// 输出错误
	ERROR_OUT << "---> " << ((s == "default")? (*errorMsg[id]): s) << endl;
}



// 跳读对应符号
void Compiler::skip(const tokenType* kind, int n) {
	//int n = sizeof(kind) / sizeof(tokenType);
	int flag;
	while (true) {
		flag = 0;
		this->getSym();
		for (int i = 0;i < n;i++) {
			if (kind[i] == curToken) {
				flag = 1; break;
			}
		}
		if (flag)
			break;
	}
}

void Compiler::skip(tokenType kind){
	do {
		this->getSym();
	} while (this->curToken != kind);
}


// 对应错误跳读符号
void Compiler::errorSkip(errorType id) {
	
}

void Compiler::errorSetup() {
	this->errorMsg[SQMISMATCH] = new string(SQMISMATCHMSG);
	this->errorMsg[DQMISMATCH] = new string(DQMISMATCHMSG);
	this->errorMsg[INVALIDSQCHAR] = new string(INVALIDSQCHARMSG);
	this->errorMsg[INVALIDDQCHAR] = new string(INVALIDDQCHARMSG);
	this->errorMsg[LACKEQL] = new string(LACKEQLMSG);
	this->errorMsg[INVALIDGLOCHAR] = new string(INVALIDGLOCHARMSG);
	this->errorMsg[ILLEGALVARTYPE] = new string(ILLEGALVARTYPEMSG);
	this->errorMsg[LACKSEMICOLON] = new string(LACKSEMICOLONMSG);
	this->errorMsg[NOINTORCHAR] = new string(NOINTORCHARMSG);
	//...'=
	this->error_count = 0;
}