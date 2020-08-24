#include <efi.h>
#include <efilib.h>

#include "kernel.h"
#include "boot.h"

#define ENUM_TO_STRING_SWITCH(enum) \
	case enum:                      \
		return L"" #enum;           \
		break;

CHAR16 *EfiTypeToString(UINT32 type);

void PrintEfiMemoryMap(struct MemoryMap *memory_map) {
	Print(L"Num Memory Map Location: %x\n", memory_map);
	Print(L"Number of Entry: %d\n", memory_map->no_entries);

    Print(L"Given size %d vs actial size %d.\n", memory_map->descriptor_size, sizeof(EFI_MEMORY_DESCRIPTOR));

    int size = memory_map->descriptor_size;
    void *end = (void *)memory_map->memory_descriptor + size * (memory_map->no_entries);

	for (EFI_MEMORY_DESCRIPTOR *p = memory_map->memory_descriptor; (void *)p < end; p = ((void *)p) + size) {
		Print(L"Type: ");
		Print(EfiTypeToString(p->Type));
		Print(L", ");
		EFI_PHYSICAL_ADDRESS addr_start = p->PhysicalStart;
		EFI_PHYSICAL_ADDRESS addr_end = addr_start + 1024 * 4 * p->NumberOfPages;
		Print(L"Address from %x to %x.\n", addr_start, addr_end);
	}
}

void PrintAddressMemoryType(EFI_PHYSICAL_ADDRESS address, struct MemoryMap *memory_map) {
    int size = memory_map->descriptor_size;
    void *end = (void *)memory_map->memory_descriptor + size * memory_map->no_entries;

	for (EFI_MEMORY_DESCRIPTOR *p = memory_map->memory_descriptor; (void *)p <= end; p = ((void *)p + size)) {
		EFI_PHYSICAL_ADDRESS addr_start = p->PhysicalStart;
		EFI_PHYSICAL_ADDRESS addr_end = addr_start + 1024 * 4 * p->NumberOfPages;
		if (addr_start <= address && address <= addr_end) {
			Print(L"Address %x belongs to memory region type %s\n", address, EfiTypeToString(p->Type));
		}
	}
}

void GetGDTR(Gdtr *gdtr) {
	asm volatile ("sgdt %0 \n" : : "m" (*gdtr) : "memory");
}

CHAR16 *EfiTypeToString(UINT32 type) {
	switch (type) {
		ENUM_TO_STRING_SWITCH(EfiReservedMemoryType)
		ENUM_TO_STRING_SWITCH(EfiLoaderCode)
		ENUM_TO_STRING_SWITCH(EfiLoaderData)
		ENUM_TO_STRING_SWITCH(EfiBootServicesCode)
		ENUM_TO_STRING_SWITCH(EfiBootServicesData)
		ENUM_TO_STRING_SWITCH(EfiRuntimeServicesCode)
		ENUM_TO_STRING_SWITCH(EfiRuntimeServicesData)
		ENUM_TO_STRING_SWITCH(EfiConventionalMemory)
		ENUM_TO_STRING_SWITCH(EfiUnusableMemory)
		ENUM_TO_STRING_SWITCH(EfiACPIReclaimMemory)
		ENUM_TO_STRING_SWITCH(EfiACPIMemoryNVS)
		ENUM_TO_STRING_SWITCH(EfiMemoryMappedIO)
		ENUM_TO_STRING_SWITCH(EfiMemoryMappedIOPortSpace)
		ENUM_TO_STRING_SWITCH(EfiPalCode)
		ENUM_TO_STRING_SWITCH(EfiMaxMemoryType)
	default:
		Print(L"Unrecognized: %x", type);
		return L"Unrecognized";
		break;
	}
}