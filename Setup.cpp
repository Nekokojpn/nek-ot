#include "nek-ot.h"

extern std::string source_filename;
extern std::vector<std::string> source;

/*												load_source
		Argument: None
		return typeÅFint
		descriptionÅFRead source codes using std::ifstream and set to variable.
*/
int load_source(std::string source_path) {
	std::ifstream ifs(source_path);
	if (ifs.fail()) {
		error("no such directory or file name", "Failed to open source file.", 0, 0);
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
	std::cout << "-----Source (" << source_path << ")-----" << std::endl;
	for (std::string t : source)
		std::cout << t;
#endif
	return 0;
}