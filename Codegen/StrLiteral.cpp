#include "../nek-ot.hpp"

Value* ASTStrLiteral::codegen() {
	return builder.CreateGlobalStringPtr(value);
}

Type* ASTStrLiteral::getType() {
	return nullptr;
}

TypeAST ASTStrLiteral::getASTType() {
	return TypeAST::StrLiteral;
}