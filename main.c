#include "Includes/preprocessor.h"
#include "Includes/memory.h"
#include "Includes/cpu.h"
#include "Includes/alu.h"
#include "Includes/compiler.h"
cpu* proc=NULL;
memory* mem=NULL;
FILE* fpcode=NULL;
FILE* fpcompiled=NULL;
void sigint_handler(int param){

	if(proc){
		endCPU(&proc);
	}
	if(mem){
		endMemory(&mem);
	}
	if(fpcode){
		fclose(fpcode);
	}
	if(fpcompiled){
		fclose(fpcompiled);
	}
	exit(-1);

}
int main(int argc, char ** argv){
	signal(SIGINT,sigint_handler);
	if(!(fpcode=fopen(PROGRAM_FILE_PATH,"r"))){

		perror("Invalid file path!!!\n");
		exit(-1);
	}
	if(!(fpcompiled=fopen(INSTR_FILE_PATH,"w"))){
		fclose(fpcode);
		perror("Invalid file path!!!\n");
		exit(-1);
	}
	
	mem=spawnMemory();
	proc=spawnCPU(mem);
	if(!proc){
		printf("ERRO A INICIAR CPU!!!\n");
		raise(SIGINT);
	}
	compile(proc,fpcode,fpcompiled);
	fclose(fpcompiled);
	if(!(fpcompiled=fopen(INSTR_FILE_PATH,"r"))){
		perror("Invalid file path!!!\n");
		raise(SIGINT);
	}
	initMemory(fpcompiled,mem);
	loadProg(proc);
	switchOnCPU(proc);
	raise(SIGINT);
}
