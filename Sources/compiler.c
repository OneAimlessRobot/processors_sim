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

static int32_t curr_code_2=0, data_size=0,code_code_start=0,code_data_start=0,code_macro_start=0,curr_code=0,num_of_names=0,num_of_labels=0,num_of_macros=0;

FILE* fpmid=NULL;
static decoder* priv_dec=NULL;
static char* data_types_table[]={
			"byte",
			"short",
			"word",
			NULL
			};

static symbol avail_names[MAX_NAMES]={0};

static symbol avail_labels[MAX_NAMES]={0};

static macro avail_macros[MAX_NAMES]={0};

static u_int32_t mask_photograph(u_int32_t mask,u_int32_t value){
	u_int8_t advance=firstBitOne(mask);
	u_int8_t retreat=firstBitOne(mask);
	u_int32_t result=0x0;
	mask>>=retreat;
	result|=(value&mask);
	result<<=advance;
	return result;
	

}

static void cleanup(void){

	for(int32_t i=0;i<num_of_names;i++){
		free(avail_names[i].string);
		
	}
	for(int32_t i=0;i<num_of_labels;i++){
		free(avail_labels[i].string);
		
	}
	/*for(int32_t i=0;i<num_of_macros;i++){
		free(avail_macros[i].macro_tag);
		free(avail_macros[i].macro_text);
	}*/



}

static void load_generic_data(char* ptr,FILE* fp,int counting_mode,u_int8_t num_parts_to_split_word){
	
	u_int32_t shift_ammount=WORD_BITS/num_parts_to_split_word;
	u_int32_t init_mask=0xFFFFFFFF>>shift_ammount;
	u_int32_t mask=init_mask;
	u_int32_t advance=0;
	u_int32_t value=0x0;
	u_int32_t curr_word=0x0;
	u_int32_t i=0;
	while(1){
		for(;i<num_parts_to_split_word;i++){
		if(!sscanf(ptr,"%u%n",&value,&advance)){
			
			break;
		}
		if(!counting_mode){
		
		curr_word|=mask_photograph(mask,value);
		
		mask<<=shift_ammount;
		}
		ptr+=advance;
		if(!strlen(ptr)){
			break;
		}
		}
		if(i){
		if(!counting_mode){
			fwrite(&curr_word,sizeof(u_int32_t),1,fp);
			curr_code++;
		}
		else{

		data_size++;
		}
		}
		if(i<num_parts_to_split_word||!i){

			return;
		}
		i=0;
		curr_word=0x0;
		mask=init_mask;
	}
	
	


}

static char* find_in_string_arr(char* arr[],char* string){
	if(!string){
		return NULL;
	}
	if(!strlen(string)){
		return NULL;
	}
	for(u_int32_t i=0;arr[i];i++){
		if(strings_are_equal(arr[i],string)){

			return arr[i];
		}
		

	}
	
	return NULL;

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
static macro* find_in_macro_table(macro table[],char* string){
	if(!string){
		printf("null string in compiler curr var finder!!!!\n");
		return NULL;
	}
	if(!strlen(string)){
		printf("empty string in compiler curr var finder!!!!\n");
		return NULL;
	}
	for(int i=0;strnlen(table[i].macro_tag,MAX_MACRO_TEXT_SIZE);i++){
		
		if(strings_are_equal(table[i].macro_tag,string)){
			return &table[i];
		}

	}
	return NULL;


}
static int32_t get_next_instruction(FILE* fp,char buff[COMPILER_BUFFSIZE]){
	
	skip_cpu_comments(fp);
	int32_t init_of_instr=ftell(fp);
	int curr_char=0;
	u_int64_t curr_char_index=0;
	memset(buff,0,COMPILER_BUFFSIZE);
	while((curr_char=fgetc(fp))!=':'&&(curr_char)!='\n'&&curr_char !=EOF){
		buff[curr_char_index++]=(char)curr_char;
	}
	if(curr_char==EOF){
	memset(buff,0,COMPILER_BUFFSIZE);
	return init_of_instr;
	}
	buff[curr_char_index]=curr_char;
	skip_cpu_comments(fp);
	return init_of_instr;
	
}
static u_int32_t decypher_instruction(u_int32_t code,instr_info *sym,char* buff){
	u_int32_t result=0x0;
	result|=code;
	char* ptr= buff;
	
	for(u_int32_t i=0;i<sym->arg_num;i++){
		int16_t arg=0x0;
		int advance=0;
		if(!sscanf(ptr,"%hd%n",&arg,&advance)){
			
			dprintf(1,"Compiling error!!!!! bad instruction syntax in instruction of code %x!!!!\nNULL instruction loaded!\n",code);
			return result;
		}
		if(code==LMEMR){

		dprintf(1,"Arg neste load relativo: %hd\n Numero de argumentos: %u\nIteraçao neste instrucao: %u\n",arg,sym->arg_num,i);
		}
		result|=mask_photograph(sym->masks[i],arg);
		ptr+=advance;
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
static u_int32_t process_instruction(decoder*dec,char buff[COMPILER_BUFFSIZE]){
	char* ptr=buff;
	char instr_name[COMPILER_BUFFSIZE]={0};
	int num_consumed=0;
	
	sscanf(ptr,"%s%n",instr_name,&num_consumed);
	ptr+=num_consumed;
	instr_info* sym=NULL;
	u_int32_t code=0x0;
	if(!(sym=find_instr_with_name(dec,instr_name))){
		return 0;
	}
	code=get_op_code_inside_group(sym,instr_name);
	return decypher_instruction(code,sym,ptr);

}

static void substitute_macros(macro table[],int instr_pos,char buff[COMPILER_BUFFSIZE]){
	
	int curr_str_len=strlen(buff);
	if(!curr_str_len){
		return;
	}
	int subst_cursor=0;
	int ntokens=0;
	char newInstr[COMPILER_BUFFSIZE]={0};
	char* ptr=newInstr;
	char* old_buff_ptr=buff;
	char curr_token[128]={0};
	while(1){

	if(sscanf(old_buff_ptr,"%s%n",curr_token,&subst_cursor)>0){
		ntokens++;
	}if(!strlen(curr_token)){
		newInstr[strlen(newInstr)]='\n';
		break;
	}
	
	if(curr_token[strlen(curr_token)-1]==':'){

		return;
	}
		macro* trip=NULL;
		
		if(ntokens==1&&!find_instr_with_name(priv_dec,curr_token)){
		

				cleanup();
				printf("ERRO DE COMPILACAO!!!! Instruçao desconhecida '%s'!!!!!",curr_token);
				raise(SIGINT);
		}

		printf("Token: %s\n",curr_token);
		if((trip=find_in_macro_table(table,curr_token))){
			ptr+=snprintf(ptr,128,"%s ",trip->macro_text);
			printf("Replaced: %s\nWith: %s\n",curr_token,trip->macro_text);
		}
		else {
			ptr+=snprintf(ptr,128,"%s ",curr_token);
		}
		old_buff_ptr+=subst_cursor;
		memset(curr_token,0,128);
	}
	
	emplaceString(fpmid, newInstr,strlen(newInstr),curr_str_len,instr_pos);

}
static void substitute_names(symbol table[],int instr_pos,char buff[COMPILER_BUFFSIZE],int32_t num,int32_t relative){
	
	int curr_str_len=strlen(buff);
	if(!curr_str_len){
		return;
	}
	int subst_cursor=0;
	int ntokens=0;
	char newInstr[COMPILER_BUFFSIZE]={0};
	char* ptr=newInstr;
	char* old_buff_ptr=buff;
	char curr_token[128]={0};
	while(1){

	if(sscanf(old_buff_ptr,"%s%n",curr_token,&subst_cursor)>0){
		ntokens++;
	}if(!strlen(curr_token)){
		newInstr[strlen(newInstr)]='\n';
		break;
	}
	
	if(curr_token[strlen(curr_token)-1]==':'){

		return;
	}
		symbol* trip=NULL;
		
		if(ntokens==1&&!find_instr_with_name(priv_dec,curr_token)){
				
				cleanup();
				printf("ERRO DE COMPILACAO!!!! Instruçao desconhecida '%s'!!!!!",curr_token);
				raise(SIGINT);
		}

		if((trip=find_in_name_table(table,curr_token))){
		if(relative){
			ptr+=snprintf(ptr,128,"%d ",trip->addr-num);
		}
		else{
			ptr+=snprintf(ptr,128,"%d ",trip->addr);
		}
		}
		else{
			
			ptr+=snprintf(ptr,128,"%s ",curr_token);

		}
	
		old_buff_ptr+=subst_cursor;
		memset(curr_token,0,128);
	}
	
	emplaceString(fpmid, newInstr,strlen(newInstr),curr_str_len,instr_pos);

}
static void load_names(char buff[COMPILER_BUFFSIZE],FILE*fpout){
while(1){
	
	get_next_instruction(fpmid,buff);
	code_code_start=ftell(fpmid);
	if(strings_are_equal(buff,CODE_STRING)){
		break;
	}
	symbol trip={NULL,0,0};
	trip.addr=curr_code;
	char* ptr=buff;
	int advance=0;
	char* data_type_str=NULL;
	sscanf(buff,"%ms%ms%n",&trip.string,&data_type_str,&advance);
	ptr+=advance;
	if(strings_are_equal(data_type_str,"byte")){

		load_generic_data(ptr,fpout,0,4);
	}
	else if(strings_are_equal(data_type_str,"short")){

		load_generic_data(ptr,fpout,0,2);
	}
	else if(strings_are_equal(data_type_str,"word")){

		load_generic_data(ptr,fpout,0,1);
	}
	if(trip.string){

		printf("data: %s %u\n",trip.string,trip.addr);
		avail_names[num_of_names]=trip;
	
	}
	else{
		printf("erro a parsear var\n");
	}
	if(data_type_str){
		free(data_type_str);
	}
	num_of_names++;
	}


}
static void load_macros(char buff[COMPILER_BUFFSIZE]){
while(1){
	
	get_next_instruction(fpmid,buff);
	code_code_start=ftell(fpmid);
	if(strings_are_equal(buff,DATA_STRING)){
		break;
	}
	macro mac={0};
	//scanf("\"%[^\"]\"",s);
	sscanf(buff,"%s%s",mac.macro_tag,mac.macro_text);
	if(strnlen(mac.macro_tag,MAX_MACRO_TEXT_SIZE)&&strnlen(mac.macro_text,MAX_MACRO_TEXT_SIZE)){

		avail_macros[num_of_macros]=mac;
		printf("macro: %s %s\n",avail_macros[num_of_macros].macro_tag,avail_macros[num_of_macros].macro_text);
		
	}
	else{
		printf("erro a parsear macro: %s %s\n",mac.macro_tag,mac.macro_text);
	}
	num_of_macros++;
	}


}
static void measure_data_size(char buff[COMPILER_BUFFSIZE],FILE*fpout){
while(1){
	
	get_next_instruction(fpmid,buff);
	code_code_start=ftell(fpmid);
	if(strings_are_equal(buff,CODE_STRING)){
		break;
	}
	char* string;
	char* ptr=buff;
	int advance=0;
	char* data_type_str=NULL;
	sscanf(buff,"%ms%ms%n",&string,&data_type_str,&advance);
	ptr+=advance;
	if(!find_in_string_arr(data_types_table,data_type_str)){

		cleanup();
		dprintf(1,"Erro de compilaçao: Tipo desconhecido '%s'.",data_type_str);
		raise(SIGINT);
	}
	if(strings_are_equal(data_type_str,"byte")){

		load_generic_data(ptr,fpout,1,4);
	}
	else if(strings_are_equal(data_type_str,"short")){

		load_generic_data(ptr,fpout,1,2);
	}
	else if(strings_are_equal(data_type_str,"word")){

		load_generic_data(ptr,fpout,1,1);
	}
	if(string){

		free(string);
	}
	if(data_type_str){
		free(data_type_str);
	}
}

}
void process_data(char buff[COMPILER_BUFFSIZE],FILE* fpout){
	
	
	do{
	get_next_instruction(fpmid,buff);
	code_data_start=ftell(fpmid);
	printf("buff em process_data: %s\n",buff);
	}while(!strings_are_equal(buff,DATA_STRING)&&!strings_are_equal(buff,CODE_STRING)&&!feof(fpmid));
	int init_data_pos=ftell(fpmid);
	code_code_start=code_data_start;
	measure_data_size(buff,fpout);
	fseek(fpmid,init_data_pos,SEEK_SET);
	fwrite(&data_size,sizeof(u_int32_t),1,fpout);
	load_names(buff,fpout);
}
void process_macros(char buff[COMPILER_BUFFSIZE]){
	
	
	do{
	get_next_instruction(fpmid,buff);
	code_macro_start=ftell(fpmid);
	printf("buff em process_macros: %s\n",buff);
	}while(!strings_are_equal(buff,MACRO_STRING)&&!strings_are_equal(buff,DATA_STRING)&&!strings_are_equal(buff,CODE_STRING)&&!feof(fpmid));
	int init_macro_pos=ftell(fpmid);
	code_data_start=code_macro_start;
	fseek(fpmid,init_macro_pos,SEEK_SET);
	load_macros(buff);
	printf("Numero de macros: %d\n",num_of_macros);
}
u_int32_t instr_buff_is_space(char buff[COMPILER_BUFFSIZE]){
	u_int32_t result=1;
	for(u_int32_t i=0;(i<strlen(buff))&&(buff[i]!=':')&&(buff[i]!='\n');i++){
		result=(result&&isspace(buff[i]));
	}
	return result;

}
void get_all_labels(void){
	char buff[COMPILER_BUFFSIZE];
	while(1){
	if(feof(fpmid)||ferror(fpmid)){
		break;
	}
	get_next_instruction(fpmid,buff);
	if(instr_buff_is_space(buff)){
		perror("EMPTY LABEL NAME!!!!!\n");
		raise(SIGINT);
	}
	if(buff[strlen(buff)-1]==':'){
	
		buff[strlen(buff)-1]=0;
		symbol* p=&avail_labels[num_of_labels++];
		p->string=malloc(strlen(buff)+1);
		memset(p->string,0,strlen(buff)+1);
		strcpy(p->string,buff);
		p->addr=curr_code;
	}
	curr_code++;
	}
	printf("Labels:\n");
	for(int32_t i=0;i<num_of_labels;i++){
		printf("Label: nome: %s, address: %u\n",avail_labels[i].string,avail_labels[i].addr);
	}
	
	
}
void substitute_all_names(symbol table[],u_int32_t relative){
	char buff[COMPILER_BUFFSIZE];
	while(1){
	if(feof(fpmid)||ferror(fpmid)){
		break;
	}
	int instr_pos=get_next_instruction(fpmid,buff);
	if(instr_buff_is_space(buff)){

	}
	else{
	substitute_names(table,instr_pos,buff,curr_code_2,relative);
	curr_code_2++;
	}
	}

	
}
void substitute_all_macros(macro table[]){
	char buff[COMPILER_BUFFSIZE];
	while(1){
	if(feof(fpmid)||ferror(fpmid)){
		break;
	}
	int instr_pos=get_next_instruction(fpmid,buff);
	if(instr_buff_is_space(buff)){

	}
	else{
	substitute_macros(table,instr_pos,buff);
	curr_code_2++;
	}
	}

	
}
void copyInputFile(FILE* fpin){

	char buff[COMPILER_BUFFSIZE]={0};
	int num_read=0;
	while((num_read=fread(buff,1,COMPILER_BUFFSIZE,fpin))){
		fwrite(buff,1,num_read,fpmid);
	}
	
}
void compile(decoder*dec,FILE* fpin,FILE* fpout){
	u_int32_t num_of_inst=0;
	priv_dec=dec;
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
	char buff[COMPILER_BUFFSIZE];
	process_macros(buff);
	fseek(fpmid,code_data_start,SEEK_SET);
	memset(buff,1,COMPILER_BUFFSIZE-1);
	process_data(buff,fpout);
	fseek(fpmid,code_code_start,SEEK_SET);
	memset(buff,1,COMPILER_BUFFSIZE-1);
	u_int32_t prog_start=curr_code_2=curr_code;

	substitute_all_macros(avail_macros);
	fseek(fpmid,code_code_start,SEEK_SET);
	memset(buff,1,COMPILER_BUFFSIZE-1);

	substitute_all_names(avail_names,0);
	fseek(fpmid,code_code_start,SEEK_SET);
	memset(buff,1,COMPILER_BUFFSIZE-1);
	curr_code=curr_code_2=prog_start;
	get_all_labels();
	buff[COMPILER_BUFFSIZE-1]=0;
	fseek(fpmid,code_code_start,SEEK_SET);
	memset(buff,1,COMPILER_BUFFSIZE-1);
	curr_code=curr_code_2;
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
	if(strlen(buff)&&((buff[strlen(buff)-1]=='\n')||(buff[strlen(buff)-1]==':'))){
	inst=process_instruction(dec,buff);
	fwrite(&inst,sizeof(u_int32_t),1,fpout);
	}
	}
	cleanup();

	fclose(fpmid);
	fpmid=NULL;
}
