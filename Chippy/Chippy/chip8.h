#pragma once
class chip8
{
public:
	unsigned char memory[4096];		// Main memory
	int pc;							// Program counter
	int sp;							// Stack pointer
	unsigned short stack[16];		// Stack
	unsigned char v[16];			// Registers
	unsigned char screen[32 * 64];	// Video buffer
	unsigned short opcode;			// Opcode
	unsigned char key[16];			// Keyboard

	chip8();
	void init();
	~chip8();
};

