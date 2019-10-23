#include "../nek-ot.hpp"


std::unique_ptr<AST> Parser::def_stct() {
	getNextToken();
	if (curtok.ty != TK::tok_identifier) {
		add_err_msg(curtok.val + " may be used as a reserved word.");
		error_unexpected(curtok);
	}
	add_userdefined_stct(curtok);
	auto name = curtok.val;
	getNextToken();
	if (curtok.ty != TK::tok_lb) {
		error_unexpected(curtok);
	}
	auto loc = curtok.loc;
	auto elements = std::move(expr_stct());

	if (curtok.ty != TK::tok_semi)
		error("Expected", "Expected --> ;", curtok);
	getNextToken();
	elements->elements.stct_name = name;
	auto ast = std::make_unique<ASTStruct>(name, std::move(elements));
	ast->loc = loc;
	return std::move(ast);
}

std::unique_ptr<ASTStctElements> Parser::expr_stct() {
	getNextToken();
	Stct_t st;
	auto i = 0ULL;
	while (true) {
		if (curtok.ty != TK::tok_identifier)
			error_unexpected(curtok);
		StctElm_t t;
		auto id = curtok.val;
		t.elem_name = id;
		t.idx = i++;
		getNextToken();
		if (curtok.ty != TK::tok_colon)
			error_unexpected(curtok);
		getNextToken();
		auto ty = this->getTypeFromCurtok();
		t.elem_ty = ty;

		if (ty.ty == AType::Nop)break;
		st.elems[id] = t;
		st.elem_names.push_back(id);

		if (curtok.ty != TK::tok_comma) break;
		getNextToken();

	}
	if (curtok.ty != TK::tok_rb)
		error("Expected", "Expected --> }", curtok);
	getNextToken();
	auto loc = curtok.loc;
	auto ast = std::make_unique<ASTStctElements>(st);
	ast->loc = loc;
	return std::move(ast);
}