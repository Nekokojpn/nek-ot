#include "../nek-ot.hpp"

std::unique_ptr<AST> Parser::expr() {

	std::unique_ptr<AST> ast = bool_expr();
	return ast;
}

std::unique_ptr<AST> Parser::bool_expr() {
	auto lhs = bool_expr_op();
	while (true) {
		Op op;
		if (consume(TK::tok_ampamp)) {
			op = Op::Ampamp;
		}
		else if (consume(TK::tok_pipepipe)) {
			op = Op::Pipepipe;
		}
		else break;
		auto rhs = bool_expr();
		auto loc = curtok.loc;
		lhs = std::make_unique<ASTBinOp>(std::move(lhs), op, std::move(rhs));
		lhs->loc = loc;
	}
	return std::move(lhs);
}

std::unique_ptr<AST> Parser::bool_expr_op() {
	auto lhs = expr_add();
	while (true) {
		Op op;
		if (consume(TK::tok_lt)) {
			op = Op::LThan;
		}
		else if (consume(TK::tok_lteq)) {
			op = Op::LThanEqual;
		}
		else if (consume(TK::tok_rt)) {
			op = Op::RThan;
		}
		else if (consume(TK::tok_rteq)) {
			op = Op::RThanEqual;
		}
		else if (consume(TK::tok_eqeq)) {
			op = Op::EqualEqual;
		}
		else if (consume(TK::tok_emeq)) {
			op = Op::NotEqual;
		}
		else {
			break;
		}
		auto rhs = expr_add();
		auto loc = curtok.loc;
		lhs = std::make_unique<ASTBinOp>(std::move(lhs), op, std::move(rhs));
		lhs->loc = loc;
	}
	return std::move(lhs);
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
	auto lhs = expr_plmi();
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
		auto rhs = expr_plmi();
		auto loc = curtok.loc;
		lhs = std::make_unique<ASTBinOp>(std::move(lhs), op, std::move(rhs));
		lhs->loc = loc;
	}
	return std::move(lhs);
}
std::unique_ptr<AST> Parser::expr_plmi() {
	auto lhs = expr_primary();
	while (true) {
		Op op;
		if (consume(TK::tok_plpl)) {
			op = Op::Plus;
		}
		else if (consume(TK::tok_mimi)) {
			op = Op::Minus;
		}
		else {
			break;
		}
		lhs = std::make_unique<ASTBinOp>(std::move(lhs), op, std::make_unique<ASTValue>(1));
		lhs->loc = curtok.loc;
	}
	return std::move(lhs);
}

std::unique_ptr<AST> Parser::expr_primary() {
	bool xsign = false;
label1:
	if (curtok.ty == TK::tok_num_int) {
		auto loc = curtok.loc;
		auto value = std::make_unique<ASTValue>(std::atoi(curtok.val.c_str()));
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
	else if (curtok.ty == TK::tok_num_long) {
		auto loc = curtok.loc;
		auto value = std::make_unique<ASTValue>(std::atoll(curtok.val.c_str()), true);
		value->loc = loc;
		getNextToken(); //eat num
		return std::move(value);
	}
	else if (curtok.ty == TK::tok_num_long_long) {
		auto loc = curtok.loc;
		auto value = std::make_unique<ASTValue>(std::atoll(curtok.val.c_str()), true);
		value->loc = loc;
		getNextToken(); //eat num
		return std::move(value);
	}
	//Array
	else if (curtok.ty == TK::tok_lb) {
		auto loc = curtok.loc;
		auto elems = expr_arr();
		elems->loc = loc;
		return std::move(elems);
	}
	else if (curtok.ty == TK::tok_dq) {
		auto loc = curtok.loc;
		auto ast = std::make_unique<ASTString>("", std::move(expr_str()));
		ast->loc = loc;
		return std::move(ast);
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
		auto name = curtok.val;
		auto identifier = expr_identifiers();
		identifier->loc = loc;
		if (curtok.ty == TK::tok_lp ||
			curtok.ty == TK::tok_doll) { //Function call.
			auto funccall = func_call(name, curtok.ty == TK::tok_doll ? true : false);
			return std::move(funccall);
		}
		else if (curtok.ty == TK::tok_plpl) {
			afs.push_back(std::make_unique<ASTBinOp>(std::move(identifier), Op::Plus, std::make_unique<ASTValue>(1)));
			//;
		}
		else if (curtok.ty == TK::tok_mimi) {
			afs.push_back(std::make_unique<ASTBinOp>(std::move(identifier), Op::Minus, std::make_unique<ASTValue>(1)));
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
	else if (curtok.ty == TK::tok_minus) {
		getNextToken();
		xsign = true;
		goto label1;
	}
	add_err_msg("You specified token is unkown for the expression.");
	error_unexpected(curtok);
	exit(1);

}