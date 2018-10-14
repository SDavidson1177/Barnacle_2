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
	int tok_count = 0;
	int &tk_ref = tok_count;
	initInterpreter(); //initializes project (global variables)
	initialize_operations();
	readTokens(argv[1], &tokens, tk_ref);
	cout << "--Lexer--\n";
	int size = tokens.size();
	for (int i = 0; i < size; i++) {
		tokens.at(i)->coutToken();
	}

	cout << "\n--Parser--\n";
	parse(&tokens);
}
