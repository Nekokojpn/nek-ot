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
	auto if_block = Codegen::createBB();
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
	if (ast_elseif.size() == 0 && ast_else == nullptr) {

		BasicBlock* cont = Codegen::createBB();
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
		auto elsif = Codegen::createBB();
		builder.CreateCondBr(astboolop, if_block, elsif);
		builder.SetInsertPoint(elsif);
		if (ast_elseif.size() > 0) {
			
			for (int i = 0; i < ast_elseif.size(); i++) {
				auto pro = ast_elseif[i]->proto->codegen();
				auto tr = Codegen::createBB();
				blocks.push_back(tr);
				auto fa = Codegen::createBB();
				builder.CreateCondBr(pro, tr, fa);
				builder.SetInsertPoint(tr);
				for (int j = 0; j < ast_elseif[i]->body.size(); j++) {
					ast_elseif[i]->body[j]->codegen();
				}
				builder.SetInsertPoint(fa);
			}
		}
		//When exists else
		if (ast_else != nullptr) {
			curbb = builder.GetInsertBlock();
			if (if_rets.first)if_rets_bb.push_back(elsif);
			ast_else->codegen();
			blocks.push_back(builder.GetInsertBlock());
			retcodegen = tmp_retcodegen;
			auto cont = Codegen::createBB();
			builder.SetInsertPoint(cont);
		}
		if (blocks.size() != 0) {
			auto cont = builder.GetInsertBlock();
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