#include "../nek-ot.hpp"

AST* Parser::expr_star() {
	getNextToken();
	//Pointer to val type
	if (curtok.ty == TK::tok_identifier) {
		auto id = expr_identifiers();
		getNextToken();
		auto loc = curtok.loc;
		auto ast = subst_expr(id);
		ast->loc = loc;
		return ast;
	}
	return nullptr;
}