#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "utils.hpp"

extern uint16_t video[];
 
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

uint16_t colors[] = {5552, 20913, 5554, 30171, 53936, 7857, 53938, 731, 40880, 48561, 40882, 57307, 34224, 49585, 34226, 27568, 16049, 27570, 60379, 19888, 50609, 19890, 36315, 18352, 29873, 18354, 38875, 37552, 41393, 37554, 31408, 31410, 15067, 51888, 36529, 51890, 23728, 23730, 27867, 2992, 2994, 60592, 52913, 60594, 56240, 56242, 6832, 6834, 9392, 24753, 9394, 34011, 58544, 50865, 58546, 28848, 4016, 28850, 37083, 14256, 29617, 14258, 63408, 32689, 63410, 35504, 43185, 35506, 42928, 42930, 15792, 23473, 15794, 64944, 57265, 64946, 39856, 47537, 39858, 48304, 63665, 48306, 25008, 25010, 33243, 26800, 19121, 26802, 6363, 46256, 46258, 21424, 29105, 21426, 25563, 52656, 56497, 52658, 25776, 18097, 25778, 51120, 62641, 51122, 31152, 31154, 55771, 9136, 12977, 9138, 58288, 58290, 3248, 18609, 3250, 56496, 56498, 7600, 22961, 7602, 1200, 16561, 1202, 6064, 6066, 34736, 34738, 46000, 15281, 46002, 16560, 16562, 40112, 36273, 40114, 38064, 38066, 944, 4785, 946, 22192, 22194, 34523, 12464, 12466, 61616, 61618, 54192, 54194, 9904, 25265, 9906, 59056, 28337, 59058, 41136, 2737, 41138, 18864, 18866, 11696, 11698, 20400, 20402, 20912, 20914, 22704, 22706, 8624, 8626, 57776, 57778, 37808, 7089, 37810, 10416, 10418, 59568, 59570, 11952, 58033, 11954, 7899, 21680, 17841, 21682, 46768, 46770, 17072, 17074, 49328, 49330, 14768, 14770, 63920, 63922, 16304, 62385, 16306, 43440, 39601, 43442, 9648, 9650, 58800, 58802, 28080, 28082, 44976, 60337, 44978, 45488, 45490, 47280, 47282, 4272, 433, 4274, 1712, 1714, 48816, 48818, 31920, 31922, 32944, 2225, 32946, 54960, 54962, 3504, 3506, 12976, 12978, 62128, 62130, 28336, 28338, 41648, 10929, 41650, 19376, 19378, 51632, 51634, 46512, 46514, 53168, 57009, 53170, 32688, 32690, 432, 434, 2224, 2226, 64176, 64178, 26032, 26034, 53680, 53682, 3760, 3762, 55472, 55474, 38576, 38578, 10928, 10930, 60080, 44721, 60082, 6576, 37297, 6578, 8112, 8114, 54448, 54450, 35248, 39089, 35250, 1456, 16817, 1458, 49840, 49842, 29360, 25521, 29362, 15280, 15282, 64432, 49073, 64434, 43952, 47793, 43954, 36784, 36786, 37296, 37298, 39088, 39090, 8368, 689, 8370, 40624, 40626, 47792, 47794, 4784, 4786, 33456, 33458, 27312, 27314, 19632, 50353, 19634, 38320, 38322, 52144, 52146, 31664, 31666, 23984, 54705, 23986, 22448, 30129, 22450, 2736, 2738, 60848, 60850, 10160, 10162, 24752, 24754, 59312, 32433, 59314, 7088, 7090, 15536, 15538, 64688, 64690, 24240, 24242, 35760, 35762, 44208, 44210, 13488, 13490, 62640, 62642, 39600, 39602, 42160, 42162, 47024, 47026, 48560, 48562, 17328, 17330, 27056, 27058, 26544, 30385, 26546, 36528, 36530, 1968, 1970, 50352, 50354, 29872, 29874, 55216, 32177, 55218, 62384, 62386, 25264, 25266, 7344, 7346, 41904, 15025, 41906, 36016, 51377, 36018, 23216, 23218, 5296, 5298, 57008, 57010, 40368, 55729, 40370, 33968, 33970, 38832, 38834, 20656, 20658, 54704, 54706, 18096, 18098, 57520, 57522, 29616, 29618, 50096, 50098, 16816, 16818, 5040, 12721, 5042, 18608, 18610, 11440, 11442, 20144, 20146, 33712, 33714, 14000, 14002, 63152, 63154, 45232, 45234, 42672, 42674, 22960, 22962, 24496, 24498, 10672, 10674, 59824, 59826, 44464, 44466, 17840, 17842, 55984, 55986, 12208, 12210, 61360, 65201, 61362, 12720, 12722, 61872, 61874, 14512, 14514, 63664, 63666, 41392, 41394, 16048, 16050, 43184, 43186, 65200, 65202, 44720, 44722, 27824, 27826, 21168, 21170, 53424, 53426, 50864, 50866, 30384, 30386, 58032, 58034, 47536, 47538, 13744, 13746, 62896, 62898, 32176, 32178, 49072, 49074, 42416, 42418, 28592, 28594, 5808, 5810, 49584, 49586, 29104, 29106, 37040, 33201, 37042, 34480, 34482, 30896, 30898, 51376, 51378, 19120, 19122, 32432, 32434, 2480, 2482, 52912, 52914, 36272, 36274, 45744, 45746, 23472, 23474, 55728, 55730, 4018, 11184, 11186, 60336, 60338, 57264, 57266, 6320, 6322, 50608, 50610, 30128, 30130, 33200, 33202, 7856, 7858, 25520, 25522, 15024, 15026, 688, 690};
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
    terminal_row = 0;
    terminal_column = 0;
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_color);
        }
    }
}
 
void terminal_setcolor(uint8_t color) 
{
    terminal_color = color;
}

void terminal_setcolor(enum vga_color fg, enum vga_color bg){
    terminal_color = vga_entry_color(fg, bg);
}
 

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) 
{
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = vga_entry(c, color);
}

void terminal_putentryat(uint16_t entry, size_t x, size_t y) 
{
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = entry;
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

void terminal_putchar(uint16_t entry) 
{
    if (get_vga_entry_char(entry) == '\n') {
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
        terminal_putentryat(entry, terminal_column, terminal_row);
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
    size_t offset = video[1] * video[2] * frame + 3;
    for (size_t c = 0; c < video[1] * video[2]; c++) {
        uint16_t entry = video[offset + c];
        terminal_putentryat(entry, c % video[1], c / video[1]);
    }
}
 
extern "C" void kernel_main(void) 
{
    /* Initialize terminal interface */
    terminal_initialize();
    terminal_writestring("Running -- <OS_NAME> -- kernel v-- <VERSION> --\nRunning fade test\n");
    
    // ░▒▓█
    char testStr[4] = {(char)176, (char)177, (char)178, (char)219};
    terminal_setcolor(VGA_COLOR_BLACK, VGA_COLOR_BLACK);
    terminal_putchar((char)219);
    terminal_setcolor(VGA_COLOR_DARK_GREY, VGA_COLOR_BLACK);
    terminal_writestring(testStr);
    terminal_setcolor(VGA_COLOR_LIGHT_GREY, VGA_COLOR_DARK_GREY);
    terminal_writestring(testStr);
    terminal_setcolor(VGA_COLOR_WHITE, VGA_COLOR_LIGHT_GREY);
    terminal_writestring(testStr);

    terminal_putchar('\n');
    terminal_setcolor(VGA_COLOR_WHITE, VGA_COLOR_LIGHT_GREY);
    terminal_putchar((char)176);
    terminal_setcolor(VGA_COLOR_BLACK, VGA_COLOR_BLACK);
    terminal_writestring(" ");

    terminal_setcolor(VGA_COLOR_WHITE, VGA_COLOR_LIGHT_GREY);
    terminal_putchar((char)177);
    terminal_setcolor(VGA_COLOR_BLACK, VGA_COLOR_BLACK);
    terminal_writestring(" ");

    terminal_setcolor(VGA_COLOR_WHITE, VGA_COLOR_LIGHT_GREY);
    terminal_putchar((char)178);
    terminal_setcolor(VGA_COLOR_BLACK, VGA_COLOR_BLACK);
    terminal_writestring(" ");

    terminal_setcolor(VGA_COLOR_WHITE, VGA_COLOR_LIGHT_GREY);
    terminal_putchar((char)219);
    terminal_setcolor(VGA_COLOR_BLACK, VGA_COLOR_BLACK);
    terminal_writestring(" ");


    terminal_setcolor(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    terminal_writestring("\n\nRunning color test\n");
    for (size_t i = 0; i < 16; i++) {
        terminal_setcolor((enum vga_color)i, VGA_COLOR_BLACK);
        terminal_putchar((char)219);
    }

    for (size_t i = 0; i < 560; i++) {
        terminal_putchar(colors[i]);
    }

    terminal_clear();

    while (true) {
        for (int frame = 0; frame < video[0]; frame++) {
            drawFrame(frame);
            for (uint64_t c = 0; c < (int)(0x1ffffff/1.05); c++) asm(" ");
        }
    }
}