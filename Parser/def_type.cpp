#include "../nek-ot.hpp"

std::unique_ptr<ASTType> Parser::def_type(std::unique_ptr<AST> ast_id) {
	bool isonlydef = curtok.ty == TK::tok_colon ? true : false;
	getNextToken();
	std::string stct_name = curtok.val;
	auto ty = getTypeFromCurtok();
	if (ty.isArr) { // for array control
		std::unique_ptr<ASTArrElements> elem;
		if (!isonlydef) {
			if (curtok.ty == TK::tok_lb) { // The array declaration has a body.
				auto loc = curtok.loc;
				auto ast = std::make_unique<ASTType>(ty, std::move(ast_id),
					std::move(expr_arr()), 
					stct_name, this->cdgen->IsGlobal()
					);
				ast->name = this->curval;
				ast->loc = loc;
				return std::move(ast);
			}
			else if (curtok.ty == TK::tok_semi) { // The array declaration has no body.
				getNextToken();
				auto loc = curtok.loc;
				auto ast = std::make_unique<ASTType>(ty, std::move(ast_id),
					std::move(elem),
					stct_name,
					this->cdgen->IsGlobal()
					);
				ast->name = this->curval;
				ast->loc = loc;
				return std::move(ast);
			}
		}
		else {
			if (curtok.ty == TK::tok_semi) {
				auto loc = curtok.loc;
				auto ast = std::make_unique<ASTType>(ty, std::move(ast_id),
					std::move(elem),
					stct_name,
					this->cdgen->IsGlobal()
					);
				ast->name = this->curval;
				ast->loc = loc;
				getNextToken();
				return std::move(ast);
			}
		}
		getNextToken();
	}
	else { // for var control
		std::unique_ptr<ASTSubst> expr_;
		if (!isonlydef) {
			if (curtok.ty == TK::tok_semi) {
				getNextToken();
				auto loc = curtok.loc;
				auto ast = std::make_unique<ASTType>(ty,
					std::move(ast_id),
					std::move(expr_),
					stct_name,
					this->cdgen->IsGlobal()
					);
				ast->name = this->curval;
				ast->loc = loc;
				return std::move(ast);
			}
			auto loc = curtok.loc;
			auto ast = std::make_unique<ASTType>(ty,
				std::move(ast_id),
				std::make_unique<ASTSubst>(std::move(ast_id),
				std::move(expr())),
				stct_name,
				cdgen->IsGlobal()
				);
			ast->name = this->curval;
			ast->loc = loc;
			if (this->isExpectedSemi && curtok.ty != TK::tok_semi)
				error_expected(";", curtok);
			if(this->isExpectedSemi)getNextToken();
			this->isExpectedSemi = true;
			return std::move(ast);
		}
		else {
			if (curtok.ty != TK::tok_semi)
				error_expected(";", curtok);
			getNextToken();
			auto loc = curtok.loc;
			auto ast = std::make_unique<ASTType>(ty,
				std::move(ast_id),
				std::move(expr_),
				stct_name,
				this->cdgen->IsGlobal()
				);
			ast->name = this->curval;
			ast->loc = loc;
			return std::move(ast);
		}
	}
	auto loc = curtok.loc;
	error_unexpected(curtok);
	return nullptr;
}