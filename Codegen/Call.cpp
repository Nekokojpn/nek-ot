#include "../nek-ot.hpp"

Value* ASTCall::codegen() {


	
	current_inst = nullptr;
	isPtr = false; 
	auto types = Codegen::genArgValues(this);
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