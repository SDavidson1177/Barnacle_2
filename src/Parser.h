#pragma once
#include "controlflow.h"
#include "languageio.h"
#include <vector>

using namespace std;

void parse(vector<Token*>* tokens, vector <VarDictionary*>* scope=nullptr);