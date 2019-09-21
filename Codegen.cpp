#pragma once
#include "nek-ot.h"

LLVMContext context;
IRBuilder<> builder(context);
std::unique_ptr<Module> module;
std::unique_ptr<legacy::FunctionPassManager> fpm;
std::unique_ptr<PassManagerBuilder> pmbuilder;

static std::map<std::string, FunctionCallee> functions_global;
static std::map<std::string, StructType*> userdefined_stcts;
static std::map<StructType*, std::map<std::string, Stct_t>> userdefined_stcts_elements;
static std::map<std::string, AllocaInst*> namedvalues_global;
static std::map<std::string, AllocaInst*> namedvalues_local;
static std::map<std::string, Value*> namedvalues_str;
static Value* underscore;

AllocaInst* curvar;
std::vector<double> curvar_v;
std::map<std::string, double> constantfoldings;

Value* lambdavalue;

AllocaInst* retvalue;
std::vector<BasicBlock*> retbbs;

bool opt = true;
bool retcodegen = false;


Module* getModule() {
	return module.get();
}

bool Parser::getOpt() {
	return opt;
}
void Parser::setOpt(bool b) {
	opt = b;
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
			llvm::Function::ExternalLinkage, "writef", module.get());
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
	functions_global["writef"] = func;

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
	if(opt)
		fpm->run(*func);
}

void Sys::IO::Printfln::CreateFunc() {
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
		llvm::CallInst* inst = builder.CreateCall(functions_global["wvsprintfA"], args);
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
		llvm::CallInst* inst = builder.CreateCall(functions_global["GetStdHandle"], builder.getInt32(-11));
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
		llvm::CallInst* inst = builder.CreateCall(functions_global["WriteConsoleA"], args);
		inst->setCallingConv(llvm::CallingConv::X86_StdCall);
	}

	// return;
	builder.CreateRetVoid();
	if (opt)
		fpm->run(*func);
}

void init_parse() {
	module = std::make_unique<Module>("top", context);
	fpm = std::make_unique<legacy::FunctionPassManager>(module.get());
	pmbuilder = std::make_unique<PassManagerBuilder>();
	pmbuilder->OptLevel = 2;
	pmbuilder->populateFunctionPassManager(*fpm);
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
void Codegen::call_writef(llvm::ArrayRef<llvm::Value*> args)
{
	llvm::Module* module = builder.GetInsertBlock()->getParent()->getParent();
	llvm::Function* func = module->getFunction("writef");
	if (func == nullptr) {
		std::vector<llvm::Type*> args;
		args.push_back(builder.getInt8PtrTy());
		bool is_var_args = true;
		func = llvm::Function::Create(
			llvm::FunctionType::get(builder.getVoidTy(), args, is_var_args),
			llvm::Function::ExternalLinkage, "writef", module);
		func->setCallingConv(llvm::CallingConv::C);
	}

	llvm::CallInst* inst = builder.CreateCall(func, args);
	inst->setCallingConv(func->getCallingConv());
}

void Sys::Internal::Chkstk::CreateFunc() {

}

void Parser::dump() {
	module->dump();
}

//std::string : TypeName, Type* : LLVM IR TYPE
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
		return nullptr;
		break;
	}
}
Type* Codegen::getTypebyAArrType(AArrType ty) {
	switch (ty)
	{
	case AArrType::Nop:
		return nullptr;
		break;
	case AArrType::I32:
		return builder.getInt32Ty();
		break;
	case AArrType::Float:
		return builder.getFloatTy();
		break;
	case AArrType::Double:
		return builder.getDoubleTy();
		break;
	case AArrType::Char:
		return builder.getInt8Ty();
		break;
	case AArrType::String:
		return builder.getInt8PtrTy();
		break;
	default:
		return nullptr;
		break;
	}
}

Value* ASTIdentifier::codegen() { //globalÇ∆localÇÃãÊï Ç»Çµ.
	auto value = namedvalues_local[name]; 
	if(constantfoldings[name])
		curvar_v.push_back(constantfoldings[name]);
	if (!value) {
		auto global = namedvalues_global[name];
		if (!global) {
			auto str = namedvalues_str[name];
			if (!str) {
				if(underscore)
					return underscore;
				error("Unsolved value name", "Unsolved value name --> " + name, this->loc);
			}
			return str;
		}
		return global;
	}
	return builder.CreateLoad(value);
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
	Value* gep;
	std::vector<Value*> p;
	p.push_back(builder.getInt64(0));
	p.push_back(expr_v[0]->codegen());
	ArrayRef<Value*> pp(p);
	if (value)
		gep = builder.CreateInBoundsGEP(value,pp);
	else
		gep = builder.CreateInBoundsGEP(val, pp);
	for (int i = 1; i < expr_v.size(); i++) {
		std::vector<Value*> p;
		p.push_back(builder.getInt64(0));
		p.push_back(expr_v[i]->codegen());
		ArrayRef<Value*> pp(p);
		gep = builder.CreateInBoundsGEP(gep, pp);
	}
	return builder.CreateLoad(gep);
	
}

Value* ASTValue::codegen() {
	/*
	if (curvar) {
		if (curvar->getType()->getElementType() == builder.getInt32Ty()) {
			if (this->value >= 2147483648)
				error("Compile error", "Overflow occurs", this->loc);
			else if (this->value < -2147483648LL)
				error("Compile error", "Exceeded i32 minimum.", this->loc);
		}
	}
	curvar_v.push_back(value);
	*/
	return builder.getInt32(value);
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

Value* ASTBinOp::codegen() {
	long size = curvar_v.size();
	Value* l = lhs->codegen();
	Value* r = rhs->codegen();
	if (!l || !r)
		return nullptr;
	if (l->getType()->isPointerTy())
		l = builder.CreateLoad(l);
	if (r->getType()->isPointerTy())
		r = builder.CreateLoad(r);
	switch (op) {
	case Op::Plus:
		
		/*
		if (l->getType() && size + 2 == curvar_v.size()) {
			curvar_v[curvar_v.size() - 2] = curvar_v[curvar_v.size() - 2] + curvar_v[curvar_v.size() - 1];
			if (l->getType() && l->getType() == builder.getInt32Ty()) {
				if (curvar_v[curvar_v.size() - 2] >= 2147483648)
					error("Compile error", "Exceeded i32 maximum.", this->loc);
			}
			curvar_v.pop_back();
		}
		*/
		return builder.CreateAdd(l, r);
	case Op::Minus:
		/*if (size + 2 == curvar_v.size()) {
			curvar_v[curvar_v.size() - 2] = curvar_v[curvar_v.size() - 2] - curvar_v[curvar_v.size() - 1];
			curvar_v.pop_back();
		}*/
		return builder.CreateSub(l, r);
	case Op::Mul:
		/*if (size + 2 == curvar_v.size()) {
			curvar_v[curvar_v.size() - 2] = curvar_v[curvar_v.size() - 2] * curvar_v[curvar_v.size() - 1];
			curvar_v.pop_back();
		}*/
		return builder.CreateMul(l, r);
	case Op::Div:
		/*if (size + 2 == curvar_v.size()) {
			curvar_v[curvar_v.size() - 2] = curvar_v[curvar_v.size() - 2] / curvar_v[curvar_v.size() - 1];
			curvar_v.pop_back();
		}*/
		return builder.CreateSDiv(l, r);
	case Op::RDiv:
		return builder.CreateSRem(l, r);
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
	if (this->arr_size_v.size() == 0) {
		AllocaInst* allocainst;
		if (this->ty != AType::Struct)
			allocainst = builder.CreateAlloca(Codegen::getTypebyAType(this->ty));
		else
			allocainst = builder.CreateAlloca(userdefined_stcts[this->stct_name]);
		namedvalues_local[this->name] = allocainst;
		if (this->expr) {
			auto value = this->expr->codegen();
			//constantfoldings[this->name] = curvar_v[0];
			//curvar_v.clear();
			if (!value)
				return nullptr;
			return value;
		}
		return allocainst;
	}
	else {
		ArrayType* ty = ArrayType::get(Codegen::getTypebyAType(this->ty), arr_size_v[arr_size_v.size() - 1]);
		long long cnt = 1;
		while (cnt < arr_size_v.size()) {
			ty = ArrayType::get(ty, arr_size_v[arr_size_v.size() - cnt]);
			cnt++;
		}
		auto allocainst = builder.CreateAlloca(ty);
		
		if (name != "_")
			namedvalues_local[name] = allocainst;
		else
			underscore = allocainst;

		if (this->elements) {
			this->elements->subst(allocainst, arr_size_v);
		}
		return allocainst;
	}
}

Value* ASTProto::codegen() {
	
	std::vector<Type*> putsArgs;

	for (int i = 0; i < args.size(); i++) putsArgs.push_back(Codegen::getTypebyAType(args[i]));

	ArrayRef<Type*>  argsRef(putsArgs);
	
	Type* return_type = Codegen::getTypebyAType(this->ret);

	Function* mainFunc =
		Function::Create(FunctionType::get(return_type, argsRef, false),
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
	mainFunc->setDSOLocal(true);
	mainFunc->setCallingConv(CallingConv::X86_StdCall);
	functions_global[name] = mainFunc;

	return mainFunc;
}
Value* ASTFunc::codegen() {
	
	auto pr = proto->codegen(); //Proto
	if(builder.GetInsertBlock()->getParent()->getReturnType() != builder.getVoidTy())
		retvalue = builder.CreateAlloca(builder.GetInsertBlock()->getParent()->getReturnType());

	for (int i = 0; i < body.size(); i++) {
		body[i]->codegen();
	}
	
	auto retbb = BasicBlock::Create(context, "", builder.GetInsertBlock()->getParent());
	builder.SetInsertPoint(retbb);
	module->dump();
	if (retvalue) {
		builder.CreateRet(builder.CreateLoad(retvalue));
	}
	else { //retun type is void
		builder.CreateRetVoid();
	}
	for (auto bb : retbbs) {
		builder.SetInsertPoint(bb);
		builder.CreateBr(retbb);
	}
	retvalue = nullptr;
	retbbs.clear();

	if(opt)
		fpm->run(*builder.GetInsertBlock()->getParent());
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
	if (name == "writef") {
		Codegen::call_writef(argsRef);
		return nullptr;
	}
	else {
		auto inst = builder.CreateCall(functions_global[name], argsRef, "");
		inst->setCallingConv(CallingConv::X86_StdCall);
		return inst;
	}
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
	BasicBlock* if_block = BasicBlock::Create(context,"",curfunc);
	auto curbb = builder.GetInsertBlock();
	builder.SetInsertPoint(if_block);
	retcodegen = false;
	for (int i = 0; i < body.size(); i++) {
		body[i]->codegen();
	}


	if(!retcodegen)
		//blocks.push_back(if_block);
	retcodegen = false;
	auto before = builder.GetInsertBlock();
	builder.SetInsertPoint(curbb);
	//When exsists only if
	if (ast_elif == nullptr && ast_else == nullptr) {

		BasicBlock* cont = BasicBlock::Create(context, "", curfunc);
		builder.CreateCondBr(astboolop, if_block, cont);
		builder.SetInsertPoint(before);
		builder.CreateBr(cont);
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
		//When exists elif
		if (ast_elif != nullptr) {
			BasicBlock* elif_block = BasicBlock::Create(context, "", curfunc);
			builder.CreateCondBr(astboolop, if_block, elif_block);

			builder.SetInsertPoint(elif_block);
			curbb = elif_block;

			ast_elif->codegen();

			if(!retcodegen)
				blocks.push_back(elif_block);
			retcodegen = false;
			
		}
		//When exists else
		if (ast_else != nullptr) {
			BasicBlock* else_block = BasicBlock::Create(context, "", curfunc);
			builder.CreateCondBr(astboolop, if_block, else_block);

			builder.SetInsertPoint(else_block);
			curbb = else_block;
			ast_else->codegen();
			if (!retcodegen)
				blocks.push_back(else_block);
			retcodegen = false;
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

			builder.SetInsertPoint(cont);
		}
		return nullptr; 
	}
}
Value* ASTFor::codegen() {
	return nullptr;
}

/*	Unfinished function. This function generates ir of while. elif is not supported.*/
Value* ASTWhile::codegen() {

	auto curfunc = builder.GetInsertBlock()->getParent();

	BasicBlock* while_block = BasicBlock::Create(context, "", curfunc);
	BasicBlock* body_block = BasicBlock::Create(context, "", curfunc);
	BasicBlock* cont_block = BasicBlock::Create(context, "", curfunc);
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
	
	/*  (Experimental) InlineAsm Implement LFENCE; for spectre.
	llvm::InlineAsm* IA = InlineAsm::get(FunctionType::get(builder.getVoidTy(),false), "LFENCE;", "~{dirflag},~{fpsr},~{flags}", true, false);
	std::vector<Value*> vec;
	ArrayRef<Value*> arg(vec);
	builder.CreateCall(IA, arg);
	*/
	return astboolop;
}
Value* ASTAction::codegen() {
	return nullptr;
}

Value* ASTSubst::codegen() {
	if (this->id) {
		//curvar = namedvalues_local[id->name];
		if (this->expr) {
			if(namedvalues_local[id->name])
				return builder.CreateStore(expr->codegen(), namedvalues_local[id->name]);
			else {
				error("Compile error:", "Undefined value --> " + id->name, this->loc);
			}
		}
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
		return builder.CreateStore(this->expr->codegen(), id2->codegen());
	}
	else
		return nullptr;
}
// This function prepares to generate a return IR. 
Value* ASTRet::codegen() {
	if (!lambdavalue) {
		retcodegen = true;
		if (expr_p) {
			auto v = expr_p->codegen();
			if (v->getType()->isPointerTy())
				v = builder.CreateLoad(v);
			builder.CreateStore(v, retvalue);
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
Value* ASTStruct::codegen() {
	ArrayRef<Type*> elements = this->elements->make_aref();
	auto stct = StructType::create(context,this->name);
	stct->setBody(elements);
	userdefined_stcts[this->name] = stct;
	return nullptr;
}
Value* ASTArrElements::subst(Value* arr, std::vector<long long> arr_size_v) {
	//TODO : Does not supported multidimentional array.
	Value* gep;
	if (this->elements.size() > 0) {
		for (int i = 0; i < elements.size(); i++) {
			std::vector<Value*> p;
			p.push_back(builder.getInt64(0));
			p.push_back(builder.getInt64(i));
			ArrayRef<Value*> pp(p);
			gep = builder.CreateInBoundsGEP(arr, pp);
			builder.CreateStore(this->elements[i]->codegen(), gep);
		}
	}
	else {
		Value* gep_main;
		auto arr_child = this->arr_type->codegen();
		for (auto i = 0ULL; i < arr_type->arr_size_v[0]; i++) {
			std::vector<Value*> p;
			p.push_back(builder.getInt64(0));
			p.push_back(builder.getInt64(i));
			ArrayRef<Value*> pp(p);
			gep_main = builder.CreateInBoundsGEP(arr, pp);
			gep = builder.CreateInBoundsGEP(arr_child, pp);
			underscore = gep;
			builder.CreateStore(this->restraint->codegen(), gep_main);
			underscore = nullptr;
		}
	}
	return nullptr;
}
Value* ASTArrElements::codegen() {

	return nullptr;
}
ArrayRef<Type*> ASTStctElements::make_aref(){
	std::vector<Type*> elem_v(this->elements.size());
	for (int i = 0; i < elements.size(); i++) {
		elem_v[i] = Codegen::getTypebyAType(this->elements[i].first);
	}
	ArrayRef<Type*> elements(elem_v);
	return elements;
}
Value* ASTStctElements::codegen() {
	return nullptr;
}
Value* ASTIdentifierStctElement::codegen() {
	if(!userdefined_stcts[this->name])
		error("Compile error:", "Undefined value --> " + this->name, this->loc);
	if(!namedvalues_local[this->name])
		error("Compile error:", "Undefined stct --> " + this->name, this->loc);
	auto cur = userdefined_stcts_elements[userdefined_stcts[this->name]];
	auto gep = builder.CreateStructGEP(namedvalues_local[this->name], cur[elem_names[0]].idx);
	for (int i = 1; i < this->elem_names.size(); i++) {
		
		std::vector<Value*> vv;
		vv.push_back(builder.getInt64(0));
		vv.push_back(builder.getInt64(cur[elem_names[i]].idx));
		ArrayRef<Value*> v(vv);
		
		gep = builder.CreateStructGEP(gep, cur[elem_names[i]].idx);
	}
	return builder.CreateLoad(gep);
}

