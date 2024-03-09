#ifndef CPU_H
#define CPU_H
#define REG_FILE_DEF_SIZE 32

typedef enum {FULL,HALF,QUARTER}reg_type;

typedef enum {ADD=0x10000000,
		LMEM=0x20000000,
			SUB=0x30000000,
			STO=0x40000000,
			LIMM=0x50000000,
			OR=0x60000000,
			AND=0x70000000,
			JMP=0x80000000,
			RET=0x90000000,
			LAB=0xa0000000,
			CMP=0xb0000000,
			BZERO=0xc0000000
			}op_code;

#define OP_CODE_MASK 0xF0000000

typedef struct cpu{

	u_int8_t* reg_file;
	u_int32_t reg_file_size;
	memory*mem;
	u_int32_t bz_flag;
	u_int32_t curr_pc,prev_pc;
	u_int32_t op_code_mask;
	u_int32_t alu_oper_1_mask,
			alu_oper_2_mask,
			alu_dst_mask,
			alu_op_size_mask;

	u_int32_t load_imm_dst_mask,
			load_imm_oper_mask;

	u_int32_t mem_reg_mask,
			mem_addr_reg_mask,
			mem_reg_type_mask,
			mem_size_mask;

	u_int32_t jmp_addr_mask;

	
	u_int32_t cmp_reg_mask,
			cmp_value_mask;

}cpu;


void switchOnCPU(cpu*proc);
cpu* spawnCPU(memory*mem);
void endCPU(cpu** processor);
void printCPU(int fd,cpu* processor);
void decodeInstruction(cpu*proc,u_int32_t inst);
u_int32_t getProcRegValue(cpu* proc,u_int8_t regIndex);
void storeValueReg(cpu* proc, u_int8_t base,reg_type type,u_int32_t value,u_int8_t reg_addr);
void loadMemValue(cpu* proc,u_int8_t base,u_int32_t basemem,reg_type type,u_int8_t reg_addr);
#endif
