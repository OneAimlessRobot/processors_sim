#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include "../Includes/defaults.h"
#include "../Includes/helper.h"
#include "../Includes/memory.h"
#include "../Includes/cpu.h"
#include "../Includes/alu.h"
#include "../Includes/mem_ops.h"
#include "../Includes/cond_ops.h"
#include "../Includes/compiler.h"
name_code_pair conv_table[]={
					{"add",ADD},
					{"sub",SUB},
					{"or",OR},
					{"and",AND},
					{"load",LMEM},
					{"limm",LIMM},
					{"store",STO},
					{"jmp",JMP},
					{"ret",RET},
					{"label",LAB},
					{"cmp",CMP},
					{"bz",BZERO},
					{"bnz",BNZERO},
					{NULL,0}
			};

static u_int32_t mask_photograph(u_int32_t mask,u_int32_t value){
	u_int8_t advance=firstBitOne(mask);
	u_int8_t retreat=firstBitOne(mask);
	u_int32_t result=0x0;
	mask>>=retreat;
	result|=(value&mask);
	result<<=advance;
	return result;
	

}
static int strings_are_equal(char* str1,char* str2){
	
	int same_length=!(strlen(str1)-strlen(str2));
	
	int same_content= !strncmp(str1,str2,strlen(str1));
	return same_content&&same_length;
	


}

static u_int32_t find_in_conv_table(name_code_pair table[],char* string){
	if(!string){
		printf("null string in compiler instruction finder!!!!\n");
		return 0;
	}
	if(!strlen(string)){
		printf("empty string in compiler instruction finder!!!!\n");
		return 0;
	}
	for(int i=0;table[i].string;i++){
		
		if(strings_are_equal(table[i].string,string)){
			dprintf(1,"%x instruction found!!!!!\n",table[i].code);
			return table[i].code;
		}

	}
	return 0;
	


}
static void get_next_instruction(FILE* fp,char buff[1024]){
	
	skip_cpu_comments(fp);
	int curr_char=0;
	u_int64_t curr_char_index=0;
	memset(buff,0,1024);
	while((curr_char=fgetc(fp))!=';'&&curr_char !=EOF){
		buff[curr_char_index++]=(char)curr_char;
	}
	
	if(curr_char==EOF){
	memset(buff,0,1024);
	return;
	}
	skip_cpu_comments(fp);
	
}
static u_int32_t decypher_instruction(cpu*proc,u_int32_t code,char* buff){
	u_int32_t result=0x0;
	u_int32_t alu1=0,alu2=0,alu3=0;
	u_int32_t mem1=0,mem2=0,mem3=0,mem4=0;
	u_int32_t loadimm_reg=0,loadimm_value=0;
	u_int32_t cmp_reg=0,cmp_value=0;
	u_int32_t jmp_addr=0;
	u_int32_t bzero_addr=0;
	u_int32_t ret_off=0;
	switch(code){
	case ADD:
		
		if(!sscanf(buff,"%u%u%u",&alu1,&alu2,&alu3)){
			perror("Compiling error!!!!! bad instruction syntax in add!!!!\nNULL instruction loaded!\n");
			return result;
		}
		result|=code;
		result|=mask_photograph(proc->alu_oper_1_mask,alu1);
		result|=mask_photograph(proc->alu_oper_2_mask,alu2);
		result|=mask_photograph(proc->alu_dst_mask,alu3);
		break;
	case LMEM:
		if(!sscanf(buff,"%u%u%u%u",&mem1,&mem2,&mem3,&mem4)){
			perror("Compiling error!!!!! bad instruction syntax in load!!!!\nNULL instruction loaded!\n");
			return result;
		}
		result|=code;
		result|=mask_photograph(proc->mem_reg_mask,mem1);
		result|=mask_photograph(proc->mem_addr_reg_mask,mem2);
		result|=mask_photograph(proc->mem_reg_type_mask,mem3);
		result|=mask_photograph(proc->mem_size_mask,mem4);
		break;
	case SUB:
		if(!sscanf(buff,"%u%u%u",&alu1,&alu2,&alu3)){
			perror("Compiling error!!!!! bad instruction syntax in sub!!!!\nNULL instruction loaded!\n");
			return result;
		}
		result|=code;
		result|=mask_photograph(proc->alu_oper_1_mask,alu1);
		result|=mask_photograph(proc->alu_oper_2_mask,alu2);
		result|=mask_photograph(proc->alu_dst_mask,alu3);
		break;
	case STO:
		if(!sscanf(buff,"%u%u%u%u",&mem1,&mem2,&mem3,&mem4)){
			perror("Compiling error!!!!! bad instruction syntax in store!!!!\nNULL instruction loaded!\n");
			return result;
		}
		result|=code;
		result|=mask_photograph(proc->mem_reg_mask,mem1);
		result|=mask_photograph(proc->mem_addr_reg_mask,mem2);
		result|=mask_photograph(proc->mem_reg_type_mask,mem3);
		result|=mask_photograph(proc->mem_size_mask,mem4);
		break;
	case LIMM:
		if(!sscanf(buff,"%u%u",&loadimm_reg,&loadimm_value)){
			perror("Compiling error!!!!! bad instruction syntax in store!!!!\nNULL instruction loaded!\n");
			return result;
		}
		result|=code;
		result|=mask_photograph(proc->load_imm_dst_mask,loadimm_reg);
		result|=mask_photograph(proc->load_imm_oper_mask,loadimm_value);
		break;
	case AND:
		if(!sscanf(buff,"%u%u%u",&alu1,&alu2,&alu3)){
			perror("Compiling error!!!!! bad instruction syntax in and!!!!\nNULL instruction loaded!\n");
			return result;
		}
		result|=code;
		result|=mask_photograph(proc->alu_oper_1_mask,alu1);
		result|=mask_photograph(proc->alu_oper_2_mask,alu2);
		result|=mask_photograph(proc->alu_dst_mask,alu3);
		break;
	case OR:
		if(!sscanf(buff,"%u%u%u",&alu1,&alu2,&alu3)){
			perror("Compiling error!!!!! bad instruction syntax in or!!!!\nNULL instruction loaded!\n");
			return result;
		}
		result|=code;
		result|=mask_photograph(proc->alu_oper_1_mask,alu1);
		result|=mask_photograph(proc->alu_oper_2_mask,alu2);
		result|=mask_photograph(proc->alu_dst_mask,alu3);
		break;
	case JMP:
		if(!sscanf(buff,"%u",&jmp_addr)){
			perror("Compiling error!!!!! bad instruction syntax in jmp!!!!\nNULL instruction loaded!\n");
			return result;
		}
		result|=code;
		result|=mask_photograph(proc->jmp_addr_mask,jmp_addr);
		break;
	case RET:
		if(!sscanf(buff,"%u",&ret_off)){
			perror("Compiling error!!!!! bad instruction syntax in jmp!!!!\nNULL instruction loaded!\n");
			return result;
		}
		result|=code;
		result|=mask_photograph(proc->jmp_addr_mask,ret_off);
		break;
	case LAB:
		result|=code;
		break;
	case CMP:
		if(!sscanf(buff,"%u%u",&cmp_reg,&cmp_value)){
			perror("Compiling error!!!!! bad instruction syntax in store!!!!\nNULL instruction loaded!\n");
			return result;
		}
		result|=code;
		result|=mask_photograph(proc->cmp_reg_mask,cmp_reg);
		result|=mask_photograph(proc->cmp_value_mask,cmp_value);
		break;
	case BZERO:
		if(!sscanf(buff,"%u",&bzero_addr)){
			perror("Compiling error!!!!! bad instruction syntax in jmp!!!!\nNULL instruction loaded!\n");
			return result;
		}
		result|=code;
		result|=mask_photograph(proc->jmp_addr_mask,bzero_addr);
		break;
	case BNZERO:
		if(!sscanf(buff,"%u",&bzero_addr)){
			perror("Compiling error!!!!! bad instruction syntax in jmp!!!!\nNULL instruction loaded!\n");
			return result;
		}
		result|=code;
		result|=mask_photograph(proc->jmp_addr_mask,bzero_addr);
		break;
	default:
		break;

	}
	return result;

}
static u_int32_t process_instruction(cpu*proc,char buff[1024]){
	char* ptr=buff;
	char inst_name[1024]={0};
	int num_consumed=0;
	sscanf(ptr,"%s%n",inst_name,&num_consumed);
	ptr+=num_consumed;
	u_int32_t code=find_in_conv_table(conv_table,inst_name);
	return decypher_instruction(proc,code,ptr);

}


void compile(cpu*proc,FILE* fpin,FILE* fpout){
	char buff[1024]={0};
	u_int32_t num_of_inst=0;
	do{
	get_next_instruction(fpin,buff);
	num_of_inst++;
	}while(strlen(buff));
	
	fprintf(fpout,"%x\n",num_of_inst);
	rewind(fpin);
	for(u_int32_t i=0;i<num_of_inst-1;i++){
	get_next_instruction(fpin,buff);
	u_int32_t inst=0x0;
	if(strlen(buff)){
	inst=process_instruction(proc,buff);
	}
	fprintf(fpout,"%x\n",inst);
	}
}
