#include "Compiler.h"

#define DEBUG_LEXI (0)
#if DEBUG_LEXI
#define LEXI_OUT cout
#else
#define LEXI_OUT fout
#endif //DEBUG_LEXI

/*******************************************************************************\
 *																			   *
 *								�ʷ�����ʵ�ֺ���							   *
 *																			   *
\*******************************************************************************/

// �ʷ�����Ŀ����ҵĿ��λ�����
ofstream fout("output/lexi_out.txt");

// ���ڴʷ����������
std::string symkey[MaxReserve] = {							//�����־�������
	"MAINTK", "CONSTTK", "INTTK", "CHARTK", "VOIDTK", "IFTK", "ELSETK", "DOTK", "WHILETK",
	"SWITCHTK", "CASETK", "DEFAULTTK", "SCANFTK", "PRINTFYK", "RETURNTK"
};
std::string symop[MaxOp]{								//�ֽ�����������������
	"ASSIGNSY", "PLUSSY", "SUBSY", "MULTSY", "DIVSY", "LESSY", "LESEQSY", "GRTSY",
	"GRTEQSY", "NEQUALSY", "EQUALSY", "COMMASY", "COLONSY", "SEMICOLONSY", "LSBRASY",
	"RSBRASY", "LMBRASY", "RMBRASY", "LBBRASY", "RBBRASY", "SIGQUOSY", "DUBQUOSY"
};

// ���ڴ���ת��

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

	this->usr[IDEN - MaxReserve - MaxOp] = "��ʶ��";
	this->usr[STRINGUSR - MaxReserve - MaxOp] = "�ַ���";
	this->usr[CHARUSR - MaxReserve - MaxOp] = "�ַ�";
	this->usr[NUMUSR - MaxReserve - MaxOp] = "��������";

}

// ��ֱ�Ӷ����symkey �� symop ������
void Compiler::otptwordSetup() {

}

void Compiler::lexicalSetup() {
	this->ch = ' ';					// ��ǰ�����ַ�
	this->strToken = "";			// �Ѿ������һ���ַ���
	this->lineNum = 0;				// ����Դ���������
	this->intgNum = 0;				// ����Ϊ����ʱ����������
	this->pin = 0;					// �����ļ�ʱ���ַ�pin
	this->tok_idx = 0;				// ��ǰ�ļ����Ѿ�ʶ�����tok����
	
	this->curToken;
	this->wordSetup();				// �ʷ�tokenType��Ӧsetup
	this->otptwordSetup();			// �ʷ��������
	// �������﷨��setup
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
	this->strToken = "";								// ��ʼ��
	this->intgNum = 0;
	tokenType ret;										// ���ؾ������Ѱ����
	getNextchar();
	skipSpace();
	// ʶ����ĸ
	if (isLetter()) {
		do {
			catToken();
			getNextchar();
		} while (isLetter() || isDigit());		//�������е�������ĸ����
		if (ch == '\'' || ch == '\"') {
			if (ch == '\"') {
				errorManipulate(DQMISMATCH);
			}
			else if (ch == '\''&&strToken.size() > 1) {
				errorManipulate(DQMISMATCH);
			}
			else {
				errorManipulate(SQMISMATCH);	//��൥����ȱʧ;
			}
			//return -1;
		}
		reTract();								//���˶�����ַ�
		toLower();								//��strTokenת��ΪСд�������ִ�Сд
		ret = searchReserve(strToken);				// ��ǰtoken�Ƿ�Ϊ������
		if (ret >= 0) {
			LEXI_OUT << ++tok_idx << ' ' << symkey[ret] << ' ' << strToken << endl;
			curToken = ret;
		}
		else {									// �Ǳ�ʶ�������ص����齻�����ű�
			LEXI_OUT << ++tok_idx << " IDEN " << strToken << endl;
			curToken = IDEN;
		}
	}
	// ʶ������
	else if (isDigit()) {
		do {
			catToken();
			getNextchar();
		} while (isDigit());					// �������������������ַ�
		/*if ((searchOpforChar(ch) < 0 && ch != ' ') || ch == '\'' || ch == '\"' || ch == '(') {		//���������ֽ�β����������˳���ǰ����
			//LEXI_OUT << "WRONG END IN NUM !" << endl;
			errorManipulate(0, lineNum, text);
			while (ch != ','&&ch != ';'&&ch != ';'&&ch != '\n'&&ch != '\t'&&ch != '\0') {
				getNextchar();
			}
			reTract();
			return -1;
		}*/
		reTract();								// ���������ַ�
		intgNum = transtoNum();		// ��ö�Ӧ�ַ���������
		curToken = NUMUSR;
		LEXI_OUT << ++tok_idx << " NUMUSR " << intgNum << endl;
	}
	// ʶ���ַ�
	else if (ch == '\'') {
		//LEXI_OUT << ++tok_idx << " SIGQUOSY " << ch << endl;	// ������������ͣ�ǰ��
		//curToken = SIGQUOSY;
		/* ---------------------------------- ������һ�����ʣ��ҵ���Ҫ��Ҫ����һ���������﷨���������������ַ�������ͬ�� ------------------------------------- */
		//catToken();
		getNextchar();									// ���������ڲ����ַ�
		if (isChar()) {
			catToken();
			getNextchar();
			if (ch == '\'') {
				//catToken();
				//getNextchar();
				LEXI_OUT << ++tok_idx << " CHARUSR " << strToken << endl;					// ����û��Զ����ַ���־
				curToken = CHARUSR;
				//LEXI_OUT << ++tok_idx << " SIGQUOSY " << ch << endl;						// ������������ͣ���
				//curToken = SIGQUOSY;												// ���Ҫ�Ǳ�ɵ����ŵ����;ͻḲ����һ���ַ������ˣ����Բ���ȡ��
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
// << ++tok_idx << " SIGQUOTE " << ch << endl;	// �������������(��)
	}
	// ʶ���ַ���
	else if (ch == '\"') {
		getNextchar();
		while (ch != '\"') {
			if (ch == '\n') {
				errorManipulate(NOMEANING, "�ַ�����\"δƥ�䣡");
				break;
			}
			if (ch == 32 || ch == 33 || ch >= 35 && ch <= 126) {			// �ַ����ǿն��������ַ��Ϸ�
				catToken();
				getNextchar();
			}
			else {
				//LEXI_OUT << "UNEXPECTED CHAR IN STRING!" << endl;			// �ַ����ǿյ��������ַ����Ϸ�
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
	// ʶ���������ı�ʶ��
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
			// ���Ⱥ����Ӧ��ֻ��Ĵ�
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
			if (ch == '\0')						// �����������Ҫ�˳�
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
