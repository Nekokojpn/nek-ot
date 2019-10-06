#include "../nek-ot.h"

std::unique_ptr<AST> Parser::expr_star() {
	getNextToken();
	//Pointer to val type
	if (curtok.ty == TK::tok_identifier) {
		auto id = curtok.val;
		getNextToken();
		auto loc = curtok.loc;
		auto ast = subst_expr(id, TypeKind::Pointer);
		ast->loc = loc;
		return std::move(ast);
	}
	return nullptr;
}