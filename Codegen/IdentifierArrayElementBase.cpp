#include "../nek-ot.hpp"

Value* ASTIdentifierArrayElementBase::codegen() {
	auto value = namedvalues_local[this->name];
	if (!value) {
		auto global = namedvalues_global[this->name];
		if (!global) {
			auto str = namedvalues_str[this->name];
			if (!str) {
				if (underscore)
					return underscore;
				error("Unsolved value name", "Unsolved value name --> " + this->name, this->loc);
			}
			return str;
		}
		current_inst = global;
		if (current_inst->getType()->isPointerTy())
			current_inst = builder.CreateLoad(current_inst);
		return global;
	}
	if (isSubst || namedvalues_local_isinitialized[this->name] == true) {
		current_inst = value;
		if (current_inst->getType()->isPointerTy())
			current_inst = builder.CreateLoad(current_inst);
		if (isSubst)namedvalues_local_isinitialized[this->name] = true;
	}
	else {
		add_err_msg("Variables must be initialized before use.");
		error_codegen("Variable '" + this->name + "' is not initialized!", this->loc);
	}
	//Build a idx_list

	Value* gep = nullptr;
	if (value->getAllocatedType()->isPointerTy())isArrTy = true;
	else isArrTy = false;
	this->indexes->codegen();
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
	auto value = namedvalues_local[this->name];
	if (!value) {
		auto global = namedvalues_global[this->name];
		if (!global) {
			auto str = namedvalues_str[this->name];
			if (!str) {
				return nullptr;
			}
			return str->getType();
		}
		current_inst = global;
		if (current_inst->getType()->isPointerTy())
			current_inst = builder.CreateLoad(current_inst);
		return global->getAllocatedType();
	}
	return value->getAllocatedType();
}