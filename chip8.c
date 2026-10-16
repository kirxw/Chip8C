#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#define PIXEL_ON  "██"
#define PIXEL_OFF "  "
#define TOP_LEFT "\033[H"

clock_t CPU_SLEEP_TIME = 1/700 * CLOCKS_PER_SEC;

uint8_t memory[4096];
uint16_t stack[16];
uint8_t registers[16];

uint8_t delay_timer; // counts down each frame
uint8_t sound_timer; // also counts down each frame

uint16_t index_register; // points to a register, probably a number index instead of a pointer?
uint16_t program_counter; // current instruction (mem index?)

uint64_t display[32]; // 64x32, will use xor operators to manipulate since it's 1d (saves 7 bits per pixel i think? ts not worth it)
uint16_t keyboard; // same concept as above, xor saves 7 bits per key :sob:

void render_display(void) {
    printf(TOP_LEFT);
    for (int y = 0; y < 32; y++) {
        // pixel rows are stored backward so this is fine.
        uint64_t row = display[y];
        
        for (int x = 0; x < 64; x++) {
            printf("%s", ((row >> (63 - x)) & 1) ? PIXEL_ON : PIXEL_OFF);
        }
        printf("\n");
    }
    fflush(stdout);
}

void execute_instruction(uint8_t byte1, uint8_t byte2) {
    uint8_t instruction_type = byte1 >> 4;
    uint8_t X = byte1 & 0x0F;
    uint8_t Y = byte2 >> 4;
    uint8_t N = byte2 & 0x0F;

    uint8_t NN = byte2; // (Y << 4) | N;
    uint16_t NNN = ((X << 8) | (Y << 4)) | N;

    switch (instruction_type) {
        case 0x0:
            switch (byte2) {
            case 0xE0: // clear display
                memset(display,0,sizeof(display));
                render_display();
                break;
            }
            break;
        case 0x1: // jump
            program_counter = NNN;
            break;
        case 0x2:
            break;
        case 0x3:
            break;
        case 0x4:
            break;
        case 0x5:
            break;
        case 0x6: // set register X
            registers[X] = NN;
            break;
        case 0x7: // add value to register X
            registers[X] += NN;
            break;
        case 0x8:
            break;
        case 0x9:
            break;
        case 0xA: // set index register
            index_register = NNN;
            break;
        case 0xB:
            break;
        case 0xC:
            break;
        case 0xD: // draw XYN
            // i genuinely can't remember why i named them xc and yc, what does the c even stand for??? character?????
            uint8_t xc = registers[X] % 64;
            uint8_t yc = registers[Y] % 32;
            
            registers[0xF] = 0; // set to 1 if collision happens
            for (int row = 0; row < N && (row+yc) < 32; row++) {
                uint64_t sprite = memory[index_register + row];
                sprite = (xc <= 56) ? sprite << (56-xc) : xc >> (xc-56); //rightshift instead if it clips off
                if (display[yc+row] & sprite) {
                    registers[0xF] = 1; // collision happened
                }
                display[yc+row] ^= sprite;
            }

            render_display();
            break;
        case 0xE:
            break;
        case 0xF:
            break;
    }
}

// LOOP
// ummm get the current instruction (pc represents it in memory)
// immediately skip ahead 2 (each instruction is 2 bytes)
// decode the instruction so i guess just handle it or sum
// why did the article have this as a third step ill just run it when i decode it or sumething idkdf

void step_execution_loop(void) {
    uint8_t byte1 = memory[program_counter];
    uint8_t byte2 = memory[program_counter+1];
    program_counter += 2;

    execute_instruction(byte1,byte2);
}

void load_file(FILE *file_pointer) {
    program_counter = 0x200;
    fread(memory+0x200,1,4096-0x200,file_pointer);
}

void loop(void) {
    clock_t last_instruction = clock();
    while (1) {
        clock_t current_time = clock();
        if ((current_time-last_instruction) > CPU_SLEEP_TIME) {
            last_instruction += CPU_SLEEP_TIME;
            step_execution_loop();
        }
    }
}