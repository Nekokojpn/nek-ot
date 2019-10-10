#include "../nek-ot.hpp"

std::unique_ptr<ASTGoto> Parser::expr_goto() {
	getNextToken();
	if (curtok.ty != TK::tok_identifier)
		error_unexpected(curtok);
	auto loc = curtok.loc;
	auto ast = std::make_unique<ASTGoto>(curtok.val);
	ast->loc = loc;
	getNextToken();
	if (curtok.ty != TK::tok_semi)
		error_expected(";", curtok);
	return std::move(ast);
}