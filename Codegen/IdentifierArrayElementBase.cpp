#include "../nek-ot.hpp"

Value* ASTIdentifierArrayElementBase::codegen() {
	auto value = Codegen::getDefinedValue(name, this->loc);
	auto ty_load = value->getType()->isPointerTy() ? value->getType()->getPointerElementType() : value->getType();

	if (isSubst || namedvalues_local_isinitialized[this->name] == true) {
		current_inst = value;
		if (isSubst)namedvalues_local_isinitialized[this->name] = true;
	}
	else {
		add_err_msg("Variables must be initialized before use.");
		error_codegen("Variable '" + this->name + "' is not initialized!", this->loc);
	}
	//ArrayTy.
	if (!ty_load->isStructTy()) {
		Value* gep = nullptr;
		bool isArrTy = false;
		if (ty_load->isPointerTy())isArrTy = true;
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
		if (ci && ci->getZExtValue() >= ty_load->getArrayNumElements()) {
			error_codegen("Array index out of range!", this->loc);
		}
		else {
			Value* len = builder.getInt32(ty_load->getArrayNumElements());
			auto tup = Codegen::doMatchType(idx_list[i], len);
			idx_list[i] = std::get<0>(tup);
			len = std::get<1>(tup);
			Codegen::createRuntimeError("Array index out of range!",
				builder.CreateICmpSLT(idx_list[i], len), this->loc);
		}
		gep = builder.CreateInBoundsGEP(value, v);
		return gep;
	}
	//List
	else {
		auto v_load = value->getType()->isPointerTy() ? builder.CreateLoad(value) : value;
		auto idx_list = Codegen::getIndices(this->indexes, true, this->loc);
		return Codegen::getListfromIndex(v_load->getType(), value, idx_list, this->loc);
	}
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