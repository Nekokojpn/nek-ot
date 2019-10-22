#include "../nek-ot.hpp"

std::unique_ptr<AST> Parser::expr_anno() {
	getNextToken();
	if (curtok.ty == TK::tok_identifier) {
		auto id = curtok.val;
		getNextToken();
		if (curtok.ty == TK::tok_colon) {
			auto ast = def_label(id);
			return ast;
		}
	}
	return nullptr;
}