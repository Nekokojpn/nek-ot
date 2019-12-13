#include "../nek-ot.hpp"

ASTType* Parser::def_type(AST* ast_id) {
	getNextToken();
	std::string stct_name = curtok.val;
	auto ty = getTypeFromCurtok();
	ASTSubst* expr_ = nullptr;
	if (curtok.ty == TK::tok_semi) {
		getNextToken();
		auto loc = curtok.loc;
		auto ast = new ASTType(ty,
			ast_id,
			expr_,
			stct_name,
			this->cdgen->IsGlobal()
			);
		ast->name = this->curval;
		ast->loc = loc;
		return ast;
	}
	auto loc = curtok.loc;
	ASTType* ast = nullptr;
	//Arr
	if (curtokIs(TK::tok_lb)) {
		ast = new ASTType(ty,
			ast_id,
			expr(),
			stct_name,
			this->cdgen->IsGlobal()
		);
	}
	else {
		ast = new ASTType(ty,
			ast_id,
			new ASTSubst(ast_id, expr()),
			stct_name,
			this->cdgen->IsGlobal()
		);
	}
	ast->name = this->curval;
	ast->loc = loc;
	if (this->isExpectedSemi && curtok.ty != TK::tok_semi)
		error_expected(";", curtok);
	if(this->isExpectedSemi)getNextToken();
	this->isExpectedSemi = true;
	return ast;
	loc = curtok.loc;
	error_unexpected(curtok);
	return nullptr;
}