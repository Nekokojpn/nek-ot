#include "../nek-ot.hpp"

Value* ASTType::codegen() {
	ASTSubst* sub = nullptr;
	ASTArrElements* elements = nullptr;
	if (this->expr) {
		if (this->expr->getASTType() == TypeAST::Subst)
			sub = (ASTSubst*)this->expr;
		else
			elements = (ASTArrElements*)this->expr;
	}
	auto type = Codegen::getTypebyType(this->ty);
	Value* inf_v = nullptr;
	//else control -> type inference
	if (this->ty.ty != AType::Nop) {
fr:
		if (!type || !type->isArrayTy()) {
			AllocaInst* allocainst;
			//If local variable
			if (!this->isGlobal) {
				//definition list
				if (type->isStructTy() && this->ty.isList) {
					//Experimental. may cause problems
					namedvalues_local_isinitialized[this->name] = true;
					if(sub)
						return Codegen::substList(this->name, type, sub->expr, this->loc);
					return Codegen::substList(this->name, type, nullptr, this->loc);
				}
				//definition stct
				if (type->isStructTy() && sub) {
					auto vec = Codegen::getStctElements(this->name, sub->expr, sub->expr->loc);
					ArrayRef<Type*> arref(vec);
					auto stctty = StructType::create(context, arref);
					stctty->setName(this->name);
					userdefined_stcts[this->name] = stctty;
					allocainst = builder.CreateAlloca(stctty);
					namedvalues_local[this->name] = allocainst;
					return allocainst;
				}
				if (this->ty.ty != AType::UserdefinedType)
					allocainst = builder.CreateAlloca(type);
				else
					allocainst = builder.CreateAlloca(userdefined_stcts[this->stct_name]);
				namedvalues_local[this->name] = allocainst;
			}
			//If global variable
			else {
				if (this->ty.ty != AType::UserdefinedType)
					allocainst = builder.CreateAlloca(type);
				else
					allocainst = builder.CreateAlloca(userdefined_stcts[this->stct_name]);
				namedvalues_local[this->name] = allocainst;
			}
			if (this->expr && !type->isStructTy()) {
				namedvalues_local_isinitialized[this->name] = true;
				auto value = this->expr->codegen();
				
				if (!value)
					return nullptr;
				return value;
			}
			return allocainst;
		}
		else {
			auto ty = type;
			if (this->ty.arrsize.size() != 0) {
				long long cnt = 1;
				while (cnt < this->ty.arrsize.size()) {
					ty = ArrayType::get(ty, this->ty.arrsize[this->ty.arrsize.size() - cnt]);
					cnt++;
				}
			}
			auto allocainst = builder.CreateAlloca(ty);
			if (name != "_") {
				namedvalues_local[name] = allocainst;
			}
			else
				underscore = allocainst;
			if (sub) {
				((ASTArrElements*)sub->expr)->subst(allocainst);
				namedvalues_local_isinitialized[this->name] = true;
			}
			return allocainst;
		}
	}
	else {
		if (sub && sub->expr) {
			type = sub->expr->getType();
			if (!type) {
				inf_v = sub->expr->codegen();
				type = inf_v->getType();
			}
			goto fr;
		}
		else if (elements) {
			type = elements->getType();
			goto fr;
		}
		else
			goto fr;
	}
}

Type* ASTType::getType() {
	return this->expr->getType();
}

TypeAST ASTType::getASTType() {
	return TypeAST::Type;
}