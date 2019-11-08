#pragma once
#include "Compiler.h"

std::string* Compiler::int2string(std::string *s, int value)
{
	std::stringstream ss = std::stringstream();
	ss << value;
	*s = ss.str();
	return s;
}


Item::Item(string name, Genre genre, tokenType kind, int value, int length)
{
	this->name = name;
	this->genre = genre;
	this->kind = kind;
	this->value = value;
	this->length = length;
	this->offset = 0;
	this->regNum = -1;
	this->global = false;
}

std::string Item::getName() {
	return this->name;
}

Genre Item::getGenre() {
	return this->genre;
}

tokenType Item::getKind() {
	return this->kind;
}

int Item::getValue() {
	return this->value;
}

int Item::getLength() {
	return this->length;
}

void Item::setName(std::string name) {
	this->name = name;
}

void Item::setGenre(Genre genre) {
	this->genre = genre;
}

void Item::setKind(tokenType kind) {
	this->kind = kind;
}

void Item::setValue(int value) {
	this->value = value;
}

void Item::setLength(int length) {
	this->length = length;
}

FuncItem::FuncItem(string name, Genre genre, tokenType kind, int paranum) : Item(name,genre,kind,0,0)
{
	this->paranum = paranum;
	this->offset = 0;
}

Item * FuncItem::get_const(std::string name)
{
	std::map<std::string, Item*>::iterator it;
	it = constMap.find(name);
	if (it != constMap.end())
		return it->second;
	return nullptr;
}

Item * FuncItem::get_var(std::string name)
{
	std::map<std::string, Item*>::iterator it;
	it = varMap.find(name);
	if (it != varMap.end())
		return it->second;
	return nullptr;
}

Item * FuncItem::get_para(std::string name)
{
	std::vector<Item>::iterator it;
	for (it = paraList.begin();it != paraList.end();it++) {
		if (it->getName() == name)
			return &(*it);
	}
	return nullptr;
}
