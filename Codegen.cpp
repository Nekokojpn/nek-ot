#pragma once
#include "nek-ot.h"

LLVMContext context;
IRBuilder<> builder(context);
std::unique_ptr<Module> module;
std::unique_ptr<DIBuilder> dbuilder;
std::unique_ptr<legacy::FunctionPassManager> fpm;

static std::map<std::string, FunctionCallee> functions_global;
static std::map<std::string, Value*> namedvalues_global;
static std::map<std::string, AllocaInst*> namedvalues_local;



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



Function* curfunc;
BasicBlock* curbb;


void init_parse() {
	module = make_unique<Module>("top", context);
	fpm = llvm::make_unique<legacy::FunctionPassManager>(module.get());
	fpm->add(createPromoteMemoryToRegisterPass());
	fpm->add(createInstructionCombiningPass());
	fpm->add(createReassociatePass());
	fpm->add(createGVNPass());
	fpm->add(createCFGSimplificationPass());
	
	fpm->doInitialization();
}
AllocaInst* createEntryBlockAlloca(Function* function, const std::string& name) {
	IRBuilder<> tmpB(&function->getEntryBlock(), function->getEntryBlock().begin());
	return tmpB.CreateAlloca(Type::getInt32Ty(context), nullptr, name.c_str());
}

void Parser::dump() {
	module->dump();
}
RType Parser::getTypeByName(std::string _name) { //INTのみ戻り値。
		return RType::Int;
}

Value* ASTIdentifier::codegen() { //globalとlocalの区別なし.
	auto value = namedvalues_local[name];
	if (!value) {
		error("Unsolved value name", "Unsolved value name", 0, 0);
	}
	return builder.CreateLoad(value, name);
	//return value;
}

Value* ASTValue::codegen() {
	return builder.getInt32(value);
}

Value* ASTStrLiteral::codegen() {
	return nullptr;
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
	//auto val = createEntryBlockAlloca(curfunc, name);
	builder.CreateStore(value,val);
	namedvalues_local[name] = val;
	//builder.CreateStore(value, builder.CreateAlloca(builder.getInt32Ty()));
	return value;
}
Value* ASTProto::codegen() {
	
	std::vector<Type*> putsArgs;
	for (int i = 0; i < args.size(); i++) {
		if (args[i] == AType::Int)
			putsArgs.push_back(builder.getInt32Ty());
	}
	ArrayRef<Type*>  argsRef(putsArgs);
	
	Type* ty = builder.getVoidTy();
	if (ret == RType::Int)
		ty = builder.getInt32Ty();
	else if (ret == RType::Void)
		ty = builder.getVoidTy();
	Function* mainFunc =
		Function::Create(FunctionType::get(ty,argsRef,false),
			Function::ExternalLinkage, name, module.get());
	
	if (name == "main") {
		curbb = BasicBlock::Create(context, "entry", mainFunc);
		builder.SetInsertPoint(curbb);
	}
	else {
		curbb = BasicBlock::Create(context, "", mainFunc);
		builder.SetInsertPoint(curbb);
	}
	
	unsigned idx = 0;
	for (auto& arg : mainFunc->args())
		arg.setName(identifier[idx++]);
		
	for (auto& arg : mainFunc->args()) {
		//auto* alloca = createEntryBlockAlloca(mainFunc, arg.getName());
		//DILocalVariable *di = dbuilder->createParameterVariable
		auto alloca = builder.CreateAlloca(Type::getInt32Ty(context));
		builder.CreateStore(&arg, alloca);
		namedvalues_local[arg.getName()] = alloca;
	}

	curfunc = mainFunc;
	functions_global[name] = mainFunc;
	
	
	return nullptr;
}
Value* ASTFunc::codegen() {
	auto pr = proto->codegen(); //Proto
	
	for (int i = 0; i < body.size(); i++) {
		body[i]->codegen();
	}
	//fpm->run(*curfunc);
	namedvalues_local.clear();

	return pr;
}
Value* ASTCall::codegen() {
	std::vector<Value*> types;
	for (int i = 0; i < args_expr.size(); i++) {
		types.push_back(args_expr[i]->codegen());
	}
	ArrayRef<Value*> argsRef(types);
	return builder.CreateCall(functions_global[name], argsRef,"calltmp");
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
		return cont; //要検討
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
Type* ASTRet::codegen1() {
	if (expr_p)
		builder.CreateRet(expr_p->codegen());
	else builder.CreateRetVoid();
	/*
	if(ret_type == RType::Nop)
		ret_type = Parser::getTypeByName(identifier->name);
	if (ret_type == RType::Void) {
		builder.CreateRetVoid();
		return builder.getVoidTy();
	}
	else if (!identifier)
		error("fn ret type is invalid.","",0,0);
	else if (ret_type == RType::Int) {
		builder.CreateRet(identifier->codegen());
		return builder.getInt32Ty();
	}
	else if (ret_type == RType::Char) {
		builder.CreateRet(identifier->codegen());
		return builder.getInt8Ty();
	}
	else if (ret_type == RType::String) {
		builder.CreateRet(identifier->codegen());
		return builder.getInt8PtrTy();
	}
	else if (ret_type == RType::Float) {
		builder.CreateRet(identifier->codegen());
		return builder.getFloatTy();
	}
	else if (ret_type == RType::Double) {
		builder.CreateRet(identifier->codegen());
		return builder.getDoubleTy();
	}
	else
		error("Unexpected token -->", "", 0, 0);
		*/
	return nullptr;
}
Value* ASTRet::codegen() {
	codegen1();
	return nullptr;
}
Value* ASTString::codegen() {
	return nullptr;
}

