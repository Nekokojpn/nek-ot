#include "../nek-ot.hpp"

Value* ASTBrk::codegen() {
	builder.CreateBr(brk_bbs[brk_bbs.size()-1]);
	return nullptr;
}

Type* ASTBrk::getType() {
	return nullptr;
}

TypeAST ASTBrk::getASTType() {
	return TypeAST::Brk;
}