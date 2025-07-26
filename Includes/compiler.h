#ifndef COMPILER_H
#define COMPILER_H
#define MAX_NAMES 1024
#define COMPILER_BUFFSIZE 1024
#define MAX_MACRO_TEXT_SIZE 1024

#define DATA_STRING ".data:"
#define MACRO_STRING ".macro:"
#define CODE_STRING ".code:"

typedef struct symbol{
	//dont free this!!!
	char* string;
	int32_t value,addr;

}symbol;

typedef struct macro{

	//dont free this!!!
	char macro_tag[MAX_MACRO_TEXT_SIZE];
	char macro_text[MAX_MACRO_TEXT_SIZE];

}macro;

void compile(decoder*dec,FILE* fin,FILE* fpout);

extern FILE* fpmid;
#endif
