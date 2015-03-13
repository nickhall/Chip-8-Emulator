#include "chip8.h"
#include <iostream>

chip8::chip8()
{
	std::cout << "Emulator created" << std::endl;
	chip8::init();
}

void chip8::init()
{
	std::cout << "Emulator initialized" << std::endl;
}

chip8::~chip8()
{
}
