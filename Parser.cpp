#include "nek-ot.h"

extern LLVMContext TheContext;
extern IRBuilder<> Builder(TheContext);
extern std::unique_ptr<Module> TheModule;

std::map<std::string, Value*> NamedValues;

Value* ASTValue::codegen() {
	Value* value = NamedValues[name];
	if (!value)
	{
	}
	return value;
}


Value* ASTBinOp::codegen() {
	Value* l = lhs->codegen();
	Value* r = lhs->codegen();
	if (!l || !r)
		return nullptr;
	switch (op) {
	case Op::Plus:
		return Builder.CreateFAdd(l, r, "addtmp");
	case Op::Minus:
		return Builder.CreateFSub(l, r, "subtmp");
	case Op::Mul:
		return Builder.CreateFMul(l, r, "multmp");
	case Op::Div:
		l = Builder.CreateFCmpULT(l, r, "cmptmp");
		return Builder.CreateUIToFP(l, Type::getDoubleTy(TheContext), "booltmp");
	default:
		return nullptr;
	}
}


bool Parser::consume(TK tk) {
	if (tokens[index].ty == tk)
	{
		index++;
		return true;
	}
	return false;
}
Token_t Parser::get() {
	Token_t t = tokens[index++];
	return t;
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
		std::unique_ptr<AST> rhs = expr_mul();
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
	Token_t t = get();
	if (t.ty != TK::tok_num_int) {
		error("Unexpected", "must be a number.");
		exit(1);
	}
	auto value = std::make_unique<ASTValue>(std::atoi(t.val.c_str()));
	return std::move(value);
}

Parser::Parser(std::vector<Token_t> _tokens) : tokens(_tokens) {
	index = 0;

}
std::unique_ptr<AST> Parser::parse() {
	return std::move(expr_add());
}