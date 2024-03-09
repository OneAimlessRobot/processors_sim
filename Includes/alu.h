#ifndef ALU_H
#define ALU_H
//alu masks
#define ALU_OPER_1       0x0FF00000
#define ALU_OPER_2       0x000FF000
#define ALU_DST          0x00000FF0
#define ALU_OPER_SIZE    0x00000003

void add(cpu*proc,u_int32_t inst);
void sub(cpu*proc,u_int32_t inst);
void or(cpu*proc,u_int32_t inst);
void and(cpu*proc,u_int32_t inst);
#endif
