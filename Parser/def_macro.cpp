#include "../nek-ot.hpp"
//map<name, <args, literal>>
std::map<std::string, std::pair<std::vector<std::string>, std::vector<std::string>>> macros;

void Parser::def_macro() {
	getNextToken();
	if (!curtokIs(TK::tok_identifier))
		error_unexpected(curtok);
	auto mac_name = curtok.val;
	std::vector<std::string> args;
	if (curtokIs(TK::tok_lp)) {
		getNextToken();
		for (;;) {
			if (!curtokIs(TK::tok_identifier))
				error_expected("identifier", curtok);
			args.push_back(curtok.val);
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
	std::vector<std::string> mac_lit;
	while (!curtokIs(TK::tok_line)) {
		mac_lit.push_back(curtok.val);
		getNextToken();
	}
	for (auto a : sources) {
		
	}
	macros[mac_name] = std::make_pair(args, mac_lit);
	return;
}