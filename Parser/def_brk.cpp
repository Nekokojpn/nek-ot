#include "../nek-ot.h"

std::unique_ptr<ASTBrk> Parser::def_brk() {
	auto loc = curtok.loc;
	getNextToken();
	if (curtok.ty != TK::tok_semi)
		error_expected(";", curtok);
	auto ast = std::make_unique<ASTBrk>();
	ast->loc = loc;
	return std::move(ast);
}