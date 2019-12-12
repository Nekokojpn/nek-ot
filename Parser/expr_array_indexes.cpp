#include "../nek-ot.hpp"

AST* Parser::expr_array_indexes() {
	getNextToken();
	auto lhs = expr();
	if (!curtokIs(TK::tok_rpb))
		error_unexpected(curtok);
	getNextToken();
	bool isRhs = false;
	while (curtokIs(TK::tok_lpb)) {
		isRhs = true;
		getNextToken(); //eat [
		auto rhs = expr();
		if (!curtokIs(TK::tok_rpb))
			error_unexpected(curtok);
		getNextToken();
		lhs = new ASTArrayIndexes(lhs, rhs);
	}
	if (isRhs)
		return lhs;
	else
		return new ASTArrayIndexes(lhs, nullptr);
}