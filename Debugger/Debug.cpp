#include "../nek-ot.hpp"

void Debug::dumpType(Value* v) {
	v->getType()->dump();
}
void Debug::dumpLine(Location_t& t) {
	uint32_t line = t.location_begin_line, column = t.location_begin_column;
	if (sources[cur_filename.top()].size() <= line) {
		std::cerr << std::endl;
		Console::SetConsoleTextRed();
		std::cerr << "Failed to dump the code!" << std::endl;
	}


	std::cerr << std::endl;
	Console::SetConsoleTextBlue();
	std::cerr << "  --> ";
	Console::SetConsoleTextWhite();
	std::cerr << cur_filename.top() << ":" << line + 1 << ":" << column << std::endl;
	Console::SetConsoleTextBlue();
	printf("      \n%5d|", line);
	Console::SetConsoleTextWhite();
	std::string t1 = sources[cur_filename.top()][line].substr(0, sources[cur_filename.top()][line].size() - 1);
	std::cerr << "     " << t1 << std::endl;
	Console::SetConsoleTextBlue();
	printf("%5d|", line + 1);
	Console::SetConsoleTextWhite();
	for (int i = 0; i < column - 1; i++)
		if (sources[cur_filename.top()][line][i] == '\t')
			std::cerr << "    ";
		else
			std::cerr << " ";
	Console::SetConsoleTextRed();
	std::cerr << "       ^" << std::endl;
	Console::SetConsoleTextGray();
}