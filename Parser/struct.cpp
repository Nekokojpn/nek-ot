#include "../nek-ot.h"
std::unique_ptr<AST> expr_stct() {
	return nullptr;
}

std::unique_ptr<AST> Parser::def_stct() {
	getNextToken();
	if (curtok.ty != TK::tok_lb) {
		error_unexpected(curtok);
	}
	getNextToken();
	while (curtok.ty != TK::tok_rb) {

	}
	return nullptr;
}