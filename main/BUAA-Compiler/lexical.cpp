#include "Compiler.h"

#define DEBUG_LEXI (0)
#if DEBUG_LEXI
#define LEXI_OUT cout
#else
#define LEXI_OUT fout
#endif //DEBUG_LEXI

/*******************************************************************************\
 *																			   *
 *								词法分析实现函数							   *
 *																			   *
\*******************************************************************************/

// 词法分析目标作业目标位置输出
ofstream fout("output/lexi_out.txt");

// 用于词法分析的输出
std::string symkey[MaxReserve] = {							//保留字具体类型
	"MAINTK", "CONSTTK", "INTTK", "CHARTK", "VOIDTK", "IFTK", "ELSETK", "DOTK", "WHILETK",
	"SWITCHTK", "CASETK", "DEFAULTTK", "SCANFTK", "PRINTFYK", "RETURNTK"
};
std::string symop[MaxOp]{								//分界符或操作符符号类型
	"ASSIGNSY", "PLUSSY", "SUBSY", "MULTSY", "DIVSY", "LESSY", "LESEQSY", "GRTSY",
	"GRTEQSY", "NEQUALSY", "EQUALSY", "COMMASY", "COLONSY", "SEMICOLONSY", "LSBRASY",
	"RSBRASY", "LMBRASY", "RMBRASY", "LBBRASY", "RBBRASY", "SIGQUOSY", "DUBQUOSY"
};

// 用于错误转换

void Compiler::wordSetup() {
	this->reserve[MAINTK] = "main";
	this->reserve[CONSTTK] = "const";
	this->reserve[INTTK] = "int";
	this->reserve[CHARTK] = "char";
	this->reserve[VOIDTK] = "void";
	this->reserve[IFTK] = "if";
	this->reserve[ELSETK] = "else";
	this->reserve[DOTK] = "do";
	this->reserve[WHILETK] = "while";
	this->reserve[SWITCHTK] = "switch";
	this->reserve[CASETK] = "case";
	this->reserve[DEFAULTTK] = "default";
	this->reserve[SCANFTK] = "scanf";
	this->reserve[PRINTFTK] = "printf";
	this->reserve[RETURNTK] = "return";

	this->op[ASSIGNSY - MaxReserve] = "=";
	this->op[PLUSSY - MaxReserve] = "+";
	this->op[SUBSY - MaxReserve] = "-";
	this->op[MULTSY - MaxReserve] = "*";
	this->op[DIVSY - MaxReserve] = "/";
	this->op[LESSY - MaxReserve] = "<";
	this->op[LESEQSY - MaxReserve] = "<=";
	this->op[GRTSY - MaxReserve] = ">";
	this->op[GRTEQSY - MaxReserve] = ">=";
	this->op[NEQUALSY - MaxReserve] = "!=";
	this->op[EQUALSY - MaxReserve] = "==";
	this->op[COMMASY - MaxReserve] = ",";
	this->op[COLONSY - MaxReserve] = ":";
	this->op[SEMICOLONSY - MaxReserve] = ";";
	this->op[LSBRASY - MaxReserve] = "(";
	this->op[RSBRASY - MaxReserve] = ")";
	this->op[LMBRASY - MaxReserve] = "[";
	this->op[RMBRASY - MaxReserve] = "]";
	this->op[LBBRASY - MaxReserve] = "{";
	this->op[RBBRASY - MaxReserve] = "}";
	this->op[SIGQUOSY - MaxReserve] = "\'";
	this->op[DUBQUOSY - MaxReserve] = "\"";

	this->usr[IDEN - MaxReserve - MaxOp] = "标识符";
	this->usr[STRINGUSR - MaxReserve - MaxOp] = "字符串";
	this->usr[CHARUSR - MaxReserve - MaxOp] = "字符";
	this->usr[NUMUSR - MaxReserve - MaxOp] = "整型数字";

}

// 用直接定义的symkey 与 symop 代替了
void Compiler::otptwordSetup() {

}

void Compiler::lexicalSetup() {
	this->ch = ' ';					// 当前读入字符
	this->strToken = "";			// 已经读入的一类字符串
	this->lineNum = 0;				// 读入源程序的行数
	this->intgNum = 0;				// 读入为数字时的整形数字
	this->pin = 0;					// 读入文件时的字符pin
	this->tok_idx = 0;				// 当前文件的已经识别出的tok序数
	
	this->curToken;
	this->wordSetup();				// 词法tokenType对应setup
	this->otptwordSetup();			// 词法输出表建立
	// 加两个语法的setup
	this->have_main = false;
	this->tmp_reg_idx = 0;
	this->string_idx = 0;
	this->label_idx = 0;
	this->global_var_offset = 0;
	this->cur_active_funtab = nullptr;
}

void Compiler::getNextchar() {
	this->ch = this->origin_text[this->pin++];
}

void Compiler::skipSpace() {
	while (this->ch == ' ' || this->ch == '\t' || this->ch == '\n') {
		if (this->ch == '\n')
			this->lineNum++;
		this->getNextchar();
	}
}

void Compiler::toLower() {
	for (unsigned int i = 0; i < this->strToken.size();++i) {
		if (isupper(this->strToken[i]))
			this->strToken[i] = this->strToken[i] - 'A' + 'a';
	}
	return;
}

bool Compiler::isLetter() {
	return (isupper(this->ch) || islower(this->ch) || this->ch == '_');
}

bool Compiler::isLetter(string name) {
	return (isupper(name[0]) || islower(name[0]) || name[0] == '_');
}

bool Compiler::isDigit() {
	return (ch >= '0'&&ch <= '9');
}

bool Compiler::isChar() {
	return(ch == '+' || ch == '-' || ch == '*' || ch == '/' || isLetter() || isDigit());
}

bool Compiler::isVarType() {
	return (this->curToken == INTTK || this->curToken == CHARTK);
}

tokenType Compiler::searchReserve(string strToken) {
	for (int i = 0; i < MaxReserve; i++) {
		if (strToken == reserve[i])
			return tokenType(i);
	}
	return NONE;
}

tokenType Compiler::searchOp(string strToken) {
	for (int i = 0; i < MaxOp; i++) {
		if (strToken == op[i])
			return tokenType(i+MaxReserve);
	}
	return NONE;
}

int Compiler::transtoNum() {
	return atoi(this->strToken.c_str());
}

void Compiler::catToken() {
	this->strToken += this->ch;
}

void Compiler::reTract() {
	--this->pin;
	this->ch = ' ';
}

int Compiler::getSym() {
	this->strToken = "";								// 初始化
	this->intgNum = 0;
	tokenType ret;										// 返回具体的搜寻类型
	getNextchar();
	skipSpace();
	// 识别字母
	if (isLetter()) {
		do {
			catToken();
			getNextchar();
		} while (isLetter() || isDigit());		//读入所有的连续字母数字
		if (ch == '\'' || ch == '\"') {
			if (ch == '\"') {
				errorManipulate(DQMISMATCH);
			}
			else if (ch == '\''&&strToken.size() > 1) {
				errorManipulate(DQMISMATCH);
			}
			else {
				errorManipulate(SQMISMATCH);	//左侧单引号缺失;
			}
			//return -1;
		}
		reTract();								//回退多读的字符
		toLower();								//将strToken转化为小写，不区分大小写
		ret = searchReserve(strToken);				// 当前token是否为保留字
		if (ret >= 0) {
			LEXI_OUT << ++tok_idx << ' ' << symkey[ret] << ' ' << strToken << endl;
			curToken = ret;
		}
		else {									// 是标识符，查重的事情交给符号表
			LEXI_OUT << ++tok_idx << " IDEN " << strToken << endl;
			curToken = IDEN;
		}
	}
	// 识别数字
	else if (isDigit()) {
		do {
			catToken();
			getNextchar();
		} while (isDigit());					// 读入所有连续的数字字符
		/*if ((searchOpforChar(ch) < 0 && ch != ' ') || ch == '\'' || ch == '\"' || ch == '(') {		//不符合数字结尾情况，报错并退出当前分析
			//LEXI_OUT << "WRONG END IN NUM !" << endl;
			errorManipulate(0, lineNum, text);
			while (ch != ','&&ch != ';'&&ch != ';'&&ch != '\n'&&ch != '\t'&&ch != '\0') {
				getNextchar();
			}
			reTract();
			return -1;
		}*/
		reTract();								// 回退最终字符
		intgNum = transtoNum();		// 获得对应字符的整型数
		curToken = NUMUSR;
		LEXI_OUT << ++tok_idx << " NUMUSR " << intgNum << endl;
	}
	// 识别字符
	else if (ch == '\'') {
		//LEXI_OUT << ++tok_idx << " SIGQUOSY " << ch << endl;	// 输出单引号类型（前）
		//curToken = SIGQUOSY;
		/* ---------------------------------- 这里有一个疑问，我到底要不要返回一个类别符给语法分析？？，后面字符串那里同理 ------------------------------------- */
		//catToken();
		getNextchar();									// 读入引号内部的字符
		if (isChar()) {
			catToken();
			getNextchar();
			if (ch == '\'') {
				//catToken();
				//getNextchar();
				LEXI_OUT << ++tok_idx << " CHARUSR " << strToken << endl;					// 输出用户自定义字符标志
				curToken = CHARUSR;
				//LEXI_OUT << ++tok_idx << " SIGQUOSY " << ch << endl;						// 输出单引号类型（后）
				//curToken = SIGQUOSY;												// 这个要是变成单引号的类型就会覆盖上一个字符类型了，绝对不可取啊
			}
			else {
				//LEXI_OUT << "WRONG END FOR CHAR ! " << endl;
				errorManipulate(SQMISMATCH);
				while (ch != ','&&ch != ';'&&ch != ';'&&ch != '\n'&&ch != '\t'&&ch != '\0') {
					getNextchar();
				}
				reTract();
				return (-1);								// wrong end for char
			}
		}
		else {
			//LEXI_OUT << "NOT A REGULATED CHAR BETWEEN SIGQUOTE" << endl;
			errorManipulate(INVALIDSQCHAR);
			while (ch != ','&&ch != ';'&&ch != ';'&&ch != '\n'&&ch != '\t'&&ch != '\''&&ch != '\0') {
				getNextchar();
			}
			if (ch != '\'')
				reTract();
			return (-1);
		}
		// wrong end for char
// << ++tok_idx << " SIGQUOTE " << ch << endl;	// 输出单引号类型(后)
	}
	// 识别字符串
	else if (ch == '\"') {
		getNextchar();
		while (ch != '\"') {
			if (ch == '\n') {
				errorManipulate(NOMEANING, "字符串的\"未匹配！");
				break;
			}
			if (ch == 32 || ch == 33 || ch >= 35 && ch <= 126) {			// 字符串非空而且里面字符合法
				catToken();
				getNextchar();
			}
			else {
				//LEXI_OUT << "UNEXPECTED CHAR IN STRING!" << endl;			// 字符串非空但是里面字符不合法
				errorManipulate(INVALIDDQCHAR);
				while (ch != ','&&ch != ';'&&ch != ';'&&ch != '\n'&&ch != '\t'&&ch != '\"'&&ch != '\0') {
					getNextchar();
				}
				if (ch != '\"')
					reTract();
				return (-1);
			}
		}
		LEXI_OUT << ++tok_idx << " STRINGUSR " << strToken << endl;
		curToken = STRINGUSR;
	}
	// 识别会有歧义的标识符
	else if (ch == '=') {
		catToken();
		getNextchar();
		// is "=="
		if (ch == '=') {
			catToken();
			LEXI_OUT << ++tok_idx << " EQUALSY " << strToken << endl;
			curToken = EQUALSY;
		}
		// is "="
		else {
			reTract();
			LEXI_OUT << ++tok_idx << " ASSIGNSY " << strToken << endl;
			curToken = ASSIGNSY;
		}
	}
	else if (ch == '<') {
		catToken();
		getNextchar();
		// is "<="
		if (ch == '=') {
			catToken();
			LEXI_OUT << ++tok_idx << " LESEQSY " << strToken << endl;
			curToken = LESEQSY;
		}
		else {
			reTract();
			LEXI_OUT << ++tok_idx << " LESSY " << strToken << endl;
			curToken = LESSY;
		}
	}
	else if (ch == '>') {
		catToken();
		getNextchar();
		// is ">="
		if (ch == '=') {
			catToken();
			LEXI_OUT << ++tok_idx << " GRTEQSY " << strToken << endl;
			curToken = GRTEQSY;
		}
		else {
			reTract();
			LEXI_OUT << ++tok_idx << " GRTSY " << strToken << endl;
			curToken = GRTSY;
		}
	}
	else if (ch == '!') {
		catToken();
		getNextchar();
		// is "!="
		if (ch == '=') {
			catToken();
			LEXI_OUT << ++tok_idx << " NEQUALSY " << strToken << endl;
			curToken = NEQUALSY;
		}
		else {
			reTract();
			errorManipulate(LACKEQL);
			// 不等号这个应该只会改错
			//LEXI_OUT << "UNKNOWN CHAR AFTER '!'" << endl;
			return (-1);
		}
	}
	else {
		catToken();
		if ((ret = searchOp(strToken)) > 0) {
			//catToken();
			LEXI_OUT << ++tok_idx << ' ' << symop[ret-MaxReserve] << ' ' << strToken << endl;
			curToken = ret;
		}
		else {
			if (ch == '\0')						// 程序结束，需要退出
			{
				cout << " PROGRAM READ COMPLETE !" << endl;
				return 0;
			}
			else {
				//LEXI_OUT << "ILLEGAL CHAR APPEARS !" << endl;
				errorManipulate(INVALIDGLOCHAR);
				return (-1);
			}
		}

	}
}
