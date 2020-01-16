#include "nek-ot.hpp"

std::stack<std::string> cur_filename;
uint32_t cnt_file = 0;
std::map<std::string, std::vector<std::string>> sources;

void writeToFile() {
	std::error_code ec;
	raw_fd_ostream os("out.bc", ec, sys::fs::OpenFlags::F_None);
	WriteBitcodeToFile(*module, os);
}

Parser* compile(std::string name, bool isDumpllvm, bool isDumpollvm) {
	cur_filename.push(name);
	std::cout << ++cnt_file << ">" << name << std::endl;
	load_source();
	Tokenizer tkn;
	tkn.tokenize();
	auto parser = Parser(tkn.tytokens);
	if (isDumpllvm)
		std::cout << "Generating LLVM IR..." << std::endl;
	auto parsed_ast = parser.parse();

	parser.codegen(parsed_ast);
	
	
	return &parser;

}