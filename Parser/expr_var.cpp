#include "../nek-ot.hpp"

AST* Parser::expr_var() {
	if (!curtokIs(TK::tok_identifier) && !curtokIs(TK::tok_op) && !curtokIs(TK::tok_lpb))
		error_unexpected(curtok);
	//If array ty
	if (curtokIs(TK::tok_lpb)) {
		getNextToken();
		auto loc = curtok.loc;
		auto index = expr();
		if (!curtokIs(TK::tok_rpb))
			error_expected("]", curtok);
		getNextToken();
		auto ast = new ASTArrayIndex(index);
		ast->loc = loc;
		return ast;
	}
	auto name = curtok.val;
	getNextToken();
	//if function call
	if (curtokIs(TK::tok_lp)) {
		if (name == "op") {
			getNextToken();
			std::vector<Op> ops;
			while (true) {
				if (curtokIs(TK::tok_plus))
					ops.push_back(Op::Plus);
				else if (curtokIs(TK::tok_minus))
					ops.push_back(Op::Minus);
				else if (curtokIs(TK::tok_star))
					ops.push_back(Op::Mul);
				else if (curtokIs(TK::tok_slash))
					ops.push_back(Op::Div);
				else if (curtokIs(TK::tok_percent))
					ops.push_back(Op::RDiv);
				getNextToken();
				if (!curtokIs(TK::tok_comma))break;
				getNextToken();
			}
			if (!curtokIs(TK::tok_rp))
				error_expected(")", curtok);
			getNextToken();
			if(!curtokIs(TK::tok_cleq))
				error_expected(":=", curtok);
			getNextToken();
			AST* ast;
			if(curtokIs(TK::tok_lb))
				ast = new ASTVarOp(ops, expr_block(false));
			else
				ast = new ASTVarOp(ops, expr_block(true));
			return ast;
		}
		auto ast = func_call(name, false);
		return ast;
	}
	//If var ty
	else {
		auto loc = curtok.loc;
		auto ast = new ASTIdentifierBase(name);
		ast->loc = loc;
		return ast;
	}
}