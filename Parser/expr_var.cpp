#include "../nek-ot.hpp"

AST* Parser::expr_var() {
	if (!curtokIs(TK::tok_identifier))
		error_unexpected(curtok);
	auto name = curtok.val;
	getNextToken();
	//If array ty
	if (curtokIs(TK::tok_lpb)) {
		auto loc = curtok.loc;
		auto index = expr_array_indexes();
		auto ast = new ASTIdentifierArrayElementBase(name, index);
		ast->loc = loc;
		return ast;
	}
	//If var ty
	else {
		auto loc = curtok.loc;
		auto ast = new ASTIdentifierBase(name);
		ast->loc = loc;
		return ast;
	}
}