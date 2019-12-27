#include "../nek-ot.hpp"

AST* Parser::expr_lpb() {
	auto loc = curtok.loc;
	getNextToken(); //eat [
	std::vector<AST*> elems;
	while (true) {
		elems.push_back(expr());
		if (curtokIs(TK::tok_rpb))
			break;
		if (!curtokIs(TK::tok_comma))
			error_expected(", or ]", curtok);
		getNextToken();
	}
	getNextToken(); //eat ]
	auto ast = new ASTListElements(elems);
	ast->loc = loc;
	return ast;
}