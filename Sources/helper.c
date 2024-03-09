#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include "../Includes/defaults.h"
#include "../Includes/helper.h"

void printWord(int fd,u_int32_t line){
	
	u_int32_t mask=0x80000000;
        for(u_int32_t i=0;i<WORD_BITS;i++){
                if(((mask&line)==mask)){
                dprintf(fd,"1");
                }
                else{
                dprintf(fd,"0");
                }
                mask>>=1;
        }
}
int32_t firstBitOne(u_int32_t line){
	
	u_int32_t mask=0x00000001;
        for(u_int32_t i=0;i<WORD_BITS;i++){
                if(((mask&line)==mask)){
                	return i;
		}
                mask<<=1;
        }
	return -1;
}
int32_t lastBitOne(u_int32_t line){
	
	u_int32_t mask=0x10000000;
        for(int32_t i=BYTE_BITS*WORD_SIZE;i>0;i--){
                if(((mask&line)==mask)){
                	return i;
		}
                mask>>=1;
        }
	return -1;
}
void skip_cpu_comments(FILE* fp){

	int curr_char=0;
	while(isspace((curr_char=fgetc(fp)))&&curr_char!=EOF);
	ungetc(curr_char,fp);
	if(curr_char=='/'){
		if(((curr_char=fgetc(fp))=='/')&&curr_char!=EOF){
			ungetc(curr_char,fp);
			while(((curr_char=fgetc(fp))!='\n')&&curr_char!=EOF);
			ungetc(curr_char,fp);
		}

	}

	while(isspace((curr_char=fgetc(fp)))&&curr_char!=EOF);
	ungetc(curr_char,fp);
}

