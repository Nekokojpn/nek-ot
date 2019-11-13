#include "../nek-ot.hpp"

std::unique_ptr<AST> Parser::expr_identifiers() {
	//Å@Build identifier ast (Ex. array, stct.)
	auto lhs = expr_var();
	while (curtokIs(TK::tok_dot)) {
		getNextToken();
		auto rhs = expr_var();
		lhs = std::make_unique<ASTIdentifier>(std::move(lhs), std::move(rhs), TypeKind::Pointer);
	}
	if (curtokIs(TK::tok_plpl)) {
		getNextToken();
		lhs = std::make_unique<ASTBinOp>(std::move(lhs), Op::Plus, std::make_unique<ASTValue>(1));
	}
	else if (curtokIs(TK::tok_mimi)) {
		getNextToken();
		lhs = std::make_unique<ASTBinOp>(std::move(lhs), Op::Minus, std::make_unique<ASTValue>(1));
	}
	return std::move(lhs);
}