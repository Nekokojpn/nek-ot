#include "../nek-ot.hpp"

Value* ASTCall::codegen() {
	std::vector<Value*> types;
	current_inst = nullptr;
	isPtr = false; 
	for (int i = 0; i < args_expr.size(); i++, current_inst = nullptr, isPtr = false) {
		auto ty = args_expr[i]->codegen();
		Value* ty_load;
		if (ty->getType()->isPointerTy())
			ty_load = builder.CreateLoad(ty);
		else
			ty_load = ty;
		if (!ty->getType()->isPointerTy()) {
			types.push_back(ty);
			continue;
		}
		else if (isStringCodegen) {
			isStringCodegen = false;
			types.push_back(ty);
			continue;
		}
		
		else if (current_inst && current_inst->getAllocatedType()->isArrayTy() && !ty_load->getType()->isArrayTy()) {
			if (ty->getType()->isPointerTy())
				ty = builder.CreateLoad(ty);
			types.push_back(ty);
		}
		else if (current_inst && current_inst->getAllocatedType()->isArrayTy()) {
			
			auto array_ty = current_inst->getAllocatedType()->getArrayElementType();
			auto gep = builder.CreateConstGEP2_64(ty, 0, 0);
			while (array_ty->isArrayTy()) {
				gep = builder.CreateConstGEP2_64(gep, 0, 0);
				array_ty = array_ty->getArrayElementType();
			}
			types.push_back(gep);
			continue;
		}
		else if (!isPtr) {
			types.push_back(builder.CreateLoad(ty));
			continue;
		}
		else
			types.push_back(ty);
			
	}
	ArrayRef<Value*> argsRef(types);
	//The system calls
	if (name == "writefln") {
		Codegen::call_writef(argsRef);
		types.clear();
		types.push_back(builder.CreateGlobalStringPtr("\n"));
		ArrayRef<Value*> argsReff(types);
		Codegen::call_writef(argsReff);
		return nullptr;
	}
	else {
		if (functions_global.find(this->name) != functions_global.end()) {
			auto inst = builder.CreateCall(functions_global[name], argsRef, "");
			//inst->setCallingConv(CallingConv::X86_StdCall);
			return inst;
		}
		else {
			std::string s = "There is no function name --> " + this->name;
			error_codegen(s, this->loc);
		}
	}
}

//TODO
Type* ASTCall::getType() {
	return nullptr;
}