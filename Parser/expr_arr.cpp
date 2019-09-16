#include "../nek-ot.h"

std::unique_ptr<ASTArrElements> Parser::expr_arr() {
	getNextToken();
	std::vector<std::unique_ptr<AST>> elements;
	while (true) {
		elements.push_back(std::move(expr()));
		if (curtok.ty != TK::tok_comma) break;
		getNextToken();
	}
	if (curtok.ty != TK::tok_rb)
		error("Expected", "Expected --> }", curtok);
	getNextToken();
	return std::move(std::make_unique<ASTArrElements>(std::move(elements)));
}