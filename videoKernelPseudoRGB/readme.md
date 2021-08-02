# Areos

A kernel that plays videos in up to 560 colors using vga text mode's 16 colors and ascii support, as well as some dithering hacks

## Usage

First create a video (or use one of the pre-compiled ones)
To do so just run `python scrips/genVideo.py [-h] <video name> [-s] [-o]`
This will produce a c++ file containing an array, and a compiled source file, both can be found in `src/videos/src` and `src/videos/compiled` respectively
Next in the `Makefile` you can change `VIDEO_TO_RUN = rick.o` on line 3 to `VIDEO_TO_RUN = <video file>`(substituting `<video file>` for your newly made video file)

Finally to compile if you can run `make build`, to run it you can use `make run-last`, and to do both in one command you can use `make test`
A bootable iso with grub, and a multiboot compatible binary can both be found in `dist`

## Requirements

- Python 3.9+
  - scipy
  - numpy
  - pillow
  - opencv-python
- Qemu
- i686-elf-gcc
- GNU Make
