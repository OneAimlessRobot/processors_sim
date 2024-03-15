#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include "../Includes/defaults.h"
#include "../Includes/helper.h"
#include "../Includes/decoder.h"

int load_cpu_masks(decoder*dec){
FILE* fp=NULL;
	if(!(fp=fopen(CPU_DEC_PATH,"r"))){

		perror("NO CPU FILE FOUND!!!!!\n");
		return 0;

	}

	skip_cpu_comments(fp);
	if(!fscanf(fp,"%x",&dec->op_code_mask)){
		fclose(fp);
		return 0;
	}
	
	
	skip_cpu_comments(fp);
	if(!fscanf(fp,"%x",&dec->arith.alu_oper_1_mask)){
		fclose(fp);
		return 0;
	}
	dprintf(1,"\n");
	printWord(1,dec->arith.alu_oper_1_mask);
	if(!fscanf(fp,"%x",&dec->arith.alu_oper_2_mask)){
		fclose(fp);
		return 0;
	}
	
	dprintf(1,"\n");
	printWord(1,dec->arith.alu_oper_2_mask);
	if(!fscanf(fp,"%x",&dec->arith.alu_dst_mask)){
		fclose(fp);
		return 0;
	}
	
	dprintf(1,"\n");
	printWord(1,dec->arith.alu_dst_mask);
	if(!fscanf(fp,"%x",&dec->arith.alu_op_size_mask)){
		fclose(fp);
		return 0;
	}
	
	dprintf(1,"\n");
	printWord(1,dec->arith.alu_op_size_mask);

	if(!fscanf(fp,"%x",&dec->arith.alu_op2_size_mask)){
		fclose(fp);
		return 0;
	}
	
	dprintf(1,"\n");
	printWord(1,dec->arith.alu_op2_size_mask);

	if(!fscanf(fp,"%x",&dec->arith.alu_dest_size_mask)){
		fclose(fp);
		return 0;
	}
	
	dprintf(1,"\n");
	printWord(1,dec->arith.alu_dest_size_mask);


	
	skip_cpu_comments(fp);
	if(!fscanf(fp,"%x",&dec->load_imm_dst_mask)){
		fclose(fp);
		return 0;
	}
	
	dprintf(1,"\n");
	printWord(1,dec->load_imm_dst_mask);
	if(!fscanf(fp,"%x",&dec->load_imm_oper_mask)){
		fclose(fp);
		return 0;
	}

	dprintf(1,"\n");
	printWord(1,dec->load_imm_oper_mask);
	
	skip_cpu_comments(fp);
	if(!fscanf(fp,"%x",&dec->mem.mem_reg_mask)){
		fclose(fp);
		return 0;
	}
	dprintf(1,"\n");
	printWord(1,dec->mem.mem_reg_mask);
	if(!fscanf(fp,"%x",&dec->mem.mem_addr_reg_mask)){
		fclose(fp);
		return 0;
	}
	dprintf(1,"\n");
	printWord(1,dec->mem.mem_addr_reg_mask);
	if(!fscanf(fp,"%x",&dec->mem.mem_reg_type_mask)){
		fclose(fp);
		return 0;
	}
	dprintf(1,"\n");
	printWord(1,dec->mem.mem_reg_type_mask);
	if(!fscanf(fp,"%x",&dec->mem.mem_size_mask)){
		fclose(fp);
		return 0;
	}
	dprintf(1,"\n");
	printWord(1,dec->mem.mem_size_mask);
	skip_cpu_comments(fp);
	if(!fscanf(fp,"%x",&dec->mem.jmp_addr_mask)){
		fclose(fp);
		return 0;
	}
	dprintf(1,"\n");
	printWord(1,dec->mem.jmp_addr_mask);
	skip_cpu_comments(fp);
	if(!fscanf(fp,"%x",&dec->arith.cmp_reg_mask)){
		fclose(fp);
		return 0;
	}
	dprintf(1,"\n");
	printWord(1,dec->arith.cmp_reg_mask);
	if(!fscanf(fp,"%x",&dec->arith.cmp_value_mask)){
		fclose(fp);
		return 0;
	}
	dprintf(1,"\n");
	skip_cpu_comments(fp);
	printWord(1,dec->arith.cmp_value_mask);
	dprintf(1,"\n");
	if(!fscanf(fp,"%x",&dec->ccu.z_flag_mask)){
		fclose(fp);
		return 0;
	}
	printWord(1,dec->ccu.z_flag_mask);
	dprintf(1,"\n");
	skip_cpu_comments(fp);
	if(!fscanf(fp,"%x",&dec->mem.stack_reg_mask)){
		fclose(fp);
		return 0;
	}
	printWord(1,dec->mem.stack_reg_mask);
	dprintf(1,"\n");
	fclose(fp);
	return 1;

}

instr_type get_instr_type(op_code code){


instr_type type=0;
	switch(code){
	case ADD:
		type=ALU;
		break;
	case LMEM:
		type=MEM;
		break;
	case LMEMR:
		type=MEM;
		break;
	case PUSH:
		type=STACK;
		break;
	case POP:
		type=STACK;
		break;
	case SUB:
		type=ALU;
		break;
	case STO:
		type=MEM;
		break;
	case LIMM:
		type=IMM;
		break;
	case AND:
		type=ALU;
		break;
	case OR:
		type=ALU;
		break;
	case JMP:
		type=CONTROL;
		break;
	case RET:
		type=CONTROL;
		break;
	case CMP:
		type=COND;
		break;
	case BZERO:
		type=CONTROL;
		break;
	case BNZERO:
		type=CONTROL;
		break;
	default:
		break;

	}
return type;
}
