#pragma once
#include "nek-ot.h"

LLVMContext context;
IRBuilder<> builder(context);
std::unique_ptr<Module> module;

static std::map<std::string, FunctionCallee> functions_global;
static std::map<std::string, Value*> namedvalues_global;
static std::map<std::string, Value*> namedvalues_local;

std::map<std::string, Value*>&  getNamedValues_Local() {
	return namedvalues_local;
}

void Sys::IO::CreateFunc() {
	//puts --標準出力
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

Value* ASTIdentifier::codegen() { //globalとlocalの区別なし.
	return namedvalues_local[name];
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
	switch (bop) {
	case BOp::LThan:
		return builder.CreateICmpSLT(l, r, "iftemp");
	case BOp::LThanEqual:
		return builder.CreateICmpSLE(l, r, "addtmp");
	case BOp::RThan:
		return builder.CreateICmpSGT(l, r, "addtmp");
	case BOp::RThanEqual:
		return builder.CreateICmpSGE(l, r, "addtmp");
	case BOp::EqualEqual:
		return builder.CreateICmpEQ(l, r, "addtmp");
	case BOp::NotEqual:
		return builder.CreateICmpNE(l, r, "addtmp");
	default:
		return nullptr;
	}
}

Value* ASTInt::codegen() {
	auto value = expr_p->codegen();
	
	if (!value)
		return nullptr;
	namedvalues_local[name] = value;
	builder.CreateAlloca(builder.getInt32Ty(), value, name);
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
	return nullptr;
}

Value* ASTIf::codegen() {
	auto astboolop = proto->codegen(); //--> BoolOp
	if (!astboolop)
		return nullptr;
	//TODO define if elif BasicBlock 

	BasicBlock* if_block = BasicBlock::Create(context,"if_block",curfunc);
	BasicBlock* cont = BasicBlock::Create(context, "cont",curfunc);
	auto branch = builder.CreateCondBr(astboolop, if_block, cont);
	builder.SetInsertPoint(if_block);
	for (int i = 0; i < body.size(); i++) {
		body[i]->codegen();
	}
	if (ast_elif) {
		BasicBlock* elif_block = BasicBlock::Create(context, "elif_block", curfunc);
		auto branch = builder.CreateCondBr(astboolop, if_block, elif_block);
		builder.SetInsertPoint(elif_block);
		auto elif = ast_elif->codegen();
		builder.CreateBr(cont);
	}
	if(ast_else){
		BasicBlock* else_block = BasicBlock::Create(context, "else_block", curfunc);
		auto branch = builder.CreateCondBr(astboolop, if_block, else_block);
		builder.SetInsertPoint(else_block);
		auto elsec = ast_else->codegen();
		builder.CreateBr(cont);
	}	
	builder.SetInsertPoint(cont);
	return cont; //要件等
}
