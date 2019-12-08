#include "../nek-ot.hpp"

Value* ASTGoto::codegen() {
	gotocodegen = true;
	jmp_bbs[this->label].push_back(builder.GetInsertBlock());
	return nullptr;
}

Type* ASTGoto::getType() {
	return nullptr;
}