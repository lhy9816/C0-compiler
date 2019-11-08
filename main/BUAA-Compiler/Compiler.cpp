#include "Compiler.h"


extern ofstream dot_fout;
extern ofstream dag_fout;
extern ofstream reg_count_fout;
extern ofstream live_fout;
extern ofstream live_dot_fout;
extern ofstream dag_dot_fout;

Compiler::Compiler(char* path)
{
	this->origin_text = readFiletoString(path);
}


Compiler::~Compiler()
{
}

string Compiler::readFiletoString(char *filename) {
	ifstream ifile(filename);
	//将文件读入到ostringstream对象buf中
	ostringstream buf;
	char ch;
	while (buf&&ifile.get(ch))
		buf.put(ch);
	//返回与流对象buf关联的字符串
	return buf.str();
}

void Compiler::begin() {
	// 错误表建立
	this->errorSetup();
	// 词法分析初始化
	this->lexicalSetup();
	// 开始语法语义分析，生成中间代码
	this->program();
	
	// 有错误就停止
	if (this->error_count > 0)
		return;

	if (!OPT) {
		this->generateMips();
	}
	else {
		// 窥孔优化
		this->peephole_optimization();
		this->peephole_optimization();

		// 构建dag图
		this->read_mips_list();
		this->ref_count_regalloc();
		this->data_flow_analysis();
		this->dag_manipulate();

		// clear一下
		for (int i = 0;i < fun_blocks_idx;i++) {
			int blocks_range = fun_blocks[i].end_block_line - fun_blocks[i].begin_block_line + 1;
			int block_begin_line = fun_blocks[i].begin_block_line;	int block_end_line = fun_blocks[i].end_block_line;
			int block_begin = fun_blocks[i].begin_block;	int block_end = fun_blocks[i].end_block;

			funRegref frr = reg_ref_tab[fun_blocks[i].name];
			for (int j = 0;j < block_end - block_begin + 1;j++) {
				delete[] this->outflow[i][j];
			}
			delete[] this->outflow[i];
		}
		delete[] this->outflow;
		for (int i = 0;i < fun_tab.size();i++) {
			delete[] this->cur_alloc_reg[i];
		}
		delete[] this->cur_alloc_reg;
		for (int i = 0;i < this->fun_blocks_idx;i++) {
			funBlocks* fb = &fun_blocks[fun_blocks_idx];
			fb->name = "";
			fb->begin_block = fb->begin_block_line = fb->end_block = fb->end_block_line = 0;
		}
		//memset(this->code_blocks, 0, sizeof(this->code_blocks));
		//memset(this->fun_blocks, 0, sizeof(this->fun_blocks));
		reg_ref_tab.clear();
		dot_fout.close();
		dag_fout.close();
		reg_count_fout.close();
		live_fout.close();
		live_dot_fout.close();
		dag_dot_fout.close();
		// clear一下

		this->read_mips_list();
		this->ref_count_regalloc();
		this->data_flow_analysis();
		// 引用计数发
		//this->ref_count_regalloc();
		// 活跃变量分析与冲突图
		//this->data_flow_analysis();

		// 开始生成目标代码
		this->generateMips();
	}

	
	system("pause");
}


