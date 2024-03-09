#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>
#include "../Includes/defaults.h"
#include "../Includes/helper.h"
#include "../Includes/memory.h"


memory* spawnMemory(void){
        memory* result= malloc(sizeof(memory));
        result->contents=(u_int8_t*)malloc((result->size=MEM_DEF_SIZE));
        memset(result->contents,0,result->size);
        return result;
}
void endMemory(memory** mem){

        free((*mem)->contents);
        (*mem)->contents=NULL;
        (*mem)->size=0;
        free(*mem);
        *mem=NULL;


}

void initMemory(FILE* fp,memory* mem){
	u_int32_t value;
	u_int32_t curr_cell=0;
	while((fscanf(fp,"%x",&value)==1)&&(curr_cell<(mem->size/WORD_SIZE))){
		//value>>=firstBitOne(value);
		storeValue(mem,curr_cell*WORD_SIZE,(u_int32_t)sizeof(value),(void*)&value);
		curr_cell++;
	}


}
static u_int32_t getMemoryWord(memory*mem,u_int32_t index){
	return *(u_int32_t*)(&mem->contents[index*WORD_SIZE]);

}
void printMemory(int fd,memory* mem){
        dprintf(fd,"State of this memory:\nSize: %u\n",mem->size);
        for(u_int32_t i=0;i<mem->size/WORD_SIZE;i++){
        	dprintf(fd,"Line %u: [",i);
	        printWord(fd, getMemoryWord(mem,i));
		dprintf(fd,"]\n");
        }


}
void storeValue(memory* mem, u_int32_t base, u_int32_t varSize, void* value) {
    
    dprintf(1,"Endereço destino: %u\nValor: %u\n",base,*(u_int32_t*)value);
    memcpy(mem->contents + base, value, varSize);
}
void loadValue(memory* mem,u_int32_t base,u_int32_t varSize,void* var){

    memcpy(var ,mem->contents + base, varSize);


}

