# Makefile for building the kernel

CC				= gcc
ARCH            = $(shell uname -m | sed s,i[3456789]86,ia32,)

#LIB AND INC

LIB             = /usr/lib

#END

#OS	

OSINC			= ../include/
OSINCS			= -I$(OSINC)
OUTDIR			= ../output/

OUTIMG			= ${OUTDIR}fat.img

#BUILD TARGETS

OBJS            = $(OUTDIR)init.o
TARGET          = $(OUTDIR)kernel

#END

CFLAGS          = $(OSINCS) -ffreestanding -fno-stack-protector -fpic \
		  		  -fshort-wchar -mno-red-zone -Wall -nostdlib -nostartfiles -nodefaultlibs

LDFLAGS         = -Bsymbolic -znocombreloc -T ./kernel.ld \
		  		  -nostdlib -nostartfiles -nodefaultlibs 

all: directory $(TARGET)

directory: ${OUTDIR}
	
${OUTDIR}:
	mkdir -p ${OUTDIR}

$(TARGET): $(OBJS)
	ld $(LDFLAGS) $(OBJS) -o $@

$(OUTDIR)%.o: $(BOOTDIR)%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm ${OUTDIR} -r -f

run:
	dd if=/dev/zero of=${OUTIMG} bs=1k count=1440
	mformat -i ${OUTIMG} -f 1440 ::
	mmd -i ${OUTIMG} ::/EFI
	mmd -i ${OUTIMG} ::/EFI/BOOT
	mcopy -i ${OUTIMG} ${TARGET} ::/EFI/BOOT
	mkgpt -o ${OUTDIR}hdimage.bin --image-size 4096 --part ${OUTIMG} --type system
	qemu-system-x86_64 -bios ./TestTool/OVMF_PURE_EFI.fd -hda ${OUTDIR}hdimage.bin