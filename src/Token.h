#pragma once
#include <string>

class Token {
private:
	std::string key;
	std::string value;
public:
	Token(std::string p_key, std::string p_value);

	~Token();

	void setToken(std::string p_key, std::string p_value);

	std::string getTokenKey();

	std::string getTokenValue();

	void coutToken();

	bool isNull();
};