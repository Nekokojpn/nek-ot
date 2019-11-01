#include "../nek-ot.hpp"

std::unique_ptr<ASTSubst> Parser::subst_expr(std::unique_ptr<AST> ast_id) {
	//Subst------------------->
	if (curtok.ty == TK::tok_equal) {
		getNextToken();
		auto loc = curtok.loc;
		auto ast = std::make_unique<ASTSubst>(std::move(ast_id), std::move(expr()));
		ast->loc = loc;
		if (curtok.ty != TK::tok_semi)
			error_unexpected(curtok);
		getNextToken();
		return std::move(ast);
	}
	//<--------------------Subst
	else
		error("Unexpected", "Unexpected token -->" + curtok.val, curtok);
	return nullptr;
}
