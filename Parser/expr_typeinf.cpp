#include "../nek-ot.hpp"

std::unique_ptr<ASTTypeinf> Parser::expr_typeinf() {
	auto ty = this->getTypeFromCurtok();
	std::vector<std::unique_ptr<AST>> exprs;
	if (ty.ty == AType::Array || ty.ty == AType::List) {
		while (curtok.ty == TK::tok_comma) {
			exprs.push_back(expr());
		}
		if (!curtokIs(TK::tok_rp))
			error_expected("}", curtok);
		auto loc = curtok.loc;
		auto ast = std::make_unique<ASTTypeinf>(ty, std::move(exprs));
		ast->loc = loc;
		return std::move(ast);
	}
	else {
		auto loc = curtok.loc;
		exprs.push_back(expr());

		auto ast = std::make_unique<ASTTypeinf>(ty, std::move(exprs));
		ast->loc = loc;
		return std::move(ast);
	}
}