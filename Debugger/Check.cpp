#include "../nek-ot.hpp"

void checkBeforeWriteToBC() {
	//Check an entry point exsists
	if (functions_global.find("main") == functions_global.end())
		error_onlymsg("No entry point!\nRequires \"main\" function.");
	return;
}