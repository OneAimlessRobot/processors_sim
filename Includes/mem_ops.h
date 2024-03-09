#ifndef MEM_OPS_H
#define MEM_OPS_H
//sto mem masks
#define REG1 0x0FF00000
#define STO_DST_ADDR_REG   0x000FF000
#define STO_DST_STORE_REG_TYPE 0x00000030
#define STO_DST_STORE_SIZE     0x00000003

//load mem mem masks
#define LOAD_REG 0x0FF00000
#define LOAD_SRC_ADDR_REG   0x000FF000
#define LOAD_SRC_LOAD_REG_TYPE 0x00000030
#define LOAD_SRC_LOAD_SIZE     0x00000003

void sto(cpu*proc,u_int32_t inst);
void load(cpu*proc,u_int32_t inst);
#endif
