#include <iostream>
#include "chip8.h"
using namespace std;

int main()
{
	chip8 emulator;
	emulator.run();

	// Update emulator
	emulator.update(0);

	system("pause");
	return 0;
}