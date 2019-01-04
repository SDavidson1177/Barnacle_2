#pragma once
#include "controlflow.h"
#include "languageio.h"
#include <vector>
#include <map>

using namespace std;

void parse(vector<Token*>* tokens, vector <map<string, pair<Variable*, const char*>>>* scope=nullptr);