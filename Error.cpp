#include "nek-ot.h"

extern std::string source_filename;
extern int line;
extern int column;
extern std::vector<std::string> source;

void error(std::string title, std::string message) {
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