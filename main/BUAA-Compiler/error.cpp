#include "Compiler.h"

#define DEBUG (1)
#if DEBUG
#define ERROR_OUT cout
#else
#define ERROR_OUT errout
#endif //DEBUG
// ���������

std::string SQMISMATCHMSG = "\'�޷�ƥ��";
std::string DQMISMATCHMSG = "�ַ�����\"�޷�ƥ��";
std::string INVALIDSQCHARMSG = "\'��\'�а������ַ������ķ�";
std::string INVALIDDQCHARMSG = "\"��\"�а������ַ������ķ�";

std::string LACKEQLMSG = "!����û����=";
string INVALIDGLOCHARMSG = "�����ķ��涨֮����ս��";
string ILLEGALVARTYPEMSG = "���Ϸ������ͣ�";
string LACKSEMICOLONMSG = "����βȱ�� \';\'";
string NOINTORCHARMSG = "������������int��char������û���ҵ� ��";

ofstream errout("output/error_out.txt");

// ��������
void Compiler::errorManipulate(errorType id, string s) {
	// ���з��ĸ���ͳ��
	int count = 0;
	this->firstinLine = 0;
	this->error_count++;
	// �ҵ�������в��������
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
	// �������
	ERROR_OUT << "ERROR in line " << lineNum + 1 << " : " << endl;
	ERROR_OUT << "---> ";
	for (int i = firstinLine;i <= lastinLine;i++)
		ERROR_OUT << origin_text[i];
	ERROR_OUT << endl;
	// �������
	ERROR_OUT << "---> " << ((s == "default")? (*errorMsg[id]): s) << endl;
}



// ������Ӧ����
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


// ��Ӧ������������
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