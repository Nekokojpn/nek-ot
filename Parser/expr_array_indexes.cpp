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
		auto ast_arr = new ASTArrayIndexes(lhs, rhs);
		ast_arr->loc = curtok.loc;
		lhs = ast_arr;
	}
	if (isRhs)
		return lhs;
	else {
		auto ast_arr = new ASTArrayIndexes(lhs, nullptr);
		ast_arr->loc = curtok.loc;
		return ast_arr;
	}
}