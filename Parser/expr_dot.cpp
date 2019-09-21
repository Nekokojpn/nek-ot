#include "../nek-ot.h"

//TODO experimental function for stct accessor.
std::unique_ptr<AST> Parser::expr_dot(std::string& identifier) {
	getNextToken(); //eat .
	if (curtok.ty != TK::tok_identifier) {
		error_unexpected(curtok);
	}
	auto id = curtok.val;
	return std::make_unique<ASTIdentifierStctElement>(id, std::move(expr()));
}