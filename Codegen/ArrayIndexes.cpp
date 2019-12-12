#include "../nek-ot.hpp"

Value* ASTArrayIndexes::codegen() {
	return nullptr;
}

Type* ASTArrayIndexes::getType() {
	//get llvm type from index at 0 
	//!NEEDED! call the function before codegen 
	return nullptr;
}

TypeAST ASTArrayIndexes::getASTType() {
	return TypeAST::ArrayIndexes;
}