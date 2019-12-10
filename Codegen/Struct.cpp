#include "../nek-ot.hpp"

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

Type* ASTStruct::getType() {
	return nullptr;
}
