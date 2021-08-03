OS_NAME = AreOS
VERSION = 0.0.0
VIDEO_TO_RUN = rgb_radial_test.o
VIDEO_FOLDER = src/videos/compiled/

mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir := $(dir $(mkfile_path))

build: build_iso clean_build

build_iso: build_os setup_dirs
	@cp dist/$(OS_NAME).bin build/isodir/boot/$(OS_NAME).bin
	@cp build/src/grub/grub.cfg build/isodir/boot/grub/grub.cfg
	@grub-mkrescue -o dist/$(OS_NAME).iso build/isodir

build_os: full_clean setup_dirs copy_src
# Compile kernel
# Uses asm to set up a c++ environment
	@i686-elf-as build/src/asm/boot/boot.s -o build/asm/boot/boot.o
	@i686-elf-g++ -c build/src/c++/kernel/kernelMain.cpp -o build/c++/kernel/kernelMain.o -std=gnu++11 -ffreestanding -O3 -Wall -Wextra -fno-exceptions -fno-rtti -fpermissive

# Link kernel
	@i686-elf-g++ -T build/src/linker/linker.ld -o dist/$(OS_NAME).bin -ffreestanding -O2 -nostdlib build/asm/boot/boot.o build/c++/kernel/kernelMain.o $(VIDEO_FOLDER)$(VIDEO_TO_RUN) -lgcc

# Verify multiboot header
	@if grub-file --is-x86-multiboot dist/$(OS_NAME).bin; then \
		echo Valid multiboot header found, continuing to generating iso with grub; \
	else \
		echo No valid multiboot header found, exiting; \
		exit 1; \
	fi

copy_src:
# Copy src to build dir for version string replacement
	@cp -r src build
# Replace version strings in src
	$(eval SRC_FILES := \
		$(patsubst %, build/%, \
			$(wildcard src/*/*.c) $(wildcard src/*/*/*.c) $(wildcard src/*/*/*/*.c) \
			$(wildcard src/*/*.h) $(wildcard src/*/*/*.h) $(wildcard src/*/*/*/*.h) \
			$(wildcard src/*/*.cpp) $(wildcard src/*/*/*.cpp) $(wildcard src/*/*/*/*.cpp) \
			$(wildcard src/*/*.hpp) $(wildcard src/*/*/*.hpp) $(wildcard src/*/*/*/*.hpp) \
			$(wildcard src/*/*.cfg) $(wildcard src/*/*/*.cfg) $(wildcard src/*/*/*/*.cfg) \
			$(wildcard src/*/*.s) $(wildcard src/*/*/*.s) $(wildcard src/*/*/*/*.s) \
		) \
	)
	@for file in $(SRC_FILES); do \
		sed -r --in-place "s/(-- <OS_NAME> <VERSION> --)\+?/$(OS_NAME) $(VERSION)/g" $${file}; \
		sed -r --in-place "s/(-- <OS_NAME> --)\+?/$(OS_NAME)/g" $${file}; \
		sed -r --in-place "s/(-- <VERSION> --)\+?/$(VERSION)/g" $${file}; \
	done

clean_build:
	@rm -rf build

clean_dist:
	@rm -rf dist

full_clean:
	@rm -rf build dist

setup_dirs:
	@-mkdir build
	@-mkdir build/asm
	@-mkdir build/asm/boot
	@-mkdir build/c++
	@-mkdir build/c++/kernel
	@-mkdir build/isodir
	@-mkdir build/isodir/boot
	@-mkdir build/isodir/boot/grub
	@-mkdir dist

test: build
	qemu-system-x86_64 -cdrom dist/$(OS_NAME).iso
run-last:
	qemu-system-x86_64 -cdrom dist/$(OS_NAME).iso