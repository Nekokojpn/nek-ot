#include "../nek-ot.hpp"


ASTStctElements* Parser::expr_stct() {
	Stct_t st;
	auto cnt = 0ULL;
	while (true) {
		if (curtok.ty != TK::tok_identifier)
			error_unexpected(curtok);
		StctElm_t t;
		auto id = curtok.val;
		t.elem_name = id;
		t.idx = cnt++;
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
	auto ast = new ASTStctElements(st);
	ast->loc = loc;
	return ast;
}