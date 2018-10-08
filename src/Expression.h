#pragma once
#include "Token.h"
#include "FunctionDictionary.h"
#include <string.h>

using namespace std;

void initialize_operations();

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

	void print();
};

class Binary : public Expr {
public:
	Binary(Expr* left, Token* op, Expr* right, string type = "");
	~Binary();
};

class Literal : public Expr {
public:
	Literal(string value, string type = "");
	~Literal();
};



