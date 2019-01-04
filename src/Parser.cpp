#include "Parser.h"
#include <assert.h>
#include <iostream>
#include <regex>

static Expr* equation(vector <Token*>* tokens, vector <map<string, pair<Variable*, const char*>>>* scope, int* index, int end, bool get_comp=true);
static Expr* prepEquation(vector <Token*>* tokens, int* tokensSize, vector <map<string, pair<Variable*, const char*>>>* scope, int* i, string ender="SEMI_COLON");
VarDictionary* globalVariables = new VarDictionary();

Function* callFunction(vector<Token*>* tokens, vector <map<string, pair<Variable*, const char*>>>* scope, int *index, const int tokensSize);

// Important global variables
LangFuncDictionary* functions = new LangFuncDictionary();
Function* (*Literal::function_parser)(vector<Token*>*, vector <map<string, pair<Variable*, const char*>>>*, int*, const int) = callFunction;

// Detection of in in function or in class
bool IN_FUNCTION = false;
bool STACK_UNWIND = false;
string RETURN = "";

string getType(Token** lookup, vector <map<string, pair<Variable*, const char*>>>* scope) {
	if (!strcmp((*lookup)->getTokenKey().c_str(), "SYMBOL")) {
		Variable* var = nullptr;
		if (scope) { // try to find the variable by checking the scope of nested functions/loops/statements
			int depth = scope->size() - 1;
			while (depth > -1 && !var) {
				auto map_to_var = (scope->at(depth).find((*lookup)->getTokenValue()));
				if (map_to_var != scope->at(depth).end()) {
					var = (*map_to_var).second.first;
				}
				depth--;
			}
		}
		if (!var) { // It must be a global variable if it is not in any functions/loops/statements scope;
			var = globalVariables->get((*lookup)->getTokenValue().c_str());
		}
		if (!var) {
			return (*lookup)->getTokenKey();
		}
		return (string)var->getType();
	}
	if (!strcmp((*lookup)->getTokenKey().c_str(), "SEMI_COLON")) { // if we're dealing with a function's type
		return "NUMBER";
	}
	return (*lookup)->getTokenKey();
}

string getValue(Token** lookup, vector <map<string, pair<Variable*, const char*>>>* scope) {
	if (!strcmp((*lookup)->getTokenKey().c_str(), "SYMBOL")) {
		const char* var = nullptr;
		if (scope) { // try to find the variable by checking the scope of nested functions/loops/statements
			int depth = scope->size() - 1;
			while (depth > -1 && !var){
				auto map_to_var = scope->at(depth).find((*lookup)->getTokenValue());
				if (map_to_var != scope->at(depth).end()) {
					var = (*map_to_var).second.second;
				}
				depth--;
			}
		}
		if (!var) { // It must be a global variable if it is not in any functions/loops/statements scope;
			var = globalVariables->lookup((*lookup)->getTokenValue().c_str());
		}
		if (!var) {
			return (*lookup)->getTokenValue();
		}
		return (string)var;
	}
	return (*lookup)->getTokenValue();
}

Variable* getKey(Token** lookup, vector <map<string, pair<Variable*, const char*>>>* scope) {
	if (!strcmp((*lookup)->getTokenKey().c_str(), "SYMBOL")) {
		Variable** var = nullptr;
		if (scope) { // try to find the variable by checking the scope of nested functions/loops/statements
			int depth = 0;
			int max_depth = scope->size();
			while (depth < max_depth && !var) {
				auto map_to_var = (scope->at(depth).find((*lookup)->getTokenValue()));
				if (map_to_var != scope->at(depth).end()) {
					*var = (*map_to_var).second.first;
				}
				depth++;
			}
		}
		if (!var) { // It must be a global variable if it is not in any functions/loops/statements scope;
			*var = globalVariables->get((*lookup)->getTokenValue().c_str());
		}
		return *var;
	}
	return nullptr;
}

const char** p_getValue(Token** lookup, vector <map<string, pair<Variable*, const char*>>>* scope) {
	if (!strcmp((*lookup)->getTokenKey().c_str(), "SYMBOL")) {
		const char** var = nullptr;
		if (scope) { // try to find the variable by checking the scope of nested functions/loops/statements
			int depth = 0;
			int max_depth = scope->size();
			while (depth < max_depth && !var) {
				auto map_to_var = (scope->at(depth).find((*lookup)->getTokenValue()));
				if (map_to_var != scope->at(depth).end()) {
					var = &((*map_to_var).second.second);
				}
				depth++;
			}
		}
		if (!var) { // It must be a global variable if it is not in any functions/loops/statements scope;
			var = globalVariables->p_lookup((*lookup)->getTokenValue().c_str());
		}
		return var;
	}
	return nullptr;
}

Variable** p_getKey(Token** lookup, vector <map<string, pair<Variable*, const char*>>>* scope) {
	if (!strcmp((*lookup)->getTokenKey().c_str(), "SYMBOL")) {
		Variable** var = nullptr;
		if (scope) { // try to find the variable by checking the scope of nested functions/loops/statements
			int depth = 0;
			int max_depth = scope->size();
			while (depth < max_depth && !var) {
				auto map_to_var = (scope->at(depth).find((*lookup)->getTokenValue()));
				if (map_to_var != scope->at(depth).end()) {
					var = &((*map_to_var).second.first);
				}
				depth++;
			}
		}
		if (!var) { // It must be a global variable if it is not in any functions/loops/statements scope;
			var = globalVariables->p_get((*lookup)->getTokenValue().c_str());
		}
		return var;
	}
	return nullptr;
}

int getPossiblyTypedTokenIndex(vector<Token*>* tokens, int index) {
	int check = index;
	while (!strcmp(tokens->at(check)->getTokenKey().c_str(), "L_PAREN")) {
		check += 1;
	}
	return check;
}

void getArguments(Function **func, vector<Token*>* tokens, vector <map<string, pair<Variable*, const char*>>>* scope, int *index) {
	int tokensSize = tokens->size();
	*index += 1;
	int argument_index = 0;
	while (*index < tokensSize && strcmp(tokens->at(*index)->getTokenKey().c_str(), "SEMI_COLON")) {
		if (argument_index >= (*func)->numOfArgs) {
			cerr << "Too many arguments passed. Expected no more than " << (*func)->numOfArgs << " argument(s)";
			throw "\n";
		}
		Expr* exp = prepEquation(tokens, &tokensSize, scope, index, "(COMMA|SEMI_COLON)");
		exp->evaluate();
		setArgument(func, argument_index, exp->value, exp->getType());

		if (!strcmp(tokens->at(*index)->getTokenKey().c_str(), "SEMI_COLON")) { // we have reached the end of the call
			break;
		}
		*index += 1;
		argument_index++;
	}
}

Function* callFunction(vector<Token*>* tokens, vector <map<string, pair<Variable*, const char*>>>* scope, int *index, const int tokensSize) {
	(*index)++;
	if (true) {
		Function* given_function = new Function;
		given_function = functions->get(getValue(&tokens->at((*index)), scope).c_str());
		Function &func_ref = *given_function;
		// If we have a valid function name given
		if (!strcmp(tokens->at((*index))->getTokenKey().c_str(), "SYMBOL") && given_function != nullptr) {
			// Get all arguments and pass it to the function for evaluation.
			getArguments(&given_function, tokens, scope, index);
			IN_FUNCTION = true;
			// vector <VarDictionary*>* s = new vector <VarDictionary*>;
			//*s = given_function->scope;
			parse(&(given_function->tokens), &(given_function->scope));
			STACK_UNWIND = false;
			IN_FUNCTION = false;
			given_function->return_value = RETURN; // if no return value is given, the default is the empty string
			RETURN = "";
			return given_function;
		}
		else {
			cout << "Expected a function name given for call.";
		}
		delete given_function;
	}
	/*else {
		cout << "Too few tokens for function call";
		throw "\n";
	}*/
	return nullptr;
}

/*Seems as though we can ignore the index variable*/
void parse(vector<Token*>* tokens, vector <map<string, pair<Variable*, const char*>>>* scope) {
	assert(tokens);
	try {
		int tokensSize = tokens->size();

		Expr** curr = nullptr;

		for (int i = 0; i < tokensSize; i++) {
			if (!strcmp(tokens->at(i)->getTokenKey().c_str(), "SYMBOL")) { // check for possible control flow (if statement)
				if (!strcmp(tokens->at(i)->getTokenValue().c_str(), "if")) {
					// Create the if statement
					struct IfStatement* if_eval = initIfStatement();
					int start = 0, end = 0;
					while (true) {
						Expr* exp;
						if (!strcmp(tokens->at(i)->getTokenValue().c_str(), "else")) {
							while (strcmp(tokens->at(i)->getTokenKey().c_str(), "COLON")) {
								i++;
							}
							exp = new Literal("1", "NUMBER"); // Set it equal to true
						}
						else {
							i += 1; // go to the beginning of the condition
							int check = getPossiblyTypedTokenIndex(tokens, i);
							string given_type = getType(&(tokens->at(check)), scope);
							exp = prepEquation(tokens, &tokensSize, scope, &i, "COLON"); // get the condition
						}
						start = i + 1;
						end = i + 1;
						int nested_ifs = 0; // keep track of nested if statements. Skip those endifs.
						while (nested_ifs || (end < tokensSize && strcmp(tokens->at(end)->getTokenValue().c_str(), "endif") &&
							strcmp(tokens->at(end)->getTokenValue().c_str(), "elif") && strcmp(tokens->at(end)->getTokenValue().c_str(), "else"))) { // set the end of the token list
							if (!strcmp(tokens->at(end)->getTokenValue().c_str(), "if")) {
								nested_ifs++;
							}
							else if (!strcmp(tokens->at(end)->getTokenValue().c_str(), "endif")) {
								nested_ifs--;
							}
							end++;
						}

						if (strcmp(tokens->at(end)->getTokenValue().c_str(), "endif") &&
							strcmp(tokens->at(end)->getTokenValue().c_str(), "elif") &&
							strcmp(tokens->at(end)->getTokenValue().c_str(), "else")) { // throw an exception if there is no end if
							cout << "Expected an elif, else or endif.";
							throw "\n";
						}
						addIfNode(&if_eval, exp, tokens, start, end, scope);

						// If we are at endif, break out of the loop and evaluate the if statement
						if (!strcmp(tokens->at(end)->getTokenValue().c_str(), "endif")) {
							break;
						}
						i = end;
					}
					struct IfNode* true_node = evaluateIfStatement(if_eval);
					if (true_node) { // we can parse the tokens as long as there is a true node
						parse(&(true_node->tokens), &(true_node->scope));
						if (STACK_UNWIND) {
							return;
						}
					}
					i = end; // skip past the end if and continue parsing from there
					destroyIfStatement(if_eval);
				}
				else if (!strcmp(tokens->at(i)->getTokenValue().c_str(), "while")) {
					i += 1; // go to the beginning of the condition
					const int reset = i;
					int check = getPossiblyTypedTokenIndex(tokens, i);
					string given_type = getType(&(tokens->at(check)), scope);
					Expr* exp = prepEquation(tokens, &tokensSize, scope, &i, "COLON"); // get the condition

					int start = i + 1;
					int end = i + 1;

					while (end < tokensSize && strcmp(tokens->at(end)->getTokenValue().c_str(), "endloop")) { // set the end of the token list
						end++;
					}

					if (strcmp(tokens->at(end)->getTokenValue().c_str(), "endloop")) { // throw an exception if there is no end if
						cout << "Expected an endloop";
						throw "\n";
					}
					vector<VarDictionary*> given_scope;

					// Create the while loop
					struct WhileLoop* while_eval = initWhileLoop(exp, tokens, start, end, scope);
					Expr cached_condition = *exp;
					exp->evaluate();

					while (!strcmp("1", while_eval->condition->value.c_str())) {
						parse(&(while_eval->tokens), &(while_eval->scope));
						if (STACK_UNWIND) {
							return;
						}
						i = reset;
						while_eval->condition = prepEquation(tokens, &tokensSize, scope, &i, "COLON"); // get the condition;
						while_eval->condition->evaluate();
					}
					i = end;
				}
				else if (i + 1 < tokensSize && !strcmp(tokens->at(i + 1)->getTokenKey().c_str(), "ASSIGNMENT")) { // reassign variable value
					i += 2;
					if (i >= tokensSize) {
						cout << "Saw reassignment of variable " << tokens->at(i - 2)->getTokenValue() << ", but reached EOF before value was given.";
						throw "\n";
					}

					int check = getPossiblyTypedTokenIndex(tokens, i);
					const int index = i;
					Expr* exp = prepEquation(tokens, &tokensSize, scope, &i);
					exp->evaluate();
					*p_getValue(&(tokens->at(index - 2)), scope) = exp->value.c_str();
				}
				else {
					string given_type = getType(&(tokens->at(i)), scope);
					Expr* exp = prepEquation(tokens, &tokensSize, scope, &i);
					exp->evaluate();
					//cout << "The value " << exp->value << endl;
					i++;
				}
			}
			else if (!strcmp(tokens->at(i)->getTokenKey().c_str(), "NUMBER") || 
				!strcmp(tokens->at(i)->getTokenKey().c_str(), "L_PAREN")) {
				int check = i;
				while (strcmp(tokens->at(check)->getTokenKey().c_str(), "NUMBER") && strcmp(tokens->at(check)->getTokenKey().c_str(), "STRING")) {
					check += 1;
				}
				string given_type = getType(&(tokens->at(check)), scope);
				prepEquation(tokens, &tokensSize, scope, &i);
			}
			else if (!strcmp(tokens->at(i)->getTokenKey().c_str(), "STRING")) {
				prepEquation(tokens, &tokensSize, scope, &i);
			}
			else if (!strcmp(tokens->at(i)->getTokenKey().c_str(), "FUNC_CALL")) {
				callFunction(tokens, scope, &i, tokensSize);
			}
			else if (!strcmp(tokens->at(i)->getTokenKey().c_str(), "SEMI_COLON")) {

			}
			else if (!strcmp(tokens->at(i)->getTokenKey().c_str(), "FUNC_DEF")) {
				// Check if the next index is a symbol. If so create a function
				i++;
				if (i < tokensSize && !strcmp(tokens->at(i)->getTokenKey().c_str(), "SYMBOL")) {
					// Set the identifier of the function
					string identifier = tokens->at(i)->getTokenValue();

					// get all parameters
					vector<string> param;
					i++;
					if (!strcmp(tokens->at(i)->getTokenKey().c_str(), "COLON")) { // If there are no given arguments
						i++; // Go past the ending colon
					}
					else {
						while (i + 1 < tokensSize && strcmp(tokens->at(i + 1)->getTokenKey().c_str(), "COLON")) {
							param.push_back(getValue(&(tokens->at(i)), scope));
							i += 2;
						}
						param.push_back(getValue(&(tokens->at(i)), scope));
						i += 2;
					}
					
					// Get the range of tokens
					const int start = i;
					int end = start;
					while (end < tokensSize && strcmp(tokens->at(end)->getTokenValue().c_str(), "endfunc")) {
						if (!strcmp(tokens->at(end)->getTokenValue().c_str(), "function")) { // There is another function definition inside the function
							cerr << "Cannot define a function from within a function.";
							throw "\n";
						}
						end++;
					}

					if (end >= tokensSize) { // throw an exception since there is no endfunc
						cout << "No end of function reached";
						throw "\n";
					}

					struct Function* f = initFunction(identifier, &param, tokens, start, end, scope);
					// printFunction(f);
					functions->insert(f,"");
					i = end;
				}
				else {
					cout << "No identifier given for function definition.";
					throw "\n";
				}
			}
			else if (!strcmp(tokens->at(i)->getTokenKey().c_str(), "CONSOLE_OUT")) {
				if (i + 1 >= tokensSize) {
					cout << "Too few tokens. Expecting a value after cout";
					throw "\n";
				}
				else {
					int check = getPossiblyTypedTokenIndex(tokens, i + 1);
					i++;
					string given_type = getType(&(tokens->at(check)), scope);
					Expr* exp = prepEquation(tokens, &tokensSize, scope, &i);
					exp->evaluate();
					cout << exp->value << endl;
				}
			}
			else if (!strcmp(tokens->at(i)->getTokenKey().c_str(), "RETURN")) {
				i++;
				if (IN_FUNCTION) {
					if (i >= tokensSize) {
						cerr << "No semi-colon after return";
						throw "\n";
					}
					else {
						Expr* exp = prepEquation(tokens, &tokensSize, scope, &i); // get the return value as an expression
						exp->evaluate();
						RETURN = exp->value; // set the return value and stop parsing the remaining tokens
						STACK_UNWIND = true; // stop parsing until we exit the function
						return; // the code returns, so do we
					}
				}
				else {
					cerr << "Cannot use \"return\" when not within a function.";
					throw "\n";
				}
			}
			else if (!strcmp(tokens->at(i)->getTokenKey().c_str(), "CONSOLE_IN")) {
				if (i + 1 >= tokensSize) {
					cout << "Too few tokens. Expecting a variable after cin";
					throw "\n";
				}
				else if (!strcmp(tokens->at(i + 1)->getTokenKey().c_str(), "SYMBOL")) {
					Variable** var = p_getKey(&(tokens->at(i + 1)), scope);

					if (!var) {
						cout << "No variable defined called " << tokens->at(i + 1)->getTokenValue();
						throw "\n";
					}
					else {
						string user_input;
						cin >> user_input;
						// cout << user_input << endl; 
						const char** val = p_getValue(&(tokens->at(i + 1)), scope);
						int size = strlen(user_input.c_str()) + 1;
						char* value = (char*)malloc(size*sizeof(char));
						for (int k = 0; k < size; k++) {
							value[k] = user_input.c_str()[k];
						}
						*val = value;
						Variable** val_var = p_getKey(&(tokens->at(i + 1)), scope);
						if (isNaN(value)) {
							(*val_var)->setType("STRING");
						}
						else {
							(*val_var)->setType("NUMBER");
						}
						i += 1;
					}
				}
			}
			else if (!strcmp(tokens->at(i)->getTokenKey().c_str(), "VAR_DEC")) {
				if (i + 2 > tokens->size()) { // If there are not enough tokens for a variable declaration
					cout << "Too few tokens after variable declaration";
					throw "\n";
				}

				// If there is an initialization with a value
				if (!strcmp(tokens->at(i + 1)->getTokenKey().c_str(), "SYMBOL") && !strcmp(tokens->at(i + 2)->getTokenKey().c_str(), "ASSIGNMENT")) {
					int temp = i + 1;
					i += 3;
					//get the equation. For now, just deal with numbers
					string given_type = getType(&tokens->at(temp + 2), scope);
					Expr* exp = new Expr;
					exp = prepEquation(tokens, &tokensSize, scope, &i);
					int size = strlen(tokens->at(temp)->getTokenValue().c_str());
					char* v_name = (char*)malloc(sizeof(char)*size + 1);
					for (int i = 0; i < size + 1; i++) {
						v_name[i] = tokens->at(temp)->getTokenValue().c_str()[i];
					}
					exp->evaluate();
					char* value = (char*)malloc((strlen(exp->value.c_str()) + 1) * sizeof(char));
					char* new_type = (char*)malloc((strlen(given_type.c_str()) + 1) * sizeof(char));
					strcpy(value, exp->value.c_str());
					strcpy(new_type, given_type.c_str());
					// cout << "Value of " << value << " : " << new_type << endl;
					if (scope && scope->size() > 0) { // We can add to the scope
						cout << "Giving local value " << value << " to " << v_name << "\n";
						scope->at(scope->size() - 1).emplace((const char*)v_name, std::make_pair(new Variable((const char*)v_name, new_type), value));
					}
					if (!strcmp("global", tokens->at(temp - 1)->getTokenValue().c_str()) || !scope) {
						cout << "Giving global value " << value << " to " << v_name << "\n";
						globalVariables->insert(new Variable((const char*)v_name, new_type), value);
					}
					// Next, if there is a variable declaration without an initial value
				}
				else if (!strcmp(tokens->at(i + 1)->getTokenKey().c_str(), "SYMBOL") && !strcmp(tokens->at(i + 2)->getTokenKey().c_str(), "SEMI_COLON")) {
					Expr* exp = new Expr;
					exp = new Literal("0", "NUMBER"); // default the value to the int 0
					int size = strlen(tokens->at(i + 1)->getTokenValue().c_str());
					char* v_name = (char*)malloc(sizeof(char)*size + 1);
					for (int k = 0; k < size + 1; k++) {
						v_name[k] = tokens->at(i + 1)->getTokenValue().c_str()[k];
					}
					exp->evaluate();
					char* value = (char*)malloc((strlen(exp->value.c_str()) + 1) * sizeof(char));
					strcpy(value, exp->value.c_str());
					// cout << "Value of " << v_name;
					if (scope && scope->size() > 0) { // We can add to the scope
						scope->at(scope->size() - 1).emplace((const char*)v_name, std::make_pair(new Variable((const char*)v_name, "NUMBER"), value));
					}
					if (!strcmp("global", tokens->at(i)->getTokenValue().c_str()) || !scope) {
						globalVariables->insert(new Variable((const char*)v_name, "NUMBER"), value);
					}
					i += 1;
				}
			}
			else {
				cout << "Expected a symbol, number, string literal or variables decleration. Saw ";
				cout << tokens->at(i)->getTokenValue() << ".";
				throw "\n";
			}
		}
	}
	catch (const char* c) {
		cout << c;
	}
}

static Expr* prepEquation(vector <Token*>* tokens, int* tokensSize, vector <map<string, pair<Variable*, const char*>>>* scope, int* i, string ender) {

	// index of the first token
	const int start_index = *i;

	// Get the end of the equation. This is important for equalities
	// If the equality is incased in parentheses (reaches parens before reaching the end), solve inner first
	// If there is not parentheses, do the whole thing
	if (*i + 1 >= *tokensSize) { // If we run out of tokens, there is a problem
		throw "Incomplete expression. Expected a semi colon or operator.\n";
	}
	else {
		int j = *i;
		int function_call_counter = 0;
		for (; j < *tokensSize + 1; j++) {
			if (j == *tokensSize) {
				throw "missing a ';'\n";
				break;
			}
			// regex match the ender

			if (!strcmp(tokens->at(j)->getTokenKey().c_str(), "FUNC_CALL")) {
				function_call_counter++;
			}

			regex matching(ender);
			if (regex_match(tokens->at(j)->getTokenKey(), matching) && !function_call_counter) {
				break;
			}
			else if (regex_match(tokens->at(j)->getTokenKey(), matching)) {
				function_call_counter--;
			}
		}
		int set = j - 1;
		int check = getPossiblyTypedTokenIndex(tokens, *i);
		// cout << "start: " << set << endl;
		// cout << "end: " << *i - 1 << endl;
		Expr* temp = equation(tokens, scope, &set, *i - 1 , true);
		if (temp == nullptr) {
			throw "Invalid expression\n";
		}
		*i = j;
		return temp;
	}
	return nullptr;
}

static void checkErrors(vector <Token*>* tokens, int index, string type) {
	if (!strcmp(tokens->at(index)->getTokenValue().c_str(), ";")) {
		cout << "Saw ';', Expected a " << type;
		throw "\n";
	}
	else if (!strcmp(tokens->at(index)->getTokenKey().c_str(), "OPERATOR")) {
		cout << "Too many operators. Require a value of type " << type;
		throw "\n";
	}
	else if (strcmp(tokens->at(index)->getTokenKey().c_str(), type.c_str())) {
		cout << "Type mismatch. Need all values to be of type " << type;
		throw "\n";
	}
}

static Expr* getMultDiv(vector <Token*>* tokens, vector <map<string, pair<Variable*, const char*>>>* scope, int* index, int end, bool get_comp) {
	if (*index == end + 1) {
		int i = *index;
		*index = end;
		return new Literal(getValue(&tokens->at(i), scope), getType(&(tokens->at(i)), scope));
	}
	else if (!strcmp(getValue(&(tokens->at(*index - 1)), scope).c_str(), "+") ||
		!strcmp(getValue(&(tokens->at(*index - 1)), scope).c_str(), "-")) {
		int i = *index; // changes where we leave off the index
		*index -= 2;
		return new Literal(getValue(&tokens->at(i), scope), getType(&(tokens->at(i)), scope));
	}
	else if (!strcmp(getValue(&(tokens->at(*index - 1)), scope).c_str(), "*") ||
		!strcmp(getValue(&(tokens->at(*index - 1)), scope).c_str(), "/")) {
		int i = *index;
		*index -= 2;
		return (new Binary(equation(tokens, scope, index, end, get_comp), tokens->at(i - 1),
			new Literal(getValue(&tokens->at(i), scope), getType(&(tokens->at(i)), scope)), getType(&(tokens->at(i)), scope)));
	}
	else {
		cout << *index;
		cout << "saw " << getValue(&(tokens->at(*index - 1)), scope) << ".";
		throw "\n";
	}
}

static Expr* equation(vector <Token*>* tokens, vector <map<string, pair<Variable*, const char*>>>* scope, int* index, int end, bool get_comp) {
	// cout << "start: " << *index << "\nend: " << end << endl;
	int end_right = *index;
	int end_left = end;
	// cout << end_right << endl;
	// cout << end_left << endl;
	try {
		if (*index == end + 1) {
			return new Literal(getValue(&tokens->at((*index)--), scope), getType(&tokens->at(*index + 1), scope)); // Return the literal then decrement the index
		} else if(!strcmp(tokens->at(*index)->getTokenKey().c_str(), "SEMI_COLON")) { // look to see if there is a function call
			int* call_index = new int;
			*call_index = *index;
			for (auto i = tokens->begin() + *index; i != tokens->begin() + end; --i) {
				if (!strcmp((*i)->getTokenKey().c_str(), "FUNC_CALL")) {
					// we have a function call
					*call_index = (*i)->getIndex() - (*(tokens->begin()))->getIndex(); // set the index to start calling the function at
					if (*call_index - 1 > end_left) {
						const int const_call_index = *call_index - 2;
						int stack_call_index = *call_index - 2;
						return (new Binary(equation(tokens, scope, &stack_call_index, end_left, get_comp), tokens->at(const_call_index + 1), new Literal(tokens, scope, call_index, tokens->size(), "NUMBER")));
					}
					else {
						return new Literal(tokens, scope, call_index, tokens->size(), "NUMBER");
					}
				}
			}
			*index -= 1; // if there is no function call, there is a stray semicolon. for now just ignore
			return equation(tokens, scope, index, end);
		} else if (!strcmp(getValue(&tokens->at(*index - 1), scope).c_str(), "*") ||
			!strcmp(getValue(&tokens->at(*index - 1), scope).c_str(), "/")) {
			Expr* multsAndDivs = getMultDiv(tokens, scope, index, end, get_comp);
			if (*index == end) {
				return multsAndDivs;
			}
			return new Binary(equation(tokens, scope, index, end, get_comp),
				tokens->at(*index + 1), multsAndDivs, getType(&(tokens->at(*index + 2)), scope));
		}
		else if (!strcmp(getValue(&tokens->at(*index - 1), scope).c_str(), "-") ||
			!strcmp(getValue(&tokens->at(*index - 1), scope).c_str(), "+")) {
			int i = *index;
			*index -= 2;
			// Check if the next side is in parentheses. Since then, the types would not match
			if (!strcmp("R_PAREN", tokens->at(i - 2)->getTokenKey().c_str())) {
				return new Binary(equation(tokens, scope, index, end, get_comp), tokens->at(i - 1),
					new Literal(getValue(&tokens->at(i), scope), getType(&(tokens->at(i)), scope)), getType(&(tokens->at(i)), scope));
			}
			else {
				Expr* newBin = (new Binary(new Literal(getValue(&tokens->at(i - 2), scope), getType(&(tokens->at(i - 2)), scope)),
					tokens->at(i - 1), new Literal(getValue(&tokens->at(i), scope), getType(&(tokens->at(i)), scope)), getType(&(tokens->at(i)), scope)));

				if (*index <= end + 1) {
					return newBin;
				}
				else if (!strcmp(tokens->at(*index - 1)->getTokenValue().c_str(), "*") ||
					!strcmp(tokens->at(*index - 1)->getTokenValue().c_str(), "/")) {
					return new Binary(equation(tokens, scope, index, end, get_comp),
						tokens->at(i - 1), new Literal(getValue(&tokens->at(i), scope), getType(&(tokens->at(i)), scope)), getType(&(tokens->at(i)), scope));
				}
				else if (!strcmp(tokens->at(*index - 1)->getTokenValue().c_str(), "+") ||
					!strcmp(tokens->at(*index - 1)->getTokenValue().c_str(), "-") ||
					!strcmp(tokens->at(*index - 1)->getTokenValue().c_str(), "==")) {
					return new Binary(equation(tokens, scope, index, end, get_comp),
						tokens->at(i - 1), new Literal(getValue(&tokens->at(i), scope), getType(&(tokens->at(i)), scope)), getType(&(tokens->at(i)), scope));
				}
				else if (!strcmp(tokens->at(*index)->getTokenKey().c_str(), "SEMI_COLON")) { // the next value is a function call
					return new Binary(equation(tokens, scope, index, end, get_comp),
						tokens->at(i - 1), new Literal(getValue(&tokens->at(i), scope), getType(&(tokens->at(i)), scope)), getType(&(tokens->at(i)), scope));
				}
				else {
					cout << "Expected an operator saw " << getValue(&tokens->at(*index - 1), scope) << ".";
					throw "\n";
				}
			}
		}
		else if (compDictionary.lookup(tokens->at(*index - 1)->getTokenValue().c_str()) || logicDictionary.lookup(tokens->at(*index - 1)->getTokenValue().c_str())) {
			int start = *index - 2;
			Expr* return_value = new Binary(equation(tokens, scope, &start, end_left), tokens->at(*index - 1), equation(tokens, scope, &end_right, *index - 1));
			*index = end;
			return return_value;
		}
		else if (!strcmp(tokens->at(*index)->getTokenValue().c_str(), ")")) { // Starting of parentheses
			int para_count = 0;
			int next_end;
			for (int k = *index - 1; k > end; k--) {
				if (!strcmp(tokens->at(k)->getTokenValue().c_str(), ")")) {
					para_count++;
				}
				else if (!strcmp(tokens->at(k)->getTokenValue().c_str(), "(") && para_count == 0) {
					next_end = k;
					// now do the equation stuff
					int before = *index;
					*index = *index - 1;
					Expr* temp = equation(tokens, scope, index, next_end, get_comp);
					if (next_end <= end + 1) {
						*index = end;
						return temp;
					}
					else {
						*index = before;
						int p_next_end = next_end - 2;
						return new Binary(equation(tokens, scope, &p_next_end, end, get_comp), tokens->at(next_end - 1), temp, getType(&(tokens->at(next_end)), scope));
					}
				}
				else if (!strcmp(tokens->at(k)->getTokenValue().c_str(), "(")) {
					para_count--;
				}
			}
		}
		else if (!strcmp(tokens->at(*index)->getTokenValue().c_str(), "COLON")) { // Potential function call inside an expression
			int nested = 0;
			while (*index > end) {
				if (!strcmp(tokens->at(*index)->getTokenKey().c_str(), "FUNC_CALL") && nested == 0) {
					break;
				}
				else if (!strcmp(tokens->at(*index)->getTokenKey().c_str(), "FUNC_CALL")) {
					nested -= 1;
				}
				else if (!strcmp(tokens->at(*index)->getTokenKey().c_str(), "COLON")) {
					nested += 1;
				}
				(*index)--;
			}

			if (*index == end) {
				cerr << "Expected a function call but no call was given.";
				throw "\n";
			}

			int next_end = *index - 1;
			Function* given_function = callFunction(tokens, scope, index, tokens->size());

			// now do the equation stuff
			int before = *index;
			*index = *index - 1;
			if (next_end <= end + 1) {
				*index = end;
				return new Literal(given_function->return_value, "STRING");
			}
			else {
				*index = before;
				int p_next_end = next_end - 2;
				return new Binary(equation(tokens, scope, &p_next_end, end, get_comp), tokens->at(next_end - 1), new Literal(given_function->return_value, "STRING"), getType(&(tokens->at(next_end)), scope));
			}
		}
		else {
			cout << "Expected an operator but saw " << getValue(&tokens->at(*index - 1), scope);
			throw "\n";
		}
	}
	catch (const char* c) {
		cout << c;
		throw nullptr;
	}
	return nullptr;
}