#ifndef COMPILER_H
#define COMPILER_H
#define MAX_NAMES 1024


typedef struct symbol{
	//dont free this!!!
	char* string;
	int32_t value,addr;

}symbol;

void compile(decoder*dec,FILE* fin,FILE* fpout);

extern FILE* fpmid;
#endif
