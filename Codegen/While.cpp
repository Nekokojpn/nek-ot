#include "../nek-ot.hpp"

/*	Unfinished function. This function generates ir of while. elif is not supported.*/
Value* ASTWhile::codegen() {

	auto curfunc = builder.GetInsertBlock()->getParent();

	BasicBlock* while_block = BasicBlock::Create(context, "", curfunc);
	BasicBlock* body_block = BasicBlock::Create(context, "", curfunc);
	BasicBlock* cont_block = BasicBlock::Create(context, "", curfunc);
	brk_bbs.push_back(cont_block);
	builder.CreateBr(while_block);
	
	builder.SetInsertPoint(while_block);

	auto astboolop = proto->codegen(); //--> BoolOp
	if (!astboolop)
		return nullptr;
	
	builder.CreateCondBr(astboolop, body_block, cont_block);
	builder.SetInsertPoint(body_block);
	for (int i = 0; i < body.size(); i++) {
		Codegen::init_on_inst();
		body[i]->codegen();
	}
	builder.CreateBr(while_block);
	builder.SetInsertPoint(cont_block);
	brk_bbs.pop_back();
	return astboolop;
}

Type* ASTWhile::getType() {
	return nullptr;
}

TypeAST ASTWhile::getASTType() {
	return TypeAST::While;
}