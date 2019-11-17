#include "../nek-ot.hpp"

std::unique_ptr<AST> Parser::expr_var() {
	if (!curtokIs(TK::tok_identifier))
		error_unexpected(curtok);
	auto name = curtok.val;
	getNextToken();
	//If array ty
	if (curtokIs(TK::tok_lpb)) {
		auto loc = curtok.loc;
		auto index = expr_array_indexes();
		auto ast = std::make_unique<ASTIdentifierArrayElementBase>(name, std::move(index));
		ast->loc = loc;
		return std::move(ast);
	}
	//If var ty
	else {
		auto loc = curtok.loc;
		auto ast = std::make_unique<ASTIdentifierBase>(name);
		ast->loc = loc;
		return std::move(ast);
	}
}