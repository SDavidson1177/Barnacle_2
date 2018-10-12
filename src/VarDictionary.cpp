#include "VarDictionary.h"
#include "Variable.h"
#include <string.h>
#include <iostream>

using namespace std;

static int voidStrCmp(void* s1, void* s2) {
	Variable* temp1 = (Variable*)s1;
	Variable* temp2 = (Variable*)s2;
	return strcmp(temp1->getKey().c_str(), temp2->getKey().c_str());
}

bool printVarDictKey(void* k) {
	Variable* temp = (Variable*)k;
	temp->print();
	return true;
}

bool printVarDictValue(void* v) {
	char* temp = (char*)v;
	cout << temp;
	return true;
}

static bool destroyVarDictKey(void* k) {
	// Does Nothing
	return true;
}

static bool destroyVarDictValue(void* v) {
	// Does Nothing
	return true;
}

VarDictionary::VarDictionary() {
	this->var_dict = new Dictionary(voidStrCmp, destroyVarDictKey, destroyVarDictValue,
		printVarDictKey, printVarDictValue);
}

VarDictionary::~VarDictionary() {
	delete this->var_dict;
}

void VarDictionary::insert(Variable* key, const char* value) {
	this->var_dict->addNode((void*)key, (void*)value);
}

void VarDictionary::remove(Variable* key) {
	this->var_dict->removeNode((void*)key);
}

const char* VarDictionary::lookup(const char* key) {
	Variable* temp_dict = new Variable(key, "NONE");
	const char* retval = (const char*)this->var_dict->lookup((void*)temp_dict);
	delete temp_dict;
	return retval;
}

const char** VarDictionary::p_lookup(const char* key) {
	Variable* temp_dict = new Variable(key, "NONE");
	const char** retval = (const char**)this->var_dict->p_lookup((void*)temp_dict);
	delete temp_dict;
	return retval;
}

Variable** VarDictionary::p_get(const char* key) {
	Variable temp_dict = Variable(key, "NONE");
	Variable** retval = (Variable**)this->var_dict->p_get((void*)&temp_dict);
	return retval;
}

Variable* VarDictionary::get(const char* key) {
	Variable temp_dict = Variable(key, "NONE");
	Variable* retval = (Variable*)this->var_dict->get((void*)&temp_dict);
	return retval;
}

int VarDictionary::size() {
	return this->var_dict->size();
}

void VarDictionary::print() {
	this->var_dict->print(INORDER);
}