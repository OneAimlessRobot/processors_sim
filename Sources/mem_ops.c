#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>
#include "../Includes/defaults.h"
#include "../Includes/helper.h"
#include "../Includes/memory.h"
#include "../Includes/cpu.h"
#include "../Includes/mem_ops.h"


static void process_mem_op(cpu*proc,u_int32_t inst,u_int8_t* reg,u_int8_t*addr_reg,u_int8_t* reg_type){

	*reg=0,*addr_reg=0,*reg_type=0;
	u_int32_t unprocessed_reg=inst&proc->mem_reg_mask;
	u_int32_t unprocessed_addr_reg=inst&proc->mem_addr_reg_mask;
	u_int32_t unprocessed_reg_type=inst&proc->mem_reg_type_mask;
	unprocessed_reg>>=firstBitOne(proc->mem_reg_mask);
	unprocessed_addr_reg>>=firstBitOne(proc->mem_addr_reg_mask);
	unprocessed_reg_type>>=firstBitOne(proc->mem_reg_type_mask);
	(*reg)|=unprocessed_reg;
	(*addr_reg)|=unprocessed_addr_reg;
	(*reg_type)|=unprocessed_reg_type;
}

void sto(cpu*proc,u_int32_t inst){
	u_int8_t memregval,memaddrreg,memregtype;
	process_mem_op(proc,inst,&memregval,&memaddrreg,&memregtype);
	u_int32_t value_to_sto=getProcRegValue(proc,memregval,FULL);
	u_int32_t addr=getProcRegValue(proc,memaddrreg,FULL);
	storeValue(proc->mem, addr*WORD_SIZE, WORD_SIZE, (void*)&value_to_sto);
}
void load(cpu*proc,u_int32_t inst){
	u_int8_t memregval,memaddrreg,memregtype;
	process_mem_op(proc,inst,&memregval,&memaddrreg,&memregtype);
	u_int32_t addr_to_load=getProcRegValue(proc,memaddrreg,FULL);
	loadMemValue(proc,memregval,addr_to_load,memregtype,0);
}
