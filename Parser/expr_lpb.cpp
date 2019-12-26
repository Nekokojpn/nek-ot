#include "../nek-ot.hpp"

AST* Parser::expr_lpb() {
	getNextToken(); //eat [
	std::vector<AST*> elems;
	while (!curtokIs(TK::tok_rpb)) {
		elems.push_back(expr());
	}

}