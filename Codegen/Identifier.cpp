#include "../nek-ot.hpp"

Value* ASTIdentifier::codegen() {
	auto value = this->lhs->codegen();
	if (!this->rhs) { //Is not a stct.
		if (this->kind == TypeKind::Pointer) {
			return value;
		}
		else if (this->kind == TypeKind::Reference) {
			return value;
		}
		else {
			return builder.CreateLoad(value);
		}
	}
	else { //Is a stct.
		auto value2 = this->rhs->codegen();
		if (this->kind == TypeKind::Pointer) {
			return value2;
		}
		else if (this->kind == TypeKind::Reference) {
			return value2;
		}
		else {
			return builder.CreateLoad(value2);
		}
		return value2;
	}
	return nullptr;
}

Type* ASTIdentifier::getType() {
	if (!rhs)
		return lhs->getType();
	else// Struct type
		return rhs->getType();
}

TypeAST ASTIdentifier::getASTType() {
	return TypeAST::Identifier;
}