#include "../nek-ot.hpp"

// This function prepares to generate a return IR. 
Value* ASTRet::codegen() {
	if (!lambdavalue) {
		retcodegen = true;
		if (expr_p) {
			auto v = expr_p->codegen();
			if (v->getType()->isPointerTy())
				v = builder.CreateLoad(v);
			builder.CreateStore(v, retvalue);
			retbbs.push_back(builder.GetInsertBlock());
		}
		else {
			retbbs.push_back(builder.GetInsertBlock());
		}
	}
	else {
		builder.CreateStore(this->expr_p->codegen(), lambdavalue);
	}
	return nullptr;
}

Type* ASTRet::getType() {
	return nullptr;
}

TypeAST ASTRet::getASTType() {
	return TypeAST::Ret;
}