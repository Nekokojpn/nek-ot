#include "nek-ot.h"


std::map<std::string, Value*> namedvalues;

LLVMContext& TheContext = getContext();
IRBuilder<>& Builder = getBuilder();
//std::unique_ptr<Module> TheModule = getModule();




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
	std::unique_ptr<AST> lhs = expr_mul();
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
	std::unique_ptr<AST> lhs = expr_primary();
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
	else if (curtok.ty == TK::tok_lp) {
		getNextToken();
		auto ast = expr();
		
		if (consume(TK::tok_rp))
			return std::move(ast);

		error("Expected", "Expected --> )",0,0);
	}
	error("Unexpected", "must be a number.", 0, 0);
	exit(1);
	
}
std::unique_ptr<AST> Parser::def_int() {
	getNextToken();
	if (curtok.ty != TK::tok_identifier)
		error("Syntax error", "After type must be an identifier.",0,0);
	auto ast = std::make_unique<ASTInt>(curtok.val);
	getNextToken();
	if (consume(TK::tok_equal)) {
		ast->expr_p = std::move(expr());
		Value* tmp = Builder.CreateAlloca(Builder.getInt32Ty(), ast->codegen(Builder), ast->name);
		if (!consume(TK::tok_semi)) {
			error("Expected", "Expected --> ;",0,0);
		}
		return std::move(ast);
	}
	else if (consume(TK::tok_semi)) { // ;
		Value* tmp = Builder.CreateAlloca(Builder.getInt32Ty(), ast->codegen(Builder), ast->name);
		return std::move(ast);
	}
	error("Unexpected", "Syntax error.", 0, 0);
	return nullptr;
}

Parser::Parser(std::vector<Token_t> _tokens) : tokens(_tokens) {
	index = 0;
	curtok = tokens[index];
}
std::unique_ptr<AST> Parser::parse_codegen() {	
	if (curtok.ty == TK::tok_int)
	{
		auto ast = std::move(def_int());
		
		return std::move(ast);
	}
	return nullptr;
}