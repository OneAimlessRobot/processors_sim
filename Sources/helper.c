#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include "../Includes/defaults.h"
#include "../Includes/helper.h"

int strings_are_equal(char* str1,char* str2){

         int same_length=!(strlen(str1)-strlen(str2));

         int same_content= !strncmp(str1,str2,strlen(str1));
         return same_content&&same_length;
}

void printWordNcurses(u_int32_t line){
 u_int32_t mask=0x80000000;
        for(u_int32_t i=0;i<WORD_BITS;i++){
                if(((mask&line)==mask)){
                printw("1");
                }
                else{
                printw("0");
                }
                mask>>=1;
        }

}
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
	char buff[BUFFSIZE]={0};
	char* result=NULL;
	while(isspace((curr_char=fgetc(fp)))&&curr_char!=EOF);
	if(curr_char==EOF){
		usleep(1000000);
		return;
	}
	printf("\nValor de EOF: %d\nValor de curr_char: %d\n",EOF,curr_char);
	//ungetc(curr_char,fp);
	//if(curr_char=='/'){
	//
		//if(((curr_char=fgetc(fp))=='/')&&curr_char!=EOF){
		/*result=fgets(buff,strlen(COMMENT_STRING)+1,fp);
		buff[strlen(COMMENT_STRING)]=0;
		if(!feof(fp)&&strings_are_equal(buff,COMMENT_STRING)){
			printf("\nComment string: %s\nString lida: '%s'\n",COMMENT_STRING,buff);
			//printf("\nComment string: %s\n",COMMENT_STRING);
			for(size_t i=strlen(COMMENT_STRING);i>0;i--){
				
				ungetc(buff[i-1],fp);
			}
			
			while(((curr_char=fgetc(fp))!='\n')&&curr_char!=EOF);
			ungetc(curr_char,fp);

		}*/
		if(((curr_char=fgetc(fp))==';')&&curr_char!=EOF){
		//if(((curr_char=fgetc(fp))==';')){
			ungetc(curr_char,fp);
			while(((curr_char=fgetc(fp))!='\n')&&curr_char!=EOF);
			if(curr_char==EOF){
				usleep(1000000);
				return;
			}
			ungetc(curr_char,fp);
		}

	//}

	while(isspace((curr_char=fgetc(fp)))&&curr_char!=EOF);
	if(curr_char==EOF){
		usleep(1000000);
		return;
	}
	ungetc(curr_char,fp);
}

