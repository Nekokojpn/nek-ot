#include "../nek-ot.hpp"

std::unique_ptr<AST> Parser::expr_array_indexes() {
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
		lhs = std::make_unique<ASTArrayIndexes>(std::move(lhs), std::move(rhs));
	}
	if (isRhs)
		return std::move(lhs);
	else
		return std::make_unique<ASTArrayIndexes>(std::move(lhs), nullptr);
}