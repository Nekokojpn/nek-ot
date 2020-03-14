#include "../nek-ot.hpp"

Value* ASTIdentifierBase::codegen() {
	//For a stct control E.g, identifier.item = 10;
	/*Value* current_inst_load;
	if (current_inst && current_inst->getType()->isPointerTy())
		current_inst_load = builder.CreateLoad(current_inst);
	else
		current_inst_load = current_inst;
	*/
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
	if (!ty_load->isStructTy()) {
		//TODO
	}
}

Type* ASTIdentifierBase::getType() {
	auto value = Codegen::getLocalVal(name, this->loc);
	if (value)
		return value->getAllocatedType();
	auto global = Codegen::getGlobalVal(name, this->loc);
	if (global)
		return global->getType();
}

TypeAST ASTIdentifierBase::getASTType() {
	return TypeAST::IdentifierBase;
}