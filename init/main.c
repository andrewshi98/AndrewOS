#include <efi.h>
#include <efilib.h>

#include <kernel.h>

struct MemoryMap mem_map;

EFI_STATUS initGraphics(EFI_GRAPHICS_OUTPUT_PROTOCOL *GProtocol);

EFI_STATUS
EFIAPI
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{

  EFI_STATUS status_indicator;

	InitializeLib(ImageHandle, SystemTable);

  mem_map.memory_descriptor = LibMemoryMap(&mem_map.no_entries, &mem_map.map_key,
                                           &mem_map.descriptor_size, &mem_map.descriptor_version);

  //Use boot service to find a instance of graphics protocol, in order to switch to graphics mode.
  Print(L"Locating GProtocol......\n");
  EFI_GUID GProtocolID = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
  EFI_GRAPHICS_OUTPUT_PROTOCOL *GProtocol;
  status_indicator = uefi_call_wrapper(SystemTable->BootServices->LocateProtocol, 3, &GProtocolID,
                                       NULL, &GProtocol);

  status_indicator = initGraphics(GProtocol);

  Print(L"Finished Initializing\n");
  if (status_indicator != EFI_SUCCESS)
    return status_indicator;

  //bitBltEFITest();

  Print(L"Exiting BootServices\n");

  status_indicator = uefi_call_wrapper(SystemTable->BootServices->ExitBootServices, 2,
                    ImageHandle, mem_map.map_key);

  while (status_indicator != EFI_SUCCESS) {
    mem_map.memory_descriptor = LibMemoryMap(&mem_map.no_entries, &mem_map.map_key,
                                             &mem_map.descriptor_size, &mem_map.descriptor_version);
    status_indicator = uefi_call_wrapper(SystemTable->BootServices->ExitBootServices, 2,
                      ImageHandle, mem_map.map_key);
  }
  /*
  Test Commit.
  uefi_call_wrapper(SystemTable->RuntimeServices->SetVirtualAddressMap, 4,
                    mem_map.no_entries, mem_map.descriptor_size,
                    mem_map.descriptor_version, mem_map.memory_descriptor);*/

  clearScreen(0x00ff00);

  // One loop takes 2.6 * 2 sec. Still surprisingly inefficient......

  for(;;){
    for (int i = 0; i < 100; i++){
      drawRectangle(0x00ff00, 1720, 1, 100 + i - 1, 100 + i - 1);
      drawRectangle(0x00ff00, 1, 280, 100 + i - 1, 100 + i - 1);
      drawRectangle(0xff, 1720, 280, 100 + i, 100 + i);
    }
    for (int i = 0; i < 100; i++){
      drawRectangle(0x66, 1720, 280, 200 - i, 200 - i);
      drawRectangle(0x00ff00, 1720, 1, 200 - i + 1, 480 - i + 2);
      drawRectangle(0x00ff00, 1, 280, 1920 - i + 1, 200 - i + 2);
    }
  }
}