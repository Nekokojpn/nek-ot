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
	else if(ty_load->getStructName().startswith("1sys.list")) {
		auto idx_list = Codegen::getIndices(this->indexes, true, this->loc);
		return Codegen::getListfromIndex(ty_load, value, idx_list, this->loc);
	}
	else {
		auto ptr = builder.CreateLoad(builder.CreateStructGEP(value, 0));
		
		return builder.CreateConstGEP1_32(ptr, 0);
	}
}

Type* ASTIdentifierArrayElementBase::getType() {
	auto value = Codegen::getDefinedValue(name, this->loc);
	auto ty_load = value->getType()->getPointerElementType();
	if (ty_load->isArrayTy())
		return ty_load->getArrayElementType();
	else if (ty_load->getStructName().startswith("1sys.list")) {
		auto idx_list = Codegen::getIndices(this->indexes, true, this->loc);
		return builder.CreateLoad(Codegen::getListfromIndex(ty_load, value, idx_list, this->loc))->getType();
	}
	else {
		auto ptr = builder.CreateLoad(builder.CreateStructGEP(value, 0));

		return builder.CreateLoad(builder.CreateConstGEP1_32(ptr, 0))->getType();
	}
	return nullptr;
}

TypeAST ASTIdentifierArrayElementBase::getASTType() {
	return TypeAST::IdentifierArrayElementBase;
}