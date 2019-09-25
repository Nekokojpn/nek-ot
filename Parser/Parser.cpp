#include "../nek-ot.h"

//Read forward if the next token is expected.
bool Parser::consume(TK tk) noexcept {
	if (tokens[index].ty == tk)
	{
		getNextToken();
		return true;
	}
	return false;
}
//set the next token to curtok.
void Parser::getNextToken() noexcept {
	if(index+1 < tokens.size())
		curtok = tokens[++index];
}
void Parser::add_userdefined_stct(Token_t& cur) {
	stcts[cur.val] = cur;
}
bool Parser::find_userdefined_stct(std::string stct_name) {
	for (auto itr = this->stcts.begin(); itr != stcts.end(); itr++)
		if (itr->first == stct_name)return true;
	return false;
}
//get Arg_t from curtok.
Type_t Parser::getTypeFromCurtok() {
	auto ty = getATypeByCurtok();
	auto isArr = false;
	auto kind = TypeKind::Value;
	std::vector<unsigned long long> arrsize_;
	if (ty == AType::Nop)goto proc;
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
	if (curtok.ty == TK::tok_star)
		kind = TypeKind::Pointer;
	else if (curtok.ty == TK::tok_amp)
		kind = TypeKind::Reference;

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
	if (curtok.ty == TK::tok_i32) {
		return AType::I32;
	}
	else if (curtok.ty == TK::tok_char) {
		return AType::Char;
	}
	else if (curtok.ty == TK::tok_float) {
		return  AType::Float;
	}
	else if (curtok.ty == TK::tok_double) {
		return AType::Double;
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




std::unique_ptr<ASTStrLiteral> Parser::expr_str() {
	if (curtok.ty != TK::tok_dq && curtok.ty != TK::tok_num_int)
		error_unexpected(curtok);
	if(curtok.ty == TK::tok_dq)
		getNextToken();
	std::string str = "";
	while (true) {
		if (curtok.ty != TK::tok_str_string && curtok.ty != TK::tok_num_int)
			error_unexpected(curtok);
		if(curtok.ty == TK::tok_str_string)
			str += curtok.val;
		else if (curtok.ty == TK::tok_num_int) {
			str += curtok.val;
			/*
			getNextToken();
			if (curtok.ty != TK::tok_dot)
				error("There must be no such like:", "num type and string type can not be combined.",curtok);
			getNextToken();
			if(curtok.ty != )
			*/
		}
		getNextToken();
		if (curtok.ty != TK::tok_dq)
			error_unexpected(curtok);
		getNextToken();
		if (curtok.ty == TK::tok_plus) {
			getNextToken();
			continue;
		}
		break;
	}
	auto loc = curtok.loc;
	auto ast = std::make_unique<ASTStrLiteral>(str);
	ast->loc = loc;
	return std::move(ast);
}


std::unique_ptr<ASTAction> Parser::def_action() {
	getNextToken();
	if (curtok.ty != TK::tok_identifier)
		error_unexpected(curtok);
	auto name = curtok.val;
	getNextToken();
	if (curtok.ty != TK::tok_equal)
		error_unexpected(curtok);

	return nullptr;
}


std::unique_ptr<ASTString> Parser::def_string() {
	getNextToken();
	if (curtok.ty != TK::tok_identifier)
		error_unexpected(curtok);
	auto str = curtok.val;
	getNextToken();
	if (curtok.ty == TK::tok_equal) {
		getNextToken();
		auto loc = curtok.loc;
		auto ast = std::make_unique<ASTString>(str, std::move(expr_str()));
		ast->loc = loc;
		if (curtok.ty != TK::tok_semi)
			error_expected(";", curtok);
		return std::move(ast);
	}
	else if (consume(TK::tok_semi)) { // ;
		auto loc = curtok.loc;
		auto ast = std::make_unique<ASTString>(str, nullptr);
		ast->loc = loc;
		return std::move(ast);
	}
	error_unexpected(curtok);
	return nullptr;
}

std::unique_ptr<AST> Parser::expr_identifier() {
	auto id = curtok.val;
	getNextToken();
	if (curtok.ty == TK::tok_semi) {
		return nullptr;
	}
	else if (curtok.ty == TK::tok_equal || 
				curtok.ty == TK::tok_lpb || 
				curtok.ty == TK::tok_darrow ||
				curtok.ty == TK::tok_plpl ||
				curtok.ty == TK::tok_mimi) {
		auto ast = subst_expr(id);
		return std::move(ast);
	}
	else if (curtok.ty == TK::tok_cleq ||
				curtok.ty == TK::tok_colon) {
		auto ast = def_type(id);
		return std::move(ast);
	}
	else if (curtok.ty == TK::tok_lp ||
				curtok.ty == TK::tok_doll) {
		auto ast = func_call(id, curtok.ty == TK::tok_doll ? true : false);
		return std::move(ast);
	}
	return nullptr;
}

std::unique_ptr<ASTFunc> Parser::def_func() {
	getNextToken();
	if (curtok.ty != TK::tok_identifier) {
		add_err_msg(curtok.val + " may be used as a reserved word.");
		error_unexpected(curtok);
	}
	auto str = curtok.val;
	getNextToken();
	if (curtok.ty != TK::tok_lp)
		error_expected("(", curtok);
	getNextToken();
	std::vector<Type_t> putsArgs;
	std::vector<std::string> argsIdentifier;
	while (true) {
		if (curtok.ty == TK::tok_rp)break;

		if (curtok.ty != TK::tok_identifier)
			error_unexpected(curtok);
		argsIdentifier.push_back(curtok.val);
		getNextToken();

		if (curtok.ty != TK::tok_colon) {
			add_err_msg("Hint: arg syntax --> Identifier : Type");
			error_expected(":", curtok);
		}
		getNextToken();

		auto ty = getTypeFromCurtok();

		if (ty.ty == AType::Nop) {
			add_err_msg("Unkown type name.");
			error_unexpected(curtok);
		}

		putsArgs.push_back(ty);

		if (curtok.ty != TK::tok_comma) break;
		getNextToken();
	}
	if (curtok.ty != TK::tok_rp)
		error("Expected", "Expected --> )", curtok);
	getNextToken();

	Type_t ret;

	if (curtok.ty == TK::tok_arrow) {
		getNextToken();
		ret = getTypeFromCurtok();
	}
	else {
		ret.ty = AType::Void;
	}

	auto loc = curtok.loc;
	auto proto = std::make_unique<ASTProto>(str, putsArgs, argsIdentifier,ret);
	proto->loc = loc;
	if (curtok.ty == TK::tok_lb) {
		auto loc = curtok.loc;
		auto ast_func = std::make_unique<ASTFunc>(std::move(proto), expr_block());
		ast_func->loc = loc;
		return std::move(ast_func);
	}
	else if (curtok.ty == TK::tok_semi) { // The function has no body
		getNextToken();
		auto loc = curtok.loc;
		auto ast_func = std::make_unique<ASTFunc>(std::move(proto), expr_block());
		ast_func->loc = loc;
		return std::move(ast_func);
	}
	else {
		error("Unexpected", "Unexpected token --> " + curtok.val, curtok);
	}
	auto loc_ = curtok.loc;
	auto ast = std::make_unique<ASTFunc>(std::move(proto), expr_block());
	ast->loc = loc_;
	return std::move(ast);
}

std::unique_ptr<ASTIf> Parser::bool_statement() {
	//IF----->
	getNextToken();
	if (curtok.ty != TK::tok_lp)
		error("Expected", "Expected --> (", curtok);
	getNextToken();
	auto boolast = bool_expr();
	if (curtok.ty != TK::tok_rp)
		error("Expected", "Expected --> )", curtok);
	getNextToken();
	if (curtok.ty != TK::tok_lb)
		error("Expected", "Expected --> {", curtok);

	auto loc = curtok.loc;
	auto ast = std::make_unique<ASTIf>(std::move(boolast), expr_block());
	ast->loc = loc;
	//<-----IF
	/*		IF‚µ‚©‚È‚¢		*/
	if(curtok.ty != TK::tok_elif&&curtok.ty != TK::tok_else)
		return std::move(ast);
	//ELIF or ELSE----->
	if (curtok.ty == TK::tok_elif) { //ELIF
		ast->ast_elif = std::move(bool_statement());
		getNextToken();
		return std::move(ast);
	}
	else if(curtok.ty == TK::tok_else) { //ELSE
		getNextToken();
		auto loc = curtok.loc;
		ast->ast_else = std::make_unique<ASTElse>(expr_block());
		ast->loc = loc;
		return std::move(ast);
	}
	error("Unexpected", "Unexpected token--> " + curtok.val, curtok);
	return nullptr;
}

std::unique_ptr<AST> Parser::bool_expr() {
	auto lhs = expr();
	while (true) {
		Op op;
		if (consume(TK::tok_lt)) {
			op = Op::LThan;
		}
		else if (consume(TK::tok_lteq)) {
			op = Op::LThanEqual;
		}
		else if (consume(TK::tok_rt)) {
			op = Op::RThan;
		}
		else if (consume(TK::tok_rteq)) {
			op = Op::RThanEqual;
		}
		else if (consume(TK::tok_eqeq)) {
			op = Op::EqualEqual;
		}
		else if(consume(TK::tok_emeq)) {
			op = Op::NotEqual;
		}
		else {
			break;
		}
		auto rhs = expr();
		auto loc = curtok.loc;
		lhs = std::make_unique<ASTBinOp>(std::move(lhs), op, std::move(rhs));
		lhs->loc = loc;
	}
	return std::move(lhs);
}

std::unique_ptr<ASTFor> Parser::for_statement() {
	getNextToken();
	if (curtok.ty != TK::tok_lp)
		error_unexpected(curtok);
	getNextToken();
	//TODO ‚±‚±‚©‚ç
	return nullptr;
}

std::unique_ptr<ASTWhile> Parser::while_statement() {
	getNextToken();
	bool doll = false;
	if (curtok.ty != TK::tok_lp && curtok.ty != TK::tok_doll)
		error("Expected", "Expected --> (", curtok);
	if (curtok.ty == TK::tok_doll) doll = true;
	getNextToken();
	auto boolast = bool_expr();
	if (!doll && curtok.ty != TK::tok_rp)
		error("Expected", "Expected --> )", curtok);
	if(!doll)getNextToken();
	if (curtok.ty != TK::tok_lb)
		error("Expected", "Expected --> {", curtok);

	auto loc = curtok.loc;
	auto ast = std::make_unique<ASTWhile>(std::move(boolast), expr_block());
	ast->loc = loc;
	return std::move(ast);
}


std::unique_ptr<ASTCall> Parser::func_call(const std::string& _id, bool isdoll) {
	getNextToken();
	std::vector<std::unique_ptr<AST>> argsIdentifier;
	while (true) {
		argsIdentifier.push_back(std::move(expr()));
		
		if (curtok.ty != TK::tok_comma) break;
		getNextToken();
	}
	if (!isdoll && curtok.ty != TK::tok_rp)
		error("Expected", "Expected --> )", curtok);
	auto loc = curtok.loc;
	auto ast = std::make_unique<ASTCall>(_id, std::move(argsIdentifier));
	ast->loc = loc;
	getNextToken();
	return std::move(ast);
}
std::unique_ptr<ASTRet> Parser::def_ret() {
	getNextToken();
	auto rty = getTypeFromCurtok();
	std::unique_ptr<ASTRet> ast;
	if (curtok.ty == TK::tok_semi) {//void
		rty.ty = AType::Void;
	}
	else {
		auto loc = curtok.loc;
		ast = std::make_unique<ASTRet>(rty);
		ast->loc = loc;
		ast->expr_p = std::move(expr());
		if (curtok.ty != TK::tok_semi)
			error_expected(";", curtok);
		return std::move(ast);
	}
	auto loc = curtok.loc;
	ast = std::make_unique<ASTRet>(rty);
	ast->loc = loc;
	getNextToken();
	
	return std::move(ast);
}

Parser::Parser(std::vector<Token_t> _tokens) : tokens(_tokens) {
	index = 0;
	curtok = tokens[index];
}
void Parser::parse_codegen() {	
	if (curtok.ty == TK::tok_fn)
	{
		auto ast = def_func();
		ast->codegen();
	}
	if (curtok.ty == TK::tok_eof)
		return;
	parse_codegen();
	return;
}
