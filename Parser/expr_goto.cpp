#include "../nek-ot.hpp"

ASTGoto* Parser::expr_goto() {
	getNextToken();
	if (curtok.ty != TK::tok_identifier)
		error_unexpected(curtok);
	auto loc = curtok.loc;
	auto ast = new ASTGoto(curtok.val);
	ast->loc = loc;
	getNextToken();
	if (curtok.ty != TK::tok_semi)
		error_expected(";", curtok);
	return ast;
}