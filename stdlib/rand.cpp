#include <stdlib.h>

int random(){
    return rand();
}
int srandom(unsigned int seed){
    return srand(seed);
}