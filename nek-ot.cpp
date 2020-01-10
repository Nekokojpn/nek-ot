#include "nek-ot.hpp"
//Common globals----->


int tytoken_size;
std::string source_filename;
//<-----
extern std::vector<std::string> source;
extern char cc;        
extern std::string cs; 
extern std::vector<TK> tokens;
extern std::vector<std::string> literals;
extern std::vector<Token_t> tytokens;
extern std::vector<Location_t> locs;

int pos = 0;
int curtok = 0;



void compile() {
	std::error_code ec;
	raw_fd_ostream os("out.bc", ec, sys::fs::OpenFlags::F_None);
	WriteBitcodeToFile(*module, os);
}

int main(int argc, char** argv) {
	std::vector<std::string> args;
	bool isDumpllvm = false;
	bool isDumpTime = false;
	bool isDumpollvm = false;
	bool isOpt = true;
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

	std::chrono::system_clock::time_point start_tokenize, end_toknize, start_parse, end_parse, start_codegen, end_codegen, start_opt, end_opt, start_llc, end_llc, start_clang, end_clang;

	if (load_source(argv[1]) == 1)
		exit(1);
	if (isDumpTime)
		start_tokenize = std::chrono::system_clock::now();
	TK tok = gettoken();
	while (tok != TK::tok_eof) {
		if (tok != TK::tok_nope)
			tokens.push_back(tok);
		tok = gettoken();
	}
	int it = tokens.size();
#ifdef HIGH_DEBUGG
	std::cout << "-----Token dump-----" << std::endl;
#endif //  HIGHDEBUGG
	std::vector<Token_t> tytokens(tokens.size());
	for (int i = 0; i < it; i++) {
		Token_t t;
		t.ty = tokens[i];
		t.val = literals[i];
		t.loc = locs[i];
		tytokens[i] = t;
#ifdef  HIGH_DEBUGG
		std::cout << locs[i].location_begin_line << ":" << locs[i].location_begin_column << "     " << (int)tokens[i] << " " << literals[i] << "   " << std::endl;
#endif //  HIGHDEBUGG
	}
	Token_t t;
	t.ty = TK::tok_eof;
	tytokens.push_back(t);
	tytoken_size = tytokens.size();
	if (isDumpTime)
		end_toknize = std::chrono::system_clock::now();
	// Parser--->  
	if (isDumpTime)
		start_parse = std::chrono::system_clock::now();
	auto parser = Parser(tytokens);

	init_parse();

	module->setSourceFileName(argv[1]);

	Test::CreateFunc();
	Sys::Exit::CreateFunc();

	if (isDumpllvm)
		std::cout << "Generating LLVM IR..." << std::endl;


	auto parsed_ast = parser.parse();
	if (isDumpTime) {
		end_parse = std::chrono::system_clock::now();
		start_codegen = std::chrono::system_clock::now();
	}

	parser.codegen(parsed_ast);
	if (isDumpTime)
		end_codegen = std::chrono::system_clock::now();
	std::cout << std::endl;

	compile();


	std::string optArg = "";
	for (int i = 0; i < args.size(); i++) {
		optArg += " " + args[i];
	}
	if (isDumpTime)
		start_opt = std::chrono::system_clock::now();
	if(isOpt)
		system("D:\\LLVM\\llvm-project\\build\\Debug\\bin\\opt.exe -Os -o D:\\LLVM\\llvm-project\\build\\examples\\Kaleidoscope\\nek-ot\\opt.bc D:\\LLVM\\llvm-project\\build\\examples\\Kaleidoscope\\nek-ot\\out.bc");
	if (isDumpTime){
		end_opt = std::chrono::system_clock::now();
		start_llc = std::chrono::system_clock::now();
	}
	if(isOpt)
		system("D:\\LLVM\\llvm-project\\build\\Debug\\bin\\llc.exe D:\\LLVM\\llvm-project\\build\\examples\\Kaleidoscope\\nek-ot\\opt.bc");
	else
		system("D:\\LLVM\\llvm-project\\build\\Debug\\bin\\llc.exe D:\\LLVM\\llvm-project\\build\\examples\\Kaleidoscope\\nek-ot\\out.bc");
	if (isDumpTime)
		end_llc = std::chrono::system_clock::now();

	if (isDumpollvm) {
		system("D:\\LLVM\\llvm-project\\build\\Debug\\bin\\llvm-dis.exe D:\\LLVM\\llvm-project\\build\\examples\\Kaleidoscope\\nek-ot\\opt.bc");
		system("cat D:\\LLVM\\llvm-project\\build\\examples\\Kaleidoscope\\nek-ot\\opt.ll");
	}

	std::string s = "";
	for (int i = 0; i < parser.imports.size(); i++) {
		s += " D:\\LLVM\\llvm-project\\build\\examples\\Kaleidoscope\\nek-ot\\stdlib\\" + parser.imports[i] + ".s";
	}

	if (isDumpTime)
		start_clang = std::chrono::system_clock::now();
	std::string cmline = "";
	if(isOpt)
		cmline = "clang -l C:\\nek-ot\\User32.lib D:\\LLVM\\llvm-project\\build\\examples\\Kaleidoscope\\nek-ot\\opt.s" + s;
	else
		cmline = "clang -l C:\\nek-ot\\User32.lib D:\\LLVM\\llvm-project\\build\\examples\\Kaleidoscope\\nek-ot\\out.s" + s;

	if (isDumpTime)
		end_clang = std::chrono::system_clock::now();

	system(cmline.c_str());

	if (isDumpllvm) {
		std::cout << "-----LLVM IR-----" << std::endl;
		parser.dump();
	}
	if (isDumpTime) {
		std::cout << "-----time-----" << std::endl;
		double tokenize_time = static_cast<double>(
			std::chrono::duration_cast<std::chrono::microseconds>(end_toknize - start_tokenize)
			.count() /
			1000000.0);
		double parse_time = static_cast<double>(
			std::chrono::duration_cast<std::chrono::microseconds>(end_parse - start_parse)
			.count() /
			1000000.0);
		double codegen_time = static_cast<double>(
			std::chrono::duration_cast<std::chrono::microseconds>(end_codegen - start_codegen)
			.count() /
			1000000.0);
		double opt_time = static_cast<double>(
			std::chrono::duration_cast<std::chrono::microseconds>(end_opt - start_opt)
			.count() /
			1000000.0);
		double llc_time = static_cast<double>(
			std::chrono::duration_cast<std::chrono::microseconds>(end_llc - start_llc)
			.count() /
			1000000.0);
		double clang_time = static_cast<double>(
			std::chrono::duration_cast<std::chrono::microseconds>(end_clang - start_clang)
			.count() /
			1000000.0);
		double all_time = tokenize_time + parse_time + codegen_time + opt_time + llc_time + clang_time;
		printf("All time %lf[s]\n", all_time);
		printf("Tokenize time %lf[s]\n", tokenize_time);
		printf("Parse time %lf[s]\n", parse_time);
		printf("Codegen time %lf[s]\n", codegen_time);
		printf("Optimization time %lf[s]\n", opt_time);
		printf("IR compile time %lf[s]\n", llc_time);
		printf("Link time %lf[s]\n", clang_time);
	}

	return 0;
}