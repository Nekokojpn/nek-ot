#include "../nek-ot.hpp"

ASTArrElements* Parser::expr_arr() {
	std::vector<AST*> elements;
	uint32_t cnt = 0;
	while (true) {
		auto loc = curtok.loc;
		auto ast = expr();
		ast->loc = loc;
		if (curtok.ty == TK::tok_dtdt) {
			getNextToken();
		}
		elements.push_back(ast);
		if (curtok.ty != TK::tok_comma) break;
		getNextToken();
		cnt++;
	}
	if (cnt == 0 && curtok.ty == TK::tok_pipe) {
		auto restrait = elements[0];
		getNextToken();
		if (curtok.ty != TK::tok_identifier)
			error_unexpected(curtok);
		auto identifier = expr_identifiers();
		if (curtok.ty != TK::tok_rarrow)
			error_expected("<-", curtok);
		auto loc = curtok.loc;
		auto ast_type = this->def_type(identifier);
		if (curtok.ty != TK::tok_rb)
			error("Expected", "Expected --> }", curtok);
		getNextToken();
		auto ast = new ASTArrElements(restrait, ast_type);
		ast->loc = loc;
		return ast;
	}
	if (cnt != 0 && curtok.ty == TK::tok_pipe) {
		add_err_msg("");
		error_unexpected(curtok);
	}
	if (curtok.ty != TK::tok_rb)
		error("Expected", "Expected --> }", curtok);
	getNextToken();
	auto loc = curtok.loc;
	auto ast = new ASTArrElements(elements);
	ast->loc = loc;
	return ast;
}