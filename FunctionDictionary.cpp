#include "FunctionDictionary.h"
#include <string.h>
#include <iostream>

using namespace std;

static int voidStrCmp(void* s1, void* s2) {
	const char* temp1 = (const char*)s1;
	const char* temp2 = (const char*)s2;
	return strcmp(temp1, temp2);
}

bool printFncDictKey(void* k) {
	char* temp = (char*)k;
	cout << temp;
	return true;
}

bool printFncDictValue(void* v) {
	// char* temp = (char*)v;
	cout << "A function at " << v;
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

FunctionDictionary::FunctionDictionary() {
	this->fnc_dict = new Dictionary(voidStrCmp, destroyVarDictKey, destroyVarDictValue,
		printFncDictKey, printFncDictValue);
}

FunctionDictionary::~FunctionDictionary() {
	delete this->fnc_dict;
}

void FunctionDictionary::insert(const char* key, void* value) {
	this->fnc_dict->addNode((void*)key, (void*)value);
}

void FunctionDictionary::remove(const char* key) {
	this->fnc_dict->removeNode((void*)key);
}

void* FunctionDictionary::lookup(const char* key) {
	return this->fnc_dict->lookup((void*)key);
}

void** FunctionDictionary::p_lookup(const char* key) {
	return this->fnc_dict->p_lookup((void*)key);
}

const char** FunctionDictionary::p_get(const char* key) {
	return (const char**)this->fnc_dict->p_get((void*)key);
}


int FunctionDictionary::size() {
	return this->fnc_dict->size();
}

void FunctionDictionary::print() {
	this->fnc_dict->print(INORDER);
}