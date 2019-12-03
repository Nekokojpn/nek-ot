#pragma once
#include "nek-ot.hpp"

LLVMContext context;
IRBuilder<> builder(context);
std::unique_ptr<Module> module;
std::unique_ptr<legacy::FunctionPassManager> fpm;
std::unique_ptr<PassManagerBuilder> pmbuilder;
std::unique_ptr<PassBuilder> pbuilder;

static std::map<std::string, FunctionCallee> functions_global;
static std::map<std::string, StructType*> userdefined_stcts;
static std::map<std::string, Stct_t> userdefined_stcts_elements;
static std::map<std::string, AllocaInst*> namedvalues_global;
static std::map<std::string, AllocaInst*> namedvalues_local;
static std::map<std::string, bool> namedvalues_local_isinitialized;
static std::map<std::string, Value*> namedvalues_str;
static Value* underscore;
static std::map<std::string, BasicBlock*> jmp_labels;
static std::map<std::string, std::vector<BasicBlock*>> jmp_bbs;

bool isSubst = false;

static std::pair<bool, std::vector<Value*>> if_rets;
static std::vector<BasicBlock*> if_rets_bb;

//ASTArrayIndexes------>
std::vector<Value*> idx_list;
auto isArrTy = false;
//<------

//AST Identifier
AllocaInst* current_inst;
bool isPtr = false;


Value* lambdavalue;

AllocaInst* retvalue;
std::vector<BasicBlock*> retbbs;

std::vector<BasicBlock*> brk_bbs;

bool retcodegen = false;
bool gotocodegen = false;
auto isStringCodegen = false;


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
	/*
	llvm::Function* func; 
	{
		std::vector<llvm::Type*> args;
		llvm::FunctionType* func_type = llvm::FunctionType::get(builder.getVoidTy(), args, false);
		func = llvm::Function::Create(
			func_type, llvm::Function::ExternalLinkage, "exit", module.get());
		func->setCallingConv(llvm::CallingConv::X86_StdCall);
	}
	functions_global["exit"] = func;

	llvm::Function* func;
	{
		std::vector<llvm::Type*> args;
		llvm::FunctionType* func_type = llvm::FunctionType::get(builder.getVoidTy(), args, false);
		func = llvm::Function::Create(
			func_type, llvm::Function::ExternalLinkage, "error", module.get());
		//func->setCallingConv(llvm::CallingConv::X86_StdCall);
		auto bb = BasicBlock::Create(context, "", func);
		builder.SetInsertPoint(bb);
		builder.CreateCall(functions_global["exit"], )
	}
	functions_global["error"] = func;
	*/
	//TODO:: IMPL error
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
	else if (auto* ce = dyn_cast<ConstantExpr>(cs)) {
		//TOOD: ConstExpr to Const value
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
		if (t.ty == AType::Struct) {
			return nullptr;
		}
		return nullptr;
	}
}

Value* ASTIdentifierBase::codegen() {
	//For a stct control E.g, identifier.item = 10;
	if (current_inst && current_inst->getAllocatedType()->isStructTy()) {
		auto stct = userdefined_stcts_elements[current_inst->getAllocatedType()->getStructName()];
		//TODO: Type check
		if (stct.elems.find(this->name) != stct.elems.end()) {
			auto gep = builder.CreateStructGEP(current_inst, stct.elems[this->name].idx);
			return gep;
		}
		else {
			error_codegen(static_cast<std::string>(current_inst->getAllocatedType()->getStructName()) + " is not stct or undefined var.", this->loc);
			return nullptr;
		}

	}
	//For a var control E.g, identifier = 10;
	else {
		auto value = namedvalues_local[this->name];
		if (!value) {
			auto global = namedvalues_global[this->name];
			if (!global) {
				auto str = namedvalues_str[this->name];
				if (!str) {
					if (underscore)
						return underscore;
					error("Unsolved value name", "Unsolved value name --> " + this->name, this->loc);
				}
				return str;
			}
			current_inst = global;
			return global;
		}
		if (isSubst || namedvalues_local_isinitialized[this->name] == true) {
			current_inst = value;
			if (isSubst)namedvalues_local_isinitialized[this->name] = true;
			return value;
		}
		else {
			add_err_msg("Variables must be initialized before use.");
			error_codegen("Variable '" + this->name + "' is not initialized!", this->loc);
		}
	}
}

Value* ASTIdentifierArrayElementBase::codegen() {
	auto value = namedvalues_local[this->name];
	if (!value) {
		auto global = namedvalues_global[this->name];
		if (!global) {
			auto str = namedvalues_str[this->name];
			if (!str) {
				if (underscore)
					return underscore;
				error("Unsolved value name", "Unsolved value name --> " + this->name, this->loc);
			}
			return str;
		}
		current_inst = global;
		return global;
	}
	if (isSubst || namedvalues_local_isinitialized[this->name] == true) {
		current_inst = value;
		if (isSubst)namedvalues_local_isinitialized[this->name] = true;
	}
	else {
		add_err_msg("Variables must be initialized before use.");
		error_codegen("Variable '" + this->name + "' is not initialized!", this->loc);
	}
	//Build a idx_list

	Value* gep = nullptr;
	if (value->getAllocatedType()->isPointerTy())isArrTy = true;
	else isArrTy = false;
	this->indexes->codegen();
	//TODO multi dimen;
	ArrayRef<Value*> v(idx_list);
	if (isArrTy) {
		gep = builder.CreateLoad(value);
		return builder.CreateInBoundsGEP(gep, v);

	}
	int i = idx_list.size() - 1;
	auto ci = Codegen::getValueInt(idx_list[i]);
	if (std::get<0>(ci)) {
		if (std::get<1>(ci) >= value->getAllocatedType()->getArrayNumElements())
			error_codegen("The specified index is out of range.", this->loc);
	}
	else {
		//Ç±Ç±
		/*
		BasicBlock* bb_t = BasicBlock::Create(context, "", builder.GetInsertBlock()->getParent());
		BasicBlock* bb_f = BasicBlock::Create(context, "", builder.GetInsertBlock()->getParent());
		std::vector<Value*> v;
		v.push_back(builder.CreateGlobalStringPtr("Error: The specified index is out of range."));
		ArrayRef<Value*> vv(v);
		Codegen::call_writefln(v);
		Codegen::call_error(1);
		builder.CreateCondBr(builder.CreateICmpSGE(idx_list[0], value), bb_t, bb_f);
		*/
	}
	
	gep = builder.CreateInBoundsGEP(value, v);
	return gep;
}

Value* ASTArrayIndexes::codegen() {
	//Disired identifier already set to current_inst
	std::vector<Value*> ind;
	if(!isArrTy)
		ind.push_back(builder.getInt32(0));
	auto l = this->lhs->codegen();
	if (l->getType()->isPointerTy())
		l = builder.CreateLoad(l);
	ind.push_back(l);
	idx_list = ind;
	return nullptr;
}

Value* ASTIdentifier::codegen() {
	auto value = this->lhs->codegen();
	if (!this->rhs) { //Is not a stct.
		if (this->kind == TypeKind::Pointer) {
			current_inst = nullptr;
			return value;
		}
		else if (this->kind == TypeKind::Reference) {
			current_inst = nullptr;
			return value;
		}
		else {
			current_inst = nullptr;
			return builder.CreateLoad(value);
		}
	}
	else { //Is a stct.
		auto value2 = this->rhs->codegen();
		if (this->kind == TypeKind::Pointer) {
			current_inst = nullptr;
			return value2;
		}
		else if (this->kind == TypeKind::Reference) {
			current_inst = nullptr;
			return value2;
		}
		else {
			current_inst = nullptr;
			return builder.CreateLoad(value2);
		}
		return value2;
	}
	return nullptr;
}

Value* ASTValue::codegen() {

	if (this->isLongLong == true)
		return builder.getInt64(value);
	if(this->isDouble == true)
		return ConstantFP::get(context, APFloat(value_d));
	return builder.getInt32(value);
}

Value* ASTBool::codegen() {
	return builder.getInt1(this->value);
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
	if (l->getType()->isIntegerTy() && r->getType()->isIntegerTy() &&
			l->getType() != r->getType()) {
		if (l->getType() == builder.getIntNTy(64) && r->getType() != builder.getIntNTy(64)) {
			r = builder.CreateSExt(r, builder.getIntNTy(64));
		}
		else if (l->getType() != builder.getIntNTy(64) && r->getType() == builder.getIntNTy(64)) {
			l = builder.CreateSExt(l, builder.getIntNTy(64));
		}
	}
	switch (op) {
	case Op::Plus:
		if (!l->getType()->isFloatingPointTy() &&
			!r->getType()->isFloatingPointTy()
			)
			return builder.CreateAdd(l, r);
		else
			return builder.CreateFAdd(l, r);
	case Op::Minus:
		if (!l->getType()->isFloatingPointTy() &&
			!r->getType()->isFloatingPointTy()
			)
			return builder.CreateSub(l, r);
		else
			return builder.CreateFSub(l, r);
	case Op::Mul:
		if (!l->getType()->isFloatingPointTy() &&
			!r->getType()->isFloatingPointTy()
			)
			return builder.CreateMul(l, r);
		else
			return builder.CreateFMul(l, r);
	case Op::Div:
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
	auto type = Codegen::getTypebyType(this->ty);
	if (this->ty.ty != AType::Nop) {
fr:
		if (!type || !type->isArrayTy()) {
			AllocaInst* allocainst;
			//If local variable
			if (!this->isGlobal) {
				if (this->ty.ty != AType::Struct)
					allocainst = builder.CreateAlloca(type);
				else
					allocainst = builder.CreateAlloca(userdefined_stcts[this->stct_name]);
				namedvalues_local[this->name] = allocainst;
			}
			//If global variable
			else {
				if (this->ty.ty != AType::Struct)
					allocainst = builder.CreateAlloca(type);
				else
					allocainst = builder.CreateAlloca(userdefined_stcts[this->stct_name]);
				namedvalues_local[this->name] = allocainst;
			}
			if (this->expr) {
				namedvalues_local_isinitialized[this->name] = true;
				auto value = this->expr->codegen();
				if (auto* C = dyn_cast<Constant>(value))
					if (auto CI = dyn_cast<ConstantInt>(C))
						std::cout << CI->getValue().getZExtValue();
				if (!value)
					return nullptr;
				return value;
			}
			return allocainst;
		}
		else {
			//ArrayType* ty = ArrayType::get(type, this->ty.arrsize[this->ty.arrsize.size() - 1]);
			auto ty = type;
			long long cnt = 1;
			while (cnt < this->ty.arrsize.size()) {
				ty = ArrayType::get(ty, this->ty.arrsize[this->ty.arrsize.size() - cnt]);
				cnt++;
			}
			auto allocainst = builder.CreateAlloca(ty);

			if (name != "_") {
				namedvalues_local[name] = allocainst;
			}
			else
				underscore = allocainst;

			if (this->elements) {
				this->elements->subst(allocainst, this->ty.arrsize);
				namedvalues_local_isinitialized[this->name] = true;
			}
			return allocainst;
		}
	}
	else {
		auto v = this->expr->expr->codegen();
		type = v->getType();
		goto fr;
	}
}

Value* ASTProto::codegen() {
	std::vector<Type*> putsArgs;

	for (int i = 0; i < args.size(); i++) {
		Type* ty;
		if (args[i].isArr == true && args[i].arrsize.size() > 0)
			ty = Codegen::getTypebyType(args[i])->getArrayElementType()->getPointerTo();
		else if (args[i].isArr == true && args[i].arrsize.size() == 0)
			ty = Codegen::getTypebyType(args[i]);
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
		namedvalues_local_isinitialized[arg.getName()] = true;
	}
	mainFunc->setDSOLocal(true);
	//mainFunc->setCallingConv(CallingConv::X86_StdCall);
	functions_global[name] = mainFunc;

	return mainFunc;
}
Value* ASTFunc::codegen() {
	//TODO nested func  change->BasicBloc
	auto pr = proto->codegen(); //Proto
	if(builder.GetInsertBlock()->getParent()->getReturnType() != builder.getVoidTy())
		retvalue = builder.CreateAlloca(builder.GetInsertBlock()->getParent()->getReturnType());

	for (int i = 0; i < body.size(); i++) {
		Codegen::init_on_inst();

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
	namedvalues_local.clear();
	namedvalues_local_isinitialized.clear();
	return pr;
}
Value* ASTCall::codegen() {
	std::vector<Value*> types;
	current_inst = nullptr;
	isPtr = false; 
	for (int i = 0; i < args_expr.size(); i++, current_inst = nullptr, isPtr = false) {
		auto ty = args_expr[i]->codegen();
		Value* ty_load;
		if (ty->getType()->isPointerTy())
			ty_load = builder.CreateLoad(ty);
		else
			ty_load = ty;
		if (!ty->getType()->isPointerTy()) {
			types.push_back(ty);
			continue;
		}
		else if (isStringCodegen) {
			isStringCodegen = false;
			types.push_back(ty);
			continue;
		}
		
		else if (current_inst && current_inst->getAllocatedType()->isArrayTy() && !ty_load->getType()->isArrayTy()) {
			if (ty->getType()->isPointerTy())
				ty = builder.CreateLoad(ty);
			types.push_back(ty);
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
		Codegen::call_writef(argsRef);
		types.clear();
		types.push_back(builder.CreateGlobalStringPtr("\n"));
		ArrayRef<Value*> argsReff(types);
		Codegen::call_writef(argsReff);
		return nullptr;
	}
	else {
		if (functions_global.find(this->name) != functions_global.end()) {
			auto inst = builder.CreateCall(functions_global[name], argsRef, "");
			//inst->setCallingConv(CallingConv::X86_StdCall);
			return inst;
		}
		else {
			std::string s = "There is no function name --> " + this->name;
			error_codegen(s, this->loc);
		}
	}
}

Value* ASTElse::codegen() {
	Value* finalret;
	for (int i = 0; i < body.size(); i++) {
		Codegen::init_on_inst();
		finalret = body[i]->codegen();
	}
	if (if_rets.first) {
		if_rets.second.push_back(finalret);
	}
	return nullptr;
}

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
Value* ASTFor::codegen() {
	typedeff->codegen();
	auto bb = BasicBlock::Create(context, "", builder.GetInsertBlock()->getParent());
	builder.CreateBr(bb);

	builder.SetInsertPoint(bb);
	auto bb2 = BasicBlock::Create(context, "", builder.GetInsertBlock()->getParent());
	

	//builder.CreateBr(bb2);
	builder.SetInsertPoint(bb2);
	for (int i = 0; i < this->proto->body.size(); i++) {
		this->proto->body[i]->codegen();
	}
	last->codegen();
	builder.CreateBr(bb);

	auto cond = BasicBlock::Create(context, "", builder.GetInsertBlock()->getParent());

	builder.SetInsertPoint(bb);
	builder.CreateCondBr(this->proto->proto->codegen(), bb2, cond);
	builder.SetInsertPoint(cond);
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
		Codegen::init_on_inst();
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
		if_rets.first = true;
		if (this->expr) {
			auto val = expr->codegen();
			if_rets.first = false;
			if (!isStringCodegen && if_rets.second.size() == 0 && val->getType()->isPointerTy())
				val = builder.CreateLoad(val);
			isSubst = true;
			auto ptr = id->codegen();
			isSubst = false;
			Value* ptr_ = ptr;
			if (if_rets.second.size() == 0 && !isStringCodegen) {
				if (ptr_->getType()->getPointerElementType() == val->getType())
					return builder.CreateStore(val, ptr_);
				else
					error_codegen("Does not match the type of the declared variable", this->loc);
			}
			else if (if_rets.second.size() > 0) {
				if (if_rets.second.size() < 2) {
					add_err_msg("Syntax: <variable> = if(<bool expression>) <expression>; else <expression>;");
					error_codegen("Expected --> else", this->loc);
				}
				if (if_rets_bb.size() != if_rets.second.size()) {
					error_codegen("Invalid if block!", this->loc);
				}
				auto phi = builder.CreatePHI(if_rets.second[0]->getType(), if_rets.second.size());
				for (int i = 0; i < if_rets.second.size(); i++) {
					if (phi->getType() == if_rets.second[i]->getType())
						phi->addIncoming(if_rets.second[i], if_rets_bb[i]);
					else
						error_codegen("if expressions must return the same type", this->loc);
				}
				return builder.CreateStore(phi, ptr_);
				if_rets.second.clear();
				if_rets_bb.clear();
			}
			else { // May be string
				return builder.CreateStore(val, ptr_);
			}
		}
	}
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
	std::vector<Type*> elem_v(this->elements->elements.elem_names.size());
	Stct_t userdefd;
	for (int i = 0; i < this->elements->elements.elem_names.size(); i++) {
		auto cur = this->elements->elements.elems[this->elements->elements.elem_names[i]];
		elem_v[i] = Codegen::getTypebyType(cur.elem_ty);
		
	}
	userdefd = this->elements->elements;
	ArrayRef<Type*> elements(elem_v);
	
	auto stct = StructType::create(context,this->name);
	stct->setBody(elements);
	userdefined_stcts[this->name] = stct;
	userdefined_stcts_elements[this->elements->elements.stct_name] = userdefd;
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
			//gep = builder.CreateInBoundsGEP(builder.CreateAlloca(ArrayType::get(builder.getInt32Ty(),5)), pp);
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
	return nullptr;
}
Value* ASTStctElements::codegen() {
	return nullptr;
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
	gotocodegen = true;
	jmp_bbs[this->label].push_back(builder.GetInsertBlock());
	return nullptr;
}