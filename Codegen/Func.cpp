#include "../nek-ot.hpp"

Value* ASTFunc::codegen() {
	//TODO nested func  change->BasicBloc
	auto pr = proto->codegen(); //Proto
	if(builder.GetInsertBlock()->getParent()->getReturnType() != builder.getVoidTy())
		retvalue = builder.CreateAlloca(builder.GetInsertBlock()->getParent()->getReturnType());

	for (int i = 0; i < body.size(); i++) {
		Codegen::init_on_inst();

		body[i]->codegen();
	}
	
	auto retbb = BasicBlock::Create(context, "", builder.GetInsertBlock()->getParent());
	builder.SetInsertPoint(retbb);
	if (retvalue) {
		if (retvalue->getAllocatedType() == ((Function*)pr)->getReturnType())
			builder.CreateRet(builder.CreateLoad(retvalue));
		else
			error_codegen("return type is invalid for the function!", this->loc);
	}
	else { //retun type is void
		if(((Function*)pr)->getReturnType() == builder.getVoidTy())
			builder.CreateRetVoid();
		else
			error_codegen("return type is invalid for the function!", this->loc);
	}
	for (auto bb : retbbs) {
		builder.SetInsertPoint(bb);
		builder.CreateBr(retbb);
	}
	retvalue = nullptr;
	retbbs.clear();
	namedvalues_local.clear();
	namedvalues_local_isinitialized.clear();
	return pr;
}

Type* ASTFunc::getType() {
	return nullptr;
}
TypeAST ASTFunc::getASTType() {
	return TypeAST::Func;
}