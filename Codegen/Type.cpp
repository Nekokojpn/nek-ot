#include "../nek-ot.hpp"

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
		//auto v = this->expr->expr->codegen();
		type = this->expr->expr->getType();
		if (!type)
			type = this->expr->expr->codegen()->getType();
		goto fr;
	}
}

Type* ASTType::getType() {
	return this->expr->getType();
}