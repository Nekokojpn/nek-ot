#include "../nek-ot.hpp"

AST* Parser::expr_identifiers() {
	//Å@Build identifier ast (Ex. array, stct.)
	auto lhs = expr_var();
	while (curtokIs(TK::tok_dot)) {
		getNextToken();
		auto rhs = expr_var();
		lhs = new ASTIdentifier(lhs, rhs, TypeKind::Pointer);
	}
	auto delta = 0;
	while (true) {
		if (curtokIs(TK::tok_plpl))
			delta++;
		else if (curtokIs(TK::tok_mimi))
			delta--;
		else
			break;
		getNextToken();
	}
	if (delta != 0) {
		lhs = new ASTSubst(lhs, new ASTBinOp(lhs, Op::Plus, new ASTValue(delta)));
	}
	return lhs;
}