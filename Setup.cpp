#include "nek-ot.h"

extern std::string source_filename;
extern std::vector<std::string> source;

/*												load_source
		引数：なし
		戻り値：int
		概要：std::ifstreamを使用してソースファイル"source_test.nk"を読み
		込みます。それをグローバル変数std::vector<std:string> sourceにセットします。
*/
int load_source(std::string source_path) {
	std::ifstream ifs(source_path);
	if (ifs.fail()) {
		error("no such directory or file name", "Failed to open source file.");
		return 1;
	}
	source_filename = source_path;
	std::string buf;
	while (getline(ifs, buf)) {
		std::string t = buf + '\n';
		source.push_back(t);
	}
	buf = "";
	buf += '\0';
	source.push_back(buf);
#ifdef HIGH_DEBUGG
	std::cout << "-----Source-----" << std::endl;
	for (std::string t : source)
		std::cout << t;
#endif
	return 0;
}