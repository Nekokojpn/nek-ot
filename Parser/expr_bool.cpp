#include "../nek-ot.hpp"

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
	auto lhs = expr();
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
		auto rhs = expr();
		auto loc = curtok.loc;
		lhs = std::make_unique<ASTBinOp>(std::move(lhs), op, std::move(rhs));
		lhs->loc = loc;
	}
	return std::move(lhs);
}