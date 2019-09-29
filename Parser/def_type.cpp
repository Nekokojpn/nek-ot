#include "../nek-ot.h"

std::unique_ptr<ASTType> Parser::def_type(const std::string& _id) {
	bool isonlydef = curtok.ty == TK::tok_colon ? true : false;
	getNextToken();
	auto ty = getTypeFromCurtok(); //TODO: Support stct;
	std::string stct_name;
	if (ty.ty == AType::Struct)
		stct_name = curtok.val;
	if (ty.ty != AType::Nop) {
		if (ty.isArr) { // for array control
			std::unique_ptr<ASTArrElements> elem;
			if (!isonlydef) {
				if (curtok.ty != TK::tok_lp) {
					error_expected("(", curtok);
				}
				getNextToken();
				if (curtok.ty != TK::tok_rp) {
					error_expected(")", curtok);
				}
				getNextToken();
				
				if (curtok.ty == TK::tok_lb) { // The array declaration has a body.
					auto loc = curtok.loc;
					auto ast = std::make_unique<ASTType>(ty, _id,
						std::move(expr_arr()), 
						stct_name, this->cdgen->IsGlobal()
						);
					ast->loc = loc;
					return std::move(ast);
				}
				else if (curtok.ty == TK::tok_semi) { // The array declaration has no body.
					getNextToken();
					auto loc = curtok.loc;
					auto ast = std::make_unique<ASTType>(ty, _id,
						std::move(elem),
						stct_name,
						this->cdgen->IsGlobal()
						);
					ast->loc = loc;
					return std::move(ast);
				}
			}
			else {
				if (curtok.ty == TK::tok_semi) {
					auto loc = curtok.loc;
					auto ast = std::make_unique<ASTType>(ty, _id,
						std::move(elem),
						stct_name,
						this->cdgen->IsGlobal()
						);
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
				if (curtok.ty != TK::tok_lp &&
					curtok.ty != TK::tok_doll)
					error_unexpected(curtok);
				if (curtok.ty == TK::tok_doll)doll = true;
				getNextToken();
				if (curtok.ty == TK::tok_rp) {
					getNextToken();
					if (curtok.ty != TK::tok_semi)
						error_expected(";", curtok);
					getNextToken();
					auto loc = curtok.loc;
					auto ast = std::make_unique<ASTType>(ty,
						_id,
						std::move(expr_),
						stct_name,
						this->cdgen->IsGlobal()
						);
					ast->loc = loc;
					return std::move(ast);
				}
				auto loc = curtok.loc;
				auto ast = std::make_unique<ASTType>(ty,
					_id,
					std::make_unique<ASTSubst>(std::make_unique<ASTIdentifier>(_id, TypeKind::Value),
					std::move(expr())),
					stct_name,
					cdgen->IsGlobal()
					);
				ast->loc = loc;
				if (!doll && curtok.ty != TK::tok_rp)
					error_expected(")", curtok);
				if (!doll)
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
					_id,
					std::move(expr_),
					stct_name,
					this->cdgen->IsGlobal()
					);
				ast->loc = loc;
				return std::move(ast);
			}
		}
	}
	auto loc = curtok.loc;
	error_unexpected(curtok);
	return nullptr;
}