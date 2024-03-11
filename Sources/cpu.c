#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include "../Includes/defaults.h"
#include "../Includes/helper.h"
#include "../Includes/memory.h"
#include "../Includes/cpu.h"
#include "../Includes/alu.h"
#include "../Includes/cond_ops.h"
#include "../Includes/mem_ops.h"

u_int32_t getProcRegValue(cpu* proc,u_int8_t regIndex,u_int8_t reg_portion_mask){
	u_int32_t half_word_mask=0x0000FFFF;
	u_int32_t short_word_mask=0x000000FF;
	u_int32_t rawvalue=*(u_int32_t*)(&proc->reg_file[regIndex*WORD_SIZE]);
	if(reg_portion_mask<3&&reg_portion_mask){
		switch(reg_portion_mask&1){
			case 1:
			rawvalue&=short_word_mask;
			break;
			case 0:
			rawvalue&=half_word_mask;
			break;
			default:
			break;
		}
	}
	return rawvalue;

}
void loadProg(cpu* proc){
	u_int32_t value;
	u_int32_t curr_cell=0;
	proc->process.proc_start|=0;
	loadValue(proc->mem,proc->process.proc_start*WORD_SIZE,WORD_SIZE,(void*)&value);
	curr_cell++;
	while((curr_cell<(proc->mem->size/WORD_SIZE))){
		loadValue(proc->mem,curr_cell*WORD_SIZE,WORD_SIZE,(void*)&value);
		curr_cell++;
		if(!value){
			break;
		}
	}
	proc->process.proc_end|=curr_cell;


}

void storeValueReg(cpu* proc, u_int8_t base,reg_type type,u_int32_t value,u_int8_t reg_addr) {
    u_int8_t word_size=WORD_SIZE;
   base*=WORD_SIZE;
   switch(type){
    case HALF:
	reg_addr&=1;
	word_size<<=1;
	if(reg_addr){
	base+=word_size;
	}
	break;
    case QUARTER:
	reg_addr&=0x00000003;
	switch(reg_addr){
	case 1:
		base+=word_size;
		break;
	
	case 2:
		base+=word_size+word_size;
		break;
	case 3:
		base+=word_size+word_size+word_size;
		break;
	default:
		break;
	}
	break;
    default:
	break;

}
	memcpy(proc->reg_file + base, &value, word_size);
}
void loadMemValue(cpu* proc,u_int8_t base,u_int32_t basemem,reg_type type,u_int8_t reg_addr){
   u_int8_t word_size=WORD_SIZE;
   base*=word_size;
   switch(type){
    case HALF:
	reg_addr&=1;
	word_size<<=1;
	if(reg_addr){
	base+=word_size;
	}
	break;
    case QUARTER:
	reg_addr&=0x00000003;
	word_size<<=2;
	switch(reg_addr){
	case 1:
		base+=word_size;
		break;
	
	case 2:
		base+=word_size+word_size;
		break;
	case 3:
		base+=word_size+word_size+word_size;
		break;
	default:
		break;
	}
	break;
    default:
	
	break;

}
 	dprintf(1,"Endereço fonte: %u\nEndereço registo alvo: %u\n",(basemem*word_size),base);
       memcpy(proc->reg_file + base,proc->mem->contents + (basemem*word_size), word_size);

}
static void load_imm(cpu*proc,u_int32_t inst){
	u_int8_t dest=0;
	u_int32_t value=inst&proc->load_imm_oper_mask;
	u_int32_t unprocessed_dest=inst&proc->load_imm_dst_mask;
	unprocessed_dest>>=firstBitOne(proc->load_imm_dst_mask);
	dest|=unprocessed_dest;
	storeValueReg(proc,dest,FULL,value,0);
}
static void execute(cpu*proc,u_int32_t inst){
	op_code code=inst&proc->op_code_mask;
	switch(code){
		case ADD:
			dprintf(1,"Soma\n");
			add(proc,inst);
			break;
		case LMEM:
			dprintf(1,"Move\n");
			load(proc,inst);
			break;
		case SUB:
			dprintf(1,"Sub\n");
			sub(proc,inst);
			break;
		case STO:
			dprintf(1,"Store\n");
			sto(proc,inst);
			break;
		case LIMM:
			dprintf(1,"Load imediato\n");
			load_imm(proc,inst);
			break;
		case OR:
			dprintf(1,"Or logico\n");
			or(proc,inst);
			break;
		case AND:
			dprintf(1,"And logico\n");
			and(proc,inst);
			break;
		case JMP:
			dprintf(1,"Jump!\n");
			jmp(proc,inst);
			break;
		case RET:
			dprintf(1,"Ret!\n");
			ret(proc,inst);
			break;
		case CMP:
			dprintf(1,"Ret!\n");
			cmp(proc,inst);
			break;
		case BZERO:
			dprintf(1,"Ret!\n");
			bz(proc,inst);
			break;
		case BNZERO:
			dprintf(1,"Ret!\n");
			bnz(proc,inst);
			break;
		default:
			dprintf(1,"Desconhecida\n");
			break;


	}
	proc->curr_pc++;


}

void switchOnCPU(cpu*proc){
	proc->curr_pc=proc->process.proc_start;
	while((proc->curr_pc<proc->process.proc_end)){
		u_int32_t value=0;
		loadValue(proc->mem,proc->curr_pc*WORD_SIZE,(u_int32_t)sizeof(value),(void*) &value);
		execute(proc,value);
		dprintf(1,"\e[2J");
		printCPU(1,proc);
		dprintf(1,"Press enter!\n");
		getchar();
	}

}
static int load_cpu_masks(cpu*proc){
FILE* fp=NULL;
	if(!(fp=fopen(CPU_FILE_PATH,"r"))){

		perror("NO CPU FILE FOUND!!!!!\n");
		return 0;

	}

	skip_cpu_comments(fp);
	if(!fscanf(fp,"%u",&proc->reg_file_size)){
		fclose(fp);
		return 0;
	}
	skip_cpu_comments(fp);
	skip_cpu_comments(fp);
	if(!fscanf(fp,"%x",&proc->op_code_mask)){
		fclose(fp);
		return 0;
	}
	
	
	skip_cpu_comments(fp);
	if(!fscanf(fp,"%x",&proc->alu_oper_1_mask)){
		fclose(fp);
		return 0;
	}
	dprintf(1,"\n");
	printWord(1,proc->alu_oper_1_mask);
	if(!fscanf(fp,"%x",&proc->alu_oper_2_mask)){
		fclose(fp);
		return 0;
	}
	
	dprintf(1,"\n");
	printWord(1,proc->alu_oper_2_mask);
	if(!fscanf(fp,"%x",&proc->alu_dst_mask)){
		fclose(fp);
		return 0;
	}
	
	dprintf(1,"\n");
	printWord(1,proc->alu_dst_mask);
	if(!fscanf(fp,"%x",&proc->alu_op_size_mask)){
		fclose(fp);
		return 0;
	}
	
	dprintf(1,"\n");
	printWord(1,proc->alu_op_size_mask);

	if(!fscanf(fp,"%x",&proc->alu_op2_size_mask)){
		fclose(fp);
		return 0;
	}
	
	dprintf(1,"\n");
	printWord(1,proc->alu_op2_size_mask);

	if(!fscanf(fp,"%x",&proc->alu_dest_size_mask)){
		fclose(fp);
		return 0;
	}
	
	dprintf(1,"\n");
	printWord(1,proc->alu_dest_size_mask);


	
	skip_cpu_comments(fp);
	if(!fscanf(fp,"%x",&proc->load_imm_dst_mask)){
		fclose(fp);
		return 0;
	}
	
	dprintf(1,"\n");
	printWord(1,proc->load_imm_dst_mask);
	if(!fscanf(fp,"%x",&proc->load_imm_oper_mask)){
		fclose(fp);
		return 0;
	}

	dprintf(1,"\n");
	printWord(1,proc->load_imm_oper_mask);
	
	skip_cpu_comments(fp);
	if(!fscanf(fp,"%x",&proc->mem_reg_mask)){
		fclose(fp);
		return 0;
	}
	dprintf(1,"\n");
	printWord(1,proc->mem_reg_mask);
	if(!fscanf(fp,"%x",&proc->mem_addr_reg_mask)){
		fclose(fp);
		return 0;
	}
	dprintf(1,"\n");
	printWord(1,proc->mem_addr_reg_mask);
	if(!fscanf(fp,"%x",&proc->mem_reg_type_mask)){
		fclose(fp);
		return 0;
	}
	dprintf(1,"\n");
	printWord(1,proc->mem_reg_type_mask);
	if(!fscanf(fp,"%x",&proc->mem_size_mask)){
		fclose(fp);
		return 0;
	}
	dprintf(1,"\n");
	printWord(1,proc->mem_size_mask);
	skip_cpu_comments(fp);
	if(!fscanf(fp,"%x",&proc->jmp_addr_mask)){
		fclose(fp);
		return 0;
	}
	dprintf(1,"\n");
	printWord(1,proc->jmp_addr_mask);
	skip_cpu_comments(fp);
	if(!fscanf(fp,"%x",&proc->cmp_reg_mask)){
		fclose(fp);
		return 0;
	}
	dprintf(1,"\n");
	printWord(1,proc->cmp_reg_mask);
	if(!fscanf(fp,"%x",&proc->cmp_value_mask)){
		fclose(fp);
		return 0;
	}
	dprintf(1,"\n");
	printWord(1,proc->cmp_value_mask);
	dprintf(1,"\n");
	fclose(fp);
	return 1;

}
cpu* spawnCPU(memory*mem){
        cpu* result= malloc(sizeof(cpu));
        result->mem=mem;
	result->process.proc_start=0;
	result->process.proc_end=0;
        result->curr_pc=0;
	result->prev_pc=0;
	result->bz_flag=0;
	if(!load_cpu_masks(result)){

		result->mem=NULL;
		endCPU(&result);
	}
	result->reg_file= (u_int8_t*)malloc(result->reg_file_size);
        memset(result->reg_file,0,result->reg_file_size);
        return result;
}
//does not free mem
void endCPU(cpu** processor){
        free((*processor)->reg_file);
        free(*processor);
        *processor=NULL;

}
static void printCPURegs(int fd,cpu* processor){

       dprintf(fd,"State of the registers:\nSize: %u\n",processor->reg_file_size);
        for(u_int8_t i=0;i<processor->reg_file_size/WORD_SIZE;i++){
        	dprintf(fd,"Reg %u: Value: %u [",i,getProcRegValue(processor,i,FULL));
	        printWord(fd, getProcRegValue(processor,i,FULL));
		dprintf(fd,"]\n");
        }


}
void printCPU(int fd,cpu* processor){
	dprintf(fd,"\n-----------------------\n|--State of this cpu--|\n-----------------------\n");
	printCPURegs(fd,processor);
	dprintf(fd,"\nPC: %u\nPrev. PC: %u\nZero flag: %u\ncontext start: %u\ncontext end: %u\n",processor->curr_pc,processor->prev_pc,processor->bz_flag,processor->process.proc_start,processor->process.proc_end);
}

