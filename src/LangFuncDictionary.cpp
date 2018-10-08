#include "LangFuncDictionary.h"
#include <string.h>
#include <iostream>

using namespace std;

static int LangFuncCmp(void* s1, void* s2) {
	struct Function* temp1 = (struct Function*)s1;
	struct Function* temp2 = (struct Function*)s2;
	return strcmp(temp1->ID.c_str(), temp2->ID.c_str());
}

bool printLangFuncDictKey(void* k) {
	struct Function* temp = (struct Function*)k;
	printFunction(temp);
	return true;
}

bool printLangFuncDictValue(void* v) {
	char* temp = (char*)v;
	cout << temp;
	return true;
}

static bool destroyLangFuncDictKey(void* k) {
	// Does Nothing
	return true;
}

static bool destroyLangFuncDictValue(void* v) {
	// Does Nothing
	return true;
}

LangFuncDictionary::LangFuncDictionary() {
	this->func_dict = new Dictionary(LangFuncCmp, destroyLangFuncDictKey, destroyLangFuncDictValue,
		printLangFuncDictKey, printLangFuncDictValue);
}

LangFuncDictionary::~LangFuncDictionary() {
	delete this->func_dict;
}

void LangFuncDictionary::insert(struct Function* key, const char* value) {
	this->func_dict->addNode((void*)key, (void*)value);
}

void LangFuncDictionary::remove(struct Function* key) {
	this->func_dict->removeNode((void*)key);
}

const char* LangFuncDictionary::lookup(const char* key) {
	struct Function* temp_dict = initFunction(key, nullptr,nullptr, 0, 0, nullptr);
	const char* retval = (const char*)this->func_dict->lookup((void*)temp_dict);
	delete temp_dict;
	return retval;
}

const char** LangFuncDictionary::p_lookup(const char* key) {
	struct Function* temp_dict = initFunction(key, nullptr, nullptr, 0, 0, nullptr);
	const char** retval = (const char**)this->func_dict->p_lookup((void*)temp_dict);
	delete temp_dict;
	return retval;
}

struct Function** LangFuncDictionary::p_get(const char* key) {
	struct Function* temp_dict = initFunction(key, nullptr, nullptr, 0, 0, nullptr);
	struct Function** retval = (struct Function**)this->func_dict->p_get((void*)temp_dict);
	return retval;
}

struct Function* LangFuncDictionary::get(const char* key) {
	struct Function* temp_dict = initFunction(key, nullptr, nullptr, 0, 0, nullptr);
	struct Function* retval = (struct Function*)this->func_dict->get((void*)temp_dict);
	return retval;
}

int LangFuncDictionary::size() {
	return this->func_dict->size();
}

void LangFuncDictionary::print() {
	this->func_dict->print(INORDER);
}
