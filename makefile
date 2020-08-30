CC				= gcc
ARCH            = $(shell uname -m | sed s,i[3456789]86,ia32,)


#GNU_EFI LIB AND INC

EFIINC          = /usr/include/efi
EFIINCS         = -I$(EFIINC) -I$(EFIINC)/$(ARCH) -I$(EFIINC)/protocol
LIB             = /usr/lib
EFILIB          = /usr/lib
EFI_CRT_OBJS    = $(EFILIB)/crt0-efi-$(ARCH).o
EFI_LDS         = $(EFILIB)/elf_$(ARCH)_efi.lds

#END

#OS	

BOOTINC			= ./boot/
OSINC			= ./include/
OSINCS			= -I$(OSINC) -I$(BOOTINC)
OUTDIR			= ./output/
BOOTDIR			= ./boot/

OUTIMG			= ${OUTDIR}fat.img

#BUILD TARGETS

OBJS            = $(OUTDIR)main.o $(OUTDIR)graphics.o $(OUTDIR)efiUtil.o $(OUTDIR)boot.o
TARGET          = $(OUTDIR)BOOTX64.efi
KERNEL_TARGET	= $(OUTDIR)kernel

#END

CFLAGS          = $(EFIINCS) $(OSINCS) -fno-stack-protector -fpic \
		  -fshort-wchar -mno-red-zone -Wall 
ifeq ($(ARCH),x86_64)
  CFLAGS += -DEFI_FUNCTION_WRAPPER
endif

LDFLAGS         = -nostdlib -znocombreloc -T $(EFI_LDS) -shared \
		  		  -Bsymbolic -L $(EFILIB) -L $(LIB) $(EFI_CRT_OBJS) 

all: directory make_kernel $(TARGET)

make_kernel:
	cd kernel && make && cd ..

directory: ${OUTDIR}
	
${OUTDIR}:
	mkdir -p ${OUTDIR}

$(TARGET): $(OUTDIR)main.so
	objcopy -j .text -j .sdata -j .data -j .dynamic \
		-j .dynsym  -j .rel -j .rela -j .reloc \
		--target=efi-app-$(ARCH) $^ $@

$(OUTDIR)main.so: $(OBJS)
	ld $(LDFLAGS) $(OBJS) -o $@ -lefi -lgnuefi

$(OUTDIR)%.o: $(BOOTDIR)%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm ${OUTDIR} -r -f

run: all
	dd if=/dev/zero of=${OUTIMG} bs=1k count=1440
	mformat -i ${OUTIMG} -f 1440 ::
	mmd -i ${OUTIMG} ::/EFI
	mmd -i ${OUTIMG} ::/EFI/BOOT
	mcopy -i ${OUTIMG} ${TARGET} ::/EFI/BOOT
	mcopy -i ${OUTIMG} ${KERNEL_TARGET} ::/
	mkgpt -o ${OUTDIR}hdimage.bin --image-size 4096 --part ${OUTIMG} --type system
	qemu-system-x86_64 -bios ./TestTool/OVMF_PURE_EFI.fd -hda ${OUTDIR}hdimage.bin
