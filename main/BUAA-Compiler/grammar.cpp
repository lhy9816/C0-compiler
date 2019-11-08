#include "Compiler.h"

#define DEBUG (0)
#if DEBUG
#define GRM_OUT cout
#else
#define GRM_OUT gram_fout
#endif //DEBUG

/*******************************************************************************\
 *																			   *
 *								�﷨����ʵ�ֺ���							   *
 *																			   *
\*******************************************************************************/

// �﷨����Ŀ����ҵĿ��λ�����
ofstream gram_fout("output/gram_out.txt");

// �����Ӧ�������Ͳ��Ҽ�
// ������
const tokenType J_DEFAULT[] = { SEMICOLONSY, COMMASY, COLONSY, PLUSSY, SUBSY, MULTSY, DIVSY, LBBRASY,LSBRASY,LMBRASY,RSBRASY,RMBRASY,RBBRASY };
const int J_DEFAULT_NUM = 13;

// ���Ҽ�
const tokenType VAR_BEGIN[] = {LMBRASY,SEMICOLONSY,COMMASY};
const int VAR_BEGIN_NUM = 3;

const tokenType STAT_BEGIN[] = {IFTK, DOTK, SWITCHTK, SCANFTK, PRINTFTK, RETURNTK, IDEN, LBBRASY, SEMICOLONSY};
const int STAT_BEGIN_NUM = 9;

const tokenType STAT_BEGIN_AND_END[] = { IFTK, DOTK, SWITCHTK, SCANFTK, PRINTFTK, RETURNTK, IDEN, LBBRASY, RBBRASY, SEMICOLONSY };
const int STAT_BEGIN_AND_END_NUM = 10;

const tokenType FUNC_BEGIN[] = {VOIDTK, INTTK, CHARTK};
const int FUNC_BEGIN_NUM = 3;

// ����ֵ"�Ĵ���"
string* Ret = new string("%RET");

// ���ű����Һ���
bool Compiler::check_defined(string name) {
	map<string, Item*>::iterator it;
	map<string, FuncItem*>::iterator it_f;
	// �ֲ�������
	if (cur_active_funtab!=nullptr) {
		Item* ite;
		if ((ite = cur_active_funtab->get_const(name)) != nullptr)
			return true;
		if ((ite = cur_active_funtab->get_var(name)) != nullptr)
			return true;
		if ((it_f = fun_tab.find(name)) != fun_tab.end() && it_f->second->paranum == 0)	// ���棺�в���
			return true;
		return false;
	}
	// ȫ�ֺ���Ϊ�գ��򷵻�δ�ҵ�
	if (fun_tab.size()==0)
		return false;
	// ����ȫ�ֺ���
	if ((it_f = fun_tab.find(name)) != fun_tab.end())	// ���棺�в���
		return true;
	// �������к����������������������
	for (it_f = fun_tab.begin();it_f != fun_tab.end();it_f++) {
		Item* ite;
		if ((ite = it_f->second->get_const(name)) != nullptr || (ite = it_f->second->get_const(name)) != nullptr)
			return true;
	}
	// ȫ�ֳ��������
	if ((it = global_const_tab.find(name))!=global_const_tab.end())
		return true;
	if ((it = global_var_tab.find(name)) != global_var_tab.end())
		return true;
	// ��֪����ô���в�
	//if (this->get_const_var_item != nullptr && this->get_func_item != nullptr)
	//	return true;
	return false;
}

Item* Compiler::get_const_var_item(string name) {
	map<string, Item*>::iterator it;
	// �ֲ�������
	if (cur_active_funtab) {
		Item* ite;
		if ((ite = cur_active_funtab->get_const(name)) != nullptr)
			return ite;
		if ((ite = cur_active_funtab->get_var(name)) != nullptr)
			return ite;
		if ((ite = cur_active_funtab->get_para(name)) != nullptr)
			return ite;
	}

	if ((it = global_const_tab.find(name)) != global_const_tab.end())
		return it->second;
	if ((it = global_var_tab.find(name)) != global_var_tab.end())
		return it->second;
	//errorManipulate(NOMEANING, "����"+name+"δ���壡");
	//exit(0);
	return nullptr;
}

FuncItem* Compiler::get_func_item(string name) {
	map<string, FuncItem*>::iterator it_f = fun_tab.find(name);

	if (it_f != fun_tab.end())	// ���棿���в����Ŀ�������
		return it_f->second;
	errorManipulate(NOMEANING, "���������δ���壡");
	//exit(0);
	return nullptr;
}

void Compiler::put_const(string name, tokenType kind, int value) {
	// û��Ԥ�ȶ���
	if (!this->check_defined(name)) {
		Item* new_const = new Item(name, CONST, kind, value);
		// global_const
		if (cur_active_funtab == nullptr) {
			global_const_tab[name] = new_const;
			global_const_tab[name]->global = true;
		}
		// fun_const
		else {
			cur_active_funtab->constMap[name] = new_const;
		}
	}
	else {
		errorManipulate(NOMEANING, "��Ҫ�����ȫ�ֳ��� "+name+" �ض��� ��");
	}
}

void Compiler::put_var(string name, tokenType kind, int value, int length) {
	// û��Ԥ�ȶ��壬��ʼ����ֵΪ-1
	if (!this->check_defined(name)) {
		Item* new_var = new Item(name, VAR, kind, value,length);
		// global_var
		if (cur_active_funtab == nullptr) {
			global_var_tab[name] = new_var;
			global_var_tab[name]->global = true;
			// is array
			if (length > 0) {
				global_var_tab[name]->offset = this->global_var_offset;
				this->global_var_offset += length;
			}
			else
				global_var_tab[name]->offset = this->global_var_offset++;
		}
		// fun_var
		else{
			cur_active_funtab->varMap[name] = new_var;
			if (length > 0) {
				cur_active_funtab->varMap[name]->offset = cur_active_funtab->offset;
				cur_active_funtab->offset += length;
			}
			else
				cur_active_funtab->varMap[name]->offset = cur_active_funtab->offset++;			
		}
	}
	else {
		errorManipulate(NOMEANING, "��Ҫ����ı��� " + name + " �ض��� ��");
	}
}

void Compiler::put_func(string name, Genre genre, tokenType kind,int paranum) {
	this->cur_active_funtab = nullptr;	//���ܺ����ж��������������
	// û��Ԥ�ȶ��壬�Ȳ��ܲ�������Ϊ-1��ֻ��½������
	if (!this->check_defined(name)) {
		FuncItem* new_func = new FuncItem(name, genre, kind, paranum);
		fun_tab[name] = new_func;
		// ��ʼ���������ñ�������
		fun_tab[name]->offset = 0;
		cur_active_funtab = new_func;
	}
	else {
		errorManipulate(NOMEANING, "��Ҫ����ĺ��� " + name + " �ض��壡");
	}
}

void Compiler::print_symbol_item() {
	map<string, Item*>::iterator iter;
	map<string, FuncItem*>::iterator iter_f;
	GRM_OUT << "--------GLOBAL CONST--------" << endl;
	GRM_OUT << "����		����		ֵ		ƫ��" << endl;
	for (iter = global_const_tab.begin();iter != global_const_tab.end();iter++) {
		GRM_OUT << iter->second->getName() << "  " << iter->second->getKind() << "  " << iter->second->getValue() << "  " << iter->second->offset << endl;
	}
	GRM_OUT << endl;
	GRM_OUT << "--------GLOBAL VAR--------" << endl;
	GRM_OUT << "����		����		����		ƫ��" << endl;
	GRM_OUT << "GLOBAL_VAR_OFFSET : " << this->global_var_offset << endl;
	for (iter = global_var_tab.begin();iter != global_var_tab.end();iter++) {
		GRM_OUT << iter->second->getName() << "  " << iter->second->getKind() << "  " << iter->second->getLength() << "  " << iter->second->offset << endl;
	}
	GRM_OUT << endl;
	GRM_OUT << "--------GLOBAL FUN--------" << endl;
	for(iter_f = fun_tab.begin();iter_f!=fun_tab.end();iter_f++){
		GRM_OUT << "��������		����ֵ	  ���޲���	  ��ƫ��" << endl;
		GRM_OUT << iter_f->second->getName() << "  " << iter_f->second->getKind() << "  " << iter_f->second->getGenre() << "  " << iter_f->second->offset << endl;
		GRM_OUT << "--------CONST IN FUN--------" << endl;
		for (iter = iter_f->second->constMap.begin();iter != iter_f->second->constMap.end();iter++) {
			GRM_OUT << iter->second->getName() << "  " << iter->second->getKind() << "  " << iter->second->getValue() << "  " << iter->second->offset << endl;
		}
		GRM_OUT << "--------VAR IN FUN--------" << endl;
		for (iter = iter_f->second->varMap.begin();iter != iter_f->second->varMap.end();iter++) {
			GRM_OUT << iter->second->getName() << "  " << iter->second->getKind() << "  " << iter->second->getLength() << "  " << iter->second->offset << endl;
		}
		GRM_OUT << endl;
	}
}

template<typename T1, typename T2, typename T3>
bool Compiler::find_in_sets(T1 set, T2 token, T3 n) {
	int flag = 0;
	for (int i = 0;i < n;i++) {
		if (token == set[i]) {
			flag = 1; break;
		}
	}
	if (flag)
		return true;
	else
		return false;
}


bool Compiler::checkType(tokenType id, tokenType skp) {
	//this->getSym();
	// ƥ��ɹ�
	if (this->curToken == id) {								
		this->getSym();
		return true;
	}
	else {
		string s;
		// ���ݴ�����token���ͱ���
		if (id < MaxReserve)
			s = reserve[id];
		else if (id < MaxOp)
			s = op[id - MaxReserve];
		else
			s = usr[id - MaxReserve - MaxOp];
		this->errorManipulate(NOMEANING, "û��ƥ�� "+s+ " !");		
		return false;
		//this->skip(skp);
	}
}

void Compiler::program() {
	//�����һ������
	this->getSym();
	// �ж������Ƿ���ȫ��const����
	if (this->curToken == CONSTTK) {
		// ȫ��const��ʼ�ĳ���
		this->const_declare();
	}
	if (isVarType()) {
		// ȫ�ֱ������з���ֵ������ʼ�ĳ���
		// ��¼��ǰ�ı�����������ֵ����������
		string name;
		tokenType retType;
		Midcode* mid;
		// ��������ͷ����ȡ����������
		this->declare_head(&name, &retType);
		// �Ƿ��Ǳ�������
		while (find_in_sets<const tokenType[],tokenType,int>(VAR_BEGIN,this->curToken,VAR_BEGIN_NUM)) {
			this->var_def(name, retType);	// ����Ե�¼���ű�
			if (checkType(SEMICOLONSY, NONE)) {
				if (isVarType()) {
					this->declare_head(&name, &retType);
				}
				else if (this->curToken == VOIDTK) {
					break;
				}
				else {
					this->skip(J_DEFAULT, J_DEFAULT_NUM);
				}
			}
			else {
				this->skip(J_DEFAULT, J_DEFAULT_NUM);
			}
		}
		if (this->curToken == VOIDTK) {
			// ��void��������������ͷ�ĳ���
			this->fun_def();
		}
		// ��һ���в����ĺ�������
		else if (this->curToken == LSBRASY) {
			GRM_OUT << "In Line " << lineNum +1 << " :  " << "This is a customerized function definition !" << endl;
			// ������¼�������ű����Ȳ��أ� name��retType
			this->put_func(name, PARA_FUNC, retType,0);
			mid = this->push_mid_code(FUN_M, &name, 0, 0);
			this->print_mid_code(mid);
			// ���������
			this->paralist(name, retType);
			this->checkType(RSBRASY, NONE);
			
			
			if (this->checkType(LBBRASY, NONE)) {
				// ��һ����������ĸ������
				this->compoundstat();
				// ����������־
				if (this->checkType(RBBRASY, NONE)) {
					// �з���ֵ�����ķ���·���Ƿ����
					if (this->isVarType(retType) && cur_active_funtab->getValue() <= 0) {
						errorManipulate(NOMEANING,"�з���ֵ��������û�з�֧���ص���� ��");
					}
					mid = this->push_mid_code(LABEL_M, &(name + "_end"), 0, 0);
					this->print_mid_code(mid);
					mid = this->push_mid_code(RET_M, &name, 0, 0);
					this->print_mid_code(mid);
					this->fun_def();
				}
				else {
					this->skip(J_DEFAULT, J_DEFAULT_NUM);
				}
			}
			else {
				// �������ʵص�
				this->skip(J_DEFAULT, J_DEFAULT_NUM);
			}
		}
		// ��һ���޲����ĺ�������
		else if (this->curToken == LBBRASY) {
			GRM_OUT << "In Line " << lineNum +1 << " :  " << "This is a customerized function definition !" << endl;
			this->put_func(name, NO_PARA_FUNC, retType, 0);
			mid = this->push_mid_code(FUN_M, &name, 0, 0);
			this->print_mid_code(mid);
			this->getSym();
			this->compoundstat();
			this->checkType(RBBRASY, NONE);
			if (this->isVarType(retType) && cur_active_funtab->getValue() <= 0) {
				errorManipulate(NOMEANING, "�з���ֵ��������û�з�֧���ص���� ��");
			}
			mid = this->push_mid_code(LABEL_M, &(name + "_end"), 0, 0);
			this->print_mid_code(mid);
			mid = this->push_mid_code(RET_M, &name, 0, 0);
			this->print_mid_code(mid);
			this->fun_def();
		}
	}
		/*int curVarkind = this->curToken;
		this->getSym();
		string iden_name = this->strToken;
		// ����var����func��������Ҫ��
		if (checkType(IDEN, SEMICOLONSY)) {
			// �������вſ������ֳ������������������
			// �в��з���ֵ����
			if (this->curToken == LSBRASY ) {
				this->getSym();
				this->paralist();
			}
			// �޲��з���ֵ����
			else if (this->curToken == LBBRASY) {
				this->getSym();
				this->compoundstat();
			}
			// ������������ ��װ����
			else if (this->curToken == LMBRASY || this->curToken == SEMICOLONSY || this->curToken == COMMASY) {
				do {
					if (this->curToken == LMBRASY) {
						this->getSym();
						if (checkType(NUMUSR, SEMICOLONSY)) {
							if (checkType(RMBRASY, SEMICOLONSY)) {
								// ��½���ű�
								continue;
							}
							else {
								this->errorManipulate();
								this->skip();
							}
						}
						else {
							this->errorManipulate();
							this->skip();
						}
					}
					else {
						//��½���ű�
						continue;
					}
					
				} while ();
				
			}
			else {
				errorManipulate(NOMEANING, "��ʶ����β��ƥ�� ��");
				this->getSym();
			}
			
		}
	}*/
	else if (this->curToken == VOIDTK) {
		// ��void��������������ͷ�ĳ���
		this->fun_def();
	}
	else {
		// ���Ϸ��ĺ�����ʼ������
		this->errorManipulate(ILLEGALVARTYPE);
		this->skip(J_DEFAULT, J_DEFAULT_NUM);
	}
	this->print_symbol_item();
}
// <��������>
void Compiler::var_def(string name, tokenType retType) {
	// ����Ƿֺţ����½���ű�����
	if (this->curToken == SEMICOLONSY) {
		GRM_OUT << "In Line " << lineNum +1 << " :  " << "This is a var definition !" << endl;
		// ��¼���ű�������
		this->put_var(name, retType, -1, 0);
		return;
	}
	bool arr_flag = false;
	if (this->curToken == LMBRASY) {
		GRM_OUT << "In Line " << lineNum +1 << " :  " << "This is a var definition !" << endl;
		this->getSym();
		// ������������
		int length;
		this->integer(&length);
		if (length <= 0)
			this->errorManipulate(NOMEANING, "���鶨��ʱ�±겻�������� ��");
		// �����������ű�
		this->put_var(name, retType, -1, length);
		this->checkType(RMBRASY,NONE);
		arr_flag = true;
	}
	// �ж��Ŵ��ڣ����ж������
	if (this->curToken == COMMASY) {
		GRM_OUT << "In Line " << lineNum +1 << " :  " << "This is a var definition !" << endl;
		// �Բ�������������� ����ű�
		if(!arr_flag)
			this->put_var(name, retType, -1, 0);
		// �ж���һ���ǲ��Ǳ�ʶ��
		this->getSym();
		if (this->curToken == IDEN) {
			name = this->strToken;
			this->getSym();
			this->var_def(name, retType);
		}
		else
			this->skip(J_DEFAULT, J_DEFAULT_NUM);
	}
	else {
		return;
		//this->getSym();
	}
}

void Compiler::const_declare() {
	do {
		this->getSym();
		this->const_def();								// ����Ҫ������ű�
		if (this->curToken == SEMICOLONSY)
			this->getSym();
		else {
			errorManipulate(LACKSEMICOLON);
			this->skip(J_DEFAULT, J_DEFAULT_NUM);						// ����޸�һ�£�����������
			this->getSym();
		}
	} while (this->curToken == CONSTTK);
	//GRM_OUT << "In Line " << lineNum +1 << " :  " << "This is a const declaration !" << endl;
}

void Compiler::const_def() {
	if (this->curToken == INTTK) {
		do {
			GRM_OUT << "In Line " << lineNum +1 << " :  " << "This is a const definition !" << endl;
			// ��ʶ������ƥ��
			this->getSym();
			string name = this->strToken;				// ��ǰ������ʶ��
			if (checkType(IDEN, SEMICOLONSY)) {			// checkType������һ������
				if (checkType(ASSIGNSY, SEMICOLONSY)) {
					int value;
					this->integer(&value);
					// ��½���ű�������
					this->put_const(name, INTTK, value);
					// ��Ԫʽ��
					
				}
			}
		} while (this->curToken == COMMASY);
	}
	else if (this->curToken == CHARTK) {
		do {
			GRM_OUT << "In Line " << lineNum +1 << " :  " << "This is a const definition !" << endl;
			// ��ʶ������ƥ��
			this->getSym();
			string name = this->strToken;
			if (checkType(IDEN, SEMICOLONSY)) {			// checkType������һ������
				if (checkType(ASSIGNSY, SEMICOLONSY)) {
					if (this->curToken == CHARUSR){
						//������ű�������
						this->put_const(name, CHARTK, strToken[0]);
						checkType(CHARUSR, SEMICOLONSY);
					}
				}
			}
		} while (this->curToken == COMMASY);
	}
	else {
		errorManipulate(NOMEANING, "û��ƥ�����ͱ�ʶ�� ��");
	}
}
// handle integer
void Compiler::integer(int* value) {
	bool neg =false;
	if (this->curToken == SUBSY) {
		neg = true;
		this->getSym();
	}
	else if (this->curToken == PLUSSY) {
		this->getSym();
	}
	//���ﲻ��checkType�ˣ���Ҫ��ת�ĵط��Ƚ϶�
	if (this->curToken == NUMUSR) {	
		if (neg) {
			*value = -this->intgNum;
		}
		else
			*value = this->intgNum;
		if (*value > MaxInt) {
			errorManipulate(NOMEANING, "����������� !");
			//*value = MaxInt;
		}
		// ������ű����������������õ�����ű�������������������������
		//this->intgNum = this->intgNum;	//�ٵĵ���
		
		this->getSym();
	}
	else {
		*value = this->intgNum;
		// ��һ�¾�������������
		this->skip(J_DEFAULT, J_DEFAULT_NUM);
	}
}

// �����������������ĺ������ֶ���
void Compiler::fun_def() {
	string fun_name;
	//this->getSym();
	Midcode* mid;
	do {
		switch (this->curToken) {
		case VOIDTK:
			this->getSym();
			fun_name = this->strToken;
			if (this->curToken == IDEN) {
				this->getSym();
				GRM_OUT << "In Line " << lineNum +1 << " :  " << "This is a customerized function definition !" << endl;
				mid = this->push_mid_code(FUN_M, &fun_name, 0, 0);
				this->print_mid_code(mid);
				this->void_iden_fun(fun_name,VOIDTK);
			}
			else if (this->curToken == MAINTK) {
				this->getSym();
				GRM_OUT << "In Line " << lineNum +1 << " :  " << "This is the main function entry !" << endl;
				mid = this->push_mid_code(FUN_M, &fun_name, 0, 0);
				this->print_mid_code(mid);
				this->main_fun("MAIN",VOIDTK);
			}
			else{
				errorManipulate(NOMEANING, "û�к������Ͷ��� ��");
				GRM_OUT << "In Line " << lineNum +1 << " :  " << "This is the customerized function entry !" << endl;
				this->void_iden_fun("computer_def",VOIDTK);
			}
			break;
		case INTTK:
		case CHARTK:
			GRM_OUT << "In Line " << lineNum +1 << " :  " << "This is a customerized function definition !" << endl;
			tokenType retType = this->curToken;
			this->getSym();
			if (this->curToken != IDEN) {
				this->skip(LBBRASY);
			}
			string fun_name = this->strToken;
			mid = this->push_mid_code(FUN_M, &fun_name, 0, 0);
			this->print_mid_code(mid);
			this->getSym();
			this->ret_iden_fun(fun_name, retType);
			break;
		}
	} while (this->find_in_sets<const tokenType[],tokenType,int>(FUNC_BEGIN,this->curToken,FUNC_BEGIN_NUM));
	if (!this->have_main) {
		errorManipulate(NOMEANING, "û��main���� ��");
	}
}

void Compiler::void_iden_fun(string name,tokenType retType){
	Midcode* mid;
	// ��С���ţ����ֲ���
	if (this->curToken == LSBRASY) {
		// �����ű�
		this->put_func(name, PARA_FUNC, retType, 0);
		this->paralist(name,retType);
		this->checkType(RSBRASY, NONE);
	}
	else {
		this->put_func(name, NO_PARA_FUNC, retType, 0);
	}

	this->checkType(LBBRASY, NONE);
	this->compoundstat();
	this->checkType(RBBRASY, NONE);
	mid = this->push_mid_code(LABEL_M, &(name + "_end"), 0, 0);
	this->print_mid_code(mid);
	mid = this->push_mid_code(RET_M, &name, 0, 0);
	this->print_mid_code(mid);
}

void Compiler::ret_iden_fun(string name, tokenType retType) {
	// ��¼�з���ֵ�������ű�
	this->void_iden_fun(name, retType);
	if (this->isVarType(retType) && cur_active_funtab->getValue() <= 0) {
		errorManipulate(NOMEANING, "�з���ֵ��������û�з�֧���ص���� ��");
	}
}

void Compiler::main_fun(string name, tokenType retType) {
	this->have_main = true;
	Midcode* mid;
	// ��¼�����ű���
	this->put_func("main", NO_PARA_FUNC, retType, 0);
	this->checkType(LSBRASY, NONE);
	this->checkType(RSBRASY, NONE);
	this->checkType(LBBRASY, NONE);
	this->compoundstat();
	this->checkType(RBBRASY, NONE);
	string main = "main_end";
	mid = this->push_mid_code(LABEL_M, &main, 0, 0);
	this->print_mid_code(mid);
	mid = this->push_mid_code(EXIT_M, 0,0, 0);
	this->print_mid_code(mid);
}

void Compiler::var_declare() {
	string name;
	tokenType retType;
	// ��������ͷ����ȡ����������
	this->declare_head(&name, &retType);
	// �Ƿ��Ǳ�������
	while (this->find_in_sets<const tokenType[], tokenType, int>(VAR_BEGIN, this->curToken, VAR_BEGIN_NUM)) {
		this->var_def(name, retType);	// ����Ե�¼���ű�
		if (checkType(SEMICOLONSY, NONE)) {
			if (isVarType()) {
				this->declare_head(&name, &retType);
			}
			// ���أ������Ǳ������岿��
		}
		else {
			this->skip(J_DEFAULT, J_DEFAULT_NUM);
		}
	}
}

void Compiler::compoundstat() {
	if (this->curToken == CONSTTK) {
		// ������const��ʼ�ĸ������
		this->const_declare();
	}
	if (isVarType()) 
		this->var_declare();
	this->statelist();
}

void Compiler::statelist() {
	while (this->find_in_sets<const tokenType[], tokenType, int>(STAT_BEGIN, this->curToken, STAT_BEGIN_NUM)){
		this->statement();
	}
}

void Compiler::statement() {
	// �����������
	string name = this->strToken;
	Midcode* mid;
	switch (this->curToken) {
	case IFTK:	
		this->if_st();
		break;
	case DOTK:	
		this->do_while_st();
		break;
	case SWITCHTK:	
		this->switch_st();
		break;
	case SCANFTK:	
		this->read_st();
		this->checkType(SEMICOLONSY, NONE);
		break;
	case PRINTFTK:	
		this->write_st();
		this->checkType(SEMICOLONSY, NONE);
		break;
	case RETURNTK:	
		this->return_st();
		this->checkType(SEMICOLONSY, NONE);
		break;
	case IDEN:	
		// ����ű�����ά����ȷ����ʶ������
		this->getSym();
		// �вε���
		FuncItem* it_f;
		if (this->curToken == LSBRASY) {
			GRM_OUT << "In Line " << lineNum +1 << " :  " << "This is a function call with some parameters !" << endl;
			it_f= this->get_func_item(name);
			if (it_f != nullptr) {
				// ����Ƿ����޲κ�����������
				if (it_f->getGenre() == NO_PARA_FUNC) {
					errorManipulate(NOMEANING, "�޲κ����ڵ���ʱ����С���ţ�");
				}
				this->fun_call(it_f);
				mid = this->push_mid_code(CALL_M, &name, 0, 0);
				this->print_mid_code(mid);
			}
			else {
				this->skip(J_DEFAULT, J_DEFAULT_NUM);
			}
		}
		// �޲ε���
		else if(this->curToken == SEMICOLONSY){
			GRM_OUT << "In Line " << lineNum +1 << " :  " << "This is a function call with no parameters !" << endl;
			it_f = this->get_func_item(name);
			if (it_f != nullptr) {
				mid = this->push_mid_code(CALL_M, &name, 0, 0);
				this->print_mid_code(mid);
				//this->fun_call(it_f);
			}
			else {
				this->skip(J_DEFAULT, J_DEFAULT_NUM);
			}
		}
		// �����Ǹ�ֵ��䣬�����ڲ�����
		else {
			this->assign_st(&name);
		}
		this->checkType(SEMICOLONSY, NONE);
		break;

	case SEMICOLONSY:
		this->checkType(SEMICOLONSY, NONE);
		break;
	case LBBRASY:	
		this->getSym();
		this->statelist();
		this->checkType(RBBRASY, NONE);
		break;
	default:
		errorManipulate(NOMEANING, "�޷�ʶ��������� ��");
	}
}

void Compiler::fun_call(FuncItem* func_it) {
	// ������ţ�ֻ����ֵ������
	int i = 0;
	// ����ʽ��ը
	bool is_const;
	int value;
	string* tmp_name = new string();
	tokenType cal_type;
	do {
		this->getSym();
		// ������ƥ�䣬����
		if (func_it->paranum <= i) {
			errorManipulate(NOMEANING,"���ú���ʱ����������ƥ�䣡");
			this->skip(J_DEFAULT,J_DEFAULT_NUM);
			break;
		}
		cal_type = this->expr(&is_const, &value, tmp_name);
		// ������������ƥ��
		if (cal_type != func_it->paraList[i++].getKind())
			errorManipulate(NOMEANING, "���� " + func_it->getName() + " �ĵ� " + std::to_string(i) + " ���������Ͳ�ƥ�䣡");
		// �����ǳ����Ļ�ֱ�ӷ���ֵ
		if (is_const)
			this->int2string(tmp_name, value);
		Midcode* mid;
		mid = push_mid_code(PUSH_M, tmp_name, &func_it->getName(), &int22string(i-1));
		this->print_mid_code(mid);
	} while (this->curToken == COMMASY);
	if (i < func_it->paranum) {
		this->errorManipulate(NOMEANING, "���ú���ʱ����������ƥ�䣡");
	}
	delete(tmp_name);
	this->checkType(RSBRASY,NONE);
}

void Compiler::assign_st(string* name) {
	// ���ҷ��ű����Դ����name
	Item* it = get_const_var_item(*name);
	if (it == nullptr) {
		errorManipulate(NOMEANING, "����" + *name + "δ����");
		this->skip(SEMICOLONSY);
		return;
	}
	if (it->getGenre() == CONST) {
		errorManipulate(NOMEANING, "�Գ�����ֵ��");
		this->skip(SEMICOLONSY);
		return;
	}
	Midcode* mid;
	// ��ʶ��������
	string* arr_idx_name = new string();
	bool arr_flag = false;
	GRM_OUT << "In Line " << lineNum +1 << " :  " << "This is an assign statement !" << endl;
	if (this->curToken == LMBRASY) {
		if (!(it->getGenre() == VAR && it->getLength() > 0) ){
			errorManipulate(NOMEANING, "��ʶ�������������ͣ���ƥ�� ��");
			this->skip(SEMICOLONSY);
			return;
		}
		arr_flag = true;
		bool is_const;
		int value;
		tokenType cal_type;
		this->getSym();
		cal_type = this->expr(&is_const, &value, arr_idx_name);
		// ����Խ�����,���س�ֵ�����жϣ���������Խ���ж�
		if (cal_type == INTTK && is_const) {
			// ����Խ��
			if (it->getLength() <= value || value <0) {
				errorManipulate(NOMEANING, "Ŀ�����鷢������Խ�� ��");
			}
		}
		if (cal_type != INTTK) {
			errorManipulate(NOMEANING, "�����±�����ֵ����int���� ��");
		}
		this->checkType(RMBRASY, NONE);
	}
	if (this->curToken == ASSIGNSY) {
		this->checkType(ASSIGNSY, NONE);
		bool as_is_const;
		int as_value;
		string* as_tmp_name = new string();
		tokenType as_cal_type;
		as_cal_type = this->expr(&as_is_const, &as_value, as_tmp_name);
		if (!(it->getGenre() == VAR || it->getGenre() == CONST || it->getGenre()==PARA)) {
			errorManipulate(NOMEANING, "�������и�ֵ ��");
			this->skip(SEMICOLONSY);
			return;
		}
		if (as_cal_type != it->getKind()) {
			errorManipulate(NOMEANING, "��ֵ�����ߵ������Ͳ�һ�� ��");
		}
		if (arr_flag) {
			mid = this->push_mid_code(ARGET_M, name, arr_idx_name, as_tmp_name);
			this->print_mid_code(mid);
		}
		else {
			mid = this->push_mid_code(ASS_M, as_tmp_name, 0, &(it->getName()));
			this->print_mid_code(mid);
		}
		delete(as_tmp_name);
	}
	else {
		this->skip(SEMICOLONSY);
	}
}

void Compiler::if_st() {
	// ��Щ������Ԫʽ������
	Midcode* mid;
	GRM_OUT << "In Line " << lineNum +1 << " :  " << "This is an if-else statement !" << endl;
	this->getSym();
	this->checkType(LSBRASY, NONE);
	bool cond_certain;
	int cond_value = 0;
	string* cond_label = new string();
	this->gen_label_name(cond_label);
	// ������Ԫʽ
	this->cond(&cond_certain,&cond_value,cond_label);
	this->checkType(RSBRASY, NONE);
	this->statement();
	string* tar_label = new string();
	this->gen_label_name(tar_label);
	mid = this->push_mid_code(J_M, tar_label, 0, 0);
	this->print_mid_code(mid);
	if (this->checkType(ELSETK, NONE)) {
		// ��Ԫʽ����
		mid = this->push_mid_code(LABEL_M, cond_label, 0, 0);
		this->print_mid_code(mid);
		this->statement();
		mid = this->push_mid_code(LABEL_M, tar_label, 0, 0);
		this->print_mid_code(mid);
	}

	delete(cond_label);
}

void Compiler::cond(bool* certain, int* is_while, string* label) {
	/* cond ���ס����ƥ��Ĵ��� */
	// �Ƚ������ token ��¼
	Midcode* mid;
	tokenType cmp_op;
	// �����������ֵ��¼
	tokenType left_type, right_type;
	bool left_certain = false, right_certain = false;
	int left_value = 0, right_value = 0;
	string* left_name = new string();
	string* right_name = new string();
	// ���˷��ű�֮��Żᱨ������ͼ��Ĵ�
	left_type = this->expr(&left_certain, &left_value, left_name);
	if (left_type != INTTK) {
		this->errorManipulate(NOMEANING, "��������ʽ����಻��int���� ��");
	}
	// �鿴�ȽϷ�
	switch (this->curToken) {
	case EQUALSY:
	case NEQUALSY:
	case LESSY:
	case LESEQSY:
	case GRTEQSY:
	case GRTSY:
		cmp_op = this->curToken;
		this->getSym();
		break;
	default:
		// ��ʹ�����ж���Ҳ�����ˣ�����������ʽ����
		mid = (*is_while==0)?(this->push_mid_code(EQJ_M, left_name, new string("0"), label)):(this->push_mid_code(NEQJ_M, left_name, new string("0"), label));
		this->print_mid_code(mid);
		*certain = left_certain;
		//*value = left_value;
		return;
	}
	right_type = this->expr(&right_certain, &right_value, right_name);
	if (right_type != INTTK) {
		this->errorManipulate(NOMEANING, "��������ʽ���Ҳ಻��int���� ��");
	}
	// ���ҵ�ֵ�Ƿ񶼿���ֵ��������Ԫʽ
	//if (left_certain && right_certain) {
		// ֱ��������Ԫʽ
		switch (cmp_op) {
		case EQUALSY:
			mid = (*is_while)?(this->push_mid_code(EQJ_M, left_name, right_name, label)):(this->push_mid_code(NEQJ_M, left_name, right_name, label));
			this->print_mid_code(mid);
			break;
		case NEQUALSY:
			mid = (*is_while) ? (this->push_mid_code(NEQJ_M, left_name, right_name, label)) : (this->push_mid_code(EQJ_M, left_name, right_name, label));
			this->print_mid_code(mid);
			break;
		case LESSY:
			mid = (*is_while) ? (this->push_mid_code(LSJ_M, left_name, right_name, label)) : (this->push_mid_code(BGEJ_M, left_name, right_name, label));
			this->print_mid_code(mid);
			break;
		case LESEQSY:
			mid = (*is_while) ? (this->push_mid_code(LSEJ_M, left_name, right_name, label)) : (this->push_mid_code(BGJ_M, left_name, right_name, label));
			this->print_mid_code(mid);
			break;
		case GRTEQSY:
			mid = (*is_while) ? (this->push_mid_code(BGEJ_M, left_name, right_name, label)) : (this->push_mid_code(LSJ_M, left_name, right_name, label));
			this->print_mid_code(mid);
			break;
		case GRTSY:
			mid = (*is_while) ? (this->push_mid_code(BGJ_M, left_name, right_name, label)) : (this->push_mid_code(LSEJ_M, left_name, right_name, label));
			this->print_mid_code(mid);
			break;
		}
	
}

void Compiler::do_while_st() {
	Midcode* mid;
	GRM_OUT << "In Line " << lineNum +1 << " :  " << "This is a do-while statement !" << endl;
	this->getSym();
	// ������Ԫʽ
	string* begin_label = new string();
	this->gen_label_name(begin_label);
	mid = this->push_mid_code(LABEL_M, begin_label, 0, 0);
	this->print_mid_code(mid);
	this->statement();
	this->checkType(WHILETK, NONE);
	this->checkType(LSBRASY, NONE);
	bool cond_certain;
	int cond_value = 1;
	this->cond(&cond_certain, &cond_value, begin_label);

	delete(begin_label);
	this->checkType(RSBRASY, NONE);
}

void Compiler::switch_st() {
	Midcode* mid;
	string* end_label = new string();
	string* next_label = new string("default");
	// ƥ��'(
	GRM_OUT << "In Line " << lineNum +1 << " :  " << "This is a switch-case-default statement !" << endl;
	this->getSym();
	this->checkType(LSBRASY, NONE);
	bool is_const;
	int value;
	string* switch_name = new string();
	tokenType cal_type;
	cal_type = this->expr(&is_const, &value, switch_name);
	// ƥ��')
	this->checkType(RSBRASY, NONE);
	this->gen_label_name(end_label);
	this->checkType(LBBRASY, NONE);
	// �����
	while (this->curToken == CASETK) {
		if (*next_label != "default") {
			mid = this->push_mid_code(LABEL_M, next_label, 0, 0);
			this->print_mid_code(mid);
		}
		this->one_case(cal_type,end_label,next_label,switch_name);		//�����з��ű��ļ�����Ԫʽ�ĵ��룬Ҫ��tmp_name����һ��label�ù�ȥ
	}
	// default���
	if (this->checkType(DEFAULTTK, NONE)) {
		GRM_OUT << "In Line " << lineNum +1 << " :  " << "This is a default statement !" << endl;
		// default��ǩ
		mid = this->push_mid_code(LABEL_M, next_label, 0, 0);
		this->print_mid_code(mid);
		this->checkType(COLONSY, NONE);
		this->statement();
		this->checkType(RBBRASY, NONE);
		mid = this->push_mid_code(LABEL_M, end_label, 0, 0);
		this->print_mid_code(mid);
		return;
	}
	// ȱ��default��ֱ��������һ�������������
	mid = this->push_mid_code(LABEL_M, end_label, 0, 0);
	this->print_mid_code(mid);
	this->skip(STAT_BEGIN_AND_END,STAT_BEGIN_AND_END_NUM);
	return;
}

void Compiler::one_case(tokenType cal_type,string* end_label,string* next_label,string*switch_name) {
	Midcode* mid;
	GRM_OUT << "In Line " << lineNum +1 << " :  " << "This is a one case statement !" << endl;
	this->getSym();
	// �����֣�������Ԫʽ��
	if (this->curToken == NUMUSR || this->curToken == PLUSSY || this->curToken == SUBSY) {
		int value;	string* value_name = new string();
		this->integer(&value);
		this->int2string(value_name, value);
		if (cal_type != INTTK) {
			errorManipulate(NOMEANING, "switch����ı���ʽ��case����ĳ������Ͳ�ƥ�� ��");
			this->skip(CASETK);
			return;
		}
		this->gen_label_name(next_label);
		mid = this->push_mid_code(NEQJ_M, switch_name, value_name, next_label);
		this->print_mid_code(mid);

		//this->getSym();
		this->checkType(COLONSY, NONE);
		this->statement();
		mid = push_mid_code(J_M, end_label, 0, 0);
		this->print_mid_code(mid);
	}
	else if (this->curToken == CHARUSR) {
		// ����switch����ĳ������,���ű�
		int value = this->strToken[0];	string* value_name = new string();
		this->int2string(value_name, value);
		if (cal_type == INTTK) {
			errorManipulate(NOMEANING, "switch����ı���ʽ��case����ĳ������Ͳ�ƥ�䣬��ʽ����ת�� ��");
		}
		else if (cal_type != CHARTK) {
			errorManipulate(NOMEANING, "switch����ı���ʽ��case����ĳ������Ͳ�ƥ�� ��");
			this->skip(CASETK);
			return;
		}
		this->gen_label_name(next_label);
		mid = this->push_mid_code(NEQJ_M, switch_name, value_name, next_label);
		this->print_mid_code(mid);

		this->getSym();
		this->checkType(COLONSY, NONE);
		this->statement();
		mid = push_mid_code(J_M, end_label, 0, 0);
		this->print_mid_code(mid);
	}
	else {
		errorManipulate(NOMEANING, "case����ӵĲ��ǳ��� ��");
		this->skip(SEMICOLONSY);
	}
}

void Compiler::read_st() {
	GRM_OUT << "In Line " << lineNum +1 << " :  " << "This is a scanf statement !" << endl;
	this->getSym();
	if (this->curToken == LSBRASY) {
		do {
			this->getSym();
			if (this->curToken == IDEN) {
				// ���ҷ��ű���������Ԫʽ�Ĺ���
				Item* it = this->get_const_var_item(this->strToken);
				if (it == nullptr) {
					errorManipulate(NOMEANING, "scanf��ȡδ������ţ�");
					this->skip(SEMICOLONSY);
					return;
				}
				else if ( it->getGenre() != VAR) {
					errorManipulate(NOMEANING, "�������������ж��룡");
				}
				else {
					Midcode* mid;
					mid = this->push_mid_code(SCANF_M, &(it->getName()), 0, 0);
					this->print_mid_code(mid);
				}
			}
			else {
				errorManipulate(NOMEANING, "��scanf��������û�б�ʶ�� ��");
			}
			this->getSym();
		} while (this->curToken == COMMASY);
		this->checkType(RSBRASY, NONE);
	}
	else {
		this->skip(SEMICOLONSY);
	}
}
// printf������ضϵĻ�����Ӱ����
void Compiler::write_st() {
	GRM_OUT << "In Line " << lineNum +1 << " :  " << "This is a printf statement ! " << endl;
	this->getSym();
	this->checkType(LSBRASY, NONE);
	// д�ַ���
	if (this->curToken == STRINGUSR) {
		// �ַ�����¼��ȫ���ַ����� LHYûʵ�����֣�
		string* name = new string();
		Midcode* mid;
		this->gen_string_name(name);
		this->string_tab.push_back(this->strToken);//[string_idx++] = this->strToken;
		mid = this->push_mid_code(PRINTS_M, name, 0, 0);
		this->print_mid_code(mid);
		this->getSym();
		if (this->curToken == RSBRASY) {
			this->getSym();
			return;
		}
		this->checkType(COMMASY, NONE);
	}
	// ����ʽ
	bool is_const;
	int value;
	string* tmp_name = new string();	string* tmp_type = new string();
	tokenType cal_type;
	cal_type = this->expr(&is_const, &value, tmp_name);
	// ��½��Ԫʽ
	if (cal_type == CHARTK) {
		*tmp_type = "char";
	}
	else if(cal_type == INTTK){
		*tmp_type = "int";
	}
	Midcode* mid;
	mid = this->push_mid_code(PRINTV_M, tmp_type, tmp_name, 0);
	this->print_mid_code(mid);
	delete(tmp_name);
	// ����ʽ���ַ�����˳�򲻿��Է���������Ȼ�����ַ�����
	if (this->curToken == COMMASY) {
		this->getSym();
		if (this->curToken == STRINGUSR) {
			errorManipulate(NOMEANING, "�����������ַ��������ʽ��˳���ˣ�");
			string* name = new string();
			Midcode* mid;
			this->gen_string_name(name);
			this->string_tab.push_back(this->strToken);// = this->strToken;
			mid = this->push_mid_code(PRINTS_M, name, 0, 0);
			this->print_mid_code(mid);
			this->getSym();
		}
	}
	this->checkType(RSBRASY, NONE);
}

void Compiler::return_st() {
	GRM_OUT << "In Line " << lineNum +1 << " :  " << "This is a return statement !" << endl;
	this->getSym();
	// ����ű�����ʲô�������͵ĺ���
	Midcode* mid;
	tokenType kind = cur_active_funtab->getKind();
	if (this->curToken == SEMICOLONSY) {
		// �޷���ֵ�����ҷ��ű����壡��
		if (kind == VOIDTK) {
			// ����Ͳ����ˣ�������Ҳ������ɶ
			mid = push_mid_code(J_M, &(cur_active_funtab->getName() + "_end"), 0, 0);
			this->print_mid_code(mid);
		}
		else {
			// warning: ��·��û�з���ֵ ��
			mid = push_mid_code(J_M, &(cur_active_funtab->getName() + "_end"), 0, 0);
			this->print_mid_code(mid);
		}
		return;
		// ��Ԫʽ
	}
	else {
		if (kind == VOIDTK) {
			errorManipulate(NOMEANING, "���޷���ֵ�ĺ����е�return�����ڷ���ֵ��");
			this->skip(SEMICOLONSY);
			return;
		}
		this->checkType(LSBRASY, NONE);
		bool is_const;
		int value;
		string* tmp_name = new string();
		tokenType cal_type;
		cal_type = this->expr(&is_const, &value, tmp_name);
		if (cur_active_funtab->getKind() != cal_type)
			this->errorManipulate(NOMEANING, "������ʵ�ʷ���ֵ�����붨�岻����");
		mid = push_mid_code(RETV_M, tmp_name, 0, 0);
		this->print_mid_code(mid);
		mid = push_mid_code(J_M, &(cur_active_funtab->getName() + "_end"), 0, 0);
		this->print_mid_code(mid);
		// ���� ע�⣬ɾ��ʱһ��Ҫ���أ�ֻ���ж�û�г���ʱ�ſ���ɾ��
		// �з���ֵ��֧ + 1
		cur_active_funtab->setValue(cur_active_funtab->getValue() + 1);
		delete(tmp_name);
		// ����Ҳ����
		this->checkType(RSBRASY, NONE);
	}
}

void Compiler::paralist(string name, tokenType type) {
	string paraname;
	tokenType paratype;
	// ����¼���ű��������ű�ָ���Ƿ���ȷ
	if (cur_active_funtab != fun_tab[name]) {
		errorManipulate(NOMEANING, "��ǰ��Ծ����ָ�����¼�����ű���ָ�벻ƥ�䣡");
	}
	// ���϶�������һ���������Ƿ�Ϊint��ʶ��
	do {
		this->getSym();
		paratype = this->curToken;
		switch (paratype) {
			// int��������char����������ʽ��ͬ
		case INTTK:
		case CHARTK:
			this->getSym();
			paraname = this->strToken;
			if (this->checkType(IDEN, NONE)) {
				// ��¼�����������ű�������
				Item* tmp_para = new Item(paraname, PARA, paratype);
				tmp_para->offset = cur_active_funtab->offset++;
				cur_active_funtab->paraList.push_back(*tmp_para);
				cur_active_funtab->paranum++;
			}
			else {
				this->skip(J_DEFAULT, J_DEFAULT_NUM);
			}
			break;
		default:
			this->errorManipulate(NOINTORCHAR);
			break;
		}
	} while (this->curToken == COMMASY);
}

void Compiler::declare_head(string* name,tokenType* retType) {
	switch (this->curToken) {
	case INTTK:
	case CHARTK: *retType = this->curToken; break;
	default:
		// Ĭ��Ϊint���ͣ���������
		this->errorManipulate(NOINTORCHAR);
		*retType = INTTK;
		break;
	}
	// ��ȡ��ʶ��
	this->getSym();
	if (this->curToken == IDEN) {
		*name = this->strToken;
	}
	else if (this->searchReserve(this->strToken) != IDEN && this->searchReserve(this->strToken) != NONE) {
		this->errorManipulate(NOMEANING, "��ʶ���뱣���ֳ�ͻ !");
		string* tmp = new string("#temp#");
		*name = *tmp;
	}
	else {
		this->errorManipulate(NOMEANING, "û��ƥ���ʶ�� !");
		string* tmp = new string("#temp#");
		*name = *tmp;
	}
	this->getSym();
}
tokenType Compiler::expr(bool* is_const, int* value, string* tmp_name) {
	Midcode* mid;
	tokenType pre_op = PLUSSY;
	// ǰһ����Ĳ���
	tokenType first_cal_type, next_cal_type;
	bool first_is_const, next_is_const;
	int first_value, next_value;
	string* first_tmp_name = new string();
	string* next_tmp_name = new string();
	// [+/-] �����������ķ��Ķ����ԣ�����һ�����ֵ���+/-��鵽������ı���ʽ��
	if (this->curToken == SUBSY) {
		pre_op = SUBSY;
		this->getSym();
	}
	else if (this->curToken == PLUSSY) {
		pre_op = PLUSSY;
		this->getSym();
	}
	// ��һ������������getSym
	// ������int�������Ҫ��Ϸ��ű�
	GRM_OUT << "In Line " << lineNum +1 << " :  " << "This is an expression !" << endl;
	if ((first_cal_type = this->item(&first_is_const, &first_value, first_tmp_name)) != VOIDTK) {
		// �еڶ���
		if (pre_op==SUBSY) {
			if (first_is_const) {
				first_value = -first_value;
				this->int2string(first_tmp_name, first_value);
			}
			else {
				string* first_ttmp_name = new string();
				this->gen_tmp_name(first_ttmp_name);
				mid = this->push_mid_code(SUB_M, new string("0"), first_tmp_name, first_ttmp_name);
				this->print_mid_code(mid);
				*first_tmp_name = *first_ttmp_name;
			}
		}
		while (this->curToken == PLUSSY || this->curToken == SUBSY) {
			tokenType op_type = this->curToken;
			this->getSym();
			next_cal_type = this->item(&next_is_const, &next_value, next_tmp_name);
			if (next_cal_type == VOIDTK) {
				//errorManipulate(NOMEANING,"����ʽ�г��ֲ�����ֵ�� ��");
				break;
			}
			*is_const = first_is_const && next_is_const;
			if (*is_const) {
				*value = (op_type == PLUSSY) ? (first_value + next_value) : (first_value - next_value);
				this->int2string(tmp_name, *value);
				if (this->isVarType(first_cal_type) && this->isVarType(next_cal_type)) {
					first_cal_type = INTTK;
				}
				first_is_const = *is_const;	first_value = *value;	*first_tmp_name = *tmp_name;
			}
			else if (first_is_const || next_is_const) {
				this->gen_tmp_name(tmp_name);
				//this->put_var(*tmp_name, INTTK, 0, 0);
				*value = 0;
				// ��ƫ�ƣ�������Ԫʽ if(first_is_const)...
				if (op_type == PLUSSY) {
					mid = this->push_mid_code(ADD_M, first_tmp_name, next_tmp_name, tmp_name);
					this->print_mid_code(mid);
				}
				else {
					mid = this->push_mid_code(SUB_M, first_tmp_name, next_tmp_name, tmp_name);
					this->print_mid_code(mid);
				}
				first_is_const = *is_const;	first_value = *value;	*first_tmp_name = *tmp_name;
				first_cal_type = INTTK;
			}
			else {
				// �����ѷ���ı�������ƫ�ƣ�������Ԫʽ
				this->gen_tmp_name(tmp_name);
				//this->put_var(*tmp_name, INTTK, 0, 0);
				if (op_type == PLUSSY) {
					mid = this->push_mid_code(ADD_M, first_tmp_name, next_tmp_name, tmp_name);
					this->print_mid_code(mid);
				}
				else {
					mid = this->push_mid_code(SUB_M, first_tmp_name, next_tmp_name, tmp_name);
					this->print_mid_code(mid);
				}
				first_is_const = *is_const;	first_value = *value;	*first_tmp_name = *tmp_name;
				first_cal_type = INTTK;
			}
		}
		*is_const = first_is_const;	*value = first_value;	*tmp_name = *first_tmp_name;
		return first_cal_type;
	}
	else {
		//�����Ҫ�����������
		//errorManipulate(NOMEANING, "����ʽ�г��ֲ�����ֵ�� ��");
		this->skip(SEMICOLONSY);
		return first_cal_type;
	}
}

			// �ϲ���һ��ͺ���Ķ�������
			/**is_const = first_is_const & next_is_const;
			*value = (first_is_const) ? ((this->curToken == PLUSSY) ? (first_value+next_value) : (first_value - next_value)) : (0);
			if (*is_const) {
				first_is_const = *is_const;	first_value = *value;	first_tmp_name = nullptr;
			}
			else if (first_is_const || next_is_const) {
				this->gen_tmp_name(tmp_name);
				this->put_var(*tmp_name, INTTK, 0, 0);
				// ��ƫ�ƣ�������Ԫʽ if(first_is_const)...
				first_is_const = *is_const;	first_value = *value;	*first_tmp_name = *tmp_name;
				first_cal_type = INTTK;
			}
			else {
				// �����ѷ���ı�������ƫ�ƣ�������Ԫʽ
			}
		}
		*is_const = first_is_const;	*value = first_value;	*tmp_name = *first_tmp_name;
		return first_cal_type;
	}
	// ���ص����ַ�
	else if (first_cal_type == CHARTK) {

	}
	else {
		//�����Ҫ�����������
		errorManipulate(NOMEANING, "����ʽ�г��ֲ�����ֵ�� ��");
		this->getSym();
		return first_cal_type;
	}
}*/

tokenType Compiler::item(bool* is_const, int* value, string* tmp_name) {
	Midcode* mid;
	// ��¼��һ�����ӵĶ���
	tokenType first_cal_type, next_cal_type;
	bool first_is_const, next_is_const;
	int first_value, next_value;
	string* first_tmp_name = new string();
	string* next_tmp_name = new string();
	// ��һ��
	// ������int���͵Ľ����ǰ�����ߺϲ������򱨴�������������в���ȥ��
	//this->getSym();
	if ((first_cal_type = this->factor(&first_is_const, &first_value, first_tmp_name)) != VOIDTK) {
		// ����Ҫ�ѷ���ֵ�ó���
		if (*first_tmp_name == *Ret) {
			string* ret_store = new string();
			this->gen_tmp_name(ret_store);
			mid = this->push_mid_code(ASS_M, first_tmp_name, 0, ret_store);
			this->print_mid_code(mid);
			*first_tmp_name = *ret_store;
		}
		// �еڶ���
		while (this->curToken == MULTSY || this->curToken == DIVSY) {
			tokenType op_type = this->curToken;
			this->getSym();
			if ((next_cal_type = this->factor(&next_is_const, &next_value, next_tmp_name)) == VOIDTK)
				break;
			if (*next_tmp_name == *Ret) {
				string* ret_store = new string();
				this->gen_tmp_name(ret_store);
				mid = this->push_mid_code(ASS_M, next_tmp_name, 0, ret_store);
				this->print_mid_code(mid);
				*next_tmp_name = *ret_store;
			}
			// �ϲ���һ��ͺ���Ķ�������
			*is_const = first_is_const && next_is_const;
			if (*is_const) {
				*value = (op_type == MULTSY) ? (first_value*next_value) : ((next_value!=0)?((int)(first_value / next_value)):(first_value));
				if (op_type == DIVSY && next_value == 0)
					this->errorManipulate(NOMEANING, "����0�쳣 ��");
				this->int2string(tmp_name, *value);
				if (this->isVarType(first_cal_type) && this->isVarType(next_cal_type)) {
					first_cal_type = INTTK;
				}
				first_is_const = *is_const;	first_value = *value;	*first_tmp_name = *tmp_name;
			}
			else if (first_is_const || next_is_const) {
				this->gen_tmp_name(tmp_name);
				//this->put_var(*tmp_name, INTTK, 0, 0);
				*value = 0;
				// ��ƫ�ƣ�������Ԫʽ if(first_is_const)...
				if (op_type == MULTSY) {
					mid = this->push_mid_code(MUL_M, first_tmp_name, next_tmp_name, tmp_name);
					this->print_mid_code(mid);
				}
				else {
					mid = this->push_mid_code(DIV_M, first_tmp_name, next_tmp_name, tmp_name);
					this->print_mid_code(mid);
				}
				first_is_const = *is_const;	first_value = *value;	*first_tmp_name = *tmp_name;
				first_cal_type = INTTK;
			}
			else {
				// �����ѷ���ı�������ƫ�ƣ�������Ԫʽ
				this->gen_tmp_name(tmp_name);
				//this->put_var(*tmp_name, INTTK, 0, 0);
				if (op_type == MULTSY) {
					mid = this->push_mid_code(MUL_M, first_tmp_name, next_tmp_name, tmp_name);
					this->print_mid_code(mid);
				}
				else {
					mid = this->push_mid_code(DIV_M, first_tmp_name, next_tmp_name, tmp_name);
					this->print_mid_code(mid);
				}
				first_is_const = *is_const;	first_value = *value;	*first_tmp_name = *tmp_name;
				first_cal_type = INTTK;
			}
		}
		*is_const = first_is_const;	*value = first_value;	*tmp_name = *first_tmp_name;
		return first_cal_type;
	}
	else {
		//�����Ҫ�����������
		errorManipulate(NOMEANING, "����ʽ�г��ֲ�����ֵ�� ��");
		this->skip(SEMICOLONSY);
		return first_cal_type;
	}
}
// ǰ�汻���ȶ�������������ȶ�
tokenType Compiler::factor(bool* is_const, int* value, string* tmp_name) {
	// ������ʶ����Ҫ����ű�
	tokenType  factor_type;
	Midcode* mid;
	// '('
	if (this->curToken == LSBRASY) {
		this->getSym();
		// �жϴ���ֵ����
		factor_type = expr(is_const, value, tmp_name);
		// ����
		// ƥ����С���Ų����ص�ǰ����ʽ����ֵ
		this->checkType(RSBRASY, NONE);
		// ������һ�����㣬���б������ͺϷ���ǿ�Ʒ���int
		if (factor_type == INTTK || factor_type == CHARTK) {
			factor_type = INTTK;
		}
		return factor_type;
	}
	// ����
	else if (this->curToken == NUMUSR || this->curToken == PLUSSY || this->curToken == SUBSY) {
		// �ǳ�ֵ�����ô����Ķ�Ӧ��
		*is_const = true;
		//this->gen_tmp_name(tmp_name);
		this->integer(value);	this->int2string(tmp_name, *value);
		return INTTK;
	}
	// �ַ�
	else if (this->curToken == CHARUSR) {
		*is_const = true;
		//this->gen_tmp_name(tmp_name); �͵���������
		*value = this->strToken[0];	this->int2string(tmp_name, *value);
		this->getSym();
		return CHARTK;//?
	}
	// ��ʶ��
	else if (this->curToken == IDEN) {
		// ����Ӧ�ò���ű���ȷ����ǰ��ʶ������ʲô���﷨���ȷ����﷨����
		string name = this->strToken;
		this->getSym();
		// ֵ����������Ҫ����ű�,�������ҳ���
		if (this->curToken == LSBRASY) {
			FuncItem* func_it= this->get_func_item(name);
			if (func_it != nullptr) {
				
				this->fun_call(func_it);				// ��Ȼ�Ѿ���½���ű��ˣ���Ҳ�Ͳ���name��
				mid = this->push_mid_code(CALL_M, &name, 0, 0);
				this->print_mid_code(mid);
				// ��ʶ�˴���ֵ��Ҫ�ӷ���ֵ��ȡ��
				*tmp_name = *Ret;	*is_const = false;	*value = 0;
				return (func_it->getKind());				// ���ű�����ʵ��Ҫ�����ű��еķ���ֵ
			}
			return VOIDTK;
		}
		// �����������Ҫ����ű���Խ�磬����
		else if (this->curToken == LMBRASY) {
			Item* it = this->get_const_var_item(name);
			if (it == nullptr) {
				errorManipulate(NOMEANING, "�������δ���壡");
				return VOIDTK;
			}
			this->getSym();
			bool idx_is_const;
			int idx_value;
			string* idx_tmp_name = new string();
			tokenType idx_cal_type;
			// �������±��Ƿ���Խ���Ϊ��ֵ
			idx_cal_type = this->expr(&idx_is_const, &idx_value, idx_tmp_name);
			if (idx_is_const && (idx_value >= it->getLength() || idx_value < 0)) {
				errorManipulate(NOMEANING, "���õ����鳬���˲��ںϷ������±귶Χ�ڣ�");
				if (idx_value < 0)
					idx_value = -idx_value;
			}
			string* arr_name = new string();
			this->gen_tmp_name(arr_name);
			// ����Ԫʽ arr_namr = name[idx_tmp_name]
			mid = this->push_mid_code(ARTO_M, &name, idx_tmp_name, arr_name);
			this->print_mid_code(mid);
			// ���Ϸ��ز�����ֵ
			*is_const = false;	*value = 0;	*tmp_name = *arr_name;
			this->checkType(RMBRASY, NONE);
			return it->getKind();
		}
		// �����������޲��з���ֵ����
		else {
			Item* it;
			// �Ǳ�������
			if ((it=this->get_const_var_item(name))!=nullptr) {
				if (it->getGenre() == VAR || it->getGenre() == PARA) {
					// ��¼��Ԫʽ��
					// �ж��Ƿ���ȫ�ֱ�����
					string* glo_name = new string();
					if (it->global) {
						this->gen_tmp_name(glo_name);
						mid = this->push_mid_code(ASS_M, &name, 0, glo_name);
						this->print_mid_code(mid);
					}
					*is_const = false;	*value = 0;	*tmp_name = (it->global) ? (*glo_name) : (name);
					return it->getKind();
				}
				// ����
				else {
					*is_const = true;	*value = it->getValue();	this->int2string(tmp_name, *value);
					return it->getKind();
				}
			}
			// �޲κ���
			FuncItem* it_f;
			if ((it_f = this->get_func_item(name)) != nullptr) {
				mid = this->push_mid_code(CALL_M, &name, 0, 0);
				this->print_mid_code(mid);
				*is_const = false;	*value = 0;	*tmp_name = *Ret;
				return (it_f->getKind());
			}
			else {
				this->skip(J_DEFAULT, J_DEFAULT_NUM);
			}
		}
	}
	else {
		errorManipulate(NOMEANING, "����ʽ�г��ֲ������﷨������ ��");
		return VOIDTK;
	}
	
}