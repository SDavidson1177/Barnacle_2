#pragma once
#include "dictionary.h"

class StringDictionary {
private:
	Dictionary* str_dict;

public:
	StringDictionary();

	~StringDictionary();

	void insert(const char* key, const char* value);

	void remove(const char* key);

	const char* lookup(const char* key);

	int size();

	void print();
};