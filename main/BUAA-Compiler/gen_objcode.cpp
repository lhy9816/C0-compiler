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
#if OPT
//mips������
string *SW = new string("sw");

string *LW = new string("lw");

string *LI = new string("li");

string *ADD = new string("add");

string *ADDI = new string("addi");

string *ADDIU = new string("addiu");

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

string *K0 = new string("$k0");

string *A0 = new string("$a0");

string *ZERO = new string("$zero");

string *NULSTR = new string();

// Ŀ���������ļ�·��
ofstream mips_fout("output/mips_out.asm");

//toolbox
int Compiler::find_func_idx(string name) {
	int fb_idx=-1;
	int* c_alloced_reg;
	for (int i = 0;i < fun_blocks_idx;i++) {
		if (fun_blocks[i].name == name) {
			fb_idx = i;
			c_alloced_reg = this->cur_alloc_reg[i];
			break;
		}
	}
	return fb_idx;
}
//toolbox

// ����mipsָ��
void Compiler::gen_mips_code(string* op, string* dst, string* src1, string* src2) {
	string* comma1 = new string("\t");	string* comma2 = new string("\t");
	if (*src1 != "")
		*comma1 += ",";
	if (*src2 != "" && *op != *LW && *op!=*SW)
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
	int num = rand() % (MAX-3) + 1; int n = 0;
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

void Compiler::init_reg(RegRecord* reg_list){
	//vector<int> occupied;
	// �ҵ�cur_alloc_reg�еĺ���ƫ�ƣ���cur_alloc_reg����ӳ�䵽RegRecord��
	// �Ƚ����мĴ���д���ڴ��ٶ�����֮���ȫ�ֱ���Ҫ��call֮ǰ��д�ش���
	// �ȿ�ʼд������
	//this->simu_reg_alloc(&occupied);
	string fun_name = this->cur_active_funtab->getName();
	funRegref frr = reg_ref_tab.at(fun_name);
	int f_idx;
	// ����fun_idx
	for (int j = 0;j < this->fun_blocks_idx;j++) {
		if (fun_blocks[j].name == fun_name) {
			f_idx = j;
			break;
		}
	}
	// ȫ����ʼ��
	for (int i = 0;i < MaxAvaiReg;++i){
		reg_list[i].available = true;
		reg_list[i].global = false;
		reg_list[i].offset = 0;
		reg_list[i].idx = -1;		// ���ǿ���������
		reg_list[i].name = "";
		reg_list[i].occupied = false;
		reg_list[i].first_hit = false;
	}
	// frr.size()�����еĺ����������ϣ���cur_alloc_reg��������Ӧ
	for (int ii = 0;ii < frr.size();ii++) {
		int reg_idx = this->cur_alloc_reg[f_idx][ii];
		Item* it;
		// ͬһ���Ĵ����������ظ������
		if (reg_idx >= 0) {
			reg_list[reg_idx].occupied = true;
			/*reg_list[reg_idx].name = frr[ii].name;
			reg_list[reg_idx].global = frr[ii].global;
			reg_list[reg_idx].available = false;
			//reg_list[reg_idx].idx
			it = this->get_const_var_item(frr[ii].name);
			if (it != nullptr) {
				reg_list[reg_idx].offset = it->offset;
			}*/
		}
	}
}

void Compiler::get_reg(string* reg_name, RegType* reg_kind, string* name, bool* hit){
	string fun_name = this->cur_active_funtab->getName();
	funRegref frr = reg_ref_tab.at(fun_name);
	bool para_flag = false;
	if (*reg_name == "&ax") {
		para_flag = true;
	}
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
	// �ҷ��������ı���
	int f_idx;
	for (int i = 0;i < fun_blocks_idx;i++) {
		funBlocks* fb = &fun_blocks[i];
		if (fb->name == fun_name) {
			f_idx = i;
			break;
		}
	}
	int var_idx;
	for (int i = 0;i < frr.size();i++) {
		if (frr[i].name == *name) {
			var_idx = i;
			break;
		}
	}
	// frr.size()�����еĺ����������ϣ���cur_alloc_reg��������Ӧ
	/*for (int ii = 0;ii < frr.size();ii++) {
		int reg_idx = this->cur_alloc_reg[f_idx][ii];
		Item* it;
		// ͬһ���Ĵ����������ظ������
		if (reg_idx >= 0) {
			regs[reg_idx].occupied = true;
			regs[reg_idx].name = frr[ii].name;
			regs[reg_idx].global = frr[ii].global;
			regs[reg_idx].available = false;
			//reg_list[reg_idx].idx
			it = this->get_const_var_item(frr[ii].name);
			if (it != nullptr) {
				regs[reg_idx].offset = it->offset;
			}
		}
	}*/
	// �����˼Ĵ��������ڼĴ����в��ҿ��üĴ����Ƿ�ʹ��
	int reg_idx = this->cur_alloc_reg[f_idx][var_idx];
	if (reg_idx >= 0) {
		*reg_name = this->get_ts_reg(reg_idx, reg_kind);
		if (regs[reg_idx].name == *name) {
			//��ǰ�ļĴ����Ѿ���ʹ�ã�ֱ�ӷ��ؼĴ�������
		}
		else {
			// ��ǰ�ļĴ�������һ����֮����ͻ�ļĴ���ռ�ã����������ͻ�ļĴ���д���ڴ棻���߸üĴ�����û�з���
			Item* it;
			//if(!regs[reg_idx].available)
				//this->put_in_memory(reg_name, name);
			regs[reg_idx].available = false;
			regs[reg_idx].global = frr[var_idx].global;
			regs[reg_idx].name = *name;
			regs[reg_idx].occupied = true;
			it = this->get_const_var_item(frr[var_idx].name);
			if (it != nullptr) {
				regs[reg_idx].offset = it->offset;
			}
			if (para_flag && it->offset < cur_active_funtab->paranum && it->offset < 4) {
				string oft = "$a"+int22string(it->offset);
				this->gen_mips_code(MOVE, reg_name, &oft, NULSTR);
			}
			else
				this->put_in_reg(reg_name, name);
		}
		return;
	}
	
	Item* it = this->get_const_var_item(*name);
	
	// δ���䵫ҲҪ��һ�¼Ĵ������Ƿ��У����ܼĴ������Ѿ�����
	for (int i = 0; i < MaxAvaiReg;++i) {
		if (regs[i].name == *name) {
			*reg_name = this->get_ts_reg(i, reg_kind);
			*hit = true;
			return;
		}
	}
	
	// �пյľͷ�һ���յ�
	for (int i = 0; i < MaxAvaiReg;++i) {
		if (regs[i].available == true && regs[i].occupied == false && i>=this->global_reg_num) {
			regs[i].name = *name;
			regs[i].offset = it->offset;
			regs[i].available = false;
			regs[i].global = it->global;
			regs[i].first_hit = false;
			*reg_name = this->get_ts_reg(i, reg_kind);
			// δ�����п����ǲ���
			// ��Բ��������ҵ��Ƿ���idx<4�����ǵĻ���ӷ��ض�Ӧ����ֵ
			//Item* it = this->get_const_var_item(*name);
			if (para_flag && it->offset < cur_active_funtab->paranum && it->offset < 4) {
				string oft = "$a" + int22string(it->offset);
				this->gen_mips_code(MOVE, reg_name, &oft, NULSTR);
			}
			else
				this->put_in_reg(reg_name, name);
			*hit = true;
			return;
		}
	}
	// Ȼ�����ڴ�
	//Item* it = this->get_const_var_item(*name);
	string* offset = new string();
	static int i_r = 0;
	while(true){
		int j = i_r % MaxAvaiReg;
		if (regs[j].occupied == false) {
			// ��save
			int save_offset = this->cur_active_funtab->offset+MaxAvaiReg+1;
			Item* it_old = this->get_const_var_item(regs[j].name);
			*reg_name = this->get_ts_reg(j, reg_kind);
			if (regs[j].global) {
				this->int2string(offset, 4*it_old->offset);
				this->gen_mips_code(SW, reg_name, offset, GP_BRC);
			}
			else {
				//this->int2string(offset, -4 * it_old->offset);
				//this->gen_mips_code(SW, reg_name, offset, FP_BRC);
				this->int2string(offset, 4 * (save_offset-it_old->offset));
				this->gen_mips_code(SW, reg_name, offset, SP_BRC);
			}
			// ��ǰreg���� ?? 
			//this->gen_mips_code(LI, reg_name, new string("0"), NULSTR);
			// �ٷ���
			*reg_name = this->get_ts_reg(j, reg_kind);
			this->put_in_reg(reg_name, name);
			regs[j].name = *name;
			regs[j].offset = it->offset;
			regs[j].available = false;
			regs[j].global = it->global;
			regs[j].first_hit = false;
			delete(offset);
			++i_r;
			return;
		}
		++i_r;
	}
}

void Compiler::get_res_reg(string* reg_name, RegType* reg_kind, string* name, bool* hit) {
	string fun_name = this->cur_active_funtab->getName();
	funRegref frr = reg_ref_tab.at(fun_name);
	// �ǲ��Ƿ���ֵ
	if (*name == "%RET") {
		*reg_kind = RET_RG;
		*reg_name = *V0;
		return;
	}
	// ��Բ��������ҵ��Ƿ�hit���ӣ�����Ҫ���Ĵ���,LHY�����滻��λ��
	if (*reg_name == *A0)
		return;
	int f_idx;
	for (int i = 0;i < fun_blocks_idx;i++) {
		funBlocks* fb = &fun_blocks[i];
		if (fb->name == fun_name) {
			f_idx = i;
			break;
		}
	}
	// �ҷ��������ı���
	int var_idx;
	for (int i = 0;i < frr.size();i++) {
		if (frr[i].name == *name) {
			var_idx = i;
			break;
		}
	}
	int reg_idx = this->cur_alloc_reg[f_idx][var_idx];
	// �����˼Ĵ��������ڼĴ����в��ҿ��üĴ����Ƿ�ʹ��
	if (reg_idx >= 0) {
		*reg_name = this->get_ts_reg(reg_idx, reg_kind);
		if (regs[reg_idx].name == *name) {
			//��ǰ�ļĴ����Ѿ���ʹ�ã�ֱ�ӷ��ؼĴ�������
		}
		else {
			// ��ǰ�ļĴ�������һ����֮����ͻ�ļĴ���ռ�ã����������ͻ�ļĴ���д���ڴ棻���߸üĴ�����û�з���
			Item* it;
			//if (!regs[reg_idx].available)
				//this->put_in_memory(reg_name, name);
			regs[reg_idx].available = false;
			regs[reg_idx].global = frr[var_idx].global;
			regs[reg_idx].name = *name;
			regs[reg_idx].occupied = true;
			it = this->get_const_var_item(frr[var_idx].name);
			if (it != nullptr) {
				regs[reg_idx].offset = it->offset;
			}
		}
		return;
	}
	// δ���䵫ҲҪ��һ�¼Ĵ������Ƿ��У����ܼĴ������Ѿ�����
	for (int i = 0; i < MaxAvaiReg;++i) {
		if (regs[i].name == *name) {
			*reg_name = this->get_ts_reg(i, reg_kind);
			*hit = true;
			return;
		}
	}
	Item* it = this->get_const_var_item(*name);
	// �пյľͷ�һ���յ�
	for (int i = 0; i < MaxAvaiReg;++i) {
		if (regs[i].available == true && regs[i].occupied == false && i>=this->global_reg_num) {
			regs[i].name = *name;
			regs[i].offset = it->offset;
			regs[i].available = false;
			regs[i].global = it->global;
			regs[i].first_hit = false;
			*reg_name = this->get_ts_reg(i, reg_kind);
			//this->put_in_reg(reg_name, name);
			*hit = true;
			return;
		}
	}
	string* offset = new string();
	static int i_r = 0;
	while (true) {
		int j = i_r % MaxAvaiReg;
		if (regs[j].occupied == false && j >= this->global_reg_num) {
			// ��save
			int save_offset = cur_active_funtab->offset + MaxAvaiReg + 1;
			Item* it_old = this->get_const_var_item(regs[j].name);
			*reg_name = this->get_ts_reg(j, reg_kind);
			if (regs[j].global) {
				this->int2string(offset, 4 * it_old->offset);
				this->gen_mips_code(SW, reg_name, offset, GP_BRC);
			}
			else {
				this->int2string(offset, 4 * (save_offset-it_old->offset));
				this->gen_mips_code(SW, reg_name, offset, SP_BRC);
			}
			// ��ǰreg���� ?? 
			//this->gen_mips_code(LI, reg_name, new string("0"), NULSTR);
			// �ٷ���
			*reg_name = this->get_ts_reg(j, reg_kind);
			//this->put_in_reg(reg_name, name);
			regs[j].name = *name;
			regs[j].offset = it->offset;
			regs[j].available = false;
			regs[j].global = it->global;
			regs[j].first_hit = false;
			delete(offset);
			++i_r;
			return;
		}
		++i_r;
	}
}

void Compiler::get_arr_reg(string* reg_name, RegType* reg_kind, string* name){
	// bug����һ����ֵ
	// �ǲ��Ƿ���ֵ
	if (*name == "%RET") {
		*reg_kind = RET_RG;
		this->gen_mips_code(MOVE, T9, V0, NULSTR);
		*reg_name = *T9;
		return;
	}
	// �ȿ��Ƿ���const
	if (this->reg_is_const(name)) {
		//int arr_idx = atoi(name->c_str());
		//this->gen_mips_code(LI, T9, name, NULSTR);
		*reg_kind = CONST_RG;
		return;
	}
	*reg_name = *T9;
	string fun_name = this->cur_active_funtab->getName();
	funRegref frr = reg_ref_tab.at(fun_name);
	int f_idx;
	for (int i = 0;i < fun_blocks_idx;i++) {
		funBlocks* fb = &fun_blocks[i];
		if (fb->name == fun_name) {
			f_idx = i;
			break;
		}
	}
	// �ҷ��������ı���
	int var_idx;
	for (int i = 0;i < frr.size();i++) {
		if (frr[i].name == *name) {
			var_idx = i;
			break;
		}
	}
	int reg_idx = this->cur_alloc_reg[f_idx][var_idx];
	// �����˼Ĵ��������ڼĴ����в��ҿ��üĴ����Ƿ�ʹ��
	if (reg_idx >= 0) {
		*reg_name = this->get_ts_reg(reg_idx, reg_kind);
		if (regs[reg_idx].name == *name) {
			//��ǰ�ļĴ����Ѿ���ʹ�ã�move��t9
			this->gen_mips_code(MOVE, T9, reg_name, NULSTR);
			*reg_name = *T9;
		}
		else {
			// ֱ��load��t9��
			this->put_in_reg(T9, name);
			*reg_name = *T9;
		}
	}
	else {
		Item* it = get_const_var_item(*name);
		for (int i = 0; i < MaxAvaiReg;++i) {
			if (regs[i].name == *name) {
				*reg_name = this->get_ts_reg(i, reg_kind);
				//*hit = true;
				return;
			}
		}

		// �пյľͷ�һ���յ�
		for (int i = 0; i < MaxAvaiReg;++i) {
			if (regs[i].available == true && regs[i].occupied == false && i >= this->global_reg_num) {
				regs[i].name = *name;
				regs[i].offset = it->offset;
				regs[i].available = false;
				regs[i].global = it->global;
				regs[i].first_hit = false;
				//*reg_name = this->get_ts_reg(i, reg_kind);
				// δ�����п����ǲ���
				// ��Բ��������ҵ��Ƿ���idx<4�����ǵĻ���ӷ��ض�Ӧ����ֵ
				//Item* it = this->get_const_var_item(*name);
				this->put_in_reg(reg_name, name);
				return;
			}
		}
		// Ȼ�����ڴ�
		//Item* it = this->get_const_var_item(*name);
		string* offset = new string();
		static int i_r = 0;
		while (true) {
			int j = i_r % MaxAvaiReg;
			if (regs[j].occupied == false) {
				// ��save
				int save_offset = this->cur_active_funtab->offset + MaxAvaiReg + 1;
				Item* it_old = this->get_const_var_item(regs[j].name);
				*reg_name = this->get_ts_reg(j, reg_kind);
				if (regs[j].global) {
					this->int2string(offset, 4 * it_old->offset);
					this->gen_mips_code(SW, reg_name, offset, GP_BRC);
				}
				else {
					//this->int2string(offset, -4 * it_old->offset);
					//this->gen_mips_code(SW, reg_name, offset, FP_BRC);
					this->int2string(offset, 4 * (save_offset - it_old->offset));
					this->gen_mips_code(SW, reg_name, offset, SP_BRC);
				}
				// ��ǰreg���� ?? 
				//this->gen_mips_code(LI, reg_name, new string("0"), NULSTR);
				// �ٷ���
				*reg_name = this->get_ts_reg(j, reg_kind);
				this->put_in_reg(reg_name, name);
				regs[j].name = *name;
				regs[j].offset = it->offset;
				regs[j].available = false;
				regs[j].global = it->global;
				regs[j].first_hit = false;
				delete(offset);
				++i_r;
				return;
			}
			++i_r;
		}
	}
}

void Compiler::put_in_reg(string* reg_name, string* name) {
	Item* it = this->get_const_var_item(*name); 
	string* offset = new string();
	this->int2string(offset, 4*it->offset);
	if (it->global) {
		this->gen_mips_code(LW, reg_name, offset, GP_BRC);
	}
	else {
		int save_offset = cur_active_funtab->offset + MaxAvaiReg + 1;
		string tmp = int22string(4 * (save_offset-it->offset));
		this->gen_mips_code(LW, reg_name, &tmp, SP_BRC);
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
		int save_offset = cur_active_funtab->offset + MaxAvaiReg + 1;
		string tmp = int22string(4 * (save_offset - it->offset));
		this->gen_mips_code(SW, reg_name, &tmp, SP_BRC);
	}
	// �����Ӧ�ļĴ���ռ�ñ�ʶ �Ż�ǰ��
	for (int i = 0; i < MaxAvaiReg;++i) {
		if (regs[i].name == *name) {
			regs[i].available = true;
			//regs[i].global = true;
			regs[i].offset = 0;
			regs[i].idx = -1;		// ���ǿ���������
			regs[i].name = "";
			return;
		}
	}
}

string Compiler::get_ts_reg(int i, RegType* reg_kind) {
	string* reg_name = new string();
	if (i <= 7) {
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

void Compiler::handle_init(Midcode* mid, int mid_i) { //? offset�ò�������������ȥ
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
	// save����
	int save_offset = fun_offset + MaxAvaiReg + 1;
	this->int2string(fun_offset_str, -4 * save_offset);
	this->gen_mips_code(ADDI, SP, SP, fun_offset_str);

	/*int paranum = item_f->paranum;	this->int2string(para_num, paranum * 4);
	if (paranum >= 0) {
		this->gen_mips_code(ADDI, SP, SP, para_num);
	}*/
	// �Ż����޸ģ�ֻ���浱ǰ����ʹ�õ�ȫ�ּĴ���
	funRegref* frr = &reg_ref_tab.at(cur_active_funtab->getName());
	if (*fun_name != "main") {
		int fb_idx = this->find_func_idx(*fun_name);
		// ����funblocks��idx
		int* c_alloced_reg=this->cur_alloc_reg[fb_idx];
		
		// Ȼ����е�ǰ��������ȫ�ֱ����Ĳ�ѯ
		set<int>* cur_used_regs = &this->cur_active_funtab->used_regs;
		for (int i = 0;i < frr->size();i++) {
			int rg = this->cur_alloc_reg[fb_idx][i];
			if ((*cur_used_regs).find(rg) == (*cur_used_regs).end()) {
				(*cur_used_regs).insert(rg);
			}
		}
		for (int i = 0;i < MaxAvaiReg;i++) {
			if ((*cur_used_regs).find(i) != (*cur_used_regs).end()) {
				string prefix = "";
				if (i <= 7) {
					this->int2string(reg_num_str, i);
					prefix = "$t" + *reg_num_str;
				}
				else {
					this->int2string(reg_num_str, i - 8);
					prefix = "$s" + *reg_num_str;
				}
				this->int2string(fun_offset_str, 4 * (save_offset-(fun_offset + i)));
				this->gen_mips_code(SW, &prefix, fun_offset_str, SP_BRC);
			}
		}
		// ��ջ֡�ͷ��ص�ַ�����Ը�Ϊֻ���õ�
		//fun_offset += MaxAvaiReg;	this->int2string(fun_offset_str, -4 * fun_offset);
		//this->gen_mips_code(SW, FP, fun_offset_str, SP_BRC);
		fun_offset += MaxAvaiReg;				this->int2string(fun_offset_str, 4 * (save_offset-(fun_offset)));
		this->gen_mips_code(SW, RA, fun_offset_str, SP_BRC);
		
	}
	/*// Ԥ���ֲ���������ʱ��������Ĵ���
	if (cur_active_funtab->getName() != "main") {
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
		// ��ջ֡�ͷ��ص�ַ�����Ը�Ϊֻ���õ�
		fun_offset += MaxAvaiReg;	this->int2string(fun_offset_str, -4 * fun_offset);
		this->gen_mips_code(SW, FP, fun_offset_str, SP_BRC);
		fun_offset++;				this->int2string(fun_offset_str, -4 * fun_offset);
		this->gen_mips_code(SW, RA, fun_offset_str, SP_BRC);
	}*/
	else {
		fun_offset += (MaxAvaiReg+1);
	}
	// ����ջ֡Ϊ��ǰ����ջ֡������ջ��sp
	//this->gen_mips_code(MOVE, FP, SP, NULSTR);
	//fun_offset++;				this->int2string(fun_offset_str, -4 * fun_offset);
	//this->gen_mips_code(ADDI, SP, SP, fun_offset_str);
	this->init_reg(regs);
	// �Ѳ�����ȡ������
	Item* it;
	string *reg_name; RegType reg_kind; bool hit;
	if (cur_active_funtab->paranum > 0) {
		for (int i = 0;i < cur_active_funtab->paranum && i < 4;i++) {
			string name = cur_active_funtab->paraList[i].getName();
			string* tmp_name = new string("&ax");
			this->get_reg(tmp_name, &reg_kind, &name, &hit);
		}
	}

	delete(fun_name);
	delete(para_num);
	delete(fun_offset_str);
	delete(reg_num_str);
}
// �ȰѲ����������ڴ���
void Compiler::handle_push(Midcode* mid, int mid_i){
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
	/*else {
		if(hit == false || hit == true)
			this->put_in_reg(reg_name, mid->src1);
	}*/
	// �õ�����ƫ����
	int para_oft = atoi(mid->dst->c_str());
	if (para_oft < 4 && para_oft >= 0) {
		this->gen_mips_code(MOVE, &("$a" + *(mid->dst)), reg_name, NULSTR);
		//this->gen_mips_code(ADDI, SP, SP, new string("-4"));
	}
	// ƫ��������4��д���ڴ�
	else {
		this->gen_mips_code(SW, reg_name, new string(int22string(-4*para_oft)), SP_BRC);
		//this->gen_mips_code(ADDI, SP, SP, new string("-4"));
	}
	/*if (this->para_offset < 4 && this->para_offset >= 0) {
		this->int2string(para_idx, this->para_offset);
		this->gen_mips_code(MOVE, &("$a" + *para_idx), reg_name,NULSTR);
	}
	
	this->gen_mips_code(SW, reg_name, new string("0"), SP_BRC);
	this->gen_mips_code(ADDI, SP, SP, new string("-4"));*/
	this->clear_tmp_reg(mid_i);
	delete(reg_name);
}
void Compiler::handle_call(Midcode* mid, int mid_i){
	this->clear_tmp_reg(mid_i);
	// ÿcallһ���ж�ǰ�����������ļĴ�������
	for (int i = 0; i < MaxAvaiReg;++i) {
		// ��������global�Ǻ��������ʱ������úõģ�����������ܻ��б������ֲ�û��д������
		if (regs[i].global && regs[i].occupied && regs[i].name!="") {
			// global����Ҫд���ڴ�
			RegType rg;
			string reg_name;
			reg_name = this->get_ts_reg(i, &rg);
			this->put_in_memory(&reg_name, &regs[i].name);
			regs[i].available = true;
			regs[i].name = "";
			regs[i].offset = 0;
		}
		/*regs[i].available = true;
		regs[i].global = true;
		regs[i].offset = 0;
		regs[i].idx = -1;		// ���ǿ���������
		regs[i].name = "";
		regs[i].first_hit = true;*/
	}
	this->gen_mips_code(JAL, mid->src1, NULSTR, NULSTR);
}

void Compiler::handle_branch(Midcode* mid, int mid_i){
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
	else if(src1_type == CONST_RG && src2_type != CONST_RG){
		this->gen_mips_code(LI, T9, src1_reg, NULSTR);
		/*if(src2_hit == false || src2_hit == true)
			this->put_in_reg(src2_reg, mid->src2);*/
		*src1_reg = *T9;
	}
	else if (src1_type != CONST_RG && src2_type == CONST_RG) {
		this->gen_mips_code(LI, T9, src2_reg, NULSTR);
		/*if (src1_hit == false || src1_hit == true)
			this->put_in_reg(src1_reg, mid->src1);*/
		*src2_reg = *T9;
	}
	/*else {
		if (src1_hit == false || src1_hit == true)
			this->put_in_reg(src1_reg, mid->src1);
		if (src2_hit == false || src2_hit == true)
			this->put_in_reg(src2_reg, mid->src2);
	}*/
	// ���д��
	this->clear_tmp_reg(mid_i);
	switch (mid->mid_op) {
	case BGJ_M:
		this->gen_mips_code(BGT, src1_reg, src2_reg,mid->dst);
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

	delete(src1_reg);
	delete(src2_reg);
}
void Compiler::handle_jmp(Midcode* mid, int mid_i){
	// ���д��
	this->clear_tmp_reg(mid_i);
	this->gen_mips_code(J, mid->src1, NULSTR, NULSTR);
}
void Compiler::handle_scanf(Midcode* mid, int mid_i){
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
	this->get_res_reg(reg_name, &rg, name, &hit);
	this->gen_mips_code(MOVE, reg_name, V0, NULSTR);
	//this->put_in_memory(V0, name);
	// ���д��
	this->clear_tmp_reg(mid_i);
	delete(reg_name);
	delete(name);
}
void Compiler::handle_printf(Midcode* mid, int mid_i){
	//string* reg_name = new string();
	if (mid->mid_op == PRINTV_M) {
		RegType rg;		bool hit = false;
		string* reg_name = new string();
		this->get_reg(reg_name, &rg, mid->src2, &hit);
		if (rg == CONST_RG) {
			this->gen_mips_code(LI, A0, reg_name, NULSTR);
			*reg_name = *A0;
		}
		/*else if (hit == false || hit == true) {			// ��hit��仰������
			this->put_in_reg(A0, mid->src2);
		}*/
		else {
			//this->put_in_reg(reg_name, mid->src2);				// ��hit��仰ɾ��
			this->gen_mips_code(MOVE, A0, reg_name, NULSTR);	
		}
		if (*(mid->src1) == "char") {
			this->gen_mips_code(LI, V0, new string("11"), NULSTR);
		}
		else {
			this->gen_mips_code(LI, V0, new string("1"), NULSTR);
		}
	}
	else{
		string tmp = (*mid->src1);
		tmp.erase(0, 5);
		this->gen_mips_code(LA, A0, &("$String"+tmp), NULSTR);
		this->gen_mips_code(LI, V0, new string("4"), NULSTR);
	}
	this->gen_mips_code(SYSCALL, NULSTR, NULSTR, NULSTR);
	//�����Ϊ�˼ӿո�ģ���������û����
	/*if (mid->mid_op == PRINTV_M) {
		string* tmp = new string();
		this->int2string(tmp, string_tab.size()-1);
		this->gen_mips_code(LA, A0, &("$String" + *tmp), NULSTR);
		this->gen_mips_code(LI, V0, new string("4"), NULSTR);
		this->gen_mips_code(SYSCALL, NULSTR, NULSTR, NULSTR);
	}*/
	// ���д��
	this->clear_tmp_reg(mid_i);
}
void Compiler::handle_return(Midcode* mid, int mid_i){
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
			/*if(hit==false || hit == true)
				this->put_in_reg(reg_name, mid->src1);*/
			this->gen_mips_code(MOVE, V0, reg_name, NULSTR);
		}
		// ���д��
		this->clear_tmp_reg(mid_i);
	}
	// RET_M
	else {
		// ���д��
		//��ȫ�ּĴ���д��
		/*for (int k = 0;k < MaxAvaiReg;k++) {
			if (regs[k].global == true && regs[k].available == false && regs[k].occupied==true) {
				RegType rg;
				string reg_name = this->get_ts_reg(k, &rg);
				string name = regs[k].name;
				this->put_in_memory(&reg_name, &name);
			}
		}*/

		// �ָ����ص�ַ��ջ֡
		string* reg_num_str = new string();
		string* offset_str = new string();
		FuncItem* item_f = this->get_func_item(*(mid->src1));
		//int load_offset = item_f->offset;
		this->gen_mips_code(LW, RA, new string("4"), SP_BRC);
		//this->gen_mips_code(LW, FP, new string("8"), SP_BRC);
		// ��ʹ�õļĴ���load����
		set<int>* cur_used_regs = &this->cur_active_funtab->used_regs;
		for (int i = MaxAvaiReg - 1; i >= 0;--i) {
			if ((*cur_used_regs).find(i) != (*cur_used_regs).end()) {
				string prefix = "";
				if (i <= 7) {
					this->int2string(reg_num_str, i);
					prefix = "$t" + *reg_num_str;
				}
				else {
					this->int2string(reg_num_str, i - 8);
					prefix = "$s" + *reg_num_str;
				}
				this->int2string(offset_str, 4 * (1 + MaxAvaiReg - i));
				this->gen_mips_code(LW, &prefix, offset_str, SP_BRC);
			}
		}
		int fun_offset = 4*(1 + MaxAvaiReg + item_f->offset);	this->int2string(offset_str, fun_offset);
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
void Compiler::handle_operation(Midcode* mid, int mid_i){
	string* OP = new string();
	switch (mid->mid_op) {
	case ADD_M:	*OP = *ADDU; break;
	case SUB_M: *OP = *SUB; break;
	case MUL_M: *OP = *MUL; break;
	case DIV_M: *OP = *DIV; break;
	}
	string* dst_reg = new string();		RegType dst_tp;		bool dst_hit = false;
	string* src1_reg = new string();	RegType src1_tp;	bool src1_hit = false;
	string* src2_reg = new string();	RegType src2_tp;	bool src2_hit = false;
	
	this->get_reg(src1_reg, &src1_tp, mid->src1, &src1_hit);
	this->get_reg(src2_reg, &src2_tp, mid->src2, &src2_hit);
	this->get_res_reg(dst_reg, &dst_tp, mid->dst, &dst_hit);
	if (src1_tp == CONST_RG && src2_tp == CONST_RG) {
		// ����dagͼ�Ż�������������������������תΪ��ֵ��ʽ
		int a1 = atoi(mid->src1->c_str());
		int a2 = atoi(mid->src2->c_str());
		int tar;
		switch (mid->mid_op) {
		case ADD_M:	tar = a1 + a2; break;
		case SUB_M: tar = a1 - a2; break;
		case MUL_M: tar = a1 * a2; break;
		case DIV_M: tar = a1 / a2; break;
		}
		string* src = new string();
		*src = int22string(tar);
		this->gen_mips_code(LI, dst_reg, src, NULSTR);
	}
	else if (src1_tp == CONST_RG && src2_tp != CONST_RG) {
		// ���ڵ�һ��������Ϊ�������������Ӻͳ˿���ֱ�ӻ�λ�ã����Ǽ��ͳ�������
		/*if (src2_hit == true)
			this->put_in_reg(src2_reg, mid->src2);*/
		if (*OP == *SUB) {
			this->gen_mips_code(LI, T9, src1_reg, NULSTR);
			*src1_reg = *T9;
			this->gen_mips_code(OP, dst_reg, src1_reg, src2_reg);
		}
		else if (*OP == *DIV) {
			this->gen_mips_code(LI, T9, src1_reg, NULSTR);
			*src1_reg = *T9;
			this->gen_mips_code(OP, NULSTR, src1_reg, src2_reg);
			this->gen_mips_code(MFLO, dst_reg, NULSTR, NULSTR);
		}
		else if (*OP == *ADDU) {
			this->gen_mips_code(ADDIU, dst_reg, src2_reg, src1_reg);
		}
		else {
			this->gen_mips_code(OP, dst_reg, src2_reg, src1_reg);
		}
	}
	else if (src1_tp != CONST_RG && src2_tp == CONST_RG) {
		switch (mid->mid_op) {
		case ADD_M: *OP = *ADDIU; break;
		case SUB_M:
			*OP = *ADDIU;
			int num = atoi(src2_reg->c_str());
			if (num >= 0) {
				*src2_reg = "-" + *src2_reg;
			}
			else {
				src2_reg->erase(0, 1);
			}
			break;
		}

		this->gen_mips_code(OP, dst_reg, src1_reg, src2_reg);
	}
	else {
		if (mid->mid_op == DIV_M) {
			this->gen_mips_code(DIV, NULSTR, src1_reg, src2_reg);
			this->gen_mips_code(MFLO, dst_reg, NULSTR, NULSTR);
		}
		else
			this->gen_mips_code(OP, dst_reg, src1_reg, src2_reg);
	}
	//this->put_in_memory(dst_reg, mid->dst);
	// ���д��
	this->clear_tmp_reg(mid_i);
	delete(dst_reg);
	delete(src1_reg);
	delete(src2_reg);
	delete(OP);
}

void Compiler::handle_assign(Midcode* mid, int mid_i){
	string* src_reg = new string();
	string* dst_reg = new string();
	RegType src_tp, dst_tp;
	bool src_hit = false, dst_hit = false;
	this->get_reg(src_reg, &src_tp, mid->src1, &src_hit);
	this->get_res_reg(dst_reg, &dst_tp, mid->dst, &dst_hit);
	if (src_tp == CONST_RG) {
		this->gen_mips_code(LI, dst_reg, src_reg, NULSTR);
	}
	else if (src_tp == RET_RG) {
		this->gen_mips_code(MOVE, dst_reg, src_reg, NULSTR);
	}
	else {
		this->gen_mips_code(MOVE, dst_reg, src_reg, NULSTR);
	}
	//this->put_in_memory(dst_reg, mid->dst);
	// ���д��
	this->clear_tmp_reg(mid_i);
	delete(src_reg);
	delete(dst_reg);
}
void Compiler::handle_arrto(Midcode* mid, int mid_i){
	// �ȿ��Ĵ�������û��
	RegType rg;
	string* arr_reg = new string();
	string* idx = new string();
	string* tar_reg = new string();
	string* offset = new string();
	bool hit = false;
	/*this->get_reg(arr_reg, &rg, mid->src1, &hit);
	if(rg==)*/
	//this->get_reg(arr_reg, &rg, mid->src1, mid->src2);
	/*if (rg != NFIND) {
		this->get_reg(tar_reg, &rg, mid->dst, &hit);
		this->gen_mips_code(MOVE, tar_reg, arr_reg, NULSTR);
	}*/
	// �Ĵ�����û��
/*else */{
		Item* it = this->get_const_var_item(*(mid->src1));
		this->get_arr_reg(idx, &rg, mid->src2);
		if (rg == CONST_RG) {
			int index = atoi(mid->src2->c_str());
			if (it->global)
				index = 4 * (it->offset+index);
			else
				index = 4 * (cur_active_funtab->offset + 1 + MaxAvaiReg - it->offset - index);
			*idx = *T9;		this->int2string(offset, index);
			this->gen_mips_code(LI, idx, offset, NULSTR);
		}
		else {
			if (it->global) {
				this->int2string(offset, it->offset);
				this->gen_mips_code(ADDIU, T9, idx, offset);
				this->gen_mips_code(SLL, T9, T9, new string("2"));
			}
			else {
				this->int2string(offset, cur_active_funtab->offset + 1 + MaxAvaiReg - it->offset);
				this->gen_mips_code(LI, T8, offset, NULSTR);
				this->gen_mips_code(SUB, T9, T8, idx);
				this->gen_mips_code(SLL, T9, T9, new string("2"));
			}
			*idx = *T9;
		}
		/*else {
			this->gen_mips_code(MOVE, T9, idx, NULSTR);
			*idx = *T9;
		}*/
		// ֱ�Ӹ�x = tmp[n]��ı�ԭ��n�ĸ�ֵ������ֻ����t9����
		if (it->global)
			this->gen_mips_code(ADDU, idx, idx, GP);
		else
			this->gen_mips_code(ADDU, idx, SP, idx);
		hit = false;
		this->get_res_reg(tar_reg, &rg, mid->dst, &hit);
		this->gen_mips_code(LW, tar_reg, new string("0"), &("(" + *idx + ")"));
		//this->put_in_memory(tar_reg, mid->dst);
	}
	// ���д��
	this->clear_tmp_reg(mid_i);
	
	delete(offset);
	delete(arr_reg);
	delete(tar_reg);
	delete(idx);
}
void Compiler::handle_arrget(Midcode* mid, int mid_i){
	string* idx = new string();
	string* from = new string();
	string* offset = new string();
	string* from_num = new string();
	RegType rg;
	bool hit = false;
	// �����ں����е�ƫ��
	this->get_arr_reg(idx, &rg, mid->src2);
	Item* it = this->get_const_var_item(*(mid->src1));
	
	if (rg == CONST_RG) {
		int index = atoi(mid->src2->c_str());
		if (it->global)
			index = 4 * (it->offset + index);
		else
			index = 4 * (cur_active_funtab->offset + 1 + MaxAvaiReg - it->offset - index);
		*idx = *T9;		this->int2string(offset, index);
		this->gen_mips_code(LI, idx, offset, NULSTR);
	}
	else {
		if (it->global) {
			this->int2string(offset, it->offset);
			this->gen_mips_code(ADDIU, T9, idx, offset);
			this->gen_mips_code(SLL, T9, T9, new string("2"));
		}
		else {
			this->int2string(offset, cur_active_funtab->offset + 1 + MaxAvaiReg - it->offset);
			this->gen_mips_code(LI, T8, offset, NULSTR);
			this->gen_mips_code(SUB, T9, T8, idx);
			this->gen_mips_code(SLL, T9, T9, new string("2"));
		}
		*idx = *T9;
	}
	if (it->global)
		this->gen_mips_code(ADDU, T9, GP, T9);
	else
		this->gen_mips_code(ADDU, T9, SP, T9);
	// dstװ���ڴ�
	hit = false;
	this->get_reg(from, &rg, mid->dst, &hit);
	if (rg == CONST_RG) {
		int num = atoi(from->c_str());
		this->int2string(from, num);
		this->gen_mips_code(LI, T8, from, NULSTR);
		*from = *T8;
	}
	/*else if(hit == false || hit == true) {
		this->put_in_reg(from, mid->dst);
	}*/
	this->gen_mips_code(SW, from, new string("0"), &("(" + *idx + ")"));
	// ���д��
	this->clear_tmp_reg(mid_i);

	delete(idx);
	delete(from);
	delete(offset);
	delete(from_num);
}

void Compiler::handle_exit() {
	this->gen_mips_code(LI, V0, new string("10"), NULSTR);
	this->gen_mips_code(SYSCALL, NULSTR, NULSTR, NULSTR);
}

void Compiler::clear_tmp_reg(int mid_i) {
	Blocks* blk;
	funBlocks* f_blk;
	funRegref* frr = &reg_ref_tab.at(cur_active_funtab->getName());
	// ȷ���Ƿ�������
	for (int j = 0;j < code_blocks_idx;j++) {
		blk = &code_blocks[j];
		if (mid_i == blk->last_line_num) {
			// �ҵ���Ծ������
			//������һ������������䣬����ʱ�Ĵ������
			int f_idx = this->find_func_idx(cur_active_funtab->getName());	// [i][j][k]�е�i
			int bblk_of_fun = fun_blocks[f_idx].begin_block;				// [i][j][k]�е�j
			for (int k = 0;k < MaxAvaiReg;k++) {
				if (regs[k].occupied == false && regs[k].available == false && regs[k].global == true) {
					string reg_name = this->get_ts_reg(k, new RegType());
					string name = regs[k].name;
					this->put_in_memory(&reg_name, &name);
				}
				if (regs[k].occupied == false && regs[k].available == false) {
					// �ҵ������ڲ�ƫ��
					int var_offset = this->find_var_offset(regs[k].name, frr);// [i][j][k]�е�k
					// ���Ƿ��Ծ����Ծ�Ĳ�д�أ�����͸��ǰ�
					if (this->outflow[f_idx][j-bblk_of_fun][var_offset]/* || this->def[f_idx][j-bblk_of_fun][var_offset]*/) {
						RegType rg;
						string* reg_name = new string();
						*reg_name = this->get_ts_reg(k, &rg);
						this->put_in_memory(reg_name, &regs[k].name);
					}
					regs[k].available = true;
					regs[k].global = false;
					regs[k].name = "";
					regs[k].offset = 0;
				}
			}
			break;
		}
	}
}

bool Compiler::reg_is_const(string * name)
{
	return (  (name[0][0]>='0' && name[0][0]<='9')  ||  (name[0][0]=='-')  );
}


void Compiler::generateMips() {
	// �Ĵ�����
	this->regs = new RegRecord[MaxAvaiReg];
	// ��ʼ���ַ�����
	this->asciiz_setup();
	// ��ʼ�����
	string* op = new string();
	MIPS_OUT << ".text" << endl;
	this->gen_mips_code(ADDI, GP, GP, new string("0x10000"));
	this->gen_mips_code(ADDU, FP, SP, ZERO);
	this->gen_mips_code(J, new string("main"), NULSTR, NULSTR);
	// ��ʽ������Ԫʽ�ķ���׶�
	
	for (int i = 0;i < this->mid_list.size();++i) {
		Midcode mid = mid_list[i];
		switch (mid_list[i].mid_op) {
		case ADD_M:
		case SUB_M:
		case MUL_M:
		case DIV_M:
			this->handle_operation(&mid,i);
			break;
		
		case BGJ_M:
		case BGEJ_M:
		case LSJ_M:
		case LSEJ_M:
		case EQJ_M:
		case NEQJ_M:
			this->handle_branch(&mid, i);
			break;

		case ASS_M:
			this->handle_assign(&mid, i);
			break;
		case J_M:
			this->handle_jmp(&mid, i);
			break;
		case LABEL_M:
			this->handle_label(mid.src1);
			break;
		case SCANF_M:
			this->handle_scanf(&mid, i);
			break;
		case PRINTS_M:
		case PRINTV_M:
			this->handle_printf(&mid, i);
			break;
		case CALL_M:
			this->handle_call(&mid, i);
			break;
		case FUN_M:
			this->handle_init(&mid, i);
			break;
		case PUSH_M:
			this->handle_push(&mid, i);
			break;
		case RETV_M:
		case RET_M:// ������j���
			this->handle_return(&mid, i);
			break;
		case EXIT_M:
			this->handle_exit();
			break;

		case ARGET_M:
			this->handle_arrget(&mid, i);
			break;
		case ARTO_M:
			this->handle_arrto(&mid, i);
			break;
		default:
			this->gen_mips_code(NOP, NULSTR, NULSTR, NULSTR);
			break;
		}
		
	}
}

#endif