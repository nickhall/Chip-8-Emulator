#pragma once
#include <iostream>
#include <fstream>
#include <SDL.h>
using namespace std;
// TODO: Make sure I'm using header files correctly

typedef unsigned char BYTE;
typedef unsigned short OPCODE;

class chip8
{
public:
	chip8();
	~chip8();
	void init();
	void run();
	void update(float dt);
	void pause();
	void start();
	void load();

private:
	//istream file;					// Input file
	unsigned char memory[4096];		// Main memory
	int pc;							// Program counter, shouldn't be int but whatever
	int sp;							// Stack pointer (see above)
	int index;						// Index register
	unsigned short stack[16];		// Stack
	BYTE v[16];						// Registers
	BYTE screen[32 * 64];			// Video buffer
	OPCODE opcode;					// Opcode
	BYTE key[16];					// Keyboard
	BYTE delay;						// Delay timer
	BYTE sound;						// Sound timer
	SDL_Renderer* renderer;
	SDL_Window* Window;
	SDL_Surface* ScreenSurface;
	bool running;

	void decode(OPCODE input);
	void clearScreen();
	
	BYTE chip8_fontset[80];
};