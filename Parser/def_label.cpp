#include "../nek-ot.hpp"

std::unique_ptr<ASTLabel> Parser::def_label(std::string& identifier) {
	auto loc = curtok.loc;
	getNextToken();
	auto ast = std::make_unique<ASTLabel>(identifier);
	ast->loc = loc;
	return std::move(ast);
}