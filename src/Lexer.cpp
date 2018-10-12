//#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "ToolFunctions.h"
#include "Lexer.h"

// Global Variables
bool inString = false; // determines when we are lexing a string
std::string in = "";
bool startingComment = false;

					   // Global listing of all key words
std::vector <std::vector <std::string>> keywords = { { "if", "If" },{ "else", "Else" },{ "function", "Function_Dec" },{ "var" , "Var_Dec" },{ "then", "Then" },
{ "true", "True" },{ "false", "False" } };
std::vector <std::vector <std::string>> symbols = { { "==", "Equality" },{ "=", "Assignment" },{ "+", "Addition" },{ "-", "Subtraction" },{ "/", "Division" },{ "*", "Multiplication" },
{ ")", "RBracket" },{ "(", "LBracket" } ,{ "}", "RBrace" } ,{ "{", "LBrace" } ,{ ";", "Semi-Colon" },{ ",", "Comma" }};
std::vector <std::vector <std::string>> combined = vectorAppend(keywords, symbols);
					   // Global listing of all symbols that end a  block. (determine when a block terminates and begins)
std::vector <std::string> block_beginners = { "LBracket", "LBrace"};
std::vector <std::string> block_enders = { "RBracket", "RBrace", "Semi-Colon" };

Lexer::Lexer() {

}

Lexer::Lexer(std::string line) {
	Lexer::parse(line);
}

Lexer::~Lexer() {

}

void Lexer::parse(std::string line) {
	std::string key = "";
	for (int i = 0; i < line.length(); i++) { // Create Token when there is a new line	
		if ((line[i] == ' ' || line[i] == '	') && !inString) {// Adds a token when there is a space
			if (isMember(key, symbols.size(), symbols) > 0) {
				std::string new_key = key.substr(isMember(key, symbols.size(), symbols));
				getTag(key.substr(0, isMember(key, symbols.size(), symbols)), combined, "Symbol", &this->tokens);
				getTag(new_key, symbols, "Symbol", &this->tokens);
				key = "";
			}
			else if (!stringEquals(key, "") && !stringEquals(key, " ")) {
				getTag(key, combined, "Symbol", &this->tokens);
				key = "";
			}
			else {
				key = "";
			}
		}else if(line[i] == '#'){
			if (startingComment) {
				break;
			}
			else {
				startingComment = true;
			}
		}
		else if (line[i] == '"') {// handles tokens that are strings
			if (!inString) {
				if (key != " " && key != "") {
					getTag(key, combined, "Symbol", &this->tokens);
				}
				key = "\"";
			}
			else {
				key = key + "\"";
				this->tokens.push_back(Token(key, "String"));
				key = "";
			}
			inString = !inString;
		}
		else if (isMember(key, symbols.size(), symbols) == 0 && !inString) {// handles tokens where the
																			// first character(s) is a symbol.
			if (stringEquals(key, std::string(1, line[i]))) {
				if (key == "=") {
					this->tokens.push_back(Token("==", "Equality"));
				}
				else if (key == "+") {
					this->tokens.push_back(Token("++", "Add1"));
				}
				else if (key == "-") {
					this->tokens.push_back(Token("--", "Subtract1"));
				}
				i++;
				if (line[i] == ' ') {
					i++;
				}
			}
			else {
				getTag(key, symbols, "Symbol", &this->tokens);
			}
			key = "";
			if (i < line.length()) {
				key.append(std::string(1, line[i]));
			}
		}
		else if (isMember(key, symbols.size(), symbols) > 0 && !inString) {// handles tokens where the trailing
																		   // letters form a symbol.
			bool append = true;
			std::string new_key = key.substr(isMember(key, symbols.size(), symbols));
			getTag(key.substr(0, isMember(key, symbols.size(), symbols)), combined, "Symbol", &this->tokens);
			if (stringEquals(new_key, std::string(1, line[i]))) {
				if (new_key == "=") {
					this->tokens.push_back(Token("==", "Equality"));
				}
				else if (new_key == "+") {
					this->tokens.push_back(Token("++", "Add1"));
				}
				else if (new_key == "-") {
					this->tokens.push_back(Token("--", "Subtract1"));
				}
				i++;
				if (line[i] == ' ') {
					i++;
				}
			}
			else {
				getTag(new_key, symbols, "Symbol", &this->tokens);
			}
			key = "";
			if (i < line.length()) {
				key.append(std::string(1, line[i]));
			}
		}
		else {
			key.append(std::string(1, line[i]));
			startingComment = false;
		}
	}
	// Preforms appropriate at the end of a line.
	if (isMember(key, symbols.size(), symbols) > 0 && !inString) { // If line ends on a symbol.
		std::string new_key = key.substr(isMember(key, symbols.size(), symbols));
		getTag(key.substr(0, isMember(key, symbols.size(), symbols)), combined, "Symbol", &this->tokens);
		getTag(new_key, symbols, "Symbol", &this->tokens);
	}
	else if (key != " " && key != "" && key != "	") { // If line ends on anything else but a space.
		getTag(key, combined, "Symbol", &this->tokens);
	}
	key = "";
}

void Lexer::getTokens() { // Prints to the console a list of all the tokens.
	for (int token_index = 0; token_index < this->tokens.size(); token_index++) {
		this->tokens.at(token_index).coutToken();
	}
}

void Lexer::add(std::string line) {
	Lexer::parse(line);
}

//Takes all of the generated tokens and produces blocks.
Block Lexer::feed() {
	Block acc_block;
	const int size = this->tokens.size();
	for (int i = 0; i < size; i++) {
		if (in == "LBracket" && this->tokens.at(0).getTokenValue() == "Comma") {
			this->tokens.erase(this->tokens.begin());
			break;
		}
		else if (isMember (this->tokens.at(0).getTokenValue(), block_beginners)) {
			in = this->tokens.at(0).getTokenValue();
			if (acc_block.isEmpty()) {
				acc_block.push_back(this->tokens.at(0));
				this->tokens.erase(this->tokens.begin());
			}
			break;
		}
		else if (isMember (this->tokens.at(0).getTokenValue(), block_enders)) {
			if (acc_block.isEmpty()) {
				acc_block.push_back(this->tokens.at(0));
				this->tokens.erase(this->tokens.begin());
			}
			in = "";
			break;
		}
		else {
			acc_block.push_back(this->tokens.at(0));
			this->tokens.erase(this->tokens.begin());
		}
	}
	return acc_block;
}

bool Lexer::isEmpty() {
	return this->tokens.empty();
}

int Lexer::size() {
	return this->tokens.size();
}