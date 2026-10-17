#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <inttypes.h>
#include "font.h"

#define COSMAC_MODE 0
#define AMIGA_MODE 0

#define PIXEL_ON  "██"
#define PIXEL_OFF "  "
#define TOP_LEFT "\033[H"

clock_t CPU_SLEEP_TIME = CLOCKS_PER_SEC / 700;
clock_t FRAME_SLEEP_TIME = CLOCKS_PER_SEC / 60;

uint8_t memory[4096];
uint16_t stack[16];
uint8_t registers[16];

uint16_t stack_position;

uint8_t delay_timer; // counts down each frame
uint8_t sound_timer; // also counts down each frame

uint16_t register_index; // points to a register, probably a number index instead of a pointer?
uint16_t program_counter; // current instruction (mem index?)

uint64_t display[32]; // 64x32, will use xor operators to manipulate since it's 1d (saves 7 bits per pixel i think? ts not worth it)
uint16_t keyboard; // same concept as above, xor saves 7 bits per key :sob:

void push_stack(uint16_t item) {
    // stack is a little wonky since this is an unsigned int... should be fine?
    stack[stack_position] = item;
    stack_position++;
}

uint16_t pop_stack(void) {
    stack_position--;
    return stack[stack_position];
}

int SHOULD_RENDER = 0;
void render_display(void) {
    SHOULD_RENDER = 0;
    printf(TOP_LEFT);
    for (int y = 0; y < 32; y++) {
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
    uint16_t NNN = (X << 8) | NN; // (Y << 4)) | N;

    uint8_t VX = registers[X],VY = registers[Y];

    switch (instruction_type) {
        case 0x0:
            switch (byte2) {
            case 0xE0: // clear display
                memset(display,0,sizeof(display));
                SHOULD_RENDER = 1;
                break;
            case 0xEE: // return from subroutine
                program_counter = pop_stack();
                printf("Returning to stack point %" PRIu16 "\n" ,stack_position);
                break;
            }
            break;
        case 0x1: // jump
            program_counter = NNN;
            break;
        case 0x2: // new subroutine
            push_stack(program_counter);
            program_counter = NNN;
            break;
        case 0x3: // 3XNN skip an instruction if VX = NN
            if (registers[X] == NN) {
                program_counter += 2;
            }
            break;
        case 0x4: // 4XNN skip an instruction if VX != NN
            if (registers[X] != NN) {
                program_counter += 2;
            }
            break;
        case 0x5: // 5XY0 skips if VX and VY are equal
            if (registers[X] == registers[Y]) {
                program_counter += 2;
            }
            break;
        case 0x6: // 6XNN set register X to NN
            registers[X] = NN;
            break;
        case 0x7: // 7XNN add NN to register X
            registers[X] += NN;
            break;
        case 0x8: // math operators
            switch (N) {
                case 0x0: // 8XY0 set the value of VX to VY
                    registers[X] = registers[Y];
                    break;
                case 0x1: // 8XY1 set value of VX to (VX bor VY)
                    registers[X] |= registers[Y];
                    break;
                case 0x2: // 8XY2 set value of VX to (VX band VY)
                    registers[X] &= registers[Y];
                    break;
                case 0x3: //8XY3 set value of VX to (VX bxor VY)
                    registers[X] ^= registers[Y];
                    break;
                case 0x4: //8XY4 set value of VX to (VX + VY), set VF to 1 if it overflows
                    registers[X] += VY;
                    registers[0xF] = (VY > (255-VX));
                    break;
                case 0x5: //8XY5 set VX to VX - VY
                    registers[X] -= VY;
                    registers[0xF] = (VX >= VY); // for whatever reason, it's backwards
                    break;
                case 0x7: // 8XY7 set VX to VY - VX
                    registers[X] = VY - VX;
                    registers[0xF] = (VY >= VX); // for whatever reason, it's backwards
                    break;
                case 0x6: // 8XY6 shift right
                    if (COSMAC_MODE) {
                        VX = VY;
                    }
                    registers[0xF] = VX & 1; // 1 if the bit shifted out was a 1
                    registers[X] = VX >> 1;
                    break;
                case 0xE: // 8XYE shift left
                    if (COSMAC_MODE) {
                        VX = VY;
                    }
                    registers[X] = VX << 1;
                    registers[0xF] = VX >> 7;
                    break;
            }
            break;
        case 0x9: // 9XY0 skips if VX and VY aint equal
            if (registers[X] != registers[Y]) {
                program_counter += 2;
            }
            break;
        case 0xA: // ANNN set the index register
            register_index = NNN;
            break;
        case 0xB: // BNNN jump with offset (or BXNN)
            program_counter = NNN + (COSMAC_MODE ? registers[0x0] : registers[X]);
            break;
        case 0xC: // CXNN set VX to ((random num) & NN)
            registers[X] = (uint8_t)rand() & NN;
            break;
        case 0xD: // DXYN draw sprite found in memory at the current register index at positon VX, VY
            // i genuinely can't remember why i named them xc and yc, what does the c even stand for??? character?????
            uint8_t xc = registers[X] % 64;
            uint8_t yc = registers[Y] % 32;
            
            registers[0xF] = 0; // set to 1 if collision happens
            for (int row = 0; row < N && (row+yc) < 32; row++) {
                uint64_t sprite = memory[register_index + row];
                sprite = (xc <= 56) ? sprite << (56-xc) : sprite >> (xc-56); //rightshift instead if it clips off
                if (display[yc+row] & sprite) {
                    registers[0xF] = 1; // collision happened
                }
                display[yc+row] ^= sprite;
            }

            SHOULD_RENDER = 1;
            break;
        case 0xE:
            uint16_t mask = 1 << (VX & 0xF);
            uint16_t key_down = keyboard & mask;
            switch (NN) {
                case 0x9E: // EX9E skip forward if key down
                    if (key_down) {
                        program_counter += 2;
                    }
                    break;
                case 0xA1: // EXA1 skip forward if key not down
                    if (!key_down) {
                        program_counter += 2;
                    }
                    break;
            }
            break;
        case 0xF: // timers
            switch (NN) {
                case 0x07: // FX07 set VX to value of delay timer
                    registers[X] = delay_timer;
                    break;
                case 0x15: // FX15 set delay_timer to value of VX
                    delay_timer = registers[X];
                    break;
                case 0x18: // FX18 set sound_timer to value of VX
                    sound_timer = registers[X];
                    break;
                case 0x1E: // FX1E add VX to the index register thingamabob
                    register_index += registers[X];
                    if (AMIGA_MODE) {
                        registers[0xF] = register_index > 0x1000;
                    }
                    break;
                case 0x0A: // FX0A block until a key pressed, set VX to the keycode
                    if (keyboard) {
                        registers[X] = __builtin_ctz(keyboard);
                    } else {
                        program_counter -= 2;
                    }
                    break;
                case 0x29: // FX29 set index reg to whereever the index of a character sprite thingy specified by VX is kept in memory
                    register_index = 0x50 + registers[X] * 5;
                    break;
                case 0x33: // FX33 convert VX to 3 decimal digits, store in 3 parts starting at memory[indexreg]
                    memory[register_index] = VX/100;
                    memory[register_index+1] = (VX/10)%10;
                    memory[register_index+2] = VX%10;
                    break;
                case 0x55: // FX55 store the values in the registry from V0 to VX in memory starting at I 
                    for (int i = 0; i <= X; i++) {
                        memory[register_index+i] = registers[i];
                    }
                    if (COSMAC_MODE) {
                        register_index += X+1;
                    }
                    break;
                case 0x65: // FX65 take values from memory from I to I+X and store them in the registers, starting at 0
                    for (int i = 0; i <= X; i++) {
                        registers[i] = memory[register_index+i];
                    }
                    if (COSMAC_MODE) {
                        register_index += X+1;
                    }
                    break;
            }
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
    register_index = 0;
    delay_timer,sound_timer = 0,0;
    stack_position = 0;
    memcpy(memory+0x50,fontset,sizeof(fontset));
    memset(stack,0,sizeof(stack));
    memset(registers,0,sizeof(registers));
    fread(memory+0x200,1,4096-0x200,file_pointer);
}

void loop(void) {
    clock_t last_instruction = clock();
    clock_t last_frame = last_instruction;
    while (1) {
        clock_t current_time = clock();
        if ((current_time-last_frame) > FRAME_SLEEP_TIME) {
            last_frame += CPU_SLEEP_TIME;
            if (delay_timer) delay_timer--;
            if (sound_timer) sound_timer--;
            if (SHOULD_RENDER) render_display();
        }
        if ((current_time-last_instruction) > CPU_SLEEP_TIME) {
            last_instruction += CPU_SLEEP_TIME;
            step_execution_loop();
        }
    }
}