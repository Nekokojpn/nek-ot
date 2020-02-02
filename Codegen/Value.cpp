#include "../nek-ot.hpp"

Value* ASTValue::codegen() {
	if (this->isNullptr) {
		return ConstantPointerNull::get(builder.getDoubleTy()->getPointerTo());
	}
	if (this->isLongLong == true)
		return builder.getInt64(value);
	if (this->isDouble == true)
		return ConstantFP::get(context, APFloat(value_d));
	return builder.getInt32(value);
}

Type* ASTValue::getType() {
	if (this->isLongLong == true)
		return builder.getInt64Ty();
	if (this->isDouble == true)
		return builder.getDoubleTy();
	return builder.getInt32Ty();
}

TypeAST ASTValue::getASTType() {
	return TypeAST::Value;
}