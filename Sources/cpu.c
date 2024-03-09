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


u_int32_t getProcRegValue(cpu* proc,u_int8_t regIndex){

	return *(u_int32_t*)(&proc->reg_file[regIndex*WORD_SIZE]);

}
void storeValueReg(cpu* proc, u_int8_t base,reg_type type,u_int32_t value,u_int8_t reg_addr) {
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
	reg_addr&=0x00000007;
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
	reg_addr&=0x00000007;
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
		case LAB:
			dprintf(1,"Ret!\n");
			label(proc,inst);
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
	u_int32_t program_init=0;
	loadValue(proc->mem,0,(u_int32_t)sizeof(program_init),(void*) &program_init);
	while((proc->curr_pc<program_init)){
		u_int32_t value=0;
		loadValue(proc->mem,proc->curr_pc*WORD_SIZE,(u_int32_t)sizeof(value),(void*) &value);
		execute(proc,value);
		dprintf(1,"\e[2J");
		printCPU(1,proc);
		dprintf(1,"Press enter!\n");
		
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
	if(!fscanf(fp,"%x",&proc->alu_oper_2_mask)){
		fclose(fp);
		return 0;
	}
	if(!fscanf(fp,"%x",&proc->alu_dst_mask)){
		fclose(fp);
		return 0;
	}
	if(!fscanf(fp,"%x",&proc->alu_op_size_mask)){
		fclose(fp);
		return 0;
	}


	
	skip_cpu_comments(fp);
	if(!fscanf(fp,"%x",&proc->load_imm_dst_mask)){
		fclose(fp);
		return 0;
	}
	if(!fscanf(fp,"%x",&proc->load_imm_oper_mask)){
		fclose(fp);
		return 0;
	}

	
	skip_cpu_comments(fp);
	if(!fscanf(fp,"%x",&proc->mem_reg_mask)){
		fclose(fp);
		return 0;
	}
	if(!fscanf(fp,"%x",&proc->mem_addr_reg_mask)){
		fclose(fp);
		return 0;
	}
	if(!fscanf(fp,"%x",&proc->mem_reg_type_mask)){
		fclose(fp);
		return 0;
	}
	if(!fscanf(fp,"%x",&proc->mem_size_mask)){
		fclose(fp);
		return 0;
	}
	skip_cpu_comments(fp);
	if(!fscanf(fp,"%x",&proc->jmp_addr_mask)){
		fclose(fp);
		return 0;
	}
	skip_cpu_comments(fp);
	if(!fscanf(fp,"%x",&proc->cmp_reg_mask)){
		fclose(fp);
		return 0;
	}
	if(!fscanf(fp,"%x",&proc->cmp_value_mask)){
		fclose(fp);
		return 0;
	}
	fclose(fp);
	return 1;

}
cpu* spawnCPU(memory*mem){
        cpu* result= malloc(sizeof(cpu));
        result->mem=mem;
        result->curr_pc=0;
	result->prev_pc=0;
	if(!load_cpu_masks(result)){

		result->mem=NULL;
		endCPU(&result);
	}
	result->reg_file= (u_int8_t*)malloc(result->reg_file_size=REG_FILE_DEF_SIZE);
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
        	dprintf(fd,"Reg %u: [",i);
	        printWord(fd, getProcRegValue(processor,i));
		dprintf(fd,"]\n");
        }


}
void printCPU(int fd,cpu* processor){
	dprintf(fd,"\n-----------------------\n|--State of this cpu--|\n-----------------------\n");
	printCPURegs(fd,processor);
	dprintf(fd,"\nPC: %u\nPrev. PC: %u\nZero flag: %u\n",processor->curr_pc,processor->prev_pc,processor->bz_flag);
}

