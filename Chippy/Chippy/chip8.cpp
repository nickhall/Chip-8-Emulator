//#include <iostream>
//#include <fstream>
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
	std::cout << "Emulator initialized" << std::endl;
	load();

	// Load fontset
	for (int i = 0; i < 80; ++i)
		memory[i] = chip8_fontset[i];

	// Debugging: update once
	update(0);
}

void chip8::run()
{
	cout << "Running..." << endl;
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
}

void chip8::decode(OPCODE input)
{
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
			v[0xF] = input & 0x000F == 1 ? 1 : 0; // Shifting right, so detect whether we're bumping a bit off
			*vxptr = *vxptr >> 1;
			break;
		case 0x7: // 8xy7 - SUBN Vx, Vy - Set Vx = Vy - Vx, set VF = NOT borrow.
			v[0xF] = *vxptr < *vyptr ? 1 : 0; // Reverse of the other SUB
			*vxptr = *vyptr - *vxptr;
			break;
		case 0xE: // 8xyE - SHL Vx {, Vy} - Set Vx = Vx SHL 1.
			v[0xF] = *vxptr & 0xF000 == 0x1000 ? 1 : 0; // Shift left, so determine if we're popping off a bit
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

chip8::~chip8()
{
}
