# Areos

A kernel that plays videos in up to 560 colors using vga text mode's 16 colors and ascii support, as well as some dithering hacks by coloring bg and fg of these chars: `░▒▓█`

<strong>Below is how i got to making this stupidness. Jump to [this](#usage) if you want to skip it</strong>

I actually started this project over a challenge from a friend to play Bad Apple in a kernel after they made it play in dos, it started with [This project](https://github.com/Vortetty/AreOS/tree/master/videoKernelGrey)
Shortly after that i decided to add dithering using the greys(going from 1bpp, to 2bpp)
Ascii has `░▒▓█` that can be used for dithering, so to this idea was added a number for what char to use to add more colors
that plan resulted in 17 possible colors! And the format was `<0b0-0b11 for first grey(fg)><0b0-0b11 for second grey(bg)><0b0-0b11 for second grey><0b0-0b11 for character to use (one of these: ░▒▓█)>00`, for example a black and white pixel with equal parts each color could be `0b11000100`
I then realized i could encode colors this same way, and now the videos are arrays of vga characters, and the dither characters allow color blending leading to the 560 colors this supports!

## Usage

First create a video (or use one of the pre-compiled ones)
To do so just run `python scrips/genVideo.py [-h] <video name> [-s] [-o]`
This will produce a c++ file containing an array, and a compiled source file, both can be found in `src/videos/src` and `src/videos/compiled` respectively
Next in the `Makefile` you can change `VIDEO_TO_RUN = rick.o` on line 3 to `VIDEO_TO_RUN = <video file>`(substituting `<video file>` for your newly made video file)

Finally to compile if you can run `make build`, to run it you can use `make run-last`, and to do both in one command you can use `make test`
A bootable iso with grub, and a multiboot compatible binary can both be found in `dist`

## Requirements

- Linux
  - tested on Arch Linux with linux 5.13.7-arch1-1
  - You could use Windows if you want to try, but you will need to compile/find binaries for i686-elf-gcc, GNU Sed, and GNU Make yourself or find replacements
  - the makefile will need to be changed to use windows commands
    - this includes changing syntax for some ifs
- Python 3.9+
  - also needs these libs from pip:
    - scipy
    - numpy
    - pillow
    - opencv-python
- Qemu
- i686-elf-gcc
- GNU Make
- GRUB
- GNU Sed
