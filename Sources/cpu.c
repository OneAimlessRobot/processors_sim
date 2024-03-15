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
#include "../Includes/alu.h"
#include "../Includes/cond_ops.h"
#include "../Includes/mem_ops.h"

int32_t getProcRegValue(cpu* proc,u_int32_t regIndex,u_int8_t reg_portion_mask){
	u_int8_t word_size=(WORD_SIZE>>reg_portion_mask);
	int32_t rawvalue=*(int32_t*)(&proc->reg_file[regIndex*WORD_SIZE]);
	u_int32_t mask=0xFFFFFFFF;
	mask>>=((BYTE_BITS*WORD_SIZE)-(BYTE_BITS*word_size));
	return rawvalue&mask;

}
static u_int8_t calculateAddr(u_int32_t*baseaddr,reg_type type,u_int8_t reg_addr){
u_int8_t word_size=(WORD_SIZE>>type);
   (*baseaddr)*=WORD_SIZE;
   int32_t base =*baseaddr;
	switch(type){
    case HALF:
	reg_addr&=1;
	break;
    case QUARTER:
	reg_addr&=3;
	break;
    default:
	
	break;

}
	for(u_int8_t i=0;i<reg_addr;i++){
		base+=word_size;
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
   memcpy(proc->reg_file + base,proc->running_system->mem->contents + basemem, word_size);

}
void storeMemValue(cpu* proc,u_int32_t base,u_int32_t basemem,reg_type type,u_int8_t reg_addr){
   u_int8_t word_size=calculateAddr(&base,type,reg_addr);
   calculateAddr(&basemem,type,reg_addr);
       memcpy(proc->running_system->mem->contents + (basemem),proc->reg_file + base, word_size);

}
void pushValue(cpu* proc,u_int32_t base,reg_type type,u_int8_t reg_addr){
   u_int8_t word_size=calculateAddr(&base,type,reg_addr);
    u_int32_t true_sp=proc->stack_pointer;
   calculateAddr(&proc->stack_pointer,type,reg_addr);
   memcpy(proc->running_system->mem->contents + proc->stack_pointer,proc->reg_file + base,word_size);
   proc->stack_pointer=true_sp;
}
void popValue(cpu* proc,u_int32_t base,reg_type type,u_int8_t reg_addr){
   u_int8_t word_size=calculateAddr(&base,type,reg_addr);
   u_int32_t true_sp=proc->stack_pointer;
   calculateAddr(&proc->stack_pointer,type,reg_addr);
       memcpy(proc->reg_file + base,proc->running_system->mem->contents + proc->stack_pointer, word_size);
   proc->stack_pointer=true_sp;
}
static void load_imm(cpu*proc){
	u_int8_t dest=0;
	int16_t value=proc->instr_reg&proc->dec2->load_imm_oper_mask;
	u_int32_t unprocessed_dest=proc->instr_reg&proc->dec2->load_imm_dst_mask;
	unprocessed_dest>>=firstBitOne(proc->dec2->load_imm_dst_mask);
	dest|=unprocessed_dest;
	storeValueReg(proc,dest,HALF,value,0);
}
void execute(cpu*proc){
	op_code code=proc->instr_reg&proc->dec2->op_code_mask;
	instr_type type=get_instr_type(code);
	u_int32_t init_pc=proc->curr_pc;
	switch(type){
		case ALU:
		proc_alu_op(proc,code);
		break;
		case STACK:
		proc_mem_op(proc,code);
		break;
		case MEM:
		proc_mem_op(proc,code);
		break;
		case IMM:
		load_imm(proc);
		break;
		case COND:
		proc_cond_op(proc,code);
		break;
		case CONTROL:
		proc_cond_op(proc,code);
		break;
		default:
		break;

	}
	u_int32_t end_pc= proc->curr_pc;
	if(init_pc==end_pc){
	proc->curr_pc++;
	}
	

}
