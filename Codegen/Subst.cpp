#include "../nek-ot.hpp"

Value* ASTSubst::codegen() {
	if (this->id) {
		if(this->expr->getASTType() == TypeAST::If)
			if_rets.first = true;
		if (this->expr) {
			auto val = expr->codegen();
			if_rets.first = false;
			if (!isStringCodegen && if_rets.second.size() == 0 && val->getType()->isPointerTy())
				val = builder.CreateLoad(val);
			isSubst = true;
			auto ptr = id->codegen();
			isSubst = false;
			Value* ptr_ = ptr;
			if (if_rets.second.size() == 0 && !isStringCodegen) {
				if (ptr_->getType()->getPointerElementType() == val->getType())
					return Codegen::createStore(val, ptr_);
				else
					error_codegen("Does not match the type of the declared variable", this->loc);
			}
			else if (if_rets.second.size() > 0) {
				if (if_rets.second.size() < 2) {
					add_err_msg("Syntax: <variable> = if(<bool expression>) <expression>; else <expression>;");
					error_codegen("Expected --> else", this->loc);
				}
				if (if_rets_bb.size() != if_rets.second.size()) {
					error_codegen("Invalid if block!", this->loc);
				}
				auto phi = builder.CreatePHI(if_rets.second[0]->getType(), if_rets.second.size());
				for (int i = 0; i < if_rets.second.size(); i++) {
					if (phi->getType() == if_rets.second[i]->getType())
						phi->addIncoming(if_rets.second[i], if_rets_bb[i]);
					else
						error_codegen("if expressions must return the same type", this->loc);
				}
				return Codegen::createStore(phi, ptr_);
				if_rets.second.clear();
				if_rets_bb.clear();
			}
			else { // May be string
				return Codegen::createStore(val, ptr_);
			}
		}
	}
}

//TODO
Type* ASTSubst::getType() {
	return nullptr;
}

TypeAST ASTSubst::getASTType() {
	return TypeAST::Subst;
}