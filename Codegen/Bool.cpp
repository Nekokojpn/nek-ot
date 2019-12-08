#include "../nek-ot.hpp"

Value* ASTBool::codegen() {
	return builder.getInt1(this->value);
}

Type* ASTBool::getType() {
	return builder.getInt1Ty();
}