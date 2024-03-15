#ifndef DEFAULTS_H
#define DEFAULTS_H

#define BYTE_BITS 8
#define WORD_SIZE sizeof(u_int32_t)
#define WORD_BITS (BYTE_BITS*WORD_SIZE)
#define MEM_WORDS 100
#define MEM_DEF_SIZE (WORD_SIZE*MEM_WORDS)

#define INSTR_FILE_PATH "resources/test1.cpu"
#define CPU_FILE_PATH "resources/test2.cpu"
#define CPU_DEC_PATH "resources/test4.cpu"
#define PROGRAM_FILE_PATH "resources/test3.cpu"
#define PROGRAM_FILE2_PATH "resources/test5.cpu"
#define TMP_FILE_NAME "tmp.tmp"

#define MAX_NUM_OF_WINDOWS 100
#define MAX_NUM_OF_PROCESSES 100

#define TIMEOUT_IN 0
#define TIMEOUT_IN_US 10
#endif
