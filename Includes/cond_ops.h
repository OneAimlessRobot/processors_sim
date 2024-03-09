#ifndef COND_OPS_H
#define COND_OPS_H
void jmp(cpu*proc,u_int32_t inst);
void ret(cpu*proc,u_int32_t inst);
void label(cpu*proc,u_int32_t inst);
void cmp(cpu*proc,u_int32_t inst);
void bz(cpu*proc,u_int32_t inst);
#endif
