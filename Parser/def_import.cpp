#include "../nek-ot.hpp"

std::map<std::string, bool> imports;

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
		imports["io"] = true;
		Sys::IO::Printf::CreateFunc();
		
		Sys::IO::Input::CreateFunc();
	}
	else if (str == "random") {
		imports["random"] = true;
		Sys::Random::CreateFunc();
	}
	else if (str == "file") {
		imports["file"] = true;
		Sys::IO::File::CreateFunc();
	}
	else if (str == "sleep") {
		imports["sleep"] = true;
		Sys::Sleep::CreateFunc();
	}
	else {
		compile(str, isDumpllvm, isDumpollvm);
		cur_filename.pop();
	}
	return;
}