#ifndef DEFAULTS_H
#define DEFAULTS_H

#define BYTE_BITS 8
#define WORD_SIZE sizeof(u_int32_t)
#define WORD_BITS (BYTE_BITS*WORD_SIZE)
#define MEM_WORDS 50
#define MEM_DEF_SIZE (WORD_SIZE*MEM_WORDS)

//load imm masks
#define LOAD_IMM_DST 0x0FF00000
#define LOAD_IMM_OPER 0x000FFFFF


#define INSTR_FILE_PATH "resources/test1.cpu"
#define CPU_FILE_PATH "resources/test2.cpu"
#define PROGRAM_FILE_PATH "resources/test3.cpu"
#define TMP_FILE_NAME "tmp.tmp"

#endif
