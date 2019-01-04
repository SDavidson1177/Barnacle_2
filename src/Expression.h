#pragma once
#include "Token.h"
#include "FunctionDictionary.h"
#include <string.h>
#include <vector>
#include <map>

using namespace std;

void initialize_operations();

class Function;
class Variable;

class Expr {
protected:
	string name;
	string type;
public:
	string value;

	Expr* left;
	Token* op;
	Expr* right;

	bool isVar() {
		return (!strcmp(name.c_str(), ""));
	}

	string getType() {
		return type;
	}

	void setType(const char* new_type) {
		type = new_type;
	}

	string evaluate();

	virtual std::string getValue();

	void print();
};

class Binary : public Expr {
public:
	Binary(Expr* left, Token* op, Expr* right, string type = "");
	~Binary();

	std::string getValue() override;
};

class VarDictionary;

class Literal : public Expr {
	struct Value {
		struct FunctionConditions {
			vector <Token*>* tokens;
			vector <map<string, pair<Variable*, const char*>>>* scope;
			int *index;
			const int tokensSize;
		};
		std::string val; // if we are not dealing with a function
		FunctionConditions* func;
		Function* (*callFunction)(vector<Token*>*, vector <map<string, pair<Variable*, const char*>>>*, int*, const int);

		Value(std::string p_value);
		Value(vector<Token*>* tokens, vector <map<string, pair<Variable*, const char*>>>* scope, int* index, const int tokensSize);
		~Value();
		
		std::string getValue(); // returns the value of either the function or the string
	};

	Value literal_value;
	static Function* (*function_parser)(vector<Token*>*, vector <map<string, pair<Variable*, const char*>>>*, int*, const int);
public:
	Literal(string value, string type = "");
	Literal(vector<Token*>* tokens, vector <map<string, pair<Variable*, const char*>>>*, int *index, const int tokensSize, string type = "");
	~Literal();

	std::string getValue() override;
};



