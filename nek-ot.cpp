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

//<-----

//Common Funcs----->

//<-----



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
  start_tokenize = std::chrono::system_clock::now();
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
  start_parse = std::chrono::system_clock::now();
  auto parser = Parser(tytokens);
  std::string uo = "-O3\0";
  char* up = &uo[0];
  if (argc >= 3 && *argv[2] == *up) {
	  parser.setOpt(true);
  }
  else
	  parser.setOpt(false);

  init_parse();
  
  Test::CreateFunc();
  //Sys::IO::OutPuti8Ptr::CreateFunc();
  //Sys::Cast::CastInt32toInt8ptr::CreateFunc();
  //Sys::Cast::CastInt32toInt8Array::CreateFunc();
  Sys::IO::Printf::CreateFunc();
  Sys::IO::Printfln::CreateFunc();
  Sys::IO::Input::CreateFunc();

  std::cout << "Generating LLVM IR..." << std::endl;

  auto parsed_ast = parser.parse();
  end_parse = std::chrono::system_clock::now();
  start_codegen = std::chrono::system_clock::now();
  parser.codegen(std::move(parsed_ast));
  end_codegen = std::chrono::system_clock::now();
  std::cout << std::endl;
  /*
  auto targetTriple = sys::getDefaultTargetTriple();
  InitializeAllTargetInfos();
  InitializeAllTargets();
  InitializeAllTargetMCs();
  InitializeAllAsmParsers();
  InitializeAllAsmPrinters();
  std::string err;
  auto target = TargetRegistry::lookupTarget(targetTriple, err);
  auto CPU = "generic";
  auto features = "";
  TargetOptions opt;
  auto rm = Optional<Reloc::Model>();
  auto targetMachine = target->createTargetMachine(targetTriple, CPU, features, opt, rm);
  getModule()->setDataLayout(targetMachine->createDataLayout());
  getModule()->setTargetTriple(targetTriple);
  */
  std::error_code ec;
  raw_fd_ostream os("out.bc", ec, sys::fs::OpenFlags::F_None);
  WriteBitcodeToFile(*getModule(), os);
  
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