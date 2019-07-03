#include "nek-ot.h"

LLVMContext& context = getContext();
IRBuilder<>& builder = getBuilder();
Module* module = getModule();


Value* ASTValue::codegen() {
	return builder.getInt32(value);
}

Value* ASTBinOp::codegen() {
	Value* l = lhs->codegen();
	Value* r = rhs->codegen();
	if (!l || !r)
		return nullptr;
	switch (op) {
	case Op::Plus:
		return builder.CreateAdd(l, r, "addtmp");
	case Op::Minus:
		return builder.CreateSub(l, r, "subtmp");
	case Op::Mul:
		return builder.CreateMul(l, r, "multmp");
	case Op::Div:
		return builder.CreateSDiv(l, r, "divtmp");
	default:
		return nullptr;
	}
}

Value* ASTInt::codegen() {
	Value* value = expr_p->codegen();
	if (!value)
		return nullptr;
	return value;
}

Value* ASTFunc::codegen() {

	return nullptr;
}