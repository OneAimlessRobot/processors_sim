#ifndef OS_H
#define OS_H

typedef struct context{
	u_int32_t proc_id;
	u_int32_t proc_data_start,proc_code_start,proc_allocd_start,proc_allocd_end;
	u_int32_t proc_data_size,proc_allocd_size;
	u_int8_t* reg_state;
        u_int32_t instr_reg;
        u_int32_t stack_pointer;
	u_int32_t frame_pointer;
	u_int32_t curr_pc;
	u_int32_t prev_pc;
	u_int32_t status_word;
}context;
typedef struct p_table{
	context** processes;
	u_int32_t num_of_processes;
	
}p_table;
typedef struct os{

	p_table proc_vec;
	u_int32_t curr_process;
	cpu* proc;
	memory* mem;
	u_int32_t avail_memory;
}os;

void switchOnCPU(int fd,os* system);

os* spawnOS(void);
void endOS(os** system);

void printMemory(int fd,memory* mem);
void loadProg(FILE* progfile,os*system);
#endif
