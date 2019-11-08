#include "Compiler.h"

#define DEBUG (0)
#if DEBUG
#define DAG_OUT cout
#else
#define DAG_OUT dag_fout
#endif //DEBUG

#define DOT_DEBUG (0)
#if DOT_DEBUG
#define DOT_OUT cout
#else
#define DOT_OUT dot_fout
#endif //DOT_DEBUG

#define REF_COUNT_DEBUG (0)
#if REF_COUNT_DEBUG
#define REF_COUNT_OUT cout
#else
#define REF_COUNT_OUT reg_count_fout
#endif // REF_COUNT_OUT

#define LIVE_DEBUG (0)
#if LIVE_DEBUG
#define LIVE_OUT cout
#else
#define LIVE_OUT live_fout
#endif //DOT_DEBUG

#define LIVE_DOT_DEBUG (0)
#if LIVE_DOT_DEBUG
#define LIVE_DOT_OUT cout
#else
#define LIVE_DOT_OUT live_dot_fout
#endif //DOT_DEBUG

#define DAG_DOT_DEBUG (0)
#if DAG_DOT_DEBUG
#define DAG_DOT_OUT cout
#else
#define DAG_DOT_OUT dag_dot_fout
#endif //DOT_DEBUG


// dag�����������
extern ofstream dot_fout("D://ԭF��//graphviz//dag_test.dot");
extern ofstream dag_fout("output/dag_out.txt");
extern ofstream reg_count_fout("output/reg_count_out.txt");
extern ofstream live_fout("output/live_out.txt");


                                      
extern ofstream live_dot_fout("D://ԭF��//graphviz//live_test.dot");
extern ofstream dag_dot_fout("D://ԭF��//graphviz//dag_picture_test.dot");

//TOOLBOX
string Compiler::int22string(int a) {
	stringstream s;
	s << a;
	return s.str();
}
string trancate_string(string a, char b) {
	stringstream ss;
	for (int i = 0;i < a.size();i++) {
		if (a[i] == b) {
			for (int j = i + 1;j < a.size();j++) {
				ss << a[j];
			}
			return ss.str();
		}
	}
}
//TOOLBOX

// �������ʼ����
void Compiler::dag_init() {
	this->code_blocks_idx = 0;
	this->fun_blocks_idx = 0;
	Blocks* blk;
	for (int i = 0;i < MaxCodeBlocks;++i) {
		blk = &code_blocks[i];
		blk->first_line_num = blk->last_line_num = blk->idx = blk->j_block = blk->follow_block = LHY;
		blk->pre_blocks_cnt = 0;
		blk->is_print = false;
		memset(blk->pre_blocks, -1, sizeof(blk->pre_blocks));
	}
}


void Compiler::read_mips_list() {
	DOT_OUT << "digraph dag {" << endl;
	this->dag_init();

	// ��һ����м���뽨��dag�����ָ�
	for (int i = 0;i < this->mid_list.size();++i) {
		Midcode mid = mid_list[i];
		switch (mid_list[i].mid_op) {

		case ADD_M:
		case SUB_M:
		case MUL_M:
		case DIV_M:
			//this->handle_operation(&mid);
			break;

		case BGJ_M:
		case BGEJ_M:
		case LSJ_M:
		case LSEJ_M:
		case EQJ_M:
		case NEQJ_M:
			code_blocks[code_blocks_idx].last_line_num = i;
			code_blocks[code_blocks_idx].j_name = *mid.dst;
			if (code_blocks_idx + 1 < MaxCodeBlocks && i + 1 < mid_list.size()) {
				code_blocks[code_blocks_idx].follow_block = code_blocks_idx + 1;
				code_blocks_idx++;
				code_blocks[code_blocks_idx].pre_blocks[code_blocks[code_blocks_idx].pre_blocks_cnt++] = code_blocks_idx - 1;
				if (mid_list[i + 1].mid_op == LABEL_M || mid_list[i + 1].mid_op == FUN_M)
					continue;
				code_blocks[code_blocks_idx].first_line_num = i + 1;
				code_blocks[code_blocks_idx].idx = code_blocks_idx;
				//code_blocks[code_blocks_idx].name = "AFTER BRANCH " + code_blocks[code_blocks_idx].name + *(int2string(new string(), i+1));
			}
			break;

		case ASS_M:
			//this->handle_assign(&mid);
			break;
		case J_M:
			code_blocks[code_blocks_idx].last_line_num = i;
			code_blocks[code_blocks_idx].j_name = *mid.src1;
			if (code_blocks_idx + 1 < MaxCodeBlocks && i + 1 < mid_list.size() - 1) {
				code_blocks[code_blocks_idx++].follow_block = -1;
				if (mid_list[i + 1].mid_op == LABEL_M || mid_list[i + 1].mid_op == FUN_M)
					continue;
				code_blocks[code_blocks_idx].first_line_num = i + 1;
				code_blocks[code_blocks_idx].idx = code_blocks_idx;
				//code_blocks[code_blocks_idx].is_print = true;
				code_blocks[code_blocks_idx].name = "AFTER J " + code_blocks[code_blocks_idx].name + *(int2string(new string(), i + 1));
				//this->print_item_dot(int2string(new string(), i + 2), int2string(new string(), i + 2));
			}
			break;
		case LABEL_M:
			if (code_blocks[code_blocks_idx].first_line_num != LHY) {
				code_blocks[code_blocks_idx].last_line_num = i - 1;
				code_blocks[code_blocks_idx].follow_block = code_blocks_idx + 1;	code_blocks_idx++;
				code_blocks[code_blocks_idx].pre_blocks[code_blocks[code_blocks_idx].pre_blocks_cnt++] = code_blocks_idx - 1;
			}
			code_blocks[code_blocks_idx].first_line_num = i;
			code_blocks[code_blocks_idx].idx = code_blocks_idx;
			code_blocks[code_blocks_idx].name = *mid.src1;

			// ��������Ż�֮�����ɾȥ
			if (mid_list[i + 1].mid_op == LABEL_M) {
				code_blocks[code_blocks_idx].last_line_num = i;
				code_blocks[code_blocks_idx].follow_block = code_blocks_idx + 1;
				code_blocks_idx++;
				code_blocks[code_blocks_idx].pre_blocks[code_blocks[code_blocks_idx].pre_blocks_cnt++] = code_blocks_idx - 1;
			}
			break;
		case SCANF_M:
			break;
		case PRINTS_M:
		case PRINTV_M:
			break;
		case CALL_M:
			code_blocks[code_blocks_idx].last_line_num = i;
			//code_blocks[code_blocks_idx].j_name = *mid.src1;
			if (code_blocks_idx + 1 < MaxCodeBlocks && i + 1 < mid_list.size()) {
				code_blocks[code_blocks_idx].follow_block = code_blocks_idx + 1;
				code_blocks_idx++;
				code_blocks[code_blocks_idx].pre_blocks[code_blocks[code_blocks_idx].pre_blocks_cnt++] = code_blocks_idx - 1;
				if (mid_list[i + 1].mid_op == LABEL_M || mid_list[i + 1].mid_op == FUN_M)
					continue;
				code_blocks[code_blocks_idx].first_line_num = i + 1;
				code_blocks[code_blocks_idx].idx = code_blocks_idx;
				code_blocks[code_blocks_idx].name = "GET RETURN FROM " + *mid.src1 + *(int2string(new string(), i + 1));
				//this->print_item_dot(int2string(new string(), i + 2), int2string(new string(), i + 2));
			}
			break;
		case FUN_M:
			if (code_blocks[code_blocks_idx].first_line_num != LHY) {
				code_blocks[code_blocks_idx].last_line_num = i - 1;
				code_blocks[code_blocks_idx].follow_block = code_blocks_idx + 1;	code_blocks_idx++;
				code_blocks[code_blocks_idx].pre_blocks[code_blocks[code_blocks_idx].pre_blocks_cnt++] = code_blocks_idx - 1;
			}
			code_blocks[code_blocks_idx].first_line_num = i;
			code_blocks[code_blocks_idx].last_line_num = i;
			fun_blocks[fun_blocks_idx].begin_block_line = i;
			fun_blocks[fun_blocks_idx].begin_block = code_blocks_idx;
			fun_blocks[fun_blocks_idx].name = *mid.src1;

			code_blocks[code_blocks_idx].idx = code_blocks_idx;
			code_blocks[code_blocks_idx].name = *mid.src1;

			if (code_blocks_idx + 1 < MaxCodeBlocks && i + 1 < mid_list.size()) {
				code_blocks[code_blocks_idx].follow_block = code_blocks_idx + 1;
				code_blocks_idx++;
				code_blocks[code_blocks_idx].pre_blocks[code_blocks[code_blocks_idx].pre_blocks_cnt++] = code_blocks_idx - 1;
				if (mid_list[i + 1].mid_op == LABEL_M || mid_list[i + 1].mid_op == FUN_M)
					continue;
				code_blocks[code_blocks_idx].first_line_num = i + 1;
				code_blocks[code_blocks_idx].idx = code_blocks_idx;
			}
			break;
		case PUSH_M:
			break;
		case RETV_M:
			break;
		case RET_M:
			code_blocks[code_blocks_idx].last_line_num = i;
			fun_blocks[fun_blocks_idx].end_block = code_blocks_idx;
			fun_blocks[fun_blocks_idx++].end_block_line = i;
			if (code_blocks_idx + 1 < MaxCodeBlocks && i + 1 < mid_list.size()) {
				code_blocks[code_blocks_idx++].follow_block = -1;
			}
			break;
		case EXIT_M:
			code_blocks[code_blocks_idx].last_line_num = i;
			code_blocks[code_blocks_idx].j_block = -2;

			fun_blocks[fun_blocks_idx].end_block = code_blocks_idx;
			code_blocks[code_blocks_idx++].follow_block = -2;
			fun_blocks[fun_blocks_idx++].end_block_line = i;
			//this->print_item_dot(new string("EXIT"), new string("<EXIT>"));
			break;

		case ARGET_M:
			break;
		case ARTO_M:
			break;
		default:
			break;
		}
	}
	// �ڶ����dag����dag��֮�������
	for (int i = 0;i < this->code_blocks_idx;++i) {
		Blocks* blk_i;
		blk_i = &code_blocks[i];
		for (int j = 0;j < this->code_blocks_idx;++j) {
			Blocks* blk_j;
			blk_j = &code_blocks[j];
			if (blk_i->name == blk_j->j_name && blk_i->name != "") {
				blk_j->j_block = blk_i->idx;
				blk_i->pre_blocks[blk_i->pre_blocks_cnt++] = blk_j->idx;
				//this->print_link_dot(&blk_j->name, &blk_i->name);
			}
		}
	}
	// ɾ��������
	/*for (int i = 0;i < code_blocks_idx;++i) {
		Blocks* blk_i;
		blk_i = &code_blocks[i];
		if (mid_list[blk_i->first_line_num].mid_op != FUN_M && blk_i->pre_blocks_cnt <= 0) {
			// �����Ƴ������
			int rmv_lines = blk_i->last_line_num - blk_i->first_line_num + 1;

			for (int j = i + 1;j < code_blocks_idx;++j) {
				Blocks* blk_j;
				blk_j = &code_blocks[j];
				blk_j->first_line_num -= rmv_lines;
				blk_j->last_line_num -= rmv_lines;
				blk_j->follow_block -= (blk_j->follow_block>=0)?(1):(0);
				//blk_j->j_block -= (blk_j->j_block>=blk_j->idx)?(1):(0);
				blk_j->idx -= 1;
				blk_j->pre
			}
			code_blocks_idx -= 1;
		}
	}*/
	// ��ʼ��ӡ
	for (int i = 0;i < this->code_blocks_idx;i++) {
		string* idx_tmp = new string();
		this->int2string(idx_tmp, i);
		string lines = *idx_tmp + ":\n";
		Blocks* blk = &code_blocks[i];
		for (int j = blk->first_line_num;j <= blk->last_line_num;++j) {
			lines += mid_sout[j] + "\\n";
		}
		this->print_item_dot(int2string(new string(), i), &lines);
		delete idx_tmp;
	}
	for (int i = 0;i < this->code_blocks_idx;i++) {
		Blocks* blk = &code_blocks[i];
		string* src = new string();
		this->int2string(src, i);
		if (blk->follow_block > 0) {
			this->print_link_dot(src, int2string(new string(), blk->follow_block));
		}
		if (blk->j_block >= 0) {
			this->print_link_dot(src, int2string(new string(), blk->j_block));
		}
		delete src;
	}

	DOT_OUT << "}" << endl;
}

int Compiler::find_dag_list(dagNodelist** dnl, int dnloffset, string name, dagNodelist* node) {
	for (int i = 0;i < dnloffset;i++) {
		if (dnl[i]->name == name) {
			node = dnl[i];
			return i;
		}
	}
	return -1;
}

void Compiler::print_item_dagpicdot(string* name, string* lines) {
	DAG_DOT_OUT << "\t" << *name << " [label=\"" << *lines << "\"]" << endl;
}

void Compiler::print_link_dagpicdot(string* src, string* dst) {
	DAG_DOT_OUT << "\t" << *src << " -> " << *dst << endl;
}

void Compiler::dag_manipulate() {
	vector<Midcode> tmp_mid_list;
	for (int i = 0;i < mid_list.size();i++) {
		Midcode* midd = new Midcode();
		midd->src1 = new string();
		midd->src2 = new string();
		midd->dst = new string();
		if (mid_list[i].dst != nullptr) { *midd->dst = *mid_list[i].dst; }
		if (mid_list[i].src1 != nullptr) { *midd->src1 = *mid_list[i].src1; }
		if (mid_list[i].src2 != nullptr) { *midd->src2 = *mid_list[i].src2; }

		midd->mid_op = mid_list[i].mid_op;
		tmp_mid_list.push_back(*midd);
	}
	if (!mid_list.empty())
		mid_list.clear();
	if (!mid_sout.empty())
		mid_sout.clear();
	// ����dagͼ��ʼ����ÿ��code_blocks
	int fun_idx;
	funRegref frr;
	for (int i = 0;i < code_blocks_idx;i++) {
		Blocks* cur_block = &code_blocks[i];
		Midcode* mid;
		int firstline = cur_block->first_line_num;
		int lastline = cur_block->last_line_num;
		funBlocks* cur_fun_blocks = nullptr;
		// ��ȡ��������
		for (int j = 0;j < fun_blocks_idx;j++) {
			cur_fun_blocks = &fun_blocks[j];
			if (cur_block->first_line_num >= cur_fun_blocks->begin_block_line && cur_block->last_line_num <= cur_fun_blocks->end_block_line) {
				// �ҵ�fun_blocks�е����
				fun_idx = j;
				this->cur_active_funtab = fun_tab.at(cur_fun_blocks->name);
				frr = reg_ref_tab.at(fun_blocks[j].name);
				break;
			}
		}
		this->dag_list_num = 0;
		this->dag_node_num = 0;
		for (int il = firstline;il <= lastline;il++) {
			mid = &tmp_mid_list[il];
			dagNode* dag_nodes[MaxDagNodes];
			dagNodelist* dag_node_list[MaxDagNodes];
			// ���õ�ֱ������������������ȡ������������һЩ  PUSH��PRINTF����Ҫ��Ҫֱ������أ������м��������ʱ�������أ�����LHY
			if (mid->mid_op == LABEL_M || mid->mid_op == FUN_M ||
				/*mid->mid_op == J_M || mid->mid_op == CALL_M ||*/ mid->mid_op == EXIT_M || mid->mid_op == RET_M) {
				this->push_mid_code(mid->mid_op, mid->src1, mid->src2, mid->dst);
				this->print_mid_code_after_dag(mid);
				if (il != lastline || (il == lastline && dag_node_num == 0))
					continue;
			}
			dagNodelist* tmp_x_dnl = nullptr;
			dagNodelist* tmp_y_dnl = nullptr;
			dagNodelist* tmp_z_dnl = nullptr;
			string src1;	string src2;	string dst;
			switch (mid->mid_op) {
			case ADD_M:
			case SUB_M:
			case MUL_M:
			case DIV_M:
			case ASS_M:
			case SCANF_M:
			case PRINTV_M:
			case PRINTS_M:
			case PUSH_M:
			case ARTO_M:
			case ARGET_M:
			case RETV_M:
				if (mid->mid_op == ARGET_M) {
					src1 = *mid->src2;
					src2 = *mid->dst;
					dst = *mid->src1;
				}
				else if (mid->mid_op == SCANF_M/* || mid->mid_op == PRINTV_M || mid->mid_op == PRINTS_M || mid->mid_op == PUSH_M*/) {
					dst = "SCANF" + int22string(il) + "_" + *mid->src1;
					src1 = "SCANF";
					src2 = *mid->dst;
				}
				else if (mid->mid_op == PRINTV_M) {
					// printvֱ����nodelist��Ѱ��ӳ�䵽node���е�idx��Ȼ��ѡ��dag_nodes[idx]->idx���
					// �½�һ�����
					src1 = *mid->src1;
					dst = "PRINTV" + int22string(il) + "_" + src1;
					src2 = *mid->src2;
					/*int print_idx = find_dag_list(dag_node_list, dag_list_num, src1, nullptr);
					dagNodelist* print_node = dag_node_list[print_idx];
					dagNode* node = dag_nodes[print_node->idx];*/
				}
				else if (mid->mid_op == PRINTS_M) {
					src1 = *mid->src1;
					src2 = *mid->src2;
					dst = "PRINTS" + int22string(il) + "_" + src1;
				}
				else if (mid->mid_op == PUSH_M) {
					src1 = *mid->src1;
					src2 = *mid->dst;
					dst = "PUSH" + int22string(il) + "_" + src1;
				}
				else if (mid->mid_op == RETV_M) {
					src1 = *mid->src1;
					src2 = *mid->src2;
					dst = "RETV" + int22string(il) + "_" + src1;
				}
				else {
					src1 = *mid->src1;
					src2 = *mid->src2;
					dst = *mid->dst;
				}
				// ��������������������������ʼ��x
				int tmp_x_idx = find_dag_list(dag_node_list, dag_list_num, src1, tmp_x_dnl);
				if (tmp_x_idx >= 0) {
					tmp_x_dnl = dag_node_list[tmp_x_idx];
					dagNode* node_tx = dag_nodes[tmp_x_dnl->idx];
					int nodelist_tx_idx = find_dag_list(dag_node_list, dag_list_num, node_tx->name, nullptr);
					// ���ȡ�ý��������ڽ���Ϸ��������޸ģ���ô��ʱ�Ľ�����ֵ�ʹ�þͻ��з��գ��������������ֻ�������������õ���������������ֻ������ý��
					if (dag_node_list[nodelist_tx_idx]->idx > tmp_x_dnl->idx) {
						string name = node_tx->name;
						node_tx->name = src1;
						node_tx->name_candidates.erase(name);
					}
				}
				else {
					// û�ҵ���Ҫ���·���
					dagNode* node = new dagNode();
					node->isLeaf = true;
					node->child_1 = node->child_2 = -1;
					node->name = src1;
					if (reg_is_const(&src1)) {
						node->value = atoi(src1.c_str());
					}
					node->name_candidates.insert(src1);
					node->idx = dag_node_num;
					node->prt_num = 0;
					dag_nodes[dag_node_num++] = node;
					//node->op = mid_op ��Ҫnotop��
					tmp_x_dnl = new dagNodelist();
					tmp_x_dnl->idx = node->idx;
					tmp_x_dnl->name = src1;
					tmp_x_dnl->modified = false;
					dag_node_list[dag_list_num++] = tmp_x_dnl;
				}
				// ��ʼ��y
				// ��assign���⴦��
				if (mid->mid_op == ASS_M || mid->mid_op == SCANF_M ||/*mid->mid_op==PRINTV_M || */mid->mid_op == PRINTS_M || mid->mid_op == PUSH_M
					|| mid->mid_op == RETV_M) {
					int ass_idx = tmp_x_dnl->idx;
					bool flag = false;
					for (int k = 0;k < dag_list_num;k++) {
						dagNodelist* node = dag_node_list[k];
						int idx;
						if (node->name == dst) {
							node->idx = ass_idx;
							flag = true;
							
							break;
						}
					}
					if (!flag) {
						// û�ҵ�����һ��
						tmp_z_dnl = new dagNodelist();
						tmp_z_dnl->idx = ass_idx;
						tmp_z_dnl->modified = false;
						tmp_z_dnl->name = dst;
						dag_node_list[dag_list_num++] = tmp_z_dnl;
					}
					if (mid->mid_op == ASS_M && tmp_x_dnl->name != "%RET") {
						dagNode* node = dag_nodes[ass_idx];
						node->name_candidates.insert(dst);
						//���ˡ�Ҷ���ڵ㿴���м���
						if (node->isLeaf == true) {
							node->leaf_ass_name = node->name;
							node->op = ASS_M;
							node->isLeaf = false;
						}
						if (isLetter(dst) && node->name[0]=='#'/*!reg_is_const(&node->name)*/) {
							node->name = dst;//�϶�������������һ����ʱ�����������������ԭ�������������Ǿֲ�/ȫ�ֱ�����
						}
					}
					else {
						dagNode* node_z = new dagNode();
						node_z->op = mid->mid_op;
						node_z->isLeaf = false;
						node_z->child_1 = tmp_x_dnl->idx;
						node_z->name = dst;
						node_z->name_candidates.insert(dst);
						node_z->idx = dag_node_num;
						node_z->prt_num = 0;
						tmp_z_dnl->idx = node_z->idx;
						if (mid->mid_op == PUSH_M) {
							node_z->value = atoi(src2.c_str());
						}
						//node_x->prt_num++;   �Ȳ����parent��
						//node_y->prt_num++;
						dag_nodes[dag_node_num++] = node_z;
					}
					break;
				}

				int tmp_y_idx = find_dag_list(dag_node_list, dag_list_num, src2, tmp_y_dnl);
				if (tmp_y_idx >= 0) {
					tmp_y_dnl = dag_node_list[tmp_y_idx];
					dagNode* node_ty = dag_nodes[tmp_y_dnl->idx];
					int nodelist_ty_idx = find_dag_list(dag_node_list, dag_list_num, node_ty->name, nullptr);
					// ���ȡ�ý��������ڽ���Ϸ��������޸ģ���ô��ʱ�Ľ�����ֵ�ʹ�þͻ��з��գ��������������ֻ�������������õ���������������ֻ������ý��
					if (dag_node_list[nodelist_ty_idx]->idx > tmp_y_dnl->idx) {
						string name = node_ty->name;
						node_ty->name = src1;
						node_ty->name_candidates.erase(name);
					}
				}
				else {
					// û�ҵ���Ҫ���·���
					dagNode* node = new dagNode();
					node->isLeaf = true;
					node->child_1 = node->child_2 = -1;
					node->name = src2;
					if (reg_is_const(&src2)) {
						node->value = atoi(src1.c_str());
					}
					node->name_candidates.insert(src2);
					node->idx = dag_node_num;
					node->prt_num = 0;
					dag_nodes[dag_node_num++] = node;
					//node->op = mid_op ��Ҫnotop��
					tmp_y_dnl = new dagNodelist();
					tmp_y_dnl->idx = node->idx;
					tmp_y_dnl->name = src2;
					tmp_y_dnl->modified = false;
					dag_node_list[dag_list_num++] = tmp_y_dnl;
				}


				dagNode* node_z = nullptr;
				dagNode* node_x = dag_nodes[tmp_x_dnl->idx];
				dagNode* node_y = dag_nodes[tmp_y_dnl->idx];
				for (int k = 0;k < dag_node_num;k++) {
					dagNode* node = dag_nodes[k];
					if (node->child_1 == tmp_x_dnl->idx && node->child_2 == tmp_y_dnl->idx && node->op == mid->mid_op) {
						if (mid->mid_op == ARGET_M) {
							if (dst != node_z->name) {
								// �ҵ���������Ȼƫ�ƺ�targetֵ����ͬ��������������ͬ����δ�ҵ�
								continue;
							}
						}
						else if (mid->mid_op == SCANF_M || mid->mid_op == PRINTV_M || mid->mid_op == PRINTS_M || mid->mid_op == PUSH_M) {
							// do nothing, ֱ���½���㼴��
						}
						else {
							// �ҵ�z��㣬��������Ҫ����idx����ʷ���֣��Ա��������½����ʱ�������ʷ���ֵĸ���
							node_z = node;
							node_z->name_candidates.insert(dst);
							string name = node_z->name;
							if (name[0] >= '0' && name[0] <= '9') {
								// �ǳ�������ô���־Ͳ�����
								continue;
							}
							if (name[0] == '#') {
								// ����ʱ�������ǾͰ�����������ɾֲ���������ȫ�ֱ���
								set<string>::iterator iter;
								for (iter = node_z->name_candidates.begin();iter != node_z->name_candidates.end();iter++) {
									string name1 = *iter;
									if (!(name1[0] == '#')) {
										node_z->name = name1;
										break;
									}
								}
							}
						}
					}
				}
				if (!node_z/* && mid->mid_op!=ASS_M*/) {
					// �½�node
					// û�ҵ���Ҫ���·���
					node_z = new dagNode();
					node_z->op = mid->mid_op;
					node_z->isLeaf = false;
					node_z->child_1 = tmp_x_dnl->idx;
					node_z->child_2 = tmp_y_dnl->idx;
					node_z->name = dst;
					node_z->name_candidates.insert(dst);
					node_z->idx = dag_node_num;
					node_z->prt_num = 0;
					node_x->prt_num++;
					node_y->prt_num++;
					dag_nodes[dag_node_num++] = node_z;
				}
				int tmp_z_idx = find_dag_list(dag_node_list, dag_list_num, dst, tmp_z_dnl);
				if (tmp_z_idx >= 0) {
					tmp_z_dnl = dag_node_list[tmp_z_idx];
					tmp_z_dnl->idx = node_z->idx;
				}
				else {
					// �����в�����
					tmp_z_dnl = new dagNodelist();
					tmp_z_dnl->idx = node_z->idx;
					tmp_z_dnl->modified = false;
					tmp_z_dnl->name = dst;
					dagNode* node_x = dag_nodes[tmp_x_dnl->idx];
					dagNode* node_y = dag_nodes[tmp_y_dnl->idx];
					dag_node_list[dag_list_num++] = tmp_z_dnl;
				}
				break;
			}

			if (il == lastline) {
				// �Ȼ�ͼ���һ��
				DAG_DOT_OUT << "digraph dag {" << endl;
				bool flag = false;
				for (int j = 0;j < dag_node_num;j++) {
					//if (dag_nodes[j]->name != "") {
					this->print_item_dagpicdot(&(dag_nodes[j]->name + "9816" + int22string(j)), &(dag_nodes[j]->name + "_" + int22string(j)));
					//}
					/*else if (dag_nodes[j]->name == "") {
						if (!flag) {
							flag = true;
							this->print_item_dagpicdot(&(dag_nodes[j]->name), &(dag_nodes[j]->name));
						}
					}*/
				}
				for (int j = 0;j < dag_node_num;j++) {
					if (dag_nodes[j]->child_1 >= 0) {
						int child1idx = dag_nodes[j]->child_1;
						string name;
						this->print_link_dagpicdot(&(dag_nodes[j]->name + "9816" + int22string(j)), &(dag_nodes[child1idx]->name + "9816" + int22string(child1idx)));
					}
					if (dag_nodes[j]->child_2 >= 0) {
						int child2idx = dag_nodes[j]->child_2;
						this->print_link_dagpicdot(&(dag_nodes[j]->name + "9816" + int22string(j)), &(dag_nodes[child2idx]->name + "9816" + int22string(child2idx)));
					}
				}
				DAG_DOT_OUT << "}" << endl;
				// ����µ��м����
				// ���Ѿ�����Ĳ������
				int mid_node_len = 0;
				dagNode* mid_nodes[MaxDagNodes];
				if (dag_node_num != 0) {
					// ��ȡ�м�ڵ����
					for (int j = 0;j < dag_node_num;j++) {
						if (dag_nodes[j]->isLeaf == false) {
							mid_nodes[mid_node_len++] = dag_nodes[j];
						}
					}
					// ���ÿ���м�����д�ӡ
					dagNode* node_x;
					dagNode* node_y;
					for (int j = 0;j < mid_node_len;j++) {
						dagNode* node = mid_nodes[j];
						dagNodelist* mkup_node;
						node_x = dag_nodes[node->child_1];
						node_y = dag_nodes[node->child_2];
						string mkup_name;
						int mkup_idx;
						if (node->op == ARGET_M) {
							// ��Ҫ����һ��˳��
							Midcode* mid;
							mid = this->push_mid_code(node->op, &node->name, &node_x->name, &node_y->name);
							this->print_mid_code_after_dag(mid);
							mkup_name = node->name;
							mkup_idx = node->idx;
						}
						else if (node->op == SCANF_M) {
							// ��Ҫ����һ��˳��
							Midcode* mid;
							//string name = node->name.erase()
							mid = this->push_mid_code(node->op, &trancate_string(node->name, '_'), &node_x->name, &node_y->name);
							this->print_mid_code_after_dag(mid);
							mkup_name = node->name;
							mkup_idx = node->idx;
						}
						else if (node->op == PRINTV_M) {
							// ��Ҫ����һ��˳��
							Midcode* mid;
							string name = trancate_string(node->name, '_');
							Item* it = get_const_var_item(name);
							//if (it != nullptr) {
								//string kind = (it->getKind() == INTTK) ? ("int") : ("char");
							//ȷ���Ƿ��滻
							int print_idx = find_dag_list(dag_node_list, dag_list_num, node->name, nullptr);
							dagNode* print_node = dag_nodes[dag_node_list[print_idx]->idx];
							dagNode* node_kind = dag_nodes[print_node->child_1];
							dagNode* node_prt = dag_nodes[print_node->child_2];
							mid = this->push_mid_code(node->op, &node_kind->name, &node_prt->name, &node->name);
							this->print_mid_code_after_dag(mid);
							mkup_name = node->name;
							mkup_idx = node->idx;
							//}
						}
						else if (node->op == ASS_M && node->child_1<0 || node->op == ASS_M && dag_nodes[node->child_1]->name_candidates.find("%RET")== dag_nodes[node->child_1]->name_candidates.end()) {
							mkup_name = node->name;
							mkup_idx = node->idx;
							/*Midcode* mid;
							string tar_name;
							for (int k = 0;k < dag_list_num;k++) {
								dagNodelist* can_node = dag_node_list[k];
								if (node->idx == can_node->idx && node->leaf_ass_name != can_node->name) {
									tar_name = can_node->name;
									break;
								}
							}
							mid = this->push_mid_code(node->op, &node->leaf_ass_name, nullptr, &tar_name);
							this->print_mid_code_after_dag(mid);
							mkup_name = node->name;
							mkup_idx = node->idx;*/
						}
						else {
							Midcode* mid;
							if (node->op == PUSH_M) {
								string oft = int22string(node->value);
								mid = this->push_mid_code(node->op, &node_x->name, &node_y->name, &oft);
							}
							else
								mid = this->push_mid_code(node->op, &node_x->name, &node_y->name, &node->name);
							this->print_mid_code_after_dag(mid);
							mkup_name = node->name;
							mkup_idx = node->idx;
						}
						// ���Ƿ���Ҫ����
						//int mkup_idx = find_dag_list(dag_node_list, dag_list_num, mkup_name, nullptr);
						int mkup_offset;
						//mkup_node = dag_node_list[mkup_idx];
						// �ڽ����в�������ͬƫ�Ƶı���
						for (int k = 0;k < dag_list_num;k++) {
							if (dag_node_list[k]->idx == mkup_idx) {
								string var_name = dag_node_list[k]->name;
								// ��ʱ����
								if (var_name[0] == '#' && var_name != mkup_name) {
									//�ҵ������ڱ���ƫ��
									mkup_offset = find_var_offset(var_name, &frr);
									// ��Ծ
									if (this->outflow[fun_idx][i - cur_fun_blocks->begin_block][mkup_offset]) {
										Midcode* mid;
										mid = this->push_mid_code(ASS_M, &mkup_name, 0, &var_name);
										this->print_mid_code_after_dag(mid);
									}
									// ���ⲻ��Ծ���������Կ��ܻ�Ծ�����ǿ��ڿ��ܵĻ�Ծ�ص㶼����dagͼ�Ƶ�������������ֻʣ�¸û���������һ�������
									// ���һ��������ǱȽ���ת�����ȷ��һ����û��ʹ��
									else {
										Midcode* lastmid = &tmp_mid_list[lastline];
										mid_op op = lastmid->mid_op;
										if (op == BGJ_M || op == BGEJ_M || op == LSJ_M || op == LSEJ_M || op == EQJ_M || op == NEQJ_M) {
											if (*lastmid->src1 == var_name) {
												*lastmid->src1 = mkup_name;
											}
											if (*lastmid->src2 == var_name) {
												*lastmid->src2 = mkup_name;
											}
										}
									}
								}
								else if (isLetter(var_name) && var_name != mkup_name) {
									Item* it = this->get_const_var_item(var_name);
									if (it != nullptr && it->global) {
										// ȫ�ֱ�����һ��Ҫ����
										Midcode* mid;
										mid = this->push_mid_code(ASS_M, &mkup_name, 0, &var_name);
										this->print_mid_code_after_dag(mid);
									}
									else if (it != nullptr) {
										// �ֲ�����
										//�ҵ������ڱ���ƫ��
										mkup_offset = find_var_offset(var_name, &frr);
										// ��Ծ
										if (this->outflow[fun_idx][i - cur_fun_blocks->begin_block][mkup_offset]) {
											Midcode* mid;
											mid = this->push_mid_code(ASS_M, &mkup_name, 0, &var_name);
											this->print_mid_code_after_dag(mid);
										}
										else {
											Midcode* lastmid = &tmp_mid_list[lastline];
											mid_op op = lastmid->mid_op;
											if (op == BGJ_M || op == BGEJ_M || op == LSJ_M || op == LSEJ_M || op == EQJ_M || op == NEQJ_M) {
												if (*lastmid->src1 == var_name) {
													*lastmid->src1 = mkup_name;
												}
												if (*lastmid->src2 == var_name) {
													*lastmid->src2 = mkup_name;
												}
											}
										}
									}
								}
							}
						}
						/*set<string>::iterator iter;
						for (iter = mkup_node->name_candidates.begin();iter != mkup_node->name_candidates.end();iter++) {
							string var_name = *iter;
							// ��ʱ����
							if (var_name[0] == '#' && var_name != mkup_name) {
								//�ҵ������ڱ���ƫ��
								mkup_offset = find_var_offset(var_name, &frr);
								// ��Ծ
								if (this->outflow[fun_idx][i - cur_fun_blocks->begin_block][mkup_offset]) {
									Midcode* mid;
									mid = this->push_mid_code(ASS_M, &mkup_name, 0, &var_name);
									this->print_mid_code_after_dag(mid);
								}
								// ���ⲻ��Ծ���������Կ��ܻ�Ծ�����ǿ��ڿ��ܵĻ�Ծ�ص㶼����dagͼ�Ƶ�������������ֻʣ�¸û���������һ�������
								// ���һ��������ǱȽ���ת�����ȷ��һ����û��ʹ��
								else {
									Midcode* lastmid = &tmp_mid_list[lastline];
									mid_op op = lastmid->mid_op;
									if (op == BGJ_M || op == BGEJ_M || op == LSJ_M || op == LSEJ_M || op == EQJ_M || op == NEQJ_M) {
										if (*lastmid->src1 == var_name) {
											*lastmid->src1 = mkup_name;
										}
										if (*lastmid->src2 == var_name) {
											*lastmid->src2 = mkup_name;
										}
									}
								}
							}
							else if (isLetter(var_name) && var_name != mkup_name) {
								Item* it = this->get_const_var_item(var_name);
								if (it!=nullptr && it->global) {
									// ȫ�ֱ�����һ��Ҫ����
									Midcode* mid;
									mid = this->push_mid_code(ASS_M, &mkup_name, 0, &var_name);
									this->print_mid_code_after_dag(mid);
								}
								else if(it!=nullptr) {
									// �ֲ�����
									Midcode* lastmid = &tmp_mid_list[lastline];
									mid_op op = lastmid->mid_op;
									if (op == BGJ_M || op == BGEJ_M || op == LSJ_M || op == LSEJ_M || op == EQJ_M || op == NEQJ_M) {
										if (*lastmid->src1 == var_name) {
											*lastmid->src1 = mkup_name;
										}
										if (*lastmid->src2 == var_name) {
											*lastmid->src2 = mkup_name;
										}
									}
								}
							}
						}*/
					}

					// ��������ж����
					Midcode* lastmid = &tmp_mid_list[lastline];
					Midcode* middd;
					mid_op op = lastmid->mid_op;
					if (op == BGJ_M || op == BGEJ_M || op == LSJ_M || op == LSEJ_M || op == EQJ_M || op == NEQJ_M || op == J_M || op == CALL_M) {
						mid = this->push_mid_code(lastmid->mid_op, lastmid->src1, lastmid->src2, lastmid->dst);
						this->print_mid_code_after_dag(mid);
					}
				}
				// ������ܵ����һ���м����
				else {
					Midcode* lastmid = &tmp_mid_list[lastline];
					Midcode* middd;
					mid_op op = lastmid->mid_op;
					if (op == BGJ_M || op == BGEJ_M || op == LSJ_M || op == LSEJ_M || op == EQJ_M || op == NEQJ_M || op == J_M || op == CALL_M) {
						mid = this->push_mid_code(lastmid->mid_op, lastmid->src1, lastmid->src2, lastmid->dst);
						this->print_mid_code_after_dag(mid);
					}
				}
			}
		}
		// ����µ��м����
		//for (int i = 0;i < dag_node_num;i++) {
			// ��dag_nodes[i]��ʼ��ǰ��
			//if(dag_node_list[i])
		//}
	}
}

void Compiler::print_item_dot(string* name, string* lines) {
	DOT_OUT << "\t" << *name << " [label=\"" << *lines << "\", shape=box]" << endl;
}

void Compiler::print_link_dot(string* src, string* dst) {
	DOT_OUT << "\t" << *src << " -> " << *dst << endl;
}

void Compiler::peephole_optimization() {
	vector<Midcode>::iterator it;
	vector<Midcode>::iterator iter_i;
	vector<Midcode>::iterator iter_j;
	for (iter_i = mid_list.begin();iter_i != mid_list.end();++iter_i) {
		string target;	string new_iter_i_src1;  int tar_i;		bool label_flag = false;
		Midcode& mid = *(iter_i);
		switch (mid.mid_op) {
		case BGJ_M:
		case BGEJ_M:
		case LSJ_M:
		case LSEJ_M:
		case EQJ_M:
		case NEQJ_M:
			// ��֧������
			if (reg_is_const(mid.src1) && reg_is_const(mid.src2)) {
				int left = atoi(mid.src1->c_str());
				int right = atoi(mid.src2->c_str());
				// ������ת
				if ((mid.mid_op == BGJ_M && left <= right) || (mid.mid_op == BGEJ_M && left < right) || (mid.mid_op == LSJ_M && left >= right) \
					|| (mid.mid_op == LSEJ_M && left > right) || (mid.mid_op == EQJ_M && left != right) || (mid.mid_op == NEQJ_M && left == right)) {
					string* label = mid.dst;
					// ��ɾ�Լ��Ľ��
					iter_i = mid_list.erase(iter_i);
					iter_i--;
					// ��ɾ����label�Ľ��
					for (it = mid_list.begin();it != mid_list.end();it++) {
						if (it->mid_op == LABEL_M && *it->src1 == *label) {
							/*if ((it - mid_list.begin() <= i)) {
								--i;
							}*/
							mid_list.erase(it);
							break;
						}
					}
				}
				// ֻ����ת
				else {
					iter_i->mid_op = J_M;
					*iter_i->src1 = *iter_i->dst;
					*(iter_i->src2) = "";
					*(iter_i->dst) = "";
					iter_i--;
				}
			}
			// goto�ĵط���jָ�ֻ��if case retʱ��jָ����ڰ�ͬ�ⷵ�ظ�Ϊ��ʱ���غ󣬾Ͳ�������������
			/*it = iter_i;
			do {
				target = *it->src1;
				if (it == iter_i)
					target = *it->dst;
				for (iter_j = mid_list.begin();iter_j != mid_list.end();iter_j++) {
					if (iter_j->mid_op == LABEL_M && *iter_j->src1 == target) {
						it = iter_j;
						break;
					}
				}
				it += 1;
			} while (it->mid_op == J_M);
			*iter_i->src1 = target;*/
			break;
		case ADD_M:
			if (reg_is_const(mid.src1) || reg_is_const(mid.src2)) {
				int num = (reg_is_const(mid.src1)) ? (atoi(mid.src1->c_str())) : (atoi(mid.src2->c_str()));
				if (num == 0) {
					mid.mid_op = ASS_M;
					if (reg_is_const(mid.src1)) {
						if (*mid.dst == *mid.src2) {
							// #1 = 0 + #1��
							iter_i = mid_list.erase(iter_i);
							iter_i--;
						}
						else {
							*mid.src1 = *mid.src2;
							*mid.src2 = "";
						}
					}
					else {
						if (*mid.dst == *mid.src1) {
							// #1 = #1 + 0��
							iter_i = mid_list.erase(iter_i);
							iter_i--;
						}
						else {
							*mid.src2 = "";
						}
					}
				}
			}
			break;
		case SUB_M:
			if (reg_is_const(mid.src2)) {
				int num = atoi(mid.src2->c_str());
				if (num == 0) {
					mid.mid_op = ASS_M;
					if (*mid.dst == *mid.src1) {
						// #1 = #1 + 0��
						iter_i = mid_list.erase(iter_i);
						iter_i--;
					}
					else {
						*mid.src2 = "";
					}
				}
			}
			break;
		case MUL_M:
			if (reg_is_const(mid.src1) || reg_is_const(mid.src2)) {
				int num = (reg_is_const(mid.src1)) ? (atoi(mid.src1->c_str())) : (atoi(mid.src2->c_str()));
				if (num == 0) {
					mid.mid_op = ASS_M;
					if (reg_is_const(mid.src2))
						*mid.src1 = *mid.src2;
					*mid.src2 = "";
				}
				if (num == 1) {
					mid.mid_op = ASS_M;
					if (reg_is_const(mid.src1)) {
						if (*mid.dst == *mid.src2) {
							// #1 = #1 * 1��
							iter_i = mid_list.erase(iter_i);
							iter_i--;
						}
						else {
							*mid.src1 = *mid.src2;
							*mid.src2 = "";
						}
					}
					else {
						if (*mid.dst == *mid.src1) {
							// #1 = #1 + 0��
							iter_i = mid_list.erase(iter_i);
							iter_i--;
						}
						else {
							*mid.src2 = "";
						}
					}
				}
			}
			break;
		case DIV_M:
			if (reg_is_const(mid.src1)) {
				int num = atoi(mid.src1->c_str());
				if (num == 0) {
					mid.mid_op = ASS_M;
					*mid.src2 = "";
					break;
				}
			}
			if (reg_is_const(mid.src2)) {
				int num = atoi(mid.src2->c_str());
				if (num == 1) {
					mid.mid_op = ASS_M;
					if (*mid.dst == *mid.src1) {
						// #1 = #1 + 0��
						iter_i = mid_list.erase(iter_i);
						iter_i--;
					}
					else {
						*mid.src2 = "";
					}
				}
			}
			break;
		case J_M:
			// j�����j����ɾ��
			it = iter_i + 1;
			while (it->mid_op == J_M) {
				it = mid_list.erase(it);
			}
			// j�����������תlabel
			it = iter_i + 1;
			if (it->mid_op == LABEL_M && *it->src1 == *mid.src1) {
				iter_i = mid_list.erase(iter_i);
				//iter_i = mid_list.erase(iter_i);
				iter_i--;
				break;
			}
			// j��ת���ĵط��ĺ��滹��j
			it = iter_i;
			do {
				target = *it->src1;
				for (iter_j = mid_list.begin();iter_j != mid_list.end();iter_j++) {
					if (iter_j->mid_op == LABEL_M && *iter_j->src1 == target) {
						it = iter_j;
						break;
					}
				}
				/*if (tar_i >= mid_list.size())
					break;*/
				it += 1;
			} while (it->mid_op == J_M);
			/*if (tar_i < mid_list.size()) {
				*mid_list[i].src1 = target;
			}*/
			*iter_i->src1 = target;
			break;

		case LABEL_M:
			// ����LABEL�Ÿ���һ���������������һ��Ϊ�����������ú�������������
			it = iter_i + 1;
			while (it->mid_op == LABEL_M) {
				target = *it->src1;
				if (target.substr(target.size() - 4) == "_end") {
					new_iter_i_src1 = target;
					label_flag = true;
				}
				it += 1;
			}
			it = iter_i + 1;
			while (it->mid_op == LABEL_M) {
				target = *it->src1;
				for (iter_j = mid_list.begin();iter_j != mid_list.end();iter_j++) {
					if (iter_j->mid_op == J_M && *iter_j->src1 == target)
						*iter_j->src1 = *iter_i->src1;
				}
				it = mid_list.erase(it);
			}
			// �����ָ�Ϊ�µ�
			if (label_flag) {
				target = *iter_i->src1;
				for (iter_j = mid_list.begin();iter_j != mid_list.end();iter_j++) {
					if (iter_j->mid_op == J_M && *iter_j->src1 == target)
						*iter_j->src1 = new_iter_i_src1;
					if (iter_j->mid_op == LABEL_M && *iter_j->src1 == target)
						*iter_j->src1 = new_iter_i_src1;
					if ((iter_j->mid_op == BGJ_M || iter_j->mid_op == BGEJ_M || iter_j->mid_op == LSJ_M || iter_j->mid_op == LSEJ_M || 
						iter_j->mid_op == EQJ_M || iter_j->mid_op == NEQJ_M ) && *iter_j->dst == target)
						*iter_j->dst = new_iter_i_src1;
				}
			}
			break;
		}
	}
	// ���»�ȡ�м����
	if (!mid_sout.empty()) {
		mid_sout.clear();
	}
	for (int i = 0;i < mid_list.size();i++) {
		print_mid_code/*_after_p*/(&(mid_list[i]));
	}
}

int Compiler::find_var_offset(string name, funRegref* frr) {
	for (int i = 0;i < frr->size();i++) {
		if ((*frr)[i].name == name) {
			return i;
		}
	}
	return -1;
}

void Compiler::print_item_livedot(string* name, string* lines) {
	LIVE_DOT_OUT << "\t" << *name << " [label=\"" << *lines << "\"]" << endl;
}

void Compiler::print_link_livedot(string* src, string* dst) {
	LIVE_DOT_OUT << "\t" << *src << " -- " << *dst << endl;
}


// ȫ���������������Ժ�����Ϊ��λ
void Compiler::data_flow_analysis() {
	// in out use def ����ÿ������һ��
	// �ȱ���һ���ÿ�������� use �� def�����
	// exitʱû�н������һ����+1
	this->global_reg_num = 8;										// hyper-parameter
	int block_num = this->code_blocks_idx;
	bool*** use = new bool**[fun_tab.size()];
	this->def = new bool**[fun_tab.size()];
	bool*** in = new bool**[fun_tab.size()];
	bool*** out = new bool**[fun_tab.size()];
	this->outflow = new bool**[fun_tab.size()];
	// ��ʼ��
	// ��ͻͼ��ʼ��
	//::memset(cflt_idx, 0, sizeof(cflt_idx));
	// ������ʼ��
	this->cur_alloc_reg = new int*[fun_tab.size()];				// ֻ����ʱ�ģ����Ҫ�����ɴ������һ��
	for (int i = 0;i < fun_blocks_idx;i++) {
		int blocks_range = fun_blocks[i].end_block_line - fun_blocks[i].begin_block_line + 1;
		int block_begin_line = fun_blocks[i].begin_block_line;	int block_end_line = fun_blocks[i].end_block_line;
		int block_begin = fun_blocks[i].begin_block;	int block_end = fun_blocks[i].end_block;
		use[i] = new bool*[blocks_range];
		def[i] = new bool*[blocks_range];
		in[i] = new bool*[blocks_range];
		out[i] = new bool*[blocks_range];
		this->outflow[i] = new bool*[blocks_range];
		// ��ͻͼ��ʼ��
		for (int j = 0;j < MaxCflNodes;j++) {
			conflict_nodes[i][j].join_num = 0;
		}
		funRegref frr = reg_ref_tab.at(fun_blocks[i].name);
		this->cur_active_funtab = fun_tab.at(fun_blocks[i].name);
		cur_alloc_reg[i] = new int[frr.size()];
		for (int j = 0;j < frr.size();j++) {
			cur_alloc_reg[i][j] = -1;
		}
		for (int j = 0;j < block_end - block_begin + 1;j++) {
			use[i][j] = new bool[frr.size()];
			def[i][j] = new bool[frr.size()];
			in[i][j] = new bool[frr.size()];
			out[i][j] = new bool[frr.size()];
			this->outflow[i][j] = new bool[frr.size()];
			for (int k = 0;k < frr.size();k++) {
				use[i][j][k] = false;	def[i][j][k] = false;	in[i][j][k] = false;	out[i][j][k] = false;	this->outflow[i][j][k] = false;
			}
			// ���� use �� def
			block_begin_line = code_blocks[j + block_begin].first_line_num;
			block_end_line = code_blocks[j + block_begin].last_line_num;
			for (int m = block_begin_line;m <= block_end_line;m++) {
				Midcode mid = mid_list[m];
				switch (mid.mid_op) {
				case ADD_M:
				case SUB_M:
				case MUL_M:
				case DIV_M:
					// ������һ��Ҫ��Ҫ����Щд�ɺ���
					if (!reg_is_const(mid.src1)) {
						int idx = -1;
						// Ѱ��ƫ��
						idx = find_var_offset(*mid.src1, &frr);
						if (idx != -1 && def[i][j][idx] == false) {
							use[i][j][idx] = true;
						}
					}
					if (!reg_is_const(mid.src2)) {
						int idx = -1;
						// Ѱ��ƫ��
						idx = find_var_offset(*mid.src2, &frr);
						if (idx != -1 && def[i][j][idx] == false) {
							use[i][j][idx] = true;
						}
					}
					if (!reg_is_const(mid.dst)) {
						int idx = -1;
						// Ѱ��ƫ��
						idx = find_var_offset(*mid.dst, &frr);
						if (idx != -1 && use[i][j][idx] == false) {
							def[i][j][idx] = true;
						}
					}
					break;
				case BGJ_M:
				case BGEJ_M:
				case LSJ_M:
				case LSEJ_M:
				case EQJ_M:
				case NEQJ_M:
					if (!reg_is_const(mid.src1)) {
						int idx = -1;
						idx = find_var_offset(*mid.src1, &frr);
						if (idx != -1 && def[i][j][idx] == false) {
							use[i][j][idx] = true;
						}
					}
					if (!reg_is_const(mid.src2)) {
						int idx = -1;
						idx = find_var_offset(*mid.src2, &frr);
						if (idx != -1 && def[i][j][idx] == false) {
							use[i][j][idx] = true;
						}
					}
					break;

				case ASS_M:
					if (!reg_is_const(mid.src1)) {
						int idx = -1;
						idx = find_var_offset(*mid.src1, &frr);
						if (idx != -1 && def[i][j][idx] == false) {
							use[i][j][idx] = true;
						}
					}
					if (!reg_is_const(mid.dst)) {
						int idx = -1;
						idx = find_var_offset(*mid.dst, &frr);
						if (idx != -1 && use[i][j][idx] == false) {
							def[i][j][idx] = true;
						}
					}
					break;
				case SCANF_M:
					if (!reg_is_const(mid.src1)) {
						int idx = -1;
						idx = find_var_offset(*mid.src1, &frr);
						if (idx != -1 && def[i][j][idx] == false) {
							use[i][j][idx] = true;
						}
					}
					break;
				case PRINTV_M:
					if (!reg_is_const(mid.src2)) {
						int idx = -1;
						idx = find_var_offset(*mid.src2, &frr);
						if (idx != -1 && def[i][j][idx] == false) {
							use[i][j][idx] = true;
						}
					}
					break;

				case PUSH_M:
					if (!reg_is_const(mid.src1)) {
						int idx = -1;
						idx = find_var_offset(*mid.src1, &frr);
						if (idx != -1 && def[i][j][idx] == false) {
							use[i][j][idx] = true;
						}
					}
					break;
				case RETV_M:
					if (!reg_is_const(mid.src1)) {
						int idx = -1;
						idx = find_var_offset(*mid.src1, &frr);
						if (idx != -1 && def[i][j][idx] == false) {
							use[i][j][idx] = true;
						}
					}
					break;

				case ARGET_M:
					if (!reg_is_const(mid.src2)) {
						int idx = -1;
						idx = find_var_offset(*mid.src2, &frr);
						if (idx != -1 && def[i][j][idx] == false) {
							use[i][j][idx] = true;
						}
					}
					// ���dst��Ϊuse��������def����
					if (!reg_is_const(mid.dst)) {
						int idx = -1;
						idx = find_var_offset(*mid.dst, &frr);
						if (idx != -1 && def[i][j][idx] == false) {
							use[i][j][idx] = true;
						}
					}
					break;
				case ARTO_M:
					if (!reg_is_const(mid.src2)) {
						int idx = -1;
						idx = find_var_offset(*mid.src2, &frr);
						if (idx != -1 && def[i][j][idx] == false) {
							use[i][j][idx] = true;
						}
					}
					if (!reg_is_const(mid.dst)) {
						int idx = -1;
						idx = find_var_offset(*mid.dst, &frr);
						if (idx != -1 && use[i][j][idx] == false) {
							def[i][j][idx] = true;
						}
					}
					break;
				}
			}
		}
		// ���� in �� out��һ��flagΪ����ô˵��in��out�Ѿ����ٸ����ˣ������˳�
			//in[B] = use[B] + (out[B] - def[B]);
		bool flag = true;
		while (flag) {
			if (block_end - block_begin == 1) {
				break;
			}
			flag = false;
			for (int j = block_end - block_begin - 1;j >= 0;j--) {
				// Ѱ�Һ�̿�
				int follow_next = code_blocks[j + block_begin].follow_block;
				int j_next = code_blocks[j + block_begin].j_block;
				if (follow_next > 0) {
					int blk_idx = follow_next - block_begin;
					for (int k = 0;k < frr.size();k++) {
						out[i][j][k] |= in[i][blk_idx][k];
						this->outflow[i][j][k] = out[i][j][k];
					}
				}
				if (j_next >= 0) {
					int blk_idx = j_next - block_begin;
					for (int k = 0;k < frr.size();k++) {
						out[i][j][k] |= in[i][blk_idx][k];
						this->outflow[i][j][k] = out[i][j][k];
					}
				}
				
				// ���� in ��
				for (int k = 0;k < frr.size();k++) {
					bool res;
					res = use[i][j][k] || (out[i][j][k] && !def[i][j][k]);
					if (res != in[i][j][k]) {
						flag = true;
						in[i][j][k] = res;
					}
				}
			}
		}
		// output���һ��
		LIVE_OUT << fun_blocks[i].name << endl;
		for (int j = 0;j < block_end - block_begin + 1;j++) {
			LIVE_OUT << block_begin + j << ": " << endl;
			for (int k = 0;k < frr.size();k++) {
				LIVE_OUT << frr[k].name << endl;
				LIVE_OUT << "use: ";
				LIVE_OUT << use[i][j][k] << endl;
				LIVE_OUT << "def: ";
				LIVE_OUT << def[i][j][k] << endl;
				LIVE_OUT << "in: ";
				LIVE_OUT << in[i][j][k] << endl;
				LIVE_OUT << "out: ";
				LIVE_OUT << out[i][j][k] << endl;
			}
		}
		// ������ͻͼ
		// ֻҪͬһ��block�� def �� in �г��ֵı�����Ϊ��Ծ�ģ���ô�����߾ͳ�ͻ
		for (int j = 0;j < frr.size();j++) {
			// ��ʱ�����������ͻͼ����
			if (frr[j].name[0] == '#')
				continue;
			for (int k = j + 1;k < frr.size();k++) {
				// ��ʱ�����������ͻͼ����
				if (frr[k].name[0] == '#')
					continue;
				for (int blk_idx = 0;blk_idx < (block_end - block_begin + 1);blk_idx++) {
					// ȷ���ÿ��Ƿ�ɴ�
					Blocks* fb = &code_blocks[blk_idx + block_begin];
					mid_op first_op = mid_list[fb->first_line_num].mid_op;
					if (first_op != FUN_M && fb->pre_blocks_cnt <= 0) {
						continue;
					}
					// ȷ�����иú�����block���Ƿ��г�ͻ
					if ((def[i][blk_idx][k] || in[i][blk_idx][k]) && (def[i][blk_idx][j] || in[i][blk_idx][j])) {
						int num_j = conflict_nodes[i][j].join_num++;
						int num_k = conflict_nodes[i][k].join_num++;
						conflict_nodes[i][j].join[num_j] = k;
						conflict_nodes[i][k].join[num_k] = j;
						conflict_nodes[i][j].connect[k] = true;
						conflict_nodes[i][k].connect[j] = true;
						break;
					}
				}
			}
		}
		// ��ʼ���г�ͻɾ�������remove�Ļ�ֵΪ-1������ֵΪ�����ͻͼʱ������
		int* remove = new int[frr.size()];
		int node_in_graph = 0;
		// node_in_graphָʾʣ�໹�ڳ�ͻͼ�еı�����out_idxָʾ��
		for (int kk = 0;kk < frr.size();kk++) {
			if (frr[kk].name[0] != '#'&&frr[kk].global==false) {
				node_in_graph++;
			}
		}
		int out_idx = 0;
		// ��ʼ��ʹ��˳��
		for (int k = 0;k < frr.size();k++) {
			remove[k] = 0;
		}
		// ��ʼ����Ⱦɫͼ
		/*for (int j = 0;j < frr.size();j++) {
			// ���滹Ҫ��join_num�����ܾ���ôɾ�ˡ�����
			for (int k = 0;k < conflict_nodes[i][j].join_num;k++) {
				conflict_nodes[i][j].connect[conflict_nodes[i][j].join[k]] = true;
			}
		}*/
		while (node_in_graph > 1) {
			int round = 0;
			for (int j = 0;j%frr.size() < frr.size();j++) {
				// ѭ����ȥ��㣬ֱ��������ȥ�ļ�����ڱ�����
				if (j - round > frr.size())
					break;
				int jj = j % frr.size();
				if (frr[jj].global==false && frr[jj].name[0]!='#' && conflict_nodes[i][jj].join_num < this->global_reg_num && remove[jj] == 0) {
					// �˽ڵ���Ա����
					round = j;
					remove[jj] = ++out_idx;
					node_in_graph--;
					// �����ߵ����ӣ�������������������ͻͼ��ʱ����Ҫ��ע���������������������Ӷ���̫���ģ�����ֻ��num�ͺ�
					for (int k = 0;k < conflict_nodes[i][jj].join_num;k++) {
						int connect = conflict_nodes[i][jj].join[k];
						conflict_nodes[i][connect].join_num--;
					}
					if (node_in_graph == 1)
						break;
				}
			}
			if (node_in_graph == 1)
				break;
			// ��Ҫɾ�����
			for (int j = 0;j < frr.size();j++) {
				if (frr[j].global == false&&frr[j].name[0]!='#' && conflict_nodes[i][j].join_num >= this->global_reg_num && remove[j] == 0) {
					remove[j] = -1;
					node_in_graph--;
					for (int k = 0;k < frr.size();k++) {
						if (frr[j].global == true&&frr[k].name[0] == '#')
							continue;
						if (conflict_nodes[i][j].connect[k] == true) {
							conflict_nodes[i][k].join_num--;
						}
					}
					if (node_in_graph == 1)
						break;
				}
			}
		}

		// ����Ⱦɫͼ�Ļָ�������
		for (int j = 0;j < frr.size();j++) {
			if (frr[j].global == false&&frr[j].name[0]!='#' && remove[j] == 0) {
				// �ҵ���һ����㲢����
				cur_alloc_reg[i][j] = 0;
				/*this->regs[0].occupied = true;
				this->regs[0].name = frr[j].name;
				this->regs[0].global = (frr[j].global) ? (true) : (false);*/
			}
		}
		// �ɴ�С������������˳�򲢽��мĴ�������
		while (out_idx > 0) {
			for (int j = 0;j < frr.size();j++) {
				if (frr[j].global == false&&frr[j].name[0]!='#' && remove[j] == out_idx) {
					// �ҵ���һ����㲢����
					bool* reg_has_alloced = new bool[this->global_reg_num];
					for (int k = 0;k < this->global_reg_num;k++) {
						reg_has_alloced[k] = false;
					}
					for (int k = 0;k < frr.size();k++) {
						if (frr[j].global == false&&frr[k].name[0]!='#' && conflict_nodes[i][j].connect[k] && remove[k] >= 0 && cur_alloc_reg[i][k] >= 0) {
							reg_has_alloced[cur_alloc_reg[i][k]] = true;
						}
					}
					for (int k = 0;k < this->global_reg_num;k++) {
						if (reg_has_alloced[k] == false) {
							cur_alloc_reg[i][j] = k;
							break;
						}
					}
					delete[] reg_has_alloced;
				}
			}
			--out_idx;
		}
		// ��ͼ~
		LIVE_DOT_OUT << "graph live {" << endl;
		for (int j = 0;j < frr.size();j++) {
			string lines = "";
			lines += frr[j].name + "\\n";
			lines += "REG: " + *(int2string(new string(), cur_alloc_reg[i][j]));
			this->print_item_livedot(int2string(new string(), j), &lines);
		}
		for (int j = 0;j < frr.size();j++) {
			for (int k = j + 1;k < frr.size();k++) {
				if (conflict_nodes[i][j].connect[k]) {
					this->print_link_livedot(int2string(new string(), j), int2string(new string(), k));
				}
			}
		}

		LIVE_DOT_OUT << "}" << endl;

		delete[] remove;
	}
	// delete�ͷ��ڴ棡
	for (int i = 0;i < fun_blocks_idx;i++) {
		int blocks_range = fun_blocks[i].end_block_line - fun_blocks[i].begin_block_line + 1;
		int block_begin_line = fun_blocks[i].begin_block_line;	int block_end_line = fun_blocks[i].end_block_line;
		int block_begin = fun_blocks[i].begin_block;	int block_end = fun_blocks[i].end_block;

		funRegref frr = reg_ref_tab[fun_blocks[i].name];
		for (int j = 0;j < block_end - block_begin + 1;j++) {
			delete[] use[i][j];
			//delete[] def[i][j];
			delete[] in[i][j];
			delete[] out[i][j];
		}
		delete[] use[i];
		//delete[] def[i];
		delete[] in[i];
		delete[] out[i];
		//delete[] cur_alloc_reg[i];
	}
	delete[] use;
	//delete[] def;
	delete[] in;
	delete[] out;
	//delete[] cur_alloc_reg;
}

// ���ü���������Ĵ�����init��
void Compiler::reg_alloc_init() {

}

// �ҵ����������Ƿ���ñ��������ȫ�ֱ�����ֲ������Լ���ʱ���������ֲ����룻�������������
void Compiler::var_count_alloc(string name) {
	// ���ȿ��Ƿ��Ƿ��ر�����������
	if (/*name[0] == '#' || */name[0] == '%' || (name[0] >= '0' && name[0] <= '9') || name[0] == '-')
		return;
	for (int i = 0;i < cur_fun_reg_ref->size();i++) {
		varRegref* var_reg_ref = &(*cur_fun_reg_ref)[i];
		if (var_reg_ref->name == name) {
			var_reg_ref->ref_count++;
			return;
		}
	}
	// �½���ȫ�ֱ�����ע��
	Item* it = get_const_var_item(name);
	varRegref* var_reg_ref_init = new varRegref();
	var_reg_ref_init->name = name;
	var_reg_ref_init->ref_count = 1;
	var_reg_ref_init->global = (it->global == true) ? (true) : (false);
	cur_fun_reg_ref->push_back(*var_reg_ref_init);
}

// �������ü�������ĺ���
static bool ref_count_cmp(const varRegref& a1, const varRegref& a2) {
	return a1.ref_count < a2.ref_count;
}

void Compiler::ref_count_begin(int begin, int end) {
	for (int i = begin;i < end;++i) {
		Midcode mid = mid_list[i];
		bool flag = false;
		FuncItem* item_f;	funRegref* frr;
		switch (mid.mid_op) {
		case ADD_M:
		case SUB_M:
		case MUL_M:
		case DIV_M:
			var_count_alloc(*mid.src1);
			var_count_alloc(*mid.src2);
			var_count_alloc(*mid.dst);
			break;
		case BGJ_M:
		case BGEJ_M:
		case LSJ_M:
		case LSEJ_M:
		case EQJ_M:
		case NEQJ_M:
			// �ж��Ƿ���do_while
			for (int ii = begin;ii < end;ii++) {
				if (mid_list[ii].mid_op == LABEL_M && *mid_list[ii].src1 == *mid.dst && ii < i) {
					flag = true;
					for (int j = 0;j < 10;j++) {
						ref_count_begin(ii, i);
					}
				}
			}
			if (!flag) {
				var_count_alloc(*mid.src1);
				var_count_alloc(*mid.src2);
			}
			else {
				for (int j = 0;j < 10;j++) {
					var_count_alloc(*mid.src1);
					var_count_alloc(*mid.src2);
				}
			}
			break;
		case FUN_M:
			item_f = this->get_func_item(*mid.src1);
			this->cur_active_funtab = item_f;
			frr = new funRegref();
			reg_ref_tab.insert({ *mid.src1,*frr });
			this->cur_fun_reg_ref = &(reg_ref_tab.at(*mid.src1));
			// �Ѳ��������Ҽӽ�ȥ...
			for (int j = 0;j < cur_active_funtab->paranum;j++) {
				string name = cur_active_funtab->paraList[j].getName();
				this->var_count_alloc(name);
			}
			break;

		case ASS_M:
			var_count_alloc(*mid.src1);
			var_count_alloc(*mid.dst);
			break;
		case SCANF_M:
			var_count_alloc(*mid.src1);
			break;
		case PRINTV_M:
			var_count_alloc(*mid.src2);
			break;

		case PUSH_M:
			var_count_alloc(*mid.src1);
			break;
		case RETV_M:
			var_count_alloc(*mid.src1);
			break;

		case ARGET_M:
			var_count_alloc(*mid.src2);
			var_count_alloc(*mid.dst);
			break;
		case ARTO_M:
			var_count_alloc(*mid.src2);
			var_count_alloc(*mid.dst);
			break;

		default:
			break;
		}
	}
}

// ���ü�����ȫ�ּĴ�������
void Compiler::ref_count_regalloc() {
	// ���ҵ�����������ͷ��֮�����м�����������ͳ�Ƹ����������������
	// ��ֵ����ߺ��ұߵ������Ϊ������������
	// printf��scanf�Լ�push����������
	// ADD, SUB, MUL, DIV, 6*��ת, assign, SCANF, PRINTV, PUSH, ARGET�Ҳ�, ARGTO��ࣿ
	// RegfunTab -> funRegref -> varRegref
	reg_alloc_init();
	// �ĳɺ���ref_count_begin(int begin, int end, midlist)
	ref_count_begin(0, mid_list.size());
	// ����
	for (RegrefTab::iterator iter = reg_ref_tab.begin();iter != reg_ref_tab.end();iter++) {
		funRegref fr = iter->second;
		sort(fr.begin(), fr.end(), ref_count_cmp);
	}
	// ���
	for (RegrefTab::iterator iter = reg_ref_tab.begin();iter != reg_ref_tab.end();iter++) {
		REF_COUNT_OUT << iter->first << endl;
		funRegref fr = iter->second;
		for (int j = 0;j < fr.size();j++) {
			REF_COUNT_OUT << fr[j].name << " " << fr[j].ref_count << endl;
		}
		REF_COUNT_OUT << endl;
	}
}

// ��������������
void Compiler::const_propagation() {
	// ���Ȼ���м�����mid_list��
	// Ȼ���û�����code_blocks�����ݣ�ÿ���������ڲ����г�����������
}
