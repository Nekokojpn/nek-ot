#include "../nek-ot.hpp"

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

Type* ASTElse::getType() {
	return nullptr;
}