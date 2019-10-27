#include "../nek-ot.hpp"


Type_t Parser::getTypeFromCurtok() {
	auto ty = getATypeByCurtok();
	auto isArr = false;
	auto kind = TypeKind::Value;
	std::vector<unsigned long long> arrsize_;
	//Typeinference
	if (ty == AType::Nop) {
		ty = getATypeByCurtok_inference();
		if (ty == AType::Nop) {
			add_err_msg("Can not recognized type " + curtok.val + ".");
			error_unexpected(curtok);
		}
		goto proc;
	}
	getNextToken();
	while (curtok.ty == TK::tok_lpb) {
		isArr = true;
		getNextToken();
		if (curtok.ty == TK::tok_num_int) {
			if (std::atoll(curtok.val.c_str()) > 0)
				arrsize_.push_back(std::atoll(curtok.val.c_str()));
			else {
				add_err_msg("Array size must be higher than 0.");
				error_unexpected(curtok);
			}
			getNextToken();
		}
		if (curtok.ty != TK::tok_rpb) {
			error_unexpected(curtok);
		}
		getNextToken();
	}
	if (curtok.ty == TK::tok_star) {
		getNextToken();
		kind = TypeKind::Pointer;
	}
	else if (curtok.ty == TK::tok_amp) {
		getNextToken();
		kind = TypeKind::Reference;
	}
proc:
	Type_t arg;
	arg.isArr = isArr;
	arg.ty = ty;
	arg.kind = kind;
	arg.arrsize = std::move(arrsize_);
	return arg;
}

//get AType from curtok.
AType Parser::getATypeByCurtok() {
	if (curtok.ty == TK::tok_i16) {
		return AType::I16;
	}
	else if (curtok.ty == TK::tok_i32) {
		return AType::I32;
	}
	else if (curtok.ty == TK::tok_i64) {
		return AType::I64;
	}
	else if (curtok.ty == TK::tok_i128) {
		return AType::I128;
	}
	else if (curtok.ty == TK::tok_char) {
		return AType::Char;
	}
	else if (curtok.ty == TK::tok_f32) {
		return AType::F32;
	}
	else if (curtok.ty == TK::tok_f64) {
		return AType::F64;
	}
	else if (curtok.ty == TK::tok_void) {
		return AType::Void;
	}
	else {
		if (find_userdefined_stct(curtok.val))
			return AType::Struct;
		return AType::Nop;
	}
}

AType Parser::getATypeByCurtok_inference() {
	switch (curtok.ty)
	{
	case TK::tok_num_int:
		return AType::I32;
	case TK::tok_num_double:
		return AType::F64;
	case TK::tok_num_long:
		return AType::I64;
	case TK::tok_num_long_long:
		return AType::I64;
	case TK::tok_lp: //{0,1,2}
		return AType::Array;
		break;
	default:
		return AType::Nop;
	}
}