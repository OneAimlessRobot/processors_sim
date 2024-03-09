#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>
#include "../Includes/defaults.h"
#include "../Includes/helper.h"
#include "../Includes/memory.h"
#include "../Includes/cpu.h"
#include "../Includes/cond_ops.h"

static void process_cond_op(cpu*proc,u_int32_t inst,u_int32_t* addr){
	*addr=0;;
	u_int32_t unprocessed_addr=inst&proc->jmp_addr_mask;
	unprocessed_addr>>=firstBitOne(proc->jmp_addr_mask);
	(*addr)|=unprocessed_addr;
}
static void cond_load(cpu*proc,u_int32_t inst,u_int8_t* reg,u_int32_t* value){
	(*reg)=0,(*value)=0;
	u_int32_t unprocessed_dest=inst&proc->cmp_reg_mask;
	u_int32_t unprocessed_value=inst&proc->cmp_value_mask;
	unprocessed_dest>>=firstBitOne(proc->cmp_reg_mask);
	unprocessed_value>>=firstBitOne(proc->cmp_value_mask);
	(*reg)|=unprocessed_dest;
	(*value)|=unprocessed_value;
}

void jmp(cpu*proc,u_int32_t inst){
	u_int32_t addr_val;
	proc->prev_pc=proc->curr_pc;
	process_cond_op(proc,inst,&addr_val);
	proc->curr_pc=addr_val;
}
void ret(cpu*proc,u_int32_t inst){
	u_int32_t addr_val;
	process_cond_op(proc,inst,&addr_val);
	proc->curr_pc=proc->prev_pc+addr_val;
	proc->prev_pc=proc->curr_pc;
}
void label(cpu*proc,u_int32_t inst){
	proc->prev_pc=proc->curr_pc;
}
void cmp(cpu*proc,u_int32_t inst){
	u_int32_t value;
	u_int8_t reg;
	cond_load(proc,inst,&reg,&value);
	proc->bz_flag=!(getProcRegValue(proc,reg)-value);
}
void bz(cpu*proc,u_int32_t inst){
	u_int32_t addr_val;
	process_cond_op(proc,inst,&addr_val);
	
	switch(proc->bz_flag){
		case 0:
			break;
		default:
			proc->prev_pc=proc->curr_pc;
			proc->curr_pc=addr_val;
			proc->bz_flag=0;
			break;

	}
}
