#ifndef COMPILER_H
#define COMPILER_H
typedef struct name_code_pair{
	//dont free this!!!
	char* string;
	op_code code;

}name_code_pair;

void compile(cpu*proc,FILE* fin,FILE* fpout);
#endif
