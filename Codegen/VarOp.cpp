#include "../nek-ot.hpp"

Value* ASTVarOp::codegen() {
	return nullptr;
}

Type* ASTVarOp::getType() {
	return nullptr;
}

TypeAST ASTVarOp::getASTType() {
	return TypeAST::VarOp;
}