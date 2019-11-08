#pragma once
#include "Compiler.h"

const int MaxCodeBlocks = 400;
const int LHY = -16;
const int MaxFunNum = 100;
const int MaxCflNodes = 100;
const int MaxDagNodes = 500;

typedef struct {
	std::string name;				// ��ǰ������
	std::string j_name;				// Ŀ�������
	bool is_print;					// ��Ҫ��ӡ��
	int idx;						// ��ǰ����������
	int first_line_num;				// �������һ�д���
	int last_line_num;				// ���������һ�д���
	int pre_blocks[MaxCodeBlocks];	// ǰ��ָ�򱾿��block�����
	int pre_blocks_cnt;				// ǰ��ָ�򱾿��count
	int follow_block;				// ֱ�Ӻ�̿�����
	int j_block;					// ��ת��̿�����
}Blocks;


typedef struct {
	int begin_block_line;
	int end_block_line;
	int begin_block;
	int end_block;
	std::string name;
}funBlocks;

// ���ü��������ݽṹ
typedef struct {
	std::string name;
	int ref_count;
	bool global;
}varRegref;

typedef std::vector<varRegref> funRegref;
typedef std::map<std::string, funRegref> RegrefTab;

// ��ͻͼ
typedef struct {
	int join[1000];
	int join_num;
	bool connect[1000];
}conflictGraph;

// DAG�ڵ��
typedef struct {
	std::string name;			// �������			
	int idx;					// dagͼ������
	bool modified;				// ��ʾ�ý���Ӧ�ķ����Ƿ����¶����
}dagNodelist;

typedef struct {
	std::string name;					// �������			
	std::string leaf_ass_name;
	mid_op op;							// ��������
	int idx;							// �����dagͼ��λ��
	int value;							// �жϸ�����ֵ�ɷ�ȷ��,���ǲ�������������idx
	int child_1;
	int child_2;
	int prt_num;
	std::set<std::string> name_candidates;// ��ʷ����������ּ�

	bool isLeaf;						// �Ƿ���Ҷ�ڵ�
}dagNode;
