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
			if (!curtokIs(TK::tok_comma))
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
			tokens.erase(tokens.begin() + i);
			auto next = tokens[i];
			//if exists argument
			if (next.ty == TK::tok_lp) {
				tokens.erase(tokens.begin() + i);
				std::vector<Token_t> real_args;
				while (true) {
					next = tokens[i];
					real_args.push_back(next);
					tokens.erase(tokens.begin() + i);
					next = tokens[i];
					tokens.erase(tokens.begin() + i);
					if (next.ty == TK::tok_comma)
						continue;
					else if (next.ty == TK::tok_rp)
						break;
					else
						error_unexpected(next);
				}
				for (int j = 0; j < mac_lit.size(); j++) {
					bool flg = false;
					for (int k = 0; k < args.size(); k++) {
						if (args[k].val == mac_lit[j].val) {
							tokens.insert(tokens.begin() + i + j, real_args[k]);
							flg = true;
						}
					}
					if(!flg)
						tokens.insert(tokens.begin() + i + j, mac_lit[j]);
				}
			}
			else {
				for (int j = 0; j < mac_lit.size(); j++) {
					tokens.insert(tokens.begin() + i + j, mac_lit[j]);
				}
			}
			t.val = mac_name;
		}
	}
	return;
}