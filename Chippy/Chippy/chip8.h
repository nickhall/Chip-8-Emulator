#pragma once
#include <iostream>
#include <fstream>
using namespace std;
// TODO: Make sure I'm using header files correctly

typedef unsigned char BYTE;

class chip8
{
public:
	chip8();
	~chip8();
	void init();
	void run();
	void pause();
	void start();
	void load();

private:
	//istream file;					// Input file
	unsigned char memory[4096];		// Main memory
	int pc;							// Program counter
	int sp;							// Stack pointer
	unsigned short stack[16];		// Stack
	BYTE v[16];						// Registers
	BYTE screen[32 * 64];			// Video buffer
	unsigned short opcode;			// Opcode
	BYTE key[16];					// Keyboard
	//TODO: Sound and timers
};