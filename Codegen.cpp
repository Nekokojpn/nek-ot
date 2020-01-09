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
std::map<Type*, StructType*> list_struct;
Value* underscore;
std::map<std::string, BasicBlock*> jmp_labels;
std::map<std::string, std::vector<BasicBlock*>> jmp_bbs;

bool isSubst = false;

std::pair<bool, std::vector<Value*>> if_rets;
std::vector<BasicBlock*> if_rets_bb;

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

void Codegen::call_exit(Value* exit_code)
{
	llvm::Module* module = builder.GetInsertBlock()->getParent()->getParent();
	llvm::Function* func = module->getFunction("exit");
	if (func == nullptr) {
		std::vector<llvm::Type*> args;
		args.push_back(builder.getInt32Ty());
		bool is_var_args = false;
		func = llvm::Function::Create(
			llvm::FunctionType::get(builder.getVoidTy(), args, is_var_args),
			llvm::Function::ExternalLinkage, "exit", module);
		func->setCallingConv(llvm::CallingConv::C);
	}
	std::vector<Value*> v;
	v.push_back(exit_code);
	llvm::CallInst* inst = builder.CreateCall(func, v);
	inst->setCallingConv(func->getCallingConv());
}

void Codegen::gen_asm(std::string statement, std::string option) {
	llvm::InlineAsm* IA = InlineAsm::get(FunctionType::get(builder.getVoidTy(), false), statement, option, true, false);
	std::vector<Value*> vec;
	ArrayRef<Value*> arg(vec);
	builder.CreateCall(IA, arg);
	return;
}


ConstantInt* Codegen::getValueInt(Value* c) {
	if (Constant* cs = dyn_cast<Constant>(c)) {
		//Const.
		if (ConstantInt* ci = dyn_cast<ConstantInt>(cs)) {
			return ci;
		}
	}
	//TODO: support other type. e.g. FP,.
	return nullptr;
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
	case AType::UserdefinedType:
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
		if (t.isList) {
			if (list_struct.find(ty) != list_struct.end())
				return list_struct[ty];
			else {
				std::vector<Type*> tys;
				auto st = StructType::create(context, "1sys.list");
				tys.push_back(st->getPointerTo());	//Prev
				tys.push_back(ty);					//elem
				tys.push_back(st->getPointerTo());	//Forw
				tys.push_back(builder.getIntNTy(2));
				ArrayRef<Type*> tyss(tys);
				st->setBody(tyss);
				list_struct[ty] = st;
				return st;
			}
		}
		if (t.kind == TypeKind::Pointer)
			ty = ty->getPointerTo();
		return ty;
	}
	else {
		if (t.ty == AType::UserdefinedType) {
			if(userdefined_stcts.find(t.name) != userdefined_stcts.end())
				return userdefined_stcts[t.name];
			else {

			}
		}
		return nullptr;
	}
}

Value* Codegen::getIdentifier(Value* v, AST* ast, Location_t& t) {
	auto ty_load = v->getType()->isPointerTy() ? v->getType()->getPointerElementType() : v->getType();
	ASTIdentifierBase* aib = nullptr;
	ASTIdentifierArrayElementBase* aiae = nullptr;
	ASTCall* ac = nullptr;
	if (ast->getASTType() == TypeAST::IdentifierBase)
		aib = (ASTIdentifierBase*)ast;
	else if (ast->getASTType() == TypeAST::Call)
		ac = (ASTCall*)ast;
	else
		aiae = (ASTIdentifierArrayElementBase*)ast;
	auto name = aib != nullptr ? aib->name : ac != nullptr ? ac->name : aiae->name;
	if (ty_load->isStructTy()) {
		if (userdefined_stcts_elements.find(ty_load->getStructName()) != userdefined_stcts_elements.end()) {
			auto cur = userdefined_stcts_elements[ty_load->getStructName()].elems;
			if (cur.find(name) != cur.end())
				return builder.CreateStructGEP(v, cur[name].idx);
		}
		//List
		else {
			if (ac && name == "add") {
				auto v_load = v->getType()->isPointerTy() ? builder.CreateLoad(v) : v;
				auto types = Codegen::genArgValues(ac);
				if (types.size() != 0)
					error_codegen("Syntax: <list>.add(<element>);", t);
				ArrayRef<Value*> argsRef(types);

				auto aloc = builder.CreateAlloca(ty_load);
				//TODO: typecheck
				builder.CreateStore(types[0], builder.CreateStructGEP(v_load, 1));
				builder.CreateStore(builder.getIntN(2, 2), builder.CreateStructGEP(v_load, 3));
				return nullptr; //Void
			}
		}
	}
	else if(ty_load->isArrayTy()) {
		if (ac && name == "len") {
			if (ty_load->isArrayTy())
				return builder.getInt32(ty_load->getArrayNumElements());
			else
				error_codegen("Is not array type.", t);
		}
	}
	else {

	}

}

std::vector<Value*> Codegen::getIndices(AST* ast, bool isArrTy, Location_t& t) {
	std::vector<Value*> ind;
	auto indices = (ASTArrayIndexes*)ast;
	if (!isArrTy)
		ind.push_back(builder.getInt32(0));
	auto l = indices->lhs->codegen();
	if (l->getType()->isPointerTy())
		l = builder.CreateLoad(l);
	ind.push_back(l);
	return ind;
}

AllocaInst* Codegen::getLocalVal(std::string name, Location_t& t) {
	if (namedvalues_local.find(name) != namedvalues_local.end())
		return namedvalues_local[name];
	error_codegen("\"" + name + "\" is not declared!", t);
	return nullptr;
}
Value* Codegen::getGlobalVal(std::string name, Location_t& t) {
	if (namedvalues_global.find(name) != namedvalues_global.end())
		return namedvalues_global[name];
	if (underscore)
		return underscore;
	return nullptr;
}

Value* Codegen::getDefinedValue(std::string name, Location_t& t) {
	auto val = Codegen::getLocalVal(name, t);
	return val ? val : getGlobalVal(name, t);
}
Value* Codegen::substList(std::string name, Type* stct, AST* ast, Location_t& t) {
	if (ast->getASTType() != TypeAST::ListElements)
		error_codegen("Invalid list elements.", t);
	auto elems = (ASTListElements*)ast;
	Value* prev = nullptr;
	auto val = builder.CreateAlloca(stct);
	namedvalues_local[name] = val;

	auto top = val;
	for (int i = 0; i < elems->elems.size(); i++) {
		if (prev) {
			builder.CreateStore(prev, builder.CreateStructGEP(val, 0));
			builder.CreateStore(builder.getIntN(2, 0), builder.CreateStructGEP(val, 3));
		}
		else
			builder.CreateStore(builder.getIntN(2, 1), builder.CreateStructGEP(val, 3));
		builder.CreateStore(elems->elems[i]->codegen(), builder.CreateStructGEP(val, 1));
		prev = val;
		val = builder.CreateAlloca(stct);
		builder.CreateStore(val, builder.CreateStructGEP(prev, 2));
	}
	builder.CreateStore(builder.getIntN(2, 2), builder.CreateStructGEP(val, 3));
	return top;
}

void Codegen::createWritefln(std::string message) {
	std::vector<Value*> v;
	v.push_back(builder.CreateGlobalStringPtr(message));
	v.push_back(builder.CreateGlobalStringPtr("\n"));
	ArrayRef<Value*> vv(v);
	Codegen::call_writef(vv);
	return;
}

void Codegen::createErrWritefln(std::string message, Location_t& t) {
	std::vector<Value*> v;
	v.push_back(builder.CreateGlobalStringPtr(message + "\n	at " + source_filename + " : " + std::to_string(t.location_begin_line + 1) + " : " + std::to_string(t.location_begin_column)));
	ArrayRef<Value*> vv(v);
	Codegen::call_writef(vv);
	return;
}

BasicBlock* Codegen::createBB() {
	return BasicBlock::Create(context, "", builder.GetInsertBlock()->getParent());
}

//cond -> false control is error handling.
void Codegen::createRuntimeError(std::string errmsg, Value* cond, Location_t& t) {
	auto tr = Codegen::createBB();
	auto fa = Codegen::createBB();
	builder.CreateCondBr(cond, tr, fa);
	builder.SetInsertPoint(fa);
	Codegen::createErrWritefln("Runtime Error: " + errmsg, t);
	Codegen::call_exit(builder.getInt32(1));
	builder.CreateBr(tr);
	builder.SetInsertPoint(tr);
	return;
}

void Codegen::doMatchType(Value* l, Value* r) {
	if (l->getType()->isFloatingPointTy())
		if (!r->getType()->isFloatingPointTy())
			r = builder.CreateSIToFP(r, builder.getDoubleTy());
	if (r->getType()->isFloatingPointTy())
		if (!l->getType()->isFloatingPointTy())
			l = builder.CreateSIToFP(l, builder.getDoubleTy());
	if (l->getType()->isIntegerTy() && r->getType()->isIntegerTy() &&
			l->getType() != r->getType()) {
		if (l->getType() == builder.getIntNTy(64) && r->getType() != builder.getIntNTy(64)) {
			r = builder.CreateSExt(r, builder.getIntNTy(64));
		}
		else if (l->getType() != builder.getIntNTy(64) && r->getType() == builder.getIntNTy(64)) {
			l = builder.CreateSExt(l, builder.getIntNTy(64));
		}
	}
	return;
}

std::vector<Value*> Codegen::genArgValues(ASTCall* ac) {
	std::vector<Value*> types;
	for (int i = 0; i < ac->args_expr.size(); i++, current_inst = nullptr, isPtr = false) {
		auto ty = ac->args_expr[i]->codegen();
		//auto curArg = module->getFunction(this->name)->getArg(i);
		//if (!curArg)
		//	error_codegen("Argument out of range.", this->loc);
		Type* ty_load = nullptr;
		if (ty->getType()->isPointerTy())
			ty_load = ty->getType()->getPointerElementType();
		else
			ty_load = ty->getType();
		//if (curArg->getType() != ty_load->getType())
		//	error_codegen("The argument " + std::to_string(i) + " is not match specified the type.", this->loc);

		if (!ty->getType()->isPointerTy()) {
			types.push_back(ty);
			continue;
		}
		else if (isStringCodegen) {
			isStringCodegen = false;
			types.push_back(ty);
			continue;
		}

		else if (current_inst && current_inst->getType()->isArrayTy() && !ty_load->isArrayTy()) {
			types.push_back(ty);
		}
		else if (ty_load->isArrayTy()) {

			auto array_ty = ty_load->getArrayElementType();
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
	return types;
}