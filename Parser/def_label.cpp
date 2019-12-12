#include "../nek-ot.hpp"

ASTLabel* Parser::def_label(std::string& identifier) {
	auto loc = curtok.loc;
	getNextToken();
	auto ast = new ASTLabel(identifier);
	ast->loc = loc;
	return ast;
}