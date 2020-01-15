#include "../nek-ot.hpp"

std::vector<std::string> imports;

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
		imports.push_back("io");
		Sys::IO::Printf::CreateFunc();
		
		Sys::IO::Input::CreateFunc();
	}
	else if (str == "random") {
		imports.push_back("random");
		Sys::Random::CreateFunc();
	}
	else if (str == "file") {
		imports.push_back("file");
		Sys::IO::File::CreateFunc();
	}
	else {
		compile(str, isDumpllvm, isDumpollvm);
		cur_filename.pop();
	}
	return;
}