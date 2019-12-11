#include "../nek-ot.hpp"

ArrayRef<Type*> ASTStctElements::make_aref(){
	return nullptr;
}
Value* ASTStctElements::codegen() {
	return nullptr;
}

Type* ASTStctElements::getType() {
	return nullptr;
}

TypeAST ASTStctElements::getASTType() {
	return TypeAST::StctElements;
}