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
#include "../Includes/alu.h"

static void process_alu_op(decoder*dec,u_int32_t inst,u_int32_t* reg1,u_int32_t* reg2, u_int32_t* dest,u_int32_t* reg1size,u_int32_t* reg2size, u_int32_t* destsize){

	*reg1=0,*reg2=0,*dest=0;
	*reg1size=0,*reg2size=0,*destsize=0;
	u_int32_t unprocessed_reg1=inst&dec->arith.alu_oper_1_mask,
			unprocessed_reg2=inst&dec->arith.alu_oper_2_mask,
			unprocessed_dest=inst&dec->arith.alu_dst_mask,
			unprocessed_size=inst&dec->arith.alu_op_size_mask,
			unprocessed_size2=inst&dec->arith.alu_op2_size_mask,
			unprocessed_destsize=inst&dec->arith.alu_dest_size_mask;
	unprocessed_reg1>>=firstBitOne(dec->arith.alu_oper_1_mask);
	unprocessed_reg2>>=firstBitOne(dec->arith.alu_oper_2_mask);
	unprocessed_dest>>=firstBitOne(dec->arith.alu_dst_mask);
	unprocessed_size>>=firstBitOne(dec->arith.alu_op_size_mask);
	unprocessed_size2>>=firstBitOne(dec->arith.alu_op2_size_mask);
	unprocessed_destsize>>=firstBitOne(dec->arith.alu_dest_size_mask);
	(*reg1)|=unprocessed_reg1;
	(*reg2)|=unprocessed_reg2;
	(*dest)|=unprocessed_dest;
	
	(*reg1size)|=unprocessed_size;
	(*reg2size)|=unprocessed_size2;
	(*destsize)|=unprocessed_destsize;
}

void proc_alu_op(cpu*proc,op_code code){
u_int32_t alureg1=0,alureg2=0,aludstreg=0,reg1size=0,reg2size=0,dstsize=0;
process_alu_op(proc->dec2,proc->instr_reg,&alureg1,&alureg2,&aludstreg,&reg1size,&reg2size,&dstsize);
u_int32_t value1=getProcRegValue(proc,alureg1,reg1size),value2=getProcRegValue(proc,alureg2,reg2size);
switch(code){
	case ADD:
		storeValueReg(proc,aludstreg,dstsize,value1+value2,0);
	break;
	case SUB:
		storeValueReg(proc,aludstreg,dstsize,value1-value2,0);
	break;
	case OR:
		storeValueReg(proc,aludstreg,dstsize,value1|value2,0);
	break;
	case AND:
		storeValueReg(proc,aludstreg,dstsize,value1&value2,0);
	break;
	default:
	break;

}


}
