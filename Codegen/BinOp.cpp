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
	Codegen::doMatchType(l, r);
	this->curTy = l->getType();
	switch (op) {
	case Op::Plus:
		if (!l->getType()->isFloatingPointTy() &&
			!r->getType()->isFloatingPointTy()
			) {
			/*
			auto inst = builder.CreateAdd(l, r);
			Value* slt = nullptr;
			if (inst->getType() == builder.getInt32Ty())
				slt = builder.getInt32(I32_MAX);
			else if (inst->getType() == builder.getInt64Ty())
				slt = builder.getInt64(I64_MAX);
			auto bb_err = Codegen::createBB();
			auto bb_cont = Codegen::createBB();
			builder.CreateCondBr(builder.CreateICmpSLT(slt, inst), bb_err, bb_cont);
			builder.SetInsertPoint(bb_err);
			Codegen::createWritefln("Runtime Error: Overflow exception.");
			builder.CreateBr(bb);
			*/
			return builder.CreateAdd(l, r);
		}
		else
			return builder.CreateFAdd(l, r);
	case Op::Minus:
		if (!l->getType()->isFloatingPointTy() &&
			!r->getType()->isFloatingPointTy()
			)
			return builder.CreateSub(l, r);
		else
			return builder.CreateFSub(l, r);
	case Op::Mul:
		if (!l->getType()->isFloatingPointTy() &&
			!r->getType()->isFloatingPointTy()
			)
			return builder.CreateMul(l, r);
		else
			return builder.CreateFMul(l, r);
	case Op::Div:
		if (!l->getType()->isFloatingPointTy() &&
			!r->getType()->isFloatingPointTy()
			) {
			auto ci = Codegen::getValueInt(r);
			if (ci && !ci->isZero())
				return builder.CreateSDiv(l, r);
			else if (!ci) {
				auto zero = builder.getInt32(0);
				Codegen::doMatchType(r, zero);
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