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
#include "../Includes/mem_ops.h"


static void process_mem_op(decoder*dec,u_int32_t inst,u_int32_t* reg,u_int32_t*addr_reg,u_int32_t* reg_type){

	*reg=0,*addr_reg=0,*reg_type=0;
	u_int32_t unprocessed_reg=inst&dec->mem.mem_reg_mask;
	u_int32_t unprocessed_addr_reg=inst&dec->mem.mem_addr_reg_mask;
	u_int32_t unprocessed_reg_type=inst&dec->mem.mem_size_mask;
	unprocessed_reg>>=firstBitOne(dec->mem.mem_reg_mask);
	unprocessed_addr_reg>>=firstBitOne(dec->mem.mem_addr_reg_mask);
	unprocessed_reg_type>>=firstBitOne(dec->mem.mem_size_mask);
	(*reg)|=unprocessed_reg;
	(*addr_reg)|=unprocessed_addr_reg;
	(*reg_type)|=unprocessed_reg_type;
}

static void process_stack_op(decoder*dec,u_int32_t inst,u_int32_t* reg){

	*reg=0;
	u_int32_t unprocessed_reg=inst&dec->mem.stack_reg_mask;
	unprocessed_reg>>=firstBitOne(dec->mem.stack_reg_mask);
	(*reg)|=unprocessed_reg;
}

void proc_mem_op(cpu*proc,op_code code){
u_int32_t memregval,memaddrreg,memregtype;
int16_t addr;
u_int32_t stack_value_reg,stack_dest_reg;
switch(code){
	case STO:
		process_mem_op(&proc->dec,proc->instr_reg,&memregval,&memaddrreg,&memregtype);
		addr=getProcRegValue(proc,memaddrreg,memregtype);
		storeMemValue(proc,memregval,addr,memregtype,0);
		break;
	case LMEM:
		process_mem_op(&proc->dec,proc->instr_reg,&memregval,&memaddrreg,&memregtype);
		addr=getProcRegValue(proc,memaddrreg,memregtype);
		loadMemValue(proc,memregval,addr,memregtype,0);
		break;
	case LMEMR:
		process_mem_op(&proc->dec,proc->instr_reg,&memregval,&memaddrreg,&memregtype);
		addr=(int16_t)getProcRegValue(proc,memaddrreg,memregtype);
		loadMemValue(proc,memregval,proc->curr_pc+addr,memregtype,0);
		break;
	case PUSH:
		process_stack_op(&proc->dec,proc->instr_reg,&stack_value_reg);
		pushValue(proc,stack_value_reg,0,0);
		proc->stack_pointer--;
		break;
	case POP:
		proc->stack_pointer++;
		process_stack_op(&proc->dec,proc->instr_reg,&stack_dest_reg);
		popValue(proc,stack_dest_reg,0,0);
		break;
	default:
		break;
}
}
