#include "../nek-ot.hpp"

Value* ASTProto::codegen() {
	std::vector<Type*> putsArgs;

	for (int i = 0; i < args.size(); i++) {
		Type* ty;
		
		if (args[i].isArr == true && args[i].arrsize.size() > 0) {
			std::vector<Type*> elems;
			auto ary = Codegen::getTypebyType(args[i]);
			if (auto stct = Codegen::getAryStruct(ary->getArrayElementType()->getPointerTo())) {
				ty = stct;
			}
			else {
				elems.push_back(ary->getArrayElementType()->getPointerTo());
				elems.push_back(builder.getInt64Ty());
				ty = StructType::create(context, elems);
				ary_struct[ary->getArrayElementType()->getPointerTo()] = (StructType*)ty;
			}
		}
		else if (args[i].isArr == true && args[i].arrsize.size() == 0) {
			std::vector<Type*> elems;
			auto ary = Codegen::getTypebyType(args[i]);
			if (auto stct = Codegen::getAryStruct(ary)) {
				ty = stct;
			}
			else {
				elems.push_back(ary);
				elems.push_back(builder.getInt64Ty());
				ty = StructType::create(context, elems, "1sys.array");
				ary_struct[ary] = (StructType*)ty;
			}
		}
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
	mainFunc->setGC("shadow-stack");
	
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

Type* ASTProto::getType() {
	return nullptr;
}

TypeAST ASTProto::getASTType() {
	return TypeAST::Proto;
}