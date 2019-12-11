#include "../nek-ot.hpp"

Value* ASTAction::codegen() {
	return nullptr;
}

Type* ASTAction::getType() {
	return nullptr;
}

TypeAST ASTAction::getASTType() {
	return TypeAST::Action;
}