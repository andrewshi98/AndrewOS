#include <efi.h>
#include <efilib.h>
#include <kernel.h>

#include "boot.h"
#include "efiUtil.h"

struct MemoryMap mem_map;

EFI_STATUS initGraphics(EFI_GRAPHICS_OUTPUT_PROTOCOL *GProtocol);

EFI_STATUS
EFIAPI
efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
	EFI_STATUS status_indicator;

	InitializeLib(ImageHandle, SystemTable);

	//Use boot service to find a instance of graphics protocol, in order to switch to graphics mode.
	Print(L"Locating GProtocol......\n");
	EFI_GUID GProtocolID = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
	EFI_GRAPHICS_OUTPUT_PROTOCOL *GProtocol;
	status_indicator = uefi_call_wrapper(SystemTable->BootServices->LocateProtocol, 3, &GProtocolID,
	                                     NULL, &GProtocol);

	status_indicator = initGraphics(GProtocol);

  	mem_map.memory_descriptor = LibMemoryMap(&mem_map.no_entries, &mem_map.map_key,
                                          &mem_map.descriptor_size, &mem_map.descriptor_version);

	// status_indicator = uefi_call_wrapper(SystemTable->BootServices->GetMemoryMap, 5, &mem_map.no_entries, mem_map.memory_descriptor, &mem_map.map_key, &mem_map.descriptor_size, &mem_map.descriptor_version);
    // // descriptor size and version should be correct values.  mem_map.no_entries should be needed size.

    // if (status_indicator == EFI_BUFFER_TOO_SMALL) {
    //     UINTN encompassing_size = mem_map.no_entries + (2 * mem_map.descriptor_size);
    //     void *buffer = NULL;
    //     status_indicator = uefi_call_wrapper(SystemTable->BootServices->AllocatePool, 3, EfiLoaderData, encompassing_size, &buffer);
    //     if (status_indicator == EFI_SUCCESS) {
    //         mem_map.memory_descriptor = (EFI_MEMORY_DESCRIPTOR*) buffer;
    //         mem_map.no_entries = encompassing_size;

    //         status_indicator = uefi_call_wrapper(SystemTable->BootServices->GetMemoryMap, 5, &mem_map.no_entries, mem_map.memory_descriptor, &mem_map.map_key, &mem_map.descriptor_size, &mem_map.descriptor_version);
    //         if (status_indicator != EFI_SUCCESS) {
    //             Print(L"Second call to GetMemoryMap failed for some reason.");
    //         } else {
    //             Print(L"Second call to GetMemoryMap succeeded.\n");
    //         }

    //     } else {
    //         Print(L"AllocatePool failure.");
    //     }
    // } else if (status_indicator == EFI_SUCCESS) { // shouldn't happen.
    //     Print(L"First call to GetMemoryMap should never succeed... ???");
    // } else {
    //     Print(L"(First) GetMemoryMap usage failure.");
    // }

	PrintEfiMemoryMap(&mem_map);

	Print(L"Finished Initializing\n");
	if (status_indicator != EFI_SUCCESS)
		return status_indicator;

	Gdtr gdt_ptr;
	GetGDTR(&gdt_ptr);

	PrintAddressMemoryType(gdt_ptr.base, &mem_map);

	Print(L"Exiting BootServices\n");

  // TODO: Have to call LibMemoryMap again. To have the correct mem_map.map_key.
	status_indicator = uefi_call_wrapper(SystemTable->BootServices->ExitBootServices, 2,
	                                     ImageHandle, mem_map.map_key);

	while (status_indicator != EFI_SUCCESS) {
		status_indicator = uefi_call_wrapper(SystemTable->BootServices->LocateProtocol, 3, &GProtocolID,
		                                     NULL, &GProtocol);
		status_indicator = uefi_call_wrapper(SystemTable->BootServices->ExitBootServices, 2,
		                                     ImageHandle, mem_map.map_key);
	}

	/*
  Test Commit.
  uefi_call_wrapper(SystemTable->RuntimeServices->SetVirtualAddressMap, 4,
                    mem_map.no_entries, mem_map.descriptor_size,
                    mem_map.descriptor_version, mem_map.memory_descriptor);*/

	for (;;) {
	}

	return EFI_SUCCESS;
}
