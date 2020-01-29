#include "../nek-ot.hpp"

Value* ASTVarOp::codegen() {
	for (int i = 0; i < this->body.size(); i++) {
		Codegen::init_on_inst();
		this->body[i]->codegen();
	}
	return nullptr;
}

Type* ASTVarOp::getType() {
	return nullptr;
}

TypeAST ASTVarOp::getASTType() {
	return TypeAST::VarOp;
}