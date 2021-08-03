#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "utils.hpp"
#include "rick.hpp"
 
/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif
 
/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif
 
/* Hardware text mode color constants. */
enum vga_color {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15,
};
 
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) 
{
    return fg | bg << 4;
}
 
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
    return (uint16_t) uc | (uint16_t) color << 8;
}

static inline uint16_t get_vga_entry_char(uint16_t entry) 
{
    return entry & 0x00FF;
}
static inline uint16_t get_vga_entry_color(uint16_t entry) 
{
    return entry & 0xFF00;
}
 
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;



size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;
uint8_t white = 0x77, black = 0x00;
 
void terminal_initialize(void) 
{
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    terminal_buffer = (uint16_t*) 0xB8000;
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_color);
        }
    }
}

inline void terminal_clear(void) {
    terminal_initialize();
}
 
void terminal_setcolor(uint8_t color) 
{
    terminal_color = color;
}
 

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) 
{
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = vga_entry(c, color);
}
 
void terminal_putchar(char c) 
{
    if (c == '\n') {
        terminal_column = 0;
        if (terminal_row++ == VGA_HEIGHT-1){
            terminal_row = VGA_HEIGHT-1;
            for (size_t y = 1; y < VGA_HEIGHT; y++) {    // Go through vga y skipping the first line (since that will be written over)
                for (size_t x = 0; x < VGA_WIDTH; x++) { // Go through vga x for each char on each line
                    size_t o_index = y * VGA_WIDTH + x;   // Index to copy from
                    size_t n_index = (y-1) * VGA_WIDTH + x; // Index to copy to
                    terminal_buffer[n_index] = terminal_buffer[o_index]; // Copy from old to new
                }
            }
            for (size_t x = (VGA_HEIGHT-1)*VGA_WIDTH; x < VGA_HEIGHT*VGA_WIDTH; x++) {
                terminal_buffer[x] = vga_entry(' ', terminal_color); // Clear the rest of the screen
            }
        }
    } else {
        terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
        if (terminal_column++ == VGA_WIDTH) {
            terminal_column = 0;
            if (terminal_row++ == VGA_HEIGHT)
                terminal_row = 0;
        }
    }
}
 
void terminal_write(const char* data, size_t size) 
{
    for (size_t i = 0; i < size; i++)
        terminal_putchar(data[i]);
}
 
void terminal_writestring(const char* data) 
{
    terminal_write(data, strlen(data));
}

void drawFrame(int frame) {
    size_t offset = (video[2] * video[3] / 8) * frame + 4;
    for (size_t c = 0; c < (video[2] * video[3] / 8); c++) {
        uint8_t b = reverseByte(video[offset+c]);
        for (int i = 0; i < 8; i++) {
            terminal_buffer[c*8+i] = vga_entry(' ', b & 0x1 ? white : black);
            b = b >> 1;
        }
    }
}
 
extern "C" void kernel_main(void) 
{
    /* Initialize terminal interface */
    terminal_initialize();
    terminal_writestring("Running -- <OS_NAME> -- kernel v-- <VERSION> --\nPlaying Bad Apple in ~1 second...");
    
    for (uint64_t i = 0; i < (int)(0x1ffffff/1.275)*24; i++) asm(" ");

    for (int i = 0; i < (video[0] << 8 | video[1]); i++) {
        drawFrame(i);
        for (uint64_t c = 0; c < (int)(0x1ffffff/1.275); c++) asm(" ");
    }

    terminal_clear();
    terminal_writestring("Running -- <OS_NAME> -- kernel v-- <VERSION> --\nThank you for watching, have a nice day!");
}