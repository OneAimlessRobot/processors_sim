#ifndef MEMORY_H
#define MEMORY_H
typedef struct memory{

	u_int8_t* contents;
	u_int32_t size;

}memory;

memory* spawnMemory(void);
void storeValue(memory* mem,u_int32_t base,u_int32_t varSize,void* value);
void loadValue(memory* mem,u_int32_t base,u_int32_t varSize,void* var);

void initMemory(FILE* fp,memory* mem);

void endMemory(memory** mem);
void printMemory(int fd,memory* mem);
#endif
