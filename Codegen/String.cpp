#include "../nek-ot.hpp"

Value* ASTString::codegen() { //—vC³
	isStringCodegen = true;
	auto str = expr_str->value;
	auto ptr = builder.CreateGlobalStringPtr(str, name);
	namedvalues_str[name] = ptr;
	return ptr;
}
Type* ASTString::getType() {
	return nullptr;
}