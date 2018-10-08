#include <iostream>
#include <vector>
#include <fstream>
#include <assert.h>
#include "Parser.h"
//#include "languageio.h"

using namespace std;

// Global Program Variables
vector <Token*> tokens;
vector <Expr*> statements;
// extern FunctionDictionary operation_functions;

int main(int argc, char** argv) {
	initInterpreter(); //initializes project (global variables)
	initialize_operations();
	readTokens(argv[1], &tokens);
	cout << "--Lexer--\n";
	int size = tokens.size();
	for (int i = 0; i < size; i++) {
		tokens.at(i)->coutToken();
	}

	cout << "\n--Parser--\n";
	parse(&tokens);
	int stSize = statements.size();
	/*for (int i = 0; i < stSize; i++) {
		//statements.at(i)->print();
		//statements.at(i)->evaluate();
		//cout << " => " << statements.at(i)->value << endl;
	}*/
}