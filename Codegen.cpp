#pragma once
#include "nek-ot.hpp"

LLVMContext context;
IRBuilder<> builder(context);
std::unique_ptr<Module> module;
std::unique_ptr<legacy::FunctionPassManager> fpm;
std::unique_ptr<PassManagerBuilder> pmbuilder;
std::unique_ptr<PassBuilder> pbuilder;

std::map<std::string, FunctionCallee> functions_global;
std::map<std::string, StructType*> userdefined_stcts;
std::map<std::string, Stct_t> userdefined_stcts_elements;
std::map<std::string, AllocaInst*> namedvalues_global;
std::map<std::string, AllocaInst*> namedvalues_local;
std::map<std::string, bool> namedvalues_local_isinitialized;
std::map<std::string, Value*> namedvalues_str;
Value* underscore;
std::map<std::string, BasicBlock*> jmp_labels;
std::map<std::string, std::vector<BasicBlock*>> jmp_bbs;

bool isSubst = false;

std::pair<bool, std::vector<Value*>> if_rets;
std::vector<BasicBlock*> if_rets_bb;

//ASTArrayIndexes------>
std::vector<Value*> idx_list;
bool isArrTy = false;
//<------

//AST Identifier
Value* current_inst;
bool isPtr = false;


AllocaInst* retvalue;
std::vector<BasicBlock*> retbbs;

std::vector<BasicBlock*> brk_bbs;

bool retcodegen = false;
bool gotocodegen = false;
bool isStringCodegen = false;


Module* getModule() {
	return module.get();
}

void Test::CreateFunc() {
	
}


void Sys::IO::Printf::CreateFunc() {
	Function* func;
	{
		std::vector<Type*> args;
		args.push_back(builder.getInt8PtrTy());
		bool is_var_args = true;
		func = Function::Create(
			llvm::FunctionType::get(builder.getInt32Ty(), args, is_var_args),
			llvm::Function::ExternalLinkage, "printf", module.get());
	}
	functions_global["writef"] = func;
}


void Sys::IO::Input::CreateFunc() {

	llvm::Function* input_func;
	{
		std::vector<llvm::Type*> args;
		//args.push_back(builder.getInt32Ty());
		llvm::FunctionType* func_type = llvm::FunctionType::get(builder.getInt32Ty(), args, false);
		input_func = llvm::Function::Create(
			func_type, llvm::Function::ExternalLinkage, "?input@@YAHXZ", module.get());
		input_func->setCallingConv(llvm::CallingConv::X86_StdCall);
	}
	functions_global["input"] = input_func;
	return;
}
void Sys::List::CreateFunc() {
	StructType* st;
	{
		std::vector<Type*> args;
		args.push_back(builder.getInt64Ty()); //elem
		args.push_back(builder.getInt64Ty()->getPointerTo()); //back
		args.push_back(builder.getInt64Ty()->getPointerTo()); //next
		ArrayRef<Type*> argsA(args);
		st = StructType::create(context, "List");
		st->setBody(argsA);
	}
}

void Sys::Range::CreateFunc() {
	/*
	Function* range_func;
	{
		std::vector<Type*> args;
		args.push_back(builder.getInt64Ty()); //start
		args.push_back(builder.getInt64Ty()); //end -1
		args.push_back(builder.getInt64Ty()); //diff
		FunctionType* func_type = FunctionType::get(ArrayType::get(builder.getInt64Ty();
		
	}
	*/
}

void Sys::Random::CreateFunc() {
	Codegen::declareFunction("random", "?random@@YAHXZ");
}

void Sys::IO::File::CreateFunc() {
	llvm::Function* func;
	{
		std::vector<llvm::Type*> args;
		args.push_back(builder.getInt8PtrTy());
		args.push_back(builder.getInt8PtrTy());
		llvm::FunctionType* func_type = llvm::FunctionType::get(builder.getInt32Ty(), args, false);
		func = llvm::Function::Create(
			func_type, llvm::Function::ExternalLinkage, "?fwrite@@YAHPEBD0@Z", module.get());
		func->setCallingConv(llvm::CallingConv::X86_StdCall);
	}
	functions_global["fwrite"] = func;
	return;
}

void Codegen::declareFunction(std::string func_name, std::string ac_func_name) {
	llvm::Function* func;
	{
		std::vector<llvm::Type*> args;
		//args.push_back(builder.getInt32Ty());
		llvm::FunctionType* func_type = llvm::FunctionType::get(builder.getInt32Ty(), args, false);
		func = llvm::Function::Create(
			func_type, llvm::Function::ExternalLinkage, ac_func_name, module.get());
		func->setCallingConv(llvm::CallingConv::X86_StdCall);
	}
	functions_global[func_name] = func;
	return;
}

void declareFunction(ArrayRef<Type*> args, Type* ret, std::string fn_name, bool isVarARgs) {
	Function* func;
	{
		auto func_type = FunctionType::get(ret, isVarARgs);
		func = Function::Create(
			func_type, Function::ExternalLinkage, fn_name, module.get());
		func->setCallingConv(CallingConv::X86_StdCall);
	}
	functions_global[fn_name] = func;
	return;
}

void Sys::Exit::CreateFunc() {
	return;
}


void init_parse() {
	module = std::make_unique<Module>("top", context);
	fpm = std::make_unique<legacy::FunctionPassManager>(module.get());
	pmbuilder = std::make_unique<PassManagerBuilder>();
	pmbuilder->OptLevel = 3;
	pmbuilder->populateFunctionPassManager(*fpm);
	fpm->doInitialization();
}
AllocaInst* createEntryBlockAlloca(Function* function, const std::string& name) {
	IRBuilder<> tmpB(&function->getEntryBlock(), function->getEntryBlock().begin());
	return tmpB.CreateAlloca(Type::getInt32Ty(context), nullptr, name.c_str());
}

void Codegen::call_writef(llvm::ArrayRef<llvm::Value*> args)
{
	llvm::Module* module = builder.GetInsertBlock()->getParent()->getParent();
	llvm::Function* func = module->getFunction("printf");
	if (func == nullptr) {
		std::vector<llvm::Type*> args;
		args.push_back(builder.getInt8PtrTy());
		bool is_var_args = true;
		func = llvm::Function::Create(
			llvm::FunctionType::get(builder.getInt32Ty(), args, is_var_args),
			llvm::Function::ExternalLinkage, "printf", module);
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


std::tuple<bool, int> Codegen::getValueInt(Value* c) {
	if (auto* cs = dyn_cast<Constant>(c)) {
		//Const.
		if (auto ci = dyn_cast<ConstantInt>(cs)) {
			return std::make_tuple<bool, int>(true, ci->getValue().getZExtValue());
		}
	}
	//TODO: support other type. e.g. FP,.
	return std::make_tuple<bool, int>(false, 0);
}

void Codegen::call_exit(int exitcode) {
	std::vector<Value*> v;
	ArrayRef<Value*> vv(v);
	builder.CreateCall(functions_global["error"], vv);
}

void Codegen::init_on_inst() {
	current_inst = nullptr;
	return;
}


void Parser::dump() {
	module->dump();
	return;
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
	case AType::I128:
		return builder.getInt128Ty();
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
	case AType::Struct:
		return StructType::get(context);
		break;
	case AType::UserdefinedStruct:
		return nullptr;
		break;
	case AType::Bool:
		return builder.getInt1Ty();
		break;
	default:
		return nullptr;
		break;
	}
}
Type* Codegen::getTypebyType(Type_t& t) {
	auto ty = getTypebyAType(t.ty);

	if (ty != nullptr) {
		if (t.isArr && t.arrsize.size() > 0)
			ty = ArrayType::get(ty, t.arrsize[0]);//TODO support multi dimention
		if (t.isArr && t.arrsize.size() == 0) {
			ty = ty->getPointerTo();
		}
		if (t.kind == TypeKind::Pointer)
			ty = ty->getPointerTo();
		return ty;
	}
	else {
		if (t.ty == AType::UserdefinedStruct) {
			if(userdefined_stcts.find(t.name) != userdefined_stcts.end())
				return userdefined_stcts[t.name];
		}
		return nullptr;
	}
}