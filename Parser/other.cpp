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