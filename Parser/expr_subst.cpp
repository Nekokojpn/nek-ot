#include "../nek-ot.h"

std::unique_ptr<ASTSubst> Parser::subst_expr(const std::string& _id) {
	auto loc = curtok.loc;
	auto id = std::make_unique<ASTIdentifier>(_id, TypeKind::Value);
	id->loc = loc;
	std::unique_ptr<ASTIdentifierArrayElement> id2;

	if (curtok.ty == TK::tok_lpb) { //Array
		std::vector<std::unique_ptr<AST>> expr_v;
		while (curtok.ty == TK::tok_lpb) { 
			getNextToken();
			expr_v.push_back(std::move(expr()));
			if (curtok.ty != TK::tok_rpb)
				error_unexpected(curtok);
			getNextToken();
		}
		auto loc = curtok.loc;
		id2 = std::make_unique<ASTIdentifierArrayElement>(_id, std::move(expr_v), TypeKind::Value);
		id2->loc = loc;
	}
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
		else { // ArrayType
			getNextToken();
			auto loc = curtok.loc;
			auto ast = std::make_unique<ASTSubst>(std::move(id2), std::move(expr()));
			ast->loc = loc;
			if (curtok.ty != TK::tok_semi)
				error_unexpected(curtok);
			getNextToken();
			return std::move(ast);
		}
	}
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
	else if (curtok.ty == TK::tok_plpl || curtok.ty == TK::tok_mimi) {
		auto loc = curtok.loc;
		auto ast = std::make_unique<ASTSubst>(std::move(id), std::move(expr()));
		ast->loc = loc;
		if (curtok.ty != TK::tok_semi)
			error("Expected", "Expected token --> ;", curtok);
		getNextToken();
		return std::move(ast);
	}
	else if (curtok.ty == TK::tok_semi) {
		return nullptr;
	}
	else
		error("Unexpected", "Unexpected token -->" + curtok.val, curtok);
	return nullptr;
}
