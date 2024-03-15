#ifndef DECODER_H
#define DECODER_H

typedef enum {ALU,MEM,IMM,COND,CONTROL,STACK}instr_type;

typedef enum {ADD=0x10000000,
		LMEM=0x20000000,
			SUB=0x30000000,
			STO=0x40000000,
			LIMM=0x50000000,
			OR=0x60000000,
			AND=0x70000000,
			JMP=0x80000000,
			RET=0x90000000,
			CMP=0xa0000000,
			BZERO=0xb0000000,
			BNZERO=0xc0000000,
			PUSH=0xd0000000,
			POP=0xe0000000,
			LMEMR=0xf0000000
			}op_code;

#define OP_CODE_MASK 0xF0000000
typedef struct code_control_unit{
	
	u_int32_t z_flag_mask;
	
}code_control_unit;
typedef struct mmu{

	u_int32_t mem_reg_mask,
		mem_addr_reg_mask,
		mem_reg_type_mask,
		mem_size_mask;
	u_int32_t stack_reg_mask;

	u_int32_t jmp_addr_mask;

}mmu;
typedef struct alu{

	u_int32_t alu_oper_1_mask,
			alu_oper_2_mask,
			alu_dst_mask,
			alu_op_size_mask,
			alu_op2_size_mask,
			alu_dest_size_mask;

	u_int32_t cmp_reg_mask,
			cmp_value_mask;


}alu;

typedef struct decoder{

	u_int32_t op_code_mask;
	alu arith;
	mmu mem;
	code_control_unit ccu;
	u_int32_t load_imm_dst_mask,
		load_imm_oper_mask;


}decoder;
int load_cpu_masks(decoder*dec);
instr_type get_instr_type(op_code code);
#endif
