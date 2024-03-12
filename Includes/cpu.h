#ifndef CPU_H
#define CPU_H
#define REG_FILE_DEF_SIZE 32


typedef enum {NONE,QUARTER,HALF,FULL}reg_type;
typedef struct reg_window{

	u_int32_t win_start,win_size;

}reg_window;
typedef struct cpu{

	u_int8_t* reg_file;
	u_int32_t reg_file_size;
	memory*mem;
	u_int32_t status_word;
	u_int32_t curr_pc,prev_pc;
	
	decoder dec;
	
	reg_window* wins;
	
	u_int8_t curr_wp;


}cpu;
void execute(cpu*proc,u_int32_t inst);
void decodeInstruction(cpu*proc,u_int32_t inst);
u_int32_t getProcRegValue(cpu* proc,u_int32_t regIndex,u_int8_t reg_size);
void storeValueReg(cpu* proc, u_int32_t base,reg_type type,u_int32_t value,u_int8_t reg_addr);
void loadMemValue(cpu* proc,u_int32_t base,u_int32_t basemem,reg_type type,u_int8_t reg_addr);
void storeMemValue(cpu* proc,u_int32_t base,u_int32_t basemem,reg_type type,u_int8_t reg_addr);
#endif
