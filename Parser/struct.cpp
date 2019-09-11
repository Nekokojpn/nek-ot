#include "../nek-ot.h"

std::unique_ptr<AST> Parser::def_stct() {
	getNextToken();
	if (curtok.ty != TK::tok_identifier) {
		add_err_msg(curtok.val + " may be used as a reserved word.");
		error_unexpected(curtok);
	}
	auto name = curtok.val;
	getNextToken();
	if (curtok.ty != TK::tok_lb) {
		error_unexpected(curtok);
	}
	getNextToken();

	std::vector<AType> putsArgs;
	std::vector<std::string> argsIdentifier;
	while (true) {
		AType ty = getATypeByCurtok();
		if (ty == AType::Nop)break;

		putsArgs.push_back(ty);

		getNextToken();
		if (curtok.ty != TK::tok_identifier)
			error_unexpected(curtok);
		argsIdentifier.push_back(curtok.val);
		getNextToken();
		if (curtok.ty != TK::tok_comma) break;
		getNextToken();
	}

	if (curtok.ty != TK::tok_rb)
		error("Expected", "Expected --> }", curtok);
	getNextToken();
	if (curtok.ty != TK::tok_semi)
		error("Expected", "Expected --> ;", curtok);
	getNextToken();
	return std::make_unique<ASTStruct>(name, putsArgs);
}