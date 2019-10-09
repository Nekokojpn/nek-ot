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
static std::map<std::string, BasicBlock*> jmp_labels;
static std::map<std::string, std::vector<BasicBlock*>> jmp_bbs;

//AST Identifier
AllocaInst* current_inst;
bool isPtr = false;

AllocaInst* curvar;
std::vector<double> curvar_v;
std::map<std::string, double> constantfoldings;

Value* lambdavalue;

AllocaInst* retvalue;
std::vector<BasicBlock*> retbbs;

std::vector<BasicBlock*> brk_bbs;

bool opt = true;
bool retcodegen = false;
auto isStringCodegen = false;


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
	llvm::Function* test_func;
	{
		std::vector<llvm::Type*> args;
		args.push_back(builder.getInt32Ty());
		args.push_back(builder.getInt32Ty());
		llvm::FunctionType* func_type = llvm::FunctionType::get(builder.getInt32Ty(), args, false);
		test_func = llvm::Function::Create(
			func_type, llvm::Function::ExternalLinkage, "test", module.get());
		test_func->setCallingConv(llvm::CallingConv::X86_StdCall);
	}
	functions_global["test"] = test_func;
	return;
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

void Sys::IO::Input::CreateFunc() {
	llvm::Function* input_func;
	{
		std::vector<llvm::Type*> args;
		args.push_back(builder.getInt32Ty()->getPointerTo());
		llvm::FunctionType* func_type = llvm::FunctionType::get(builder.getVoidTy(), args, false);
		input_func = llvm::Function::Create(
			func_type, llvm::Function::ExternalLinkage, "input", module.get());
		input_func->setCallingConv(llvm::CallingConv::X86_StdCall);
	}
	functions_global["input"] = input_func;
	return;
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
void Codegen::gen_asm(std::string statement, std::string option) {
	llvm::InlineAsm* IA = InlineAsm::get(FunctionType::get(builder.getVoidTy(), false), statement, option, true, false);
	std::vector<Value*> vec;
	ArrayRef<Value*> arg(vec);
	builder.CreateCall(IA, arg);
	return;
}

void Parser::dump() {
	module->dump();
}

//std::string : TypeName, Type* : LLVM IR TYPE
Type* Codegen::getTypebyAType(AType& ty) {
	switch (ty)
	{
	case AType::Nop:
		return nullptr;
		break;
	case AType::I16:
		return builder.getInt16Ty();
		break;
	case AType::I32:
		return builder.getInt32Ty();
		break;
	case AType::I64:
		return builder.getInt64Ty();
		break;
	case AType::F32:
		return builder.getDoubleTy();
		break;
	case AType::F64:
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
Type* Codegen::getTypebyType(Type_t& t) {
	auto ty = getTypebyAType(t.ty);
	if (t.kind == TypeKind::Pointer)
		ty = ty->getPointerTo();
	return ty;
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
				if (underscore)
					return underscore;
				error("Unsolved value name", "Unsolved value name --> " + name, this->loc);
			}
			return str;
		}
		current_inst = global;
		return global;
	}
	current_inst = value;
	if (this->kind == TypeKind::Pointer) {
		isPtr = true;
		if (value->getAllocatedType()->isPointerTy()) {
			isPtr = false;
			return builder.CreateLoad(value);
		}
	}
	else if (this->kind == TypeKind::Reference)
		return builder.CreateLoad(value);
	else
		isPtr = false;
	if (!value->getAllocatedType()->isArrayTy())
		return value;
	else
		return value;
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
	else {
		// is array type
		if (!value->getAllocatedType()->isArrayTy()) {
			val = builder.CreateLoad(value);
			value = nullptr;
		}
	}
	current_inst = value;
	if (this->kind == TypeKind::Pointer)
		isPtr = true;
	else
		isPtr = false;
	Value* gep;
	std::vector<Value*> p;
	if(value)
		p.push_back(builder.getInt64(0));
	if (expr_v[0]) current_inst = nullptr;
	auto index = expr_v[0]->codegen();
	if (index->getType()->isPointerTy())
		index = builder.CreateLoad(index);
	p.push_back(index);
	ArrayRef<Value*> pp(p);
	if(value)
		gep = builder.CreateInBoundsGEP(value, pp);
	else
		gep = builder.CreateInBoundsGEP(val, pp);
	for (int i = 1; i < expr_v.size(); i++) {
		std::vector<Value*> p;
		if(value)
			p.push_back(builder.getInt64(0));
		index = expr_v[i]->codegen();
		if (index->getType()->isPointerTy())
			index = builder.CreateLoad(index);
		p.push_back(index);
		ArrayRef<Value*> pp(p);
		gep = builder.CreateInBoundsGEP(gep, pp);
	}
	return gep;
	
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
	if (!this->isDouble)
		return builder.getInt32(value);
	else
		return ConstantFP::get(context, APFloat(value_d));
}

Value* ASTStrLiteral::codegen() {
	return builder.CreateGlobalStringPtr(value);
}

Value* ASTString::codegen() { //óvèCê≥
	isStringCodegen = true;
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
	if (!isPtr && l->getType()->isPointerTy())
		l = builder.CreateLoad(l);
	if (!isPtr && r->getType()->isPointerTy())
		r = builder.CreateLoad(r);
	if (l->getType()->isFloatingPointTy())
		if (!r->getType()->isFloatingPointTy())
			r = builder.CreateSIToFP(r, builder.getDoubleTy());
	if (r->getType()->isFloatingPointTy())
		if (!l->getType()->isFloatingPointTy())
			l = builder.CreateSIToFP(l, builder.getDoubleTy());
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
		if (!l->getType()->isFloatingPointTy() &&
			!r->getType()->isFloatingPointTy()
			)
			return builder.CreateAdd(l, r);
		else
			return builder.CreateFAdd(l, r);
	case Op::Minus:
		/*if (size + 2 == curvar_v.size()) {
			curvar_v[curvar_v.size() - 2] = curvar_v[curvar_v.size() - 2] - curvar_v[curvar_v.size() - 1];
			curvar_v.pop_back();
		}*/
		if (!l->getType()->isFloatingPointTy() &&
			!r->getType()->isFloatingPointTy()
			)
			return builder.CreateSub(l, r);
		else
			return builder.CreateFSub(l, r);
	case Op::Mul:
		/*if (size + 2 == curvar_v.size()) {
			curvar_v[curvar_v.size() - 2] = curvar_v[curvar_v.size() - 2] * curvar_v[curvar_v.size() - 1];
			curvar_v.pop_back();
		}*/
		if (!l->getType()->isFloatingPointTy() &&
			!r->getType()->isFloatingPointTy()
			)
			return builder.CreateMul(l, r);
		else
			return builder.CreateFMul(l, r);
	case Op::Div:
		/*if (size + 2 == curvar_v.size()) {
			curvar_v[curvar_v.size() - 2] = curvar_v[curvar_v.size() - 2] / curvar_v[curvar_v.size() - 1];
			curvar_v.pop_back();
		}*/
		if (!l->getType()->isFloatingPointTy() &&
			!r->getType()->isFloatingPointTy()
			)
			return builder.CreateSDiv(l, r);
		else
			return builder.CreateFDiv(l, r);
	case Op::RDiv:
		if (!l->getType()->isFloatingPointTy() &&
			!r->getType()->isFloatingPointTy()
			)
			return builder.CreateSRem(l, r);
		else
			return builder.CreateFRem(l, r);
	case Op::Xor:
			return builder.CreateXor(l, r);
	case Op::Or:
		return builder.CreateOr(l, r);
	case Op::And:
		return builder.CreateAnd(l, r);
	case Op::LShift:
		if (!l->getType()->isFloatingPointTy() &&
			!r->getType()->isFloatingPointTy()
			)
			return builder.CreateShl(l, r);
	case Op::RShift:
		if (!l->getType()->isFloatingPointTy() &&
			!r->getType()->isFloatingPointTy()
			)
			return builder.CreateAShr(l, r);
	case Op::LThan:
		if (!l->getType()->isFloatingPointTy() &&
			!r->getType()->isFloatingPointTy()
			)
			return builder.CreateICmpSLT(l, r);
		else
			return builder.CreateFCmpOLT(l, r);
	case Op::LThanEqual:
		if (!l->getType()->isFloatingPointTy() &&
			!r->getType()->isFloatingPointTy()
			)
			return builder.CreateICmpSLE(l, r);
		else
			return builder.CreateFCmpOLE(l, r);
	case Op::RThan:
		if (!l->getType()->isFloatingPointTy() &&
			!r->getType()->isFloatingPointTy()
			)
			return builder.CreateICmpSGT(l, r);
		else
			return builder.CreateFCmpOGT(l, r);
	case Op::RThanEqual:
		if (!l->getType()->isFloatingPointTy() &&
			!r->getType()->isFloatingPointTy()
			)
			return builder.CreateICmpSGE(l, r);
		else
			return builder.CreateFCmpOGE(l, r);
	case Op::EqualEqual:
		if (!l->getType()->isFloatingPointTy() &&
			!r->getType()->isFloatingPointTy()
			)
			return builder.CreateICmpEQ(l, r);
		else
			return builder.CreateFCmpOEQ(l, r);
	case Op::NotEqual:
		if (!l->getType()->isFloatingPointTy() &&
			!r->getType()->isFloatingPointTy()
			)
			return builder.CreateICmpNE(l, r);
		else
			return builder.CreateFCmpONE(l, r);
	case Op::Ampamp:
		return builder.CreateAnd(l, r);
	case Op::Pipepipe:
		return builder.CreateOr(l, r);
	default:
		return nullptr;
	}
}

Value* ASTType::codegen() {
	if (!this->ty.isArr) {
		AllocaInst* allocainst;
		//If local variable
		if (!this->isGlobal) {
			if (this->ty.ty != AType::Struct)
				allocainst = builder.CreateAlloca(Codegen::getTypebyType(this->ty));
			else
				allocainst = builder.CreateAlloca(userdefined_stcts[this->stct_name]);
			namedvalues_local[this->name] = allocainst;
		}
		//If global variable
		else {
			if (this->ty.ty != AType::Struct)
				allocainst = builder.CreateAlloca(Codegen::getTypebyType(this->ty));
			else
				allocainst = builder.CreateAlloca(userdefined_stcts[this->stct_name]);
			namedvalues_local[this->name] = allocainst;
		}
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
		ArrayType* ty = ArrayType::get(Codegen::getTypebyType(this->ty), this->ty.arrsize[this->ty.arrsize.size() - 1]);
		long long cnt = 1;
		while (cnt < this->ty.arrsize.size()) {
			ty = ArrayType::get(ty, this->ty.arrsize[this->ty.arrsize.size() - cnt]);
			cnt++;
		}
		auto allocainst = builder.CreateAlloca(ty);
		
		if (name != "_")
			namedvalues_local[name] = allocainst;
		else
			underscore = allocainst;

		if (this->elements) {
			this->elements->subst(allocainst, this->ty.arrsize);
		}
		return allocainst;
	}
}

Value* ASTProto::codegen() {
	std::vector<Type*> putsArgs;

	for (int i = 0; i < args.size(); i++) {
		Type* ty;
		if (args[i].isArr == true)
			ty = Codegen::getTypebyType(args[i])->getPointerTo();
		else if (args[i].kind == TypeKind::Pointer)
			ty = Codegen::getTypebyType(args[i])->getPointerTo();
		else
			ty = Codegen::getTypebyType(args[i]);
		putsArgs.push_back(ty);
	}

	ArrayRef<Type*>  argsRef(putsArgs);
	
	Type* return_type = Codegen::getTypebyType(this->ret);

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
		auto alloca = builder.CreateAlloca(arg.getType());
		builder.CreateStore(&arg, alloca);
		namedvalues_local[arg.getName()] = alloca;
	}
	mainFunc->setDSOLocal(true);
	mainFunc->setCallingConv(CallingConv::X86_StdCall);
	functions_global[name] = mainFunc;

	return mainFunc;
}
Value* ASTFunc::codegen() {
	//TODO nested func  change->BasicBloc
	auto pr = proto->codegen(); //Proto
	if(builder.GetInsertBlock()->getParent()->getReturnType() != builder.getVoidTy())
		retvalue = builder.CreateAlloca(builder.GetInsertBlock()->getParent()->getReturnType());

	for (int i = 0; i < body.size(); i++) {
		body[i]->codegen();
	}
	
	auto retbb = BasicBlock::Create(context, "", builder.GetInsertBlock()->getParent());
	builder.SetInsertPoint(retbb);
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
	current_inst = nullptr;
	isPtr = false; 
	for (int i = 0; i < args_expr.size(); i++, current_inst = nullptr, isPtr = false) {
		auto ty = args_expr[i]->codegen();
		if (!ty->getType()->isPointerTy()) {
			types.push_back(ty);
			continue;
		}
		else if (isStringCodegen) {
			isStringCodegen = false;
			types.push_back(ty);
			continue;
		}
		else if (current_inst && current_inst->getAllocatedType()->isArrayTy()) {
			auto array_ty = current_inst->getAllocatedType()->getArrayElementType();
			auto gep = builder.CreateConstGEP2_64(ty, 0, 0);
			while (array_ty->isArrayTy()) {
				gep = builder.CreateConstGEP2_64(gep, 0, 0);
				array_ty = array_ty->getArrayElementType();
			}
			types.push_back(gep);
			continue;
		}
		else if (!isPtr) {
			types.push_back(builder.CreateLoad(ty));
			continue;
		}
		else
			types.push_back(ty);
			
	}
	ArrayRef<Value*> argsRef(types);
	//The system calls
	if (name == "writefln") {
		Codegen::call_writefln(argsRef);
		return nullptr;
	}
	else if (name == "writef") {
		Codegen::call_writef(argsRef);
		return nullptr;
	}
	else if (name == "asm") {
		if (this->asm_args.size() == 2)
			Codegen::gen_asm(this->asm_args[0], this->asm_args[1]);
		else {
			add_err_msg("asm syntax: asm(string, string);");
			error_codegen("Argument does not match!!", this->loc);
		}
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
	auto tmp_retcodegen = retcodegen;
	retcodegen = false;
	for (int i = 0; i < body.size(); i++) {
		body[i]->codegen();
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
		if(!retcodegen)
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

			if(!retcodegen)
				blocks.push_back(elif_block);
			retcodegen = tmp_retcodegen;
			
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
Value* ASTFor::codegen() {
	return nullptr;
}

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
		body[i]->codegen();
	}
	builder.CreateBr(while_block);
	builder.SetInsertPoint(cont_block);
	brk_bbs.pop_back();
	return astboolop;
}
Value* ASTAction::codegen() {
	return nullptr;
}

Value* ASTSubst::codegen() {
	if (this->id) {
		//curvar = namedvalues_local[id->name];
		if (this->expr) {
			auto val = expr->codegen();
			if (val->getType()->isPointerTy())
				val = builder.CreateLoad(val);
			auto ptr = id->codegen();
			Value* ptr_ = ptr;
			//if (!ptr->getAllocatedType()->isPointerTy())
			//	ptr_ = builder.CreatePointerCast(ptr, ptr->getAllocatedType()->getPointerElementType());
			return builder.CreateStore(val, ptr_);
		}
		else {
			lambdavalue = id->codegen();
			for (auto ast = body.begin(); ast != body.end(); ast++) {
				ast->get()->codegen();
			}
			lambdavalue = nullptr;
			return nullptr;
		}
	}
	/*   For array substination control ex. a[0] = 0;
	else if (id2) {
		auto val = this->expr->codegen();
		auto ptr = id2->codegen();
		if (val->getType()->isPointerTy())
			val = builder.CreateLoad(val);
		return builder.CreateStore(val, ptr);
	}
	else
		return nullptr;
		*/
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
Value* ASTArrElements::subst(Value* arr, std::vector<unsigned long long> arr_size_v) {
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
		for (auto i = 0ULL; i < arr_type->ty.arrsize[0]; i++) {
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
		elem_v[i] = Codegen::getTypebyType(this->elements[i].first);
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
		gep = builder.CreateStructGEP(gep, cur[elem_names[i]].idx);
	}
	return builder.CreateLoad(gep);
}
Value* ASTBrk::codegen() {
	builder.CreateBr(brk_bbs[brk_bbs.size()-1]);
	return nullptr;
}
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
Value* ASTGoto::codegen() {
	jmp_bbs[this->label].push_back(builder.GetInsertBlock());
	return nullptr;
}