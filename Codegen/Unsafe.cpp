#include "../nek-ot.hpp"
bool isUnsafe = false;

Value* ASTUnsafe::codegen() {
	isUnsafe = true;
	for (int i = 0; i < this->body.size(); i++)
		this->body[i]->codegen();
	isUnsafe = false;
	return nullptr;
}

Type* ASTUnsafe::getType() {
	return nullptr;
}

TypeAST ASTUnsafe::getASTType() {
	return TypeAST::Unsafe;
}