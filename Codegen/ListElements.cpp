#include "../nek-ot.hpp"

Value* ASTListElements::codegen() {
	return nullptr;
}

Type* ASTListElements::getType() {
	return nullptr;
}

TypeAST ASTListElements::getASTType() {
	return TypeAST::ListElements;
}