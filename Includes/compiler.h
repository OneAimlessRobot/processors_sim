#ifndef COMPILER_H
#define COMPILER_H
typedef struct name_code_pair{
	//dont free this!!!
	char* string;
	op_code code;

}name_code_pair;

typedef struct name_addr_value_triple{
	//free this!!!
	char* string;
	u_int32_t value,addr;

}name_addr_value_triple;

typedef struct name_addr_pair{
	//free this!!!
	char* string;
	u_int32_t addr;

}name_addr_pair;

void compile(cpu*proc,FILE* fin,FILE* fpout);
void process_data(char buff[1024]);
void copyInputFile(FILE* fpin);
#endif
