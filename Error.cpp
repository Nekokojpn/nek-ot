#include "nek-ot.h"

extern std::string source_filename;
extern std::vector<std::string> source;

void error(std::string title, std::string message, int line, int column) {
	std::cerr << std::endl;
	Console::SetConsoleTextRed();
	std::cerr << "error: " << title << std::endl;
	Console::SetConsoleTextBlue();
	std::cerr << " --> ";
	Console::SetConsoleTextWhite();
	std::cerr << source_filename << ":" << line + 1 << ":" << column << std::endl;
	Console::SetConsoleTextBlue();
	std::cerr << "  |" << std::endl << line + 1 << " |";
	Console::SetConsoleTextWhite();
	std::string t = source[line].substr(0, source[line].size() - 1);
	int i = 0;
	while (isspace(t[i++]));
	t = t.substr(i - 1, t.size() - 1);
	std::cerr << "     " << t << std::endl;
	Console::SetConsoleTextBlue();
	std::cerr << "  |";
	Console::SetConsoleTextWhite();
	std::cerr << "     ";
	for (int i = 0; i < column - 1; i++)
		std::cerr << " ";
	Console::SetConsoleTextRed();
	std::cerr << "^" << std::endl;
	Console::SetConsoleTextBlue();
	std::cerr << "  |";
	Console::SetConsoleTextWhite();
	for (int i = 0; i < column - 1; i++)
		std::cerr << " ";
	Console::SetConsoleTextRed();
	std::cerr << message << std::endl;
	Console::SetConsoleTextBlue();
	std::cerr << "  |";
	exit(1);
}
void error(std::string title, std::string message, Location_t& loc) {
	std::cerr << std::endl;
	Console::SetConsoleTextRed();
	std::cerr << "error: " << title << std::endl;
	Console::SetConsoleTextBlue();
	std::cerr << " --> ";
	Console::SetConsoleTextWhite();
	std::cerr << source_filename << ":" << loc.location_begin_line + 1 << ":" << loc.location_begin_column << std::endl; //"~" << loc.location_end_line << ":" << loc.location_end_column << std::endl;
	Console::SetConsoleTextBlue();
	std::cerr << "  |" << std::endl << loc.location_begin_line+1 << " |";
	Console::SetConsoleTextWhite();
	std::string t = source[loc.location_begin_line].substr(0, source[loc.location_begin_line].size() - 1);
	int i = 0;
	while (isspace(t[i++]));
	t = t.substr(i - 1, t.size() - 1);
	std::cerr << "     " << t << std::endl;
	Console::SetConsoleTextBlue();
	std::cerr << "  |";
	Console::SetConsoleTextWhite();
	std::cerr << "     ";
	for (int i = 0; i < loc.location_begin_column - 1; i++)
		std::cerr << " ";
	Console::SetConsoleTextRed();
	if (loc.location_begin_column <= loc.location_end_column) {
		for (int i = loc.location_begin_column; i <= loc.location_end_column; i++) {
			std::cerr << "^" << std::endl;
		}
	}
	else {
		std::cerr << "^" << std::endl;
	}
	Console::SetConsoleTextBlue();
	std::cerr << "  |";
	Console::SetConsoleTextWhite();
	for (int i = 0; i < loc.location_begin_column - 1; i++)
		std::cerr << " ";
	Console::SetConsoleTextRed();
	std::cerr << message << std::endl;
	Console::SetConsoleTextBlue();
	std::cerr << "  |";
	exit(1);
}
void error(std::string title, std::string message, Token_t& curtok) {
	error(title, message, curtok.loc);
}

void error_unexpected(Token_t& curtok) {
	error("Unexpected", "Unexpected token --> " + curtok.val, curtok);
}