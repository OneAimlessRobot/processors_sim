#ifndef MEMORY_H
#define MEMORY_H
typedef struct memory{

	u_int8_t* contents;
	u_int32_t size;

}memory;

void storeValue(memory* mem,u_int32_t base,u_int32_t varSize,void* value);
void loadValue(memory* mem,u_int32_t base,u_int32_t varSize,void* var);
void initMemory(FILE* fp,memory* mem);

#endif
