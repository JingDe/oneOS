#include"idt.h"
#include"string.h"
#include"debug.h"
#include"vmm.h"
#include"pmm.h"

pgd_t pdg_kern[PGD_SIZE] 
