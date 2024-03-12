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
#include "../Includes/decoder.h"
#include "../Includes/compiler.h"

static u_int32_t code_code_start=0,code_data_start=0,curr_code=0,num_of_names=0,num_of_labels=0;
static FILE* fpmid=NULL;

static symbol conv_table[]={
					{"add",ADD,0},
					{"sub",SUB,0},
					{"or",OR,0},
					{"and",AND,0},
					{"load",LMEM,0},
					{"limm",LIMM,0},
					{"store",STO,0},
					{"jmp",JMP,0},
					{"ret",RET,0},
					{"cmp",CMP,0},
					{"bz",BZERO,0},
					{"bnz",BNZERO,0},
					{NULL,0,0},
			};

static symbol avail_names[MAX_NAMES]={0};

static symbol avail_labels[MAX_NAMES]={0};

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

static symbol* find_in_name_table(symbol table[],char* string){
	if(!string){
		printf("null string in compiler curr var finder!!!!\n");
		return NULL;
	}
	if(!strlen(string)){
		printf("empty string in compiler curr var finder!!!!\n");
		return NULL;
	}
	for(int i=0;table[i].string;i++){
		
		if(strings_are_equal(table[i].string,string)){
			return &table[i];
		}

	}
	return NULL;


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
static u_int32_t decypher_instruction(decoder*dec,u_int32_t code,char* buff){
	u_int32_t result=0x0;
	u_int32_t alu1=0,alu2=0,alu3=0,alu4=0,alu5=0,alu6=0;
	u_int32_t mem1=0,mem2=0,mem3=0,mem4=0;
	u_int32_t loadimm_reg=0,loadimm_value=0;
	u_int32_t cmp_reg=0,cmp_value=0;
	int16_t jmp_addr=0;
	int16_t bzero_addr=0;
	int16_t ret_off=0;
	instr_type type=get_instr_type(code);
	switch(type){
		case ALU:
		if(!sscanf(buff,"%u%u%u%u%u%u",&alu1,&alu2,&alu3,&alu4,&alu5,&alu6)){
			perror("Compiling error!!!!! bad instruction syntax in add!!!!\nNULL instruction loaded!\n");
			return result;
		}
		result|=code;
		result|=mask_photograph(dec->arith.alu_oper_1_mask,alu1);
		result|=mask_photograph(dec->arith.alu_oper_2_mask,alu2);
		result|=mask_photograph(dec->arith.alu_dst_mask,alu3);
	
		result|=mask_photograph(dec->arith.alu_op_size_mask,alu4);
		result|=mask_photograph(dec->arith.alu_op2_size_mask,alu5);
		result|=mask_photograph(dec->arith.alu_dest_size_mask,alu6);
		
			break;
		case MEM:
		if(!sscanf(buff,"%u%u%u%u",&mem1,&mem2,&mem3,&mem4)){
			perror("Compiling error!!!!! bad instruction syntax in store!!!!\nNULL instruction loaded!\n");
			return result;
		}
		result|=code;
		result|=mask_photograph(dec->mem.mem_reg_mask,mem1);
		result|=mask_photograph(dec->mem.mem_addr_reg_mask,mem2);
		result|=mask_photograph(dec->mem.mem_reg_type_mask,mem3);
		result|=mask_photograph(dec->mem.mem_size_mask,mem4);
			break;
		case IMM:
		if(!sscanf(buff,"%u%u",&loadimm_reg,&loadimm_value)){
			perror("Compiling error!!!!! bad instruction syntax in store!!!!\nNULL instruction loaded!\n");
			return result;
		}
		result|=code;
		result|=mask_photograph(dec->load_imm_dst_mask,loadimm_reg);
		result|=mask_photograph(dec->load_imm_oper_mask,loadimm_value);
		break;
		case COND:
		if(!sscanf(buff,"%u%u",&cmp_reg,&cmp_value)){
			perror("Compiling error!!!!! bad instruction syntax in store!!!!\nNULL instruction loaded!\n");
			return result;
		}
		result|=code;
		result|=mask_photograph(dec->arith.cmp_reg_mask,cmp_reg);
		result|=mask_photograph(dec->arith.cmp_value_mask,cmp_value);
		
			break;
		case CONTROL:
		if(!sscanf(buff,"%hd",&bzero_addr)){
			perror("Compiling error!!!!! bad instruction syntax in jmp!!!!\nNULL instruction loaded!\n");
			return result;
		}
		result|=code;
		result|=mask_photograph(dec->mem.jmp_addr_mask,bzero_addr);
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
static u_int32_t process_instruction(decoder*dec,char buff[1024]){
	char* ptr=buff;
	char inst_name[1024]={0};
	int num_consumed=0;
	sscanf(ptr,"%s%n",inst_name,&num_consumed);
	ptr+=num_consumed;
	symbol* sym;
	u_int32_t code=0x0;
	if((sym=find_in_name_table(conv_table,inst_name))){
	code=sym->value;
	}

	return decypher_instruction(dec,code,ptr);

}

void substitute_names(symbol table[],int instr_pos,char buff[1024],u_int32_t num,u_int32_t relative){
	
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
		symbol* trip=NULL;
		
		if(ntokens==1&&!find_in_name_table(conv_table,curr_token)){
				printf("ERRO DE COMPILACAO!!!! Instruçao desconhecida '%s'!!!!!",curr_token);
				raise(SIGINT);
		}

		if((trip=find_in_name_table(table,curr_token))){
		if(relative){

			ptr+=snprintf(ptr,128,"%d ",trip->value-num);
		}
		else{
			ptr+=snprintf(ptr,128,"%d ",trip->value);
		}
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
	symbol trip={NULL,0,0};
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
void get_all_labels(void){
	char buff[1024];
	while(1){
	if(feof(fpmid)||ferror(fpmid)){
		break;
	}
	get_next_instruction(fpmid,buff);
	if(instr_buff_is_space(buff)){
		continue;
	}
	if(buff[strlen(buff)-1]==':'){
		buff[strlen(buff)-1]=0;
		symbol* p=&avail_labels[num_of_labels++];
		p->string=malloc(strlen(buff)+1);
		memset(p->string,0,strlen(buff)+1);
		strcpy(p->string,buff);
		p->value=curr_code-1;
	}
	
	curr_code++;
	}
	printf("Labels:\n");
	for(u_int32_t i=0;i<num_of_labels;i++){
		printf("Label: nome: %s, address: %u\n",avail_labels[i].string,avail_labels[i].value);
	}
	
	
}
void substitute_all_names(symbol table[],u_int32_t relative){
	char buff[1024];
	int num=0;
	while(1){
	if(feof(fpmid)||ferror(fpmid)){
		break;
	}
	int instr_pos=get_next_instruction(fpmid,buff);
	if(instr_buff_is_space(buff)){

		continue;
	}
	substitute_names(table,instr_pos,buff,num,relative);
	num++;
	}

	
}
void copyInputFile(FILE* fpin){

	char buff[1024]={0};
	int num_read=0;
	while((num_read=fread(buff,1,1024,fpin))){
		fwrite(buff,1,num_read,fpmid);
	}
	
}
void compile(decoder*dec,FILE* fpin,FILE* fpout){
	u_int32_t num_of_inst=0;

	//remove(TMP_FILE_NAME);
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
	substitute_all_names(avail_names,0);
	fseek(fpmid,code_code_start,SEEK_SET);
	memset(buff,1,1023);
	get_all_labels();
	buff[1023]=0;
	fseek(fpmid,code_code_start,SEEK_SET);
	memset(buff,1,1023);
	substitute_all_names(avail_labels,1);
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
	inst=process_instruction(dec,buff);
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
}
