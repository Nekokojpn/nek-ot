#include "../nek-ot.hpp"

std::unique_ptr<ASTType> Parser::def_type(std::unique_ptr<AST> ast_id) {
	bool isonlydef = curtok.ty == TK::tok_colon ? true : false;
	bool istypeinf = false;
	getNextToken();
	std::string stct_name = curtok.val;
	auto ty = getTypeFromCurtok();
ty_inf:
	if (ty.ty != AType::Nop) {
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
				bool doll = false;
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
				if (!istypeinf && curtok.ty != TK::tok_lp &&
					curtok.ty != TK::tok_doll)
					error_unexpected(curtok);
				if (curtok.ty == TK::tok_doll)doll = true;
				if(!istypeinf) getNextToken();
				if (curtok.ty == TK::tok_rp) {
					getNextToken();
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
				auto loc = curtok.loc;
				auto ast = std::make_unique<ASTType>(ty,
					std::move(ast_id),
					std::make_unique<ASTSubst>(std::move(ast_id), std::move(expr_typeinf())),
					stct_name,
					cdgen->IsGlobal()
					);
				ast->name = this->curval;
				ast->loc = loc;
				if (!istypeinf && !doll && curtok.ty != TK::tok_rp)
					error_expected(")", curtok);
				if (!istypeinf && !doll)
					getNextToken();
				if (curtok.ty != TK::tok_semi)
					error_expected(";", curtok);
				getNextToken();
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
	}
	else { //Type inference
		Type_t ty_;
		if (curtok.ty == TK::tok_num_int) {
			ty_.isArr = false;
			ty_.kind = TypeKind::Value;
			ty_.ty = AType::I32;
			ty = ty_;
		}
		else if (curtok.ty == TK::tok_num_double) {
			ty_.isArr = false;
			ty_.kind = TypeKind::Value;
			ty_.ty = AType::F64;
			ty = ty_;
		}
		istypeinf = true;
		goto ty_inf;
	}
	auto loc = curtok.loc;
	error_unexpected(curtok);
	return nullptr;
}