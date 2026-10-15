#include <stdint.h>

uint8_t memory[4096];
uint16_t stack[16];
uint8_t registers[16];

uint8_t delay_timer; // counts down each frame
uint8_t sound_timer; // also counts down each frame

uint16_t index_register; // points to a register, probably a number index instead of a pointer?
uint16_t program_counter; // current instruction (mem index?)

uint64_t display[32]; // 64x32, will use xor operators to manipulate since it's 1d (saves 7 bits per pixel i think? ts not worth it)
uint16_t keyboard; // same concept as above, xor saves 7 bits per key :sob:

// LOOP
// ummm get the current instruction (pc represents it in memory)
// immediately skip ahead 2 (each instruction is 2 bytes)
// decode the instruction so i guess just handle it or sum
// why did the article have this as a third step ill just run it when i decode it or sumething idkdf
