#pragma once
#include "Token.h"
#include "Block.h"

class Lexer {
public:
	std::vector <Token> tokens;

	void parse(std::string line);

	void getTokens(); // Prints to the console a list of all the tokens.

	Lexer();

	Lexer(std::string line);

	~Lexer();

	void add(std::string line);

	int size();

	Block feed(); // Returns the next valid block in a vector of tokens
	              // the rest of the vector of tokens is left behind

	bool isEmpty(); // Returns true if the vector of tokens is empty and
	                // Returns false otherwise.
};