#include "../nek-ot.hpp"

Value* ASTArrayIndexes::codegen() {
	//Disired identifier already set to current_inst
	std::vector<Value*> ind;
	if (!isArrTy)
		ind.push_back(builder.getInt32(0));
	auto l = this->lhs->codegen();
	if (l->getType()->isPointerTy())
		l = builder.CreateLoad(l);
	ind.push_back(l);
	idx_list = ind;
	return nullptr;
}

Type* ASTArrayIndexes::getType() {
	//get llvm type from index at 0 
	//!NEEDED! call the function before codegen 
	return idx_list[0]->getType();
}

TypeAST ASTArrayIndexes::getASTType() {
	return TypeAST::ArrayIndexes;
}