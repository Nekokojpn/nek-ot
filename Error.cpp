#include "nek-ot.h"

extern std::string source_filename;
extern std::vector<std::string> source;
std::string errmsg = "";

void add_err_msg(std::string _errmsg) {
	errmsg += _errmsg + '\n';
}

void error(std::string title, std::string message, uint32_t line, uint32_t column) {
	std::cerr << std::endl;
	Console::SetConsoleTextWhite();
	std::cerr << "nek-ot compiler error repoting:" << std::endl;

	if (source.size() <= line && source[line].size() <= column) {
		std::cerr << std::endl;
		Console::SetConsoleTextRed();
		std::cerr << "Serious error: unknown error." << title << std::endl;
	}

	std::cerr << std::endl;
	Console::SetConsoleTextRed();
	std::cerr << "error: " << title << std::endl;
	Console::SetConsoleTextBlue();
	std::cerr << " --> ";
	Console::SetConsoleTextWhite();
	std::cerr << source_filename << ":" << line + 1 << ":" << column << std::endl;
	Console::SetConsoleTextBlue();
	std::cerr << line - 1 << "  |" << std::endl << line << " |";
	Console::SetConsoleTextWhite();
	std::vector<std::string> t;
	if (line - 1 >= 0)
		t.push_back(source[line - 1].substr(0, source[line - 1].size() - 1));
	std::string t1 = source[line].substr(0, source[line].size() - 1);
	t.push_back(t1);
	int ll = 1;
	for (std::string s : t) {
		std::cerr << "     " << s << std::endl;
		Console::SetConsoleTextBlue();
		std::cerr << line + ll++ << " |";
		Console::SetConsoleTextWhite();
	}
	Console::SetConsoleTextWhite();
	for (int i = 0; i < column - 1; i++)
		std::cerr << " ";
	Console::SetConsoleTextRed();
	std::cerr << message << std::endl;
	Console::SetConsoleTextBlue();
	std::cerr << line + ll << " |" << std::endl;
	Console::SetConsoleTextRed();
	std::cerr << errmsg << std::endl;
	Console::SetConsoleTextGray();
	exit(1);
}
void error(std::string title, std::string message, Location_t& loc) {
	error(title, message, loc.location_begin_line, loc.location_begin_column);
}
void error(std::string title, std::string message, Token_t& curtok) {
	error(title, message, curtok.loc);
}

void error_unexpected(Token_t& curtok) {
	error("Compile error", "Unexpected token --> " + curtok.val, curtok);
}
void error_expected(std::string lit,Token_t& curtok) {
	error("Compile error", "Expected token --> " + lit, curtok);
}