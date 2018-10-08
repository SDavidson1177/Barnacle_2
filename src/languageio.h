#pragma once
#include <vector>
#include <fstream>
#include "Token.h"
#include "StringDictionary.h"
#include "VarDictionary.h"

using namespace std;

void initInterpreter();

void readTokens(const char* file, vector <Token*>* a);

/*strToNum(str, dest) converts str to a number which is stored either as
an integer or a float in dest. Returns f for float and i for int.
requires: str to be valid
dest not to be malloc'd
client is responsible to free dest*/
char strToNum(const char* str, void** s);

bool isNaN(const char* str);

extern StringDictionary compDictionary;
extern StringDictionary logicDictionary;
