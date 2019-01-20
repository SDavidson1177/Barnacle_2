#include "languageio.h"
#include "StringDictionary.h"
#include <fstream>
#include <iostream>
#include <string>
#include <string.h>
#include <cstdlib>
#include <cstdio>
#include <assert.h>

const char* NO_STRING = "no_string";
const int NO_STRING_LEN = 10;
int COUNT = 0;

// Declaration for important global variables-------
static StringDictionary stopSymbols = StringDictionary(); // every character that will indicate the end of a token
static StringDictionary specialSymbols = StringDictionary();
static StringDictionary stringCharacters = StringDictionary(); // characters that indicate when to read a token until
															   // the closing char (Ex. "this is a token")
//------------------------------------------------------
// Other important variables
StringDictionary compDictionary = StringDictionary();
StringDictionary logicDictionary = StringDictionary();

void removeQuotes(char** str) {
	int size = strlen(*str);
	for (int i = 0; i < size - 2; i++) {
		(*str)[i] = (*str)[i + 1];
	}
	(*str)[size - 2] = 0;
}

void initInterpreter() {
	// Initialize important global variables
	stopSymbols.insert("-", "DASH");
	stopSymbols.insert(";", "SEMI_COLON");
	stopSymbols.insert(":", "COLON");
	stopSymbols.insert("(", "L_PAREN");
	stopSymbols.insert(")", "R_PAREN");
	stopSymbols.insert("{", "L_BRACE");
	stopSymbols.insert("}", "R_BRACE");
	stopSymbols.insert("=", "ASSIGNMENT");
	stopSymbols.insert("\"", "STRING");
	stopSymbols.insert("\'", "STRING");
	stopSymbols.insert("/", "OPERATOR");
	stopSymbols.insert("-", "OPERATOR");
	stopSymbols.insert("+", "OPERATOR");
	stopSymbols.insert("*", "OPERATOR");
	stopSymbols.insert("|", "COMPARE");
	stopSymbols.insert(",", "COMMA");

	specialSymbols.insert("<-", "L_ARROW");
	specialSymbols.insert(".", "DOT_OP");
	specialSymbols.insert(",", "COMMA");
	specialSymbols.insert(":", "COLON");
	specialSymbols.insert("-", "OPERATOR");
	specialSymbols.insert("--", "DECREMENT");
	specialSymbols.insert(";", "SEMI_COLON");
	specialSymbols.insert("->", "R_ARROW");
	specialSymbols.insert("+", "OPERATOR");
	specialSymbols.insert("++", "INCREMENT");
	specialSymbols.insert("/", "OPERATOR");
	specialSymbols.insert("var", "VAR_DEC");
	specialSymbols.insert("global", "VAR_DEC");
	specialSymbols.insert("return", "RETURN");
	specialSymbols.insert("cin", "CONSOLE_IN");
	specialSymbols.insert("cout", "CONSOLE_OUT");
	specialSymbols.insert("endl", "NEW_LINE"); // ERROR here causing endlooop instead of endloop
	specialSymbols.insert("function", "FUNC_DEF");
	specialSymbols.insert("call", "FUNC_CALL");
	specialSymbols.insert("*", "OPERATOR");
	specialSymbols.insert("=", "ASSIGNMENT");
	specialSymbols.insert("==", "EQUALITY");
	specialSymbols.insert("!", "NOT");
	specialSymbols.insert("!=", "INEQUALITY");
	specialSymbols.insert("<", "LESS_THAN");
	specialSymbols.insert(">", "GREATER_THAN");
	specialSymbols.insert(">=", "GREATER_THAN_OR_EQUAL");
	specialSymbols.insert("<=", "LESS_THAN_OR_EQUAL");
	specialSymbols.insert("||", "OR");
	specialSymbols.insert("&&", "AND");
	specialSymbols.insert("|", "COMPARE");
	specialSymbols.insert("(", "L_PAREN");
	specialSymbols.insert(")", "R_PAREN");

	stringCharacters.insert("\"", "STRING");
	stringCharacters.insert("\'", "STRING");

	// The comparison dictionary
	compDictionary.insert("==", "EQUALS");
	compDictionary.insert("!=", "EQUALS");
	compDictionary.insert("|", "COMPARE");
	compDictionary.insert("<", "LESS_THAN");
	compDictionary.insert(">", "GREATER_THAN");
	compDictionary.insert("<=", "LESS_THAN_OR_EQUAL");
	compDictionary.insert(">=", "GREATER_THAN_OR_EQUAL");

	// The logic dictionary
	logicDictionary.insert("||", "OR");
	logicDictionary.insert("&&", "AND");
}

static void getSpecialSymbols(FILE** f, vector <Token*>* a, char** token, int* len, int* maxlen, int &index, char** stringChar=nullptr) {
	char c;

	//Handle the case for special symbols
	const char* special = specialSymbols.lookup(*token);
	if (special != nullptr) {
			if (fscanf(*f, "%c", &c) == 1 && c != ' ' && c != '\n' && c != '\t') {
				int size = strlen(*token);
				char* bonus = (char*)malloc((2 + size)*sizeof(char));
				strcpy(bonus, *token);
				bonus[size] = c;
				bonus[size + 1] = 0;
				const char* special_bonus = specialSymbols.lookup(bonus);
				if (special_bonus != nullptr) {
					a->push_back(new Token(special_bonus, bonus, index));
					index++;
					*token = (char*)realloc(*token, 2 * sizeof(char));
					*len = 1;
					*maxlen = 2;
					(*token)[0] = 0;
				}
				else {
					if (c == ' ' || c == '\n') {
						a->push_back(new Token(special, *token, index));
						index++;
						*token = (char*)realloc(*token, 2 * sizeof(char));
						*len = 1;
						*maxlen = 2;
						(*token)[0] = 0;
					}
					else {
						// possible string character
						char val[] = { c, 0 };
						char* string_char = (char*)stringCharacters.lookup(val); // get the possible string character
						bool is_string_char = (string_char != nullptr); // is it a string character boolean value
						if (is_string_char && !strcmp(*stringChar, NO_STRING)) { // if it starts a string where a string has not yet been started
							a->push_back(new Token(special, *token, index)); // push back the token we have
							index++;
							strcpy(*stringChar, string_char); // change stringChar to new string character
							*token = (char*)realloc(*token, 4 * sizeof(char));
							*len = 2;
							*maxlen = 4;
							(*token)[0] = c;
							(*token)[1] = 0;
							getSpecialSymbols(f, a, token, len, maxlen, index, stringChar);
						}
						else if (stopSymbols.lookup(*token) || compDictionary.lookup(*token)) {
							a->push_back(new Token(special, *token, index)); // push back the token we have
							index++;
							*token = (char*)realloc(*token, 4 * sizeof(char));
							*len = 2;
							*maxlen = 4;
							(*token)[0] = c;
							(*token)[1] = 0;
							getSpecialSymbols(f, a, token, len, maxlen, index, stringChar);
						}
						// Do nothing if nothing else is satisfied. The special symbol may be a small part of another symbol
					}
				}
		}
		else {
			a->push_back(new Token(special, *token, index));
			index++;
			*token = (char*)realloc(*token, 2 * sizeof(char));
			*len = 1;
			*maxlen = 2;
			(*token)[0] = 0;
		}
	}
	//------------------------------------------
}

void readTokens(const char* file, vector <Token*>* a, int &index) {
	FILE* f;
	f = fopen(file,  "r");
	char c;
	char* token = (char*)malloc(2*sizeof(char));
	char* stringChar = (char*)malloc(NO_STRING_LEN*sizeof(char));
	strcpy(stringChar, NO_STRING);
	int len = 1;
	int maxlen = 2;
	//Inital additions to the token.
	if (fscanf(f, " %c", &c) != 1) {
		free(token);
		return;
	}
	else {
		token[0] = 0;
	}

	if (f) {
		do {
			char val[] = { c , '\0' };
			bool is_stop_char = (stopSymbols.lookup(val) != nullptr);
			char* string_char = (char*)stringCharacters.lookup(val);
			bool is_string_char = (string_char != nullptr);
			if (!strcmp(stringChar, NO_STRING) && (c == ' ' || c == '\n' || c == '\t' || is_stop_char || is_string_char)) {
				if (len > 1) {
					// ERROR NOT HERE
					if (!isNaN(token)) {
						a->push_back(new Token("NUMBER", token, index));
						index++;
					}
					else if (!strcmp("true", token) || !strcmp("false", token)) {
						a->push_back(new Token("BOOLEAN", token, index));
						index++;
					}
					else {
						const char* key = stopSymbols.lookup(token);
						if (!key) {
							key = specialSymbols.lookup(token);
						}
						a->push_back(new Token(!key ? "SYMBOL" : key, token, index));
						index++;
					}
				}

				if (is_string_char) {
					strcpy(stringChar, string_char);
					token = (char*)realloc(token, 4 * sizeof(char));
					// CHANGE later to not add string character at the beginning of the token
					token[0] = c;
					token[1] = 0;
					len = 2;
					maxlen = 4;
				}else if (is_stop_char) {
					token = (char*)realloc(token, 4 * sizeof(char));
					token[0] = c;
					token[1] = 0;
					len = 2;
					maxlen = 4;
					getSpecialSymbols(&f, a, &token, &len, &maxlen, index, &stringChar);

				}
				else {
					token = (char*)realloc(token, 2 * sizeof(char));
					len = 1;
					maxlen = 2;
					token[0] = 0;
				}
			}
			else {
				if (len >= maxlen) {
					maxlen *= 2;
					token = (char*)realloc(token, maxlen * sizeof(char));
				}
				token[len - 1] = c;
				token[len] = 0;
				len++;
				if (is_string_char && !strcmp(stringChar, string_char)) {
					removeQuotes(&token);
					a->push_back(new Token(string_char, token, index));
					index++;
					stringChar = (char*)realloc(stringChar, (NO_STRING_LEN + 1) * sizeof(char));
					strcpy(stringChar, NO_STRING);
					token = (char*)realloc(token, 2*sizeof(char));
					len = 1;
					maxlen = 2;
					token[0] = 0;
				}
				else {
					getSpecialSymbols(&f, a, &token, &len, &maxlen, index, &stringChar);
				}
			}
		} while (fscanf(f, "%c", &c) == 1);
		if (len > 1) {
			if (!isNaN(token)) {
				a->push_back(new Token("NUMBER", token, index));
				index++;
			}
			else if (!strcmp("true", token) || !strcmp("false", token)) {
				a->push_back(new Token("BOOLEAN", token, index));
				index++;
			}
			else {
				const char* key = stopSymbols.lookup(token);
				if (!key) {
					key = specialSymbols.lookup(token);
				}
				a->push_back(new Token(!key ? "SYMBOL" : key, token, index));
				index++;
			}
		}
		free(token);
		fclose(f);
	}
}

char strToNum(const char* str, void** s) {
	assert(str);

	char res = 'i';
	float negator = 1.0;
	int num = 0;
	float numf = 0.0;
	int decimalMag = 1;
	int size = strlen(str);

	for (int i = 0; i < size; i++) {
		if (str[i] == '.' && res != 'f') {
			res = 'f';
			numf = (float)num;
			continue;
		}
		else if (str[i] == '-' && i == 0) {
			negator = -1.0;
			continue;
		}
		else if (str[i] < '0' || str[i] > '9') {
			break;
		}

		if (res == 'f') {
			decimalMag *= 10;
			numf += ((float)(str[i] - '0') / (float)decimalMag)*negator;
		}
		else {
			num *= 10;
			num += (str[i] - '0')*(int)negator;
		}
	}


	if (res == 'i') {
		int* temp = (int*)malloc(sizeof(int));
		temp[0] = num;
		*s = temp;
		//cout << ((int*)(*s))[0] << endl;
	}
	else if (res == 'f') {
		float* temp = (float*)malloc(sizeof(int));
		temp[0] = numf;
		*s = temp;
	}

	return res;
}

bool isNaN(const char* str) {
	bool has_decimal = false;
	int size = strlen(str);

	int i = 0;

	while (str[i] == ' ' || str[i] == '	' || str[i] == '\n') {
		i++;
	}

	for (int i = 0; i < size; i++) {
		if (str[i] == '-' && i != 0) {
			return true;
		}
		else if (str[i] == '.' && !has_decimal) {
			has_decimal = true;
		}
		else if (str[i] < '0' || str[i] > '9') {
			return true;
		}
	}

	return false;
}

