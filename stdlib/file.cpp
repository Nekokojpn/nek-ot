#include <stdio.h>
#include <stdlib.h>

int fwrite(const char* file_name, const char* text){
    FILE *fp;
    fp = fopen(file_name, "w");
    if(fp == NULL){
        return 1;
    }
    fprintf(fp, "%s", text);
    fclose(fp);
    
    return 0;
}