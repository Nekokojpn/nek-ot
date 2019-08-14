#include "nek-ot.h"

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
		lhs = std::make_unique<ASTBinOp>(std::move(lhs), op, std::move(rhs));
		lhs->loc = curtok.loc;
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
		else {
			break;
		}
		auto rhs = expr_primary();
		lhs = std::make_unique<ASTBinOp>(std::move(lhs), op, std::move(rhs));
		lhs->loc = curtok.loc;
	}
	return std::move(lhs);
}
std::unique_ptr<AST> Parser::expr_primary() {

	if (curtok.ty == TK::tok_num_int) {
		auto value = std::make_unique<ASTValue>(std::atoi(curtok.val.c_str()));
		value->loc = curtok.loc;
		getNextToken(); //eat num
		return std::move(value);
	}
	else if (curtok.ty == TK::tok_str_string) {
		auto ast = std::make_unique<ASTStrLiteral>(curtok.val.substr(1, curtok.val.length() - 1));
		ast->loc = curtok.loc;
		getNextToken();
		return std::move(ast);
	}
	else if (curtok.ty == TK::tok_plpl || curtok.ty == TK::tok_mimi) {
		
	}
	else if (curtok.ty == TK::tok_identifier) {
		auto identifier = std::make_unique<ASTIdentifier>(curtok.val);
		identifier->loc = curtok.loc;
		getNextToken();
		if (curtok.ty == TK::tok_lp) { //Function call.
			auto funccall = func_call(identifier->name);
			return std::move(funccall);
		}
		else if (curtok.ty == TK::tok_lpb) { //Array
			getNextToken();
			auto expr_ = expr();
			if (curtok.ty != TK::tok_rpb)
				error_unexpected(curtok);
			getNextToken();
			auto ast = std::make_unique<ASTIdentifierArrayElement>(identifier->name, std::move(expr_));
			ast->loc = curtok.loc;
			return std::move(ast);
		}
		return std::move(identifier);
	}
	else if (curtok.ty == TK::tok_lp) {
		getNextToken();
		auto ast = expr();
		ast->loc = curtok.loc;
		if (curtok.ty == TK::tok_rp)
		{
			getNextToken();
			return std::move(ast);
		}

		error("Expected", "Expected --> )", curtok);
	}
	error_unexpected(curtok);
	exit(1);

}