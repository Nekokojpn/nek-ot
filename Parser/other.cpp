#include "../nek-ot.hpp"

bool Parser::curtokIs(TK tok) {
	return curtok.ty == tok;
}

bool Parser::nexttokIs(TK tok) {
	if (index + 1 < tokens.size())
		return tokens[index+1].ty == tok;
	else
		return false;
}

void Parser::addUserdefinedType(std::string name) {
	this->userdefinedtype_v.push_back(name);
}

bool Parser::isUserdefinedType(std::string name) {
	for (int i = 0; i < this->userdefinedtype_v.size(); i++)
		if (name == this->userdefinedtype_v[i])return true;
	return false;
}