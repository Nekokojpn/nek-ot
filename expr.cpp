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
	}
	return std::move(lhs);
}
std::unique_ptr<AST> Parser::expr_primary() {

	if (curtok.ty == TK::tok_num_int) {
		auto value = std::make_unique<ASTValue>(std::atoi(curtok.val.c_str()));
		getNextToken(); //eat num
		return std::move(value);
	}
	else if (curtok.ty == TK::tok_identifier) {
		auto identifier = std::make_unique<ASTIdentifier>(curtok.val);
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
			return std::move(ast);
		}
		return std::move(identifier);
	}
	else if (curtok.ty == TK::tok_lp) {
		getNextToken();
		auto ast = expr();
		if (curtok.ty == TK::tok_rp)
		{
			getNextToken();
			return std::move(ast);
		}

		error("Expected", "Expected --> )", curtok);
	}
	error("Unexpected", "must be a number.", curtok);
	exit(1);

}