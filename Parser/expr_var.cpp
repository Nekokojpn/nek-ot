#include "../nek-ot.hpp"

std::unique_ptr<AST> Parser::expr_var() {
	if (!curtokIs(TK::tok_identifier))
		error_unexpected(curtok);
	auto name = curtok.val;
	getNextToken();
	//If array ty
	if (curtokIs(TK::tok_lpb)) {
		auto index = expr_array_indexes();
		return std::make_unique<ASTIdentifierArrayElementBase>(name, std::move(index));
	}
	//If var ty
	else {
		return std::make_unique<ASTIdentifierBase>(name);
	}
}