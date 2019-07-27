#include "nek-ot.h"

std::vector<std::unique_ptr<AST>> Parser::expr_block() { //  {expr block} 
	if (curtok.ty != TK::tok_lb)
		error("Expected", "Expected --> {", curtok);
	getNextToken();
	std::vector<std::unique_ptr<AST>> asts;
	while (curtok.ty != TK::tok_rb)
	{
		if (curtok.ty == TK::tok_int)
		{
			auto ast = def_int();
			asts.push_back(std::move(ast));
		}
		else if (curtok.ty == TK::tok_int_arr) {
			auto ast = def_int_arr();
			asts.push_back(std::move(ast));
		}
		else if (curtok.ty == TK::tok_string) {
			auto ast = def_string();
			asts.push_back(std::move(ast));
		}
		else if (curtok.ty == TK::tok_if) {
			auto ast = bool_statement();
			asts.push_back(std::move(ast));
		}
		else if (curtok.ty == TK::tok_while) {
			auto ast = while_statement();
			asts.push_back(std::move(ast));
		}
		else if (curtok.ty == TK::tok_identifier) {
			auto ast = expr_identifier();
			asts.push_back(std::move(ast));
		}
		else if (curtok.ty == TK::tok_ret) {
			auto ast = def_ret();
			asts.push_back(std::move(ast));
		}
		else getNextToken();
	}
	return asts;
}