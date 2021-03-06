BASEDIR = $(PWD)
SRCDIR = src
CROSSDIR = $(BASEDIR)/cross
BINUTILS_SRC = $(CROSSDIR)/binutils-2.22
BINUTILS_BUILD = $(CROSSDIR)/binutils-build
GCC_SRC = $(CROSSDIR)/gcc-4.6.4
GCC_BUILD = $(CROSSDIR)/gcc-build
CROSSBINDIR = $(CROSSDIR)/bin
TARGET = i586-elf
CFILES = $(wildcard $(SRCDIR)/*.c)
OBJS = $(subst .c,.o,$(subst src/,build/,$(CFILES)))

.PHONY: clean qemu bochs all dist cross

qemu: image/boot/jdsos.bin
	qemu-system-i386 -kernel image/boot/jdsos.bin -serial file:log/qemu.log

$(BINUTILS_SRC):
	cd $(CROSSDIR) && \
	wget ftp://ftp.gnu.org/gnu/binutils/binutils-2.22.tar.gz && \
	tar xzvf binutils-2.22.tar.gz && rm binutils-2.22.tar.gz

build-cross-binutils: $(BINUTILS_SRC)
	mkdir -p $(BINUTILS_BUILD) && cd $(BINUTILS_BUILD) && \
	$(BINUTILS_SRC)/configure --target=$(TARGET) --prefix="$(CROSSDIR)" --disable-nls && \
	make && make install

$(GCC_SRC):
	cd $(CROSSDIR) && \
	wget ftp://ftp.gnu.org/gnu/gcc/gcc-4.6.4/gcc-core-4.6.4.tar.gz && \
	tar xzvf gcc-core-4.6.4.tar.gz && rm gcc-core-4.6.4.tar.gz && \
	cd $(GCC_SRC) && contrib/download_prerequisites

build-cross-gcc: $(GCC_SRC)
	mkdir -p $(GCC_BUILD) && cd $(GCC_BUILD) && \
	$(GCC_SRC)/configure --target=$(TARGET) --prefix="$(CROSSDIR)" --disable-nls --enable-languages=c --without-headers && \
	make all-gcc && make all-target-libgcc && make install-gcc && make install-target-libgcc

cross: build-cross-binutils build-cross-gcc

build/boot.o: src/boot.asm
	nasm -felf $< -o $@

build/%.o: src/%.c
	export PATH="$(CROSSBINDIR):$(PATH)" && \
	i586-elf-gcc -c $< -o $@ -std=gnu99 -ffreestanding -O2 -Wall -Wextra

build/jdsos.bin: build/boot.o $(OBJS)
	export PATH="$(CROSSBINDIR):$(PATH)" && \
	i586-elf-gcc -T src/linker.ld -o $@ -std=gnu99 -ffreestanding -O2 -nostdlib $? -lgcc

image/boot/jdsos.bin: build/jdsos.bin
	cp $< $@

dist/jdsos.iso: image/boot/jdsos.bin
	grub-mkrescue -o $@ image

dist: dist/jdsos.iso
all: dist

bochs: dist/jdsos.iso
	bochs -f .bochsrc

clean:
	rm -rf $(BINUTILS_BUILD) $(GCC_BUILD) build/* dist/* image/boot/*.bin

