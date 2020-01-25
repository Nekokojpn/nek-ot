#include "../nek-ot.hpp"

AST* Parser::expr() {

	AST* ast = bool_expr();
	return ast;
}

AST* Parser::bool_expr() {
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
		auto rhs = bool_expr_op();
		auto loc = curtok.loc;
		lhs = new ASTBinOp(lhs, op, rhs);
		lhs->loc = loc;
	}
	return lhs;
}

AST* Parser::bool_expr_op() {
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
		lhs = new ASTBinOp(lhs, op, rhs);
		lhs->loc = loc;
	}
	return lhs;
}

AST* Parser::expr_add() {
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
		lhs = new ASTBinOp(lhs, op, rhs);
		lhs->loc = loc;
	}
	return lhs;
}
AST* Parser::expr_mul() {
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
		lhs = new ASTBinOp(lhs, op, rhs);
		lhs->loc = loc;
	}
	return lhs;
}
AST* Parser::expr_plmi() {
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
		auto rhs = new ASTValue(1);
		rhs->loc = curtok.loc;
		lhs = new ASTBinOp(lhs, op, rhs);
		lhs->loc = curtok.loc;
	}
	return lhs;
}

AST* Parser::expr_primary() {
	bool xsign = false;
label1:
	if (curtok.ty == TK::tok_num_int) {
		auto loc = curtok.loc;
		ASTValue* ast = nullptr;
		if (std::atoll(curtok.val.c_str()) > 2147483647LL || std::atoll(curtok.val.c_str()) < -2147483648LL)
			ast = new ASTValue(std::atoll(curtok.val.c_str()), true);
		else
			ast = new ASTValue(std::atoi(curtok.val.c_str()));
		ast->loc = loc;
		getNextToken(); //eat num
		return ast;
	}
	else if (curtok.ty == TK::tok_num_double) {
		auto loc = curtok.loc;
		auto value = new ASTValue(std::atof(curtok.val.c_str()), true);
		value->loc = loc;
		getNextToken(); //eat num
		return value;
	}
	else if (curtok.ty == TK::tok_num_long) {
		auto loc = curtok.loc;
		auto value = new ASTValue(std::atoll(curtok.val.c_str()), true);
		value->loc = loc;
		getNextToken(); //eat num
		return value;
	}
	else if (curtok.ty == TK::tok_num_long_long) {
		auto loc = curtok.loc;
		auto value = new ASTValue(std::atoll(curtok.val.c_str()), true);
		value->loc = loc;
		getNextToken(); //eat num
		return value;
	}
	//Array or stct or tuple
	else if (curtok.ty == TK::tok_lb) {
		auto loc = curtok.loc;
		auto elems = expr_lb();
		elems->loc = loc;
		return elems;
	}
	//List
	else if (curtok.ty == TK::tok_lpb) {
		auto loc = curtok.loc;
		auto elems = expr_lpb();
		elems->loc = loc;
		return elems;
	}
	else if (curtok.ty == TK::tok_dq) {
		auto loc = curtok.loc;
		auto ast = new ASTString("", expr_str());
		ast->loc = loc;
		return ast;
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
			return funccall;
		}
		//unused
		else if (identifier->getASTType() == TypeAST::Subst) {

			afs.push_back(identifier);
			return ((ASTSubst*)identifier)->id;
			//;
		}
		return identifier;
	}
	else if (curtok.ty == TK::tok_lp) {
		getNextToken();
		if (curtok.ty == TK::tok_identifier && nexttokIs(TK::tok_comma)) {
			std::vector<AST*> ids;
			ids.push_back(expr_identifiers());
			while (curtok.ty == TK::tok_comma) {
				getNextToken();
				if (curtok.ty != TK::tok_identifier)
					break;
				ids.push_back(expr_identifiers());
			}
			if (curtok.ty != TK::tok_rp)
				error_expected(")", curtok);
			getNextToken();
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
				error_unexpected(curtok);
			}
			auto rhs = expr();
			std::vector<AST*> stmt;
			auto loc = curtok.loc;
			auto ids_bin = new ASTBinOp(ids[0], op, rhs);
			ids_bin->loc = loc;
			stmt.push_back(ids_bin);
			for (int i = 1; i < ids.size(); i++) {
				auto idsi_bin = new ASTBinOp(ids[i], op, rhs);
				idsi_bin->loc = ids[i]->loc;
				stmt.push_back(idsi_bin);
			}
			auto l = stmt[0];
			for (int i = 1; i < stmt.size(); i++) {
				l = new ASTBinOp(l, Op::Ampamp, stmt[i]);
				l->loc = stmt[i]->loc;
			}
			return l;
		}
		auto loc = curtok.loc;
		auto ast = expr();
		ast->loc = loc;
		if (curtok.ty == TK::tok_rp)
		{
			getNextToken();
			return ast;
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
	else if (curtok.ty == TK::tok_if) {
		getNextToken();
		if (!curtokIs(TK::tok_lp))
			error_expected("(", curtok);
		getNextToken();
		auto boolast = expr();
		if (!curtokIs(TK::tok_rp))
			error_expected(")", curtok);
		getNextToken();
		ASTIf* ast;
		if (curtok.ty == TK::tok_lb)
			error_unexpected(curtok);
		{
			auto loc = curtok.loc;
			std::vector<AST*> body;
			body.push_back(expr());
			ast = new ASTIf(boolast, body);
			ast->loc = loc;
			if (!curtokIs(TK::tok_semi))
				error_expected(";", curtok);
			getNextToken();
		}
		if (!curtokIs(TK::tok_else)) {
			add_err_msg("Syntax: <variable> = if(<bool expression>) <expression>; else <expression>;");
			error_expected("else", curtok);
		}
		getNextToken();
		{
			auto loc = curtok.loc;
			std::vector<AST*> body;
			body.push_back(expr());
			auto astelse = new ASTElse(body);
			astelse->loc = loc;
			if (!curtokIs(TK::tok_semi))
				error_expected(";", curtok);
			getNextToken();
			ast->ast_else = astelse;
		}
		isExpectedSemi = false;
		return ast;
	}
	else if (curtokIs(TK::tok_true) || curtokIs(TK::tok_false)) {
		if (curtokIs(TK::tok_true)) {
			auto loc = curtok.loc;
			auto ast = new ASTBool(true);
			ast->loc = loc;
			getNextToken();
			return ast;
		}
		else {
			auto loc = curtok.loc;
			auto ast = new ASTBool(false);
			ast->loc = loc;
			getNextToken();
			return ast;
		}
	}
	add_err_msg("You specified expression is unkown for this syntax.");
	error_unexpected(curtok);
}