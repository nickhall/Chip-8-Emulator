#include <stdlib.h>
#include <time.h>
#include "chip8.h"

BYTE chip8_fontset[80] =
{
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

chip8::chip8()
{
	std::cout << "Emulator created" << std::endl;
	chip8::init();
}

void chip8::init()
{
	pc = 0x200;
	sp = 0;
	index = 0;
	// TODO: Clear screen, stack, registers, memory, etc.
	clearScreen();
	srand(time(NULL)); // Seed random number for later
	std::cout << "Emulator initialized" << std::endl;
	load();

	// Load fontset
	for (int i = 0; i < 80; ++i)
		memory[i] = chip8_fontset[i];

	//The window we'll be rendering to
	Window = NULL;

	//The surface contained by the window
	ScreenSurface = NULL;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}
	else
	{
		//Create window
		Window = SDL_CreateWindow("There's my chippy", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 320, SDL_WINDOW_SHOWN);
		if (Window == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		}
		else
		{
			//Get window surface
			ScreenSurface = SDL_GetWindowSurface(Window);
		}
	}

	renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == nullptr)
	{
		SDL_DestroyWindow(Window);
		std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
	}
}

void chip8::run()
{
	running = true;
	while (running)
	{
		update(0);
	}
	SDL_Quit();
}

void chip8::update(float dt)
{
	// Update
	// Fetch
	opcode = memory[pc] << 8 | memory[pc + 1]; // Need 2 bytes. Bit magic to fit both into the 16-bit unsigned short
	printf("%04X\n", opcode);
	// Increment pc first so we don't mess up jump instructions
	pc += 2;
	// Decode/execute
	decode(opcode);

	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_QUIT)
		{
			running = false;
		}
	}
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Clear to black
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Draw white
	SDL_Rect rect;
	rect.x = 50;
	rect.y = 50;
	rect.h = 50;
	rect.w = 50;
	SDL_Rect rect2;
	rect2.x = 150;
	rect2.y = 150;
	rect2.h = 50;
	rect2.w = 50;

	SDL_Rect rects[2];
	rects[0] = rect;
	rects[1] = rect2;
	SDL_RenderFillRects(renderer, rects, 2);
	SDL_RenderPresent(renderer);
}

void chip8::decode(OPCODE input)
{
	// TODO: Clean this up and consolidate pointer usage
	bool throwError = false; // Use this as a flag to determine if an invalid opcode was entered
	BYTE vx = v[input & 0x0F00 >> 8]; // Caching for operations of form #xkk
	BYTE *vxptr = &v[input & 0x0F00 >> 8];
	BYTE vy = v[input & 0x00F0 >> 4]; // Caching for operations of form #xy0
	BYTE *vyptr = &v[input & 0x00F0 >> 4];
	BYTE kk = input & 0x00FF; // Caching for operations of form #xkk
	switch (input >> 12 & 0xF) // Determine first nibble
	{
	case 0x0:
		switch (input & 0x0FFF)
		{
		case 0x00E0: // 00E0 - CLS - Clear display
			cout << "Clear screen" << endl;
			break;
		case 0x00EE: // 00EE - RET - Return from a subroutine.
			cout << "Return from subroutine";
			sp--;
			pc = stack[sp];
			break;
		default:
			throwError = true;
		}
		break;
	 case 0x1: // 1nnn - JP addr - Jump (= set pc) to location nnn.
		 pc = input & 0x0FFF;
		 break;
	case 0x2: // 2nnn - CALL addr - Call subroutine at nnn.
		stack[sp] = pc; // To call a new subroutine, we have to remember where we are before moving to a new stack frame
		sp++; // And then increase our stack pointer
		pc = input & 0x0FFF; // And reset the program counter to the location of the subroutine
		break;
	case 0x3: // 3xkk - SE Vx, byte - Skip next instruction if Vx = kk.
		if (vx == kk)
		{
			pc += 2; // Skip next instruction
		}
		break;
	case 0x4: // 4xkk - SNE Vx, byte - Skip next instruction if Vx != kk.
		if (vx != kk)
		{
			pc += 2; // Skip next instruction
		}
		break;
	case 0x5: // 5xy0 - SE Vx, Vy - Skip next instruction if Vx = Vy.
		if (vx == vy)
		{
			pc += 2; // Skip next instruction
		}
		break;
	case 0x6: // 6xkk - LD Vx, byte - Set Vx = kk.
		v[input & 0x0F00 >> 8] = kk; // Don't use variable since we're setting. TODO: See if I can use a pointer here
		break;
	case 0x7: // 7xkk - ADD Vx, byte - Set Vx = Vx + kk.
		v[input & 0x0F00 >> 8] += kk; // TODO: see above
		break;
	case 0x8: // A bunch of intra-register operations. 8xy#, so determine operation by checking last bit
		switch (input & 0x000F)
		{
		case 0x0: // 8xy0 - LD Vx, Vy - Set Vx = Vy.
			*vxptr = *vyptr;
			break;
		case 0x1: // 8xy1 - OR Vx, Vy - Set Vx = Vx OR Vy.
			*vxptr = *vxptr | *vyptr;
			break;
		case 0x2: // 8xy2 - AND Vx, Vy - Set Vx = Vx AND Vy.
			*vxptr = *vxptr & *vyptr;
			break;
		case 0x3: // 8xy3 - XOR Vx, Vy - Set Vx = Vx XOR Vy.
			*vxptr = *vxptr ^ *vyptr;
			break;
		case 0x4: // 8xy4 - ADD Vx, Vy - Set Vx = Vx + Vy, set VF = carry.
			*vxptr = *vxptr + *vyptr;
			v[0xF] = (int)*vxptr + (int)*vyptr > 255 ? 1 : 0; // Lazily using integers to check overflow
			break;
		case 0x5: // 8xy5 - SUB Vx, Vy - Set Vx = Vx - Vy, set VF = NOT borrow.
			v[0xF] = *vxptr > *vyptr ? 1 : 0;
			break;
		case 0x6: // 8xy6 - SHR Vx {, Vy} - Set Vx = Vx SHR 1.
			v[0xF] = (input & 0x000F) == 1 ? 1 : 0; // Shifting right, so detect whether we're bumping a bit off
			*vxptr = *vxptr >> 1;
			break;
		case 0x7: // 8xy7 - SUBN Vx, Vy - Set Vx = Vy - Vx, set VF = NOT borrow.
			v[0xF] = *vxptr < *vyptr ? 1 : 0; // Reverse of the other SUB
			*vxptr = *vyptr - *vxptr;
			break;
		case 0xE: // 8xyE - SHL Vx {, Vy} - Set Vx = Vx SHL 1.
			v[0xF] = (*vxptr & 0xF000) == 0x1000 ? 1 : 0; // Shift left, so determine if we're popping off a bit
			*vxptr = *vxptr << 1;
			break;
		default:
			// Some other 8xxx instruction
			throwError = true;
		}
	case 0x9: // SNE Vx, Vy - Skip next instruction if Vx != Vy.
		if (vx != vy) pc += 2;
		break;
	case 0xA: // Annn - LD I, addr - Set I = nnn.
		index = input & 0x0FFF;
		break;
	case 0xB: // Bnnn - JP V0, addr - Jump to location nnn + V0.
		pc = v[0] + input & 0x0FFF;
		break;
	case 0xC: // Cxkk - RND Vx, byte - Set Vx = random byte AND kk.
		*vxptr = kk & (rand() % 255);
		break;
	case 0xD: // Dxyn - DRW Vx, Vy, nibble - Display n - byte sprite starting at memory location I at(Vx, Vy), set VF = collision.
		// TODO: Implement this
		cout << "SPRITE OUTPUT: PLEASE IMPLEMENT" << endl;
		break;
	case 0xE:
		if (kk == 0x9E) // Ex9E - SKP Vx - Skip next instruction if key with the value of Vx is pressed.
		{
			// TODO: Implement this plus key checking
			cout << "WAITING FOR KEYBOARD" << endl;
		}
		if (kk == 0xA1) // ExA1 - SKNP Vx - Skip next instruction if key with the value of Vx is not pressed.
		{
			// TODO: Implement this plus key checking
			cout << "WAITING FOR KEYBOARD" << endl;
			pc += 2; // Skip anyway for now
		}
		break;
	case 0xF: // 0xF instructions all follow 0xFx##, so we can use our kk shortcut
		switch (kk)
		{
		case 0x07: // Fx07 - LD Vx, DT - Set Vx = delay timer value.
			*vxptr = delay;
			break;
		case 0x0A: // Fx0A - LD Vx, K - Wait for a key press, store the value of the key in Vx.
			// TODO: Implement
			cout << "Input: ";
			int number;
			cin >> number;
			cout << endl;
			break;
		case 0x15: // Fx15 - LD DT, Vx - Set delay timer = Vx.
			delay = *vxptr;
			break;
		case 0x18: // Fx18 - LD ST, Vx - Set sound timer = Vx.
			sound = *vxptr;
			break;
		case 0x1E: // Fx1E - ADD I, Vx - Set I = I + Vx.
			index += *vxptr;
			break;
		case 0x29: // Fx29 - LD F, Vx - Set I = location of sprite for digit Vx.
			// TODO: Implement
			cout << "Sprite location" << endl;
			break;
		case 0x33: // Fx33 - LD B, Vx - Store BCD representation of Vx in memory locations I, I + 1, and I + 2.
			memory[index] = *vxptr / 100; // Hundreds
			memory[index + 1] = *vxptr % 100 / 10; // Tens
			memory[index + 2] = *vxptr % 10; // Ones
			break;
		case 0x55: // Fx55 - LD [I], Vx - Store registers V0 through Vx in memory starting at location I.
			for (int i = 0; i < 16; i++)
			{
				memory[index + i] = v[i];
			}
			break;
		case 0x65: // Fx65 - LD Vx, [I] - Read registers V0 through Vx from memory starting at location I.
			for (int i = 0; i < 16; i++)
			{
				v[i] = memory[index + i];
			}
			break;
		}
	}
}

void chip8::load()
{
	//TODO: Remove hard coded location and update loading code with ifstream
	FILE *file = fopen("C:\\Users\\Nick\\Documents\\GitHub\\Chip-8-Emulator\\Chippy\\Debug\\PONG", "rb");
	if (file != NULL)
	{
		cout << "File opened" << endl;
		
		// Determine file size
		fseek(file, 0L, SEEK_END);
		int fsize = ftell(file);
		fseek(file, 0L, SEEK_SET);

		// Allocate memory buffer. There's probably a  better way to do this but for now let's just get it working
		BYTE *buffer = (BYTE*) malloc(fsize);
		// Read file contents into buffer and close file
		fread(buffer, fsize, 1, file);
		fclose(file);

		// Loop over each byte and add it into the memory array at the correct offset
		for (int i = 0; i < fsize; ++i)
		{
			memory[0x200 + i] = buffer[i];
			// Print for debug purposes
			printf("%02X", memory[0x200 + i]);
		}
		cout << endl;
	}
	else
	{
		cout << "Couldn't find pong";
	}
}

void chip8::clearScreen()
{
	for (int i = 0; i < (32 * 64); i++)
	{
		memory[i] = 0;
	}
}

chip8::~chip8()
{
}
