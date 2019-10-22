#include "../nek-ot.hpp"

std::unique_ptr<AST> Parser::expr_star() {
	getNextToken();
	//Pointer to val type
	if (curtok.ty == TK::tok_identifier) {
		auto id = expr_identifiers();
		getNextToken();
		auto loc = curtok.loc;
		auto ast = subst_expr(std::move(id));
		ast->loc = loc;
		return std::move(ast);
	}
	return nullptr;
}