#include "nek-ot.h"


std::map<std::string, Value*>& namedvalues_local = getNamedValues_Local();


bool Parser::consume(TK tk) {
	if (tokens[index].ty == tk)
	{
		getNextToken();
		return true;
	}
	return false;
}
void Parser::getNextToken() {
	curtok = tokens[++index];
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
		auto identifier = std::make_unique<ASTIdentifier>(namedvalues_local[curtok.val]);
		getNextToken();
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

		error("Expected", "Expected --> )",0,0);
	}
	error("Unexpected", "must be a number.", 0, 0);
	exit(1);
	
}

std::unique_ptr<ASTInt> Parser::def_int() {
	getNextToken();
	if (curtok.ty != TK::tok_identifier)
		error("Syntax error", "After type must be an identifier.",0,0);
	auto ast = std::make_unique<ASTInt>(curtok.val);
	getNextToken();
	if (consume(TK::tok_equal)) {
		ast->expr_p = std::move(expr());
		if (!consume(TK::tok_semi)) {
			error("Expected", "Expected --> ;",0,0);
		}
		return std::move(ast);
	}
	else if (consume(TK::tok_semi)) { // ;
		return std::move(ast);
	}
	error("Unexpected", "Syntax error.", 0, 0);
	return nullptr;
}

std::unique_ptr<ASTFunc> Parser::def_func() {
	getNextToken();
	if (curtok.ty != TK::tok_identifier)
		error("Synatax error", "After fn must be an identifier", 0, 0);
	std::vector<ASTArgProto> v;
	auto proto = std::make_unique<ASTProto>(curtok.val, v);
	getNextToken();
	if (curtok.ty != TK::tok_lp)
		error("Expected", "Expected --> (", 0, 0);
	/*
		ˆø”–¢ŽÀ‘•
	*/
	getNextToken();
	if (curtok.ty != TK::tok_rp)
		error("Expected", "Expected --> )", 0, 0);
	getNextToken();
	if (curtok.ty != TK::tok_arrow)
		error("Expected", "Expected --> ->", 0, 0);
	getNextToken();
	if (curtok.ty != TK::tok_void)
		error("", "", 0, 0);
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
		error("Unexpected", "Unexpected token --> " + curtok.val, 0, 0);
	}
	auto ast = std::make_unique<ASTFunc>(std::move(proto), expr_block());
	return std::move(ast);
}

std::vector<std::unique_ptr<AST>> Parser::expr_block() { //  {expr block} 
	if (curtok.ty != TK::tok_lb)
		error("Expected", "Expected --> {", 0, 0);
	getNextToken();
	std::vector<std::unique_ptr<AST>> asts;
	while (curtok.ty != TK::tok_rb)
	{
		if (curtok.ty == TK::tok_int)
		{
			auto ast = def_int();
			asts.push_back(std::move(ast));
		}
		else if (curtok.ty == TK::tok_if) {
			auto ast = bool_statement();
			asts.push_back(std::move(ast));
		}
		else getNextToken();
	}
	return asts;
}
//Top of bool expr. body‚àŠÜ‚ß‚½top
std::unique_ptr<ASTIf> Parser::bool_statement() {
	//IF----->
	getNextToken();
	if (curtok.ty != TK::tok_lp)
		error("Expected", "Expected --> (", 0, 0);
	getNextToken();
	auto boolast = bool_expr();
	if (curtok.ty != TK::tok_rp)
		error("Expected", "Expected --> )", 0, 0);
	getNextToken();
	if (curtok.ty != TK::tok_lb)
		error("Expected", "Expected --> {", 0, 0);

	
	auto ast = std::make_unique<ASTIf>(std::move(boolast), expr_block());
	if (curtok.ty != TK::tok_rb)
		error("Expected", "Expected --> }", 0, 0);
	getNextToken();
	//<-----IF
	/*		IF‚µ‚©‚È‚¢ê‡return		*/
	if(curtok.ty != TK::tok_elif&&curtok.ty != TK::tok_else)
		return std::move(ast);
	//ELIF or ELSE----->
	if (curtok.ty == TK::tok_elif) { //ELIF
		getNextToken();
		ast->ast_elif = bool_statement();
	}
	else { //ELSE
		getNextToken();
		ast->ast_else = std::make_unique<ASTElse>(expr_block());
	}
	return std::move(ast);
}
//boolŽ®‚Ì‚½‚ß‚Ìexpr.
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