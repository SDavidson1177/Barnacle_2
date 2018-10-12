//#pragma once
#include <iostream>
#include <string>
#include "Token.h"

Token::Token(std::string p_key, std::string p_value, int p_index) {
	this->key = p_key;
	this->value = p_value;
	this->index = p_index;
}

Token::~Token() {

}

void Token::setToken(std::string p_key, std::string p_value, int p_index) {
	this->key = p_key;
	this->value = p_value;
	this->index = p_index;
}

std::string Token::getTokenKey() {
	return this->key;
}

std::string Token::getTokenValue() {
	return this->value;
}

void Token::coutToken() {
	std::cout << "(" << this->key << "," << this->value << ", " << this->index << ")\n";
}

bool Token::isNull() {
	return (this->key == "" && this->value == "");
}