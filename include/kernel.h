#pragma once			//Use #define KERNEL.h Instead if not supported by the compiler
#include <efi.h>
#include <efilib.h>
#include <efiprot.h>	//Structures for GNU-EFI

//Setting up graphics

#define DESIRED_HORIZONTAL_RESOLUTION 800
#define DESIRED_VERTICAL_RESOLUTION 600

typedef struct {
    unsigned short limit;
    unsigned int base __attribute__((packed));
} __attribute__((packed)) Gdtr;