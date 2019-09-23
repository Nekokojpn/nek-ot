#include "../nek-ot.h"

std::unique_ptr<AST> Parser::expr() {

	std::unique_ptr<AST> ast = expr_add();
	return ast;
}
std::unique_ptr<AST> Parser::expr_add() {
	auto lhs = expr_mul();
	while (true) {
		Op op;
		if (consume(TK::tok_plus)) {
			op = Op::Plus;
		}
		else if (consume(TK::tok_minus)) {
			op = Op::Minus;
		}
		else {
			break;
		}
		auto rhs = expr_mul();
		auto loc = curtok.loc;
		lhs = std::make_unique<ASTBinOp>(std::move(lhs), op, std::move(rhs));
		lhs->loc = loc;
	}
	return std::move(lhs);
}
std::unique_ptr<AST> Parser::expr_mul() {
	auto lhs = expr_primary();
	while (true) {
		Op op;
		if (consume(TK::tok_star)) {
			op = Op::Mul;
		}
		else if (consume(TK::tok_slash)) {
			op = Op::Div;
		}
		else if (consume(TK::tok_percent)) {
			op = Op::RDiv;
		}
		else {
			break;
		}
		auto rhs = expr_primary();
		auto loc = curtok.loc;
		lhs = std::make_unique<ASTBinOp>(std::move(lhs), op, std::move(rhs));
		lhs->loc = loc;
	}
	return std::move(lhs);
}
std::unique_ptr<AST> Parser::expr_primary() {

	if (curtok.ty == TK::tok_num_int) {
		auto loc = curtok.loc;
		auto value = std::make_unique<ASTValue>(std::atoll(curtok.val.c_str()));
		value->loc = loc;
		getNextToken(); //eat num
		return std::move(value);
	}
	else if (curtok.ty == TK::tok_dq) {
		auto loc = curtok.loc;
		auto ast = std::make_unique<ASTString>("", std::move(expr_str()));
		ast->loc = loc;
		return std::move(ast);
	}
	else if (curtok.ty == TK::tok_plpl || curtok.ty == TK::tok_mimi) {
		
	}
	else if (curtok.ty == TK::tok_identifier) {
		auto loc = curtok.loc;
		auto identifier = std::make_unique<ASTIdentifier>(curtok.val);
		identifier->loc = loc;
		getNextToken();
		if (curtok.ty == TK::tok_lp ||
			curtok.ty == TK::tok_doll) { //Function call.
			auto funccall = func_call(identifier->name, curtok.ty == TK::tok_doll ? true : false);
			return std::move(funccall);
		}
		else if (curtok.ty == TK::tok_lpb) { //Array
			std::vector<std::unique_ptr<AST>> expr_v;
			while (curtok.ty == TK::tok_lpb) { //Array
				getNextToken();
				expr_v.push_back(std::move(expr()));
				if (curtok.ty != TK::tok_rpb) {
					add_err_msg("Have you forgotten ] ?");
					add_err_msg("Hint: The specified array element accessor is invalid! Starts with [ and ends with ].");
					error_unexpected(curtok);
				}
					
				getNextToken();
			}
			if (curtok.ty == TK::tok_dot) {
				//TODO:  for arrray structure accessor.
			}
			auto loc = curtok.loc;
			auto ast = std::make_unique<ASTIdentifierArrayElement>(identifier->name, std::move(expr_v));
			ast->loc = loc;
			return std::move(ast);
		}
		else if (curtok.ty == TK::tok_dot) { //struct, class etc.
			auto loc = curtok.loc;
			auto ast = expr_dot(identifier->name);
			ast->loc = loc;
		}
		return std::move(identifier);
	}
	else if (curtok.ty == TK::tok_lp) {
		getNextToken();
		auto loc = curtok.loc;
		auto ast = expr();
		ast->loc = loc;
		if (curtok.ty == TK::tok_rp)
		{
			getNextToken();
			return std::move(ast);
		}

		add_err_msg("Have you forgotten ) ?");
		add_err_msg("Hint: You specified nested expression must start with ( and end with ).");
		error_expected(")", curtok);
	}
	else if (curtok.ty == TK::tok_doll) {
		getNextToken();
		auto loc = curtok.loc;
		auto ast = expr();
		ast->loc = loc;
		return std::move(ast);
	}
	add_err_msg("You specified token is unkown for the expression.");
	error_unexpected(curtok);
	exit(1);

}