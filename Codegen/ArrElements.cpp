#include "../nek-ot.hpp"

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

Type* ASTArrElements::getType() {
	return ArrayType::get(this->elements[0]->getType(), this->elements.size());
}