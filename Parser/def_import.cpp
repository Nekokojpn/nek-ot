#include "../nek-ot.hpp"

void Parser::def_import() {
	getNextToken();
	if (!curtokIs(TK::tok_dq))error("Syntax error. Expected syntax --> import \"<lib name>\"", curtok);
	getNextToken();
	if(!curtokIs(TK::tok_str_string))error("Syntax error. Expected syntax --> import \"<lib name>\"", curtok);
	auto str = curtok.val;
	getNextToken();
	if (!curtokIs(TK::tok_dq))error("Syntax error. Expected syntax --> import \"<lib name>\"", curtok);
	getNextToken();
	if (!curtokIs(TK::tok_semi))error_expected(";", curtok);
	getNextToken();
	if (str == "io") {
		this->imports.push_back("io");
		Sys::IO::Printf::CreateFunc();
		Sys::IO::Printfln::CreateFunc();
		Sys::IO::Input::CreateFunc();
	}
	else {
		std::string s = "There is no lib name --> " + str;
		error(s, curtok);
	}
	return;
}