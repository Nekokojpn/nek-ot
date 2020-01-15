#include "nek-ot.hpp"

/*												load_source
		Argument: None
		return typeÅFint
		descriptionÅFRead source codes using std::ifstream and set to variable.
*/
void load_source() {
	std::ifstream ifs(cur_filename.top());
	if (ifs.fail()) {
		error("No such directory or file name", "", 0, 0);
		return;
	}
	std::string buf;
	while (getline(ifs, buf)) {
		std::string t = buf + '\n';
		sources[cur_filename.top()].push_back(t);
	}
	buf = "";
	buf += '\0';
	sources[cur_filename.top()].push_back(buf);
	return;
}