#include "nek-ot.h"

//Read forward if the next token is expected.
bool Parser::consume(TK tk) noexcept {
	if (tokens[index].ty == tk)
	{
		getNextToken();
		return true;
	}
	return false;
}
//set the next token to curtok.
void Parser::getNextToken() noexcept {
	curtok = tokens[++index];
}

//get AType from curtok.
AType Parser::getATypeByCurtok() {
	if (curtok.ty == TK::tok_i32) {
		return AType::I32;
	}
	else if (curtok.ty == TK::tok_char) {
		return AType::Char;
	}
	else if (curtok.ty == TK::tok_string) {
		return  AType::String;
	}
	else if (curtok.ty == TK::tok_float) {
		return  AType::Float;
	}
	else if (curtok.ty == TK::tok_double) {
		return AType::Double;
	}
	else {
		return AType::Nop;
	}
}



std::unique_ptr<ASTStrLiteral> Parser::expr_str() {
	if (curtok.ty != TK::tok_dq && curtok.ty != TK::tok_num_int)
		error("Unexpected", "Unexpected token --> "+curtok.val, curtok);
	if(curtok.ty == TK::tok_dq)
		getNextToken();
	std::string str = "";
	while (true) {
		if (curtok.ty != TK::tok_str_string && curtok.ty != TK::tok_num_int)
			error("Unexpected", "Unexpected token --> " + curtok.val, curtok);
		if(curtok.ty == TK::tok_str_string)
			str += curtok.val;
		else if (curtok.ty == TK::tok_num_int) {
			str += curtok.val;
			/*
			getNextToken();
			if (curtok.ty != TK::tok_dot)
				error("There must be no such like:", "num type and string type can not be combined.",curtok);
			getNextToken();
			if(curtok.ty != )
			*/
		}
		getNextToken();
		if (curtok.ty != TK::tok_dq)
			error("Unexpected", "Unexpected token --> " + curtok.val, curtok);
		getNextToken();
		if (curtok.ty == TK::tok_plus) {
			getNextToken();
			continue;
		}
		break;
	}
	auto ast = std::make_unique<ASTStrLiteral>(str);
	ast->loc = curtok.loc;
	return std::move(ast);
}

std::unique_ptr<ASTType> Parser::def_type() {
	auto ty = this->getATypeByCurtok();
	getNextToken();
	if (curtok.ty != TK::tok_identifier)
		error_unexpected(curtok);
	auto id = curtok.val;
	getNextToken();
	if (curtok.ty == TK::tok_equal) {
		auto ast = std::make_unique<ASTType>(ty, id, std::move(this->subst_expr(id)));
		ast->loc = curtok.loc;
		if (!consume(TK::tok_semi)) {
			error("Expected", "Expected --> ;", curtok);
		}
		return std::move(ast);
	}
	else if (consume(TK::tok_semi)) { // ;
		auto ast = std::make_unique<ASTType>(ty, id, nullptr);
		ast->loc = curtok.loc;
		return std::move(ast);
	}
	error_unexpected(curtok);
	return nullptr;
}

std::unique_ptr<ASTIntArray> Parser::def_int_arr() {
	getNextToken();
	if(curtok.ty != TK::tok_identifier)
		error("Syntax error", "Unexpected token --> " + curtok.val, curtok);
	auto name = curtok.val;
	getNextToken();
	if (curtok.ty == TK::tok_equal) {
		getNextToken();
		if (curtok.ty != TK::tok_new)
			error("", "", curtok);
		getNextToken();
		if(curtok.ty != TK::tok_i32)
			error("", "", curtok);
		getNextToken();
		if(curtok.ty != TK::tok_lpb)
			error("", "", curtok);
		getNextToken();
		if(curtok.ty != TK::tok_num_int)
			error("", "", curtok);
		auto size = std::atoi(curtok.val.c_str());
		getNextToken();
		if(curtok.ty != TK::tok_rpb)
			error("", "", curtok);
		getNextToken();
		if(curtok.ty != TK::tok_semi)
			error("", "", curtok);
		auto ast = std::make_unique<ASTIntArray>(name, size);
		ast->loc = curtok.loc;
		getNextToken();
		return std::move(ast);
	}
	else if (curtok.ty == TK::tok_semi) {

	}
	error("Syntax error", "Unexpected token--> " + curtok.val, curtok);
	return nullptr;
}

std::unique_ptr<ASTString> Parser::def_string() {
	getNextToken();
	if (curtok.ty != TK::tok_identifier)
		error("Syntax error", "After type must be an identifier.", curtok);
	auto str = curtok.val;
	getNextToken();
	if (curtok.ty == TK::tok_equal) {
		getNextToken();
		auto ast = std::make_unique<ASTString>(str, std::move(expr_str()));
		ast->loc = curtok.loc;
		if(curtok.ty != TK::tok_semi)
			error("Expected", "Expected --> ;", curtok);
		return std::move(ast);
	}
	else if (consume(TK::tok_semi)) { // ;
		auto ast = std::make_unique<ASTString>(str, nullptr);
		ast->loc = curtok.loc;
		return std::move(ast);
	}
	error("Unexpected", "Syntax error.", curtok);
	return nullptr;
}

std::unique_ptr<AST> Parser::expr_identifier() {
	auto id = curtok.val;
	getNextToken();
	if (curtok.ty == TK::tok_semi) {
		return nullptr;
	}
	else if (curtok.ty == TK::tok_equal || 
				curtok.ty == TK::tok_lpb || 
				curtok.ty == TK::tok_darrow ||
				curtok.ty == TK::tok_plpl ||
				curtok.ty == TK::tok_mimi) {
		auto ast = subst_expr(id);
		return std::move(ast);
	}
	else if (curtok.ty == TK::tok_lp) {
		auto ast = func_call(id);
		return std::move(ast);
	}
	return nullptr;
}

std::unique_ptr<ASTFunc> Parser::def_func() {
	getNextToken();
	if (curtok.ty != TK::tok_identifier)
		error("Synatax error", "After fn must be an identifier", curtok);
	auto str = curtok.val;
	getNextToken();
	if (curtok.ty != TK::tok_lp)
		error("Expected", "Expected --> (", curtok);
	getNextToken();
	std::vector<AType> putsArgs;
	std::vector<std::string> argsIdentifier;
	while (true) {
		AType ty = getATypeByCurtok();
		if (ty == AType::Nop)break;

		putsArgs.push_back(ty);

		getNextToken();
		if (curtok.ty != TK::tok_identifier)
			error("Unexpected token", "Unexpected token --> " + curtok.val, curtok);
		argsIdentifier.push_back(curtok.val);
		getNextToken();
		if (curtok.ty != TK::tok_comma) break;
		getNextToken();
	}
	if (curtok.ty != TK::tok_rp)
		error("Expected", "Expected --> )", curtok);
	getNextToken();

	AType ret;

	if (curtok.ty == TK::tok_arrow) {
		getNextToken();
		ret = getATypeByCurtok();
		getNextToken();
	}
	else {
		ret = AType::Void;
	}

	auto proto = std::make_unique<ASTProto>(str, putsArgs, argsIdentifier,ret);
	proto->loc = curtok.loc;
	if (curtok.ty == TK::tok_lb) {
		auto ast_func = std::make_unique<ASTFunc>(std::move(proto), expr_block());
		ast_func->loc = curtok.loc;
		return std::move(ast_func);
	}
	else if (curtok.ty == TK::tok_semi) { // The function has no body
		getNextToken();
		auto ast_func = std::make_unique<ASTFunc>(std::move(proto), expr_block());
		ast_func->loc = curtok.loc;
		return std::move(ast_func);
	}
	else {
		error("Unexpected", "Unexpected token --> " + curtok.val, curtok);
	}
	auto ast = std::make_unique<ASTFunc>(std::move(proto), expr_block());
	ast->loc = curtok.loc;
	return std::move(ast);
}

std::unique_ptr<ASTIf> Parser::bool_statement() {
	//IF----->
	getNextToken();
	if (curtok.ty != TK::tok_lp)
		error("Expected", "Expected --> (", curtok);
	getNextToken();
	auto boolast = bool_expr();
	if (curtok.ty != TK::tok_rp)
		error("Expected", "Expected --> )", curtok);
	getNextToken();
	if (curtok.ty != TK::tok_lb)
		error("Expected", "Expected --> {", curtok);

	auto ast = std::make_unique<ASTIf>(std::move(boolast), expr_block());
	ast->loc = curtok.loc;
	if (curtok.ty != TK::tok_rb)
		error("Expected", "Expected --> }", curtok);
	getNextToken();
	//<-----IF
	/*		IF‚µ‚©‚È‚¢		*/
	if(curtok.ty != TK::tok_elif&&curtok.ty != TK::tok_else)
		return std::move(ast);
	//ELIF or ELSE----->
	if (curtok.ty == TK::tok_elif) { //ELIF
		ast->ast_elif = std::move(bool_statement());
		getNextToken();
		return std::move(ast);
	}
	else if(curtok.ty == TK::tok_else) { //ELSE
		getNextToken();
		ast->ast_else = std::make_unique<ASTElse>(expr_block());
		ast->loc = curtok.loc;
		getNextToken();
		return std::move(ast);
	}
	error("Unexpected", "Unexpected token--> " + curtok.val, curtok);
}

std::unique_ptr<AST> Parser::bool_expr() {
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
		else if(consume(TK::tok_emeq)) {
			op = Op::NotEqual;
		}
		else {
			break;
		}
		auto rhs = expr();
		lhs = std::make_unique<ASTBinOp>(std::move(lhs), op, std::move(rhs));
		lhs->loc = curtok.loc;
	}
	return std::move(lhs);
}

std::unique_ptr<ASTFor> Parser::for_statement() {
	return nullptr;
}

std::unique_ptr<ASTWhile> Parser::while_statement() {
	getNextToken();
	if (curtok.ty != TK::tok_lp)
		error("Expected", "Expected --> (", curtok);
	getNextToken();
	auto boolast = bool_expr();
	if (curtok.ty != TK::tok_rp)
		error("Expected", "Expected --> )", curtok);
	getNextToken();
	if (curtok.ty != TK::tok_lb)
		error("Expected", "Expected --> {", curtok);


	auto ast = std::make_unique<ASTWhile>(std::move(boolast), expr_block());
	ast->loc = curtok.loc;
	if (curtok.ty != TK::tok_rb)
		error("Expected", "Expected --> }", curtok);
	getNextToken();
	//<-----IF

	return std::move(ast);
}

std::unique_ptr<ASTSubst> Parser::subst_expr(const std::string& _id) {
	auto id = std::make_unique<ASTIdentifier>(_id);
	id->loc = curtok.loc;
	std::unique_ptr<ASTIdentifierArrayElement> id2;
	if (curtok.ty == TK::tok_lpb) {
		getNextToken();
		auto expr_ = expr();
		if (curtok.ty != TK::tok_rpb)
			error_unexpected(curtok);
		id2 = std::make_unique<ASTIdentifierArrayElement>(_id, std::move(expr_));
		id2->loc = curtok.loc;
		getNextToken();
	}
	if (curtok.ty == TK::tok_equal) {
		if (!id2) {
			getNextToken();
			auto ast = std::make_unique<ASTSubst>(std::move(id), std::move(expr()));
			ast->loc = curtok.loc;
			if (curtok.ty != TK::tok_semi)
				error_unexpected(curtok);
			return std::move(ast);
		}
		else {
			getNextToken();
			auto ast = std::make_unique<ASTSubst>(std::move(id2), std::move(expr()));
			ast->loc = curtok.loc;
			if (curtok.ty != TK::tok_semi)
				error_unexpected(curtok);
			return std::move(ast);
		}
	}
	else if (curtok.ty == TK::tok_darrow) {
		getNextToken();
		auto ast = std::make_unique<ASTSubst>(std::move(id), std::move(expr_block()));
		getNextToken();
		if (curtok.ty != TK::tok_semi)
			error("Expected", "Expected token --> ;", curtok);
		getNextToken();
		return std::move(ast);
	}
	else if (curtok.ty == TK::tok_plpl || curtok.ty == TK::tok_mimi) {
		auto ast = std::make_unique<ASTSubst>(std::move(id), std::move(expr()));
		if (curtok.ty != TK::tok_semi)
			error("Expected", "Expected token --> ;", curtok);
		getNextToken();
		return std::move(ast);
	}
	else if (curtok.ty == TK::tok_semi) {
		return nullptr;
	}
	else
		error("Unexpected", "Unexpected token -->" + curtok.val, curtok);
}

std::unique_ptr<ASTCall> Parser::func_call(const std::string& _id) {
	if (curtok.ty != TK::tok_lp)
		error("Expected", "Expected token --> (",curtok);
	getNextToken();
	std::vector<std::unique_ptr<AST>> argsIdentifier;
	while (true) {
		argsIdentifier.push_back(std::move(expr()));
		
		if (curtok.ty != TK::tok_comma) break;
		getNextToken();
	}
	if (curtok.ty != TK::tok_rp)
		error("Expected", "Expected --> )", curtok);
	auto ast = std::make_unique<ASTCall>(_id, std::move(argsIdentifier));
	ast->loc = curtok.loc;
	getNextToken();
	return std::move(ast);
}
std::unique_ptr<ASTRet> Parser::def_ret() {
	getNextToken();
	AType rty = AType::Nop;
	std::unique_ptr<ASTRet> ast;
	if (curtok.ty == TK::tok_semi) {//void
		rty = AType::Void;
	}
	else {
		ast = std::make_unique<ASTRet>(rty);
		ast->loc = curtok.loc;
		ast->expr_p = std::move(expr());
		return std::move(ast);
	}
	ast = std::make_unique<ASTRet>(rty);
	ast->loc = curtok.loc;
	getNextToken();
	
	return std::move(ast);
}

Parser::Parser(std::vector<Token_t> _tokens) : tokens(_tokens) {
	index = 0;
	curtok = tokens[index];
}
void Parser::parse_codegen() {	
	if (curtok.ty == TK::tok_fn)
	{
		auto ast = def_func();
		ast->codegen();
	}
	if (curtok.ty == TK::tok_eof)
		return;
	getNextToken();
	parse_codegen();
	return;
}
