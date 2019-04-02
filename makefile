CC				= gcc
ARCH            = $(shell uname -m | sed s,i[3456789]86,ia32,)

OBJS            = ./Debug/main.o ./Debug/graphics.o
TARGET          = ./Debug/main.efi

#GNU_EFI LIB AND INC

EFIINC          = /usr/local/include/efi
EFIINCS         = -I$(EFIINC) -I$(EFIINC)/$(ARCH) -I$(EFIINC)/protocol
LIB             = /usr/local/lib64
EFILIB          = /usr/local/lib64
EFI_CRT_OBJS    = $(EFILIB)/crt0-efi-$(ARCH).o
EFI_LDS         = $(EFILIB)/elf_$(ARCH)_efi.lds

#END

#OS

OSINC			= ./include/
OSINCS			= -I$(OSINC)
OUTDIR			= ./Debug/
INITDIR			= ./init/

#END

CFLAGS          = $(EFIINCS) $(OSINCS) -fno-stack-protector -fpic \
		  -fshort-wchar -mno-red-zone -Wall 
ifeq ($(ARCH),x86_64)
  CFLAGS += -DEFI_FUNCTION_WRAPPER
endif

LDFLAGS         = -nostdlib -znocombreloc -T $(EFI_LDS) -shared \
		  -Bsymbolic -L $(EFILIB) -L $(LIB) $(EFI_CRT_OBJS) 

all: $(TARGET)

$(TARGET): $(OUTDIR)main.so
	objcopy -j .text -j .sdata -j .data -j .dynamic \
		-j .dynsym  -j .rel -j .rela -j .reloc \
		--target=efi-app-$(ARCH) $^ $@
	sudo mv $(TARGET) /media/sf_Share_Folder/ -f
	#cleaning up
	rm $(OUTDIR)* -f

$(OUTDIR)main.so: $(OBJS)
	ld $(LDFLAGS) $(OBJS) -o $@ -lefi -lgnuefi

Debug/%.o: init/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

