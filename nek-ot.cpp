#include "nek-ot.hpp"

bool isDumpllvm = false;
bool isDumpTime = false;
bool isDumpollvm = false;
bool isOpt = true;


int main(int argc, char** argv) {
	std::vector<std::string> args;
	for (int i = 2; i < argc; i++) {
		if (strcmp(argv[i], "-O0") == 0) {
			isOpt = false;
			args.push_back("-O0");
		}
		else if (strcmp(argv[i], "-O1") == 0)
			args.push_back("-O1");
		else if (strcmp(argv[i], "-O2") == 0)
			args.push_back("-O2");
		else if (strcmp(argv[i], "-O3") == 0)
			args.push_back("-O3");
		else if (strcmp(argv[i], "-Ofast") == 0)
			args.push_back("-Ofast");
		else if (strcmp(argv[i], "-Os") == 0)
			args.push_back("-Os");
		else if (strcmp(argv[i], "-Og") == 0)
			args.push_back("-Og");
		else if (strcmp(argv[i], "-dllvm") == 0)
			isDumpllvm = true;
		else if (strcmp(argv[i], "-time") == 0)
			isDumpTime = true;
		else if (strcmp(argv[i], "-dollvm") == 0)
			isDumpollvm = true;
		else {
			std::string a = argv[i];
			error_onlymsg("Unkown argument --> " + a);
		}
	}

	std::chrono::system_clock::time_point ctime, ectime;
	if (isDumpTime)
		ctime = std::chrono::system_clock::now();

	init_parse();
	Sys::Exit::CreateFunc();
	compile(argv[1], isDumpllvm, isDumpollvm);

	checkBeforeWriteToBC();
	if (isDumpllvm) {
		std::cout << "-----LLVM IR-----" << std::endl;
		dump();
	}
	writeToFile();

	std::string optArg = "";
	for (int i = 0; i < args.size(); i++) {
		optArg += " " + args[i];
	}
	if(isOpt)
		system("D:\\LLVM\\llvm-project\\build\\Debug\\bin\\opt.exe -Os -o D:\\LLVM\\llvm-project\\build\\examples\\Kaleidoscope\\nek-ot\\opt.bc D:\\LLVM\\llvm-project\\build\\examples\\Kaleidoscope\\nek-ot\\out.bc");

	if(isOpt)
		system("D:\\LLVM\\llvm-project\\build\\Debug\\bin\\llc.exe D:\\LLVM\\llvm-project\\build\\examples\\Kaleidoscope\\nek-ot\\opt.bc");
	else
		system("D:\\LLVM\\llvm-project\\build\\Debug\\bin\\llc.exe D:\\LLVM\\llvm-project\\build\\examples\\Kaleidoscope\\nek-ot\\out.bc");

	if (isDumpollvm) {
		system("D:\\LLVM\\llvm-project\\build\\Debug\\bin\\llvm-dis.exe D:\\LLVM\\llvm-project\\build\\examples\\Kaleidoscope\\nek-ot\\opt.bc");
		system("cat D:\\LLVM\\llvm-project\\build\\examples\\Kaleidoscope\\nek-ot\\opt.ll");
	}

	std::string s = "";
	for (auto i : imports) {
		s += " D:\\LLVM\\llvm-project\\build\\examples\\Kaleidoscope\\nek-ot\\stdlib\\" + i.first + ".s";
	}

	std::string cmline = "";
	if(isOpt)
		cmline = "clang -l C:\\nek-ot\\User32.lib D:\\LLVM\\llvm-project\\build\\examples\\Kaleidoscope\\nek-ot\\opt.s" + s;
	else
		cmline = "clang -l C:\\nek-ot\\User32.lib D:\\LLVM\\llvm-project\\build\\examples\\Kaleidoscope\\nek-ot\\out.s" + s;

	system(cmline.c_str());

	
	if (isDumpTime) {
		std::cout << "-----time-----" << std::endl;
		ectime = std::chrono::system_clock::now();
		double all_time = static_cast<double>(
			std::chrono::duration_cast<std::chrono::microseconds>(ectime - ctime)
			.count() /
			1000000.0);
		printf("Time %lf[s]\n", all_time);
	}

	return 0;
}