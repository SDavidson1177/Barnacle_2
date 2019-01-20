#include "controlflow.h"
#include <vector>
#include <iostream>

const int FAIL = -1;
const int SUCCESS = 0;

using namespace std;

static struct IfNode* evaluateIfNode(struct IfNode** nd);

// Initialize the IfStatement
struct IfStatement* initIfStatement() {
	struct IfStatement* st = new struct IfStatement;
	st->start = nullptr;
	return st;
}

// Appends an IfNode to the linked list of IfNodes in the given IfStatement
// The given condition is set as the condition of the IfNode.
int addIfNode(struct IfStatement** st, Expr* cond, vector<Token*>* tokens, int start, int end, vector <map<string, pair<Variable*, const char*>>>* inher_scope) {
	try {
		if (!cond) {
			// No valid condition is passed
			cout << "Could not create an IfNode. nullptr condition is passed";
			throw "\n";
		}

		struct IfNode** nd = &((*st)->start);
		while (*nd) {
			nd = &((*nd)->next);
		}
		(*nd) = initIfNode(cond, inher_scope);

		if (!nd) {
			cout << "Error creating an IfNode";
			throw "\n";
		}

		for (auto i = tokens->begin() + start; i != tokens->end() && i != tokens->begin() + end; ++i) {
			(*nd)->tokens.push_back(*i);
		}
	}
	catch (const char* c) {
		cout << c;
		return FAIL;
	}
}

void appendScope(struct IfNode* st, map<string, pair<Variable*, const char*>>* appending_scope) {
	if (appending_scope) {
		st->scope->push_back(*appending_scope);
	}
}

template <class control_flow>
void extendScope(control_flow st, vector <map<string, pair<Variable*, const char*>>>* extending_scope) {
	if (extending_scope) {
		for (auto i = extending_scope->begin(); i != extending_scope->end(); ++i) {
			st->scope.emplace_back(*i);
		}
	}
}

static struct IfNode* evaluateIfNode(struct IfNode** nd) {
	if (!(*nd)) {
		// cout << "nullptr IfNode\n";
		return nullptr;
	}
	(*nd)->condition->evaluate();

	if (!strcmp("1", (*nd)->condition->value.c_str())) {
		return (*nd);
	}
	else {
		return evaluateIfNode(&((*nd)->next));
	}
}

struct IfNode* evaluateIfStatement(struct IfStatement* st) {
	return evaluateIfNode(&(st->start));
}

// Destroy the IfStatement and it's IfNode
void destroyIfStatement(struct IfStatement* is) {
	destroyIfNode(is->start);
	delete is;
}

// Initialize an IfNode
struct IfNode* initIfNode(Expr* cond, vector <map<string, pair<Variable*, const char*>>>* inher_scope) {
	if (!cond) {
		cout << "nullptr condition passed\n";
		return nullptr;
	}
	struct IfNode* nd = new struct IfNode;
	nd->condition = cond;
	nd->scope = inher_scope; // point to the given scope's information
	nd->next = nullptr;
	return nd;
}

// Destroy the IfNode
void destroyIfNode(struct IfNode* nd) {
	if (nd) {
		delete nd->condition;
		delete nd;
	}
}

// While Loops

struct WhileLoop* initWhileLoop(Expr* cond, vector<Token*>* tokens, int start, int end, vector <map<string, pair<Variable*, const char*>>>* inher_scope) {
	if (!cond) {
		cout << "nullptr cond passed\n";
		return nullptr;
	}
	struct WhileLoop* wl = new struct WhileLoop;
	wl->condition = cond;
	wl->scope = inher_scope;

	int size = tokens->size();
	for (auto i = tokens->begin() + start; i != tokens->end() && i != tokens->begin() + end; ++i) {
		wl->tokens.push_back(*i);
	}
	return wl;
}

void destroyWhileLoop(struct WhileLoop* wl){
	if (wl) {
		delete wl->condition;
		delete wl;
	}
}

// Functions

struct Function* initFunction(string given_ID, vector<string>* argv, vector<Token*>* tokens, int start, int end, vector <map<string, pair<Variable*, const char*>>>* inher_scope) {
	struct Function* f = new struct Function;

	f->ID = given_ID;

	if (inher_scope) {
		extendScope(f, inher_scope);
	}
	f->scope.push_back(map<string, pair<Variable*, const char*>>());

	int arg_size = 0;
	if (argv) {
		arg_size = argv->size();
		f->argv = *argv;
	}

	// set the arguments
	f->numOfArgs = arg_size;

	for (int i = 0; i < arg_size; i++) {
		f->scope.at(f->scope.size() - 1).emplace(std::make_pair(argv->at(i).c_str(),std::make_pair(new Variable(argv->at(i).c_str(), "NUMBER"), "0"))); // by default initialize all arguments to 0
	}

	// setting the tokens
	if (tokens) {
		int size = tokens->size();
		for (int i = start; i < size && i < end; i++) {
			f->tokens.push_back(tokens->at(i));
		}
	}
	return f;
}

void setArgument(struct Function** f, int index, std::string value, std::string type) {
	string var_id = (*f)->argv.at(index);
	string* new_val = new string;
	*new_val = value;

	(*(*f)->scope.at((*f)->scope.size() - 1).find(var_id)).second.second = new_val->c_str();
	(*(*f)->scope.at((*f)->scope.size() - 1).find(var_id)).second.first->setType(type);
	// *((*f)->scope.at((*f)->scope.size() - 1)->p_lookup(var_id.c_str())) = new_val->c_str();
	// (*f)->scope.at((*f)->scope.size() - 1)->get(var_id.c_str())->setType(type);
}

void printFunction(struct Function* f) {
	cout << "--function--\n ID: " << f->ID << endl;
	cout << f->numOfArgs << endl;

	for (int i = 0; i < f->numOfArgs; i++) {
		cout << "argument " << i << ":" << " " << f->argv.at(i) << " : " << f->scope.at(f->scope.size() - 1).find(f->argv.at(i))->first << endl;
	}

	int size = f->tokens.size();
	for (int i = 0; i < size; i++) {
		f->tokens.at(i)->coutToken();
	}
}


void destroyFunction(struct Function* f) {
	if (f) {
		delete f;
	}
}

