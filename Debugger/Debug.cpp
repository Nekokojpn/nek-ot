#include "../nek-ot.hpp"

void Debug::dumpType(Value* v) {
	v->getType()->dump();
}