#include "../nek-ot.hpp"

bool Parser::curtokIs(TK tok) {
	return curtok.ty == tok;
}