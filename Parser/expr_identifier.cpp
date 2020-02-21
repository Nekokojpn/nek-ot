#include "../nek-ot.hpp"

AST* Parser::expr_identifiers() {
	//@Build identifier ast (Ex. array, stct.)
	// identifier [] [] [] . call()
	auto lhs = expr_var();
	while (curtokIs(TK::tok_dot) || curtokIs(TK::tok_lpb)) {
		if(curtokIs(TK::tok_dot))
			getNextToken();
		auto loc = curtok.loc;
		auto rhs = expr_var();
		if (lhs->getASTType() == TypeAST::IdentifierBase) {
			if (rhs->getASTType() == TypeAST::VarOp)
				((ASTVarOp*)rhs)->name = ((ASTIdentifierBase*)lhs)->name;
		}
		lhs = new ASTIdentifier(lhs, rhs, TypeKind::Pointer);
		lhs->loc = loc;
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
		auto loc = curtok.loc;
		auto ast_value = new ASTValue(delta);
		ast_value->loc = loc;
		auto ast_bin = new ASTBinOp(lhs, Op::Plus, ast_value);
		ast_bin->loc = loc;
		lhs = new ASTSubst(lhs, ast_bin);
		lhs->loc;
	}
	return lhs;
}