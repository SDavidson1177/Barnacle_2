#include "Variable.h"
#include <iostream>
#include <string.h>

using namespace std;

Variable::Variable(const char* p_key, const char* p_type) {
	this->key = p_key;
	this->type = p_type;
}

Variable::~Variable() {

}

void Variable::print() {
	std::cout << "Key " << this->key << " Type " << this->type;
}

string Variable::getKey() {
	return this->key.c_str();
}

string Variable::getType() {
	return this->type.c_str();
}

void Variable::setType(string new_type) {
	this->type = new_type;
}
