#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>
#include "../Includes/defaults.h"
#include "../Includes/helper.h"
#include "../Includes/memory.h"
#include "../Includes/cpu.h"
#include "../Includes/alu.h"

static void process_alu_op(cpu*proc,u_int32_t inst,u_int32_t* reg1,u_int32_t* reg2, u_int32_t* dest,u_int32_t* reg1size,u_int32_t* reg2size, u_int32_t* destsize){

	*reg1=0,*reg2=0,*dest=0;
	*reg1size=0,*reg2size=0,*destsize=0;
	u_int32_t unprocessed_reg1=inst&proc->alu_oper_1_mask,
			unprocessed_reg2=inst&proc->alu_oper_2_mask,
			unprocessed_dest=inst&proc->alu_dst_mask,
			unprocessed_size=inst&proc->alu_op_size_mask,
			unprocessed_size2=inst&proc->alu_op2_size_mask,
			unprocessed_destsize=inst&proc->alu_dest_size_mask;
	unprocessed_reg1>>=firstBitOne(proc->alu_oper_1_mask);
	unprocessed_reg2>>=firstBitOne(proc->alu_oper_2_mask);
	unprocessed_dest>>=firstBitOne(proc->alu_dst_mask);
	unprocessed_size>>=firstBitOne(proc->alu_op_size_mask);
	unprocessed_size2>>=firstBitOne(proc->alu_op2_size_mask);
	unprocessed_destsize>>=firstBitOne(proc->alu_dest_size_mask);
	(*reg1)|=unprocessed_reg1;
	(*reg2)|=unprocessed_reg2;
	(*dest)|=unprocessed_dest;
	
	(*reg1size)|=unprocessed_size;
	(*reg2size)|=unprocessed_size2;
	(*destsize)|=unprocessed_destsize;
}
void add(cpu*proc,u_int32_t inst){

	u_int32_t alureg1,alureg2,aludstreg,reg1size,reg2size,dstsize;
	process_alu_op(proc,inst,&alureg1,&alureg2,&aludstreg,&reg1size,&reg2size,&dstsize);
	u_int32_t value1=getProcRegValue(proc,alureg1,reg1size),value2=getProcRegValue(proc,alureg2,reg2size);
	storeValueReg(proc,aludstreg,dstsize,value1+value2,0);
}
void sub(cpu*proc,u_int32_t inst){


	u_int32_t alureg1,alureg2,aludstreg,reg1size,reg2size,dstsize;
	process_alu_op(proc,inst,&alureg1,&alureg2,&aludstreg,&reg1size,&reg2size,&dstsize);
	u_int32_t value1=getProcRegValue(proc,alureg1,reg1size),value2=getProcRegValue(proc,alureg2,reg2size);
	storeValueReg(proc,aludstreg,dstsize,value1-value2,0);
}
void or(cpu*proc,u_int32_t inst){

	u_int32_t alureg1,alureg2,aludstreg,reg1size,reg2size,dstsize;
	process_alu_op(proc,inst,&alureg1,&alureg2,&aludstreg,&reg1size,&reg2size,&dstsize);
	u_int32_t value1=getProcRegValue(proc,alureg1,reg1size),value2=getProcRegValue(proc,alureg2,reg2size);
	storeValueReg(proc,aludstreg,dstsize,value1|value2,0);
}
void and(cpu*proc,u_int32_t inst){

	u_int32_t alureg1,alureg2,aludstreg,reg1size,reg2size,dstsize;
	process_alu_op(proc,inst,&alureg1,&alureg2,&aludstreg,&reg1size,&reg2size,&dstsize);
	u_int32_t value1=getProcRegValue(proc,alureg1,reg1size),value2=getProcRegValue(proc,alureg2,reg2size);
	storeValueReg(proc,aludstreg,dstsize,value1&value2,0);
}
