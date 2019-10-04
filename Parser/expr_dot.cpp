#include "../nek-ot.h"

//TODO experimental function for stct accessor.
std::unique_ptr<AST> Parser::expr_dot(std::string& identifier) {
	getNextToken(); //eat .
	if (curtok.ty != TK::tok_identifier) {
		error_unexpected(curtok);
	}
	std::vector<std::string> elems;
	while (true) {
		elems.push_back(curtok.val);
		getNextToken();
		//Does not supported array.
		/*
		if (curtok.ty == TK::tok_lpb) {
			getNextToken();
			auto expr = expr();
			if (curtok.ty != TK::tok_rpb){}
		}
		*/
		if (curtok.ty != TK::tok_dot)break;
		getNextToken();
	}
	auto loc = curtok.loc;
	auto ast = std::make_unique<ASTIdentifierStctElement>(identifier, elems);
	ast->loc = loc;
	return std::move(ast);
}