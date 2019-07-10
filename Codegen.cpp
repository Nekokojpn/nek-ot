#include "nek-ot.h"

LLVMContext& context = getContext();
IRBuilder<>& builder = getBuilder();
Module* module = getModule();
std::map<std::string, Value*>& namedvalueslocal = getNamedValues_Local();

Value* ASTIdentifier::codegen() {
	return value;
}

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
Value* ASTBoolOp::codegen() {
	Value* l = lhs->codegen();
	Value* r = rhs->codegen();
	if (!l || !r)
		return nullptr;
	switch (bop) {
	case BOp::LThan:
		return builder.CreateFCmpONE(l, r, "iftemp");
	case BOp::LThanEqual:
		return builder.CreateAdd(l, r, "addtmp");
	case BOp::RThan:
		return builder.CreateAdd(l, r, "addtmp");
	case BOp::RThanEqual:
		return builder.CreateAdd(l, r, "addtmp");
	case BOp::EqualEqual:
		return builder.CreateAdd(l, r, "addtmp");
	case BOp::NotEqual:
		return builder.CreateAdd(l, r, "addtmp");
	default:
		return nullptr;
	}
}

Value* ASTInt::codegen() {
	auto value = expr_p->codegen();
	namedvalueslocal[name] = value;
	if (!value)
		return nullptr;
	return value;
}

Value* ASTFunc::codegen() {
	return nullptr;
}
Value* ASTArgProto::codegen() {
	return nullptr;
}

Value* ASTIf::codegen() {
	auto astboolop = proto->codegen();
	//TODO define if elif BasicBlock 
	auto ast = body->codegen();
	return nullptr;
}
