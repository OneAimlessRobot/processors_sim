#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include "../Includes/defaults.h"
#include "../Includes/helper.h"
#include "../Includes/memory.h"
#include "../Includes/decoder.h"
#include "../Includes/cpu.h"
#include "../Includes/os.h"

context* spawnCtx(cpu*proc,u_int32_t id,u_int32_t data_start,u_int32_t data_size,u_int32_t allocd_size){
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
	return result;

}
void endCtx(context**ctx){
	free((*ctx)->reg_state);
	free((*ctx));
	*ctx=NULL;
	

}
void loadProg(FILE* progfile,cpu* proc,context* ctx){
	u_int32_t value;
	memset(proc->mem->contents+(ctx->proc_data_start*WORD_SIZE),0xFF,(ctx->proc_data_size)*WORD_SIZE);
	u_int32_t curr_cell=ctx->proc_code_start;
	while((fscanf(progfile,"%x",&value)==1)&&(curr_cell<(proc->mem->size/WORD_SIZE))){
		curr_cell++;
	}
	ctx->proc_allocd_start=curr_cell;
	curr_cell=ctx->proc_code_start;
	rewind(progfile);
	while((fscanf(progfile,"%x",&value)==1)&&(curr_cell<(ctx->proc_allocd_start))){
		storeValue(proc->mem, curr_cell*WORD_SIZE, WORD_SIZE, (void*)&value);
		curr_cell++;

	}
	memset(proc->mem->contents+(ctx->proc_allocd_start*WORD_SIZE),0xFF,(ctx->proc_allocd_size)*WORD_SIZE);
	ctx->proc_allocd_end=ctx->proc_allocd_start+ctx->proc_allocd_size;

}
void switchOnCPU(cpu*proc,context* prog){
	proc->curr_pc=prog->proc_code_start;
	printCPU(1,proc,prog);
		
	while((proc->curr_pc>=prog->proc_code_start)&&(proc->curr_pc<(prog->proc_allocd_start))){
		u_int32_t value=0;
		loadValue(proc->mem,(proc->curr_pc)*WORD_SIZE,(u_int32_t)sizeof(value),(void*) &value);
		printMemory(1,proc->mem);
		printCPU(1,proc,prog);
		execute(proc,value);
		dprintf(1,"Press enter!\n");
		getchar();
	}

}
cpu* spawnCPU(memory*mem){
        cpu* result= malloc(sizeof(cpu));
        result->mem=mem;
        result->curr_pc=0;
	result->prev_pc=0;
	result->status_word=0;
	result->reg_file=NULL;
	result->wins=NULL;
	FILE* fp=NULL;
	if(!(fp=fopen(CPU_FILE_PATH,"r"))){

		perror("NO CPU FILE FOUND!!!!!\n");
		result->mem=NULL;
		endCPU(&result);
		return 0;

	}

	skip_cpu_comments(fp);
	
	if(!fscanf(fp,"%u",&result->reg_file_size)){
		fclose(fp);
		perror("INVALID INPUT AT CPU CONFIG FILE!!!!!\n");
		result->mem=NULL;
		endCPU(&result);
		return 0;
	}
	skip_cpu_comments(fp);
	
	fclose(fp);
	result->reg_file= (u_int8_t*)malloc(result->reg_file_size*WORD_SIZE);
	memset(result->reg_file,0,result->reg_file_size*WORD_SIZE);
	if(!load_cpu_masks(&result->dec)){

		perror("INVALID INPUT AT DECODER CONFIG FILE!!!!!\n");
		result->mem=NULL;
		endCPU(&result);
	}

	result->curr_wp=0;
	result->wins= malloc(sizeof(reg_window)*MAX_NUM_OF_WINDOWS);
	memset(result->wins,0,sizeof(reg_window)*MAX_NUM_OF_WINDOWS);
        return result;
}
//does not free the mem field
void endCPU(cpu** processor){
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
static void printCPURegs(int fd,cpu* processor){

       dprintf(fd,"State of the registers:\nSize: %u\n",processor->reg_file_size);
        for(u_int8_t i=0;i<processor->reg_file_size;i++){
        	dprintf(fd,"Reg %u: [",i);
	        printWord(fd, getProcRegValue(processor,i,FULL));
		dprintf(fd,"] Value: %u\n",getProcRegValue(processor,i,FULL));
        }
		dprintf(fd,"\nStatus word :[");
	        printWord(fd, processor->status_word);
		dprintf(fd,"] Value: %u\n",processor->status_word);
        

}
void printCPU(int fd,cpu* processor,context* c){
	dprintf(fd,"\n-----------------------\n|--State of this cpu--|\n-----------------------\n");
	printCPURegs(fd,processor);
	dprintf(fd,"\nPC: %u\nPrev. PC: %u\ncontext start: %u\ncontext end: %u\ncontext real start: %u\ncontext real end %u\n",processor->curr_pc,processor->prev_pc,c->proc_code_start,c->proc_allocd_start,c->proc_data_start,c->proc_allocd_end);
}



memory* spawnMemory(void){
        memory* result= malloc(sizeof(memory));
        result->contents=(u_int8_t*)malloc((result->size=MEM_DEF_SIZE));
        memset(result->contents,0,result->size);
        return result;
}
void endMemory(memory** mem){

        free((*mem)->contents);
        (*mem)->contents=NULL;
        (*mem)->size=0;
        free(*mem);
        *mem=NULL;


}

static u_int32_t getMemoryWord(memory*mem,u_int32_t index){
	return *(u_int32_t*)(&mem->contents[index*WORD_SIZE]);

}
void printMemory(int fd,memory* mem){
        dprintf(fd,"State of this memory:\nSize: %u\n",mem->size);
        for(u_int32_t i=0;i<mem->size/WORD_SIZE;i++){
        	dprintf(fd,"Line %u: [",i);
	        printWord(fd, getMemoryWord(mem,i));
		dprintf(fd,"]\n");
        }


}
