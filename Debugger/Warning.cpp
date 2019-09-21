#include "../nek-ot.h"

extern std::string source_filename;
extern std::vector<std::string> source;
std::string warningmsg = "";

void add_warning_msg(std::string _warningmsg) {
	warningmsg += _warningmsg + '\n';
}

void warning(std::string title, std::string message, uint32_t line, uint32_t column) {
	std::cerr << std::endl;
	Console::SetConsoleTextWhite();

	if (source.size() <= line && source[line].size() <= column) {
		std::cerr << std::endl;
		Console::SetConsoleTextRed();
		std::cerr << "Serious error: unknown error." << title << std::endl;
	}

	Console::SetConsoleTextBlue();
	std::cerr << "Warning: " << title << std::endl;
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
	std::cerr << warningmsg << std::endl;
	Console::SetConsoleTextGray();
}
void warning(std::string title, std::string message, Location_t& loc) {
	error(title, message, loc.location_begin_line, loc.location_begin_column);
}
void warning(std::string title, std::string message, Token_t& curtok) {
	error(title, message, curtok.loc);
}

void warning_unexpected(Token_t& curtok) {
	error("Warning", "Unexpected token --> " + curtok.val, curtok);
}
void warning_expected(std::string lit,Token_t& curtok) {
	error("Warning", "Expected token --> " + lit, curtok);
}