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

static struct timeval tv={TIMEOUT_IN,TIMEOUT_IN_US};
fd_set filedescs;
static int print_os=1, print_memory=0,print_cpu=1,print_proc_table=0;
static void printCPURegs(int fd,cpu* processor){
	if(fd>=1){
	
       dprintf(fd,"State of the registers:\nSize: %u\n",processor->reg_file_size);
        for(u_int8_t i=0;i<processor->reg_file_size;i++){
        	dprintf(fd,"Reg %u: [",i);
	        printWord(fd,getProcRegValue(processor,i,0));
		dprintf(fd,"] Value: %d\n",(int32_t)getProcRegValue(processor,i,0));
        }
		dprintf(fd,"\nStatus word :[");
	        printWord(fd, processor->status_word);
		dprintf(fd,"] Value: %x\n",processor->status_word);
	}

	else{
       printw("State of the registers:\nSize: %u\n",processor->reg_file_size);
        for(u_int8_t i=0;i<processor->reg_file_size;i++){
        	printw("Reg %u: [",i);
	        printWordNcurses(getProcRegValue(processor,i,0));
		printw("] Value: %d\n",(int32_t)getProcRegValue(processor,i,0));
        }
		printw("\nStatus word :[");
	        printWordNcurses( processor->status_word);
		printw("] Value: %x\n",processor->status_word);
	}

}
static void copyCPUStateToContext(cpu* proc,context* c){
	u_int32_t reg_file_size=proc->reg_file_size;
	memcpy(c->reg_state,proc->reg_file,reg_file_size*WORD_SIZE);
	c->status_word=proc->status_word;
	c->curr_pc=proc->curr_pc;
	c->prev_pc=proc->prev_pc;
       	c->instr_reg=proc->instr_reg;
        c->stack_pointer=proc->stack_pointer;
        c->frame_pointer=proc->frame_pointer;

}
static void loadContextIntoCPU(context* c,cpu*proc){
	memcpy(proc->reg_file,c->reg_state,proc->reg_file_size*WORD_SIZE);
	proc->status_word=c->status_word;
	proc->curr_pc=c->curr_pc;
	proc->prev_pc=c->prev_pc;
       	proc->instr_reg=c->instr_reg;
        proc->stack_pointer=c->stack_pointer;
        proc->frame_pointer=c->frame_pointer;


}
static void printCPU(int fd,cpu* processor){
	if(fd>=1){
	dprintf(fd,"\n-----------------------\n|--State of this cpu--|\n-----------------------\n");
	printCPURegs(fd,processor);
	dprintf(fd,"\nPC: %u\nPrev. PC: %u\n",processor->curr_pc,processor->prev_pc);
	dprintf(fd,"\nIR: %x\n",processor->instr_reg);
	dprintf(fd,"\nSP: %d\n",processor->stack_pointer);
	dprintf(fd,"\nFP: %d\n",processor->frame_pointer);
	}
	else{
	printw("\n-----------------------\n|--State of this cpu--|\n-----------------------\n");
	printCPURegs(fd,processor);
	printw("\nPC: %u\nPrev. PC: %u\n",processor->curr_pc,processor->prev_pc);
	printw("\nIR: %x\n",processor->instr_reg);
	printw("\nSP: %d\n",processor->stack_pointer);
	printw("\nFP: %d\n",processor->frame_pointer);
	}
}

static u_int32_t getMemoryWord(memory*mem,u_int32_t index){
	return *(u_int32_t*)(&mem->contents[index*WORD_SIZE]);

}
void printMemory(int fd,memory* mem){
        if(fd>=1){
	dprintf(fd,"State of this memory:\nSize: %u\n",mem->size);
        for(u_int32_t i=0;i<mem->size/WORD_SIZE;i++){
        	dprintf(fd,"Line %u: [",i);
	        printWord(fd, getMemoryWord(mem,i));
		dprintf(fd,"] Value: %u\n",getMemoryWord(mem,i));
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
		dprintf(fd,"] Value: %d\n",(int32_t)*(int32_t*)(&c->reg_state[i*WORD_SIZE]));
        }
		dprintf(fd,"\nStatus word :[");
	        printWord(fd,c->status_word);
		dprintf(fd,"] Value: %x\n\n",c->status_word);
        	dprintf(fd,"Process stored PC: %u\n",c->curr_pc);
        	dprintf(fd,"Process stored Prev PC: %u\n",c->prev_pc);
        	dprintf(fd,"\nIR: %x\n",c->instr_reg);
		dprintf(fd,"\nSP: %d\n",c->stack_pointer);
		dprintf(fd,"\nFP: %d\n",c->frame_pointer);
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
		printw("] Value: %d\n",(int32_t)*(u_int32_t*)(&c->reg_state[i*WORD_SIZE]));
        }
		printw("\nStatus word :[");
	        printWordNcurses(c->status_word);
		printw("] Value: %u\n\n",c->status_word);
        	printw("Process stored PC: %u\n",c->curr_pc);
        	printw("Process stored Prev PC: %u\n",c->prev_pc);
        	printw("\nIR: %x\n",c->instr_reg);
		printw("\nSP: %d\n",c->stack_pointer);
		printw("\nFP: %d\n",c->frame_pointer);
	
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
	if(print_os){
	//dprintf(fd,"\033[2J");
	dprintf(fd,"\n-----------------------\n|--State of this os--|\n-----------------------\n");
	if(print_cpu){
	printCPU(fd,system->proc);
	}
	if(print_proc_table){
	printProcTable(fd,&(system->proc_vec),system->proc);
	}
	if(print_memory){

	printMemory(fd,system->mem);
	}
	}
	}
	else{
	if(print_os){
	//clear();
	erase();
	printw("\n-----------------------\n|--State of this os--|\n-----------------------\n");
	if(print_cpu){
	printCPU(fd,system->proc);
	}
	if(print_proc_table){
	printProcTable(fd,&(system->proc_vec),system->proc);
	}
	if(print_memory){

	printMemory(fd,system->mem);
	}
	refresh();
	}
	}
}

static void printThings(int fd,os*system){
		/*int pid=fork();
		switch(pid){
			case -1:
				endwin();
				perror("Erro no fork no menu!!!!\n");
				raise(SIGPIPE);
				break;
			case 0:
				raise(SIGPIPE);
				break;
			default:
				break;
		
		}*/
			printOS(fd,system);
				

}

void menu(int fd){
		int c;
		int result;
		if(fd>=1){
		FD_ZERO(&filedescs);
                FD_SET(0,&filedescs);
		result=select(1,&filedescs,NULL,NULL,&tv);
		if(result>0){
		read(0,(char*)&c,1);
		}
		}
		else{
		c=(int )getch();
		}
		switch(c){
			case 's':
				print_os=((print_os+1)%2);
				break;
			case 'p':
				print_proc_table=((print_proc_table+1)%2);
				break;
			case 'm':
				
				print_memory=((print_memory+1)%2);
				break;
			case 'c':
				
				print_cpu=((print_cpu+1)%2);
				break;
			default:
				break;
		
		}
		if(fd>=1&&result<=0){

			//dprintf(1,"Timeout!!!\n");
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
	result->instr_reg=0;
	result->stack_pointer=0;
	result->frame_pointer=0;
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
	u_int32_t spawned_data_size=0;
	u_int32_t spawned_allocd_size=300;
	u_int32_t code_size=0;
	u_int32_t value=0;
	u_int32_t curr_data=0;
	fscanf(progfile,"%x",&spawned_data_size);
	while(curr_data<spawned_data_size){
		fscanf(progfile,"%x",&value);
		dprintf(1,"Carreguei dados. curr_data_cell= %d\n",curr_data);
		storeValue(system->mem, curr_data*WORD_SIZE, WORD_SIZE, (void*)&value);
		curr_data++;
	}
	u_int32_t code_pos=ftell(progfile);
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
	fseek(progfile,code_pos,SEEK_SET);
	addCtx(system,spawned_start_addr,spawned_data_size,spawned_allocd_size);
	context*ctx=system->proc_vec.processes[system->proc_vec.num_of_processes-1];
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
	ctx->frame_pointer=ctx->proc_allocd_size;
	ctx->stack_pointer=ctx->frame_pointer-1;
	rewind(progfile);
}
void switchOnCPU(int fd,os*system){
	if(!(fd>=1)){
	initscr();
	start_color();
	timeout(0);
	curs_set(0);
	noecho();
	}
	if(system->proc_vec.num_of_processes){
	
		
	context* prog=system->proc_vec.processes[0];
	system->proc->curr_pc=prog->curr_pc;
	system->proc->stack_pointer=prog->stack_pointer;
	system->proc->frame_pointer=prog->frame_pointer;
	while((system->proc->curr_pc>=prog->proc_code_start)&&(system->proc->curr_pc<(prog->proc_allocd_start))){
		
		
		loadValue(system->mem,(system->proc->curr_pc)*WORD_SIZE,(u_int32_t)sizeof(system->proc->instr_reg),(void*) &system->proc->instr_reg);
		
		menu(fd);
		execute(system->proc);
		//printThings(fd,system);
		copyCPUStateToContext(system->proc,prog);
		system->curr_process=((system->curr_process+1)%system->proc_vec.num_of_processes);
		prog=system->proc_vec.processes[system->curr_process];
		loadContextIntoCPU(prog,system->proc);
		usleep(100);
	}
		//printThings(fd,system);
		usleep(100);
		getc(stdin);
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
	if((*processor)->dec2){

		freeDecoder(&(*processor)->dec2);
		(*processor)->dec2=NULL;
	}
	}
		free(*processor);
        	*processor=NULL;
	

}

static cpu* spawnCPU(void){
        cpu* result= malloc(sizeof(cpu));
        result->curr_pc=0;
	result->prev_pc=0;
	result->instr_reg=0;
        result->stack_pointer=0;
	result->frame_pointer=0;
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
	result->dec2=malloc(sizeof(decoder));
	if(!load_cpu_masks(result->dec2)){

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

