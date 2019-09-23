#include "../nek-ot.h"

std::unique_ptr<ASTType> Parser::def_type(const std::string& _id) {
	bool isonlydef = curtok.ty == TK::tok_colon ? true : false;
	getNextToken();
	auto ty = getATypeByCurtok(); //TODO: Support stct;
	std::string stct_name;
	if (ty == AType::Struct)
		stct_name = curtok.val;
	if (ty != AType::Nop) {
		getNextToken();

		if (curtok.ty == TK::tok_lpb) { // for array control
			std::vector<long long> size_v;
			while (curtok.ty == TK::tok_lpb) {
				getNextToken();
				//Experimental 
				if (curtok.ty != TK::tok_num_int && curtok.ty != TK::tok_dtdt)
					error_unexpected(curtok);
				if (curtok.ty == TK::tok_num_int) {
					if (std::atoll(curtok.val.c_str()) > 0)
						size_v.push_back(std::atoll(curtok.val.c_str()));
					else {
						error("Compile error", "Array elements size must be higher than 0.", curtok);
					}
				}
				else {
					// TODO : For array block structure.
					// evaluate when codegen.
					size_v.push_back(0LL);
				}
				getNextToken();
				if (curtok.ty != TK::tok_rpb)
					error_unexpected(curtok);
				getNextToken();
			}
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
					auto ast = std::make_unique<ASTType>(ty, _id, size_v, std::move(expr_arr()), stct_name);
					ast->loc = loc;
					return std::move(ast);
				}
				else if (curtok.ty == TK::tok_semi) { // The array declaration has no body.
					getNextToken();
					auto loc = curtok.loc;
					auto ast = std::make_unique<ASTType>(ty, _id, size_v, nullptr, stct_name);
					ast->loc = loc;
					return std::move(ast);
				}
			}
			else {
				if (curtok.ty == TK::tok_semi) {
					auto loc = curtok.loc;
					auto ast = std::make_unique<ASTType>(ty, _id, size_v, nullptr, stct_name);
					ast->loc = loc;
					return std::move(ast);
				}
			}
			getNextToken();
		}
		else { // for var control
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
					auto ast = std::make_unique<ASTType>(ty, _id, nullptr, stct_name);
					ast->loc = loc;
					return std::move(ast);
				}
				auto loc = curtok.loc;
				auto ast = std::make_unique<ASTType>(ty, _id, std::make_unique<ASTSubst>(std::make_unique<ASTIdentifier>(_id), std::move(expr())), stct_name);
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
				auto ast = std::make_unique<ASTType>(ty, _id, nullptr, stct_name);
				ast->loc = loc;
				return std::move(ast);
			}
		}
	}
	auto loc = curtok.loc;
	/*    ONLY HAVE TYPE
	else if (consume(TK::tok_semi)) { // ;
		auto loc = curtok.loc;
		auto ast = std::make_unique<ASTType>(ty, id, nullptr);
		ast->loc = loc;
		return std::move(ast);
	}
	*/
	error_unexpected(curtok);
	return nullptr;
}