#ifndef XC_H
#define XC_H

typedef unsigned long _paddr_t; /* a physical address */
#define KVA_TO_PA(v)    ((_paddr_t)(v) & 0x1fffffff)

// TODO: include relevant file with SREGS defs here

#include "p32mx250f128b.h"

#endif
