#include "../nek-ot.hpp"

Value* ASTIf::codegen() {
	auto astboolop = proto->codegen(); //--> BoolOp
	if (!astboolop)
		return nullptr;
	//type of astboolop must be i1.
	if (astboolop->getType()->isPointerTy())astboolop = builder.CreateLoad(astboolop);
	if (astboolop->getType() != builder.getInt1Ty())
		error_codegen("The expression is must be reutrn bool", this->loc);
	auto curfunc = builder.GetInsertBlock()->getParent();

	std::vector<BasicBlock*> blocks;
	BasicBlock* if_block = BasicBlock::Create(context,"",curfunc);
	auto curbb = builder.GetInsertBlock();
	builder.SetInsertPoint(if_block);
	auto tmp_retcodegen = retcodegen;
	retcodegen = false;
	gotocodegen = false;

	this->bodyBB = builder.GetInsertBlock();

	Value* finalret; 

	for (int i = 0; i < body.size(); i++) {
		Codegen::init_on_inst();
		finalret = body[i]->codegen();
	}
	if (if_rets.first) {
		if_rets.second.push_back(finalret);
		if_rets_bb.push_back(if_block);
	}

	//if(!retcodegen)
		//blocks.push_back(if_block);
	//retcodegen = false;
	auto before = builder.GetInsertBlock();
	builder.SetInsertPoint(curbb);
	//When exsists only if
	if (ast_elif == nullptr && ast_else == nullptr) {

		BasicBlock* cont = BasicBlock::Create(context, "", curfunc);
		builder.CreateCondBr(astboolop, if_block, cont);
		builder.SetInsertPoint(before);
		this->contBB = cont;

		if(!retcodegen && !gotocodegen)
			builder.CreateBr(cont);
		for (int i = 0; i < blocks.size(); i++) {
			builder.SetInsertPoint(blocks[i]);
			curbb = blocks[i];
			builder.CreateBr(cont);
		}
		curbb = cont;
		builder.SetInsertPoint(cont);
		retcodegen = tmp_retcodegen;
		return cont;
	}
	else {
		//When exists elif
		if (ast_elif != nullptr) {
			BasicBlock* elif_block = BasicBlock::Create(context, "", curfunc);
			builder.CreateCondBr(astboolop, if_block, elif_block);

			builder.SetInsertPoint(elif_block);
			curbb = elif_block;

			ast_elif->codegen();

			if (!retcodegen && !gotocodegen)
				blocks.push_back(elif_block);
			retcodegen = tmp_retcodegen;
			
		}
		//When exists else
		if (ast_else != nullptr) {
			BasicBlock* else_block = BasicBlock::Create(context, "", curfunc);
			builder.CreateCondBr(astboolop, if_block, else_block);

			builder.SetInsertPoint(else_block);
			curbb = else_block;
			if (if_rets.first)if_rets_bb.push_back(else_block);
			ast_else->codegen();
			if (!retcodegen && !gotocodegen)
				blocks.push_back(else_block);
			retcodegen = tmp_retcodegen;
		}
		if (blocks.size() != 0) {
			auto cont = BasicBlock::Create(context, "", curfunc);
			builder.SetInsertPoint(before);
			builder.CreateBr(cont);
			for (int i = 0; i < blocks.size(); i++) {
				builder.SetInsertPoint(blocks[i]);
				curbb = blocks[i];
				builder.CreateBr(cont);
			}
			retcodegen = tmp_retcodegen;
			builder.SetInsertPoint(cont);
		}
		return nullptr; 
	}
}

Type* ASTIf::getType() {
	return nullptr;
}

TypeAST ASTIf::getASTType() {
	return TypeAST::If;
}