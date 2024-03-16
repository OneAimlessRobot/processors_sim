#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
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
   context* prog=proc->running_system->proc_vec.processes[proc->running_system->curr_process];
   memcpy(proc->reg_file + base,proc->running_system->mem->contents + prog->proc_data_start+basemem, word_size);

}
void storeMemValue(cpu* proc,u_int32_t base,u_int32_t basemem,reg_type type,u_int8_t reg_addr){
   u_int8_t word_size=calculateAddr(&base,type,reg_addr);
   calculateAddr(&basemem,type,reg_addr);
   context* prog=proc->running_system->proc_vec.processes[proc->running_system->curr_process];
   memcpy(proc->running_system->mem->contents + prog->proc_data_start+basemem,proc->reg_file + base, word_size);

}
void pushValue(cpu* proc,u_int32_t base,reg_type type,u_int8_t reg_addr){
   
   context* prog=proc->running_system->proc_vec.processes[proc->running_system->curr_process];
   if(proc->stack_pointer<=prog->proc_allocd_start){
	dprintf(1,"ALREADY AT STACK TOP!!! SEGFAULT ON STACK PUSH!!!\n");
	raise(SIGINT);

   }
   u_int8_t word_size=calculateAddr(&base,type,reg_addr);
    u_int32_t true_sp=proc->stack_pointer;
   calculateAddr(&true_sp,type,reg_addr);
   memcpy(proc->running_system->mem->contents + prog->proc_data_start+true_sp,proc->reg_file + base,word_size);
   dprintf(1,"Stack pointer: %d\n",proc->stack_pointer+1);
}
void popValue(cpu* proc,u_int32_t base,reg_type type,u_int8_t reg_addr){
   
   context* prog=proc->running_system->proc_vec.processes[proc->running_system->curr_process];
   if(proc->stack_pointer>=prog->proc_allocd_size){
	dprintf(1,"ALREADY AT STACK BOTTOM!!! SEGFAULT ON STACK POP!!!\n");
	raise(SIGINT);

   }
   u_int8_t word_size=calculateAddr(&base,type,reg_addr);
   u_int32_t true_sp=proc->stack_pointer;
   calculateAddr(&true_sp,type,reg_addr);
   memcpy(proc->reg_file + base,proc->running_system->mem->contents + prog->proc_data_start+ true_sp, word_size);
}
void makeCall(cpu* proc,u_int32_t addr){
   
   context* prog=proc->running_system->proc_vec.processes[proc->running_system->curr_process];
   if((proc->stack_pointer-3)<=prog->proc_allocd_start){
	dprintf(1,"ALREADY AT STACK TOP!!! NO MORE CALLS POSSIBLE!!!\nFATAL SKILL FAILURE (RETURN ALREADY!!!! MEMORY AINT CHEAP YOU UNGRATEFUL PRICK)!!!\n");
	raise(SIGINT);

   }
   u_int32_t prev_pc=proc->curr_pc+1;
   proc->curr_pc+=addr;
   u_int8_t word_size=calculateAddr(&addr,0,0);
   u_int32_t next_fp=proc->stack_pointer-2;
   u_int32_t prev_pc_addr=next_fp-1;
   u_int32_t next_sp=prev_pc_addr-1;
   u_int32_t prev_fp=proc->frame_pointer;
   u_int32_t prev_sp=prev_fp-3;
   calculateAddr(&next_fp,0,0);
   calculateAddr(&next_sp,0,0);
   calculateAddr(&prev_pc_addr,0,0);
   memcpy(proc->running_system->mem->contents + prog->proc_data_start+ next_fp,&prev_fp ,word_size);
   memcpy(proc->running_system->mem->contents + prog->proc_data_start+ next_sp,&prev_sp, word_size);
   memcpy(proc->running_system->mem->contents + prog->proc_data_start+ prev_pc_addr,&prev_pc, word_size);
   proc->frame_pointer=proc->stack_pointer-2;
   proc->stack_pointer=proc->frame_pointer-3;

}
void makeReturn(cpu* proc){
   
   context* prog=proc->running_system->proc_vec.processes[proc->running_system->curr_process];
   if(proc->frame_pointer==prog->proc_allocd_size){
	dprintf(1,"ALREADY AT STACK BOTTOM!!! NO FUNCTION CALLED THIS FRAME DUMBASSS!!!!\nFATAL SKILL FAILURE (CALL SOMETHING ROCK HEAD)!!!\n");
	raise(SIGINT);
	
   }// pfp 294
	//psp 293
   u_int32_t prev_fp_addr=proc->frame_pointer;
   u_int32_t prev_pc_addr=prev_fp_addr-1;
   u_int32_t prev_sp_addr=prev_pc_addr-1;
   calculateAddr(&prev_fp_addr,0,0);
   calculateAddr(&prev_sp_addr,0,0);
   calculateAddr(&prev_pc_addr,0,0);
   memcpy(&proc->frame_pointer,proc->running_system->mem->contents + prog->proc_data_start+ prev_fp_addr ,sizeof(u_int32_t));
   memcpy(&proc->stack_pointer,proc->running_system->mem->contents + prog->proc_data_start+ prev_sp_addr ,sizeof(u_int32_t));
   memcpy(&proc->curr_pc,proc->running_system->mem->contents + prog->proc_data_start+ prev_pc_addr ,sizeof(u_int32_t));
}
static void load_imm(cpu*proc){
	u_int8_t dest=0;
	int16_t value=proc->instr_reg&proc->dec2->load_imm_oper_mask;
	u_int32_t unprocessed_dest=proc->instr_reg&proc->dec2->load_imm_dst_mask;
	unprocessed_dest>>=firstBitOne(proc->dec2->load_imm_dst_mask);
	dest|=unprocessed_dest;
	storeValueReg(proc,dest,0,value,0);
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
		case MEMI:
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
