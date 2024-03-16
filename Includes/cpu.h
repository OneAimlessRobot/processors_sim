#ifndef CPU_H
#define CPU_H

struct os;
typedef enum {NONE,HALF,QUARTER}reg_type;
typedef struct reg_window{

	u_int32_t win_start,win_size;

}reg_window;
typedef struct cpu{

	u_int8_t* reg_file;
	u_int32_t reg_file_size;
	u_int32_t status_word;
	u_int32_t instr_reg;
	u_int32_t stack_pointer;
	u_int32_t frame_pointer;
	u_int32_t curr_pc,prev_pc;
	
	decoder dec;
	decoder* dec2;
	reg_window* wins;
	
	u_int8_t curr_wp;
	
	struct os* running_system;

}cpu;
void execute(cpu*proc);
void decodeInstruction(cpu*proc);
void pushValue(cpu* proc,u_int32_t base,reg_type type,u_int8_t reg_addr);
void popValue(cpu* proc,u_int32_t base,reg_type type,u_int8_t reg_addr);

int32_t getProcRegValue(cpu* proc,u_int32_t regIndex,u_int8_t reg_size);
void storeValueReg(cpu* proc, u_int32_t base,reg_type type,u_int32_t value,u_int8_t reg_addr);
void loadMemValue(cpu* proc,u_int32_t base,u_int32_t basemem,reg_type type,u_int8_t reg_addr);
void storeMemValue(cpu* proc,u_int32_t base,u_int32_t basemem,reg_type type,u_int8_t reg_addr);
void makeCall(cpu* proc,u_int32_t addr);
void makeReturn(cpu* proc);
#endif
