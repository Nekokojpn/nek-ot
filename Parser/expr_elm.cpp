#include "../nek-ot.hpp"

AST* Parser::expr_lb() {
	getNextToken();
	if (nexttokIs(TK::tok_colon)) {
		//ASTStctElements
		auto ast = expr_stct();
		return ast;
	}
	else {
		auto ast = expr_arr();
		return ast;
	}
	return nullptr;
}