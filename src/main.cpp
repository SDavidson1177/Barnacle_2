#include <iostream>
#include <vector>
#include <fstream>
#include <assert.h>
#include "Parser.h"
#include "languageio.h"

// Defines
#undef DEBUG_TOKENS

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

	if (argc > 1) { // for opening and running a file
		readTokens(argv[1], &tokens, tk_ref);
#ifdef DEBUG_TOKENS
		cout << "--Lexer--\n";
		int size = tokens.size();
		for (int i = 0; i < size; i++) {
			tokens.at(i)->coutToken();
		}
#endif
		cout << "\n--Parser--\n";
		parse(&tokens);
	}
	else { // the command line interface
		cout << "Barnacle Alpha version\n";
		string input;

		// the output the screen displays
		cout << ">>>";
		while (getline(cin, input)) {
			readTokens(nullptr, &tokens, tk_ref, input);
#ifdef DEBUG_TOKENS
			cout << "--Lexer--\n";
			int size = tokens.size();
			for (int i = 0; i < size; i++) {
				tokens.at(i)->coutToken();
			}
#endif
			parse(&tokens);
			tokens.clear();
			cout << "\n>>>";
		}
	}
}
