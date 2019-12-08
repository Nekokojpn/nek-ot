#include "../nek-ot.hpp"

Value* ASTLabel::codegen() {
	auto bb = BasicBlock::Create(context, "", builder.GetInsertBlock()->getParent());
	builder.CreateBr(bb);
	for (auto bb_ : jmp_bbs[this->label]) {
		
		builder.SetInsertPoint(bb_);
		builder.CreateBr(bb);
	}
	jmp_bbs[this->label].clear();
	builder.SetInsertPoint(bb);
	return nullptr;
}

Type* ASTLabel::getType() {
	return nullptr;
}