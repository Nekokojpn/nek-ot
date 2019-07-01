#include "nek-ot.h"


std::map<std::string, Value*> NamedValues;
LLVMContext& TheContext = getContext();
IRBuilder<>& Builder = getBuilder();
//std::unique_ptr<Module> TheModule = getModule();

Value* ASTValue::codegen() {
	return Builder.getInt32(value);
}


Value* ASTBinOp::codegen() {
	Value* l = lhs->codegen();
	Value* r = rhs->codegen();
	if (!l || !r)
		return nullptr;
	switch (op) {
	case Op::Plus:
		return Builder.CreateAdd(l, r, "addtmp");
	case Op::Minus:
		return Builder.CreateSub(l, r, "subtmp");
	case Op::Mul:
		return Builder.CreateMul(l, r, "multmp");
	case Op::Div:
		return Builder.CreateSDiv(l, r, "divtmp");
	default:
		return nullptr;
	}
}


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
	return std::move(expr_add());
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
		getNextToken();
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
	return nullptr;

}

Parser::Parser(std::vector<Token_t> _tokens) : tokens(_tokens) {
	index = 0;
	curtok = tokens[index];
}
std::unique_ptr<AST> Parser::parse() {	
	std::unique_ptr<AST> ast;
	if (curtok.ty == TK::tok_int)
	{
		ast = std::move(def_int());
	}
	if(curtok.ty == TK::tok_num_int)
		 ast = std::move(expr());
	Value* tmp = Builder.CreateAlloca(Builder.getInt32Ty(), ast->codegen());
	return std::move(ast);
}