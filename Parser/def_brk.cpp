#include "../nek-ot.hpp"

ASTBrk* Parser::def_brk() {
	auto loc = curtok.loc;
	getNextToken();
	if (curtok.ty != TK::tok_semi)
		error_expected(";", curtok);
	auto ast = new ASTBrk();
	ast->loc = loc;
	return ast;
}