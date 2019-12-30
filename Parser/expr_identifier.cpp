#include "../nek-ot.hpp"

AST* Parser::expr_identifiers() {
	//Å@Build identifier ast (Ex. array, stct.)
	auto lhs = expr_var();
	while (curtokIs(TK::tok_dot)) {
		getNextToken();
		auto rhs = expr_var();
		lhs = new ASTIdentifier(lhs, rhs, TypeKind::Pointer);
	}
	if (curtokIs(TK::tok_plpl)) {
		getNextToken();
		lhs = new ASTSubst(lhs, new ASTBinOp(lhs, Op::Plus, new ASTValue(1)));
	}
	else if (curtokIs(TK::tok_mimi)) {
		getNextToken();
		lhs = new ASTSubst(lhs, new ASTBinOp(lhs, Op::Minus, new ASTValue(1)));
	}
	return lhs;
}