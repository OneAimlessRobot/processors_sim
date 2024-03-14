#include <stdlib.h>
#include <ncurses.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include "../Includes/defaults.h"
#include "../Includes/helper.h"
#include "../Includes/memory.h"
#include "../Includes/decoder.h"
#include "../Includes/cpu.h"
#include "../Includes/os.h"

static void printCPURegs(int fd,cpu* processor){
	if(fd>=1){
	
       dprintf(fd,"State of the registers:\nSize: %u\n",processor->reg_file_size);
        for(u_int8_t i=0;i<processor->reg_file_size;i++){
        	dprintf(fd,"Reg %u: [",i);
	        printWord(fd,getProcRegValue(processor,i,0));
		dprintf(fd,"] Value: %u\n",getProcRegValue(processor,i,0));
        }
		dprintf(fd,"\nStatus word :[");
	        printWord(fd, processor->status_word);
		dprintf(fd,"] Value: %u\n",processor->status_word);
	}

	else{
       printw("State of the registers:\nSize: %u\n",processor->reg_file_size);
        for(u_int8_t i=0;i<processor->reg_file_size;i++){
        	printw("Reg %u: [",i);
	        printWordNcurses(getProcRegValue(processor,i,0));
		printw("] Value: %u\n",getProcRegValue(processor,i,0));
        }
		printw("\nStatus word :[");
	        printWordNcurses( processor->status_word);
		printw("] Value: %u\n",processor->status_word);
	}

}
static void copyCPUStateToContext(cpu* proc,context* c){
	u_int32_t reg_file_size=proc->reg_file_size;
	memcpy(c->reg_state,proc->reg_file,reg_file_size*WORD_SIZE);
	c->status_word=proc->status_word;
	c->curr_pc=proc->curr_pc;
	c->prev_pc=proc->prev_pc;

}
static void loadContextIntoCPU(context* c,cpu*proc){
	memcpy(proc->reg_file,c->reg_state,proc->reg_file_size*WORD_SIZE);
	proc->status_word=c->status_word;
	proc->curr_pc=c->curr_pc;
	proc->prev_pc=c->prev_pc;

}
static void printCPU(int fd,cpu* processor){
	if(fd>=1){
	dprintf(fd,"\n-----------------------\n|--State of this cpu--|\n-----------------------\n");
	printCPURegs(fd,processor);
	dprintf(fd,"\nPC: %u\nPrev. PC: %u\n",processor->curr_pc,processor->prev_pc);
	}
	else{
	printw("\n-----------------------\n|--State of this cpu--|\n-----------------------\n");
	printCPURegs(fd,processor);
	printw("\nPC: %u\nPrev. PC: %u\n",processor->curr_pc,processor->prev_pc);
	}
}

static u_int32_t getMemoryWord(memory*mem,u_int32_t index){
	return *(u_int32_t*)(&mem->contents[index*WORD_SIZE]);

}
static void printMemory(int fd,memory* mem){
        if(fd>=1){
	dprintf(fd,"State of this memory:\nSize: %u\n",mem->size);
        for(u_int32_t i=0;i<mem->size/WORD_SIZE;i++){
        	dprintf(fd,"Line %u: [",i);
	        printWord(fd, getMemoryWord(mem,i));
		dprintf(fd,"]\n");
        }
	}
	else{
	printw("State of this memory:\nSize: %u\n",mem->size);
        for(u_int32_t i=0;i<mem->size/WORD_SIZE;i++){
        	printw("Line %u: [",i);
	        printWordNcurses( getMemoryWord(mem,i));
		printw("]\n");
        }
	}


}

static void printProc(int fd,context*c,cpu*proc){
	if(fd >=1){

	dprintf(fd,"\nProcesso com id: %d\n",c->proc_id);
	if(c){
	dprintf(fd,"Inicio de processo: %d\nInicio de codigo do processo: %d\nInicio de memoria reservda ao processo: %d\nFim do processo: %d\n",c->proc_data_start,c->proc_code_start,c->proc_allocd_start,c->proc_allocd_end);
	dprintf(fd,"Tamanho de processo: %d\nNumero de registos: %d\nEstado de registos:\n\n",c->proc_allocd_end-c->proc_data_start,proc->reg_file_size);
	for(u_int8_t i=0;i<proc->reg_file_size;i++){
        	dprintf(fd,"Reg %u: [",i);
	        printWord(fd,*(u_int32_t*)(&c->reg_state[i*WORD_SIZE]));
		dprintf(fd,"] Value: %u\n",*(u_int32_t*)(&c->reg_state[i*WORD_SIZE]));
        }
		dprintf(fd,"\nStatus word :[");
	        printWord(fd,c->status_word);
		dprintf(fd,"] Value: %u\n\n",c->status_word);
        	dprintf(fd,"Process stored PC: %u\n",c->curr_pc);
        	dprintf(fd,"Process stored Prev PC: %u\n",c->prev_pc);
        }
	else{
		dprintf(fd,"Processo nulo!\n");
	}
	dprintf(fd,"\nFim deste processo\n");
	}
	else{
	printw("\nProcesso com id: %d\n",c->proc_id);
	if(c){
	printw("Inicio de processo: %d\nInicio de codigo do processo: %d\nInicio de memoria reservda ao processo: %d\nFim do processo: %d\n",c->proc_data_start,c->proc_code_start,c->proc_allocd_start,c->proc_allocd_end);
	printw("Tamanho de processo: %d\nNumero de registos: %d\nEstado de registos:\n\n",c->proc_allocd_end-c->proc_data_start,proc->reg_file_size);
	for(u_int8_t i=0;i<proc->reg_file_size;i++){
        	printw("Reg %u: [",i);
	        printWordNcurses(*(u_int32_t*)(&c->reg_state[i*WORD_SIZE]));
		printw("] Value: %u\n",*(u_int32_t*)(&c->reg_state[i*WORD_SIZE]));
        }
		printw("\nStatus word :[");
	        printWordNcurses(c->status_word);
		printw("] Value: %u\n\n",c->status_word);
        	printw("Process stored PC: %u\n",c->curr_pc);
        	printw("Process stored Prev PC: %u\n",c->prev_pc);
        }
	else{
		printw("Processo nulo!\n");
	}
	printw("\nFim deste processo\n");
	}

}
static void printProcTable(int fd,p_table* procs,cpu*proc){
	if(fd>=1){
	dprintf(fd,"Tabela de processos:\nNumero de processos: %d\n",procs->num_of_processes);
	for(u_int32_t i=0;i<procs->num_of_processes;i++){
		printProc(fd,procs->processes[i],proc);

	}
	
	}
	else{
	printw("Tabela de processos:\nNumero de processos: %d\n",procs->num_of_processes);
	for(u_int32_t i=0;i<procs->num_of_processes;i++){
		printProc(fd,procs->processes[i],proc);

	}
	}

}
static void printOS(int fd,os* system){
	if(fd>=1){
	dprintf(fd,"\033[2J");
	dprintf(fd,"\n-----------------------\n|--State of this os--|\n-----------------------\n");
	printCPU(fd,system->proc);
	//printProcTable(fd,&(system->proc_vec),system->proc);
	
	}
	else{
	erase();
	printw("\n-----------------------\n|--State of this os--|\n-----------------------\n");
	printCPU(fd,system->proc);
	//printProcTable(fd,&(system->proc_vec),system->proc);
	refresh();
	}
}

static void menu(int fd,os*system){
		int pid=fork();
		switch(pid){
			case -1:
				endwin();
				perror("Erro no fork no menu!!!!\n");
				raise(SIGINT);
				break;
			case 0:
				//printMemory(fd,system->mem);
				printOS(fd,system);
				raise(SIGINT);
				break;
			default:
				break;
		
		}
				

}

static context* spawnCtx(cpu*proc,u_int32_t id,u_int32_t data_start,u_int32_t data_size,u_int32_t allocd_size){
	context* result=malloc(sizeof(context));
	result->proc_id=id;
	result->proc_data_start=data_start;
	result->proc_code_start=data_start+data_size;
	result->proc_allocd_start=0;
	result->proc_allocd_end=0;
	result->proc_data_size=data_size;
	result->proc_allocd_size=allocd_size;
	result->reg_state=malloc(WORD_SIZE*proc->reg_file_size);
	memset(result->reg_state,0,WORD_SIZE*proc->reg_file_size);
	result->status_word=0;
	result->curr_pc=0;
	result->prev_pc=0;
	return result;

}
static void addCtx(os* system,u_int32_t proc_start,u_int32_t data_size,u_int32_t allocd_size){
	system->proc_vec.num_of_processes++;
	for(u_int32_t i=0;i<system->proc_vec.num_of_processes;i++){
		if(!system->proc_vec.processes[i]){
			system->proc_vec.processes[i]=spawnCtx(system->proc,system->proc_vec.num_of_processes-1,proc_start,data_size,allocd_size);
			break;
		}
	}

}
static void endCtx(context**ctx){
	if(*ctx){
	if((*ctx)->reg_state){

		free((*ctx)->reg_state);

	}
	free((*ctx));
	*ctx=NULL;
	}

}
static int procs_intersect(u_int32_t start,u_int32_t proc_size,context*c2){
	u_int32_t proc2_size= c2->proc_allocd_end-c2->proc_allocd_start;
	u_int32_t result=1;
	if(proc_size>proc2_size){
		result&=((c2->proc_data_start>=start)||(c2->proc_allocd_end<=(start+proc_size)));
		
	}
	else {
		result&=((start>=c2->proc_data_start)||((start+proc_size)<=c2->proc_allocd_end));
		
	}
	
	return result;

}
static int32_t check_fit_memory_addr(os*system,u_int32_t proc_size){
	u_int32_t pos=0;
	for(u_int32_t i=0;i<system->proc_vec.num_of_processes;i++){
		context* cmp_ctx=system->proc_vec.processes[i];
			if(!procs_intersect(pos,proc_size,cmp_ctx)){
			break;
			}
			pos=cmp_ctx->proc_allocd_end+1;
			
	}
	printf("Pos: %d\n",pos);
	return pos;

}
void loadProg(FILE* progfile,os* system){

	u_int32_t spawned_start_addr=0;
	u_int32_t spawned_data_size=5;
	u_int32_t spawned_allocd_size=20;
	u_int32_t code_size=0;
	u_int32_t value=0;
	while((fscanf(progfile,"%x",&value)==1)){
		dprintf(1,"Li instruçao. code_size= %d\n",code_size);
		code_size++;
	}
	
	u_int32_t proc_size=code_size+spawned_data_size+spawned_allocd_size;
	if(system->avail_memory<proc_size){

		perror("OUT OF MEMORY!!!!\n");
		fclose(progfile);
		raise(SIGINT);

	}
	spawned_start_addr=check_fit_memory_addr(system,proc_size);
	rewind(progfile);
	addCtx(system,spawned_start_addr,spawned_data_size,spawned_allocd_size);
	context*ctx=system->proc_vec.processes[system->proc_vec.num_of_processes-1];
	memset(system->mem->contents+(ctx->proc_data_start*WORD_SIZE),0x0,(ctx->proc_data_size)*WORD_SIZE);
	ctx->proc_code_start=ctx->proc_data_start+ctx->proc_data_size;
	u_int32_t curr_cell=ctx->proc_code_start;
	curr_cell=ctx->proc_code_start;
	while((fscanf(progfile,"%x",&value)==1)&&(curr_cell<(ctx->proc_code_start+code_size))){
		dprintf(1,"Carreguei instruçao. curr_mem_cell= %d\n",curr_cell);
		storeValue(system->mem, curr_cell*WORD_SIZE, WORD_SIZE, (void*)&value);
		curr_cell++;

	}
	
	ctx->proc_allocd_start=curr_cell;
	memset(system->mem->contents+(ctx->proc_allocd_start*WORD_SIZE),0x0,(ctx->proc_allocd_size)*WORD_SIZE);
	ctx->proc_allocd_end=ctx->proc_allocd_start+ctx->proc_allocd_size;
	system->avail_memory-=proc_size;
	ctx->curr_pc=ctx->proc_code_start;
	rewind(progfile);
}
void switchOnCPU(int fd,os*system){
	if(system->proc_vec.num_of_processes){
	if(!(fd>=1)){
	initscr();
	start_color();
	timeout(10);
	curs_set(0);
	noecho();
	}
		
	context* prog=system->proc_vec.processes[0];
	system->proc->curr_pc=prog->curr_pc;
	while((system->proc->curr_pc>=prog->proc_code_start)&&(system->proc->curr_pc<(prog->proc_allocd_start))){
		
		
		u_int32_t value=0;
		loadValue(system->mem,(system->proc->curr_pc)*WORD_SIZE,(u_int32_t)sizeof(value),(void*) &value);
		
		menu(fd,system);
		execute(system->proc,value);
		copyCPUStateToContext(system->proc,prog);
		system->curr_process=((system->curr_process+1)%system->proc_vec.num_of_processes);
		prog=system->proc_vec.processes[system->curr_process];
		loadContextIntoCPU(prog,system->proc);
		usleep(10000);
	}
		if(!(fd>=1)){
		endwin();
		}
		printf("Finished!\n");
		raise(SIGINT);
	}
}
static void endMemory(memory** mem){
	if(*mem){
	if((*mem)->contents){
		free((*mem)->contents);
        	(*mem)->contents=NULL;
        }
		(*mem)->size=0;
        	free(*mem);
        	*mem=NULL;

	}

}


memory* spawnMemory(void){
        memory* result= malloc(sizeof(memory));
        result->contents=(u_int8_t*)malloc((result->size=MEM_DEF_SIZE));
        memset(result->contents,0,result->size);
        return result;
}
//does not free the mem field
static void endCPU(cpu** processor){
        if(*processor){
	if((*processor)->reg_file){
		free((*processor)->reg_file);
	}
	if((*processor)->wins){

		free((*processor)->wins);
        
	}
		free(*processor);
        	*processor=NULL;

	}

}

static cpu* spawnCPU(void){
        cpu* result= malloc(sizeof(cpu));
        result->curr_pc=0;
	result->prev_pc=0;
	result->status_word=0;
	result->reg_file=NULL;
	result->wins=NULL;
	FILE* fp=NULL;
	if(!(fp=fopen(CPU_FILE_PATH,"r"))){

		perror("NO CPU FILE FOUND!!!!!\n");
		endCPU(&result);
		return 0;

	}

	skip_cpu_comments(fp);
	
	if(!fscanf(fp,"%u",&result->reg_file_size)){
		fclose(fp);
		perror("INVALID INPUT AT CPU CONFIG FILE!!!!!\n");
		endCPU(&result);
		return 0;
	}
	skip_cpu_comments(fp);
	
	fclose(fp);
	result->reg_file= (u_int8_t*)malloc(result->reg_file_size*WORD_SIZE);
	memset(result->reg_file,0,result->reg_file_size*WORD_SIZE);
	if(!load_cpu_masks(&result->dec)){

		perror("INVALID INPUT AT DECODER CONFIG FILE!!!!!\n");
		endCPU(&result);
	}

	result->curr_wp=0;
	result->wins= malloc(sizeof(reg_window)*MAX_NUM_OF_WINDOWS);
	memset(result->wins,0,sizeof(reg_window)*MAX_NUM_OF_WINDOWS);
        return result;
}

os* spawnOS(void){
	os* result= malloc(sizeof(os));
	result->proc_vec.num_of_processes=0;
	result->curr_process=0;
	result->proc_vec.processes=malloc(sizeof(context*)*MAX_NUM_OF_PROCESSES);
	memset(result->proc_vec.processes,0,sizeof(context*)*MAX_NUM_OF_PROCESSES);
	result->mem=spawnMemory();
	result->avail_memory=result->mem->size/WORD_SIZE;
	result->proc=spawnCPU();
	result->proc->running_system=result;
	if(!result->proc){
		perror("ERRO A SPAWNAR OS!!!!\n");
		endOS(&result);
	}
	return result;

}
void endOS(os** system){
        if(*system){
	endMemory(&((*system)->mem));
        endCPU(&((*system)->proc));
	if((*system)->proc_vec.processes){
	for(u_int32_t i=0;i<((*system)->proc_vec.num_of_processes);i++){
		
		if((*system)->proc_vec.processes[i]){
			endCtx(&((*system)->proc_vec.processes[i]));
		}
	
	}
		free((*system)->proc_vec.processes);
		(*system)->proc_vec.processes=NULL;
	}
		free(*system);
		*system=NULL;
	}
	

}

