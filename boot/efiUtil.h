#pragma once

#include <kernel.h>
#include "boot.h"

void PrintEfiMemoryMap(struct MemoryMap *memory_map);
void PrintAddressMemoryType(EFI_PHYSICAL_ADDRESS address, struct MemoryMap *memory_map);
void GetGDTR(Gdtr *gdtr);