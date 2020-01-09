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
			auto len = builder.getInt32(ty_load->getArrayNumElements());
			Codegen::doMatchType(idx_list[i], len);
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
		//TODO: multi dimen
		auto bb = Codegen::createBB();
		auto tr = Codegen::createBB();
		auto fa = Codegen::createBB();

		/*
			struct mys stct;
			struct mys* pstct = &stct;
			for(int i = 0; i < 10; i++){
				pstct = pstct->next;
			}
			int suu = pstct->elm;
		*/

		//struct stct* pstct = &declaredstct;
		auto v_copy = builder.CreateAlloca(v_load->getType()->getPointerTo());
		builder.CreateStore(value, v_copy);

		//for(int i = 0;
		//    ^^^^^^^^^
		auto aloc = builder.CreateAlloca(builder.getInt32Ty());
		builder.CreateStore(builder.getInt32(0), aloc);
		
		builder.CreateBr(bb);
		builder.SetInsertPoint(bb);

		//for(int i = 0; i < idx[0];
		//               ^^^^^^^^^^
		auto cnt = builder.CreateLoad(aloc);
		Codegen::doMatchType(cnt, idx_list[0]);
		builder.CreateCondBr(builder.CreateICmpSLT(cnt, idx_list[0]), tr, fa);
		builder.SetInsertPoint(tr);

		//pstct = pstct->next;
		Codegen::createRuntimeError("List index out of range!", builder.CreateICmpEQ(builder.getIntN(2, 0), builder.CreateLoad(builder.CreateStructGEP(builder.CreateLoad(v_copy), 3))), this->loc);
		auto gep = builder.CreateStructGEP(builder.CreateLoad(v_copy), 2);
		builder.CreateStore(builder.CreateLoad(gep), v_copy);

		//for(int i = 0; i < idx[0]; i++
		//                           ^^^
		builder.CreateStore(builder.CreateAdd(cnt, builder.getInt32(1)), aloc);
		
		builder.CreateBr(bb);

		builder.SetInsertPoint(fa);
		return builder.CreateStructGEP(builder.CreateLoad(v_copy), 1);
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