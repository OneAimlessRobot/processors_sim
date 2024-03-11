#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <signal.h>
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

static u_int32_t code_code_start=0,code_data_start=0,curr_code=0,num_of_names=0,num_of_labels=0;
#define MAX_NAMES 1024
static FILE* fpmid=NULL;

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
					{"cmp",CMP},
					{"bz",BZERO},
					{"bnz",BNZERO},
					{NULL,0}
			};


name_addr_value_triple avail_names[MAX_NAMES]={0};

name_addr_pair avail_labels[MAX_NAMES]={0};

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

static name_addr_pair* find_in_label_table(char* string){
	if(!string){
		printf("null string in compiler curr label finder!!!!\n");
		return NULL;
	}
	if(!strlen(string)){
		printf("empty string in compiler curr label finder!!!!\n");
		return NULL;
	}
	for(int i=0;avail_labels[i].string;i++){
		
		if(strings_are_equal(avail_labels[i].string,string)){
		printf("label %s %u found!!! label finder!!!!\n",avail_labels[i].string,avail_labels[i].addr);
			return &avail_labels[i];
		}

	}
	return NULL;


}
static name_addr_value_triple* find_in_var_table(char* string){
	if(!string){
		printf("null string in compiler curr var finder!!!!\n");
		return NULL;
	}
	if(!strlen(string)){
		printf("empty string in compiler curr var finder!!!!\n");
		return NULL;
	}
	for(int i=0;avail_names[i].string;i++){
		
		if(strings_are_equal(avail_names[i].string,string)){
		printf("var %s %u %d found!!! var finder!!!!\n",avail_names[i].string,avail_names[i].value,avail_names[i].addr);
			return &avail_names[i];
		}

	}
	return NULL;


}
static u_int32_t find_in_conv_table(char* string){
	if(!string){
		printf("null string in compiler instruction finder!!!!\n");
		return 0;
	}
	if(!strlen(string)){
		printf("empty string in compiler instruction finder!!!!\n");
		return 0;
	}
	for(int i=0;conv_table[i].string;i++){
		
		if(strings_are_equal(conv_table[i].string,string)){
			dprintf(1,"%x instruction found!!!!!\n",conv_table[i].code);
			return conv_table[i].code;
		}

	}
	return 0;
	


}
static int32_t find_in_string_arr(char* table[],char* string){
	if(!string){
		printf("null string in compiler keyword finder!!!!\n");
		return -1;
	}
	if(!strlen(string)){
		printf("empty string in compiler keyword finder!!!!\n");
		return -1;
	}
	for(int i=0;table[i];i++){
		
		if(strings_are_equal(table[i],string)){
			return i;
		}

	}
	return -1;
	


}
static int32_t get_next_instruction(FILE* fp,char buff[1024]){
	
	skip_cpu_comments(fp);
	int32_t init_of_instr=ftell(fp);
	int curr_char=0;
	u_int64_t curr_char_index=0;
	memset(buff,0,1024);
	while((curr_char=fgetc(fp))!=';'&&(curr_char)!=':'&&curr_char !=EOF){
		buff[curr_char_index++]=(char)curr_char;
	}
	if(curr_char==EOF){
	memset(buff,0,1024);
	return init_of_instr;
	}
	buff[curr_char_index]=curr_char;
	skip_cpu_comments(fp);
	return init_of_instr;
	
}
static u_int32_t decypher_instruction(cpu*proc,u_int32_t code,char* buff){
	u_int32_t result=0x0;
	u_int32_t alu1=0,alu2=0,alu3=0,alu4=0,alu5=0,alu6=0;
	u_int32_t mem1=0,mem2=0,mem3=0,mem4=0;
	u_int32_t loadimm_reg=0,loadimm_value=0;
	u_int32_t cmp_reg=0,cmp_value=0;
	u_int32_t jmp_addr=0;
	u_int32_t bzero_addr=0;
	u_int32_t ret_off=0;
	switch(code){
	case ADD:
		
		if(!sscanf(buff,"%u%u%u%u%u%u",&alu1,&alu2,&alu3,&alu4,&alu5,&alu6)){
			perror("Compiling error!!!!! bad instruction syntax in add!!!!\nNULL instruction loaded!\n");
			return result;
		}
		result|=code;
		result|=mask_photograph(proc->alu_oper_1_mask,alu1);
		result|=mask_photograph(proc->alu_oper_2_mask,alu2);
		result|=mask_photograph(proc->alu_dst_mask,alu3);
	
		result|=mask_photograph(proc->alu_op_size_mask,alu4);
		result|=mask_photograph(proc->alu_op2_size_mask,alu5);
		result|=mask_photograph(proc->alu_dest_size_mask,alu6);
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
		if(!sscanf(buff,"%u%u%u%u%u%u",&alu1,&alu2,&alu3,&alu4,&alu5,&alu6)){
			perror("Compiling error!!!!! bad instruction syntax in sub!!!!\nNULL instruction loaded!\n");
			return result;
		}
		result|=code;
		result|=mask_photograph(proc->alu_oper_1_mask,alu1);
		result|=mask_photograph(proc->alu_oper_2_mask,alu2);
		result|=mask_photograph(proc->alu_dst_mask,alu3);
	
		result|=mask_photograph(proc->alu_op_size_mask,alu4);
		result|=mask_photograph(proc->alu_op2_size_mask,alu5);
		result|=mask_photograph(proc->alu_dest_size_mask,alu6);
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
		if(!sscanf(buff,"%u%u%u%u%u%u",&alu1,&alu2,&alu3,&alu4,&alu5,&alu6)){
			perror("Compiling error!!!!! bad instruction syntax in and!!!!\nNULL instruction loaded!\n");
			return result;
		}
		result|=code;
		result|=mask_photograph(proc->alu_oper_1_mask,alu1);
		result|=mask_photograph(proc->alu_oper_2_mask,alu2);
		result|=mask_photograph(proc->alu_dst_mask,alu3);
	
		result|=mask_photograph(proc->alu_op_size_mask,alu4);
		result|=mask_photograph(proc->alu_op2_size_mask,alu5);
		result|=mask_photograph(proc->alu_dest_size_mask,alu6);
		break;
	case OR:
		if(!sscanf(buff,"%u%u%u%u%u%u",&alu1,&alu2,&alu3,&alu4,&alu5,&alu6)){
			perror("Compiling error!!!!! bad instruction syntax in or!!!!\nNULL instruction loaded!\n");
			return result;
		}
		result|=code;
		result|=mask_photograph(proc->alu_oper_1_mask,alu1);
		result|=mask_photograph(proc->alu_oper_2_mask,alu2);
		result|=mask_photograph(proc->alu_dst_mask,alu3);
	
		result|=mask_photograph(proc->alu_op_size_mask,alu4);
		result|=mask_photograph(proc->alu_op2_size_mask,alu5);
		result|=mask_photograph(proc->alu_dest_size_mask,alu6);
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
//Powered by chatgpt

static void emplaceString(FILE* fp,char* string,int newstrsize,int whitespacelength,int instr_pos){
// Calculate the end position of the file
fseek(fp, 0, SEEK_END);
int end = ftell(fp);

// Calculate the position after inserting new instruction
int pos = instr_pos + whitespacelength;

// Calculate the size of the content after the insertion point
int size = end - pos;

// Allocate memory for content after the insertion point
char auxil[size];

// Read content after insertion point
fseek(fp, pos, SEEK_SET);
fread(auxil, 1, size, fp);

// Write whitespace to replace old instruction
fseek(fp, instr_pos, SEEK_SET);
char whitespace[whitespacelength];
memset(whitespace, ' ', whitespacelength);
fwrite(whitespace, 1, whitespacelength, fp);

// Write new instruction
fwrite(string, 1, newstrsize, fp);

// Write content after insertion point
int pos2 = ftell(fp);
fwrite(auxil, 1, size, fp);

// Move file pointer to correct position
fseek(fp, pos2, SEEK_SET);


}
static u_int32_t process_instruction(cpu*proc,char buff[1024]){
	char* ptr=buff;
	char inst_name[1024]={0};
	int num_consumed=0;
	sscanf(ptr,"%s%n",inst_name,&num_consumed);
	ptr+=num_consumed;
	u_int32_t code=find_in_conv_table(inst_name);
	return decypher_instruction(proc,code,ptr);

}

void substitute_vars(int instr_pos,char buff[1024]){
	
	int curr_str_len=strlen(buff);
	if(!curr_str_len){
		return;
	}
	int subst_cursor=0;
	int ntokens=0;
	char newInstr[1024]={0};
	char* ptr=newInstr;
	char* old_buff_ptr=buff;
	char curr_token[128]={0};
	while(1){

	if(sscanf(old_buff_ptr,"%s%n",curr_token,&subst_cursor)>0){
		ntokens++;
	}if(!strlen(curr_token)){

		break;
	}
	int putsemicolon=0;
	if(curr_token[strlen(curr_token)-1]==';'){
		putsemicolon=1;
		curr_token[strlen(curr_token)-1]=0;
	}
	else if(curr_token[strlen(curr_token)-1]==':'){

		curr_token[strlen(curr_token)-1]=0;
		return;
	}
		name_addr_value_triple* trip=NULL;
		
		if(ntokens==1&&!find_in_conv_table(curr_token)){
				printf("ERRO DE COMPILACAO!!!! Instruçao desconhecida '%s'!!!!!",curr_token);
				raise(SIGINT);
		}
		if((trip=find_in_var_table(curr_token))){
			ptr+=snprintf(ptr,128,"%d ",trip->value);

		}
		else{
			
			ptr+=snprintf(ptr,128,"%s ",curr_token);

		}
			if(putsemicolon){
			ptr[-1]=';';
				break;
			}

		old_buff_ptr+=subst_cursor;
		memset(curr_token,0,128);
	}
	
	emplaceString(fpmid, newInstr,strlen(newInstr),curr_str_len,instr_pos);

}

void substitute_labels(int instr_pos,char buff[1024]){
	
	int curr_str_len=strlen(buff);
	if(!curr_str_len){
		return;
	}
	int subst_cursor=0;
	char newInstr[1024]={0};
	int ntokens=0;
	char* ptr=newInstr;
	char* old_buff_ptr=buff;
	char curr_token[128]={0};
	while(1){
	if(sscanf(old_buff_ptr,"%s%n",curr_token,&subst_cursor)>0){
		ntokens++;
	}
	if(!strlen(curr_token)){

		break;
	}
	int putsemicolon=0;
	
	if(curr_token[strlen(curr_token)-1]==';'){
		putsemicolon=1;
		curr_token[strlen(curr_token)-1]=0;
	}
	else if(curr_token[strlen(curr_token)-1]==':'){

		curr_token[strlen(curr_token)-1]=0;
		return;
	}
		name_addr_pair* trip=NULL;
		
		if(ntokens==1&&!find_in_conv_table(curr_token)){
				printf("ERRO DE COMPILACAO!!!! Instruçao desconhecida '%s'!!!!!",curr_token);
				raise(SIGINT);
		}
		if((trip=find_in_label_table(curr_token))){
			ptr+=snprintf(ptr,128,"%d ",trip->addr);

		}
		else{
			ptr+=snprintf(ptr,128,"%s ",curr_token);

		}
			if(putsemicolon){
			ptr[-1]=';';
				break;
			}

		old_buff_ptr+=subst_cursor;
		memset(curr_token,0,128);
	}
	emplaceString(fpmid, newInstr,strlen(newInstr),curr_str_len,instr_pos);

}

void process_data(char buff[1024]){

	do{
	get_next_instruction(fpmid,buff);
	code_data_start=ftell(fpmid);
	}while(!strings_are_equal(buff,".data:"));
	
	code_code_start=code_data_start;
	while(1){
	get_next_instruction(fpmid,buff);
	code_code_start=ftell(fpmid);
	if(strings_are_equal(buff,".code:")){
		break;
	}
	name_addr_value_triple trip={NULL,0,0};
	sscanf(buff,"%ms%u",&trip.string,&trip.value);
	
	trip.addr=num_of_names;
	if(trip.string){

		printf("%s %u %d\n",trip.string,trip.addr, trip.value);
		avail_names[num_of_names]=trip;
	}
	else{
		printf("erro a parsear var\n");
	}
	num_of_names++;
	}
}
u_int32_t instr_buff_is_space(char buff[1024]){
	u_int32_t result=1;
	for(int i=0;(buff[i]!=';')&&(buff[i]!=':');i++){
		result=(result&&isspace(buff[i]));
	}
	return result;

}
void substitute_all_vars(void){
	char buff[1024];
	while(1){
	int instr_pos=get_next_instruction(fpmid,buff);
	if(instr_buff_is_space(buff)){
	continue;
	}
	if(feof(fpmid)||ferror(fpmid)){
		break;
	}
	substitute_vars(instr_pos,buff);
	}
	
}
void get_all_labels(void){
	char buff[1024];
	while(1){
	get_next_instruction(fpmid,buff);
	if(instr_buff_is_space(buff)){
		continue;
	}
	if(feof(fpmid)||ferror(fpmid)){
		break;
	}
	if(buff[strlen(buff)-1]==':'){
		buff[strlen(buff)-1]=0;
		name_addr_pair* p=&avail_labels[num_of_labels++];
		p->string=malloc(strlen(buff)+1);
		memset(p->string,0,strlen(buff)+1);
		strcpy(p->string,buff);
		p->addr=curr_code-1;
	}
	
	curr_code++;
	}
	printf("Labels:\n");
	for(u_int32_t i=0;i<num_of_labels;i++){
		printf("Label: nome: %s, address: %u\n",avail_labels[i].string,avail_labels[i].addr);
	}
	
	
}
void substitute_all_labels(void){
	char buff[1024];
	while(1){
	int instr_pos=get_next_instruction(fpmid,buff);
	if(instr_buff_is_space(buff)){

		continue;
	}
	if(feof(fpmid)||ferror(fpmid)){
		break;
	}
	substitute_labels(instr_pos,buff);
	}
	
}
void copyInputFile(FILE* fpin){

	char buff[1024]={0};
	int num_read=0;
	while((num_read=fread(buff,1,1024,fpin))){
		fwrite(buff,1,num_read,fpmid);
	}
	
}
void compile(cpu*proc,FILE* fpin,FILE* fpout){
	u_int32_t num_of_inst=0;
	if(!(fpmid=fopen(TMP_FILE_NAME,"w"))){
		perror("Error opening tmp file while compiling!!!!\n");
		return;
	}
	copyInputFile(fpin);
	fclose(fpmid);
	
	if(!(fpmid=fopen(TMP_FILE_NAME,"r+"))){
		perror("Error opening tmp file while compiling!!!!\n");
		return;
	}
	char buff[1024];
	process_data(buff);
	fseek(fpmid,code_code_start,SEEK_SET);
	memset(buff,1,1023);
	substitute_all_vars();
	fseek(fpmid,code_code_start,SEEK_SET);
	memset(buff,1,1023);
	get_all_labels();
	buff[1023]=0;
	fseek(fpmid,code_code_start,SEEK_SET);
	memset(buff,1,1023);
	substitute_all_labels();
	fseek(fpmid,code_code_start,SEEK_SET);
	while(strlen(buff)){
	get_next_instruction(fpmid,buff);
	num_of_inst++;
	}

	fseek(fpmid,code_code_start,SEEK_SET);
	for(u_int32_t i=0;i<num_of_inst;i++){
	get_next_instruction(fpmid,buff);
	u_int32_t inst=0x0;
	if(strlen(buff)){
	inst=process_instruction(proc,buff);
	if(inst){
	fprintf(fpout,"%x\n",inst);
	}
	}
	}
	for(u_int32_t i=0;i<num_of_names;i++){
		free(avail_names[i].string);
		
	}
	for(u_int32_t i=0;i<num_of_labels;i++){
		free(avail_labels[i].string);
		
	}
	fclose(fpmid);
	remove(TMP_FILE_NAME);
}
