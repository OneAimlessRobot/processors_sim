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
		printMemory(1,sys->mem);
		endOS(&sys);
	}
	if(fpcode){
		fclose(fpcode);
	}
	if(fpcompiled){
		fclose(fpcompiled);
	}
	if(fpmid){
		fclose(fpmid);
	}
	nocbreak();
	endwin();
	getc(stdin);
	remove(TMP_FILE_NAME);
	exit(param);

}
void sigpipe_handler(int param){

	if(sys){
		endOS(&sys);
	}
	if(fpcode){
		fclose(fpcode);
	}
	if(fpcompiled){
		fclose(fpcompiled);
	}
	if(fpmid){
		fclose(fpmid);
	}
	endwin();
	remove(TMP_FILE_NAME);
	exit(param);

}
int main(void){
	signal(SIGINT,sigint_handler);
	signal(SIGPIPE,sigpipe_handler);
	if(!(fpcode=fopen(PROGRAM_FILE2_PATH,"r"))){

		perror("Invalid file path!!!\n");
		exit(-1);
	}
	if(!(fpcompiled=fopen(INSTR_FILE_PATH,"w+"))){
		fclose(fpcode);
		perror("Invalid file path!!!\n");
		exit(-1);
	}
	if(!(sys=spawnOS())){

		raise(SIGINT);
	}
	compile(sys->proc->dec2,fpcode,fpcompiled);
	fclose(fpcompiled);
	if(!(fpcompiled=fopen(INSTR_FILE_PATH,"r"))){
		fclose(fpcode);
		perror("Invalid file path!!!\n");
		raise(SIGINT);
	}
	
	int fd=1;
	int flags=0;
	if(fd>=1){

	flags = fcntl(0, F_GETFD);
    	if (flags == -1){
		perror("Erro nas flags!!!\n");
		raise(SIGINT);
	}
    	flags |= FD_CLOEXEC;
    	if (fcntl(0, F_SETFD, flags) == -1){

		perror("Erro nas flags!!!\n");
		raise(SIGINT);
	
	}

	}
	loadProg(fpcompiled,sys);
	switchOnCPU(fd,sys);
	
	raise(SIGINT);
}
