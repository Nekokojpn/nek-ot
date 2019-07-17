#pragma once
#include "nek-ot.h"

LLVMContext context;
IRBuilder<> builder(context);
std::unique_ptr<Module> module;

static std::map<std::string, FunctionCallee> functions_global;
static std::map<std::string, Value*> namedvalues_global;
static std::map<std::string, Value*> namedvalues_local;



void Sys::IO::CreateFunc() {
	//puts --ïWèÄèoóÕ
	std::vector<Type*> putsArgs;
	putsArgs.push_back(builder.getInt8Ty()->getPointerTo());
	ArrayRef<Type*>  argsRef(putsArgs);
	FunctionType* putsType =
		FunctionType::get(builder.getInt32Ty(), argsRef, false);
	FunctionCallee putsFunc = module->getOrInsertFunction("puts", putsType);
	functions_global["puts"] = putsFunc;
}

/*
LLVMContext& context = getContext();
IRBuilder<>& builder = getBuilder();
Module* module = getModule();
*/

//Internal Library Classes----->


//<-----


Function* curfunc;
BasicBlock* curbb;


void Parser::init_parse() {
	module = make_unique<Module>("top", context);
}
void Parser::dump() {
	module->dump();
}

Value* ASTIdentifier::codegen() { //globalÇ∆localÇÃãÊï Ç»Çµ.
	auto value = namedvalues_local[name];
	if (!value)
		error("Unsolved value name", "Unsolved value name", 0, 0);
	return builder.CreateLoad(value);
}

Value* ASTValue::codegen() {
	return builder.getInt32(value);
}

Value* ASTBinOp::codegen() {
	Value* l = lhs->codegen();
	Value* r = rhs->codegen();
	if (!l || !r)
		return nullptr;
	switch (op) {
	case Op::Plus:
		return builder.CreateAdd(l, r, "addtmp");
	case Op::Minus:
		return builder.CreateSub(l, r, "subtmp");
	case Op::Mul:
		return builder.CreateMul(l, r, "multmp");
	case Op::Div:
		return builder.CreateSDiv(l, r, "divtmp");
	default:
		return nullptr;
	}
}
Value* ASTBoolOp::codegen() {
	Value* l = lhs->codegen();
	Value* r = rhs->codegen();
	if (!l || !r)
		return nullptr;
	
	//auto a = builder.CreateLoad(l);
	//auto b = builder.CreateLoad(r);
	switch (bop) {
	case BOp::LThan:
		return builder.CreateICmpSLT(l, r);
	case BOp::LThanEqual:
		return builder.CreateICmpSLE(l, r);
	case BOp::RThan:
		return builder.CreateICmpSGT(l, r);
	case BOp::RThanEqual:
		return builder.CreateICmpSGE(l, r);
	case BOp::EqualEqual:
		return builder.CreateICmpEQ(l, r);
	case BOp::NotEqual:
		return builder.CreateICmpNE(l, r);
	default:
		return nullptr;
	}
}

Value* ASTInt::codegen() {
	auto value = expr_p->codegen();
	if (!value)
		return nullptr;
	auto val = builder.CreateAlloca(builder.getInt32Ty());
	builder.CreateStore(value,val);
	namedvalues_local[name] = val;
	//builder.CreateStore(value, builder.CreateAlloca(builder.getInt32Ty()));
	return value;
}
Value* ASTProto::codegen() {
	Function* mainFunc =
		Function::Create(FunctionType::get(Type::getVoidTy(context), false),
			Function::ExternalLinkage, name, module.get());
	curfunc = mainFunc;
	if (name == "main") {
		curbb = BasicBlock::Create(context, "entry", mainFunc);
		builder.SetInsertPoint(curbb);
	}
	else {
		curbb = BasicBlock::Create(context, "", mainFunc);
		builder.SetInsertPoint(curbb);
	}
	return nullptr;
}
Value* ASTFunc::codegen() {
	auto pr = proto->codegen(); //Proto
	for (int i = 0; i < body.size(); i++) {
		body[i]->codegen();
	}
	return pr;
}
Value* ASTArgProto::codegen() {
	return nullptr; 
}
Value* ASTElse::codegen() {
	for (int i = 0; i < body.size(); i++) {
		body[i]->codegen();
	}
	return nullptr;
}

Value* ASTIf::codegen() {
	auto astboolop = proto->codegen(); //--> BoolOp
	if (!astboolop)
		return nullptr;

	std::vector<BasicBlock*> blocks;
	BasicBlock* if_block = BasicBlock::Create(context,"if_block",curfunc);
	builder.SetInsertPoint(if_block);
	for (int i = 0; i < body.size(); i++) {
		body[i]->codegen();
	}
	blocks.push_back(if_block);
	builder.SetInsertPoint(curbb);
	if (ast_elif.size() == 0 && ast_else == nullptr) {
		BasicBlock* cont = BasicBlock::Create(context, "cont", curfunc);
		auto branch = builder.CreateCondBr(astboolop, if_block, cont);
		for (int i = 0; i < blocks.size(); i++) {
			builder.SetInsertPoint(blocks[i]);
			curbb = blocks[i];
			builder.CreateBr(cont);
		}
		curbb = cont;
		builder.SetInsertPoint(cont);
		return cont;
	}
	else {
		
		if (ast_elif.size() != 0) {
			for (int i = 0; i < ast_elif.size(); i++) {
				BasicBlock* elif_block = BasicBlock::Create(context, "elif_block", curfunc);
				auto branch = builder.CreateCondBr(astboolop, if_block, elif_block);
				builder.SetInsertPoint(elif_block);
				curbb = elif_block;
				
				builder.CreateCondBr(ast_elif[i]->proto->codegen(), if_block, elif_block);
				for (int j = 0; j < ast_elif[i]->body.size(); j++) {
					ast_elif[i]->body[j]->codegen();
					if (ast_elif[i]->ast_else) {
						BasicBlock* else_block = BasicBlock::Create(context, "else_block", curfunc);
						auto branch = builder.CreateCondBr(astboolop, if_block, else_block);
						builder.SetInsertPoint(else_block);
						curbb = else_block;
						ast_elif[i]->ast_else->codegen();
						blocks.push_back(else_block);
						builder.SetInsertPoint(elif_block);
					}
				}
				blocks.push_back(elif_block);
			}
		}
		auto cont = BasicBlock::Create(context, "cont", curfunc);
		if (ast_elif.size() != 0) {
			for (int i = 0; i < blocks.size(); i++) {
				builder.SetInsertPoint(blocks[i]);
				curbb = blocks[i];
				builder.CreateBr(cont);
			}
		}
		builder.SetInsertPoint(cont);
		return cont; //óvåüì¢
	}
}
Value* ASTWhile::codegen() {
	

	BasicBlock* while_block = BasicBlock::Create(context, "while_block", curfunc);
	BasicBlock* body_block = BasicBlock::Create(context, "body_block", curfunc);
	BasicBlock* cont_block = BasicBlock::Create(context, "cont", curfunc);
	builder.CreateBr(while_block);
	
	builder.SetInsertPoint(while_block);

	auto astboolop = proto->codegen(); //--> BoolOp
	if (!astboolop)
		return nullptr;
	
	builder.CreateCondBr(astboolop, body_block, cont_block);
	builder.SetInsertPoint(body_block);
	for (int i = 0; i < body.size(); i++) {
		body[i]->codegen();
	}
	builder.CreateBr(while_block);
	builder.SetInsertPoint(cont_block);
	curbb = cont_block;
	return astboolop;
}
Value* ASTSubst::codegen() {
	return builder.CreateStore(expr->codegen(), namedvalues_local[id->name]);
}
