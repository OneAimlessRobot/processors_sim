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
	
	if(!fscanf(fp,"%d",&dec->instr_num)){
		fclose(fp);
		return 0;
	}
	
	if(!fscanf(fp,"%x",&dec->op_code_mask)){
		fclose(fp);
		return 0;
	}
	
	dec->instructs= malloc(sizeof(instr_info)*dec->instr_num);
	for(u_int32_t i=0;i<dec->instr_num;i++){
		
		if(!fscanf(fp,"%u",&dec->instructs[i].arg_num)){
			fclose(fp);
			return 0;
		}
		dprintf(1,"Cheguei ao numero de argumentos deste grupo!!!\n");
			
		dec->instructs[i].masks=malloc(sizeof(u_int32_t)*dec->instructs[i].arg_num);
		for(u_int32_t j=0;j<dec->instructs[i].arg_num;j++){
			
		if(!fscanf(fp,"%x",&dec->instructs[i].masks[j])){
			
			fclose(fp);
			return 0;
		}
		dprintf(1,"Cheguei a mascara do argumento numero %d deste grupo!!!\n",j);
		}
		if(!fscanf(fp,"%u",&dec->instructs[i].num_group_ops)){
			fclose(fp);
			return 0;
		}
		dprintf(1,"Cheguei ao numero de ops deste grupo!!!\n");
		dec->instructs[i].headers=malloc(sizeof(instr_header)*dec->instructs[i].num_group_ops);

		for(u_int32_t j=0;j<dec->instructs[i].num_group_ops;j++){
			
		if(!fscanf(fp,"%ms %x",&dec->instructs[i].headers[j].instr_name,&dec->instructs[i].headers[j].instr_code)){
			fclose(fp);
			return 0;
		}
		dprintf(1,"Cheguei a op numero %d deste grupo!!!\n",j);
		}
	}
	for(u_int32_t i=0;i<dec->instr_num;i++){
		
		dprintf(1,"Grupo %u:\nMascaras:\n\n",i);
		for(u_int32_t j=0;j<dec->instructs[i].arg_num;j++){
			
		printWord(1,dec->instructs[i].masks[j]);
		dprintf(1,"\n");
		}
		
		dprintf(1,"Numero de operaçoes %u:\n",dec->instructs[i].num_group_ops);

		for(u_int32_t j=0;j<dec->instructs[i].num_group_ops;j++){

		dprintf(1,"\n\nOperaçao %u:\nNome: %s\nCodigo: %x\n\n",j,dec->instructs[i].headers[j].instr_name,dec->instructs[i].headers[j].instr_code);

		}
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
	if(!fscanf(fp,"%x",&dec->mem.mem_i_reg_mask)){
		fclose(fp);
		return 0;
	}
	dprintf(1,"\n");
	printWord(1,dec->mem.mem_i_reg_mask);
	if(!fscanf(fp,"%x",&dec->mem.mem_i_addr_mask)){
		fclose(fp);
		return 0;
	}
	dprintf(1,"\n");
	printWord(1,dec->mem.mem_i_addr_mask);
	if(!fscanf(fp,"%x",&dec->mem.mem_i_addr_off_mask)){
		fclose(fp);
		return 0;
	}
	dprintf(1,"\n");
	printWord(1,dec->mem.mem_i_addr_off_mask);
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
instr_info* find_instr_with_code(decoder* dec,u_int32_t code){
	for(u_int32_t i=0;i<dec->instr_num;i++){

		for(u_int32_t j=0;j<dec->instructs[i].num_group_ops;j++){
			if(code==dec->instructs[i].headers[j].instr_code){

			return &dec->instructs[i];

		}
		}

	}
	dprintf(1,"Codigo de Instrucao inexistente %x\n",code);
	return NULL;
	

}
instr_info* find_instr_with_name(decoder* dec,char* name){
	for(u_int32_t i=0;i<dec->instr_num;i++){

		
		for(u_int32_t j=0;j<dec->instructs[i].num_group_ops;j++){
			if(strings_are_equal(dec->instructs[i].headers[j].instr_name,name)){

			return &dec->instructs[i];

		}
		}

	}
	dprintf(1,"Nome de Instrucao inexistente %s\n",name);
	return NULL;
	

}
void freeDecoder(decoder**dec){
	if(*dec){
	u_int32_t num_of_instr=(*dec)->instr_num;
	if((*dec)->instructs){
	for(u_int32_t i=0;i<num_of_instr;i++){
		
	
	if((*dec)->instructs[i].headers){
		
		for(u_int32_t j=0;j<(*dec)->instructs[i].num_group_ops;j++){
		if((*dec)->instructs[i].headers[j].instr_name){
		free((*dec)->instructs[i].headers[j].instr_name);
		(*dec)->instructs[i].headers[j].instr_name=NULL;
		}
	}
		free((*dec)->instructs[i].headers);
		(*dec)->instructs[i].headers=NULL;
	}
	if((*dec)->instructs[i].masks){
		free((*dec)->instructs[i].masks);
		(*dec)->instructs[i].masks=NULL;
		}
	
	}
	free((*dec)->instructs);
	(*dec)->instructs=NULL;
	}
	free(*dec);
	*dec=NULL;
	
	}


}
u_int32_t get_op_code_inside_group(instr_info* info,char* string){
	for(u_int32_t i=0;i<info->num_group_ops;i++){
		if(strings_are_equal(string,info->headers[i].instr_name)){
			
			return info->headers[i].instr_code;
			
		}

	}
	return 0;


}
char* get_op_name_inside_group(instr_info* info,u_int32_t code){
	for(u_int32_t i=0;i<info->num_group_ops;i++){
		if(code==info->headers[i].instr_code){
			
			return info->headers[i].instr_name;
			
		}

	}
	return NULL;


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
	case STOI:
		type=MEMI;
		break;
	case LMEMI:
		type=MEMI;
		break;
	case LMEMIR:
		type=MEMI;
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
	case CALL:
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
