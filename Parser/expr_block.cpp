#include "../nek-ot.hpp"

std::vector<AST*> Parser::expr_block(bool isOneExpr) { //  {expr block} 
	if (!isOneExpr && curtok.ty != TK::tok_lb) {
		add_err_msg("Expressions block must be start with {.");
		error_expected("{", curtok);
	}
	if(!isOneExpr)
		getNextToken();
	std::vector<AST*> asts;
	while (curtok.ty != TK::tok_rb && curtok.ty != TK::tok_eof)
	{
		this->isGlobal = false;
		if (curtok.ty == TK::tok_string) {
			auto ast = def_string();
			asts.push_back(ast);
		}
		else if (curtok.ty == TK::tok_if) {
			auto ast = bool_statement();
			asts.push_back(ast);
		}
		else if (curtok.ty == TK::tok_for) {
			auto ast = for_statement();
			asts.push_back(ast);
		}
		else if (curtok.ty == TK::tok_while) {
			auto ast = while_statement();
			asts.push_back(ast);
		}
		else if (curtok.ty == TK::tok_identifier) {
			auto ast = expr_identifier();
			asts.push_back(ast);
		}
		else if (curtok.ty == TK::tok_ret) {
			auto ast = def_ret();
			asts.push_back(ast);
		}
		else if (curtok.ty == TK::tok_action) {
			auto ast = def_action();
			asts.push_back(ast);
		}
		else if (curtok.ty == TK::tok_brk) {
			auto ast = def_brk();
			asts.push_back(ast);
		}
		else if (curtok.ty == TK::tok_fn) {
			auto ast = def_func();
			asts.push_back(ast);
		}
		else if (curtok.ty == TK::tok_star) {
			auto ast = expr_star();
			asts.push_back(ast);
		}
		else if (curtok.ty == TK::tok_goto) {
			auto ast = expr_goto();
			asts.push_back(ast);
		}
		else if (curtok.ty == TK::tok_anno) {
			auto ast = expr_anno();
			asts.push_back(ast);
		}
		else if (curtok.ty == TK::tok_num_int) {
			auto ast = expr();
			asts.push_back(ast);
			if (!curtokIs(TK::tok_semi)) error_expected(";", curtok);
			getNextToken();
		}
		else if (curtok.ty == TK::tok_unsafe) {
			getNextToken();
			auto loc = curtok.loc;
			auto ast = new ASTUnsafe(expr_block(false));
			ast->loc = loc;
			asts.push_back(ast);
		}
		else getNextToken();
		if (isOneExpr)
			break;
		//finally
		{
			if (afs.size() > 0) {
				for (int i = 0; i < afs.size(); i++) {
					asts.push_back(afs[i]);
				}
				afs.clear();
			}
		}
	}
	if (!isOneExpr && curtok.ty == TK::tok_eof) {
		add_err_msg("Have you forgotten } ?");
		error_expected("}", curtok);
	}
	if(!isOneExpr)getNextToken();
	return asts;
}