#include "Compiler.h"

#define DEBUG (0)
#if DEBUG
#define GRM_OUT cout
#else
#define GRM_OUT gram_fout
#endif //DEBUG

/*******************************************************************************\
 *																			   *
 *								语法分析实现函数							   *
 *																			   *
\*******************************************************************************/

// 语法分析目标作业目标位置输出
ofstream gram_fout("output/gram_out.txt");

// 定义对应跳读集和查找集
// 跳读集
const tokenType J_DEFAULT[] = { SEMICOLONSY, COMMASY, COLONSY, PLUSSY, SUBSY, MULTSY, DIVSY, LBBRASY,LSBRASY,LMBRASY,RSBRASY,RMBRASY,RBBRASY };
const int J_DEFAULT_NUM = 13;

// 查找集
const tokenType VAR_BEGIN[] = {LMBRASY,SEMICOLONSY,COMMASY};
const int VAR_BEGIN_NUM = 3;

const tokenType STAT_BEGIN[] = {IFTK, DOTK, SWITCHTK, SCANFTK, PRINTFTK, RETURNTK, IDEN, LBBRASY, SEMICOLONSY};
const int STAT_BEGIN_NUM = 9;

const tokenType STAT_BEGIN_AND_END[] = { IFTK, DOTK, SWITCHTK, SCANFTK, PRINTFTK, RETURNTK, IDEN, LBBRASY, RBBRASY, SEMICOLONSY };
const int STAT_BEGIN_AND_END_NUM = 10;

const tokenType FUNC_BEGIN[] = {VOIDTK, INTTK, CHARTK};
const int FUNC_BEGIN_NUM = 3;

// 返回值"寄存器"
string* Ret = new string("%RET");

// 符号表查找函数
bool Compiler::check_defined(string name) {
	map<string, Item*>::iterator it;
	map<string, FuncItem*>::iterator it_f;
	// 局部函数表
	if (cur_active_funtab!=nullptr) {
		Item* ite;
		if ((ite = cur_active_funtab->get_const(name)) != nullptr)
			return true;
		if ((ite = cur_active_funtab->get_var(name)) != nullptr)
			return true;
		if ((it_f = fun_tab.find(name)) != fun_tab.end() && it_f->second->paranum == 0)	// 后面：有参数
			return true;
		return false;
	}
	// 全局函数为空，则返回未找到
	if (fun_tab.size()==0)
		return false;
	// 定义全局函数
	if ((it_f = fun_tab.find(name)) != fun_tab.end())	// 后面：有参数
		return true;
	// 遍历所有函数，不能与变量常量重名
	for (it_f = fun_tab.begin();it_f != fun_tab.end();it_f++) {
		Item* ite;
		if ((ite = it_f->second->get_const(name)) != nullptr || (ite = it_f->second->get_const(name)) != nullptr)
			return true;
	}
	// 全局常量与变量
	if ((it = global_const_tab.find(name))!=global_const_tab.end())
		return true;
	if ((it = global_var_tab.find(name)) != global_var_tab.end())
		return true;
	// 不知道这么改行不
	//if (this->get_const_var_item != nullptr && this->get_func_item != nullptr)
	//	return true;
	return false;
}

Item* Compiler::get_const_var_item(string name) {
	map<string, Item*>::iterator it;
	// 局部函数表
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
	//errorManipulate(NOMEANING, "变量"+name+"未定义！");
	//exit(0);
	return nullptr;
}

FuncItem* Compiler::get_func_item(string name) {
	map<string, FuncItem*>::iterator it_f = fun_tab.find(name);

	if (it_f != fun_tab.end())	// 后面？：有参数的可以重用
		return it_f->second;
	errorManipulate(NOMEANING, "函数或变量未定义！");
	//exit(0);
	return nullptr;
}

void Compiler::put_const(string name, tokenType kind, int value) {
	// 没有预先定义
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
		errorManipulate(NOMEANING, "想要定义的全局常量 "+name+" 重定义 ！");
	}
}

void Compiler::put_var(string name, tokenType kind, int value, int length) {
	// 没有预先定义，初始变量值为-1
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
		errorManipulate(NOMEANING, "想要定义的变量 " + name + " 重定义 ！");
	}
}

void Compiler::put_func(string name, Genre genre, tokenType kind,int paranum) {
	this->cur_active_funtab = nullptr;	//不能和所有定义变量重名？？
	// 没有预先定义，先不管参数，设为-1，只登陆函数名
	if (!this->check_defined(name)) {
		FuncItem* new_func = new FuncItem(name, genre, kind, paranum);
		fun_tab[name] = new_func;
		// 初始化函数所用变量个数
		fun_tab[name]->offset = 0;
		cur_active_funtab = new_func;
	}
	else {
		errorManipulate(NOMEANING, "想要定义的函数 " + name + " 重定义！");
	}
}

void Compiler::print_symbol_item() {
	map<string, Item*>::iterator iter;
	map<string, FuncItem*>::iterator iter_f;
	GRM_OUT << "--------GLOBAL CONST--------" << endl;
	GRM_OUT << "名称		类型		值		偏移" << endl;
	for (iter = global_const_tab.begin();iter != global_const_tab.end();iter++) {
		GRM_OUT << iter->second->getName() << "  " << iter->second->getKind() << "  " << iter->second->getValue() << "  " << iter->second->offset << endl;
	}
	GRM_OUT << endl;
	GRM_OUT << "--------GLOBAL VAR--------" << endl;
	GRM_OUT << "名称		类型		长度		偏移" << endl;
	GRM_OUT << "GLOBAL_VAR_OFFSET : " << this->global_var_offset << endl;
	for (iter = global_var_tab.begin();iter != global_var_tab.end();iter++) {
		GRM_OUT << iter->second->getName() << "  " << iter->second->getKind() << "  " << iter->second->getLength() << "  " << iter->second->offset << endl;
	}
	GRM_OUT << endl;
	GRM_OUT << "--------GLOBAL FUN--------" << endl;
	for(iter_f = fun_tab.begin();iter_f!=fun_tab.end();iter_f++){
		GRM_OUT << "函数名称		返回值	  有无参数	  总偏移" << endl;
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
	// 匹配成功
	if (this->curToken == id) {								
		this->getSym();
		return true;
	}
	else {
		string s;
		// 根据传来的token类型报错
		if (id < MaxReserve)
			s = reserve[id];
		else if (id < MaxOp)
			s = op[id - MaxReserve];
		else
			s = usr[id - MaxReserve - MaxOp];
		this->errorManipulate(NOMEANING, "没有匹配 "+s+ " !");		
		return false;
		//this->skip(skp);
	}
}

void Compiler::program() {
	//读入第一个单词
	this->getSym();
	// 判断首先是否有全局const声明
	if (this->curToken == CONSTTK) {
		// 全局const开始的程序
		this->const_declare();
	}
	if (isVarType()) {
		// 全局变量或有返回值函数开始的程序
		// 记录当前的变量或函数返回值类型与名字
		string name;
		tokenType retType;
		Midcode* mid;
		// 利用声明头部获取类型与名字
		this->declare_head(&name, &retType);
		// 是否是变量定义
		while (find_in_sets<const tokenType[],tokenType,int>(VAR_BEGIN,this->curToken,VAR_BEGIN_NUM)) {
			this->var_def(name, retType);	// 里面对登录符号表
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
			// 以void函数或主函数开头的程序
			this->fun_def();
		}
		// 第一个有参数的函数定义
		else if (this->curToken == LSBRASY) {
			GRM_OUT << "In Line " << lineNum +1 << " :  " << "This is a customerized function definition !" << endl;
			// 参数登录函数符号表，先查重， name，retType
			this->put_func(name, PARA_FUNC, retType,0);
			mid = this->push_mid_code(FUN_M, &name, 0, 0);
			this->print_mid_code(mid);
			// 进入参数表
			this->paralist(name, retType);
			this->checkType(RSBRASY, NONE);
			
			
			if (this->checkType(LBBRASY, NONE)) {
				// 第一个函数定义的复合语句
				this->compoundstat();
				// 函数结束标志
				if (this->checkType(RBBRASY, NONE)) {
					// 有返回值函数的返回路径是否存在
					if (this->isVarType(retType) && cur_active_funtab->getValue() <= 0) {
						errorManipulate(NOMEANING,"有返回值函数存在没有分支返回的情况 ！");
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
				// 跳到合适地点
				this->skip(J_DEFAULT, J_DEFAULT_NUM);
			}
		}
		// 第一个无参数的函数定义
		else if (this->curToken == LBBRASY) {
			GRM_OUT << "In Line " << lineNum +1 << " :  " << "This is a customerized function definition !" << endl;
			this->put_func(name, NO_PARA_FUNC, retType, 0);
			mid = this->push_mid_code(FUN_M, &name, 0, 0);
			this->print_mid_code(mid);
			this->getSym();
			this->compoundstat();
			this->checkType(RBBRASY, NONE);
			if (this->isVarType(retType) && cur_active_funtab->getValue() <= 0) {
				errorManipulate(NOMEANING, "有返回值函数存在没有分支返回的情况 ！");
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
		// 不管var还是func这两个都要有
		if (checkType(IDEN, SEMICOLONSY)) {
			// 到了这列才可以区分出函数定义与变量定义
			// 有参有返回值函数
			if (this->curToken == LSBRASY ) {
				this->getSym();
				this->paralist();
			}
			// 无参有返回值函数
			else if (this->curToken == LBBRASY) {
				this->getSym();
				this->compoundstat();
			}
			// 数组或变量定义 封装！！
			else if (this->curToken == LMBRASY || this->curToken == SEMICOLONSY || this->curToken == COMMASY) {
				do {
					if (this->curToken == LMBRASY) {
						this->getSym();
						if (checkType(NUMUSR, SEMICOLONSY)) {
							if (checkType(RMBRASY, SEMICOLONSY)) {
								// 登陆符号表
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
						//登陆符号表
						continue;
					}
					
				} while ();
				
			}
			else {
				errorManipulate(NOMEANING, "标识符结尾不匹配 ！");
				this->getSym();
			}
			
		}
	}*/
	else if (this->curToken == VOIDTK) {
		// 以void函数或主函数开头的程序
		this->fun_def();
	}
	else {
		// 不合法的函数开始，报错
		this->errorManipulate(ILLEGALVARTYPE);
		this->skip(J_DEFAULT, J_DEFAULT_NUM);
	}
	this->print_symbol_item();
}
// <变量定义>
void Compiler::var_def(string name, tokenType retType) {
	// 如果是分号，则登陆符号表走人
	if (this->curToken == SEMICOLONSY) {
		GRM_OUT << "In Line " << lineNum +1 << " :  " << "This is a var definition !" << endl;
		// 登录符号表！！！
		this->put_var(name, retType, -1, 0);
		return;
	}
	bool arr_flag = false;
	if (this->curToken == LMBRASY) {
		GRM_OUT << "In Line " << lineNum +1 << " :  " << "This is a var definition !" << endl;
		this->getSym();
		// 进行整数解析
		int length;
		this->integer(&length);
		if (length <= 0)
			this->errorManipulate(NOMEANING, "数组定义时下标不是正整数 ！");
		// 将数组登入符号表
		this->put_var(name, retType, -1, length);
		this->checkType(RMBRASY,NONE);
		arr_flag = true;
	}
	// 有逗号存在，则有多个变量
	if (this->curToken == COMMASY) {
		GRM_OUT << "In Line " << lineNum +1 << " :  " << "This is a var definition !" << endl;
		// 对不是数组变量类型 填符号表
		if(!arr_flag)
			this->put_var(name, retType, -1, 0);
		// 判断下一个是不是标识符
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
		this->const_def();								// 这里要填入符号表
		if (this->curToken == SEMICOLONSY)
			this->getSym();
		else {
			errorManipulate(LACKSEMICOLON);
			this->skip(J_DEFAULT, J_DEFAULT_NUM);						// 如何修改一下，不让他报错
			this->getSym();
		}
	} while (this->curToken == CONSTTK);
	//GRM_OUT << "In Line " << lineNum +1 << " :  " << "This is a const declaration !" << endl;
}

void Compiler::const_def() {
	if (this->curToken == INTTK) {
		do {
			GRM_OUT << "In Line " << lineNum +1 << " :  " << "This is a const definition !" << endl;
			// 标识符类型匹配
			this->getSym();
			string name = this->strToken;				// 提前保留标识符
			if (checkType(IDEN, SEMICOLONSY)) {			// checkType会向后读一个单词
				if (checkType(ASSIGNSY, SEMICOLONSY)) {
					int value;
					this->integer(&value);
					// 登陆符号表！！！
					this->put_const(name, INTTK, value);
					// 四元式！
					
				}
			}
		} while (this->curToken == COMMASY);
	}
	else if (this->curToken == CHARTK) {
		do {
			GRM_OUT << "In Line " << lineNum +1 << " :  " << "This is a const definition !" << endl;
			// 标识符类型匹配
			this->getSym();
			string name = this->strToken;
			if (checkType(IDEN, SEMICOLONSY)) {			// checkType会向后读一个单词
				if (checkType(ASSIGNSY, SEMICOLONSY)) {
					if (this->curToken == CHARUSR){
						//登入符号表！！！
						this->put_const(name, CHARTK, strToken[0]);
						checkType(CHARUSR, SEMICOLONSY);
					}
				}
			}
		} while (this->curToken == COMMASY);
	}
	else {
		errorManipulate(NOMEANING, "没有匹配类型标识符 ！");
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
	//这里不用checkType了，需要跳转的地方比较多
	if (this->curToken == NUMUSR) {	
		if (neg) {
			*value = -this->intgNum;
		}
		else
			*value = this->intgNum;
		if (*value > MaxInt) {
			errorManipulate(NOMEANING, "整型数据溢出 !");
			//*value = MaxInt;
		}
		// 登入符号表！！！，整数不用登入符号表。。。连带这检查了整数类型
		//this->intgNum = this->intgNum;	//假的登入
		
		this->getSym();
	}
	else {
		*value = this->intgNum;
		// 看一下具体跳到了哪里
		this->skip(J_DEFAULT, J_DEFAULT_NUM);
	}
}

// 跳过了声明，真正的函数部分定义
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
				errorManipulate(NOMEANING, "没有函数类型定义 ！");
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
		errorManipulate(NOMEANING, "没有main函数 ！");
	}
}

void Compiler::void_iden_fun(string name,tokenType retType){
	Midcode* mid;
	// 有小括号，出现参数
	if (this->curToken == LSBRASY) {
		// 传符号表
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
	// 登录有返回值函数符号表
	this->void_iden_fun(name, retType);
	if (this->isVarType(retType) && cur_active_funtab->getValue() <= 0) {
		errorManipulate(NOMEANING, "有返回值函数存在没有分支返回的情况 ！");
	}
}

void Compiler::main_fun(string name, tokenType retType) {
	this->have_main = true;
	Midcode* mid;
	// 登录主符号表！
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
	// 利用声明头部获取类型与名字
	this->declare_head(&name, &retType);
	// 是否是变量定义
	while (this->find_in_sets<const tokenType[], tokenType, int>(VAR_BEGIN, this->curToken, VAR_BEGIN_NUM)) {
		this->var_def(name, retType);	// 里面对登录符号表
		if (checkType(SEMICOLONSY, NONE)) {
			if (isVarType()) {
				this->declare_head(&name, &retType);
			}
			// 返回，遇到非变量定义部分
		}
		else {
			this->skip(J_DEFAULT, J_DEFAULT_NUM);
		}
	}
}

void Compiler::compoundstat() {
	if (this->curToken == CONSTTK) {
		// 函数内const开始的复合语句
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
	// 处理各种语句
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
		// 查符号表进行维护先确定标识符名字
		this->getSym();
		// 有参调用
		FuncItem* it_f;
		if (this->curToken == LSBRASY) {
			GRM_OUT << "In Line " << lineNum +1 << " :  " << "This is a function call with some parameters !" << endl;
			it_f= this->get_func_item(name);
			if (it_f != nullptr) {
				// 检查是否是无参函数调用括号
				if (it_f->getGenre() == NO_PARA_FUNC) {
					errorManipulate(NOMEANING, "无参函数在调用时加入小括号！");
				}
				this->fun_call(it_f);
				mid = this->push_mid_code(CALL_M, &name, 0, 0);
				this->print_mid_code(mid);
			}
			else {
				this->skip(J_DEFAULT, J_DEFAULT_NUM);
			}
		}
		// 无参调用
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
		// 可能是赋值语句，函数内部处理
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
		errorManipulate(NOMEANING, "无法识别的语句符号 ！");
	}
}

void Compiler::fun_call(FuncItem* func_it) {
	// 参数序号，只处理值参数表
	int i = 0;
	// 表达式轰炸
	bool is_const;
	int value;
	string* tmp_name = new string();
	tokenType cal_type;
	do {
		this->getSym();
		// 数量不匹配，跳出
		if (func_it->paranum <= i) {
			errorManipulate(NOMEANING,"调用函数时参数个数不匹配！");
			this->skip(J_DEFAULT,J_DEFAULT_NUM);
			break;
		}
		cal_type = this->expr(&is_const, &value, tmp_name);
		// 函数参数类型匹配
		if (cal_type != func_it->paraList[i++].getKind())
			errorManipulate(NOMEANING, "函数 " + func_it->getName() + " 的第 " + std::to_string(i) + " 个参数类型不匹配！");
		// 参数是常数的话直接返回值
		if (is_const)
			this->int2string(tmp_name, value);
		Midcode* mid;
		mid = push_mid_code(PUSH_M, tmp_name, &func_it->getName(), &int22string(i-1));
		this->print_mid_code(mid);
	} while (this->curToken == COMMASY);
	if (i < func_it->paranum) {
		this->errorManipulate(NOMEANING, "调用函数时参数个数不匹配！");
	}
	delete(tmp_name);
	this->checkType(RSBRASY,NONE);
}

void Compiler::assign_st(string* name) {
	// 查找符号表，对传入的name
	Item* it = get_const_var_item(*name);
	if (it == nullptr) {
		errorManipulate(NOMEANING, "变量" + *name + "未定义");
		this->skip(SEMICOLONSY);
		return;
	}
	if (it->getGenre() == CONST) {
		errorManipulate(NOMEANING, "对常量赋值！");
		this->skip(SEMICOLONSY);
		return;
	}
	Midcode* mid;
	// 标识符是数组
	string* arr_idx_name = new string();
	bool arr_flag = false;
	GRM_OUT << "In Line " << lineNum +1 << " :  " << "This is an assign statement !" << endl;
	if (this->curToken == LMBRASY) {
		if (!(it->getGenre() == VAR && it->getLength() > 0) ){
			errorManipulate(NOMEANING, "标识符不是数组类型，不匹配 ！");
			this->skip(SEMICOLONSY);
			return;
		}
		arr_flag = true;
		bool is_const;
		int value;
		tokenType cal_type;
		this->getSym();
		cal_type = this->expr(&is_const, &value, arr_idx_name);
		// 数组越界操作,返回常值可以判断，否则不能做越界判断
		if (cal_type == INTTK && is_const) {
			// 发生越界
			if (it->getLength() <= value || value <0) {
				errorManipulate(NOMEANING, "目标数组发生数组越界 ！");
			}
		}
		if (cal_type != INTTK) {
			errorManipulate(NOMEANING, "数组下标引用值不是int类型 ！");
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
			errorManipulate(NOMEANING, "向函数进行赋值 ！");
			this->skip(SEMICOLONSY);
			return;
		}
		if (as_cal_type != it->getKind()) {
			errorManipulate(NOMEANING, "赋值号两边单词类型不一致 ！");
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
	// 这些用于四元式的生成
	Midcode* mid;
	GRM_OUT << "In Line " << lineNum +1 << " :  " << "This is an if-else statement !" << endl;
	this->getSym();
	this->checkType(LSBRASY, NONE);
	bool cond_certain;
	int cond_value = 0;
	string* cond_label = new string();
	this->gen_label_name(cond_label);
	// 生成四元式
	this->cond(&cond_certain,&cond_value,cond_label);
	this->checkType(RSBRASY, NONE);
	this->statement();
	string* tar_label = new string();
	this->gen_label_name(tar_label);
	mid = this->push_mid_code(J_M, tar_label, 0, 0);
	this->print_mid_code(mid);
	if (this->checkType(ELSETK, NONE)) {
		// 四元式生成
		mid = this->push_mid_code(LABEL_M, cond_label, 0, 0);
		this->print_mid_code(mid);
		this->statement();
		mid = this->push_mid_code(LABEL_M, tar_label, 0, 0);
		this->print_mid_code(mid);
	}

	delete(cond_label);
}

void Compiler::cond(bool* certain, int* is_while, string* label) {
	/* cond 会截住类型匹配的错误 */
	// 比较运算符 token 记录
	Midcode* mid;
	tokenType cmp_op;
	// 左右两侧的数值记录
	tokenType left_type, right_type;
	bool left_certain = false, right_certain = false;
	int left_value = 0, right_value = 0;
	string* left_name = new string();
	string* right_name = new string();
	// 有了符号表之后才会报这个类型检测的错
	left_type = this->expr(&left_certain, &left_value, left_name);
	if (left_type != INTTK) {
		this->errorManipulate(NOMEANING, "条件表达式的左侧不是int类型 ！");
	}
	// 查看比较符
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
		// 即使后面有东西也不管了，当做单表达式处理
		mid = (*is_while==0)?(this->push_mid_code(EQJ_M, left_name, new string("0"), label)):(this->push_mid_code(NEQJ_M, left_name, new string("0"), label));
		this->print_mid_code(mid);
		*certain = left_certain;
		//*value = left_value;
		return;
	}
	right_type = this->expr(&right_certain, &right_value, right_name);
	if (right_type != INTTK) {
		this->errorManipulate(NOMEANING, "条件表达式的右侧不是int类型 ！");
	}
	// 左右的值是否都可求值，用于四元式
	//if (left_certain && right_certain) {
		// 直接生成四元式
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
	// 生成四元式
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
	// 匹配'(
	GRM_OUT << "In Line " << lineNum +1 << " :  " << "This is a switch-case-default statement !" << endl;
	this->getSym();
	this->checkType(LSBRASY, NONE);
	bool is_const;
	int value;
	string* switch_name = new string();
	tokenType cal_type;
	cal_type = this->expr(&is_const, &value, switch_name);
	// 匹配')
	this->checkType(RSBRASY, NONE);
	this->gen_label_name(end_label);
	this->checkType(LBBRASY, NONE);
	// 情况表
	while (this->curToken == CASETK) {
		if (*next_label != "default") {
			mid = this->push_mid_code(LABEL_M, next_label, 0, 0);
			this->print_mid_code(mid);
		}
		this->one_case(cal_type,end_label,next_label,switch_name);		//里面有符号表的检查和四元式的登入，要把tmp_name，下一个label拿过去
	}
	// default语句
	if (this->checkType(DEFAULTTK, NONE)) {
		GRM_OUT << "In Line " << lineNum +1 << " :  " << "This is a default statement !" << endl;
		// default标签
		mid = this->push_mid_code(LABEL_M, next_label, 0, 0);
		this->print_mid_code(mid);
		this->checkType(COLONSY, NONE);
		this->statement();
		this->checkType(RBBRASY, NONE);
		mid = this->push_mid_code(LABEL_M, end_label, 0, 0);
		this->print_mid_code(mid);
		return;
	}
	// 缺少default就直接跳到下一个语句或结束程序处
	mid = this->push_mid_code(LABEL_M, end_label, 0, 0);
	this->print_mid_code(mid);
	this->skip(STAT_BEGIN_AND_END,STAT_BEGIN_AND_END_NUM);
	return;
}

void Compiler::one_case(tokenType cal_type,string* end_label,string* next_label,string*switch_name) {
	Midcode* mid;
	GRM_OUT << "In Line " << lineNum +1 << " :  " << "This is a one case statement !" << endl;
	this->getSym();
	// 是数字，生成四元式！
	if (this->curToken == NUMUSR || this->curToken == PLUSSY || this->curToken == SUBSY) {
		int value;	string* value_name = new string();
		this->integer(&value);
		this->int2string(value_name, value);
		if (cal_type != INTTK) {
			errorManipulate(NOMEANING, "switch后面的表达式和case后面的常量类型不匹配 ！");
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
		// 检查和switch后面的常量类别,符号表
		int value = this->strToken[0];	string* value_name = new string();
		this->int2string(value_name, value);
		if (cal_type == INTTK) {
			errorManipulate(NOMEANING, "switch后面的表达式和case后面的常量类型不匹配，隐式类型转换 ！");
		}
		else if (cal_type != CHARTK) {
			errorManipulate(NOMEANING, "switch后面的表达式和case后面的常量类型不匹配 ！");
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
		errorManipulate(NOMEANING, "case后面接的不是常量 ！");
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
				// 查找符号表，进行四元式的构建
				Item* it = this->get_const_var_item(this->strToken);
				if (it == nullptr) {
					errorManipulate(NOMEANING, "scanf读取未定义符号！");
					this->skip(SEMICOLONSY);
					return;
				}
				else if ( it->getGenre() != VAR) {
					errorManipulate(NOMEANING, "向常量或函数名进行读入！");
				}
				else {
					Midcode* mid;
					mid = this->push_mid_code(SCANF_M, &(it->getName()), 0, 0);
					this->print_mid_code(mid);
				}
			}
			else {
				errorManipulate(NOMEANING, "在scanf函数后面没有标识符 ！");
			}
			this->getSym();
		} while (this->curToken == COMMASY);
		this->checkType(RSBRASY, NONE);
	}
	else {
		this->skip(SEMICOLONSY);
	}
}
// printf如果被截断的话很受影响吗？
void Compiler::write_st() {
	GRM_OUT << "In Line " << lineNum +1 << " :  " << "This is a printf statement ! " << endl;
	this->getSym();
	this->checkType(LSBRASY, NONE);
	// 写字符串
	if (this->curToken == STRINGUSR) {
		// 字符串登录到全局字符串表 LHY没实现区分！
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
	// 表达式
	bool is_const;
	int value;
	string* tmp_name = new string();	string* tmp_type = new string();
	tokenType cal_type;
	cal_type = this->expr(&is_const, &value, tmp_name);
	// 登陆四元式
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
	// 表达式和字符串的顺序不可以反，反了依然登入字符串表
	if (this->curToken == COMMASY) {
		this->getSym();
		if (this->curToken == STRINGUSR) {
			errorManipulate(NOMEANING, "在输出语句中字符串与表达式的顺序反了！");
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
	// 查符号表看是什么返回类型的函数
	Midcode* mid;
	tokenType kind = cur_active_funtab->getKind();
	if (this->curToken == SEMICOLONSY) {
		// 无返回值，查找符号表定义！！
		if (kind == VOIDTK) {
			// 这里就不管了，反正你也不返回啥
			mid = push_mid_code(J_M, &(cur_active_funtab->getName() + "_end"), 0, 0);
			this->print_mid_code(mid);
		}
		else {
			// warning: 此路径没有返回值 ！
			mid = push_mid_code(J_M, &(cur_active_funtab->getName() + "_end"), 0, 0);
			this->print_mid_code(mid);
		}
		return;
		// 四元式
	}
	else {
		if (kind == VOIDTK) {
			errorManipulate(NOMEANING, "在无返回值的函数中的return语句存在返回值！");
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
			this->errorManipulate(NOMEANING, "函数的实际返回值类型与定义不符！");
		mid = push_mid_code(RETV_M, tmp_name, 0, 0);
		this->print_mid_code(mid);
		mid = push_mid_code(J_M, &(cur_active_funtab->getName() + "_end"), 0, 0);
		this->print_mid_code(mid);
		// 操作 注意，删除时一定要慎重，只有判断没有常量时才可以删除
		// 有返回值分支 + 1
		cur_active_funtab->setValue(cur_active_funtab->getValue() + 1);
		delete(tmp_name);
		// 错了也不跳
		this->checkType(RSBRASY, NONE);
	}
}

void Compiler::paralist(string name, tokenType type) {
	string paraname;
	tokenType paratype;
	// 检查登录符号表后函数符号表指针是否正确
	if (cur_active_funtab != fun_tab[name]) {
		errorManipulate(NOMEANING, "当前活跃函数指针与登录进符号表的指针不匹配！");
	}
	// 不断读，看第一个读到的是否为int标识符
	do {
		this->getSym();
		paratype = this->curToken;
		switch (paratype) {
			// int参数或者char参数处理方式相同
		case INTTK:
		case CHARTK:
			this->getSym();
			paraname = this->strToken;
			if (this->checkType(IDEN, NONE)) {
				// 登录函数参数符号表！！！
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
		// 默认为int类型，继续进行
		this->errorManipulate(NOINTORCHAR);
		*retType = INTTK;
		break;
	}
	// 获取标识符
	this->getSym();
	if (this->curToken == IDEN) {
		*name = this->strToken;
	}
	else if (this->searchReserve(this->strToken) != IDEN && this->searchReserve(this->strToken) != NONE) {
		this->errorManipulate(NOMEANING, "标识符与保留字冲突 !");
		string* tmp = new string("#temp#");
		*name = *tmp;
	}
	else {
		this->errorManipulate(NOMEANING, "没有匹配标识符 !");
		string* tmp = new string("#temp#");
		*name = *tmp;
	}
	this->getSym();
}
tokenType Compiler::expr(bool* is_const, int* value, string* tmp_name) {
	Midcode* mid;
	tokenType pre_op = PLUSSY;
	// 前一个项的参数
	tokenType first_cal_type, next_cal_type;
	bool first_is_const, next_is_const;
	int first_value, next_value;
	string* first_tmp_name = new string();
	string* next_tmp_name = new string();
	// [+/-] 这里由于有文法的二义性，所以一旦出现单个+/-则归到最外面的表达式中
	if (this->curToken == SUBSY) {
		pre_op = SUBSY;
		this->getSym();
	}
	else if (this->curToken == PLUSSY) {
		pre_op = PLUSSY;
		this->getSym();
	}
	// 第一项，不用外面帮着getSym
	// 类型是int结果，需要结合符号表
	GRM_OUT << "In Line " << lineNum +1 << " :  " << "This is an expression !" << endl;
	if ((first_cal_type = this->item(&first_is_const, &first_value, first_tmp_name)) != VOIDTK) {
		// 有第二项
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
				//errorManipulate(NOMEANING,"表达式中出现不可求值项 ！");
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
				// 加偏移，生成四元式 if(first_is_const)...
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
				// 均是已分配的变量，加偏移，生成四元式
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
		//这个需要语义分析啊！
		//errorManipulate(NOMEANING, "表达式中出现不可求值项 ！");
		this->skip(SEMICOLONSY);
		return first_cal_type;
	}
}

			// 合并第一项和后面的东西！！
			/**is_const = first_is_const & next_is_const;
			*value = (first_is_const) ? ((this->curToken == PLUSSY) ? (first_value+next_value) : (first_value - next_value)) : (0);
			if (*is_const) {
				first_is_const = *is_const;	first_value = *value;	first_tmp_name = nullptr;
			}
			else if (first_is_const || next_is_const) {
				this->gen_tmp_name(tmp_name);
				this->put_var(*tmp_name, INTTK, 0, 0);
				// 加偏移，生成四元式 if(first_is_const)...
				first_is_const = *is_const;	first_value = *value;	*first_tmp_name = *tmp_name;
				first_cal_type = INTTK;
			}
			else {
				// 均是已分配的变量，加偏移，生成四元式
			}
		}
		*is_const = first_is_const;	*value = first_value;	*tmp_name = *first_tmp_name;
		return first_cal_type;
	}
	// 返回单个字符
	else if (first_cal_type == CHARTK) {

	}
	else {
		//这个需要语义分析啊！
		errorManipulate(NOMEANING, "表达式中出现不可求值项 ！");
		this->getSym();
		return first_cal_type;
	}
}*/

tokenType Compiler::item(bool* is_const, int* value, string* tmp_name) {
	Midcode* mid;
	// 记录第一个因子的东西
	tokenType first_cal_type, next_cal_type;
	bool first_is_const, next_is_const;
	int first_value, next_value;
	string* first_tmp_name = new string();
	string* next_tmp_name = new string();
	// 第一项
	// 类型是int类型的结果则将前后两者合并，否则报错，语义分析进行不下去了
	//this->getSym();
	if ((first_cal_type = this->factor(&first_is_const, &first_value, first_tmp_name)) != VOIDTK) {
		// 首先要把返回值拿出来
		if (*first_tmp_name == *Ret) {
			string* ret_store = new string();
			this->gen_tmp_name(ret_store);
			mid = this->push_mid_code(ASS_M, first_tmp_name, 0, ret_store);
			this->print_mid_code(mid);
			*first_tmp_name = *ret_store;
		}
		// 有第二项
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
			// 合并第一项和后面的东西！！
			*is_const = first_is_const && next_is_const;
			if (*is_const) {
				*value = (op_type == MULTSY) ? (first_value*next_value) : ((next_value!=0)?((int)(first_value / next_value)):(first_value));
				if (op_type == DIVSY && next_value == 0)
					this->errorManipulate(NOMEANING, "除以0异常 ！");
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
				// 加偏移，生成四元式 if(first_is_const)...
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
				// 均是已分配的变量，加偏移，生成四元式
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
		//这个需要语义分析啊！
		errorManipulate(NOMEANING, "表达式中出现不可求值项 ！");
		this->skip(SEMICOLONSY);
		return first_cal_type;
	}
}
// 前面被人先读，后面给别人先读
tokenType Compiler::factor(bool* is_const, int* value, string* tmp_name) {
	// 遇到标识符就要查符号表
	tokenType  factor_type;
	Midcode* mid;
	// '('
	if (this->curToken == LSBRASY) {
		this->getSym();
		// 判断传回值类型
		factor_type = expr(is_const, value, tmp_name);
		// 处理
		// 匹配右小括号并返回当前表达式类型值
		this->checkType(RSBRASY, NONE);
		// 经过了一次运算，所有变量类型合法则强制返回int
		if (factor_type == INTTK || factor_type == CHARTK) {
			factor_type = INTTK;
		}
		return factor_type;
	}
	// 整数
	else if (this->curToken == NUMUSR || this->curToken == PLUSSY || this->curToken == SUBSY) {
		// 是常值，设置传来的对应域
		*is_const = true;
		//this->gen_tmp_name(tmp_name);
		this->integer(value);	this->int2string(tmp_name, *value);
		return INTTK;
	}
	// 字符
	else if (this->curToken == CHARUSR) {
		*is_const = true;
		//this->gen_tmp_name(tmp_name); 就当成整数呗
		*value = this->strToken[0];	this->int2string(tmp_name, *value);
		this->getSym();
		return CHARTK;//?
	}
	// 标识符
	else if (this->curToken == IDEN) {
		// 这里应该查符号表来确定当前标识符属于什么。语法就先分析语法项了
		string name = this->strToken;
		this->getSym();
		// 值参数表，需要查符号表,把类型找出来
		if (this->curToken == LSBRASY) {
			FuncItem* func_it= this->get_func_item(name);
			if (func_it != nullptr) {
				
				this->fun_call(func_it);				// 既然已经登陆符号表了，那也就不用name了
				mid = this->push_mid_code(CALL_M, &name, 0, 0);
				this->print_mid_code(mid);
				// 标识此处的值需要从返回值处取出
				*tmp_name = *Ret;	*is_const = false;	*value = 0;
				return (func_it->getKind());				// 符号表！！实际要看符号表中的返回值
			}
			return VOIDTK;
		}
		// 数组变量，需要查符号表，越界，类型
		else if (this->curToken == LMBRASY) {
			Item* it = this->get_const_var_item(name);
			if (it == nullptr) {
				errorManipulate(NOMEANING, "数组变量未定义！");
				return VOIDTK;
			}
			this->getSym();
			bool idx_is_const;
			int idx_value;
			string* idx_tmp_name = new string();
			tokenType idx_cal_type;
			// 看数组下标是否可以解析为常值
			idx_cal_type = this->expr(&idx_is_const, &idx_value, idx_tmp_name);
			if (idx_is_const && (idx_value >= it->getLength() || idx_value < 0)) {
				errorManipulate(NOMEANING, "引用的数组超过了不在合法数组下标范围内！");
				if (idx_value < 0)
					idx_value = -idx_value;
			}
			string* arr_name = new string();
			this->gen_tmp_name(arr_name);
			// 登四元式 arr_namr = name[idx_tmp_name]
			mid = this->push_mid_code(ARTO_M, &name, idx_tmp_name, arr_name);
			this->print_mid_code(mid);
			// 向上返回不可求值
			*is_const = false;	*value = 0;	*tmp_name = *arr_name;
			this->checkType(RMBRASY, NONE);
			return it->getKind();
		}
		// 变量或常量或无参有返回值函数
		else {
			Item* it;
			// 是变量或常量
			if ((it=this->get_const_var_item(name))!=nullptr) {
				if (it->getGenre() == VAR || it->getGenre() == PARA) {
					// 登录四元式？
					// 判断是否是全局变量！
					string* glo_name = new string();
					if (it->global) {
						this->gen_tmp_name(glo_name);
						mid = this->push_mid_code(ASS_M, &name, 0, glo_name);
						this->print_mid_code(mid);
					}
					*is_const = false;	*value = 0;	*tmp_name = (it->global) ? (*glo_name) : (name);
					return it->getKind();
				}
				// 常量
				else {
					*is_const = true;	*value = it->getValue();	this->int2string(tmp_name, *value);
					return it->getKind();
				}
			}
			// 无参函数
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
		errorManipulate(NOMEANING, "表达式中出现不符合语法分析项 ！");
		return VOIDTK;
	}
	
}