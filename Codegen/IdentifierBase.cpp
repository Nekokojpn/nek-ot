#include "../nek-ot.hpp"

Value* ASTIdentifierBase::codegen() {
	//For a stct control E.g, identifier.item = 10;
	Value* current_inst_load;
	if (current_inst && current_inst->getType()->isPointerTy())
		current_inst_load = builder.CreateLoad(current_inst);
	else
		current_inst_load = current_inst;
	if (current_inst_load && current_inst_load->getType()->isStructTy()) {
		auto stct = userdefined_stcts_elements[current_inst_load->getType()->getStructName()];
		//TODO: Type check
		if (stct.elems.find(this->name) != stct.elems.end()) {
			//TODO current_inst must be ptr
			auto gep = builder.CreateStructGEP(current_inst, stct.elems[this->name].idx);
			current_inst = gep;
			return gep;
		}
		else {
			error_codegen(static_cast<std::string>(current_inst_load->getType()->getStructName()) + " is not stct or undefined var.", this->loc);
			return nullptr;
		}

	}
	//For a var control E.g, identifier = 10;
	else {
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
			return global;
		}
		if (isSubst || namedvalues_local_isinitialized[this->name] == true) {
			current_inst = value;
			if (isSubst)namedvalues_local_isinitialized[this->name] = true;
			return value;
		}
		else {
			add_err_msg("Variables must be initialized before use.");
			error_codegen("Variable '" + this->name + "' is not initialized!", this->loc);
		}
	}
}

Type* ASTIdentifierBase::getType() {
	auto value = namedvalues_local[this->name];
	if (!value) {
		auto global = namedvalues_global[this->name];
		if (!global) {
			auto str = namedvalues_str[this->name];
			if (!str) {
				if (underscore)
					return underscore->getType();
				return nullptr;
			}
			return str->getType();
		}
		current_inst = global;
		return global->getAllocatedType();
	}
	return value->getAllocatedType();
}

TypeAST ASTIdentifierBase::getASTType() {
	return TypeAST::IdentifierBase;
}