#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h> // while true
#include <assert.h>

#include <time.h>

#include "SDL.h"

// window
SDL_Window * window;
SDL_Renderer * renderer;
SDL_Texture * screen;


// emulator states
typedef enum {
        QUIT, // 0
        RUNNING, // 1
        PAUSED, // 2
} state_t;

typedef struct {
        state_t state; // contains quit/running/paused
	// define other chip8 variables (resolution, registers, etc)
	uint8_t display [64 * 32]; // 64x32 display
	uint8_t memory[4096]; // 4KB memory
	uint16_t PC; // program counter :>
	uint16_t I; // instruction pointer
	uint8_t v[16]; // REGISTERS
	uint16_t stack[16]; // stackk
	uint16_t sp; // stack pointer
	uint8_t keyboard[16]; // keyboard input
	uint8_t delay_timer; // used in some opcodes
	uint8_t speed; // speed of console
	// skipping sound lol
} chip8_t;

void init_sdl(void) { // initializes SDL
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
                SDL_Log("SDL error: %s", SDL_GetError());
                exit(1);
        }
        return;
}

void create_window() {
        window = SDL_CreateWindow(
                "CHIP-8",
                SDL_WINDOWPOS_CENTERED, // x
                SDL_WINDOWPOS_CENTERED, // y
                1024,
                512,
                0
        );

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        screen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,64,32);
        //SDL_SetRenderDrawColor(renderer, 255,191,0,255); // amber
	SDL_SetRenderDrawColor(renderer, 0, 128, 0,255); // green
	SDL_RenderClear(renderer);
}

void cleanup(void) { // ends program
        SDL_DestroyTexture(screen);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
}

void draw(chip8_t * chip8) { // update screen
	//(void)chip8; // delete later
	//SDL_RenderSetLogicalSize(renderer, 64, 32); // the key to success??
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // black
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // green
	
	// probably shouldn't be hardcoding these but whatever
	for (int x = 0; x < 64; x++) {
		for (int y = 0; y < 32; y++) {
			if (chip8->display[y * 64 + x]) {
				// draw 16x16 rectangle at [x*16, y*16]
				// x16 because that's my scaling factor on the resolution
				SDL_Rect pixel = { x * 16, y * 16, 16, 16 };
				SDL_RenderFillRect(renderer, &pixel); // fill...
			}
			//pixel[y * 64 + x] = // something
			// scale factor of 16

		}
	}
	SDL_RenderPresent(renderer); // draw to screen
}

void exec(chip8_t * chip8) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
                switch (event.type) {
                        case SDL_QUIT: // exit
				(*chip8).state = QUIT;
				break;
                                //return;
                	case SDL_KEYDOWN: // keyboard input brrrr
				switch (event.key.keysym.sym) {
					case SDLK_x:
						(*chip8).keyboard[0] = 1;
						break;
					case SDLK_1:
						(*chip8).keyboard[1] = 1;
						break;
					case SDLK_2:
                                                (*chip8).keyboard[2] = 1;   
					        break;
					case SDLK_3:
                                                (*chip8).keyboard[3] = 1;   
					        break;
					case SDLK_q:
                                                (*chip8).keyboard[4] = 1;   
					        break;
					case SDLK_w:
                                                (*chip8).keyboard[5] = 1;   
					        break;
					case SDLK_e:
                                                (*chip8).keyboard[6] = 1;   
						break;
					case SDLK_a:
                                                (*chip8).keyboard[7] = 1;   
					        break;
					case SDLK_s:
                                                (*chip8).keyboard[8] = 1;   
					        break;
					case SDLK_d:
                                                (*chip8).keyboard[9] = 1;   
					        break;
					case SDLK_z:
                                                (*chip8).keyboard[10] = 1;   
					        break;
					case SDLK_c:
                                                (*chip8).keyboard[11] = 1;   
					        break;
					case SDLK_4:
                                                (*chip8).keyboard[12] = 1;   
					        break;
					case SDLK_r:
                                                (*chip8).keyboard[13] = 1;   
					        break;
					case SDLK_f:
                                                (*chip8).keyboard[14] = 1;   
					        break;
					case SDLK_v:
                                                (*chip8).keyboard[15] = 1;
					        break;
					case SDLK_PERIOD:
						if ((*chip8).speed < 64) {
							(*chip8).speed++;
						}
						break;
					case SDLK_COMMA:
						if ((*chip8).speed > 0) {
							(*chip8).speed--;
					}
						break;
				}break; // end keysym switch
			case SDL_KEYUP:
				switch(event.key.keysym.sym) {
                                        case SDLK_1:
                                                (*chip8).keyboard[1] = 0;
                                                break;
                                        case SDLK_2:
                                                (*chip8).keyboard[2] = 0;   
                                                break;
                                        case SDLK_3:
                                                (*chip8).keyboard[3] = 0;   
                                                break;
                                        case SDLK_q:
                                                (*chip8).keyboard[4] = 0;   
                                                break;
                                        case SDLK_w:
                                                (*chip8).keyboard[5] = 0;   
                                                break;
                                        case SDLK_e:
                                                (*chip8).keyboard[6] = 0;   
                                                break;
                                        case SDLK_a:
                                                (*chip8).keyboard[7] = 0;   
                                                break;
                                        case SDLK_s:
                                                (*chip8).keyboard[8] = 0;   
                                                break;
                                        case SDLK_d:
                                                (*chip8).keyboard[9] = 0;   
                                                break;
                                        case SDLK_z:
                                                (*chip8).keyboard[10] = 0;   
                                                break;
                                        case SDLK_c:
                                                (*chip8).keyboard[11] = 0;   
                                                break;
                                        case SDLK_4:
                                                (*chip8).keyboard[12] = 0;   
                                                break;
                                        case SDLK_r:
                                                (*chip8).keyboard[13] = 0;   
                                                break;
                                        case SDLK_f:
                                                (*chip8).keyboard[14] = 0;   
                                                break;
                                        case SDLK_v:
                                                (*chip8).keyboard[15] = 0;
						break;
				}break; // end keysym switch
		} // end event switch
        } // end while loop
}// end exec

void init_chip8(chip8_t* chip8) {
	chip8->PC = 0x200; // all programs start at 200
	chip8->I = 0; // instruction pointer
	chip8->sp = 0; // stack
	chip8->delay_timer = 0;
	chip8->speed = 16;
	// add more stuff
	memset(chip8->memory, 0, sizeof(chip8->memory));
	memset(chip8->v, 0, sizeof(chip8->v));
	memset(chip8->stack, 0, sizeof(chip8->stack));
	memset(chip8->display, 0, sizeof(chip8->display));
	memset(chip8->keyboard, 0, sizeof(chip8->keyboard));


	// fontset should be loaded starting @ 0x50
	static const uint8_t fontset[80] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};
	for (int i = 0; i < 80; ++i) {
	chip8->memory[0x50 + i] = fontset[i];
	}
}

void cycle(chip8_t * chip8) { // WHERE COOL THINGS HAPPEN
	//fetch, decode, execute cycle
	//uint8_t X, Y, kk, n;
	//uint16_t nnn;


	//(void)chip8; // delete later
	uint16_t opcode = chip8->memory[chip8->PC] << 8 | chip8->memory[chip8->PC + 1];
	chip8->PC += 2;	
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t y = (opcode & 0x00F0) >> 4;
	uint8_t n = (opcode & 0x000F);
	uint8_t nn = (opcode & 0x00FF);
	uint16_t nnn = (opcode & 0xFFF);
	printf("Opcode: 0x%04X\n", opcode);
	printf("Program counter: 0x%04x\n", chip8->PC);
	printf("Instruction: %04x\n", chip8->I);
	printf("Speed: %d\n", 64 - (chip8->speed));
	switch (opcode & 0xf000) {
		case 0x0000:
			// DONE
			switch(opcode & 0x00ff) {	
			case 0x00E0: // clear screen
				memset(chip8->display, 0, sizeof(chip8->display));
				break;
			case 0x00EE:
				// return from subroutine - backwards order(???)
				chip8->sp--;
				chip8->PC = chip8->stack[chip8->sp];
						
				break;
			default:
			printf("Unknown opcode: 0x%04X\n", opcode);
			break;
		}
			break;	
		// DONE
		case 0x1000: // set pc to NNN
			chip8->PC = nnn;
			break;
		// DONE
		case 0x2000: // jump to subroutine
			//chip8->sp++;
			chip8->stack[chip8->sp] = chip8->PC;
			chip8->sp++;
			chip8->PC = nnn;
			break;
		// DONE
		case 0x3000: // skip instruction if vx==nn
			if (chip8->v[x] == nn) {
				chip8->PC += 2;
			}
			break;
		//DONE
		case 0x4000: // skip instruction if vx!=nn
			if (chip8->v[x] != nn) {
                                chip8->PC += 2;
                        }
			break;
		//DONE
		case 0x5000: // skip instruction if vx!=vy
			if (chip8->v[x] == chip8->v[y])
				chip8->PC += 2;
			break;
		//DONE
		case 0x6000: // set v[x] to nn
			chip8->v[x] = nn;
			break;
		//DONE
		case 0x7000: // add nn to v[x]
			chip8->v[x] += nn;
			break;
		//DONE
		case 0x8000:
			// nested switch
			switch(opcode & 0x00F) {
				//DONE
				case 0: // set v[x] = v[y]
					chip8->v[x] = chip8->v[y];
					break;
				//DONE
				case 1: // logical OR
					chip8->v[x] |= chip8->v[y];
					break;
				//DONE
				case 2: // logical AND
					chip8->v[x] &= chip8->v[y];
					break;
				//DONE
				case 3: // logical XOR
					chip8->v[x] ^= chip8->v[y];
					break;
				//DONE
				case 4: //add v[x]+v[y], and V[f]=1 if overflow
					chip8->v[0xf] = 0;
					if ((int)chip8->v[x] + (int)chip8->v[y] > 255) { // that's right, we're cheating :>
						chip8->v[0xf] = 1;
					}
					chip8->v[x] += chip8->v[y];
					break;
				//DONE
				case 5: // sub v[x]-v[y], VF=1 if VX>= VY
					chip8->v[0xf] = 0;
                                        if (chip8->v[x] >= chip8->v[y]) {
                                                chip8->v[0xf] = 1;
                                        }
					chip8->v[x] -= chip8->v[y];
					break;
				//DONE
				case 6:
					chip8->v[0xf] = chip8->v[x] >> 7; // WRONG maybe
					chip8->v[x] >>= 1;
					break;
				//DONE
				case 7:
					chip8->v[0xf] = 0;
					if (chip8->v[y] >= chip8->v[x]) {
						chip8->v[0xf] = 1;
					}

					chip8->v[x] = chip8->v[y] - chip8->v[x];
					break;

				//DONE
				case 0xe:
				// if E, shift VX left by 1 and set VF to 1 if (MSB of VX)=1
					int8_t sign_flag;
					if ((chip8->v[x] >> 7) == 1) // WRONG maybe
						sign_flag = 1;
					chip8->v[x] <<= 1;

					if (sign_flag) {
						chip8->v[0xf] = 1;
					} else {
						chip8->v[0xf] = 0;
					}
					break;
				default:
					printf("Unknown opcode: 0x%04X\n", opcode);
			}
			break;
			//if (chip8->v[x] == chip->x[y])
		//DONE
		case 0x9000: // skip instruction if v[x] != v[y]
			if (chip8->v[x] != chip8->v[y])
				chip8->PC += 2;
			break;
		//DONE
		case 0xA000: // set I to nnn
			chip8->I = nnn;
			break;
		//DONE
		case 0xB000: // jump to nnn plus v0
			chip8->PC = nnn + chip8->v[0];
			break;
		

		// DONE LOL
		case 0xC000:
			// no idea how to efficiently get a random number
			chip8->v[x] = ((rand() % 255) & nn);
			// so we cheating 8)
			break;

		// DONE BUT WOW THIS ONE SUCKS
		case 0xD000:
			uint8_t width = 8;
			uint16_t height = n;
			uint16_t vx = chip8->v[x] % 64;
			uint16_t vy = chip8->v[y] % 32;
			uint8_t pixel;

			chip8->v[0xf] = 0;

			// big mess incoming
			for (int i = 0; i < height; i++) {
				pixel = chip8->memory[chip8->I+i];
				for (int j = 0; j < width; j++) {
					if ((pixel & (0x80>>j)) != 0) {
						if (chip8->display[(vx + j + ((vy + i) * 64))]) {
							chip8->v[0xf] = 1;
						}
						chip8->display[(vx + j) + ((vy + i) * 64)] ^= 1;
					}
				}
			}

			break;

		
		case 0xE000:
			// nested switch incoming
			// DONE
			switch(opcode & 0xff) {
				// DONE
				case 0x9e:
					// skip instruction if keyboard[v[x]] is held down
					if (chip8->keyboard[chip8->v[x]] == 1)
						chip8->PC += 2;
					break;
				// DONE
				case 0xa1:
					// skip instruction if keyboard[v[x]] isn't held
					if (chip8->keyboard[chip8->v[x]] == 0)
						chip8->PC +=2;
					break;
				default:
					printf("Unknown opcode: 0x%04X\n", opcode);
					break;
			}
			break;
		case 0xF000:
			// nested switch
			switch(opcode & 0x00FF) {
				// delay timer
				// DONE
				case 7:
					// set vx to delay timer
					chip8->v[x] = chip8->delay_timer;
					break;

				case 0xa:
					// halt until key press (hard?)
					uint8_t loop = 1;
					for (uint8_t i = 0; i < 16; i++) {
						if (chip8->keyboard[i] == 1) {
							chip8->v[x] = i;
							loop = 0;
							break; // maybe better
						}
					}
					if (loop == 1)
						chip8->PC -= 2; // this should loop lol
					break;

				case 0x15:
					// set delay timer to v[x]
					chip8->delay_timer = chip8->v[x];
					break;

				case 0x18:
					// sound timer so we can ignore it lol
					break;

				case 0x1E: // I += Vx
					chip8->I += chip8->v[x];
					break;

				case 0x29:
					chip8->I = 0x50 + (chip8->v[x] * 5);
					break;

				case 0x33:
					// store hundreds, tens, and ones digit of v[x]
					uint8_t hundreds;
					uint8_t tens;
					uint8_t ones;

					// store hundreds digit into memory[I]
					hundreds = (chip8->v[x] - (chip8->v[x] % 100));
					if (hundreds != 0) {
						hundreds = hundreds / 100;
					}
					chip8->memory[chip8->I] = hundreds;

					// store tens digit into memory[I+1]
					tens = (chip8->v[x] - (chip8->v[x] % 10)) % 100;
					if (tens != 0) {
						tens = tens / 10;
					}
					chip8->memory[(chip8->I)+1] = tens;

					// store ones digit into memory[I+2]
					ones = chip8->v[x] % 10;
					chip8->memory[(chip8->I)+2] = ones;

					break;

				case 0x55: // store v0-v[x] into I, with I unaffected
					uint8_t end = (opcode & (0xF00)) >> 8;
					for (uint8_t i = 0; i <= end; i++) {
						chip8->memory[(chip8->I)+i] = chip8->v[i];
					}
					break;

				case 0x65:
					uint8_t ending = (opcode & (0xF00)) >> 8;
                                        for (uint8_t i = 0; i <= ending; i++) {
						chip8->v[i]=(*chip8).memory[((*chip8).I)+i];
                                               // chip8->memory[(chip8->I)+i] = chip8->v[i];
                                        }
					break;
				default: // unknown upcode
					printf("Unknown opcode 0x%04X\n", opcode);
					break;
			}

			break;


		default:
			printf("Unknown opcode 0x%04X\n", opcode);
			//chip8->PC += 2; // advance program counter
			break;
	}// end switch
}//end cycle

int load_rom(char * filename, chip8_t * chip8) {
	FILE *rom = fopen(filename, "rb");
 	assert(rom != 0);

	size_t bytes_read = fread(&chip8->memory[0x200], 1, sizeof(chip8->memory)-0x200, rom);
    fclose(rom);

    	assert(bytes_read != 0);

	return 1; // success!
}

int main(int argc, char ** argv) {
	srand(time(NULL));
        //(void)argc;
        //(void)argv; // just so compiler shuts up lol

	if (argc != 2) {
		printf("./chip8 <rom_path>\n");
	}
	assert(argc == 2);

        // init SDL
        init_sdl();
        create_window();


	//chip8.speed = 16; // speed of console
	
        // init chip8
        chip8_t chip8 = {.state = RUNNING};
        init_chip8(&chip8);

	int retCode = load_rom(argv[1], &chip8);
        assert(retCode == 1);

	while (chip8.state != QUIT) {
                exec(&chip8); // user input
		cycle(&chip8); // fetch-decode-execute
		draw(&chip8); // update display


		/* experimental */
		//if (chip8.delay_timer > 30)
		//	chip8.delay_timer -= 30;
		/* experimental */


		if (chip8.delay_timer > 0)
			chip8.delay_timer--;
                SDL_Delay(chip8.speed); // 60hz ish if left at 16
        }


        // cleanup
        cleanup();
        puts("finished");
}


