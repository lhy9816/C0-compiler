#include "Compiler.h"

#define DEBUG (0)
#if DEBUG
#define MID_OUT cout
#else
#define MID_OUT mid_fout
#endif //DEBUG
#define DAG_DEBUG (0)
#if DAG_DEBUG
#define DAG_MID_OUT cout
#else
#define DAG_MID_OUT dag_mid_fout
#endif //DEBUG

/*******************************************************************************\
 *																			   *
 *								生成中间代码实现函数							   *
 *																			   *
\*******************************************************************************/

// 中间代码的文件输出位置
ofstream mid_fout("output/mid_out.txt");
ofstream dag_mid_fout("output/dag_mid_out.txt");

void Compiler::gen_tmp_name(string* name) {
	*name = "#" + std::to_string(this->tmp_reg_idx++);
	this->put_var(*name, INTTK, 0, 0);
}

void Compiler::gen_string_name(string* name) {
	*name = "^str_" + std::to_string(this->label_idx++);
}

void Compiler::gen_label_name(string* name) {
	*name = "LABEL" + std::to_string(this->string_idx++);
}

Midcode* Compiler::push_mid_code(mid_op M, string* src1, string* src2, string* dst) {
	Midcode* mid = new Midcode();
	mid->mid_op = M;
	
	mid->src1 = new string();	
	mid->src2 = new string();	
	mid->dst = new string();	
	mid->is_print = false;
	if (src1 != nullptr) {
		*(mid->src1) = *src1;
	}
	if (src2 != nullptr) {
		*(mid->src2) = *src2;
	}
	if (dst != nullptr) {
		*(mid->dst) = *dst;
	}
	this->mid_list.push_back(*mid);
	return mid;
}

void Compiler::print_mid_code(Midcode* mid) {
	std::string dst = *(mid->dst);
	std::string src1 = *(mid->src1);
	std::string src2 = *(mid->src2);
	switch (mid->mid_op) {
	case ADD_M:
		MID_OUT << dst << " = " << src1 << " + " << src2 << endl;
		mid_sout.push_back(dst + " = " + src1 + " + " + src2);
		break;
	case SUB_M:
		MID_OUT << dst << " = " << src1 << " - " << src2 << endl;
		mid_sout.push_back(dst + " = " + src1 + " - " + src2);
		break;
	case MUL_M:
		MID_OUT << dst << " = " << src1 << " * " << src2 << endl;
		mid_sout.push_back(dst + " = " + src1 + " * " + src2);
		break;
	case DIV_M:
		MID_OUT << dst << " = " << src1 << " / " << src2 << endl;
		mid_sout.push_back(dst + " = " + src1 + " / " + src2);
		break;
	case BGJ_M:
		MID_OUT << "if " << src1 << " > " << src2 << " goto ~" << dst << endl;
		mid_sout.push_back("if " + src1 + " > " + src2 + " goto ~" + dst);
		break;
	case BGEJ_M:
		MID_OUT << "if " << src1 << " >= " << src2 << " goto ~" << dst << endl;
		mid_sout.push_back("if " + src1 + " >= " + src2 + " goto ~" + dst);
		break;
	case LSJ_M:
		MID_OUT << "if " << src1 << " < " << src2 << " goto ~" << dst << endl;
		mid_sout.push_back("if " + src1 + " < " + src2 + " goto ~" + dst);
		break;
	case LSEJ_M:
		MID_OUT << "if " << src1 << " <= " << src2 << " goto ~" << dst << endl;
		mid_sout.push_back("if " + src1 + " <= " + src2 + " goto ~" + dst);
		break;
	case EQJ_M:
		MID_OUT << "if " << src1 << " == " << src2 << " goto ~" << dst << endl;
		mid_sout.push_back("if " + src1 + " == " + src2 + " goto ~" + dst);
		break;
	case NEQJ_M:
		MID_OUT << "if " << src1 << " != " << src2 << " goto ~" << dst << endl;
		mid_sout.push_back("if " + src1 + " != " + src2 + " goto ~" + dst);
		break;

	case ASS_M:
		MID_OUT << dst << " = " << src1 << endl;
		mid_sout.push_back(dst + " = " + src1);
		break;
	case J_M:
		MID_OUT << "@j " << " ~" << src1 << endl;
		mid_sout.push_back("@j  ~" + src1);
		break;
	case LABEL_M:
		MID_OUT << "~" << src1 << endl;
		mid_sout.push_back("~" + src1);
		break;
	case SCANF_M:
		MID_OUT << "@scanf " << src1 << endl;
		mid_sout.push_back("@scanf " + src1);
		break;
	case PRINTS_M:
		MID_OUT << "@prints " << src1 << endl;
		mid_sout.push_back("@prints " + src1);
		break;
	case PRINTV_M:
		MID_OUT << "@printv " << src1 << " " << src2 << endl;
		mid_sout.push_back("@printv " + src1 + " " + src2);
		break;
	case CALL_M:
		MID_OUT << "@call " << src1 << endl;
		mid_sout.push_back("@call " + src1);
		break;
	case FUN_M:
		MID_OUT << "@init " << src1 << endl;
		mid_sout.push_back("@init " + src1);
		break;
	case PUSH_M:
		MID_OUT << "@push " << src1 << " " << src2 << " " << dst << endl;
		mid_sout.push_back("@push " + src1 + " " + src2 + " " + dst);
		break;
	case RETV_M:
		MID_OUT << "%RET = " << src1 << endl;
		mid_sout.push_back("%RET = " + src1);
		break;
	case RET_M:
		MID_OUT << "@ret " << src1 << endl;
		mid_sout.push_back("@ret " + src1);
		break;
	case EXIT_M:
		MID_OUT << "<EXIT> " << endl;
		mid_sout.push_back("<EXIT> ");
		break;

	case ARGET_M:
		MID_OUT << src1 << "[" << src2 << "] = " << dst << endl;
		mid_sout.push_back(src1 + "[" + src2 + "] = " + dst);
		break;
	case ARTO_M:
		MID_OUT << dst << " = " << src1 << "[" << src2 << "]" << endl;
		mid_sout.push_back(dst + " = " + src1 + "[" + src2 + "]");
		break;

	default:
		MID_OUT << "nop" << endl;
		mid_sout.push_back("nop");
		break;
	}
}

void Compiler::print_mid_code_after_dag(Midcode* mid) {
	std::string dst = *(mid->dst);
	std::string src1 = *(mid->src1);
	std::string src2 = *(mid->src2);
	switch (mid->mid_op) {
	case ADD_M:
		DAG_MID_OUT << dst << " = " << src1 << " + " << src2 << endl;
		mid_sout.push_back(dst + " = " + src1 + " + " + src2);
		break;
	case SUB_M:
		DAG_MID_OUT << dst << " = " << src1 << " - " << src2 << endl;
		mid_sout.push_back(dst + " = " + src1 + " - " + src2);
		break;
	case MUL_M:
		DAG_MID_OUT << dst << " = " << src1 << " * " << src2 << endl;
		mid_sout.push_back(dst + " = " + src1 + " * " + src2);
		break;
	case DIV_M:
		DAG_MID_OUT << dst << " = " << src1 << " / " << src2 << endl;
		mid_sout.push_back(dst + " = " + src1 + " / " + src2);
		break;
	case BGJ_M:
		DAG_MID_OUT << "if " << src1 << " > " << src2 << " goto ~" << dst << endl;
		mid_sout.push_back("if " + src1 + " > " + src2 + " goto ~" + dst);
		break;
	case BGEJ_M:
		DAG_MID_OUT << "if " << src1 << " >= " << src2 << " goto ~" << dst << endl;
		mid_sout.push_back("if " + src1 + " >= " + src2 + " goto ~" + dst);
		break;
	case LSJ_M:
		DAG_MID_OUT << "if " << src1 << " < " << src2 << " goto ~" << dst << endl;
		mid_sout.push_back("if " + src1 + " < " + src2 + " goto ~" + dst);
		break;
	case LSEJ_M:
		DAG_MID_OUT << "if " << src1 << " <= " << src2 << " goto ~" << dst << endl;
		mid_sout.push_back("if " + src1 + " <= " + src2 + " goto ~" + dst);
		break;
	case EQJ_M:
		DAG_MID_OUT << "if " << src1 << " == " << src2 << " goto ~" << dst << endl;
		mid_sout.push_back("if " + src1 + " == " + src2 + " goto ~" + dst);
		break;
	case NEQJ_M:
		DAG_MID_OUT << "if " << src1 << " != " << src2 << " goto ~" << dst << endl;
		mid_sout.push_back("if " + src1 + " != " + src2 + " goto ~" + dst);
		break;

	case ASS_M:
		DAG_MID_OUT << dst << " = " << src1 << endl;
		mid_sout.push_back(dst + " = " + src1);
		break;
	case J_M:
		DAG_MID_OUT << "@j " << " ~" << src1 << endl;
		mid_sout.push_back("@j  ~" + src1);
		break;
	case LABEL_M:
		DAG_MID_OUT << "~" << src1 << endl;
		mid_sout.push_back("~" + src1);
		break;
	case SCANF_M:
		DAG_MID_OUT << "@scanf " << src1 << endl;
		mid_sout.push_back("@scanf " + src1);
		break;
	case PRINTS_M:
		DAG_MID_OUT << "@prints " << src1 << endl;
		mid_sout.push_back("@prints " + src1);
		break;
	case PRINTV_M:
		DAG_MID_OUT << "@printv " << src1 << " " << src2 << endl;
		mid_sout.push_back("@printv " + src1 + " " + src2);
		break;
	case CALL_M:
		DAG_MID_OUT << "@call " << src1 << endl;
		mid_sout.push_back("@call " + src1);
		break;
	case FUN_M:
		DAG_MID_OUT << "@init " << src1 << endl;
		mid_sout.push_back("@init " + src1);
		break;
	case PUSH_M:
		DAG_MID_OUT << "@push " << src1 << " " << src2 << " " << dst << endl;
		mid_sout.push_back("@push " + src1 + " " + src2 + " " + dst);
		break;
	case RETV_M:
		DAG_MID_OUT << "%RET = " << src1 << endl;
		mid_sout.push_back("%RET = " + src1);
		break;
	case RET_M:
		DAG_MID_OUT << "@ret " << src1 << endl;
		mid_sout.push_back("@ret " + src1);
		break;
	case EXIT_M:
		DAG_MID_OUT << "<EXIT> " << endl;
		mid_sout.push_back("<EXIT> ");
		break;

	case ARGET_M:
		DAG_MID_OUT << src1 << "[" << src2 << "] = " << dst << endl;
		mid_sout.push_back(src1 + "[" + src2 + "] = " + dst);
		break;
	case ARTO_M:
		DAG_MID_OUT << dst << " = " << src1 << "[" << src2 << "]" << endl;
		mid_sout.push_back(dst + " = " + src1 + "[" + src2 + "]");
		break;

	default:
		DAG_MID_OUT << "nop" << endl;
		mid_sout.push_back("nop");
		break;
	}
}

bool Compiler::isVarType(tokenType type) {
	return(type == INTTK || type == CHARTK);
}