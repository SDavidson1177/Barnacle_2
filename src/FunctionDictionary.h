#pragma once
#include "dictionary.h"

class FunctionDictionary {
private:
	Dictionary* fnc_dict;

public:
	FunctionDictionary();

	~FunctionDictionary();

	void insert(const char* key, void* value);

	void remove(const char* key);

	void* lookup(const char* key);

	void** p_lookup(const char* key);

	const char** p_get(const char* key);

	int size();

	void print();
};
