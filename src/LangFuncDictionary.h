#pragma once
#include "dictionary.h"
#include "controlflow.h"

class LangFuncDictionary {
private:
	Dictionary* func_dict;

public:
	LangFuncDictionary();

	~LangFuncDictionary();

	void insert(struct Function* key, const char* value);

	void remove(struct Function* key);

	const char* lookup(const char* key);

	const char** p_lookup(const char* key);

	struct Function* get(const char * key);

	struct Function** p_get(const char* key);

	int size();

	void print();
};
