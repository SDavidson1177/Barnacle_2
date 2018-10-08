#include "StringDictionary.h"
#include <iostream>
#include <string>

using namespace std;

static int voidStrCmp(void* s1, void* s2) {
	const char* temp1 = (const char*)s1;
	const char* temp2 = (const char*)s2;
	return strcmp(temp1, temp2);
}

static bool destroyStrDictKey(void* k) {
	// Does Nothing
	return true;
}

static bool destroyStrDictValue(void* v) {
	// Does Nothing
	return true;
}

bool printStrDictKey(void* k){
	char* temp = (char*)k;
	cout << temp;
	return true;
}

bool printStrDictValue(void* v) {
	char* temp = (char*)v;
	cout << temp;
	return true;
}

StringDictionary::StringDictionary() {
	this->str_dict = new Dictionary(voidStrCmp, destroyStrDictKey, destroyStrDictValue, 
		printStrDictKey, printStrDictValue);
}

StringDictionary::~StringDictionary() {
	delete this->str_dict;
}

void StringDictionary::insert(const char* key, const char* value) {
	this->str_dict->addNode((void*)key, (void*)value);
}

void StringDictionary::remove(const char* key) {
	this->str_dict->removeNode((void*)key);
}

const char* StringDictionary::lookup(const char* key) {
	return (const char*)this->str_dict->lookup((void*)key);
}

int StringDictionary::size() {
	return this->str_dict->size();
}

void StringDictionary::print() {
	this->str_dict->print(INORDER);
}
