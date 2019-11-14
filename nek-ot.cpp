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
	WriteBitcodeToFile(*getModule(), os);
}

int main(int argc, char** argv) {
#ifdef DEBUGG
	std::chrono::system_clock::time_point start_tokenize, end_toknize, start_parse, end_parse, start_codegen, end_codegen;
#endif
#ifdef DEBUGG
	if (load_source(argv[1]) == 1)
		exit(1);
#else
  if (load_source(static_cast<std::string>(argv[1])) == 1)
    return 1;
#endif
#ifdef DEBUGG
  start_tokenize = std::chrono::system_clock::now();
#endif
  TK tok = gettoken();
  while (tok != TK::tok_eof) {
	if(tok!=TK::tok_nope)
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
	std::cout  << locs[i].location_begin_line << ":" << locs[i].location_begin_column << "     " << (int)tokens[i] << " " << literals[i] << "   " << std::endl;
#endif //  HIGHDEBUGG
  }
  Token_t t;
  t.ty = TK::tok_eof;
  tytokens.push_back(t);
  tytoken_size = tytokens.size();
#ifdef DEBUGG
  end_toknize = std::chrono::system_clock::now();
#endif
  // Parser--->  
#ifdef DEBUGG
  start_parse = std::chrono::system_clock::now();
#endif
  auto parser = Parser(tytokens);
  char o0[] = "-O0";
  char o1[] = "-O1";
  char o2[] = "-O2";
  char o3[] = "-O3";
  char o4[] = "-Ofast";
  char os[] = "-Os";
  char og[] = "-Og";
  parser.setOpt(3);
  /*
  for (int i = 2; i <= argc; i++) {
	  if (*argv[i] == *o0)
		  parser.setOpt(0);
	  else if (*argv[i] == *o1)
		  parser.setOpt(1);
	  else if (*argv[i] == *o2)
		  parser.setOpt(2);
	  else if (*argv[i] == *o3)
		  parser.setOpt(3);
	  else if (*argv[i] == *o4)
		  parser.setOpt(4);
	  else if (*argv[i] == *os)
		  parser.setSOpt(1);
	  else if (*argv[i] == *og)
		  parser.setSOpt(0);
	  else
		  parser.setOpt(0);
  }
  */
  init_parse();
  
  Test::CreateFunc();
  //Sys::IO::OutPuti8Ptr::CreateFunc();
  //Sys::Cast::CastInt32toInt8ptr::CreateFunc();
  //Sys::Cast::CastInt32toInt8Array::CreateFunc();
  Sys::IO::Printf::CreateFunc();
  Sys::IO::Printfln::CreateFunc();
  Sys::IO::Input::CreateFunc();

#ifdef DEBUGG
  std::cout << "Generating LLVM IR..." << std::endl;
#endif

  auto parsed_ast = parser.parse();
#ifdef DEBUGG
  end_parse = std::chrono::system_clock::now();
  start_codegen = std::chrono::system_clock::now();
#endif
  parser.codegen(std::move(parsed_ast));
#ifdef DEBUGG
  end_codegen = std::chrono::system_clock::now();
#endif
  std::cout << std::endl;

  compile();
  if(parser.getOpt() == 0)
	  system("D:\\LLVM\\llvm-project\\build\\Debug\\bin\\opt.exe -O0 -o opt.bc out.bc");
  else if (parser.getOpt() == 1)
	  system("D:\\LLVM\\llvm-project\\build\\Debug\\bin\\opt.exe -O1 -o opt.bc out.bc");
  else if (parser.getOpt() == 2)
	  system("D:\\LLVM\\llvm-project\\build\\Debug\\bin\\opt.exe -O2 -o opt.bc out.bc");
  else if (parser.getOpt() == 3)
	  system("D:\\LLVM\\llvm-project\\build\\Debug\\bin\\opt.exe -O3 -o opt.bc out.bc");
  else if (parser.getOpt() == 4)
	  system("D:\\LLVM\\llvm-project\\build\\Debug\\bin\\opt.exe -Ofast -o opt.bc out.bc");
  else if (parser.getSOpt() == 0)
	  system("D:\\LLVM\\llvm-project\\build\\Debug\\bin\\opt.exe -Og -o opt.bc out.bc");
  else
	  system("D:\\LLVM\\llvm-project\\build\\Debug\\bin\\opt.exe -Os -o opt.bc out.bc");

  system("D:\\LLVM\\llvm-project\\build\\Debug\\bin\\llc.exe opt.bc");
  system("clang -l C:\\nek-ot\\User32.lib opt.s input.s");
  
#ifdef DEBUGG
  std::cout << "-----LLVM IR-----" << std::endl;
  parser.dump();
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
  double all_time = tokenize_time + parse_time + codegen_time;
  printf("All time %lf[s]\n", all_time);
  printf("Tokenize time %lf[s]\n", tokenize_time);
  printf("Parse time %lf[s]\n", parse_time);
  printf("Codegen time %lf[s]\n", codegen_time);
#endif
  
  return 0;
}