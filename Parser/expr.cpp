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
		else if (consume(TK::tok_xor)) {
			op = Op::Xor;
		}
		else if (consume(TK::tok_ltlt)) {
			op = Op::LShift;
		}
		else if (consume(TK::tok_rtrt)) {
			op = Op::RShift;
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
	else if (curtok.ty == TK::tok_num_double) {
		auto loc = curtok.loc;
		auto value = std::make_unique<ASTValue>(std::atof(curtok.val.c_str()), true);
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
		// marge string operator
	}
	else if (curtok.ty == TK::tok_identifier || curtok.ty == TK::tok_amp || curtok.ty == TK::tok_star) {
		auto kind = TypeKind::Value;
		if (curtok.ty == TK::tok_amp) {
			getNextToken();
			if (curtok.ty != TK::tok_identifier) {
				error_unexpected(curtok);
			}
			kind = TypeKind::Reference;
		}
		else if (curtok.ty == TK::tok_star) {
			getNextToken();
			if (curtok.ty != TK::tok_identifier) {
				error_unexpected(curtok);
			}
			kind = TypeKind::Pointer;
		}
		auto loc = curtok.loc;
		auto identifier = std::make_unique<ASTIdentifier>(curtok.val, kind);
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
			auto ast = std::make_unique<ASTIdentifierArrayElement>(identifier->name, std::move(expr_v), kind);
			ast->loc = loc;
			return std::move(ast);
		}
		else if (curtok.ty == TK::tok_dot) { //struct, class etc.
			auto loc = curtok.loc;
			auto ast = expr_dot(identifier->name);
			ast->loc = loc;
		}
		else if (curtok.ty == TK::tok_plpl) {
			auto loc = curtok.loc;
			// ‚È‚É‚µ‚Ä‚½‚Á‚¯
		}
		else if (curtok.ty == TK::tok_mimi) {

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
	else if (curtok.ty == TK::tok_minus) {
		getNextToken();
		auto lloc = curtok.loc;
		auto lhs = std::make_unique<ASTValue>(0);
		lhs->loc = lloc;
		auto rloc = curtok.loc;
		auto rhs = expr();
		rhs->loc = rloc;
		auto bloc = curtok.loc;
		auto binop = std::make_unique<ASTBinOp>(std::move(lhs), Op::Minus, std::move(rhs));
		binop->loc = bloc;
		return std::move(binop);
	}
	add_err_msg("You specified token is unkown for the expression.");
	error_unexpected(curtok);
	exit(1);

}