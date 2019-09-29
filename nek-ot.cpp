#include "nek-ot.h"
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
	std::chrono::system_clock::time_point start, end_toknize, end;
	start = std::chrono::system_clock::now();
#endif
#ifdef DEBUGG
	if (load_source(argv[1]) == 1)
		exit(1);
#else
  if (load_source(static_cast<std::string>(argv[1])) == 1)
    return 1;
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
  for (int i = 0; i < it; i++) {
    Token_t t;
    t.ty = tokens[i];
    t.val = literals[i];
	t.loc = locs[i];
    tytokens.push_back(t);
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
  auto parser = Parser(tytokens);
  std::string uo = "-Uo\0";
  char* up = &uo[0];
  if (argc >= 3 && *argv[2] == *up) {
	  parser.setOpt(false);
  }

  init_parse();
  
  Test::CreateFunc();
  //Sys::IO::OutPuti8Ptr::CreateFunc();
  //Sys::Cast::CastInt32toInt8ptr::CreateFunc();
  //Sys::Cast::CastInt32toInt8Array::CreateFunc();
  Sys::IO::Printf::CreateFunc();
  Sys::IO::Printfln::CreateFunc();
  Sys::IO::Input::CreateFunc();

  std::cout << "Generating LLVM IR..." << std::endl;

  parser.codegen(std::move(parser.parse()));
  
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
  end = std::chrono::system_clock::now();
  std::cout << "-----LLVM IR-----" << std::endl;
  parser.dump();
  std::cout << "-----time-----" << std::endl;
  double all_time = static_cast<double>(
	  std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() /
	  1000000.0);
  double toknize_time = static_cast<double>(
	  std::chrono::duration_cast<std::chrono::microseconds>(end_toknize - start)
	  .count() /
	  1000000.0);
  double parse_time = static_cast<double>(
	  std::chrono::duration_cast<std::chrono::microseconds>(end - end_toknize)
	  .count() /
	  1000000.0);
  printf("All time %lf[s]\n", all_time);
  printf("Tokenize time %lf[s]\n", toknize_time);
  printf("Parse time %lf[s]\n", parse_time);  
#endif
  
  return 0;
}