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
			auto next = tokens[i+1];
			//if exists argument
			if (next.ty == TK::tok_lp) {
				tokens.erase(tokens.begin() + i + 1);
				std::vector<Token_t> real_args;
				while (true) {
					next = tokens[i + 1];
					real_args.push_back(next);
					tokens.erase(tokens.begin() + i + 1);
					next = tokens[i + 1];
					tokens.erase(tokens.begin() + i + 1);
					if (next.ty == TK::tok_comma)
						continue;
					else if (next.ty == TK::tok_rp)
						break;
					else
						error_unexpected(next);
				}
				bool overrideflg = false;
				for (int j = 0; j < mac_lit.size(); j++) {
					auto& tkn = mac_lit[j];
					if (tkn.ty == TK::tok_identifier) {
						
						for (int k = 0; k < args.size(); k++) {
							auto a = args[k];
							if (a.val == tkn.val) {
								if (overrideflg) {
									tokens.insert(tokens.begin() + i + j, real_args[k]);
								}
								else {
									overrideflg = true;
									tokens[i+j] = real_args[k];
								}
							}
							else {
								if (overrideflg) {
									tokens.insert(tokens.begin() + i + j, tkn);
								}
								else {
									overrideflg = true;
									tokens[i + j] = tkn;
								}
							}
						}
					}
					else {
						if (overrideflg) {
							tokens.insert(tokens.begin() + i + j, tkn);
						}
						else {
							overrideflg = true;
							tokens[i+j] = tkn;
						}
					}	
				}
			}
			else {
				tokens[i] = mac_lit[0];
				for (int j = 1; j < mac_lit.size(); j++) {
					tokens.insert(tokens.begin() + i + j, mac_lit[j]);
				}

			}
			t.val = mac_name;
		}
	}
	for (auto& t : tokens)
		std::cout << t.val << std::endl;
	return;
}