#include "../nek-ot.hpp"

ASTSubst* Parser::subst_expr(AST* ast_id) {
	//Subst------------------->
	if (curtok.ty == TK::tok_equal) {
		getNextToken();
		auto loc = curtok.loc;
		auto ast = new ASTSubst(ast_id, expr());
		ast->loc = loc;

		if (this->isExpectedSemi && curtok.ty != TK::tok_semi)
			error_expected(";", curtok);
		if (this->isExpectedSemi)getNextToken();
		this->isExpectedSemi = true;
		return ast;
	}
	//<--------------------Subst
	else
		error_unexpected(curtok);
	return nullptr;
}
