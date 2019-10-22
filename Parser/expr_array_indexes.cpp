#include "../nek-ot.hpp"

std::unique_ptr<AST> Parser::expr_array_indexes() {
	getNextToken();
	auto lhs = expr();
	if (!curtokIs(TK::tok_rpb))
		error_unexpected(curtok);
	getNextToken();
	while (curtokIs(TK::tok_lpb)) {
		getNextToken(); //eat [
		auto rhs = expr();
		if (!curtokIs(TK::tok_rpb))
			error_unexpected(curtok);
		getNextToken();
		lhs = std::make_unique<ASTArrayIndexes>(std::move(lhs), std::move(rhs));
	}
	return std::move(lhs);
}