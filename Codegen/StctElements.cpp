#include "../nek-ot.hpp"

ArrayRef<Type*> ASTStctElements::make_aref(){
	return nullptr;
}
Value* ASTStctElements::codegen() {
	return nullptr;
}

Type* ASTStctElements::getType() {
	return nullptr;
}

TypeAST ASTStctElements::getASTType() {
	return TypeAST::StctElements;
}

std::vector<Type*> Codegen::getStctElements(std::string origin_stctname, AST* ast, Location_t& t) {
	if (ast->getASTType() != TypeAST::StctElements)
		error_codegen("SYSTEM BUG", t);
	auto stctelem = (ASTStctElements*)ast;
	std::vector<Type*> elem_v;
	for (int i = 0; i < stctelem->elements.elem_names.size(); i++) {
		auto cur = stctelem->elements.elems[stctelem->elements.elem_names[i]];
		elem_v.push_back(Codegen::getTypebyType(cur.elem_ty));
	}

	userdefined_stcts_elements[origin_stctname] = stctelem->elements;
	//builder.CreateAlloca(StructType::create(context, elem_v));
	//module->dump();
	return elem_v;
}
