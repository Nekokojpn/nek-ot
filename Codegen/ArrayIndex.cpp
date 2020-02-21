#include "../nek-ot.hpp"

Value* ASTArrayIndex::codegen() {
	auto v = this->expr->codegen();
	if (v->getType()->isPointerTy())
		v = builder.CreateLoad(v);
	return v;
}

Type* ASTArrayIndex::getType() {	
	return this->expr->getType();
}

TypeAST ASTArrayIndex::getASTType() {
	return TypeAST::ArrayIndex;
}