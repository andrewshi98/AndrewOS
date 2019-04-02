#pragma once			//Use #define KERNEL.h Instead if not supported by the compiler
#include <efi.h>
#include <efilib.h>
#include <efiprot.h>	//Structures for GNU-EFI

//Setting up graphics

#define DESIRED_HORIZONTAL_RESOLUTION 1920
#define DESIRED_VERTICAL_RESOLUTION 1080

struct MemoryMap {
	//LibmemoryMap() return
	EFI_MEMORY_DESCRIPTOR *memory_descriptor;

	//LibmemoryMap() OUT parameter
	UINTN no_entries;
	UINTN map_key;
	UINTN descriptor_size;
	UINT32 descriptor_version;
};

extern struct MemoryMap MemoryMap;

struct GGraphics {
	EFI_GRAPHICS_OUTPUT_PROTOCOL *GProtocol;
	EFI_GRAPHICS_OUTPUT_MODE_INFORMATION info;
	EFI_GRAPHICS_PIXEL_FORMAT PixelFormat;
	EFI_PHYSICAL_ADDRESS FrameBufferBase;
	UINTN FrameBufferSize;

	//Calculation Helper
	UINT32 lineBits; //Offset to move from one (x, y) to (x, y + 1)
	
};

extern struct GGraphics GGraphics;

//Declearing Graphics.c Methods
void setPixel(int w, int h, uint32_t value);
void setRGBPixel(int w, int h, uint32_t rgb);
void setBGRPixel(int w, int h, uint32_t bgr);
void drawRectangle(uint32_t rgb, int width, int height, int x, int y);
void clearScreen(uint32_t rgb);