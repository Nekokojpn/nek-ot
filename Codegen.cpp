#pragma once
#include "nek-ot.h"

LLVMContext context;
IRBuilder<> builder(context);
std::unique_ptr<Module> module;
std::unique_ptr<legacy::FunctionPassManager> fpm;

static std::map<std::string, FunctionCallee> functions_global;
static std::map<std::string, AllocaInst*> namedvalues_global;
static std::map<std::string, AllocaInst*> namedvalues_local;
static std::map<std::string, Value*> namedvalues_str;

Value* lambdavalue;

AllocaInst* retvalue;
std::vector<BasicBlock*> retbbs;

bool retcodegen = false;

Module* getModule() {
	return module.get();
}


void Test::CreateFunc() {
	
}

void Sys::IO::OutPuti8Ptr::CreateFunc() {
	//puts --ïWèÄèoóÕ
	std::vector<Type*> putsArgs;
	putsArgs.push_back(builder.getInt8Ty()->getPointerTo());
	ArrayRef<Type*>  argsRef(putsArgs);
	FunctionType* putsType =
		FunctionType::get(builder.getInt32Ty(), argsRef, false);
	FunctionCallee putsFunc = module->getOrInsertFunction("puts", putsType);
	functions_global["puts"] = putsFunc;
}
void Sys::Cast::CastInt32toInt8ptr::CreateFunc() {
	//cast --i32 to i8*
	std::vector<Type*> putsArgs;
	putsArgs.push_back(builder.getInt32Ty());
	ArrayRef<Type*>  argsRef(putsArgs);
	Function* mainFunc =
		Function::Create(FunctionType::get(builder.getInt8Ty()->getPointerTo(), argsRef, false),
			Function::ExternalLinkage, "casti32toi8ptr", module.get());
	builder.SetInsertPoint(BasicBlock::Create(context, "", mainFunc));
	for (auto& arg : mainFunc->args()) {
		builder.CreateRet(builder.CreateIntToPtr(builder.CreateTruncOrBitCast(&arg, builder.getInt8Ty()), builder.getInt8Ty()->getPointerTo()));
		break;
	}
	//fpm->run(*mainFunc);
	functions_global["casti32toi8ptr"] = mainFunc;
}
void Sys::Cast::CastInt32toInt8Array::CreateFunc() {
	//cast --i32 to i8[]
	/*
	std::vector<Type*> putsArgs;
	putsArgs.push_back(builder.getInt32Ty());
	ArrayRef<Type*>  argsRef(putsArgs);

	Function* mainFunc =
		Function::Create(FunctionType::get(builder.getInt8Ty()->getArrayElementType(), argsRef, false),
			Function::ExternalLinkage, "casti32toi8array", module.get());
	curbb = BasicBlock::Create(context, "", mainFunc);
	builder.SetInsertPoint(curbb);
	for (auto& arg : mainFunc->args()) {
		builder.CreateRet(builder.CreateIntCast(&arg.getType(), builder.getInt8Ty()->getArrayElementType()));
		break;
	}
	fpm->run(*mainFunc);
	functions_global["casti32toi8array"] = mainFunc;
	*/
}
void Sys::IO::Printf::CreateFunc() {
	// create function
	llvm::Function* func;
	{
		// void writefln(const char*, ...)
		std::vector<llvm::Type*> args;
		args.push_back(builder.getInt8PtrTy());
		bool is_var_args = true;
		func = Function::Create(
			llvm::FunctionType::get(builder.getVoidTy(), args, is_var_args),
			llvm::Function::ExternalLinkage, "writefln", module.get());
		func->setCallingConv(llvm::CallingConv::C);
	}
	llvm::Argument* args_[1];
	assert(func->arg_size() == 1);
	int i = 0;
	for (auto itr = func->arg_begin(); itr != func->arg_end(); itr++) {
		args_[i++] = itr;
	}

	llvm::BasicBlock* entry = llvm::BasicBlock::Create(context, "", func);
	builder.SetInsertPoint(entry);
	functions_global["writefln"] = func;

	// declare wvsprintfA
	llvm::Function* wvsprintf_func;
	{
		// int32 wvsprintfA(char*, const char*, va_list)
		std::vector<llvm::Type*> args;
		args.push_back(builder.getInt8PtrTy());
		args.push_back(builder.getInt8PtrTy());
		args.push_back(builder.getInt8PtrTy());
		llvm::FunctionType* func_type = llvm::FunctionType::get(builder.getInt32Ty(), args, false);
		wvsprintf_func = llvm::Function::Create(
			func_type, llvm::Function::ExternalLinkage, "wvsprintfA", module.get());
		wvsprintf_func->setCallingConv(llvm::CallingConv::X86_StdCall);
	}
	functions_global["wvsprintfA"] = wvsprintf_func;
	// declare GetStdHandle
	llvm::Function* getstdhandle_func;
	{
		// void* GetStdHandle(uint32)
		llvm::FunctionType* func_type = llvm::FunctionType::get(
			builder.getInt8PtrTy(), llvm::ArrayRef<llvm::Type*>{builder.getInt32Ty()}, false);
		getstdhandle_func = llvm::Function::Create(
			func_type, llvm::Function::ExternalLinkage, "GetStdHandle", module.get());
		getstdhandle_func->setCallingConv(llvm::CallingConv::X86_StdCall);
	}
	functions_global["GetStdHandle"] = getstdhandle_func;
	// declare WriteConsoleA
	llvm::Function* writeconsolea_func;
	{
		// int32 WriteConsoleA(void*, const char*, uint32, uint32*, void*)
		std::vector<llvm::Type*> args;
		args.push_back(builder.getInt8PtrTy());
		args.push_back(builder.getInt8PtrTy());
		args.push_back(builder.getInt32Ty());
		args.push_back(builder.getInt32Ty()->getPointerTo());
		args.push_back(builder.getInt8PtrTy());
		llvm::FunctionType* func_type = llvm::FunctionType::get(
			builder.getInt32Ty(), args, false);
		writeconsolea_func = llvm::Function::Create(
			func_type, llvm::Function::ExternalLinkage, "WriteConsoleA", module.get());
		writeconsolea_func->setCallingConv(llvm::CallingConv::X86_StdCall);
	}
	functions_global["WriteConsoleA"] = writeconsolea_func;
	// char buf[256];
	llvm::Value* buf = builder.CreateAlloca(builder.getInt8Ty(), builder.getInt32(256));
	// int32 len_out[1];
	llvm::Value* len_out = builder.CreateAlloca(builder.getInt32Ty());
	// va_list ap[1];
	llvm::Value* ap_ = builder.CreateAlloca(builder.getInt8PtrTy());
	llvm::Value* ap = builder.CreateBitCast(ap_, builder.getInt8PtrTy());

	// va_start(ap);
	llvm::Function* vastart = llvm::Intrinsic::getDeclaration(module.get(), llvm::Intrinsic::vastart);
	builder.CreateCall(vastart, ap);

	// len = wvsprintfA(buf, fmt, ap);
	llvm::Value* len;
	{
		std::vector<llvm::Value*> args;
		args.push_back(buf);
		args.push_back(args_[0]);
		args.push_back(builder.CreateLoad(ap_));
		llvm::CallInst* inst = builder.CreateCall(wvsprintf_func, args);
		inst->setCallingConv(llvm::CallingConv::X86_StdCall);
		len = inst;
	}

	// va_end(ap);
	llvm::Function* vaend = llvm::Intrinsic::getDeclaration(module.get(), llvm::Intrinsic::vaend);
	builder.CreateCall(vaend, ap);

	// buf[len++] = '\r';
	// buf[len++] = '\n';
	builder.CreateStore(builder.getInt8('\r'), builder.CreateGEP(buf, len));
	len = builder.CreateAdd(len, builder.getInt32(1));
	builder.CreateStore(builder.getInt8('\n'), builder.CreateGEP(buf, len));
	len = builder.CreateAdd(len, builder.getInt32(1));

	// handle = GetStdHandle(-11);//stdout
	llvm::Value* handle;
	{
		llvm::CallInst* inst = builder.CreateCall(getstdhandle_func, builder.getInt32(-11));
		inst->setCallingConv(llvm::CallingConv::X86_StdCall);
		handle = inst;
	}

	// WriteConsoleA(handle, buf, len, len_out, null);
	{
		std::vector<llvm::Value*> args;
		args.push_back(handle);
		args.push_back(buf);
		args.push_back(len);
		args.push_back(len_out);
		args.push_back(llvm::ConstantPointerNull::getNullValue(builder.getInt8PtrTy()));
		llvm::CallInst* inst = builder.CreateCall(writeconsolea_func, args);
		inst->setCallingConv(llvm::CallingConv::X86_StdCall);
	}

	// return;
	builder.CreateRetVoid();
	//fpm->run(*func);
}

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

void Codegen::call_writefln(llvm::ArrayRef<llvm::Value*> args)
{
	llvm::Module* module = builder.GetInsertBlock()->getParent()->getParent();
	llvm::Function* func = module->getFunction("writefln");
	if (func == nullptr) {
		std::vector<llvm::Type*> args;
		args.push_back(builder.getInt8PtrTy());
		bool is_var_args = true;
		func = llvm::Function::Create(
			llvm::FunctionType::get(builder.getVoidTy(), args, is_var_args),
			llvm::Function::ExternalLinkage, "writefln", module);
		func->setCallingConv(llvm::CallingConv::C);
	}

	llvm::CallInst* inst = builder.CreateCall(func, args);
	inst->setCallingConv(func->getCallingConv());
}

void Parser::dump() {
	module->dump();
}
Type* Codegen::getTypebyAType(AType ty) {
	switch (ty)
	{
	case AType::Nop:
		return nullptr;
		break;
	case AType::I32:
		return builder.getInt32Ty();
		break;
	case AType::Float:
		return builder.getFloatTy();
		break;
	case AType::Double:
		return builder.getDoubleTy();
		break;
	case AType::Char:
		return builder.getInt8Ty();
		break;
	case AType::String:
		return builder.getInt8PtrTy();
		break;
	case AType::Void:
		return builder.getVoidTy();
		break;
	default:
		break;
	}
}

Value* ASTIdentifier::codegen() { //globalÇ∆localÇÃãÊï Ç»Çµ.
	auto value = namedvalues_local[name];
	if (!value) {
		auto global = namedvalues_global[name];
		if (!global) {
			auto str = namedvalues_str[name];
			if(!str)
				error("Unsolved value name", "Unsolved value name --> "+name, this->loc);
			return str;
		}
		return global;
	}
	return builder.CreateLoad(value);
	//return value;
}

Value* ASTIdentifierArrayElement::codegen() {
	auto value = namedvalues_local[name];
	Value* val = nullptr;
	if (!value) {
		value = namedvalues_global[name];
		if (!value) {
			val = namedvalues_str[name];
			if (!val)
				error("Unsolved value name", "Unsolved value name --> " + name, this->loc);
		}
	}
	std::vector<Value*> p;
	p.push_back(builder.getInt64(0));
	p.push_back(expr->codegen());
	ArrayRef<Value*> pp(p);
	if(!val)
		return builder.CreateLoad(builder.CreateInBoundsGEP(value, pp));
	else
		return builder.CreateLoad(builder.CreateInBoundsGEP(val, pp));
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
		return builder.CreateAdd(l, r);
	case Op::Minus:
		return builder.CreateSub(l, r);
	case Op::Mul:
		return builder.CreateMul(l, r);
	case Op::Div:
		return builder.CreateSDiv(l, r);
	case Op::LThan:
		return builder.CreateICmpSLT(l, r);
	case Op::LThanEqual:
		return builder.CreateICmpSLE(l, r);
	case Op::RThan:
		return builder.CreateICmpSGT(l, r);
	case Op::RThanEqual:
		return builder.CreateICmpSGE(l, r);
	case Op::EqualEqual:
		return builder.CreateICmpEQ(l, r);
	case Op::NotEqual:
		return builder.CreateICmpNE(l, r);
	default:
		return nullptr;
	}
}

Value* ASTType::codegen() {
	auto allocainst = builder.CreateAlloca(Codegen::getTypebyAType(this->ty));
	namedvalues_local[this->name] = allocainst;
	if (this->expr) {
		auto value = this->expr->codegen();
		if (!value)
			return nullptr;
		return value;
	}
	return allocainst;
}

Value* ASTIntArray::codegen() {
	auto value = builder.CreateAlloca(ArrayType::get(builder.getInt32Ty(),size));
	namedvalues_local[name] = value;
	return value;
}

Value* ASTProto::codegen() {
	
	std::vector<Type*> putsArgs;

	for (int i = 0; i < args.size(); i++) putsArgs.push_back(Codegen::getTypebyAType(args[i]));

	ArrayRef<Type*>  argsRef(putsArgs);
	
	Type* return_type = Codegen::getTypebyAType(this->ret);

	Function* mainFunc =
		Function::Create(FunctionType::get(return_type,argsRef,false),
			Function::ExternalLinkage, name, module.get());
	if (name == "main") {
		builder.SetInsertPoint(BasicBlock::Create(context, "entry", mainFunc));
	}
	else {
		builder.SetInsertPoint(BasicBlock::Create(context, "", mainFunc));
	}
	
	unsigned idx = 0;
	for (auto& arg : mainFunc->args())
		arg.setName(identifier[idx++]);
		
	for (auto& arg : mainFunc->args()) {
		auto alloca = builder.CreateAlloca(Type::getInt32Ty(context));
		builder.CreateStore(&arg, alloca);
		namedvalues_local[arg.getName()] = alloca;
	}
	mainFunc->setCallingConv(CallingConv::X86_StdCall);
	functions_global[name] = mainFunc;
	return nullptr;
}
Value* ASTFunc::codegen() {
	
	auto pr = proto->codegen(); //Proto
	retvalue = builder.CreateAlloca(builder.GetInsertBlock()->getParent()->getReturnType());

	for (int i = 0; i < body.size(); i++) {
		body[i]->codegen();
	}
	auto retbb = BasicBlock::Create(context, "ret", builder.GetInsertBlock()->getParent());
	builder.SetInsertPoint(retbb);

	builder.CreateRet(builder.CreateLoad(retvalue));
	for (auto bb : retbbs) {
		builder.SetInsertPoint(bb);
		builder.CreateBr(retbb);
	}

	retvalue = nullptr;
	retbbs.clear();
	//fpm->run(*builder.GetInsertBlock()->getParent());
	namedvalues_local.clear();

	return pr;
}
Value* ASTCall::codegen() {
	std::vector<Value*> types;
	for (int i = 0; i < args_expr.size(); i++) {
		types.push_back(args_expr[i]->codegen());
	}
	ArrayRef<Value*> argsRef(types);
	if (name == "writefln") {
		Codegen::call_writefln(argsRef);
		return nullptr;
	}
	else
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

	auto curfunc = builder.GetInsertBlock()->getParent();

	std::vector<BasicBlock*> blocks;
	BasicBlock* if_block = BasicBlock::Create(context,"if",curfunc);
	auto curbb = builder.GetInsertBlock();
	builder.SetInsertPoint(if_block);
	for (int i = 0; i < body.size(); i++) {
		body[i]->codegen();
	}
	if(!retcodegen)
		blocks.push_back(if_block);
	retcodegen = false;

	builder.SetInsertPoint(curbb);
	if (ast_elif == nullptr && ast_else == nullptr) {

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
		//elif Ç™ë∂ç›Ç∑ÇÈ.
		if (ast_elif != nullptr) {
			BasicBlock* elif_block = BasicBlock::Create(context, "elif", curfunc);
			builder.CreateCondBr(astboolop, if_block, elif_block);

			builder.SetInsertPoint(elif_block);
			curbb = elif_block;

			ast_elif->codegen();

			if(!retcodegen)
				blocks.push_back(elif_block);
			retcodegen = false;
			
		}
		if (ast_else != nullptr) {
			BasicBlock* else_block = BasicBlock::Create(context, "el", curfunc);
			builder.CreateCondBr(astboolop, if_block, else_block);

			builder.SetInsertPoint(else_block);
			curbb = else_block;
			ast_else->codegen();
			if (!retcodegen)
				blocks.push_back(else_block);
			retcodegen = false;
		}
		if (blocks.size() != 0) {
			auto cont = BasicBlock::Create(context, "cont", curfunc);
			for (int i = 0; i < blocks.size(); i++) {
				builder.SetInsertPoint(blocks[i]);
				curbb = blocks[i];
				builder.CreateBr(cont);
			}

			builder.SetInsertPoint(cont);
		}
		return nullptr; 
	}
}
Value* ASTWhile::codegen() {

	auto curfunc = builder.GetInsertBlock()->getParent();

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
	return astboolop;
}

Value* ASTSubst::codegen() {
	if (this->id) {
		if(this->expr)
			return builder.CreateStore(expr->codegen(), namedvalues_local[id->name]);
		else {
			lambdavalue = namedvalues_local[this->id->name];
			for (auto ast = body.begin(); ast != body.end(); ast++) {
				ast->get()->codegen();
			}
			lambdavalue = nullptr;
			return nullptr;
		}
	}
	else if (id2) {
		std::vector<Value*> p;
		p.push_back(builder.getInt64(0));
		p.push_back(id2->expr->codegen());
		ArrayRef<Value*> pp(p);
		return builder.CreateStore(expr->codegen(), builder.CreateInBoundsGEP(namedvalues_local[id2->name], pp));
	}
	else
		return nullptr;
}
Value* ASTRet::codegen() {
	retcodegen = true;
	if (!lambdavalue) {
		if (expr_p) {
			builder.CreateStore(expr_p->codegen(), retvalue);
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
Value* ASTStrLiteral::codegen() {
	return builder.CreateGlobalStringPtr(value);
}
Value* ASTString::codegen() { //óvèCê≥
	auto str = expr_str->value;
	auto ptr = builder.CreateGlobalStringPtr(str, name);
	namedvalues_str[name] = ptr;
	return ptr;
}

