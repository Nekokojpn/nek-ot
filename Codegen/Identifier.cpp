#include "../nek-ot.hpp"

Value* ASTIdentifier::codegen() {
	auto value = this->lhs->codegen();
	if (!this->rhs) { //Is not a stct.
		if (this->kind == TypeKind::Pointer) {
			current_inst = nullptr;
			return value;
		}
		else if (this->kind == TypeKind::Reference) {
			current_inst = nullptr;
			return value;
		}
		else {
			current_inst = nullptr;
			return builder.CreateLoad(value);
		}
	}
	else { //Is a stct.
		auto value2 = this->rhs->codegen();
		if (this->kind == TypeKind::Pointer) {
			current_inst = nullptr;
			return value2;
		}
		else if (this->kind == TypeKind::Reference) {
			current_inst = nullptr;
			return value2;
		}
		else {
			current_inst = nullptr;
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