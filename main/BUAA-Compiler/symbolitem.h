#pragma once
#include "Compiler.h"

enum Genre { PARA, CONST, VAR, PARA_FUNC, NO_PARA_FUNC };

class Item;
class FuncItem;


class Item {
private:
	std::string name;			// 常量或变量名称
	Genre genre;				// 变量种类有常量，变量，参数， 对函数为有参函数以及无参函数
	tokenType kind;				// 变量类型有int/char,void代表无法计算，对函数为函数返回值void/int/char
	int value;					// 对常量代表值
	int length;					// 对变量代表变量长度
	
public:
	int offset;					// 对全局变量与常量代表相对gp偏移；对函数内部则代表相对当前函数fp的偏移
	int regNum;					// 全局/局部/临时变量存放的寄存器
	bool global;				// 指示是否为全局变量，便于生成代码时使用
	
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
	std::map<std::string, Item*>constMap;	// 函数常量表
	std::vector<Item>paraList;				// 函数参数表
	std::map<std::string, Item*>varMap;		// 函数变量表
	int paranum;							// 函数参数个数
	

public:
	FuncItem(std::string name, Genre genre, tokenType kind, int paranum = 0);
	std::set<int> used_regs;				// 当前函数分配寄存器情况
	Item* get_const(std::string name);
	Item* get_var(std::string name);
	Item* get_para(std::string name);
	friend class Compiler;					// 设置compiler为友元以便读取
	
};




