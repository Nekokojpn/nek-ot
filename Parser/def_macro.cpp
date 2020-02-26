#include "../nek-ot.hpp"

void Parser::def_macro() {
	getNextToken();
	if (!curtokIs(TK::tok_identifier))
		error_unexpected(curtok);
	auto mac_name = curtok.val;
	getNextToken();
	std::vector<Token_t> args;
	if (curtokIs(TK::tok_lp)) {
		getNextToken();
		for (;;) {
			if (!curtokIs(TK::tok_identifier))
				error_expected("identifier", curtok);
			args.push_back(curtok);
			getNextToken();
			if (!curtokIs(TK::tok_colon))
				break;
			getNextToken();
		}
		if (!curtokIs(TK::tok_rp))
			error_expected(")", curtok);
		getNextToken();
	}
	if (!curtokIs(TK::tok_arrow))
		error_expected("->", curtok);
	getNextToken();
	std::vector<Token_t> mac_lit;
	while (!curtokIs(TK::tok_line)) {
		mac_lit.push_back(curtok);
		getNextToken();
	}
	//All tokens
	for (int i = this->index; i < tokens.size(); i++) {
		auto t = tokens[i];
		//if curtok is replacable by macro rules.
		if (t.ty == TK::tok_identifier && t.val == mac_name) {
			auto next = tokens[i+1];
			//if exists argument
			if (next.ty == TK::tok_lp) {
				auto cnt = 2;
				next = tokens[i + cnt++];
				
			}
			else {
				for (auto& tkn : mac_lit) {
					/*if (tkn.ty == TK::tok_identifier) {
						for (auto& c : args) {
							if (c.val == tkn.val) {
								//tokens[i++] =
							}
							else
								tokens[i++] = tkn;
						}
					}*/
					tokens[i] = tkn;
				}
			}
			t.val = mac_name;
		}
	}
	return;
}