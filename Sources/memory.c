#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>
#include "../Includes/defaults.h"
#include "../Includes/helper.h"
#include "../Includes/memory.h"
void storeValue(memory* mem, u_int32_t base, u_int32_t varSize, void* value) {
    
    memcpy(mem->contents + base, value, varSize);
}
void loadValue(memory* mem,u_int32_t base,u_int32_t varSize,void* var){

    memcpy(var ,mem->contents + base, varSize);


}

