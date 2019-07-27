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
//get RType from curtok.
RType Parser::getRTypeByCurtok() {
	RType ret;
	if (curtok.ty == TK::tok_int) {
		ret = RType::Int;
	}
	else if (curtok.ty == TK::tok_char) {
		ret = RType::Char;
	}
	else if (curtok.ty == TK::tok_string) {
		ret = RType::String;
	}
	else if (curtok.ty == TK::tok_float) {
		ret = RType::Float;
	}
	else if (curtok.ty == TK::tok_double) {
		ret = RType::Double;
	}
	else if (curtok.ty == TK::tok_void) {
		ret = RType::Void;
	}
	else
		error("Unexpected token", "Unexpected token --> " + curtok.val, curtok);
	return ret;
}
//get AType from curtok.
AType Parser::getATypeByCurtok() {
	AType ret;
	if (curtok.ty == TK::tok_int) {
		ret = AType::Int;
	}
	else if (curtok.ty == TK::tok_char) {
		ret = AType::Char;
	}
	else if (curtok.ty == TK::tok_string) {
		ret = AType::String;
	}
	else if (curtok.ty == TK::tok_float) {
		ret = AType::Float;
	}
	else if (curtok.ty == TK::tok_double) {
		ret = AType::Double;
	}
	else
		return AType::Nop;
	return ret;
}

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
		else{
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

		error("Expected", "Expected --> )",curtok);
	}
	error("Unexpected", "must be a number.", curtok);
	exit(1);
	
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
	return std::move(ast);
}

std::unique_ptr<ASTInt> Parser::def_int() {
	getNextToken();
	if (curtok.ty != TK::tok_identifier)
		error("Syntax error", "After type must be an identifier.",curtok);
	auto str = curtok.val;
	getNextToken();
	if (consume(TK::tok_equal)) {
		auto ast = std::make_unique<ASTInt>(str,std::move(expr()));
		if (!consume(TK::tok_semi)) {
			error("Expected", "Expected --> ;",curtok);
		}
		return std::move(ast);
	}
	else if (consume(TK::tok_semi)) { // ;
		auto ast = std::make_unique<ASTInt>(str, nullptr);
		return std::move(ast);
	}
	error("Unexpected", "Syntax error.", curtok);
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
		if(curtok.ty != TK::tok_semi)
			error("Expected", "Expected --> ;", curtok);
		return std::move(ast);
	}
	else if (consume(TK::tok_semi)) { // ;
		auto ast = std::make_unique<ASTString>(str, nullptr);
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
	else if (curtok.ty == TK::tok_equal) {
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
	if (curtok.ty != TK::tok_arrow)
		error("Expected", "Expected --> ->", curtok);
	getNextToken();

	RType ret = getRTypeByCurtok();

	auto proto = std::make_unique<ASTProto>(str, putsArgs, argsIdentifier,ret);
	getNextToken();
	if (curtok.ty == TK::tok_lb) {
		auto ast_func = std::make_unique<ASTFunc>(std::move(proto), expr_block());
		return std::move(ast_func);
	}
	else if (curtok.ty == TK::tok_semi) { // The function has no body
		getNextToken();
		auto ast_func = std::make_unique<ASTFunc>(std::move(proto), expr_block());
		return std::move(ast_func);
	}
	else {
		error("Unexpected", "Unexpected token --> " + curtok.val, curtok);
	}
	auto ast = std::make_unique<ASTFunc>(std::move(proto), expr_block());
	return std::move(ast);
}

std::vector<std::unique_ptr<AST>> Parser::expr_block() { //  {expr block} 
	if (curtok.ty != TK::tok_lb)
		error("Expected", "Expected --> {", curtok);
	getNextToken();
	std::vector<std::unique_ptr<AST>> asts;
	while (curtok.ty != TK::tok_rb)
	{
		if (curtok.ty == TK::tok_int)
		{
			auto ast = def_int();
			asts.push_back(std::move(ast));
		}
		else if (curtok.ty == TK::tok_string) {
			auto ast = def_string();
			asts.push_back(std::move(ast));
		}
		else if (curtok.ty == TK::tok_if) {
			auto ast = bool_statement();
			asts.push_back(std::move(ast));
		}
		else if (curtok.ty == TK::tok_while) {
			auto ast = while_statement();
			asts.push_back(std::move(ast));
		}
		else if (curtok.ty == TK::tok_identifier) {
			auto ast = expr_identifier();
			asts.push_back(std::move(ast));
		}
		else if (curtok.ty == TK::tok_ret) {
			auto ast = def_ret();
			asts.push_back(std::move(ast));
		}
		else getNextToken();
	}
	return asts;
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
		return std::move(ast);
	}
	else if(curtok.ty == TK::tok_else) { //ELSE
		getNextToken();
		ast->ast_else = std::make_unique<ASTElse>(expr_block());
		return std::move(ast);
	}
	error("Unexpected", "Unexpected token--> " + curtok.val, curtok);
}

std::unique_ptr<AST> Parser::bool_expr() {
	auto lhs = expr();
	while (true) {
		BOp op;
		if (consume(TK::tok_lt)) {
			op = BOp::LThan;
		}
		else if (consume(TK::tok_lteq)) {
			op = BOp::LThanEqual;
		}
		else if (consume(TK::tok_rt)) {
			op = BOp::RThan;
		}
		else if (consume(TK::tok_rteq)) {
			op = BOp::RThanEqual;
		}
		else if (consume(TK::tok_eqeq)) {
			op = BOp::EqualEqual;
		}
		else if(consume(TK::tok_emeq)) {
			op = BOp::NotEqual;
		}
		else {
			break;
		}
		auto rhs = expr();
		lhs = std::make_unique<ASTBoolOp>(std::move(lhs), op, std::move(rhs));
	}
	return std::move(lhs);
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
	if (curtok.ty != TK::tok_rb)
		error("Expected", "Expected --> }", curtok);
	getNextToken();
	//<-----IF

	return std::move(ast);
}

std::unique_ptr<ASTSubst> Parser::subst_expr(const std::string& _id) {
	auto id = std::make_unique<ASTIdentifier>(_id);

	if (curtok.ty == TK::tok_equal) {
		getNextToken();
		auto ast = std::make_unique<ASTSubst>(std::move(id),std::move(expr()));
		//getNextToken();
		if(curtok.ty != TK::tok_semi)
			error("Expected", "Expected token --> ;" , curtok);
		return ast;
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
	getNextToken();
	return std::move(ast);
}
std::unique_ptr<ASTRet> Parser::def_ret() {
	getNextToken();
	RType rty = RType::Nop;
	std::unique_ptr<ASTRet> ast;
	if (curtok.ty == TK::tok_semi) {//void
		rty = RType::Void;
	}
	else if (curtok.ty == TK::tok_identifier) {
		ast = std::make_unique<ASTRet>(rty);
		ast->expr_p = std::move(expr());
		return std::move(ast);
	}
	ast = std::make_unique<ASTRet>(rty);
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