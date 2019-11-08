#pragma once
#include "Compiler.h"

enum Genre { PARA, CONST, VAR, PARA_FUNC, NO_PARA_FUNC };

class Item;
class FuncItem;


class Item {
private:
	std::string name;			// �������������
	Genre genre;				// ���������г����������������� �Ժ���Ϊ�вκ����Լ��޲κ���
	tokenType kind;				// ����������int/char,void�����޷����㣬�Ժ���Ϊ��������ֵvoid/int/char
	int value;					// �Գ�������ֵ
	int length;					// �Ա��������������
	
public:
	int offset;					// ��ȫ�ֱ����볣���������gpƫ�ƣ��Ժ����ڲ��������Ե�ǰ����fp��ƫ��
	int regNum;					// ȫ��/�ֲ�/��ʱ������ŵļĴ���
	bool global;				// ָʾ�Ƿ�Ϊȫ�ֱ������������ɴ���ʱʹ��
	
	Item(std::string name, Genre genre, tokenType kind, int value = 0, int length = 0);
	std::string getName();
	Genre getGenre();
	tokenType getKind();
	int getValue();
	int getLength();
	void setName(std::string s);
	void setGenre(Genre genre);
	void setKind(tokenType kind);
	void setValue(int value);
	void setLength(int length);
};

class FuncItem : public Item {
private:
	std::map<std::string, Item*>constMap;	// ����������
	std::vector<Item>paraList;				// ����������
	std::map<std::string, Item*>varMap;		// ����������
	int paranum;							// ������������
	

public:
	FuncItem(std::string name, Genre genre, tokenType kind, int paranum = 0);
	std::set<int> used_regs;				// ��ǰ��������Ĵ������
	Item* get_const(std::string name);
	Item* get_var(std::string name);
	Item* get_para(std::string name);
	friend class Compiler;					// ����compilerΪ��Ԫ�Ա��ȡ
	
};




