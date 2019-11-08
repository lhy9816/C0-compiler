#include "Compiler.h"

#define DEBUG (0)
#if DEBUG
#define MIPS_OUT cout
#else
#define MIPS_OUT mips_fout
#endif //DEBUG

/*******************************************************************************\
 *																			   *
 *								目标代码实现函数								   *
 *																			   *
\*******************************************************************************/
#if !OPT
//mips保留字
string *SW = new string("sw");

string *LW = new string("lw");

string *LI = new string("li");

string *ADD = new string("add");

string *ADDI = new string("addi");

string *SUBI = new string("subi");

string *SUB = new string("sub");

string *BEQ = new string("beq");

string *BNE = new string("bne");

string *BGE = new string("bge");

string *BGT = new string("bgt");

string *BLE = new string("ble");

string *BLT = new string("blt");

string *J = new string("j");

string *JAL = new string("jal");

string *JR = new string("jr");

string *MUL = new string("mul");

string *DIV = new string("div");

string *MFLO = new string("mflo");

string *SYSCALL = new string("syscall");

string *LA = new string("la");

// 没有空指令？
string *NOP = new string("nop");

string *SLL = new string("sll");

string *MOVE = new string("move");

string *ADDU = new string("addu");

// 寄存器编号
string *SP = new string("$sp");

string *SP_BRC = new string("($sp)");

string *FP = new string("$fp");

string *FP_BRC = new string("($fp)");

string *RA = new string("$ra");

string *GP = new string("$gp");

string *GP_BRC = new string("($gp)");

//t9作为需要用的临时变量不参与分配

string *T9 = new string("$t9");

//t8作为需要用的临时变量不参与分配

string *T8 = new string("$t8");


string *V0 = new string("$v0");

// 使用暴力法，只给3个寄存器，一旦完成操作就要写回内存！

string *A0 = new string("$a0");

string *ZERO = new string("$zero");

string *NULSTR = new string();

// 目标代码输出文件路径
ofstream mips_fout("output/mips_out.asm");

// 寄存器组
RegRecord* regs = new RegRecord[MaxAvaiReg];
// 产生mips指令
void Compiler::gen_mips_code(string* op, string* dst, string* src1, string* src2) {
	string* comma1 = new string("\t");	string* comma2 = new string("\t");
	if (*src1 != "")
		*comma1 += ",";
	if (*src2 != "" && *op != *LW && *op != *SW)
		*comma2 += ",";
	else if (*op != *LW || *op != *SW)
		*comma2 = "";
	MIPS_OUT << '\t' << *op << '\t' << *dst << *comma1 << *src1 << *comma2 << *src2 << endl;
	delete(comma1);
	delete(comma2);
}


// 初始化分配字符串空间
void Compiler::asciiz_setup() {
	string* target_str = new string();
	//vector<string>::iterator iter;
	MIPS_OUT << ".data" << endl;
	size_t i;
	for (i = 0; i < string_tab.size(); i++) {
		MIPS_OUT << '\t' << "$String" << i << ":.asciiz " << "\"" << string_tab[i] << "\\n\"" << endl;
	}
	string_tab.push_back("");
	MIPS_OUT << '\t' << "$String" << i << ":.asciiz " << "\"" << string_tab[i] << "\\n\"" << endl;
}

void Compiler::simu_reg_alloc(vector<int>* reg)
{
	const int MAX = 16;
	reg->clear();
	srand((unsigned)time(NULL));
	int num = rand() % (MAX - 3) + 1; int n = 0;
	while (true) {
		int tmp = rand() % MAX + 1;
		vector<int>::iterator it;
		if ((it = find(reg->begin(), reg->end(), tmp)) == reg->end()) {
			reg->push_back(tmp);
			++n;
		}
		if (n == num)
			break;
	}
}

void Compiler::init_reg(RegRecord* reg_list) {
	vector<int> occupied;
	this->simu_reg_alloc(&occupied);
	for (int i = 0;i < MaxAvaiReg;++i) {
		reg_list[i].available = true;
		reg_list[i].global = true;
		if (find(occupied.begin(), occupied.end(), i) == occupied.end())
			reg_list[i].occupied = false;
		else
			reg_list[i].occupied = true;

		reg_list[i].offset = 0;
		reg_list[i].idx = -1;		// 不是可索引变量
		reg_list[i].name = "";
	}
}

void Compiler::get_reg(string* reg_name, RegType* reg_kind, string* name, bool* hit) {
	// 还可以判断前4个取a0-a3
	// 先看是否是const
	if (this->reg_is_const(name)) {
		*reg_kind = CONST_RG;
		*reg_name = *name;
		return;
	}
	// 是不是返回值
	if (*name == "%RET") {
		*reg_kind = RET_RG;
		*reg_name = *V0;
		return;
	}
	// 针对参数查找找到是否hit足矣，不需要换寄存器,LHY与下面换了位置
	if (*reg_name == *A0)
		return;
	// 先在寄存器中查找
	/*for (int i = 0; i < MaxAvaiReg;++i) {
		if (regs[i].name == *name) {
			*reg_name = this->get_ts_reg(i, reg_kind);
			*hit = true;
			return;
		}
	}

	Item* it = this->get_const_var_item(*name);
	// 有空的就分一个空的
	for (int i = 0; i < MaxAvaiReg;++i) {
		if (regs[i].available == true && regs[i].occupied == false) {
			regs[i].name = *name;
			regs[i].offset = it->offset;
			regs[i].available = false;
			regs[i].global = it->global;
			*reg_name = this->get_ts_reg(i, reg_kind);
			return;
		}
	}*/
	// 然后找内存
	Item* it = this->get_const_var_item(*name);
	string* offset = new string();
	static int i_r = 0;
	while (true) {
		int j = i_r % MaxAvaiReg;
		//if (regs[j].occupied == false) {
			// 先save
			/*Item* it_old = this->get_const_var_item(regs[j].name);
			*reg_name = this->get_ts_reg(j, reg_kind);
			if (regs[j].global) {
				this->int2string(offset, 4*it_old->offset);
				this->gen_mips_code(SW, reg_name, offset, GP_BRC);
			}
			else {
				this->int2string(offset, -4 * it_old->offset);
				this->gen_mips_code(SW, reg_name, offset, FP_BRC);
			}
			// 当前reg清零 ??
			this->gen_mips_code(LI, reg_name, new string("0"), NULSTR);*/
			// 再分配
		*reg_name = this->get_ts_reg(j, reg_kind);
		regs[j].name = *name;
		regs[j].offset = it->offset;
		regs[j].available = false;
		regs[j].global = it->global;
		delete(offset);
		++i_r;
		return;
		//}
		++i_r;
	}
}

void Compiler::get_reg(string* reg_name, RegType* reg_kind, string* name, string* idx) {
	// bug：不一定常值
	// 是不是返回值
	if (*name == "%RET") {
		*reg_kind = RET_RG;
		*reg_name = *V0;
		return;
	}
	// 先看是否是const
	if (this->reg_is_const(idx)) {
		int arr_idx = atoi(idx->c_str());
		for (int i = 0; i < MaxAvaiReg;++i) {
			if (regs[i].name == *name && regs[i].idx == arr_idx) {
				*reg_name = this->get_ts_reg(i, reg_kind);
				return;
			}
		}
		*reg_kind = NFIND;
		return;
	}
	else {
		*reg_kind = NFIND;
		return;
	}

	int arr_idx = atoi(idx->c_str());
	for (int i = 0; i < MaxAvaiReg;++i) {
		if (regs[i].name == *name && regs[i].idx == arr_idx) {
			*reg_name = this->get_ts_reg(i, reg_kind);
			return;
		}
	}
	Item* it = this->get_const_var_item(*name);
	// 有空的就分一个空的
	for (int i = 0; i < MaxAvaiReg;++i) {
		if (regs[i].available == true && regs[i].occupied == false) {
			regs[i].name = *name;
			regs[i].offset = it->offset;
			regs[i].idx = arr_idx;
			regs[i].available = false;
			regs[i].global = it->global;
			*reg_name = this->get_ts_reg(i, reg_kind);
			return;
		}
	}
	// 然后找内存
	string* offset = new string();
	for (int i = 0;i < MaxAvaiReg;i++) {
		if (regs[i].occupied == false) {
			// 先save
			Item* it_old = this->get_const_var_item(regs[i].name);
			*reg_name = this->get_ts_reg(i, reg_kind);
			if (regs[i].global) {
				this->int2string(offset, 4 * it_old->offset);
				this->gen_mips_code(SW, reg_name, offset, GP_BRC);
			}
			else {
				this->int2string(offset, -4 * it_old->offset);
				this->gen_mips_code(SW, reg_name, offset, FP_BRC);
			}
			// 再分配
			regs[i].name = *name;
			regs[i].offset = it->offset;
			regs[i].idx = arr_idx;
			regs[i].available = false;
			regs[i].global = it->global;
			delete(offset);
			return;
		}
	}
}

void Compiler::put_in_reg(string* reg_name, string* name) {
	Item* it = this->get_const_var_item(*name);
	string* offset = new string();
	this->int2string(offset, 4 * it->offset);
	if (it->global) {
		this->gen_mips_code(LW, reg_name, offset, GP_BRC);
	}
	else {
		string tmp = (it->offset > 0) ? ("-" + *offset) : (*offset);
		this->gen_mips_code(LW, reg_name, &tmp, FP_BRC);
	}
}

void Compiler::put_in_memory(string* reg_name, string* name) {
	Item* it = this->get_const_var_item(*name);
	string* offset = new string();
	this->int2string(offset, 4 * it->offset);
	if (it->global) {
		this->gen_mips_code(SW, reg_name, offset, GP_BRC);
	}
	else {
		string tmp = (it->offset > 0) ? ("-" + *offset) : (*offset);
		this->gen_mips_code(SW, reg_name, &tmp, FP_BRC);
	}
	// 清除对应的寄存器占用标识 优化前！
	for (int i = 0; i < MaxAvaiReg;++i) {
		if (regs[i].name == *name) {
			regs[i].available = true;
			regs[i].global = true;
			regs[i].offset = 0;
			regs[i].idx = -1;		// 不是可索引变量
			regs[i].name = "";
			return;
		}
	}
}

string Compiler::get_ts_reg(int i, RegType* reg_kind) {
	string* reg_name = new string();
	if (i <= 8) {
		this->int2string(reg_name, i);
		*reg_name = "$t" + *reg_name;
		*reg_kind = T_RG;
	}
	else {
		this->int2string(reg_name, i - 8);
		*reg_name = "$s" + *reg_name;
		*reg_kind = S_RG;
	}
	return *reg_name;
}

void Compiler::handle_label(string* label) {
	MIPS_OUT << *label << ":" << endl;
}

void Compiler::handle_init(Midcode* mid) { //? offset用不用最后给他传回去
	// 打出函数名
	this->handle_label(mid->src1);
	// 先把参数位置加回去
	string* para_num = new string();
	string* fun_offset_str = new string();
	string* reg_num_str = new string();
	// 这个para_offset是给push用来计数参数的，每次push完之后要置零为下一次push做准备
	this->para_offset_stack.push(this->para_offset);
	this->para_offset = -1;
	string* fun_name = new string();	*fun_name = *(mid->src1);
	FuncItem* item_f = this->get_func_item(*fun_name);
	this->fun_stack.push(this->cur_active_funtab);
	this->cur_active_funtab = item_f;
	int fun_offset = item_f->offset;
	int paranum = item_f->paranum;	this->int2string(para_num, paranum * 4);
	this->gen_mips_code(ADDI, SP, SP, para_num);
	// 预留局部变量与临时变量，存寄存器
	for (int i = 0;i < MaxAvaiReg;i++) {
		string prefix = "";
		if (i <= 7) {
			this->int2string(reg_num_str, i);
			prefix = "$t" + *reg_num_str;
		}
		else {
			this->int2string(reg_num_str, i - 8);
			prefix = "$s" + *reg_num_str;
		}
		this->int2string(fun_offset_str, -4 * (fun_offset + i));
		this->gen_mips_code(SW, &prefix, fun_offset_str, SP_BRC);
	}
	// 存栈帧和返回地址
	fun_offset += MaxAvaiReg;	this->int2string(fun_offset_str, -4 * fun_offset);
	this->gen_mips_code(SW, FP, fun_offset_str, SP_BRC);
	fun_offset++;				this->int2string(fun_offset_str, -4 * fun_offset);
	this->gen_mips_code(SW, RA, fun_offset_str, SP_BRC);
	// 更新栈帧为当前函数栈帧，更新栈顶sp
	this->gen_mips_code(MOVE, FP, SP, NULSTR);
	fun_offset++;				this->int2string(fun_offset_str, -4 * fun_offset);
	this->gen_mips_code(ADDI, SP, SP, fun_offset_str);
	this->init_reg(regs);

	delete(fun_name);
	delete(para_num);
	delete(fun_offset_str);
	delete(reg_num_str);
}
// 先把参数都存在内存中
void Compiler::handle_push(Midcode* mid) {
	string* reg_name = new string();
	string* para_idx = new string();
	RegType rg;  bool hit = false;
	// 函数参数+1
	this->para_offset++;
	this->get_reg(reg_name, &rg, mid->src1, &hit);
	Item* it = this->get_const_var_item(*(mid->src1));
	if (rg == CONST_RG) {
		this->gen_mips_code(LI, T9, reg_name, NULSTR);
		*reg_name = *T9;
	}
	else {
		if (hit == false || hit == true)
			this->put_in_reg(reg_name, mid->src1);
	}
	if (this->para_offset < 4 && this->para_offset >= 0) {
		this->int2string(para_idx, this->para_offset);
		this->gen_mips_code(MOVE, &("$a" + *para_idx), reg_name, NULSTR);
	}

	this->gen_mips_code(SW, reg_name, new string("0"), SP_BRC);
	this->gen_mips_code(ADDI, SP, SP, new string("-4"));
	delete(reg_name);
}
void Compiler::handle_call(Midcode* mid) {
	this->gen_mips_code(JAL, mid->src1, NULSTR, NULSTR);
	// 每call一次切断前后两个函数的寄存器需求
	for (int i = 0; i < MaxAvaiReg;++i) {
		regs[i].available = true;
		regs[i].global = true;
		regs[i].offset = 0;
		regs[i].idx = -1;		// 不是可索引变量
		regs[i].name = "";
		return;
	}
}

void Compiler::handle_branch(Midcode* mid) {
	bool flag = false;
	string* src1_reg = new string();
	string* src2_reg = new string();
	RegType src1_type, src2_type;
	bool src1_hit = false, src2_hit = false;
	this->get_reg(src1_reg, &src1_type, mid->src1, &src1_hit);
	this->get_reg(src2_reg, &src2_type, mid->src2, &src2_hit);
	if (src1_type == CONST_RG && src2_type == CONST_RG) {
		int distance = atoi(src1_reg->c_str()) - atoi(src2_reg->c_str());
		this->int2string(src1_reg, distance);
		this->gen_mips_code(LI, T9, src1_reg, NULSTR);
		*src1_reg = *T9;
		*src2_reg = *ZERO;
		flag = true;
	}
	else if (src1_type == CONST_RG && src2_type != CONST_RG) {
		this->gen_mips_code(LI, T9, src1_reg, NULSTR);
		if (src2_hit == false || src2_hit == true)
			this->put_in_reg(src2_reg, mid->src2);
		*src1_reg = *T9;
	}
	else if (src1_type != CONST_RG && src2_type == CONST_RG) {
		this->gen_mips_code(LI, T9, src2_reg, NULSTR);
		if (src1_hit == false || src1_hit == true)
			this->put_in_reg(src1_reg, mid->src1);
		*src2_reg = *T9;
	}
	else {
		if (src1_hit == false || src1_hit == true)
			this->put_in_reg(src1_reg, mid->src1);
		if (src2_hit == false || src2_hit == true)
			this->put_in_reg(src2_reg, mid->src2);
	}
	switch (mid->mid_op) {
	case BGJ_M:
		this->gen_mips_code(BGT, src1_reg, src2_reg, mid->dst);
		break;
	case BGEJ_M:
		this->gen_mips_code(BGE, src1_reg, src2_reg, mid->dst);
		break;
	case LSJ_M:
		this->gen_mips_code(BLT, src1_reg, src2_reg, mid->dst);
		break;
	case LSEJ_M:
		this->gen_mips_code(BLE, src1_reg, src2_reg, mid->dst);
		break;
	case EQJ_M:
		this->gen_mips_code(BEQ, src1_reg, src2_reg, mid->dst);
		break;
	case NEQJ_M:
		this->gen_mips_code(BNE, src1_reg, src2_reg, mid->dst);
		break;
	}
	// 未优化前的下下策
	if (src1_type != CONST_RG) {
		this->put_in_memory(src1_reg, mid->src1);
	}
	if (src2_type != CONST_RG) {
		this->put_in_memory(src2_reg, mid->src2);
	}
	delete(src1_reg);
	delete(src2_reg);
}
void Compiler::handle_jmp(Midcode* mid) {
	this->gen_mips_code(J, mid->src1, NULSTR, NULSTR);
}
void Compiler::handle_scanf(Midcode* mid) {
	string* name = new string();
	*name = *(mid->src1);
	// 在符号表中寻找该标识符的类型
	Item* it = this->get_const_var_item(*name);
	if (it->getKind() == CHARTK) {
		this->gen_mips_code(LI, V0, new string("12"), NULSTR);
		this->gen_mips_code(SYSCALL, NULSTR, NULSTR, NULSTR);
	}
	else if (it->getKind() == INTTK) {
		this->gen_mips_code(LI, V0, new string("5"), NULSTR);
		this->gen_mips_code(SYSCALL, NULSTR, NULSTR, NULSTR);
	}
	// 写回寄存器或内存，先按照写回寄存器处理，因为我们的变量应该还够用，个人感觉，，，LHY
	string* reg_name = new string();
	RegType rg;
	bool hit = false;
	//this->get_reg(reg_name, &rg, name, &hit);
	//this->gen_mips_code(MOVE, reg_name, V0, NULSTR);
	this->put_in_memory(V0, name);
	delete(reg_name);
	delete(name);
}
void Compiler::handle_printf(Midcode* mid) {
	//string* reg_name = new string();
	if (mid->mid_op == PRINTV_M) {
		RegType rg;		bool hit = false;
		string* reg_name = new string("A0");
		this->get_reg(reg_name, &rg, mid->src2, &hit);
		if (rg == CONST_RG) {
			this->gen_mips_code(LI, A0, reg_name, NULSTR);
			*reg_name = *A0;
		}
		/*else if (hit == false || hit == true) {			// 有hit这句话留下来
			this->put_in_reg(A0, mid->src2);
		}*/
		else {
			this->put_in_reg(reg_name, mid->src2);				// 有hit这句话删掉
			this->gen_mips_code(MOVE, A0, reg_name, NULSTR);
		}
		if (*(mid->src1) == "char") {
			this->gen_mips_code(LI, V0, new string("11"), NULSTR);
		}
		else {
			this->gen_mips_code(LI, V0, new string("1"), NULSTR);
		}
	}
	else {
		string tmp = (*mid->src1).erase(0, 5);
		this->gen_mips_code(LA, A0, &("$String" + tmp), NULSTR);
		this->gen_mips_code(LI, V0, new string("4"), NULSTR);
	}
	this->gen_mips_code(SYSCALL, NULSTR, NULSTR, NULSTR);
	if (mid->mid_op == PRINTV_M) {
		string* tmp = new string();
		this->int2string(tmp, string_tab.size() - 1);
		this->gen_mips_code(LA, A0, &("$String" + *tmp), NULSTR);
		this->gen_mips_code(LI, V0, new string("4"), NULSTR);
		this->gen_mips_code(SYSCALL, NULSTR, NULSTR, NULSTR);
	}
}
void Compiler::handle_return(Midcode* mid) {
	if (mid->mid_op == RETV_M) {
		string* reg_name = new string();
		RegType reg_type;	bool hit = false;
		this->get_reg(reg_name, &reg_type, mid->src1, &hit);
		if (reg_type == CONST_RG) {
			this->gen_mips_code(LI, V0, reg_name, NULSTR);
		}
		else if (reg_type == RET_RG) {
			// v0处已经是返回值了
		}
		else {
			if (hit == false || hit == true)
				this->put_in_reg(reg_name, mid->src1);
			this->gen_mips_code(MOVE, V0, reg_name, NULSTR);
		}
	}
	// RET_M
	else {
		// 恢复返回地址，栈帧
		string* reg_num_str = new string();
		string* offset_str = new string();
		FuncItem* item_f = this->get_func_item(*(mid->src1));
		this->gen_mips_code(LW, RA, new string("4"), SP_BRC);
		this->gen_mips_code(LW, FP, new string("8"), SP_BRC);
		for (int i = MaxAvaiReg - 1; i >= 0;--i) {
			string prefix = "";
			if (i <= 7) {
				this->int2string(reg_num_str, i);
				prefix = "$t" + *reg_num_str;
			}
			else {
				this->int2string(reg_num_str, i - 8);
				prefix = "$s" + *reg_num_str;
			}
			this->int2string(offset_str, 4 * (2 + MaxAvaiReg - i));
			this->gen_mips_code(LW, &prefix, offset_str, SP_BRC);
		}
		int fun_offset = 4 * (2 + MaxAvaiReg + item_f->offset);	this->int2string(offset_str, fun_offset);
		this->gen_mips_code(ADDI, SP, SP, offset_str);
		// 将当前函数指针与参数偏移指针指回原来所指项
		this->cur_active_funtab = fun_stack.top();
		this->fun_stack.pop();
		this->para_offset = para_offset_stack.top();
		this->para_offset_stack.pop();
		this->gen_mips_code(JR, RA, NULSTR, NULSTR);
		delete(reg_num_str);
		delete(offset_str);
	}
}
void Compiler::handle_operation(Midcode* mid) {
	string* OP = new string();
	switch (mid->mid_op) {
	case ADD_M:	*OP = *ADD; break;
	case SUB_M: *OP = *SUB; break;
	case MUL_M: *OP = *MUL; break;
	case DIV_M: *OP = *DIV; break;
	}
	string* dst_reg = new string();		RegType dst_tp;		bool dst_hit = false;
	string* src1_reg = new string();	RegType src1_tp;	bool src1_hit = false;
	string* src2_reg = new string();	RegType src2_tp;	bool src2_hit = false;
	this->get_reg(dst_reg, &dst_tp, mid->dst, &dst_hit);
	this->get_reg(src1_reg, &src1_tp, mid->src1, &src1_hit);
	this->get_reg(src2_reg, &src2_tp, mid->src2, &src2_hit);
	if (src1_tp == CONST_RG && src2_tp != CONST_RG) {
		// 对于第一个操作数为常数操作数，加和乘可以直接换位置，但是减和除不可以
		if (src2_hit == false || src2_hit == true)
			this->put_in_reg(src2_reg, mid->src2);
		if (*OP == *SUB || *OP == *DIV) {
			this->gen_mips_code(LI, T9, src1_reg, NULSTR);
			*src1_reg = *T9;
			this->gen_mips_code(OP, dst_reg, src1_reg, src2_reg);
		}
		else {
			this->gen_mips_code(OP, dst_reg, src2_reg, src1_reg);
		}
	}
	else if (src1_tp != CONST_RG && src2_tp == CONST_RG) {
		if (src1_hit == false || src1_hit == true)
			this->put_in_reg(src1_reg, mid->src1);
		this->gen_mips_code(OP, dst_reg, src1_reg, src2_reg);
	}
	else {
		if ((src2_hit == false || src2_hit == true) && src2_tp != RET_RG)
			this->put_in_reg(src2_reg, mid->src2);
		if ((src1_hit == false || src1_hit == true) && src1_tp != RET_RG)
			this->put_in_reg(src1_reg, mid->src1);
		this->gen_mips_code(OP, dst_reg, src1_reg, src2_reg);
	}
	this->put_in_memory(dst_reg, mid->dst);
	delete(dst_reg);
	delete(src1_reg);
	delete(src2_reg);
}

void Compiler::handle_assign(Midcode* mid) {
	string* src_reg = new string();
	string* dst_reg = new string();
	RegType src_tp, dst_tp;
	bool src_hit = false, dst_hit = false;
	this->get_reg(src_reg, &src_tp, mid->src1, &src_hit);
	this->get_reg(dst_reg, &dst_tp, mid->dst, &dst_hit);
	if (src_tp == CONST_RG) {
		this->gen_mips_code(LI, dst_reg, src_reg, NULSTR);
	}
	else if (src_tp == RET_RG) {
		this->gen_mips_code(MOVE, dst_reg, src_reg, NULSTR);
	}
	else {
		if (src_hit == false || src_hit == true)
			this->put_in_reg(src_reg, mid->src1);
		this->gen_mips_code(MOVE, dst_reg, src_reg, NULSTR);
	}
	this->put_in_memory(dst_reg, mid->dst);
	delete(src_reg);
	delete(dst_reg);
}
void Compiler::handle_arrto(Midcode* mid) {
	// 先看寄存器中有没有
	RegType rg;
	string* arr_reg = new string();
	string* idx = new string();
	string* tar_reg = new string();
	string* offset = new string();
	bool hit = false;
	this->get_reg(arr_reg, &rg, mid->src1, mid->src2);
	if (rg != NFIND) {
		this->get_reg(tar_reg, &rg, mid->dst, &hit);
		this->gen_mips_code(MOVE, tar_reg, arr_reg, NULSTR);
	}
	// 寄存器中没有
	else {
		this->get_reg(idx, &rg, mid->src2, &hit);
		if (rg == CONST_RG) {
			this->gen_mips_code(LI, T9, idx, NULSTR);
			*idx = *T9;
		}
		else {
			this->put_in_reg(idx, mid->src2);
		}
		Item* it = this->get_const_var_item(*(mid->src1));
		this->int2string(offset, it->offset);
		this->gen_mips_code(ADDI, idx, idx, offset);
		this->gen_mips_code(SLL, idx, idx, new string("2"));
		if (it->global)
			this->gen_mips_code(ADD, idx, idx, GP);
		else
			this->gen_mips_code(SUB, idx, FP, idx);
		hit = false;
		this->get_reg(tar_reg, &rg, mid->dst, &hit);
		this->gen_mips_code(LW, tar_reg, new string("0"), &("(" + *idx + ")"));
		this->put_in_memory(tar_reg, mid->dst);
	}

	delete(offset);
	delete(arr_reg);
	delete(tar_reg);
	delete(idx);
}
void Compiler::handle_arrget(Midcode* mid) {
	string* idx = new string();
	string* from = new string();
	string* offset = new string();
	string* from_num = new string();
	RegType rg;
	bool hit = false;
	// 变量在函数中的偏移
	this->get_reg(idx, &rg, mid->src2, &hit);
	Item* it = this->get_const_var_item(*(mid->src1));

	if (rg == CONST_RG) {
		int index = atoi(idx->c_str());
		index = 4 * (index + it->offset);
		*idx = *T9;		this->int2string(offset, index);
		this->gen_mips_code(LI, idx, offset, NULSTR);
	}
	else {
		if (hit == false || hit == true)
			this->put_in_reg(idx, mid->src2);
		this->int2string(offset, it->offset);
		this->gen_mips_code(ADDI, T9, idx, offset);
		this->gen_mips_code(SLL, T9, T9, new string("2"));

		*idx = *T9;
	}
	if (it->global)
		this->gen_mips_code(ADD, T9, GP, T9);
	else
		this->gen_mips_code(SUB, T9, FP, T9);
	// dst装入内存
	hit = false;
	this->get_reg(from, &rg, mid->dst, &hit);
	if (rg == CONST_RG) {
		int num = atoi(from->c_str());
		this->int2string(from, num);
		this->gen_mips_code(LI, T8, from, NULSTR);
		*from = *T8;
	}
	else if (hit == false || hit == true) {
		this->put_in_reg(from, mid->dst);
	}
	this->gen_mips_code(SW, from, new string("0"), &("(" + *idx + ")"));

	delete(idx);
	delete(from);
	delete(offset);
	delete(from_num);
}

void Compiler::handle_exit() {
	this->gen_mips_code(LI, V0, new string("10"), NULSTR);
	this->gen_mips_code(SYSCALL, NULSTR, NULSTR, NULSTR);
}

bool Compiler::reg_is_const(string * name)
{
	return ((name[0][0] >= '0' && name[0][0] <= '9') || (name[0][0] == '-'));
}


void Compiler::generateMips() {
	// 初始化字符串表
	this->asciiz_setup();
	// 开始代码段
	string* op = new string();
	MIPS_OUT << ".text" << endl;
	this->gen_mips_code(ADDI, GP, GP, new string("0x10000"));
	this->gen_mips_code(ADD, FP, SP, ZERO);
	this->gen_mips_code(J, new string("main"), NULSTR, NULSTR);
	// 正式进入四元式的翻译阶段
	for (int i = 0;i < this->mid_list.size();++i) {
		Midcode mid = mid_list[i];
		switch (mid_list[i].mid_op) {
		case ADD_M:
		case SUB_M:
		case MUL_M:
		case DIV_M:
			this->handle_operation(&mid);
			break;

		case BGJ_M:
		case BGEJ_M:
		case LSJ_M:
		case LSEJ_M:
		case EQJ_M:
		case NEQJ_M:
			this->handle_branch(&mid);
			break;

		case ASS_M:
			this->handle_assign(&mid);
			break;
		case J_M:
			this->handle_jmp(&mid);
			break;
		case LABEL_M:
			this->handle_label(mid.src1);
			break;
		case SCANF_M:
			this->handle_scanf(&mid);
			break;
		case PRINTS_M:
		case PRINTV_M:
			this->handle_printf(&mid);
			break;
		case CALL_M:
			this->handle_call(&mid);
			break;
		case FUN_M:
			this->handle_init(&mid);
			break;
		case PUSH_M:
			this->handle_push(&mid);
			break;
		case RETV_M:
		case RET_M:
			this->handle_return(&mid);
			break;
		case EXIT_M:
			this->handle_exit();
			break;

		case ARGET_M:
			this->handle_arrget(&mid);
			break;
		case ARTO_M:
			this->handle_arrto(&mid);
			break;
		default:
			this->gen_mips_code(NOP, NULSTR, NULSTR, NULSTR);
			break;
		}
	}
}

#endif