#include "../nek-ot.hpp"

Value* ASTBinOp::codegen() {
	Value* l = lhs->codegen();
	Value* r = rhs->codegen();
	if (!l || !r)
		return nullptr;
	if (!isPtr && l->getType()->isPointerTy())
		l = builder.CreateLoad(l);
	if (!isPtr && r->getType()->isPointerTy())
		r = builder.CreateLoad(r);
	auto tup = Codegen::doMatchType(l, r);
	l = std::get<0>(tup);
	r = std::get<1>(tup);
	this->curTy = l->getType();
	std::vector<Type*> tys;
	tys.push_back(l->getType());
	tys.push_back(r->getType());
	std::vector<Value*> v;
	v.push_back(l);
	v.push_back(r);
	std::vector<unsigned int> ar0;
	ar0.push_back(0);
	std::vector<unsigned int> ar1;
	ar1.push_back(1);
	switch (op) {
	case Op::Plus:
	{
		/*auto lefthand = Codegen::getValueInt(l);
		auto righthand = Codegen::getValueInt(r);
		if (lefthand && righthand) {
			if(l->getType() == builder.getInt32Ty())
				if (lefthand->getZExtValue() + righthand->getZExtValue() > I32_MAX ||
							lefthand->getZExtValue() + righthand->getZExtValue() < I32_MIN)
					error_codegen("Overflow occured!", this->loc);
			if(l->getType() == builder.getInt64Ty())
				if (lefthand->getZExtValue() + righthand->getZExtValue() > I64_MAX ||
						lefthand->getZExtValue() + righthand->getZExtValue() < I64_MIN)
					error_codegen("Overflow occured!", this->loc);
		}*/
		auto add_inst = Intrinsic::getDeclaration(module.get(), Intrinsic::sadd_with_overflow, tys);
		auto addc_inst = builder.CreateCall(add_inst, v);
		Codegen::createRuntimeError("Overflow occured!", builder.CreateICmpEQ(builder.CreateExtractValue(addc_inst, ar1), builder.getInt1(0)), this->loc);
		return builder.CreateExtractValue(addc_inst, ar0);	
	}
	case Op::Minus:
	{
		/*auto lefthand = Codegen::getValueInt(l);
		auto righthand = Codegen::getValueInt(r);
		if (lefthand && righthand) {
			if (l->getType() == builder.getInt32Ty())
				if (lefthand->getZExtValue() - righthand->getZExtValue() > I32_MAX ||
					lefthand->getZExtValue() - righthand->getZExtValue() < I32_MIN)
					error_codegen("Overflow occured!", this->loc);
			if (l->getType() == builder.getInt64Ty())
				if (lefthand->getZExtValue() - righthand->getZExtValue() > I64_MAX ||
					lefthand->getZExtValue() - righthand->getZExtValue() < I64_MIN)
					error_codegen("Overflow occured!", this->loc);
		}*/
		auto sub_inst = Intrinsic::getDeclaration(module.get(), Intrinsic::ssub_with_overflow, tys);
		auto subc_inst = builder.CreateCall(sub_inst, v);
		Codegen::createRuntimeError("Overflow occured!", builder.CreateICmpEQ(builder.CreateExtractValue(subc_inst, ar1), builder.getInt1(0)), this->loc);
		return builder.CreateExtractValue(subc_inst, ar0);
	}
	case Op::Mul:
	{
		/*auto lefthand = Codegen::getValueInt(l);
		auto righthand = Codegen::getValueInt(r);
		if (lefthand && righthand) {
			if (l->getType() == builder.getInt32Ty())
				if (lefthand->getZExtValue() * righthand->getZExtValue() > I32_MAX ||
					lefthand->getZExtValue() * righthand->getZExtValue() < I32_MIN)
					error_codegen("Overflow occured!", this->loc);
			if (l->getType() == builder.getInt64Ty())
				if (lefthand->getZExtValue() * righthand->getZExtValue() > I64_MAX ||
					lefthand->getZExtValue() * righthand->getZExtValue() < I64_MIN)
					error_codegen("Overflow occured!", this->loc);
		}*/
		auto mul_inst = Intrinsic::getDeclaration(module.get(), Intrinsic::smul_with_overflow, tys);
		auto mulc_inst = builder.CreateCall(mul_inst, v);
		Codegen::createRuntimeError("Overflow occured!", builder.CreateICmpEQ(builder.CreateExtractValue(mulc_inst, ar1), builder.getInt1(0)), this->loc);
		return builder.CreateExtractValue(mulc_inst, ar0);
	}
	case Op::Div:
		if (!l->getType()->isFloatingPointTy() &&
			!r->getType()->isFloatingPointTy()
			) {
			auto ci = Codegen::getValueInt(r);
			if (ci && !ci->isZero())
				return builder.CreateSDiv(l, r);
			else if (!ci) {
				Value* zero = builder.getInt32(0);
				auto tup = Codegen::doMatchType(r, zero);
				r = std::get<0>(tup);
				zero = std::get<1>(tup);
				Codegen::createRuntimeError("Divide-by-zero detected!", 
					builder.CreateICmpNE(r, zero), this->loc);
				return builder.CreateSDiv(l, r);
			}
			else {
				error_codegen("Divide-by-zero detected!", this->loc);
			}
		}
		else
			return builder.CreateFDiv(l, r);
	case Op::RDiv:
		if (!l->getType()->isFloatingPointTy() &&
			!r->getType()->isFloatingPointTy()
			) {
			return builder.CreateSRem(l, r);
		}
		else
			return builder.CreateFRem(l, r);
	case Op::Xor:
			return builder.CreateXor(l, r);
	case Op::Or:
		return builder.CreateOr(l, r);
	case Op::And:
		return builder.CreateAnd(l, r);
	case Op::LShift:
		if (!l->getType()->isFloatingPointTy() &&
			!r->getType()->isFloatingPointTy()
			)
			return builder.CreateShl(l, r);
	case Op::RShift:
		if (!l->getType()->isFloatingPointTy() &&
			!r->getType()->isFloatingPointTy()
			)
			return builder.CreateAShr(l, r);
	case Op::LThan:
		if (!l->getType()->isFloatingPointTy() &&
			!r->getType()->isFloatingPointTy()
			)
			return builder.CreateICmpSLT(l, r);
		else
			return builder.CreateFCmpOLT(l, r);
	case Op::LThanEqual:
		if (!l->getType()->isFloatingPointTy() &&
			!r->getType()->isFloatingPointTy()
			)
			return builder.CreateICmpSLE(l, r);
		else
			return builder.CreateFCmpOLE(l, r);
	case Op::RThan:
		if (!l->getType()->isFloatingPointTy() &&
			!r->getType()->isFloatingPointTy()
			)
			return builder.CreateICmpSGT(l, r);
		else
			return builder.CreateFCmpOGT(l, r);
	case Op::RThanEqual:
		if (!l->getType()->isFloatingPointTy() &&
			!r->getType()->isFloatingPointTy()
			)
			return builder.CreateICmpSGE(l, r);
		else
			return builder.CreateFCmpOGE(l, r);
	case Op::EqualEqual:
		if (!l->getType()->isFloatingPointTy() &&
			!r->getType()->isFloatingPointTy()
			)
			return builder.CreateICmpEQ(l, r);
		else
			return builder.CreateFCmpOEQ(l, r);
	case Op::NotEqual:
		if (!l->getType()->isFloatingPointTy() &&
			!r->getType()->isFloatingPointTy()
			)
			return builder.CreateICmpNE(l, r);
		else
			return builder.CreateFCmpONE(l, r);
	case Op::Ampamp:
		return builder.CreateAnd(l, r);
	case Op::Pipepipe:
		return builder.CreateOr(l, r);
	default:
		return nullptr;
	}
}

Type* ASTBinOp::getType() {
	return this->curTy;
}

TypeAST ASTBinOp::getASTType() {
	return TypeAST::BinOp;
}