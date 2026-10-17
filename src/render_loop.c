#define SCREEN_SCALE 20

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <time.h>
#include "chip8.h"

clock_t CPU_SLEEP_TIME = CLOCKS_PER_SEC / 700;
clock_t FRAME_SLEEP_TIME = CLOCKS_PER_SEC / 60;

SDL_Scancode keyboard_layout[16] = {
    SDL_SCANCODE_X, SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3,
    SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_A,
    SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_Z, SDL_SCANCODE_C,
    SDL_SCANCODE_4, SDL_SCANCODE_R, SDL_SCANCODE_F, SDL_SCANCODE_V,
};

SDL_Window *win;
SDL_Renderer *renderer;
SDL_Texture *texture;

int get_key_num(SDL_Scancode scancode) {
    for (int i = 0; i < 16; i++) {
        if (keyboard_layout[i] == scancode) return i;
    }
    return -1;
}

void render_display(void) {
    SHOULD_RENDER = 0;

    // okay so now i realized if i wanna use sdl my cool bitshifting thing to set pixels bites me in the ass here :skull:
    // instead of undoing it we're going to make an array of pixels every time we need to render yaaayy

    uint32_t pixels[64*32];
    for (int y = 0; y < 32; y++) {
        uint64_t row = display[y];
        for (int x = 0; x < 64; x++) {
            pixels[y*64+x] = (((row >> (63-x))&1)) ? 0xFFFFFFFF : 0xFF000000;
        }
    }

    SDL_UpdateTexture(texture,NULL,pixels,64*sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer,texture,NULL,NULL);
    SDL_RenderPresent(renderer);
}

int make_display(void) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) return 1;
    win = SDL_CreateWindow("Chip8",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,SCREEN_WIDTH*SCREEN_SCALE,SCREEN_HEIGHT*SCREEN_SCALE,0);
    if (!win) {
        SDL_Quit();
        return 1;
    }

    renderer = SDL_CreateRenderer(win,-1,SDL_RENDERER_ACCELERATED);
    SDL_RenderSetLogicalSize(renderer,64,32);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"0");

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_STREAMING,64,32);

    return 0;
}

void cleanup_display(void) {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();
}

int main_loop(void) {
    make_display();

    clock_t last_instruction = clock();
    clock_t last_frame = last_instruction;

    int running = 1;
    SDL_Event event;
    while (running) {
        clock_t current_time = clock();

        if ((current_time-last_frame) > FRAME_SLEEP_TIME) {
            last_frame += FRAME_SLEEP_TIME;
            if (sound_timer) sound_timer--;
            if (delay_timer) delay_timer--;

            if (SHOULD_RENDER) render_display();
        }

        if ((current_time-last_instruction) > CPU_SLEEP_TIME) {
            last_instruction += CPU_SLEEP_TIME;
            step_chip();
        }

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = 0;
                    break;
                case SDL_KEYDOWN: {
                    int keynum = get_key_num(event.key.keysym.scancode);
                    if (keynum >= 0) keyboard |= (1 << keynum);
                    break;
                }
                case SDL_KEYUP: {
                    int keynum = get_key_num(event.key.keysym.scancode);
                    if (keynum >= 0) keyboard &= ~(1 << keynum);
                    break;
                }
            }

            if (event.type == SDL_QUIT) {
                running = 0;
                break;
            }
        }
    }

    cleanup_display();

    return 0;
}