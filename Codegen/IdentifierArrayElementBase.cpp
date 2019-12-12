#include "../nek-ot.hpp"

Value* ASTIdentifierArrayElementBase::codegen() {
	auto value = Codegen::getLocalVal(name, this->loc);
	if (!value)
		return Codegen::getGlobalVal(name, this->loc);

	if (isSubst || namedvalues_local_isinitialized[this->name] == true) {
		current_inst = value;
		if (isSubst)namedvalues_local_isinitialized[this->name] = true;
	}
	else {
		add_err_msg("Variables must be initialized before use.");
		error_codegen("Variable '" + this->name + "' is not initialized!", this->loc);
	}
	//Build a idx_list

	Value* gep = nullptr;
	bool isArrTy = false;
	if (value->getAllocatedType()->isPointerTy())isArrTy = true;
	else isArrTy = false;

	auto idx_list = Codegen::getIndices(this->indexes, isArrTy, this->loc);

	//TODO multi dimen;
	ArrayRef<Value*> v(idx_list);
	if (isArrTy) {
		gep = builder.CreateLoad(value);
		return builder.CreateInBoundsGEP(gep, v);
	}
	int i = idx_list.size() - 1;
	auto ci = Codegen::getValueInt(idx_list[i]);
	if (std::get<0>(ci)) {
		if (std::get<1>(ci) >= value->getAllocatedType()->getArrayNumElements())
			error_codegen("The specified index is out of range.", this->loc);
	}
	else {
		//‚±‚±
		/*
		BasicBlock* bb_t = BasicBlock::Create(context, "", builder.GetInsertBlock()->getParent());
		BasicBlock* bb_f = BasicBlock::Create(context, "", builder.GetInsertBlock()->getParent());
		std::vector<Value*> v;
		v.push_back(builder.CreateGlobalStringPtr("Error: The specified index is out of range."));
		ArrayRef<Value*> vv(v);
		Codegen::call_writefln(v);
		Codegen::call_error(1);
		builder.CreateCondBr(builder.CreateICmpSGE(idx_list[0], value), bb_t, bb_f);
		*/
	}

	gep = builder.CreateInBoundsGEP(value, v);
	return gep;
}

Type* ASTIdentifierArrayElementBase::getType() {
	auto value = Codegen::getLocalVal(name, this->loc);
	if (value)
		return value->getAllocatedType();
	auto global = Codegen::getGlobalVal(name, this->loc);
	if (global)
		return global->getType();
}

TypeAST ASTIdentifierArrayElementBase::getASTType() {
	return TypeAST::IdentifierArrayElementBase;
}