#pragma once
#include "dictionary.h"
#include "Variable.h"

class VarDictionary {
private:
	Dictionary* var_dict;

public:
	VarDictionary();

	VarDictionary(Variable* key, const char* value);

	~VarDictionary();

	void insert(Variable* key, const char* value);

	void remove(Variable* key);

	const char* lookup(const char* key);

	const char** p_lookup(const char* key);

	Variable* get(const char * key);

	Variable** p_get(const char* key);

	int size();

	void print();
};
