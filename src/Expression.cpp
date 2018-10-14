#include "Expression.h"
#include "languageio.h"
#include <iostream>

using namespace std;

FunctionDictionary operation_functions = FunctionDictionary();

// Functions for all comparison and arithmetic operations
static string add(Expr* leftSide, Expr* rightSide, Expr* parent) {
	leftSide->evaluate();
	rightSide->evaluate();
	if (!strcmp(leftSide->getType().c_str(), rightSide->getType().c_str())) {
		if (!strcmp(leftSide->getType().c_str(), "NUMBER")) {
			parent->setType("NUMBER");
			void* resLeft;
			void* resRight;
			char typeL = strToNum(leftSide->value.c_str(), &resLeft);
			char typeR = strToNum(rightSide->value.c_str(), &resRight);
			int* leftInt = nullptr;
			int* rightInt = nullptr;
			float* leftFloat = nullptr;
			float* rightFloat = nullptr;

			if (typeL == 'f') {
				leftFloat = (float*)resLeft;
			}
			else if (typeL == 'i') {
				leftInt = (int*)resLeft;
			}
			if (typeR == 'f') {
				rightFloat = (float*)resRight;
			}
			else if (typeR == 'i') {
				rightInt = (int*)resRight;
			}

			if (rightFloat == nullptr && leftFloat == nullptr) {
				return to_string(*leftInt + *rightInt);
			}
			else if (rightFloat == nullptr && leftInt == nullptr) {
				return to_string((int)(*leftFloat) + *rightInt);
			}
			else if (rightInt == nullptr && leftInt == nullptr) {
				return to_string(*leftFloat + *rightFloat);
			}
			else if (rightInt == nullptr && leftFloat == nullptr) {
				return to_string(*leftInt + (int)(*rightFloat));
			}
		}
		else if (!strcmp(leftSide->getType().c_str(), "STRING")) {
			parent->setType("STRING");
			int length = strlen(leftSide->value.c_str()) + strlen(rightSide->value.c_str()) + 1;
			char* new_value = (char*)malloc(sizeof(char)*length);
			strcpy(new_value, leftSide->value.c_str());
			strcat(new_value, rightSide->value.c_str());
			return new_value;
		}
	}
	else if ((!strcmp(leftSide->getType().c_str(), "STRING") && !strcmp(rightSide->getType().c_str(), "NUMBER"))) {
		parent->setType("STRING");
		int length = strlen(leftSide->value.c_str()) + strlen(rightSide->value.c_str()) + 1;
		char* new_value = (char*)malloc(sizeof(char)*length);
		strcpy(new_value, leftSide->value.c_str());
		strcat(new_value, rightSide->value.c_str());
		return new_value;
	}
	else if ((!strcmp(leftSide->getType().c_str(), "NUMBER") && !strcmp(rightSide->getType().c_str(), "STRING"))) {
		parent->setType("STRING");
		int length = strlen(leftSide->value.c_str()) + strlen(rightSide->value.c_str()) + 1;
		char* new_value = (char*)malloc(sizeof(char)*length);
		strcpy(new_value, leftSide->value.c_str());
		strcat(new_value, rightSide->value.c_str());
		return new_value;
	}
	else {
		cout << "Type missmatch on evaluation of expression. Got " << leftSide->getType().c_str() << " and " << rightSide->getType().c_str() << "\n";
	}
	return nullptr;
}

static string subtract(Expr* leftSide, Expr* rightSide, Expr* parent) {
	parent->setType("NUMBER");
	leftSide->evaluate();
	rightSide->evaluate();
	if (!strcmp(leftSide->getType().c_str(), rightSide->getType().c_str()) && !strcmp(leftSide->getType().c_str(), "NUMBER")) {
		void* resLeft;
		void* resRight;
		char typeL = strToNum(leftSide->value.c_str(), &resLeft);
		char typeR = strToNum(rightSide->value.c_str(), &resRight);
		int* leftInt = nullptr;
		int* rightInt = nullptr;
		float* leftFloat = nullptr;
		float* rightFloat = nullptr;

		if (typeL == 'f') {
			leftFloat = (float*)resLeft;
		}
		else if (typeL == 'i') {
			leftInt = (int*)resLeft;
		}
		if (typeR == 'f') {
			rightFloat = (float*)resRight;
		}
		else if (typeR == 'i') {
			rightInt = (int*)resRight;
		}

		if (rightFloat == nullptr && leftFloat == nullptr) {
			return to_string(*leftInt - *rightInt);
		}
		else if (rightFloat == nullptr && leftInt == nullptr) {
			return to_string((int)(*leftFloat) - *rightInt);
		}
		else if (rightInt == nullptr && leftInt == nullptr) {
			return to_string(*leftFloat - *rightFloat);
		}
		else if (rightInt == nullptr && leftFloat == nullptr) {
			return to_string(*leftInt - (int)(*rightFloat));
		}
	}
	else {
		cout << "Type missmatch on evaluation of expression.\n";
	}
	return nullptr;
}

static string multiply(Expr* leftSide, Expr* rightSide, Expr* parent) {
	parent->setType("NUMBER");
	leftSide->evaluate();
	rightSide->evaluate();
	if (!strcmp(leftSide->getType().c_str(), rightSide->getType().c_str()) && !strcmp(leftSide->getType().c_str(), "NUMBER")) {
		void* resLeft;
		void* resRight;
		char typeL = strToNum(leftSide->value.c_str(), &resLeft);
		char typeR = strToNum(rightSide->value.c_str(), &resRight);
		int* leftInt = nullptr;
		int* rightInt = nullptr;
		float* leftFloat = nullptr;
		float* rightFloat = nullptr;

		if (typeL == 'f') {
			leftFloat = (float*)resLeft;
		}
		else if (typeL == 'i') {
			leftInt = (int*)resLeft;
		}
		if (typeR == 'f') {
			rightFloat = (float*)resRight;
		}
		else if (typeR == 'i') {
			rightInt = (int*)resRight;
		}

		if (rightFloat == nullptr && leftFloat == nullptr) {
			return to_string(*leftInt * *rightInt);
		}
		else if (rightFloat == nullptr && leftInt == nullptr) {
			return to_string((int)(*leftFloat) * *rightInt);
		}
		else if (rightInt == nullptr && leftInt == nullptr) {
			return to_string(*leftFloat * *rightFloat);
		}
		else if (rightInt == nullptr && leftFloat == nullptr) {
			return to_string(*leftInt * (int)(*rightFloat));
		}
	}
	else {
		cout << "Type missmatch on evaluation of expression.\n";
	}
	return nullptr;
}

static string divide(Expr* leftSide, Expr* rightSide, Expr* parent) {
	parent->setType("NUMBER");
	leftSide->evaluate();
	rightSide->evaluate();
	if (!strcmp(leftSide->getType().c_str(), rightSide->getType().c_str()) && !strcmp(leftSide->getType().c_str(), "NUMBER")) {
		void* resLeft;
		void* resRight;
		char typeL = strToNum(leftSide->value.c_str(), &resLeft);
		char typeR = strToNum(rightSide->value.c_str(), &resRight);
		int* leftInt = nullptr;
		int* rightInt = nullptr;
		float* leftFloat = nullptr;
		float* rightFloat = nullptr;

		if (typeL == 'f') {
			leftFloat = (float*)resLeft;
		}
		else if (typeL == 'i') {
			leftInt = (int*)resLeft;
		}
		if (typeR == 'f') {
			rightFloat = (float*)resRight;
		}
		else if (typeR == 'i') {
			rightInt = (int*)resRight;
		}

		if (rightFloat == nullptr && leftFloat == nullptr) {
			return to_string(*leftInt / *rightInt);
		}
		else if (rightFloat == nullptr && leftInt == nullptr) {
			return to_string((int)(*leftFloat) / *rightInt);
		}
		else if (rightInt == nullptr && leftInt == nullptr) {
			return to_string(*leftFloat / *rightFloat);
		}
		else if (rightInt == nullptr && leftFloat == nullptr) {
			return to_string(*leftInt / (int)(*rightFloat));
		}
	}
	else {
		cout << "Type missmatch on evaluation of expression.\n";
	}
	return nullptr;
}

static string equality(Expr* leftSide, Expr* rightSide, Expr* parent) {
	parent->setType("NUMBER");
	leftSide->evaluate();
	rightSide->evaluate();
	if (!strcmp(leftSide->getType().c_str(), rightSide->getType().c_str()) && !strcmp(leftSide->getType().c_str(), "NUMBER")) {
		void* resLeft;
		void* resRight;
		char typeL = strToNum(leftSide->value.c_str(), &resLeft);
		char typeR = strToNum(rightSide->value.c_str(), &resRight);
		int* leftInt = nullptr;
		int* rightInt = nullptr;
		float* leftFloat = nullptr;
		float* rightFloat = nullptr;

		if (typeL == 'f') {
			leftFloat = (float*)resLeft;
		}
		else if (typeL == 'i') {
			leftInt = (int*)resLeft;
		}
		if (typeR == 'f') {
			rightFloat = (float*)resRight;
		}
		else if (typeR == 'i') {
			rightInt = (int*)resRight;
		}

		if (rightFloat == nullptr && leftFloat == nullptr) {
			return to_string(*leftInt == *rightInt);
		}
		else if (rightFloat == nullptr && leftInt == nullptr) {
			return to_string((int)(*leftFloat) == *rightInt);
		}
		else if (rightInt == nullptr && leftInt == nullptr) {
			return to_string(*leftFloat == *rightFloat);
		}
		else if (rightInt == nullptr && leftFloat == nullptr) {
			return to_string(*leftInt == (int)(*rightFloat));
		}
	}
	else {
		cout << "Type missmatch on evaluation of expression.\n";
	}
	return nullptr;
}

static string inequality(Expr* leftSide, Expr* rightSide, Expr* parent) {
	parent->setType("NUMBER");
	leftSide->evaluate();
	rightSide->evaluate();
	if (!strcmp(leftSide->getType().c_str(), rightSide->getType().c_str()) && !strcmp(leftSide->getType().c_str(), "NUMBER")) {
		void* resLeft;
		void* resRight;
		char typeL = strToNum(leftSide->value.c_str(), &resLeft);
		char typeR = strToNum(rightSide->value.c_str(), &resRight);
		int* leftInt = nullptr;
		int* rightInt = nullptr;
		float* leftFloat = nullptr;
		float* rightFloat = nullptr;

		if (typeL == 'f') {
			leftFloat = (float*)resLeft;
		}
		else if (typeL == 'i') {
			leftInt = (int*)resLeft;
		}
		if (typeR == 'f') {
			rightFloat = (float*)resRight;
		}
		else if (typeR == 'i') {
			rightInt = (int*)resRight;
		}

		if (rightFloat == nullptr && leftFloat == nullptr) {
			return to_string(*leftInt != *rightInt);
		}
		else if (rightFloat == nullptr && leftInt == nullptr) {
			return to_string((int)(*leftFloat) != *rightInt);
		}
		else if (rightInt == nullptr && leftInt == nullptr) {
			return to_string(*leftFloat != *rightFloat);
		}
		else if (rightInt == nullptr && leftFloat == nullptr) {
			return to_string(*leftInt != (int)(*rightFloat));
		}
	}
	else {
		cout << "Type missmatch on evaluation of expression.\n";
	}
	return nullptr;
}

static string lt(Expr* leftSide, Expr* rightSide, Expr* parent) {
	parent->setType("NUMBER");
	leftSide->evaluate();
	rightSide->evaluate();
	if (!strcmp(leftSide->getType().c_str(), rightSide->getType().c_str()) && !strcmp(leftSide->getType().c_str(), "NUMBER")) {
		void* resLeft;
		void* resRight;
		char typeL = strToNum(leftSide->value.c_str(), &resLeft);
		char typeR = strToNum(rightSide->value.c_str(), &resRight);
		int* leftInt = nullptr;
		int* rightInt = nullptr;
		float* leftFloat = nullptr;
		float* rightFloat = nullptr;

		if (typeL == 'f') {
			leftFloat = (float*)resLeft;
		}
		else if (typeL == 'i') {
			leftInt = (int*)resLeft;
		}
		if (typeR == 'f') {
			rightFloat = (float*)resRight;
		}
		else if (typeR == 'i') {
			rightInt = (int*)resRight;
		}

		if (rightFloat == nullptr && leftFloat == nullptr) {
			return to_string(*leftInt < *rightInt);
		}
		else if (rightFloat == nullptr && leftInt == nullptr) {
			return to_string((int)(*leftFloat) < *rightInt);
		}
		else if (rightInt == nullptr && leftInt == nullptr) {
			return to_string(*leftFloat < *rightFloat);
		}
		else if (rightInt == nullptr && leftFloat == nullptr) {
			return to_string(*leftInt < (int)(*rightFloat));
		}
	}
	else {
		cout << "Type missmatch on evaluation of expression.\n";
	}
	return nullptr;
}

static string gt(Expr* leftSide, Expr* rightSide, Expr* parent) {
	parent->setType("NUMBER");
	leftSide->evaluate();
	rightSide->evaluate();
	if (!strcmp(leftSide->getType().c_str(), rightSide->getType().c_str()) && !strcmp(leftSide->getType().c_str(), "NUMBER")) {
		void* resLeft;
		void* resRight;
		char typeL = strToNum(leftSide->value.c_str(), &resLeft);
		char typeR = strToNum(rightSide->value.c_str(), &resRight);
		int* leftInt = nullptr;
		int* rightInt = nullptr;
		float* leftFloat = nullptr;
		float* rightFloat = nullptr;

		if (typeL == 'f') {
			leftFloat = (float*)resLeft;
		}
		else if (typeL == 'i') {
			leftInt = (int*)resLeft;
		}
		if (typeR == 'f') {
			rightFloat = (float*)resRight;
		}
		else if (typeR == 'i') {
			rightInt = (int*)resRight;
		}

		if (rightFloat == nullptr && leftFloat == nullptr) {
			return to_string(*leftInt > *rightInt);
		}
		else if (rightFloat == nullptr && leftInt == nullptr) {
			return to_string((int)(*leftFloat) > *rightInt);
		}
		else if (rightInt == nullptr && leftInt == nullptr) {
			return to_string(*leftFloat > *rightFloat);
		}
		else if (rightInt == nullptr && leftFloat == nullptr) {
			return to_string(*leftInt > (int)(*rightFloat));
		}
	}
	else {
		cout << "Type missmatch on evaluation of expression.\n";
	}
	return nullptr;
}

static string lte(Expr* leftSide, Expr* rightSide, Expr* parent) {
	parent->setType("NUMBER");
	leftSide->evaluate();
	rightSide->evaluate();
	if (!strcmp(leftSide->getType().c_str(), rightSide->getType().c_str()) && !strcmp(leftSide->getType().c_str(), "NUMBER")) {
		void* resLeft;
		void* resRight;
		char typeL = strToNum(leftSide->value.c_str(), &resLeft);
		char typeR = strToNum(rightSide->value.c_str(), &resRight);
		int* leftInt = nullptr;
		int* rightInt = nullptr;
		float* leftFloat = nullptr;
		float* rightFloat = nullptr;

		if (typeL == 'f') {
			leftFloat = (float*)resLeft;
		}
		else if (typeL == 'i') {
			leftInt = (int*)resLeft;
		}
		if (typeR == 'f') {
			rightFloat = (float*)resRight;
		}
		else if (typeR == 'i') {
			rightInt = (int*)resRight;
		}

		if (rightFloat == nullptr && leftFloat == nullptr) {
			return to_string(*leftInt <= *rightInt);
		}
		else if (rightFloat == nullptr && leftInt == nullptr) {
			return to_string((int)(*leftFloat) <= *rightInt);
		}
		else if (rightInt == nullptr && leftInt == nullptr) {
			return to_string(*leftFloat <= *rightFloat);
		}
		else if (rightInt == nullptr && leftFloat == nullptr) {
			return to_string(*leftInt <= (int)(*rightFloat));
		}
	}
	else {
		cout << "Type missmatch on evaluation of expression.\n";
	}
	return nullptr;
}

static string gte(Expr* leftSide, Expr* rightSide, Expr* parent) {
	parent->setType("NUMBER");
	leftSide->evaluate();
	rightSide->evaluate();
	if (!strcmp(leftSide->getType().c_str(), rightSide->getType().c_str()) && !strcmp(leftSide->getType().c_str(), "NUMBER")) {
		void* resLeft;
		void* resRight;
		char typeL = strToNum(leftSide->value.c_str(), &resLeft);
		char typeR = strToNum(rightSide->value.c_str(), &resRight);
		int* leftInt = nullptr;
		int* rightInt = nullptr;
		float* leftFloat = nullptr;
		float* rightFloat = nullptr;

		if (typeL == 'f') {
			leftFloat = (float*)resLeft;
		}
		else if (typeL == 'i') {
			leftInt = (int*)resLeft;
		}
		if (typeR == 'f') {
			rightFloat = (float*)resRight;
		}
		else if (typeR == 'i') {
			rightInt = (int*)resRight;
		}

		if (rightFloat == nullptr && leftFloat == nullptr) {
			return to_string(*leftInt >= *rightInt);
		}
		else if (rightFloat == nullptr && leftInt == nullptr) {
			return to_string((int)(*leftFloat) >= *rightInt);
		}
		else if (rightInt == nullptr && leftInt == nullptr) {
			return to_string(*leftFloat >= *rightFloat);
		}
		else if (rightInt == nullptr && leftFloat == nullptr) {
			return to_string(*leftInt >= (int)(*rightFloat));
		}
	}
	else {
		cout << "Type missmatch on evaluation of expression.\n";
	}
	return nullptr;
}

static string or_f(Expr* leftSide, Expr* rightSide, Expr* parent) {
	parent->setType("NUMBER");
	leftSide->evaluate();
	if (strcmp("0", leftSide->value.c_str())) {
		return "1";
	}
	rightSide->evaluate();
	if (strcmp("0", rightSide->value.c_str())) {
		return "1";
	}
	return "0";
}

static string and_f(Expr* leftSide, Expr* rightSide, Expr* parent) {
	parent->setType("NUMBER");
	leftSide->evaluate();
	if (!strcmp("0", leftSide->value.c_str())) {
		return "0";
	}
	rightSide->evaluate();
	if (!strcmp("0", rightSide->value.c_str())) {
		return "0";
	}
	return "1";
}

static string compare(Expr* leftSide, Expr* rightSide, Expr* parent) {
	parent->setType("NUMBER");
	leftSide->evaluate();
	rightSide->evaluate();
	if (!strcmp(leftSide->getType().c_str(), rightSide->getType().c_str()) && !strcmp(leftSide->getType().c_str(), "STRING")) {
		return to_string(strcmp(leftSide->value.c_str(), rightSide->value.c_str()));
	}
	else {
		cout << "Type missmatch on evaluation of expression.\n";
	}
	return nullptr;
}
// Initialize the operation functions
void initialize_operations() {
	operation_functions.insert("+", (void *)&add);
	operation_functions.insert("-", (void *)&subtract);
	operation_functions.insert("*", (void *)&multiply);
	operation_functions.insert("/", (void *)&divide);
	operation_functions.insert("==", (void *)&equality);
	operation_functions.insert("!=", (void *)&inequality);
	operation_functions.insert("|", (void *)&compare);
	operation_functions.insert("<", (void *)&lt);
	operation_functions.insert(">", (void *)&gt);
	operation_functions.insert("<=", (void *)&lte);
	operation_functions.insert(">=", (void *)&gte);
	operation_functions.insert("||", (void *)&or_f);
	operation_functions.insert("&&", (void *)&and_f);
}
// Expr

void Expr::print() {
	if (this != nullptr) {
		if (!strcmp(this->value.c_str(), "")) {
			cout << "(" << this->op->getTokenValue() << " ";
			this->left->print();
			cout << " ";
			this->right->print();
			cout << ")";
		}
		else {
			cout << this->value;
		}
	}
}

string Expr::evaluate() {
		if (this == nullptr) {
			return "";
		}
		else if (this->left == nullptr && this->right == nullptr) {
			// cout << this->getType();
			return this->value;
		}
		
		string (*op_function)(Expr*, Expr*, Expr*) = (string (*) (Expr*, Expr*, Expr*))operation_functions.lookup(this->op->getTokenValue().c_str());
		this->value = (*op_function)(this->left, this->right, this);
		//cout << "the value: " << this->value;
		return this->value;
}

// Binary

Binary::Binary(Expr* left, Token* op, Expr* right, string type) {
	this->name = "";
	this->type = type;
	this->left = left;
	this->op = op;
	this->right = right;
}

Binary::~Binary() {
	
}

// Literal

Literal::Literal(string value, string type) {
	this->type = type;
	this->value = value;
	this->left = nullptr;
	this->op = nullptr;
	this->right = nullptr;
}

Literal::~Literal() {
	if (this->left != nullptr) {
		delete left;
	}

	if (this->right != nullptr) {
		delete right;
	}

	if (this->op != nullptr) {
		delete op;
	}
}