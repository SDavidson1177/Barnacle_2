#pragma once
#include <string>

class Variable {
private:
	std::string key;
	std::string type;
public:
	Variable(const char* p_key, const char* p_type);
	~Variable();

	void print();
	std::string getKey();
	std::string getType();
	void setType(std::string new_type);
};
