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
	
	base+=word_size;
	break;
    case QUARTER:
	reg_addr&=0x00000003;
	word_size>>=2;
	for(u_int8_t i=0;i<reg_addr;i++){

		base+=word_size;
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
	u_int32_t value=inst&proc->dec.load_imm_oper_mask;
	u_int32_t unprocessed_dest=inst&proc->dec.load_imm_dst_mask;
	unprocessed_dest>>=firstBitOne(proc->dec.load_imm_dst_mask);
	dest|=unprocessed_dest;
	storeValueReg(proc,dest,FULL,value,0);
}
void execute(cpu*proc,u_int32_t inst){
	op_code code=inst&proc->dec.op_code_mask;
	instr_type type=get_instr_type(code);
	switch(type){
		case ALU:
		proc_alu_op(proc,code,inst);
		break;
		case MEM:
		proc_mem_op(proc,code,inst);
		break;
		case IMM:
		load_imm(proc,inst);
		break;
		case COND:
		proc_cond_op(proc,code,inst);
		break;
		case CONTROL:
		proc_cond_op(proc,code,inst);
		break;
		default:
		break;

	}
	
	proc->curr_pc++;
	

}
