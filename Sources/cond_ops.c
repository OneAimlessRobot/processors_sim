#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>
#include "../Includes/defaults.h"
#include "../Includes/helper.h"
#include "../Includes/memory.h"
#include "../Includes/decoder.h"
#include "../Includes/cpu.h"
#include "../Includes/cond_ops.h"

static void process_cond_op(decoder*dec,u_int32_t inst,int16_t* addr){
	*addr=0;;
	int16_t unprocessed_addr=inst&dec->mem.jmp_addr_mask;
	unprocessed_addr>>=firstBitOne(dec->mem.jmp_addr_mask);
	(*addr)|=unprocessed_addr;
}
static void cond_load(decoder*dec,u_int32_t inst,u_int8_t* reg,u_int32_t* value){
	(*reg)=0,(*value)=0;
	u_int32_t unprocessed_dest=inst&dec->arith.cmp_reg_mask;
	u_int32_t unprocessed_value=inst&dec->arith.cmp_value_mask;
	unprocessed_dest>>=firstBitOne(dec->arith.cmp_reg_mask);
	unprocessed_value>>=firstBitOne(dec->arith.cmp_value_mask);
	(*reg)|=unprocessed_dest;
	(*value)|=unprocessed_value;
}
static void bz(cpu*proc,u_int32_t inst){
	int16_t addr_val;
	process_cond_op(&proc->dec,inst,&addr_val);
	
	switch(proc->status_word&proc->dec.ccu.z_flag_mask){
		case 0:
			break;
		default:
			proc->prev_pc=proc->curr_pc;
			proc->curr_pc+=(int16_t)addr_val;
			break;

	}
}
static void bnz(cpu*proc,u_int32_t inst){
	int16_t addr_val;
	process_cond_op(&proc->dec,inst,&addr_val);
	
	switch(proc->status_word&proc->dec.ccu.z_flag_mask){
		case 0:
			proc->prev_pc=proc->curr_pc;
			proc->curr_pc+=(int16_t)addr_val;
			break;
		default:
			break;

	}
}
void proc_cond_op(cpu*proc,op_code code,u_int32_t inst){
	int16_t addr_val;
	u_int32_t value;
	u_int8_t reg;
	process_cond_op(&proc->dec,inst,&addr_val);
switch(code){
	case JMP:
	proc->prev_pc=proc->curr_pc;
	proc->curr_pc+=(int16_t)addr_val;
	break;
	case CMP:
	cond_load(&proc->dec,inst,&reg,&value);
	proc->status_word|=(!(getProcRegValue(proc,reg,0)-value) ? 0x1 : 0x0);
	break;
	case BZERO:
	bz(proc,inst);
	break;
	case BNZERO:
	bnz(proc,inst);
	break;
	case RET:
	proc->curr_pc=proc->prev_pc+addr_val;
	proc->prev_pc=proc->curr_pc;
	break;
	default:
	break;
}


}
