#include "Parser.h"
#include <assert.h>
#include <iostream>
#include <regex>

static Expr* equation(vector <Token*>* tokens, vector<VarDictionary*>* scope, int* index, int end, bool get_comp=true);
static Expr* prepEquation(vector <Token*>* tokens, int* tokensSize, vector<VarDictionary*>* scope, int* i, string ender="SEMI_COLON");
VarDictionary* globalVariables = new VarDictionary();

// Important global variables
LangFuncDictionary* functions = new LangFuncDictionary();

// Detection of in in function or in class
bool IN_FUNCTION = false;

string getType(Token** lookup, vector <VarDictionary*>* scope) {
	if (!strcmp((*lookup)->getTokenKey().c_str(), "SYMBOL")) {
		Variable* var = nullptr;
		if (scope) { // try to find the variable by checking the scope of nested functions/loops/statements
			int depth = scope->size() - 1;
			while (depth > -1 && !var) {
				var = scope->at(depth)->get((*lookup)->getTokenValue().c_str());
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
	return (*lookup)->getTokenKey();
}

string getValue(Token** lookup, vector <VarDictionary*>* scope) {
	if (!strcmp((*lookup)->getTokenKey().c_str(), "SYMBOL")) {
		const char* var = nullptr;
		if (scope) { // try to find the variable by checking the scope of nested functions/loops/statements
			int depth = scope->size() - 1;
			while (depth > -1 && !var){
				var = scope->at(depth)->lookup((*lookup)->getTokenValue().c_str());
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

Variable* getKey(Token** lookup, vector <VarDictionary*>* scope) {
	if (!strcmp((*lookup)->getTokenKey().c_str(), "SYMBOL")) {
		Variable** var = nullptr;
		if (scope) { // try to find the variable by checking the scope of nested functions/loops/statements
			int depth = 0;
			int max_depth = scope->size();
			while (depth < max_depth && !var) {
				*var = scope->at(depth)->get((*lookup)->getTokenValue().c_str());
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

const char** p_getValue(Token** lookup, vector <VarDictionary*>* scope) {
	if (!strcmp((*lookup)->getTokenKey().c_str(), "SYMBOL")) {
		const char** var = nullptr;
		if (scope) { // try to find the variable by checking the scope of nested functions/loops/statements
			int depth = 0;
			int max_depth = scope->size();
			while (depth < max_depth && !var) {
				var = scope->at(depth)->p_lookup((*lookup)->getTokenValue().c_str());
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

Variable** p_getKey(Token** lookup, vector <VarDictionary*>* scope) {
	if (!strcmp((*lookup)->getTokenKey().c_str(), "SYMBOL")) {
		Variable** var = nullptr;
		if (scope) { // try to find the variable by checking the scope of nested functions/loops/statements
			int depth = 0;
			int max_depth = scope->size();
			while (depth < max_depth && !var) {
				var = scope->at(depth)->p_get((*lookup)->getTokenValue().c_str());
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

void getArguments(Function **func, vector<Token*>* tokens, vector <VarDictionary*>* scope, int *index) {
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

/*Seems as though we can ignore the index variable*/
void parse(vector<Token*>* tokens, vector <VarDictionary*>* scope) {
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
				i++;
				if (i < tokensSize) {
					Function* given_function = nullptr;
					given_function = functions->get(getValue(&tokens->at(i), scope).c_str());
					Function &func_ref = *given_function;
					// If we have a valid function name given
					if (!strcmp(tokens->at(i)->getTokenKey().c_str(), "SYMBOL") && given_function != nullptr) {
						// Get all arguments and pass it to the function for evaluation.
						getArguments(&given_function, tokens, scope, &i);
						parse(&(given_function->tokens), &(given_function->scope));
					}
					else {
						cout << "Expected a function name given for call.";
					}
				}
				else {
					cout << "Too few tokens for function call";
					throw "\n";
				}
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
					strcpy_s(value, (strlen(exp->value.c_str()) + 1)*sizeof(char), exp->value.c_str());
					strcpy_s(new_type, (strlen(given_type.c_str()) + 1) * sizeof(char), given_type.c_str());
					// cout << "Value of " << value << " : " << new_type << endl;
					if (scope && scope->at(0)) { // We can add to the scope
						scope->at(scope->size() - 1)->insert(new Variable((const char*)v_name, new_type), value);
					}
					if (!strcmp("global", tokens->at(temp - 1)->getTokenValue().c_str()) || !scope) {
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
					strcpy_s(value, (strlen(exp->value.c_str()) + 1) * sizeof(char), exp->value.c_str());
					// cout << "Value of " << v_name;
					if (scope && scope->at(0)) { // We can add to the scope
						scope->at(scope->size() - 1)->insert(new Variable((const char*)v_name, "NUMBER"), value);
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

static Expr* prepEquation(vector <Token*>* tokens, int* tokensSize, vector<VarDictionary*>* scope, int* i, string ender) {

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
		for (; j < *tokensSize + 1; j++) {
			if (j == *tokensSize) {
				throw "missing a ';'\n";
				break;
			}
			// regex match the ender

			regex matching(ender);
			if (regex_match(tokens->at(j)->getTokenKey(), matching)) {
				break;
			}
		}
		int set = j - 1;
		int check = getPossiblyTypedTokenIndex(tokens, *i);
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

static Expr* getMultDiv(vector <Token*>* tokens, vector<VarDictionary*>* scope, int* index, int end, bool get_comp) {
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

static Expr* equation(vector <Token*>* tokens, vector<VarDictionary*>* scope, int* index, int end, bool get_comp) {
	// cout << "start: " << *index << "\nend: " << end << endl;
	int end_right = *index;
	int end_left = end;
	try {
		if (*index == end + 1) {
			return new Literal(getValue(&tokens->at((*index)--), scope), getType(&tokens->at(*index + 1), scope)); // Return the literal then decrement the index
		}else if (!strcmp(getValue(&tokens->at(*index - 1), scope).c_str(), "*") ||
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
		else if (!strcmp(tokens->at(*index)->getTokenValue().c_str(), ")")) {
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