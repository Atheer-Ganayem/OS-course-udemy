INCLUDES = -I./src
FLAGS = -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops \
        -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function \
        -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter \
        -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc

# Auto-discover all C and ASM source files (excluding boot and the kernel entry point)
C_SRCS   := $(shell find ./src -name '*.c')
ASM_SRCS := $(shell find ./src -name '*.asm' -not -path '*/boot/*' -not -path '*/kernel.asm')

# Map sources to object files under ./build
C_OBJS   := $(patsubst ./src/%.c,   ./build/%.o,     $(C_SRCS))
ASM_OBJS := $(patsubst ./src/%.asm, ./build/%.asm.o, $(ASM_SRCS))

# kernel.asm.o MUST be first so _start lands at 0x100000
ALL_OBJS := ./build/kernel.asm.o $(C_OBJS) $(ASM_OBJS)

.PHONY: all clean qemu

all: ./bin/boot.bin ./bin/kernel.bin
	rm -rf ./bin/os.bin
	dd if=./bin/boot.bin >> ./bin/os.bin
	dd if=./bin/kernel.bin >> ./bin/os.bin
	dd if=/dev/zero bs=512 count=100 >> ./bin/os.bin

qemu:
	qemu-system-i386 -hda ./bin/os.bin

./bin/kernel.bin: $(ALL_OBJS)
	i686-elf-ld -g -relocatable $(ALL_OBJS) -o ./build/kernelfull.o
	i686-elf-gcc $(FLAGS) -T ./src/linker.ld -o ./bin/kernel.bin -ffreestanding -O0 -nostdlib ./build/kernelfull.o

./bin/boot.bin: ./src/boot/boot.asm
	nasm -f bin ./src/boot/boot.asm -o ./bin/boot.bin

# Kernel ASM entry point (explicit, always first)
./build/kernel.asm.o: ./src/kernel.asm
	@mkdir -p $(dir $@)
	nasm -f elf -g ./src/kernel.asm -o ./build/kernel.asm.o

# Compile all other C files
./build/%.o: ./src/%.c
	@mkdir -p $(dir $@)
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c $< -o $@


./build/%.asm.o: ./src/%.asm
	@mkdir -p $(dir $@)
	nasm -f elf -g $< -o $@

clean:
	rm -rf ./bin/boot.bin ./bin/kernel.bin ./bin/os.bin
	rm -rf ./build