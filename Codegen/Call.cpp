#include "../nek-ot.hpp"

Value* ASTCall::codegen() {


	std::vector<Value*> types;
	current_inst = nullptr;
	isPtr = false; 
	for (int i = 0; i < args_expr.size(); i++, current_inst = nullptr, isPtr = false) {
		auto ty = args_expr[i]->codegen();
		//auto curArg = module->getFunction(this->name)->getArg(i);
		//if (!curArg)
		//	error_codegen("Argument out of range.", this->loc);
		Value* ty_load = ty->getType()->isPointerTy() ? builder.CreateLoad(ty) : ty;
		//if (curArg->getType() != ty_load->getType())
		//	error_codegen("The argument " + std::to_string(i) + " is not match specified the type.", this->loc);

		if (!ty->getType()->isPointerTy()) {
			types.push_back(ty);
			continue;
		}
		else if (isStringCodegen) {
			isStringCodegen = false;
			types.push_back(ty);
			continue;
		}
		
		else if (current_inst && current_inst->getType()->isArrayTy() && !ty_load->getType()->isArrayTy()) {
			types.push_back(ty);
		}
		else if (ty_load->getType()->isArrayTy()) {
			
			auto array_ty = ty_load->getType()->getArrayElementType();
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
	if (name == "writef") {
		Codegen::call_writef(argsRef);
		return nullptr;
	}
	if (functions_global.find(this->name) == functions_global.end()) {
		std::string s = "There is no function name --> " + this->name;
		error_codegen(s, this->loc);
	}
	else {
		auto inst = builder.CreateCall(functions_global[this->name], argsRef, "");
		return inst;
	}
}

//TODO
Type* ASTCall::getType() {
	if (functions_global.find(this->name) != functions_global.end()) {
		return functions_global[this->name].getCallee()->getType();
	}
	else {
		std::string s = "There is no function name --> " + this->name;
		error_codegen(s, this->loc);
	}
	return nullptr;
}

TypeAST ASTCall::getASTType() {
	return TypeAST::Call;
}