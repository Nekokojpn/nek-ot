#include "../nek-ot.hpp"

extern std::vector<std::string> source;
std::string errmsg = "";

void add_err_msg(std::string _errmsg) {
	errmsg += _errmsg + '\n';
}

void error(std::string title, std::string message, uint32_t line, uint32_t column) {

	if (source.size() <= line) {
		std::cerr << std::endl;
		Console::SetConsoleTextRed();
		std::cerr << "Compile error: " << title << std::endl << message << std::endl;
		exit(1);
	}


	std::cerr << std::endl;
	Console::SetConsoleTextRed();
	std::cerr << "Error: " << title << std::endl;
	Console::SetConsoleTextBlue();
	std::cerr << "  --> ";
	Console::SetConsoleTextWhite();
	std::cerr << source_filename << ":" << line + 1 << ":" << column << std::endl;
	Console::SetConsoleTextBlue();
	printf("      \n%5d|", line);
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
		printf("%5d|", line + ll++);
		Console::SetConsoleTextWhite();
	}
	Console::SetConsoleTextWhite();
	for (int i = 0; i < column - 1; i++)
		if (source[line][i] == '\t')
			std::cerr << "    ";
		else
			std::cerr << " ";
	Console::SetConsoleTextRed();
	std::cerr << "       ^" << std::endl;
	Console::SetConsoleTextBlue();
	printf("      ");
	Console::SetConsoleTextRed();
	std::cerr << message << std::endl;
	Console::SetConsoleTextBlue();
	printf("      ");
	Console::SetConsoleTextRed();
	std::cerr << errmsg << std::endl;
	Console::SetConsoleTextGray();
	exit(1);
}

void error_onlymsg(std::string message) {

	std::cerr << std::endl;
	Console::SetConsoleTextRed();
	std::cerr << "Error: Compile error" << std::endl;

	
	std::cerr << std::endl << message << std::endl;

	exit(1);
}

void error(std::string title, std::string message, Location_t& loc) {
	error(title, message, loc.location_begin_line, loc.location_begin_column);
}
void error(std::string title, std::string message, Token_t& curtok) {
	error(title, message, curtok.loc);
}
void error(std::string message, Token_t& curtok) {
	error("Compile error",message, curtok);
}

void error_unexpected(Token_t& curtok) {
	error("Compile error", "Unexpected token --> " + curtok.val, curtok);
}
void error_expected(std::string lit,Token_t& curtok) {
	error("Compile error", "Expected token --> " + lit, curtok);
}
void error_codegen(std::string message, Location_t loc) {
	error("Compile error", message, loc);
}