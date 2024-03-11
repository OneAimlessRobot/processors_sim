#ifndef OS_H
#define OS_H
typedef struct context{
	u_int32_t proc_id;
	u_int32_t proc_data_start,proc_code_start,proc_allocd_start,proc_allocd_end;
	u_int32_t proc_data_size,proc_allocd_size;
	u_int8_t* reg_state;
	
}context;
typedef struct os{

	context** process;
	

}os;

context* spawnCtx(cpu*proc,u_int32_t id,u_int32_t data_start,u_int32_t data_size,u_int32_t allocd_size);

void endCtx(context**ctx);

memory* spawnMemory(void);
void endMemory(memory** mem);
void printMemory(int fd,memory* mem);

void switchOnCPU(cpu*proc,context* prog);

cpu* spawnCPU(memory*mem);
void endCPU(cpu** processor);

void loadProg(FILE* progfile,cpu*proc,context* ctx);
void printCPU(int fd,cpu* processor,context* c);

#endif
