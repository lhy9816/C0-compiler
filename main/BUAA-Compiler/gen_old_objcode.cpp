#include "Compiler.h"

#define DEBUG (0)
#if DEBUG
#define MIPS_OUT cout
#else
#define MIPS_OUT mips_fout
#endif //DEBUG

/*******************************************************************************\
 *																			   *
 *								Ŀ�����ʵ�ֺ���								   *
 *																			   *
\*******************************************************************************/
#if !OPT
//mips������
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

// û�п�ָ�
string *NOP = new string("nop");

string *SLL = new string("sll");

string *MOVE = new string("move");

string *ADDU = new string("addu");

// �Ĵ������
string *SP = new string("$sp");

string *SP_BRC = new string("($sp)");

string *FP = new string("$fp");

string *FP_BRC = new string("($fp)");

string *RA = new string("$ra");

string *GP = new string("$gp");

string *GP_BRC = new string("($gp)");

//t9��Ϊ��Ҫ�õ���ʱ�������������

string *T9 = new string("$t9");

//t8��Ϊ��Ҫ�õ���ʱ�������������

string *T8 = new string("$t8");


string *V0 = new string("$v0");

// ʹ�ñ�������ֻ��3���Ĵ�����һ����ɲ�����Ҫд���ڴ棡

string *A0 = new string("$a0");

string *ZERO = new string("$zero");

string *NULSTR = new string();

// Ŀ���������ļ�·��
ofstream mips_fout("output/mips_out.asm");

// �Ĵ�����
RegRecord* regs = new RegRecord[MaxAvaiReg];
// ����mipsָ��
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


// ��ʼ�������ַ����ռ�
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
		reg_list[i].idx = -1;		// ���ǿ���������
		reg_list[i].name = "";
	}
}

void Compiler::get_reg(string* reg_name, RegType* reg_kind, string* name, bool* hit) {
	// �������ж�ǰ4��ȡa0-a3
	// �ȿ��Ƿ���const
	if (this->reg_is_const(name)) {
		*reg_kind = CONST_RG;
		*reg_name = *name;
		return;
	}
	// �ǲ��Ƿ���ֵ
	if (*name == "%RET") {
		*reg_kind = RET_RG;
		*reg_name = *V0;
		return;
	}
	// ��Բ��������ҵ��Ƿ�hit���ӣ�����Ҫ���Ĵ���,LHY�����滻��λ��
	if (*reg_name == *A0)
		return;
	// ���ڼĴ����в���
	/*for (int i = 0; i < MaxAvaiReg;++i) {
		if (regs[i].name == *name) {
			*reg_name = this->get_ts_reg(i, reg_kind);
			*hit = true;
			return;
		}
	}

	Item* it = this->get_const_var_item(*name);
	// �пյľͷ�һ���յ�
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
	// Ȼ�����ڴ�
	Item* it = this->get_const_var_item(*name);
	string* offset = new string();
	static int i_r = 0;
	while (true) {
		int j = i_r % MaxAvaiReg;
		//if (regs[j].occupied == false) {
			// ��save
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
			// ��ǰreg���� ??
			this->gen_mips_code(LI, reg_name, new string("0"), NULSTR);*/
			// �ٷ���
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
	// bug����һ����ֵ
	// �ǲ��Ƿ���ֵ
	if (*name == "%RET") {
		*reg_kind = RET_RG;
		*reg_name = *V0;
		return;
	}
	// �ȿ��Ƿ���const
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
	// �пյľͷ�һ���յ�
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
	// Ȼ�����ڴ�
	string* offset = new string();
	for (int i = 0;i < MaxAvaiReg;i++) {
		if (regs[i].occupied == false) {
			// ��save
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
			// �ٷ���
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
	// �����Ӧ�ļĴ���ռ�ñ�ʶ �Ż�ǰ��
	for (int i = 0; i < MaxAvaiReg;++i) {
		if (regs[i].name == *name) {
			regs[i].available = true;
			regs[i].global = true;
			regs[i].offset = 0;
			regs[i].idx = -1;		// ���ǿ���������
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

void Compiler::handle_init(Midcode* mid) { //? offset�ò�������������ȥ
	// ���������
	this->handle_label(mid->src1);
	// �ȰѲ���λ�üӻ�ȥ
	string* para_num = new string();
	string* fun_offset_str = new string();
	string* reg_num_str = new string();
	// ���para_offset�Ǹ�push�������������ģ�ÿ��push��֮��Ҫ����Ϊ��һ��push��׼��
	this->para_offset_stack.push(this->para_offset);
	this->para_offset = -1;
	string* fun_name = new string();	*fun_name = *(mid->src1);
	FuncItem* item_f = this->get_func_item(*fun_name);
	this->fun_stack.push(this->cur_active_funtab);
	this->cur_active_funtab = item_f;
	int fun_offset = item_f->offset;
	int paranum = item_f->paranum;	this->int2string(para_num, paranum * 4);
	this->gen_mips_code(ADDI, SP, SP, para_num);
	// Ԥ���ֲ���������ʱ��������Ĵ���
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
	// ��ջ֡�ͷ��ص�ַ
	fun_offset += MaxAvaiReg;	this->int2string(fun_offset_str, -4 * fun_offset);
	this->gen_mips_code(SW, FP, fun_offset_str, SP_BRC);
	fun_offset++;				this->int2string(fun_offset_str, -4 * fun_offset);
	this->gen_mips_code(SW, RA, fun_offset_str, SP_BRC);
	// ����ջ֡Ϊ��ǰ����ջ֡������ջ��sp
	this->gen_mips_code(MOVE, FP, SP, NULSTR);
	fun_offset++;				this->int2string(fun_offset_str, -4 * fun_offset);
	this->gen_mips_code(ADDI, SP, SP, fun_offset_str);
	this->init_reg(regs);

	delete(fun_name);
	delete(para_num);
	delete(fun_offset_str);
	delete(reg_num_str);
}
// �ȰѲ����������ڴ���
void Compiler::handle_push(Midcode* mid) {
	string* reg_name = new string();
	string* para_idx = new string();
	RegType rg;  bool hit = false;
	// ��������+1
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
	// ÿcallһ���ж�ǰ�����������ļĴ�������
	for (int i = 0; i < MaxAvaiReg;++i) {
		regs[i].available = true;
		regs[i].global = true;
		regs[i].offset = 0;
		regs[i].idx = -1;		// ���ǿ���������
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
	// δ�Ż�ǰ�����²�
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
	// �ڷ��ű���Ѱ�Ҹñ�ʶ��������
	Item* it = this->get_const_var_item(*name);
	if (it->getKind() == CHARTK) {
		this->gen_mips_code(LI, V0, new string("12"), NULSTR);
		this->gen_mips_code(SYSCALL, NULSTR, NULSTR, NULSTR);
	}
	else if (it->getKind() == INTTK) {
		this->gen_mips_code(LI, V0, new string("5"), NULSTR);
		this->gen_mips_code(SYSCALL, NULSTR, NULSTR, NULSTR);
	}
	// д�ؼĴ������ڴ棬�Ȱ���д�ؼĴ���������Ϊ���ǵı���Ӧ�û����ã����˸о�������LHY
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
		/*else if (hit == false || hit == true) {			// ��hit��仰������
			this->put_in_reg(A0, mid->src2);
		}*/
		else {
			this->put_in_reg(reg_name, mid->src2);				// ��hit��仰ɾ��
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
			// v0���Ѿ��Ƿ���ֵ��
		}
		else {
			if (hit == false || hit == true)
				this->put_in_reg(reg_name, mid->src1);
			this->gen_mips_code(MOVE, V0, reg_name, NULSTR);
		}
	}
	// RET_M
	else {
		// �ָ����ص�ַ��ջ֡
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
		// ����ǰ����ָ�������ƫ��ָ��ָ��ԭ����ָ��
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
		// ���ڵ�һ��������Ϊ�������������Ӻͳ˿���ֱ�ӻ�λ�ã����Ǽ��ͳ�������
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
	// �ȿ��Ĵ�������û��
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
	// �Ĵ�����û��
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
	// �����ں����е�ƫ��
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
	// dstװ���ڴ�
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
	// ��ʼ���ַ�����
	this->asciiz_setup();
	// ��ʼ�����
	string* op = new string();
	MIPS_OUT << ".text" << endl;
	this->gen_mips_code(ADDI, GP, GP, new string("0x10000"));
	this->gen_mips_code(ADD, FP, SP, ZERO);
	this->gen_mips_code(J, new string("main"), NULSTR, NULSTR);
	// ��ʽ������Ԫʽ�ķ���׶�
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