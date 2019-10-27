#include "../nek-ot.hpp"

std::unique_ptr<ASTSubst> Parser::subst_expr(std::unique_ptr<AST> ast_id) {
	//Subst------------------->
	if (curtok.ty == TK::tok_equal) {
		getNextToken();
		auto loc = curtok.loc;
		auto ast = std::make_unique<ASTSubst>(std::move(ast_id), std::move(expr_typeinf()));
		ast->loc = loc;
		if (curtok.ty != TK::tok_semi)
			error_unexpected(curtok);
		return std::move(ast);
	}
	//<--------------------Subst
	/*
	else if (curtok.ty == TK::tok_plpl || curtok.ty == TK::tok_mimi) {
		auto loc = curtok.loc;
		auto ast = std::make_unique<ASTSubst>(std::move(ast_id), std::move(expr()));
		ast->loc = loc;
		if (curtok.ty != TK::tok_semi)
			error("Expected", "Expected token --> ;", curtok);
		getNextToken();
		return std::move(ast);
	}
	*/
	else
		error("Unexpected", "Unexpected token -->" + curtok.val, curtok);
	return nullptr;
}
