#include "../nek-ot.hpp"

std::unique_ptr<AST> Parser::expr_identifiers() {
	//Å@Build identifier ast (Ex. array, stct.)
	auto lhs = expr_var();
	while (curtokIs(TK::tok_dot)) {
		getNextToken();
		auto rhs = expr_var();
		lhs = std::make_unique<ASTIdentifier>(std::move(lhs), std::move(rhs), TypeKind::Pointer);
	}
	return std::move(lhs);
}