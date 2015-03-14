//#include <iostream>
//#include <fstream>
#include "chip8.h"

chip8::chip8()
{
	std::cout << "Emulator created" << std::endl;
	chip8::init();
}

void chip8::init()
{
	pc = 0x200;
	std::cout << "Emulator initialized" << std::endl;
	load();
}

void chip8::run()
{
	cout << "Running..." << endl;
}

void chip8::load()
{
	//TODO: Remove hard coded location
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
