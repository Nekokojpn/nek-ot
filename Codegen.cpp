#include "nek-ot.h"


Value* ASTValue::codegen(IRBuilder<>& Builder) {
	return Builder.getInt32(value);
}

Value* ASTBinOp::codegen(IRBuilder<>& Builder) {
	Value* l = lhs->codegen(Builder);
	Value* r = rhs->codegen(Builder);
	if (!l || !r)
		return nullptr;
	switch (op) {
	case Op::Plus:
		return Builder.CreateAdd(l, r, "addtmp");
	case Op::Minus:
		return Builder.CreateSub(l, r, "subtmp");
	case Op::Mul:
		return Builder.CreateMul(l, r, "multmp");
	case Op::Div:
		return Builder.CreateSDiv(l, r, "divtmp");
	default:
		return nullptr;
	}
}

Value* ASTInt::codegen(IRBuilder<>& Builder) {
	Value* value = expr_p->codegen(Builder);
	if (!value)
		return nullptr;
	return value;
}