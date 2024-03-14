#include "Includes/preprocessor.h"
#include "Includes/memory.h"
#include "Includes/decoder.h"
#include "Includes/cpu.h"
#include "Includes/compiler.h"
#include "Includes/os.h"
os* sys=NULL;
FILE* fpcode=NULL;
FILE* fpcompiled=NULL;
void sigint_handler(int param){

	if(sys){
		endOS(&sys);
	}
	if(fpcode){
		fclose(fpcode);
	}
	if(fpcompiled){
		fclose(fpcompiled);
	}
	endwin();
	remove(TMP_FILE_NAME);
	exit(param);

}
int main(void){
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
	sys=spawnOS();
	compile(&sys->proc->dec,fpcode,fpcompiled);
	fclose(fpcompiled);
	if(!(fpcompiled=fopen(INSTR_FILE_PATH,"r"))){
		perror("Invalid file path!!!\n");
		raise(SIGINT);
	}
	loadProg(fpcompiled,sys);
	loadProg(fpcompiled,sys);
	loadProg(fpcompiled,sys);
	loadProg(fpcompiled,sys);
	loadProg(fpcompiled,sys);
	loadProg(fpcompiled,sys);
	switchOnCPU(0,sys);
	raise(SIGINT);
}
