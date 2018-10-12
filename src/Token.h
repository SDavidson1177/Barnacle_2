#pragma once
#include <string>

class Token {
private:
	std::string key;
	std::string value;
	int index;
public:
	Token(std::string p_key, std::string p_value, int p_index);

	~Token();

	void setToken(std::string p_key, std::string p_value, int p_index);

	std::string getTokenKey();

	std::string getTokenValue();

	void coutToken();

	bool isNull();
};