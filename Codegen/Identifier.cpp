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
		auto v = Codegen::getIdentifier(value, this->rhs, this->loc);
		if (this->kind == TypeKind::Pointer) {
			return v;
		}
		else if (this->kind == TypeKind::Reference) {
			return v;
		}
		else {
			return builder.CreateLoad(v);
		}
		return v;
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