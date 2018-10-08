//#pragma once
#include <iostream>
#include <string>
#include "Token.h"

Token::Token(std::string p_key, std::string p_value) {
	this->key = p_key;
	this->value = p_value;
}

Token::~Token() {

}

void Token::setToken(std::string p_key, std::string p_value) {
	this->key = p_key;
	this->value = p_value;
}

std::string Token::getTokenKey() {
	return this->key;
}

std::string Token::getTokenValue() {
	return this->value;
}

void Token::coutToken() {
	std::cout << "(" + this->key + "," + this->value + ")\n";
}

bool Token::isNull() {
	return (this->key == "" && this->value == "");
}
