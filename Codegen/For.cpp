#include "../nek-ot.hpp"

Value* ASTFor::codegen() {
	typedeff->codegen();
	auto bb = BasicBlock::Create(context, "", builder.GetInsertBlock()->getParent());
	builder.CreateBr(bb);

	builder.SetInsertPoint(bb);
	auto bb2 = BasicBlock::Create(context, "", builder.GetInsertBlock()->getParent());
	

	//builder.CreateBr(bb2);
	builder.SetInsertPoint(bb2);
	for (int i = 0; i < this->proto->body.size(); i++) {
		this->proto->body[i]->codegen();
	}
	last->codegen();
	builder.CreateBr(bb);

	auto cond = BasicBlock::Create(context, "", builder.GetInsertBlock()->getParent());

	builder.SetInsertPoint(bb);
	builder.CreateCondBr(this->proto->proto->codegen(), bb2, cond);
	builder.SetInsertPoint(cond);
	return nullptr;
}

Type* ASTFor::getType() {
	return nullptr;
}