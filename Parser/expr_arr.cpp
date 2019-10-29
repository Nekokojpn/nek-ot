#include "../nek-ot.hpp"

std::unique_ptr<ASTArrElements> Parser::expr_arr() {
	getNextToken();
	std::vector<std::unique_ptr<AST>> elements;
	uint32_t cnt = 0;
	while (true) {
		auto loc = curtok.loc;
		auto ast = expr();
		ast->loc = loc;
		if (curtok.ty == TK::tok_dtdt) {
			getNextToken();
		}
		elements.push_back(std::move(ast));
		if (curtok.ty != TK::tok_comma) break;
		getNextToken();
		cnt++;
	}
	if (cnt == 0 && curtok.ty == TK::tok_pipe) {
		auto restrait = std::move(elements[0]);
		getNextToken();
		if (curtok.ty != TK::tok_identifier)
			error_unexpected(curtok);
		auto identifier = expr_identifiers(); //TODO FIx
		if (curtok.ty != TK::tok_rarrow)
			error_expected("<-", curtok);
		auto loc = curtok.loc;
		auto ast_type = this->def_type(std::move(identifier));
		if (curtok.ty != TK::tok_rb)
			error("Expected", "Expected --> }", curtok);
		getNextToken();
		auto ast = std::move(std::make_unique<ASTArrElements>(std::move(restrait), std::move(ast_type)));
		ast->loc = loc;
		return std::move(ast);
	}
	if (cnt != 0 && curtok.ty == TK::tok_pipe) {
		add_err_msg("");
		error_unexpected(curtok);
	}
	if (curtok.ty != TK::tok_rb)
		error("Expected", "Expected --> }", curtok);
	getNextToken();
	auto loc = curtok.loc;
	auto ast = std::move(std::make_unique<ASTArrElements>(std::move(elements)));
	ast->loc = loc;
	return std::move(ast);
}