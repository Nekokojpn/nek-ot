#include "../nek-ot.hpp"


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
Type_t Parser::getTypeFromCurtok() {
	auto ty = getATypeByCurtok();
	if (ty == AType::Nop && isUserdefinedType(curtok.val))
		ty = AType::UserdefinedType;
	std::string name = curtok.val;
	auto isArr = false;
	auto isList = false;
	curtokIs(TK::tok_lb) ? isArr = true : isArr = false;
	auto kind = TypeKind::Value;
	std::vector<unsigned long long> arrsize_;
	if (ty == AType::Nop)goto proc;
	getNextToken();
	if (curtokIs(TK::tok_lpb)) {
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
	}
	else if (curtokIs(TK::tok_list)) {
		isList = true;
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
	arg.name = name;
	arg.isArr = isArr;
	arg.isList = isList;
	arg.ty = ty;
	arg.kind = kind;
	arg.arrsize = arrsize_;
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
	else if (curtok.ty == TK::tok_string) {
		return AType::String;
	}
	else if (curtok.ty == TK::tok_void) {
		return AType::Void;
	}
	else if (curtok.ty == TK::tok_bool) {
		return AType::Bool;
	}
	else if (curtok.ty == TK::tok_stct) {
		return AType::Struct;
	}
	else if (curtok.ty == TK::tok_list) {
		return AType::List;
	}
	else {
		return AType::Nop;
	}
}




ASTStrLiteral* Parser::expr_str() {
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
	auto ast = new ASTStrLiteral(str);
	ast->loc = loc;
	return ast;
}


ASTAction* Parser::def_action() {
	getNextToken();
	if (curtok.ty != TK::tok_identifier)
		error_unexpected(curtok);
	auto name = curtok.val;
	getNextToken();
	if (curtok.ty != TK::tok_equal)
		error_unexpected(curtok);

	return nullptr;
}


ASTString* Parser::def_string() {
	getNextToken();
	if (curtok.ty != TK::tok_identifier)
		error_unexpected(curtok);
	auto str = curtok.val;
	getNextToken();
	if (curtok.ty == TK::tok_equal) {
		getNextToken();
		auto loc = curtok.loc;
		auto ast = new ASTString(str, expr_str());
		ast->loc = loc;
		if (curtok.ty != TK::tok_semi)
			error_expected(";", curtok);
		return ast;
	}
	else if (consume(TK::tok_semi)) { // ;
		auto loc = curtok.loc;
		auto ast = new ASTString(str, nullptr);
		ast->loc = loc;
		return ast;
	}
	error_unexpected(curtok);
	return nullptr;
}

AST* Parser::expr_identifier() {
	this->curval = curtok.val;
	auto identifier = expr_identifiers();
	if (curtok.ty == TK::tok_semi) {
		getNextToken();
		return identifier;
	}
	else if (curtok.ty == TK::tok_equal || 
				curtok.ty == TK::tok_lpb || 
				curtok.ty == TK::tok_darrow) {
		auto ast = subst_expr(identifier);
		return ast;
	}
	else if (curtok.ty == TK::tok_cleq ||
				curtok.ty == TK::tok_colon) {
		auto ast = def_type(identifier);
		return ast;
	}
	else if (curtok.ty == TK::tok_pleq) {
		getNextToken();
		auto loc = curtok.loc;
		auto ast_bin = new ASTBinOp(identifier, Op::Plus, expr());
		ast_bin->loc = loc;
		auto ast = new ASTSubst(identifier, ast_bin);
		ast->loc = loc;
		return ast;
	}
	else if (curtok.ty == TK::tok_mieq) {
		getNextToken();
		auto loc = curtok.loc;
		auto ast_bin = new ASTBinOp(identifier, Op::Minus, expr());
		ast_bin->loc = loc;
		auto ast = new ASTSubst(identifier, ast_bin);
		ast->loc = loc;
		return ast;
	}
	else {
		if (identifier->getASTType() == TypeAST::Call) {
			if (curtok.ty == TK::tok_where) {
				getNextToken();
				std::vector<AST*> body;
				body.push_back(identifier);
				auto bast = expr();
				auto l = new ASTIf(bast, body);
				l->loc = curtok.loc;
				if (curtok.ty != TK::tok_semi)
					error_expected(";", curtok);
				getNextToken();
				return l;
			}
		}
		return identifier;
	}
	/*
	else { //Expr
		curtok = tokens[----index];
		return expr();
	}
	*/
}

ASTFunc* Parser::def_func() {
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
		ret.name = curtok.val;
		ret.isArr = false;
		ret.isList = false;
		ret.kind = TypeKind::Value;
		ret.ty = AType::Void;
	}

	auto loc = curtok.loc;
	auto proto = new ASTProto(str, putsArgs, argsIdentifier,ret);
	proto->loc = loc;
	if (curtok.ty == TK::tok_lb) {
		auto loc = curtok.loc;
		auto ast_func = new ASTFunc(proto, expr_block(false));
		ast_func->loc = loc;
		return ast_func;
	}
	else if (curtok.ty == TK::tok_semi) { // The function has no body
		getNextToken();
		auto loc = curtok.loc;
		auto ast_func = new ASTFunc(proto, expr_block(false));
		ast_func->loc = loc;
		return ast_func;
	}
	else {
		error("Unexpected", "Unexpected token --> " + curtok.val, curtok);
	}
	auto loc_ = curtok.loc;
	auto ast = new ASTFunc(proto, expr_block(false));
	ast->loc = loc_;
	return ast;
}

ASTIf* Parser::bool_statement() {
	getNextToken();
	if (curtok.ty != TK::tok_lp)
		error_expected("(", curtok);
	auto loc = curtok.loc;
	getNextToken();
	auto boolast = expr();
	if (curtok.ty != TK::tok_rp)
		error_expected(")", curtok);
	getNextToken();
	ASTIf* ast;
	if (curtok.ty == TK::tok_lb) {
		ast = new ASTIf(boolast, expr_block(false));
		ast->loc = loc;
	}
	else {
		ast = new ASTIf(boolast, expr_block(true));
		ast->loc = loc;
	}
	std::vector<ASTIf*> elsif;
	ast->ast_elseif = elsif;
	if(curtok.ty != TK::tok_else)
		return ast;
	else if(curtok.ty == TK::tok_else) { //ELSE
		
		if (nexttokIs(TK::tok_if)) {
			getNextToken();
			while (curtok.ty == TK::tok_if) {
				getNextToken();
				if (!curtokIs(TK::tok_lp))
					error_expected("(", curtok);
				getNextToken();
				auto bast = expr();
				if (curtok.ty != TK::tok_rp)
					error_expected(")", curtok);
				getNextToken();
				ASTIf* ast_;
				auto loc = curtok.loc;
				if (curtok.ty == TK::tok_lb) {
					ast_ = new ASTIf(bast, expr_block(false));
					ast_->loc = loc;
				}
				else {
					ast_ = new ASTIf(bast, expr_block(true));
					ast_->loc = loc;
				}
				elsif.push_back(ast_);
				ast->ast_elseif = elsif;

				if (!curtokIs(TK::tok_else))break;
				else {
					if (nexttokIs(TK::tok_if)) {
						getNextToken();
					}
					else break;
				}
			}
		}
		if (curtokIs(TK::tok_else)) {
			getNextToken();
			if (curtok.ty == TK::tok_lb) {
				auto loc = curtok.loc;
				ast->ast_else = new ASTElse(expr_block(false));
				ast->loc = loc;
				return ast;
			}
			else {
				auto loc = curtok.loc;
				ast->ast_else = new ASTElse(expr_block(true));
				ast->loc = loc;
				return ast;
			}
		}
		return ast;
		
	}
	error("Unexpected", "Unexpected token--> " + curtok.val, curtok);
	return nullptr;
}


ASTFor* Parser::for_statement() {
	getNextToken();
	if (curtok.ty != TK::tok_lp)
		error_unexpected(curtok);
	getNextToken();
	//for(i in 0..9)
	if (nexttokIs(TK::tok_in)) {
		auto _name = curtok.val;
		auto ident = expr_identifiers();
		if (!curtokIs(TK::tok_in))
			error_expected("in", curtok);
		getNextToken();
		auto start = expr();
		if (!curtokIs(TK::tok_dtdt))
			error_expected("..", curtok);
		getNextToken();
		auto end = expr();
		if (curtokIs(TK::tok_rp)) {
			getNextToken();
			auto ty = getTypeFromCurtok();
			auto subst = new ASTSubst(ident, start);
			subst->loc = curtok.loc;
			auto typedeff = new ASTType(ty, ident, subst, "", isGlobal);
			typedeff->loc = curtok.loc;
			typedeff->name = _name;
			auto proto = new ASTBinOp(ident, Op::LThanEqual, end);
			proto->loc = curtok.loc;
			
			auto val = new ASTValue(1);
			val->loc = curtok.loc;
			auto last_bin = new ASTBinOp(ident, Op::Plus, val);
			last_bin->loc = curtok.loc;
			auto last = new ASTSubst(ident, last_bin);
			last->loc = curtok.loc;
			if (curtokIs(TK::tok_lb)) {
				auto loc = curtok.loc;
				auto ast_if = new ASTIf(proto, expr_block(false));
				ast_if->loc = loc;
				auto ast = new ASTFor(typedeff, ast_if, last);
				ast->loc = loc;
				return ast;
			}
			else {
				auto loc = curtok.loc;
				auto ast_if = new ASTIf(proto, expr_block(true));
				ast_if->loc = loc;
				auto ast = new ASTFor(typedeff, ast_if, last);
				ast->loc = loc;
				return ast;
			}
		}
	}
	auto typedeff = expr_identifier();
	auto proto = expr();
	if (!curtokIs(TK::tok_semi))
		error_unexpected(curtok);
	getNextToken();
	isExpectedSemi = false;
	auto last = expr_identifier();
	isExpectedSemi = true;
	if (!curtokIs(TK::tok_rp))
		error_unexpected(curtok);
	getNextToken();
	if (curtokIs(TK::tok_lb)) {
		auto loc = curtok.loc;
		auto ast_if = new ASTIf(proto, expr_block(false));
		ast_if->loc = loc;
		auto ast = new ASTFor(typedeff, ast_if, last);
		ast->loc = loc;
		return ast;
	}
	else {
		auto loc = curtok.loc;
		auto ast_if = new ASTIf(proto, expr_block(true));
		ast_if->loc = loc;
		auto ast = new ASTFor(typedeff, ast_if, last);		
		ast->loc = loc;
		return ast;
	}
	return nullptr;
}

ASTWhile* Parser::while_statement() {
	getNextToken();
	auto boolast = expr();
	ASTWhile* ast;
	if (curtok.ty == TK::tok_lb) {
		auto loc = curtok.loc;
		ast = new ASTWhile(boolast, expr_block(false));
		ast->loc = loc;
		return ast;
	}
	else {
		auto loc = curtok.loc;
		ast = new ASTWhile(boolast, expr_block(true));
		ast->loc = loc;
		return ast;
	}
	return nullptr;
}


ASTCall* Parser::func_call(std::string func_name, bool isdoll) {
	//not allowed for stct has a function. E.g. stct.func()

	getNextToken();
	std::vector<AST*> argsIdentifier;
	while (true) {
		if (curtok.ty != TK::tok_rp)
			argsIdentifier.push_back(expr());
		else
			break;
		if (curtok.ty != TK::tok_comma) break;
		getNextToken();
	}
	if (curtok.ty != TK::tok_rp)
		error("Expected", "Expected --> )", curtok);
	auto loc = curtok.loc;
	auto ast = new ASTCall(func_name, argsIdentifier);
	ast->loc = loc;
	getNextToken();
	return ast;
}
ASTRet* Parser::def_ret() {
	getNextToken();
	Type_t rty;
	if (!curtokIs(TK::tok_semi))
		rty = getTypeFromCurtok();
	else {
		rty.isArr = false;
		rty.isList = false;
		rty.kind = TypeKind::Value;
		rty.ty = AType::Void;
		rty.name = curtok.val;
	}
	ASTRet* ast;
	if(!curtokIs(TK::tok_semi)) {
		auto loc = curtok.loc;
		ast = new ASTRet(rty, expr());
		ast->loc = loc;
		if (curtok.ty != TK::tok_semi)
			error_expected(";", curtok);
		return ast;
	}
	auto loc = curtok.loc;
	ast = new ASTRet(rty, nullptr);
	ast->loc = loc;
	getNextToken();
	
	return ast;
}

Parser::Parser(std::vector<Token_t> _tokens) : tokens(_tokens) {
	index = 0;
	curtok = tokens[index];
	cdgen = std::make_unique<Codegen>();
}
void Parser::codegen(std::vector<AST*> _ast) {
	for (int i = 0; i < _ast.size(); i++) {
		_ast[i]->codegen();
	}
	return;
}
std::vector<AST*> Parser::parse() {
	//Top level parser
	std::vector<AST*> asts;
	while (curtok.ty != TK::tok_eof) {
		this->isGlobal = true;
		if (curtok.ty == TK::tok_fn)
		{
			auto ast = def_func();
			asts.push_back(ast);
		}
		else if (curtok.ty == TK::tok_identifier) {
			auto ast = expr_identifier();
			asts.push_back(ast);
		}
		else if (curtok.ty == TK::tok_import) {
			def_import();
		}
		else if (curtok.ty == TK::tok_macro) {
			def_macro();
		}
		else getNextToken();
	}
	return asts;
}
