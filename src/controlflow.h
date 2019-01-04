#pragma once
#include "Expression.h"
#include "VarDictionary.h"
#include "LangFuncDictionary.h"
#include <vector>

// If Statements

struct IfStatement {
	struct IfNode* start;
};

struct IfNode {
	Expr* condition;
	struct IfNode* next;
	vector <map<string, pair<Variable*, const char*>>> scope;
	vector <Token*> tokens;
};

struct IfStatement* initIfStatement();

struct IfNode* initIfNode(Expr* cond, vector <map<string, pair<Variable*, const char*>>>* inher_scope = nullptr);

int addIfNode(struct IfStatement** st, Expr* cond, std::vector<Token*>* tokens, int start, int end, vector <map<string, pair<Variable*, const char*>>>* inher_scope = nullptr);

struct IfNode* evaluateIfStatement(struct IfStatement* st);

void appendScope(struct IfNode* st, VarDictionary** appending_scope);

template <class control_flow>
void extendScope(control_flow st, vector <map<string, pair<Variable*, const char*>>>* extending_scope);

void destroyIfStatement(struct IfStatement* st);

void destroyIfNode(struct IfNode* nd);

// While Loops

struct WhileLoop {
	Expr* condition;
	vector <map<string, pair<Variable*, const char*>>> scope;
	vector <Token*> tokens;
};

struct WhileLoop* initWhileLoop(Expr* cond, vector<Token*>* tokens, int start, int end, vector <map<string, pair<Variable*, const char*>>>* inher_scope);

void destroyWhileLoop(struct WhileLoop* wl);

// Functions

struct Function {
	int numOfArgs;
	string ID;
	vector<string> argv;
	vector <map<string, pair<Variable*, const char*>>> scope;
	vector <Token*> tokens;
	std::string return_value;
};

struct Function* initFunction(string given_ID, vector<string>*argv, vector<Token*>* tokens, int start, int end, vector <map<string, pair<Variable*, const char*>>>* inher_scope);

void printFunction(struct Function* f);

void setArgument(struct Function** f, int index, std::string value, std::string type);

void destroyFunction(struct Function* f);