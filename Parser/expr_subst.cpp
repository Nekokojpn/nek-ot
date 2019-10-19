#include "../nek-ot.hpp"

std::unique_ptr<ASTSubst> Parser::subst_expr(const std::string& _id, TypeKind ty_kind) {
	auto loc = curtok.loc;
	auto id = std::make_unique<ASTIdentifier>(_id, ty_kind);
	id->loc = loc;
	std::unique_ptr<ASTIdentifierArrayElement> id2;
	std::unique_ptr<AST> id_stct;
	//Array------------------->
	if (curtok.ty == TK::tok_lpb) { 
		std::vector<std::unique_ptr<AST>> expr_v;
		while (curtok.ty == TK::tok_lpb) { 
			getNextToken();
			expr_v.push_back(std::move(expr()));
			if (curtok.ty != TK::tok_rpb)
				error_unexpected(curtok);
			getNextToken();
		}
		auto loc = curtok.loc;
		id2 = std::make_unique<ASTIdentifierArrayElement>(_id, std::move(expr_v), ty_kind);
		id2->loc = loc;
	}
	//<--------------------Array
	//Stct-------------------->
	if (curtok.ty == TK::tok_dot) {
		auto loc = curtok.loc;
		id_stct = expr_dot(id->name);
		id_stct->loc = loc;
	}
	//<--------------------Stct
	//Subst------------------->
	if (curtok.ty == TK::tok_equal) {
		if (!id2) {
			getNextToken();
			auto loc = curtok.loc;
			auto ast = std::make_unique<ASTSubst>(std::move(id), std::move(expr()));
			ast->loc = loc;
			if (curtok.ty != TK::tok_semi)
				error_unexpected(curtok);
			return std::move(ast);
		}
		else if(id2) { // ArrayType
			getNextToken();
			auto loc = curtok.loc;
			auto ast = std::make_unique<ASTSubst>(std::move(id2), std::move(expr()));
			ast->loc = loc;
			if (curtok.ty != TK::tok_semi)
				error_unexpected(curtok);
			getNextToken();
			return std::move(ast);
		}
		else { //Stct
			getNextToken();
			auto loc = curtok.loc;
			auto ast = std::make_unique<ASTSubst>(std::move(id_stct), std::move(expr()));
			ast->loc = loc;
			if (curtok.ty != TK::tok_semi)
				error_unexpected(curtok);
			getNextToken();
			return std::move(ast);
		}
	}
	//<--------------------Subst
	//Lambda------------------->
	else if (curtok.ty == TK::tok_darrow) {
		getNextToken();
		auto loc = curtok.loc;
		auto ast = std::make_unique<ASTSubst>(std::move(id), std::move(expr_block(false)));
		ast->loc = loc;
		getNextToken();
		if (curtok.ty != TK::tok_semi)
			error("Expected", "Expected token --> ;", curtok);
		getNextToken();
		return std::move(ast);
	}
	//<-------------------Lambda
	else if (curtok.ty == TK::tok_plpl || curtok.ty == TK::tok_mimi) {
		auto loc = curtok.loc;
		auto ast = std::make_unique<ASTSubst>(std::move(id), std::move(expr()));
		ast->loc = loc;
		if (curtok.ty != TK::tok_semi)
			error("Expected", "Expected token --> ;", curtok);
		getNextToken();
		return std::move(ast);
	}
	//do nothing--------------->
	else if (curtok.ty == TK::tok_semi) {
		return nullptr;
	}
	//<----------------do nothing
	else
		error("Unexpected", "Unexpected token -->" + curtok.val, curtok);

	return nullptr;
}
