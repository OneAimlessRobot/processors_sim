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

u_int32_t getProcRegValue(cpu* proc,u_int32_t regIndex,u_int8_t reg_portion_mask){
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
static u_int8_t calculateAddr(u_int32_t*baseaddr,reg_type type,u_int8_t reg_addr){
u_int8_t word_size=WORD_SIZE;
   (*baseaddr)*=WORD_SIZE;
   u_int32_t base =*baseaddr;
   switch(type){
    case HALF:
	reg_addr&=1;
	word_size>>=1;
	if(reg_addr){
	base+=word_size;
	}
	break;
    case QUARTER:
	reg_addr&=0x00000003;
	word_size>>=2;
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
	(*baseaddr)=base;
	return word_size;
}
void storeValueReg(cpu* proc, u_int32_t base,reg_type type,u_int32_t value,u_int8_t reg_addr) {
 
u_int8_t word_size=calculateAddr(&base,type,reg_addr);
	memcpy(proc->reg_file + base, &value, word_size);
}
void loadMemValue(cpu* proc,u_int32_t base,u_int32_t basemem,reg_type type,u_int8_t reg_addr){
   u_int8_t word_size=calculateAddr(&base,type,reg_addr);
   calculateAddr(&basemem,type,reg_addr);
   memcpy(proc->reg_file + base,proc->mem->contents + basemem, word_size);

}
void storeMemValue(cpu* proc,u_int32_t base,u_int32_t basemem,reg_type type,u_int8_t reg_addr){
   u_int8_t word_size=calculateAddr(&base,type,reg_addr);
   calculateAddr(&basemem,type,reg_addr);
       memcpy(proc->mem->contents + (basemem),proc->reg_file + base, word_size);

}
static void load_imm(cpu*proc,u_int32_t inst){
	u_int8_t dest=0;
	u_int32_t value=inst&proc->load_imm_oper_mask;
	u_int32_t unprocessed_dest=inst&proc->load_imm_dst_mask;
	unprocessed_dest>>=firstBitOne(proc->load_imm_dst_mask);
	dest|=unprocessed_dest;
	storeValueReg(proc,dest,FULL,value,0);
}
void execute(cpu*proc,u_int32_t inst){
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
			dprintf(1,"Cmp!\n");
			cmp(proc,inst);
			break;
		case BZERO:
			dprintf(1,"bzero!\n");
			bz(proc,inst);
			break;
		case BNZERO:
			dprintf(1,"bnzero!\n");
			bnz(proc,inst);
			break;
		default:
			dprintf(1,"Desconhecida\n");
			break;


	}
	
	proc->curr_pc++;
	

}

int load_cpu_masks(cpu*proc){
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
